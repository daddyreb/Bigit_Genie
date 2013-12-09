#include <QPluginLoader>
#include <assert.h>
#include <QtDebug>
#include <QSharedMemory>
#ifdef Q_OS_WIN
#include <QApplication>
#elif defined Q_OS_MACX
#include <QCoreApplication>
#endif

#include "SingleInstance.h"
#include "InternetDaemonMsgProcessor.h"
#include "InternetDaemonDefs.h"

#define SINGLE_INSTANCE_LOCKFILE "internetdaemon.pid"

#define SINGLE_INTERNET_DAEMON_MAGIC \
"{7154D6DB-9705-45a4-825D-A1DA2A72D9D7}"

#ifdef Q_OS_WIN
#include "DiagnoseInterface.h"

#define INTERNET_DAEMON_LIB           \
        "DiagnosePlugin.dll"

        DiagnoseInterface   *diagnose_inf = 0;
#elif defined Q_OS_MACX
#include "INetApiInterface.h"
#include "WiFiApiInterface.h"

#define INTERNET_DAEMON_INET_LIB "libINetApiPlugin.dylib"
#define INTERNET_DAEMON_WIFI_LIB "libWiFiApiPlugin.dylib"

        INetApiInterface *inet_api = 0;
        WiFiApiInterface *wifi_api = 0;
#include <unistd.h>
#include <stdio.h>

#define INIT_DAEMON/*\
{\
if(fork() > 0) exit(0);\
setsid();\
if(fork() > 0) exit(0);\
}*/
#endif

int main(int argc, char *argv[])
{
    qDebug()<<"Enter InternetDaemon main$$$$$$$$$$$$$$$$$";

#ifdef Q_OS_WIN
    QSharedMemory single_shared_mem(SINGLE_INTERNET_DAEMON_MAGIC);
    if(!single_shared_mem.create(4)
        && single_shared_mem.error() == QSharedMemory::AlreadyExists)
    {
        qDebug() << "Internet daemon already running";
        return -2;
    }
#elif defined Q_OS_MACX
    INIT_DAEMON

    pid_t pid = getpid();
    fwrite(&pid,sizeof(pid_t),1,stdout);
    fflush(stdout);

    QString lock_path(argv[0]);
    lock_path.resize(lock_path.lastIndexOf("/")+1);
    lock_path += SINGLE_INSTANCE_LOCKFILE;
    if( already_running(lock_path.toStdString().c_str()) )
    {
        qDebug() << "Internet daemon already running";
        return -2;
    }
#endif

    if(argc != 2 || strcmp(argv[1],INTERNET_DAEMON_START_MAGIC))
    {
        qDebug() << "Internet daemon parameter not right";
        return -111;
    }
#ifdef Q_OS_WIN
    QApplication a(argc, argv);
#elif defined Q_OS_MACX
    QCoreApplication a(argc, argv);
#endif

#ifdef Q_OS_WIN
    QPluginLoader loader(qApp->applicationDirPath()+"/"+QString(INTERNET_DAEMON_LIB));
    diagnose_inf = qobject_cast<DiagnoseInterface*>(loader.instance());
    assert(diagnose_inf);
#elif defined Q_OS_MACX
    QPluginLoader loader_inet(qApp->applicationDirPath()+QString("/")+QString(INTERNET_DAEMON_INET_LIB));
    inet_api = qobject_cast<INetApiInterface*>(loader_inet.instance());
    assert(inet_api);

    QPluginLoader loader_wifi(qApp->applicationDirPath()+QString("/")+QString(INTERNET_DAEMON_WIFI_LIB));
    wifi_api = qobject_cast<WiFiApiInterface*>(loader_wifi.instance());
    assert(wifi_api);
#endif

    new InternetDaemonMsgProcessor(&a);
    qDebug() << "Internet daemon initialized.";

    return a.exec();
}
