//add by caiyujun
#include "sendfile.h"
#include <fstream>
#include <QMessageBox>
#include <QtDebug>
#include <io.h>
#include <sys/stat.h>
#include <QFileInfo>
#include "udt.h"

sendfile::sendfile(QString strIp, QString strFile, QString strDevName, QObject *parent) :
    QThread(parent)
{
    UDT::startup();
    m_strFileName = strFile;
    strDestIp = strIp.toStdString();
    m_iMode = 0;
    m_strDeviceName = strDevName;
    closeFlag = false;
}

sendfile::sendfile(QString strIp, QStringList fileList, QString strDevName, QObject *parent) :
        QThread(parent)
{
    UDT::startup();
    m_files = fileList;
    strDestIp = strIp.toStdString();
    m_iMode = 2;
    m_strDeviceName = strDevName;
    closeFlag = false;
}

sendfile::sendfile(QString strIp, QString strFolder, QStringList files, QStringList emptyFolders, qint64 iSize, QString strDevName, QObject *parent) :
		QThread(parent)
{
    UDT::startup();
    m_strFileName = strFolder;
    m_files = files;
    strDestIp = strIp.toStdString();
    m_iMode = 1;
    m_iFolderSize = iSize;
    m_emptyFoldersList = emptyFolders;
    m_strDeviceName = strDevName;
    closeFlag = false;
}

bool sendfile::initSocket(QString strIp, QList<QUrl> urls)
{
    struct addrinfo hints, *peer;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    m_fhandle = UDT::socket(hints.ai_family, hints.ai_socktype, hints.ai_protocol);

    strDestIp = strIp.toStdString();
    std::string strDestPort = "7777";
    if (0 != getaddrinfo(strDestIp.c_str(), strDestPort.c_str(), &hints, &peer))
    {
        return false;
    }

    // connect to the server, implict bind
    if (UDT::ERROR == UDT::connect(m_fhandle, peer->ai_addr, peer->ai_addrlen))
    {
        qDebug() << "connect error : " << UDT::getlasterror().getErrorMessage() << endl;

        return false;
    }

    freeaddrinfo(peer);
    return true;
}

void sendfile::startThread()
{
    start(HighestPriority);
}

QString sendfile::getActualFileName(QString strFileName)
{
    QString strActualFileName;
    if (strFileName.contains('/'))
    {
        QStringList strList = strFileName.split("/");
        int iSize = strList.size();
        int iLen = strFileName.length();
        if ('/' == strFileName.at(iLen-1))
        {
            strActualFileName = strList.at(iSize-2);
        }
        else
        {
            strActualFileName = strList.at(iSize-1);
        }

    }
    else if(strFileName.contains('\\'))
    {
        QStringList strList = strFileName.split("\\");
        int iSize = strList.size();
        int iLen = strFileName.length();
        if ('\\' == strFileName.at(iLen-1))
        {
            strActualFileName = strList.at(iSize-2);
        }
        else
        {
            strActualFileName = strList.at(iSize-1);
        }
    }

    return strActualFileName;
}

void sendfile::onSendProgress(qint64 iSize, double iProgress)
{
    emit sendProgress(iSize,iProgress);
}

void sendfile::run()
{
    struct addrinfo hints, *peer;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    QString strErrorMessage;
    m_fhandle = UDT::socket(hints.ai_family, hints.ai_socktype, hints.ai_protocol);
    std::string strDestPort = "7777";
    if (0 != getaddrinfo(strDestIp.c_str(), strDestPort.c_str(), &hints, &peer))
    {
        UDT::close(m_fhandle);
        return ;
    }

    // connect to the server, implict bind
    if (UDT::ERROR == UDT::connect(m_fhandle, peer->ai_addr, peer->ai_addrlen))
    {
        qDebug() << "connect error :" << UDT::getlasterror().getErrorMessage();
        sendFailMessage(UDT::getlasterror().getErrorMessage());
        return ;
    }

    freeaddrinfo(peer);

    if (0 == m_iMode)
    {
        sendfiles(m_fhandle);
    }
    else if (2 == m_iMode)
    {
        sendMultiFiles(m_fhandle);
    }
    else if (1 == m_iMode)
    {
        sendfolder(m_fhandle);
    }
}

