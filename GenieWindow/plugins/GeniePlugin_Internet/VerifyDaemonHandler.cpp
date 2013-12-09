#include "VerifyDaemonHandler.h"
#include <Security/Security.h>
#include <QProcess>
#include <QApplication>

#define UNIX_SCRIPTS_DIR (qApp->applicationDirPath() + "/scripts")
#define DAEMON_PATH (qApp->applicationDirPath() + "/NETGEARGenieDaemon")
#define WAITDONE_TIMEOUT_MS 5000

VerifyDaemonHandler::VerifyDaemonHandler()
{
}

bool VerifyDaemonHandler::doVerify()
{
    QProcess process;
    process.setWorkingDirectory(UNIX_SCRIPTS_DIR);
    process.start("./verifydaemon.sh");
    if(!process.waitForFinished(WAITDONE_TIMEOUT_MS))
    {
        return false;
    }

    QString data = process.readAll();
    data.trimmed();
    if(data.contains("1"))
    {
        return true;
    }

    return tryStartDaemon();
}

bool VerifyDaemonHandler::tryStartDaemon()
{
    AuthorizationRef    myAuthorizationRef = NULL;
    OSStatus            myStatus           = noErr;

    myStatus = AuthorizationCreate (NULL, kAuthorizationEmptyEnvironment,
                                    kAuthorizationFlagDefaults, &myAuthorizationRef);

    if(myStatus != noErr)
    {
        return false;
    }


    myStatus = AuthorizationExecuteWithPrivileges(myAuthorizationRef,DAEMON_PATH.toStdString().c_str(),kAuthorizationFlagDefaults,NULL,NULL);


    AuthorizationFree (myAuthorizationRef,
                       kAuthorizationFlagDestroyRights);

    return (myStatus == noErr);
}
