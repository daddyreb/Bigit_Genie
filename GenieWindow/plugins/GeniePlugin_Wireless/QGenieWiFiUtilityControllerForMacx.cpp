#include "QGenieWiFiUtilityControllerForMacx.h"
#include <QList>
#include <QDir>
#include <QApplication>

//#include <QSharedMemory>

//#define SHAREDMEMORY_KEY "WiFiManageUtility_EXCHANGE_SERVERNAME_SHAREDMEM"

#define UTILITYNAME "WiFiManageUtility"
#define LOCALSERVER_NAME "WiFiManageUtility"

QGenieWiFiUtilityControllerForMacx::QGenieWiFiUtilityControllerForMacx(QObject *parent) :
        QObject(parent)
{
    connect(&m_utilityProcess,SIGNAL(finished(int,QProcess::ExitStatus)),
            this,SLOT(finished(int,QProcess::ExitStatus)));
}

//QString QGenieWiFiUtilityControllerForMacx::getFullServerName()
//{
//    QSharedMemory sharedMem(SHAREDMEMORY_KEY,this);

//    if(sharedMem.isAttached() || sharedMem.attach())
//    {
//        qDebug() << "shared memory data = " << (const char*)sharedMem.data();
//        return QString::fromAscii((const char*)sharedMem.data());
//    }
//    else
//    {
//        qDebug () << "shared mem key = " << sharedMem.key()
//                << "errorString = " << sharedMem.errorString();
//        return QString();
//    }
//}

bool QGenieWiFiUtilityControllerForMacx::init()
{
    if(m_utilityProcess.state() != QProcess::Running)
    {
        QString utilitypath = QApplication::applicationDirPath();
        QDir plugins_dir(utilitypath);
        plugins_dir.cdUp();
        plugins_dir.cd("PlugIns");
        utilitypath = plugins_dir.absolutePath();
        utilitypath += "/";
        utilitypath += UTILITYNAME;

        m_utilityProcess.start(utilitypath,QStringList());
        m_utilityProcess.waitForStarted();
    }

    bool bok = true;

    if(!m_socket.isValid() || (m_socket.state() != QLocalSocket::ConnectedState))
    {
        reset();

        m_socket.connectToServer(LOCALSERVER_NAME);

        bok = m_socket.waitForConnected(5000);
    }

    //qDebug () << "init" << m_socket.errorString();

    return bok;
}

bool QGenieWiFiUtilityControllerForMacx::readCommandLine(QString &cmd)
{
    bool bok = false;

    m_socket.waitForReadyRead();
    QByteArray byteArr = m_socket.readAll();

    qDebug () << "readCommandLine" << byteArr;

    if(byteArr.contains('\n'))
    {
        byteArr = m_cmdLineBuffer + byteArr;
        QList<QByteArray> tmpList = byteArr.split('\n');

        for(int k = 0 ; k < tmpList.size() - 1 ; ++k)
        {
            m_cmdLines.append(QString::fromAscii(tmpList[k].data()));
        }

        if((tmpList.size() > 0) && (!tmpList[tmpList.size() - 1].isEmpty()))
        {
            m_cmdLineBuffer = tmpList[tmpList.size() - 1];
        }
        else
        {
            m_cmdLineBuffer.clear();
        }

    }
    else
    {
        m_cmdLineBuffer += byteArr;
    }

    if(m_cmdLines.size() > 0)
    {
        cmd = m_cmdLines.takeFirst();
        bok = true;
    }

    return bok;
}

bool QGenieWiFiUtilityControllerForMacx::writeCommandLine(const QString &cmd)
{
    bool bok = true;

    const QByteArray byteArr = cmd.toAscii();
    const char *data = byteArr.data();
    int length = byteArr.length();
    int bytesWriten = 0;

    while(length > 0)
    {
        bytesWriten = m_socket.write(data,length);
        if(bytesWriten < 0)
        {
            bok = false;
            break;
        }

        length -= bytesWriten;
    }

    return bok;
}

void QGenieWiFiUtilityControllerForMacx::reset()
{
    m_socket.disconnectFromServer();
    m_socket.abort();

    m_cmdLineBuffer.clear();
    m_cmdLines.clear();
}

void QGenieWiFiUtilityControllerForMacx::finished (int exitCode,QProcess::ExitStatus exitStatus )
{
    reset();
}

bool QGenieWiFiUtilityControllerForMacx::updateProfileList(QStringList &profiles)
{
    bool bok = false;
    QString cmd = "GetProfileList\n";
    QString result;

    profiles.clear();

    if(writeCommandLine(cmd) && readCommandLine(result))
    {
        QStringList parts = result.split("##",QString::SkipEmptyParts);
        if(parts[0] == "GetProfileList")
        {
            if((parts.size() >= 2) && (parts[1] == "ok"))
            {
                for(int k = 2; k < parts.size() ; ++k)
                {
                    profiles.append(parts[k]);
                }

                bok = true;
            }
        }
    }

    return bok;
}

bool QGenieWiFiUtilityControllerForMacx::deleteProfile(int index)
{
    return executeSimpleCmd("DeleteProfile",index);
}

bool QGenieWiFiUtilityControllerForMacx::addProfile(const QString &ssid,
                                                    const QString &sec,
                                                    const QString &pwd)
{
    bool bok = false;
    QString cmd = "AddProfile##";
    cmd += ssid;
    cmd += "##";
    cmd += sec;
    cmd += "##";
    cmd += pwd;
    cmd += "\n";

    QString result;

    if(writeCommandLine(cmd) && readCommandLine(result))
    {
        QStringList parts = result.split("##",QString::SkipEmptyParts);
        if(parts[0] == "AddProfile")
        {
            if((parts.size() >= 2) && (parts[1] == "ok"))
            {
                bok = true;
            }
        }
    }

    return bok;
}

bool QGenieWiFiUtilityControllerForMacx::moveUpProfile(int index)
{
    return executeSimpleCmd("MoveUpProfile",index);
}

bool QGenieWiFiUtilityControllerForMacx::moveDownProfile(int index)
{
    return executeSimpleCmd("MoveDownProfile",index);
}

bool QGenieWiFiUtilityControllerForMacx::executeSimpleCmd(const QString &action, int index)
{
    bool bok = false;
    QString strIndex;
    strIndex.setNum(index);

    QString cmd = action;
    cmd += "##";
    cmd += strIndex;
    cmd += "\n";

    QString result;

    if(writeCommandLine(cmd) && readCommandLine(result))
    {
        QStringList parts = result.split("##",QString::SkipEmptyParts);
        if(parts[0] == action)
        {
            if((parts.size() >= 2) && (parts[1] == "ok"))
            {
                bok = true;
            }
        }
    }

    return bok;
}

bool QGenieWiFiUtilityControllerForMacx::getProfileAuthAndConnType(const QString &profileName,QString &auth,bool &binfrastructure)
{
    bool bok = false;
    QString cmd = "GetSecurityAndConnType##";
    cmd += profileName;
    cmd += "\n";

    QString result;

    if(writeCommandLine(cmd) && readCommandLine(result))
    {
        QStringList parts = result.split("##",QString::SkipEmptyParts);
        if(parts[0] == "GetSecurityAndConnType")
        {
            if((parts.size() >= 4) && (parts[1] == "ok"))
            {
                auth = parts[2];
                binfrastructure = (parts[3] == "ESS");
                bok = true;
            }
        }
    }

    return bok;
}

QGenieWiFiUtilityControllerForMacx g_WifiProfileManager;