int sendfile::sendfiles(UDTSOCKET m_fhandle)
{
    //send FSR request
    const char *ptRequest = "FSR";
    int iRequest = strlen(ptRequest);
    if (UDT::ERROR == UDT::send(m_fhandle, ptRequest, iRequest, 0))
    {
        qDebug() << "send FSR request error : " << UDT::getlasterror().getErrorMessage();
        sendFailMessage(UDT::getlasterror().getErrorMessage());
        return 0;
    }

    //get file size
    QString strActualFileName = getActualFileName(m_strFileName);
    QByteArray btFileName = m_strFileName.toLocal8Bit();
    std::string strFileName = btFileName.constData();

    QFileInfo fileInfo(m_strFileName);
    qint64 size = fileInfo.size();

    qDebug() << "(File) file size:" << size;

    strActualFileName += "\\";
    strActualFileName += m_strDeviceName;


    const char *pstrFileName = "";
    //QByteArray btActualFileName = strActualFileName.toLocal8Bit();
    QByteArray btActualFileName = strActualFileName.toUtf8();
    pstrFileName = btActualFileName.constData();
    const int iFileNameSize = strlen(pstrFileName);
    char pStrActualFileName[256];
    memset(pStrActualFileName, 0, iFileNameSize);
    memcpy(pStrActualFileName, pstrFileName, iFileNameSize);

    // send file name size
    if (UDT::ERROR == UDT::send(m_fhandle, (char*)&iFileNameSize, sizeof(int), 0))
    {
        qDebug() << "send file name size error : " << UDT::getlasterror().getErrorMessage();
        sendFailMessage(UDT::getlasterror().getErrorMessage());
        return 0;
    }

    // send file name
    if (UDT::ERROR == UDT::send(m_fhandle, pStrActualFileName, iFileNameSize, 0))
    {
        qDebug() << "send file name error :" << UDT::getlasterror().getErrorMessage();
        sendFailMessage(UDT::getlasterror().getErrorMessage());
        return 0;
    }

    // receive accept or reject result
    int iResponse = 4;
    char response[4];
    if (UDT::ERROR == UDT::recv(m_fhandle, response, iResponse, 0))
    {
        qDebug() << "recv accept or reject error : " << UDT::getlasterror().getErrorMessage();
        sendFailMessage(UDT::getlasterror().getErrorMessage());
        return 0;
    }

    if ('1' == response[3])
    {
        // send FCS request
        const char *ptContent = "FCS";
        int iContent = strlen(ptContent);
        if (UDT::ERROR == UDT::send(m_fhandle, ptContent, iContent, 0))
        {
            qDebug() << "send FCS request error : " << UDT::getlasterror().getErrorMessage();
            sendFailMessage(UDT::getlasterror().getErrorMessage());
            return 0;
        }

        std::fstream ifs(strFileName.c_str(), std::ios::in | std::ios::binary);
        if (!ifs)
        {
            qDebug() << "open file error : " << UDT::getlasterror().getErrorMessage();
            sendFailMessage("Open File Error");
            return 0;
        }

        ifs.seekg(0LL, std::ios::beg);
        // send file size
        if (UDT::ERROR == UDT::send(m_fhandle, (char*)&size, sizeof(uint64_t), 0))
        {
            qDebug() << "send file size error : " << UDT::getlasterror().getErrorMessage();
            sendFailMessage(UDT::getlasterror().getErrorMessage());
            ifs.close();
            return 0;
        }

//        CUDT *udt = CUDT::getUdt(m_fhandle);
//        udt->setFolderSize(size);
//        connect(udt,SIGNAL(sendProgress(qint64,double)),this,SLOT(onSendProgress(qint64,double)));
//        int64_t offset = 0;
//        if (UDT::ERROR == udt->sendfile(m_fhandle, ifs, offset, size))
//        {
//            qDebug() << UDT::getlasterror().getErrorMessage() << endl;
//            ifs.close();
//            return 0;
//        }


        qint64 iLastPercent = 0;

        int64_t offset = 0;
        int64_t left = size;
        bool bFirstTime = true;
        while(left > 0 && !closeFlag)
        {
            UDT::TRACEINFO trace;
            UDT::perfmon(m_fhandle, &trace);

            int send = 0;

            if (left > 51200)
                send = UDT::sendfile(m_fhandle, ifs, offset, 51200);
            else
                send = UDT::sendfile(m_fhandle, ifs, offset, left);

            if (UDT::ERROR == send)
            {
                qDebug() << "sendfile error : " << UDT::getlasterror().getErrorMessage();
                sendFailMessage(UDT::getlasterror().getErrorMessage());
                return 0;
            }else
            {
                //qDebug() << "send size: " << send << endl;
            }
            left -= send;
            offset += send;

            qint64 iPercent = (offset*100/size);
            if(iPercent ==1) {
                iPercent = 1;
            }
            if(iPercent > 100)
            {
                iPercent = iPercent;
            }
            if (iPercent != iLastPercent)
            {
                iLastPercent = iPercent;
                //emit sendProgress(iPercent, trace.mbpsSendRate / 8);

                char strContent[8];
                memset(strContent, 0, 8);
                if (UDT::ERROR == UDT::recv(m_fhandle, (char*)strContent, 4, 0))
                {
                    qDebug() << "sendfile error : " << UDT::getlasterror().getErrorMessage() << endl;
                    sendFailMessage(UDT::getlasterror().getErrorMessage());
                    return 0;
                }
                if (0 == strcmp(strContent,"FSC1"))
                {
                    emit sendProgress(iPercent, trace.mbpsSendRate / 8);
                }
            }

            if (0 == iPercent && bFirstTime)
            {
                bFirstTime = false;
                emit sendProgress(iPercent, 0);
            }
        }

        if(closeFlag)
        {
            UDT::close(m_fhandle);
            return 0;
        }else
        {
            //UDT::perfmon(m_fhandle, &trace);

            UDT::close(m_fhandle);
            emit sendSuccess();
            qDebug() << "send file end";
        }
        ifs.close();
    }
    else if ('0' == response[3])
    {
        UDT::close(m_fhandle);
        emit rejectSaveFile();
    }
    else if ('2' == response[3])
    {
        UDT::close(m_fhandle);
        emit unsupported();
    }

    //UDT::close(m_fhandle);
    return 1;
}

