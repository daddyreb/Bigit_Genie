#include "QGenieExport2DiskFileThread.h"

#include "GeniePlugin_WSetupPlugin.h"
#include <QFile>
#include <QDir>
#include <QtDebug>

#ifdef PLATFORM_WIN32
#include "WSetupApiInterface.h"
extern WSetupApiInterface * setupapi_inf ;
#elif defined PLATFORM_MACOSX
#include "QGenieMacOSXApi.h"
#endif

#define  EXPORT_TO_USB_FILENAME     "wsetup.ws"
#define  EXPORT_TO_USB_EXEFILENAME  "GenieWirelessConfig.exe"
#define  EXPORT_TO_USB_MACXFILENAME "GenieWirelessConfig"
#define  EXPORT_TO_USB_INF          "GenieWirelessConfig.inf"

QGenieExport2DiskFileThread::QGenieExport2DiskFileThread(QObject *parent) :
        QThread(parent)
{
    this->setTerminationEnabled(true);
}

QGenieExport2DiskFileThread:: ~QGenieExport2DiskFileThread()
{
    this->wait();
}

bool QGenieExport2DiskFileThread::begin_export(const QString &strnicguid,
                                               const QString &strssid,
                                               const QString &strprofilename_or_pwd,
                                               const QString &auth,
                                               const QString &cipher,
                                               const QString &strflashdisk )
{
    if(
#ifdef PLATFORM_WIN32
        0 == setupapi_inf ||
        strnicguid.isEmpty() ||
#endif
        this->isRunning() ||
        strssid.isEmpty() || /*strprofilename_or_pwd.isEmpty() ||*/ strflashdisk.isEmpty())
    {
        return false;
    }
    else
    {
#ifdef PLATFORM_WIN32
        m_strnicguid                       = strnicguid;
#endif
        m_strssid                          = strssid;
        m_strprofilename_or_pwd            = strprofilename_or_pwd;
        m_auth = auth;
        m_cipher = cipher;
        m_strflashdisk                     = strflashdisk;

        this->start();
        return true;
    }
}

void QGenieExport2DiskFileThread::run()
{
    if(
#ifdef PLATFORM_WIN32
       0 == setupapi_inf ||
       m_strnicguid.isEmpty() ||
#endif
       m_strssid.isEmpty() ||
       /*m_strprofilename_or_pwd.isEmpty() ||*/ m_strflashdisk.isEmpty())
    {
        emit export_completed(false);
        return;
    }

    bool bok = false;
#ifdef PLATFORM_WIN32
    QString  file_name = m_strflashdisk+EXPORT_TO_USB_FILENAME;

    if( QString("from router") == m_strnicguid)
    {
        //bok = setupapi_inf->export_to_file2(m_strssid,m_strprofilename_or_pwd,file_name);
        bok = setupapi_inf->export_to_file2_v2(m_strssid,m_strprofilename_or_pwd,m_auth,m_cipher,file_name);
     //   GeniePlugin_WSetupPlugin::output_log(QString("export wireless info from router"));
    }
    else
    {
        bok = setupapi_inf->export_to_file(m_strnicguid,m_strssid,m_strprofilename_or_pwd,file_name);
//        GeniePlugin_WSetupPlugin::output_log(QString("export wireless info from local"));
    }
#elif defined PLATFORM_MACOSX
    QString  file_name = QGenieMacOSXApi::get_save_path(m_strflashdisk,EXPORT_TO_USB_FILENAME);
    bok = QGenieMacOSXApi::export_to_file(m_strssid,m_strprofilename_or_pwd,
                                          m_auth,m_cipher,file_name);
//    GeniePlugin_WSetupPlugin::output_log(QString("export wireless info from router"));
#endif

    if(bok)
    {
        QString macx_filename;
#ifdef PLATFORM_WIN32
        file_name = m_strflashdisk+EXPORT_TO_USB_EXEFILENAME/*"GenieWirelessConfig.exe"*/;
        macx_filename = m_strflashdisk+EXPORT_TO_USB_MACXFILENAME;
#elif defined PLATFORM_MACOSX
        file_name = QGenieMacOSXApi::get_save_path(m_strflashdisk,EXPORT_TO_USB_EXEFILENAME);
        macx_filename = QGenieMacOSXApi::get_save_path(m_strflashdisk,EXPORT_TO_USB_MACXFILENAME);
#endif

        if(QFile::exists(file_name))
        {
            QFile::setPermissions(file_name,QFile::WriteOther);
            QFile::remove(file_name);
        }

        if(QFile::exists(macx_filename))
        {
            QFile::setPermissions(macx_filename,QFile::WriteOther);
            QFile::remove(macx_filename);
        }

        bok = (QFile::copy(QString(":/%1").arg(EXPORT_TO_USB_EXEFILENAME),file_name)
#ifdef WIRELESSIMPORT_MACX_SUPPORT
              /*&& QFile::copy(QString(":/%1").arg(EXPORT_TO_USB_MACXFILENAME),macx_filename)*/
               && copyMacOSAppBundle(m_strflashdisk)
#endif
              );

        if(bok)
        {
#ifdef PLATFORM_WIN32
            file_name = m_strflashdisk + EXPORT_TO_USB_INF;
#elif defined PLATFORM_MACOSX
            file_name = QGenieMacOSXApi::get_save_path(m_strflashdisk, EXPORT_TO_USB_INF);
#endif

            if(QFile::exists(file_name))
            {
                QFile::setPermissions(file_name,QFile::WriteOther);
                QFile::remove(file_name);
            }

            QFile::copy(QString(":/autorun.inf"),file_name);
        }
    }

    emit export_completed(bok);
}


bool QGenieExport2DiskFileThread::copyFile(const QString &filePath,const QString &destDir)
{
    qDebug () << "Copyt " << filePath << "to " << destDir;

    QString path2MK = filePath.mid(2,filePath.lastIndexOf("/") - 2);
    QDir workingDir(destDir);
    if(!workingDir.mkpath(path2MK))
    {
        return false;
    }

    QDir destDir_(destDir);
    QString fileName = destDir_.absolutePath() + QDir::separator() + filePath.mid(2);

    if(QFile::exists(fileName))
    {
        QFile::setPermissions(fileName,QFile::WriteOther);
        QFile::remove(fileName);
    }

    return QFile::copy(filePath,fileName);
}

bool QGenieExport2DiskFileThread::copyMacOSAppBundle(const QString &destDir)
{
    const char* file2copy_array[] =
    {
        ":/GenieWirelessConfig.app/Contents/MacOS/GenieWirelessConfig",
        ":/GenieWirelessConfig.app/Contents/Resources/English.lproj/InfoPlist.strings",
        ":/GenieWirelessConfig.app/Contents/Resources/English.lproj/MainMenu.nib",
        ":/GenieWirelessConfig.app/Contents/Resources/GenieWirelessConfig.icns",
        ":/GenieWirelessConfig.app/Contents/Info.plist",
        ":/GenieWirelessConfig.app/Contents/PkgInfo",
        NULL
    };

    int index = 0;
    while(file2copy_array[index] != NULL)
    {
        if(!copyFile(QString(file2copy_array[index]),destDir))
        {
            return false;
        }

        ++index;
    }

    return true;
}
