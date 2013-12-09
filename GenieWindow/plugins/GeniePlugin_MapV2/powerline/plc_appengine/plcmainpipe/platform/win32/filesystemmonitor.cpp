#include "filesystemmonitor.h"
#include "helperroutines.h"

#include <QtDebug>
#include <QDir>
#include <QFile>
#include <QLibrary>

FileSystemMonitor::FileSystemMonitor(const QString &monitorDirPath) :
    m_isExiting(false)
  , m_monitorDirPath(monitorDirPath)
  , m_hExitEvent(NULL)
{
    if((m_hExitEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL)) != NULL){
        start();
    }else{
        qDebug () << "FileSystemMonitor -- create exit event failed, error code = " << ::GetLastError();
    }
}

FileSystemMonitor::~FileSystemMonitor()
{
    m_isExiting = true;

    if(m_hExitEvent != NULL){
        ::SetEvent(m_hExitEvent);
    }

    wait();

    if(m_hExitEvent != NULL){
        ::CloseHandle(m_hExitEvent);
    }
}

void FileSystemMonitor::run()
{
    HANDLE hMonitorDir = ::CreateFile(m_monitorDirPath.toStdWString().c_str(), FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);
    if(INVALID_HANDLE_VALUE == hMonitorDir){
        qDebug() << "FileSystemMonitor -- Open directory failed, error code = " <<  ::GetLastError();
        return;
    }

    OVERLAPPED ovl;
    ZeroMemory(&ovl, sizeof(OVERLAPPED));

    if(NULL == (ovl.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL))){
        qDebug() << "FileSystemMonitor -- Create event failed, error code = " << ::GetLastError();
        return;
    }

    HANDLE events[2] = {ovl.hEvent , m_hExitEvent};

    BYTE notifyInfoBuffer[1024];
    DWORD dataReturned = 0;

    while(!m_isExiting){

        ::ResetEvent(ovl.hEvent);
        if(!::ReadDirectoryChangesW(hMonitorDir,notifyInfoBuffer , sizeof(notifyInfoBuffer)
                                    , FALSE, FILE_NOTIFY_CHANGE_FILE_NAME, &dataReturned, &ovl, NULL)){
            QThread::msleep(1000);
            continue;
        }

        if(::WaitForMultipleObjects(sizeof(events) / sizeof(events[0]), events, FALSE, INFINITE)  != WAIT_OBJECT_0){
            if(m_isExiting){
                break;
            }else{
                continue;
            }
        }

        if(!::GetOverlappedResult(hMonitorDir, &ovl, &dataReturned, FALSE) || 0 == dataReturned){
            continue;
        }


        PFILE_NOTIFY_INFORMATION fileNotifyInfo = (PFILE_NOTIFY_INFORMATION)notifyInfoBuffer;

        for( ; !m_isExiting ; ){

            if(FILE_ACTION_ADDED == fileNotifyInfo->Action){
                QString tmpPath = m_monitorDirPath + QDir::separator() + QString::fromWCharArray(fileNotifyInfo->FileName , fileNotifyInfo->FileNameLength / sizeof(WCHAR));

                if(QLibrary::isLibrary(tmpPath)){
                    QFile file(tmpPath);
                    int tryTimes = 30;
                    while((tryTimes > 0) && (!m_isExiting) && QFile::exists(tmpPath) && (!file.open(QFile::ReadOnly))){
                        --tryTimes;
                        QThread::msleep(1000);
                    }//while
                    file.close();

                    if(m_isExiting){
                        break;
                    }else if(tryTimes > 0){//not time out
                        emit newPLCDriver(tmpPath);
                    }//time out no action
                }
            }

            if(!(fileNotifyInfo->NextEntryOffset)){
                break;
            }

            fileNotifyInfo = (PFILE_NOTIFY_INFORMATION) (((BYTE*)fileNotifyInfo) + fileNotifyInfo->NextEntryOffset);
        }//for
    }//while

    ::CloseHandle(ovl.hEvent);
    ::CloseHandle(hMonitorDir);

    qDebug () << "File System Monitor thread is exit now !!!";

}