int sendfile::sendMultiFiles(UDTSOCKET m_fhandle)
{
    //send MSR request
    const char *ptRequest = "MSR";
    int iRequest = strlen(ptRequest);
    if (UDT::ERROR == UDT::send(m_fhandle, ptRequest, iRequest, 0))
    {
        qDebug() << "send MSR request error : " << UDT::getlasterror().getErrorMessage();
        sendFailMessage(UDT::getlasterror().getErrorMessage());
        return 0;
    }

    // send total size
    qint64 iSize = 0;
    qint64 iReceiveSize = 0;
    qint64 iLastPercent = 0;
    foreach(QString strFile, m_files)
    {
        QFileInfo fileInfo(strFile);
        qint64 iTemp = fileInfo.size();
        iSize += iTemp;
    }
    if (UDT::ERROR == UDT::send(m_fhandle, (char*)&iSize, sizeof(uint64_t), 0))
    {
        qDebug() << "send multi files size error : " << UDT::getlasterror().getErrorMessage();
        sendFailMessage(UDT::getlasterror().getErrorMessage());
        return 0;
    }

    // send file count
    int iCount = m_files.count();
    if (UDT::ERROR == UDT::send(m_fhandle, (char*)&iCount, sizeof(int), 0))
    {
        qDebug() << "send file count error : " << UDT::getlasterror().getErrorMessage();
        sendFailMessage(UDT::getlasterror().getErrorMessage());
        return 0;
    }

    QString strOneFileName = m_files.at(0);
    QString strActualFileName = getActualFileName(strOneFileName);
    strActualFileName += "\\";
    strActualFileName += m_strDeviceName;

    //QByteArray btFileName = strActualFileName.toLocal8Bit();
    QByteArray btFileName = strActualFileName.toUtf8();
    const char * pstrFileName = btFileName.constData();
    const int iFileNameSize = strlen(pstrFileName);
    char pStrActualFileName[256];
    memset(pStrActualFileName, 0, iFileNameSize);
    strcpy(pStrActualFileName, pstrFileName);
    // send file name size
    if (UDT::ERROR == UDT::send(m_fhandle, (char*)&iFileNameSize, sizeof(int), 0))
    {
        qDebug() << "send multi file name size error :" << UDT::getlasterror().getErrorMessage();
        sendFailMessage(UDT::getlasterror().getErrorMessage());
        return 0;
    }

    // send file name
    if (UDT::ERROR == UDT::send(m_fhandle, pStrActualFileName, iFileNameSize, 0))
    {
        qDebug() << "send multi file name error : " << UDT::getlasterror().getErrorMessage() << endl;
        sendFailMessage(UDT::getlasterror().getErrorMessage());
        return 0;
    }

    // receive accept or reject result
    int iResponse = 4;
    char response[4];
    if (UDT::ERROR == UDT::recv(m_fhandle, response, iResponse, 0))
    {
        qDebug() << "(FILE) recv accept or reject error : " << UDT::getlasterror().getErrorMessage();
        sendFailMessage(UDT::getlasterror().getErrorMessage());
        return 0;
    }
    if ('1' == response[3])
    {
        //CUDT *udt = CUDT::getUdt(m_fhandle);
        //udt->setFolderSize(iSize);
        //connect(udt,SIGNAL(sendProgress(qint64,double)),this,SLOT(onSendProgress(qint64,double)));

        bool bFirstTime = true;
        foreach(QString strItem, m_files)
        {

            // send MCS request
            const char *ptContent = "MCS";
            int iContent = strlen(ptContent);
            if (UDT::ERROR == UDT::send(m_fhandle, ptContent, iContent, 0))
            {
                qDebug() << "send MCS request error : " << UDT::getlasterror().getErrorMessage();
                sendFailMessage(UDT::getlasterror().getErrorMessage());
                return 0;
            }

            std::string strFileName = strItem.toStdString();
            QString strActualFileName = getActualFileName(strItem);
            qDebug() << "emit send file change";
            emit fileChange(strActualFileName);

            //QByteArray btActualFileName = strActualFileName.toLocal8Bit();
            QByteArray btActualFileName = strActualFileName.toUtf8();
            const char *pstrFileName = btActualFileName.constData();
            const int iFileNameSize = strlen(pstrFileName);
            char pStrActualFileName[256];
            memset(pStrActualFileName, 0, iFileNameSize);
            memcpy(pStrActualFileName, pstrFileName, iFileNameSize);

            QFileInfo fileInfo(strItem);
            qint64 size = fileInfo.size();

            // send file name size
            if (UDT::ERROR == UDT::send(m_fhandle, (char*)&iFileNameSize, sizeof(int), 0))
            {
                qDebug() << "send folder file name size error : " << UDT::getlasterror().getErrorMessage();
                sendFailMessage(UDT::getlasterror().getErrorMessage());
                return 0;
            }

            // send file name
            if (UDT::ERROR == UDT::send(m_fhandle, pStrActualFileName, iFileNameSize, 0))
            {
                qDebug() << "send folder file name error :" << UDT::getlasterror().getErrorMessage();
                sendFailMessage(UDT::getlasterror().getErrorMessage());
                return 0;
            }

            QByteArray btItem = strItem.toLocal8Bit();
            std::fstream ifs(btItem.constData(), std::ios::in | std::ios::binary);
            if (!ifs)
            {
                qDebug() << "open folder file error";
                QString strMessage = "Open File Error";
                sendFailMessage(strMessage);
                return 0;
            }

            // send file size
            ifs.seekg(0LL, std::ios::beg);
            if (UDT::ERROR == UDT::send(m_fhandle, (char*)&size, sizeof(uint64_t), 0))
            {
                qDebug() << "send folder file size error : " << UDT::getlasterror().getErrorMessage();
                sendFailMessage(UDT::getlasterror().getErrorMessage());
                ifs.close();
                return 0;
            }

            // send file content
            int64_t offset = 0;
            int64_t left = size;
            while(left > 0 && !closeFlag)
            {
                UDT::TRACEINFO trace;
                UDT::perfmon(m_fhandle, &trace);

                int send = 0;

                if (left > 51200)
                    send = UDT::sendfile(m_fhandle, ifs, offset, 51200);
                else
                    send = UDT::sendfile(m_fhandle, ifs, offset, left);

                if (UDT::ERROR == send)
                {
                    //strErrorMessage = UDT::getlasterror().getErrorMessage();
                    //strInfoMessage  = "sendfile: ";
                    //strErrorMessage  = UDT::getlasterror().getErrorMessage()s
                    sendFailMessage(UDT::getlasterror().getErrorMessage());
                    qDebug() << "sendfile: " << UDT::getlasterror().getErrorMessage() << endl;
                    ifs.close();
                    return 0;
                }else
                {
    //                qDebug() << "send size: " << send << endl;
                }
                left -= send;
                offset += send;
                iReceiveSize += send;

                qint64 iPercent = (iReceiveSize*100/iSize);
                if(iPercent ==1) {
                    iPercent = 1;
                }
                if(iPercent > 100)
                {
                    iPercent = iPercent;
                }
                if (iPercent != iLastPercent)
                {
                    iLastPercent = iPercent;

                    char strContent[8];
                    memset(strContent, 0, 8);
                    if (UDT::ERROR == UDT::recv(m_fhandle, (char*)strContent, 4, 0))
                    {
                        qDebug() << "sendfile error : " << UDT::getlasterror().getErrorMessage() << endl;
                        sendFailMessage(UDT::getlasterror().getErrorMessage());
                        ifs.close();
                        return 0;
                    }
                    if (0 == strcmp(strContent,"FSC1"))
                    {
                        emit sendProgress(iPercent, trace.mbpsSendRate / 8);
                    }
                }
                if (0 == iPercent && bFirstTime)
                {
                    bFirstTime = false;
                    emit sendProgress(iPercent, 0);
                }
            }
            if(closeFlag)
            {
                UDT::close(m_fhandle);
                ifs.close();
                return 0;
            }else
            {
                //UDT::perfmon(m_fhandle, &trace);

                qDebug() << "send file end";
            }
            ifs.close();
        }

        //send DSF request
        const char *ptEndSendMultiFiles = "MSF";
        int iEndSendDir = strlen(ptEndSendMultiFiles);
        if (UDT::ERROR == UDT::send(m_fhandle, ptEndSendMultiFiles, iEndSendDir, 0))
        {
            qDebug() << "send MSF request error : " << UDT::getlasterror().getErrorMessage();
            sendFailMessage(UDT::getlasterror().getErrorMessage());
            return 0;
        }

        //UDT::perfmon(m_fhandle, &trace);
        emit sendSuccess();
        UDT::close(m_fhandle);
        qDebug() << "send multi files end";
    }
    else if ('0' == response[3])
    {
        UDT::close(m_fhandle);
        emit rejectSaveFile();
    }
    else if ('2' == response[3])
    {
        UDT::close(m_fhandle);
        emit unsupported();
    }

}


int sendfile::sendfolder(UDTSOCKET m_fhandle)
{
    //send DSR request
    QString strInfoMessage;
    QString strErrorMessage;
    const char *ptRequest = "DSR";
    int iRequest = strlen(ptRequest);
    if (UDT::ERROR == UDT::send(m_fhandle, ptRequest, iRequest, 0))
    {
        qDebug() << "send DSR request error : " << UDT::getlasterror().getErrorMessage();
        sendFailMessage(UDT::getlasterror().getErrorMessage());
        return 0;
    }

    // send folder size
    if (UDT::ERROR == UDT::send(m_fhandle, (char*)&m_iFolderSize, sizeof(uint64_t), 0))
    {
        qDebug() << "send folder size error : " << UDT::getlasterror().getErrorMessage();
        sendFailMessage(UDT::getlasterror().getErrorMessage());
        return 0;
    }

    QString strActualFileName = getActualFileName(m_strFileName);
    std::string strFileName = m_strFileName.toStdString();


    QFileInfo fileInfo(strFileName.c_str());
    qint64 size = fileInfo.size();
    qDebug() << "file name :" << strFileName.c_str();
    qDebug() << "file size :" << size;

    strActualFileName += "\\";
    strActualFileName += m_strDeviceName;

    qDebug() << "folder and device:" << strActualFileName;

    //QByteArray btActualFileName = strActualFileName.toLocal8Bit();
    QByteArray btActualFileName = strActualFileName.toUtf8();
    const char *pstrFileName = btActualFileName.constData();

    const int iFileNameSize = strlen(pstrFileName);
    char pStrActualFileName[256];
    memset(pStrActualFileName, 0, iFileNameSize+1);
    strcpy(pStrActualFileName, pstrFileName);

    // send folder name size
    if (UDT::ERROR == UDT::send(m_fhandle, (char*)&iFileNameSize, sizeof(int), 0))
    {
        qDebug() << "send folder name size error : " << UDT::getlasterror().getErrorMessage();
        sendFailMessage(UDT::getlasterror().getErrorMessage());
        return 0;
    }

    // send folder name
    if (UDT::ERROR == UDT::send(m_fhandle, pStrActualFileName, iFileNameSize, 0))
    {
        qDebug() << "send folder name error : " << UDT::getlasterror().getErrorMessage();
        sendFailMessage(UDT::getlasterror().getErrorMessage());
        return 0;
    }

    // receive accept or reject result
    int iResponse = 4;
    char response[4];
    if (UDT::ERROR == UDT::recv(m_fhandle, response, iResponse, 0))
    {
        qDebug() << "recv accept or reject error : " << UDT::getlasterror().getErrorMessage();
        sendFailMessage(UDT::getlasterror().getErrorMessage());
        return 0;
    }

    if ('1' == response[3])
    {
        //send empty dirs
        //CUDT *udt = CUDT::getUdt(m_fhandle);
        //udt->setFolderSize(m_iFolderSize);
        //connect(udt,SIGNAL(sendProgress(qint64,double)),this,SLOT(onSendProgress(qint64,double)));


        foreach(QString strEmptyDir, m_emptyFoldersList)
        {
            //send DCR request
            const char *ptDirCreateReq = "DCR";
            int iDirCreateReqLen = strlen(ptDirCreateReq);
            if (UDT::ERROR == UDT::send(m_fhandle, ptDirCreateReq, iDirCreateReqLen, 0))
            {
                qDebug() << "send create dir req error : " << UDT::getlasterror().getErrorMessage();
                sendFailMessage(UDT::getlasterror().getErrorMessage());
                return 0;
            }

            QString strTemp = strEmptyDir;
            QString strTopFolderName = getActualFileName(m_strFileName); //123
            QString strActualFileName = strTemp.remove(m_strFileName);   //test
            //QString strDirName = getActualFileName(strEmptyDir);
            QString strRelativeDir;
            if (strTopFolderName.contains('/') || strTopFolderName.contains('\\'))
            {
                strRelativeDir = strTopFolderName + strActualFileName;
            }
            else
            {
                strRelativeDir = strTopFolderName;
                strRelativeDir += "/";
                strRelativeDir += strActualFileName;
            }


            //send dir name length
            //QByteArray btRelativeDir = strRelativeDir.toLocal8Bit();
            QByteArray btRelativeDir = strRelativeDir.toUtf8();
            const char *pstrRelativeDir = btRelativeDir.constData();
            int iEmptyDirLength= strlen(pstrRelativeDir);
            if (UDT::ERROR == UDT::send(m_fhandle, (char*)&iEmptyDirLength, sizeof(int), 0))
            {
                qDebug() << "send dir name length error : " << UDT::getlasterror().getErrorMessage();
                sendFailMessage(UDT::getlasterror().getErrorMessage());
                return 0;
            }
            else
            {
                qDebug() << "send folder empty dir name size success";
            }

            //send empty dir name
            qDebug() << "relative dir name : " << strRelativeDir;
            //iEmptyDirLength = strRelativeDir.length();
            if (UDT::ERROR == UDT::send(m_fhandle, pstrRelativeDir, iEmptyDirLength, 0))
            {
                qDebug() << "send dir name error : " << UDT::getlasterror().getErrorMessage();
                sendFailMessage(UDT::getlasterror().getErrorMessage());
                return 0;
            }
            else
            {
                qDebug() << "send empty dir name success";
            }
        }



        QString strTopFolderName = getActualFileName(m_strFileName);


        //send folder files
        qint64 iReceiveSize = 0;
        qint64 iLastPercent = 0;
        bool bFirstTime = true;
        foreach(QString file, m_files)
        {
            //send DFS request
            const char *ptDirFileReq = "DFS";
            int iDirFileReqLen = strlen(ptDirFileReq);
            if (UDT::ERROR == UDT::send(m_fhandle, ptDirFileReq, iDirFileReqLen, 0))
            {
                qDebug() << "send DFS request error : " << UDT::getlasterror().getErrorMessage();
                sendFailMessage(UDT::getlasterror().getErrorMessage());
                return 0;
            }

            QString strTemp = file;
            QString strActualFileName = strTemp.remove(m_strFileName);
            std::string strFileName = file.toStdString();

            QFileInfo fileInfo(file);
            qint64 size = fileInfo.size();
            qDebug() << "file size :" << size;

            QString strRelativeFileName;
            if (strTopFolderName.contains('/') || strTopFolderName.contains('\\'))
            {
                strRelativeFileName = strTopFolderName + strActualFileName;
            }
            else
            {
                strRelativeFileName = strTopFolderName;
                strRelativeFileName += "/";
                strRelativeFileName += strActualFileName;
            }
            qDebug() << "relative file name : " << strRelativeFileName;
            //QByteArray btRelativeFileName = strRelativeFileName.toLocal8Bit();
            QByteArray btRelativeFileName = strRelativeFileName.toUtf8();
            const char *pstrFileName = btRelativeFileName.constData();
            const int iFileNameSize = strlen(pstrFileName);
            char pStrActualFileName[256];
            memset(pStrActualFileName, 0, iFileNameSize);
            memcpy(pStrActualFileName, pstrFileName, iFileNameSize);

            // send folder file name size
            if (UDT::ERROR == UDT::send(m_fhandle, (char*)&iFileNameSize, sizeof(int), 0))
            {
                qDebug() << "send folder file name size error : " << UDT::getlasterror().getErrorMessage();
                sendFailMessage(UDT::getlasterror().getErrorMessage());
                return 0;
            }

            // send folder file name information
            if (UDT::ERROR == UDT::send(m_fhandle, pStrActualFileName, iFileNameSize, 0))
            {
                qDebug() << "send folder file name error :" << UDT::getlasterror().getErrorMessage();
                sendFailMessage(UDT::getlasterror().getErrorMessage());
                return 0;
            }

			QByteArray btFile = file.toLocal8Bit();
			const char *fileName = btFile.constData();
            std::fstream ifs(fileName, std::ios::in | std::ios::binary);
            if (!ifs)
            {
                qDebug() << "open folder file error" << UDT::getlasterror().getErrorMessage();
                sendFailMessage("Open File Error");

                return 0;
            }

            // send folder file size
            ifs.seekg(0LL, std::ios::beg);
            if (UDT::ERROR == UDT::send(m_fhandle, (char*)&size, sizeof(uint64_t), 0))
            {
                qDebug() << "send folder file size error : " << UDT::getlasterror().getErrorMessage();
                sendFailMessage(UDT::getlasterror().getErrorMessage());
                ifs.close();

                return 0;
            }

            // send folder file content
//            int64_t offset = 0;
//            if (UDT::ERROR == udt->sendfile(m_fhandle, ifs, offset, size))
//            {
//                qDebug() << UDT::getlasterror().getErrorMessage() << endl;
//                ifs.close();
//                return 0;
//            }

            int64_t offset = 0;
            int64_t left = size;
            while(left > 0 && !closeFlag)
            {
                UDT::TRACEINFO trace;
                UDT::perfmon(m_fhandle, &trace);

                int send = 0;

                if (left > 51200)
                    send = UDT::sendfile(m_fhandle, ifs, offset, 51200);
                else
                    send = UDT::sendfile(m_fhandle, ifs, offset, left);

                if (UDT::ERROR == send)
                {
                    //strErrorMessage = UDT::getlasterror().getErrorMessage();
                    //strInfoMessage  = "sendfile: ";
                    //strErrorMessage  = UDT::getlasterror().getErrorMessage()
                    sendFailMessage(UDT::getlasterror().getErrorMessage());
                    qDebug() << "sendfile: " << UDT::getlasterror().getErrorMessage() << endl;
                    ifs.close();
                    return 0;
                }else
                {
    //                qDebug() << "send size: " << send << endl;
                }
                left -= send;
                offset += send;

                iReceiveSize += send;

                qint64 iPercent = (iReceiveSize*100/m_iFolderSize);
                if(iPercent ==1) {
                    iPercent = 1;
                }
                if(iPercent > 100)
                {
                    iPercent = iPercent;
                }
                if (iPercent != iLastPercent)
                {
                    iLastPercent = iPercent;
                    char strContent[8];
                    memset(strContent, 0, 8);
                    if (UDT::ERROR == UDT::recv(m_fhandle, (char*)strContent, 4, 0))
                    {
                        qDebug() << "sendfile error : " << UDT::getlasterror().getErrorMessage() << endl;
                        sendFailMessage(UDT::getlasterror().getErrorMessage());
                        ifs.close();
                        return 0;
                    }
                    if (0 == strcmp(strContent,"FSC1"))
                    {
                        emit sendProgress(iPercent, trace.mbpsSendRate / 8);
                    }
                }

                if (0 == iPercent && bFirstTime)
                {
                    bFirstTime = false;
                    emit sendProgress(iPercent, 0);
                }
            }
            if(closeFlag)
            {
                UDT::close(m_fhandle);
                return 0;
            }else
            {
                //UDT::perfmon(m_fhandle, &trace);
                qDebug() << "send file end";
            }
            ifs.close();
        }

        //send DSF request
        const char *ptEndSendDir = "DSF";
        int iEndSendDir = strlen(ptEndSendDir);
        if (UDT::ERROR == UDT::send(m_fhandle, ptEndSendDir, iEndSendDir, 0))
        {
            qDebug() << "send DSF request error :" << UDT::getlasterror().getErrorMessage();
            sendFailMessage(UDT::getlasterror().getErrorMessage());
            return 0;
        }

        UDT::close(m_fhandle);
        emit sendSuccess();

        //UDT::perfmon(m_fhandle, &trace);
    }
    else if ('0' == response[3])
    {
        UDT::close(m_fhandle);
        emit rejectSaveFile();
    }
    else if ('2' == response[3])
    {
        UDT::close(m_fhandle);
        emit unsupported();
    }

    //UDT::close(m_fhandle);
    return 1;
}

void sendfile::slot_CancelSend()
{
    qDebug()<<"-------sendfile::slot_CancelSend()------";
    closeFlag = true;
}

void sendfile::sendFailMessage(QString strErrorMessage)
{
    UDT::close(m_fhandle);
    emit sendFail(strErrorMessage);
}

