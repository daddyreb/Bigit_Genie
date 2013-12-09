#include "QGenieSingleProcessHandler.h"
#include "QLaunchSemaphore.h"
#include "QGenieWatchDog.h"

#include <QApplication>
#include "SingleInstance.h"
#include <QDesktopServices>
#include "interface2_macros.h"
#define LAUNCH_SINGLEINSTANCE_LOCKFILE "NETGEARGenie.pid"


QGenieSingleProcessHandler::QGenieSingleProcessHandler()
    :mDog(new QGenieWatchDog)
#ifdef Q_OS_WIN32
    ,mSharedMemory("QGenie2SingleProcessHandler")
#endif
{
    connect(mDog,SIGNAL(dogwakeup()),this,SIGNAL(signal_OtherProcessStarted()));
}

QGenieSingleProcessHandler::~QGenieSingleProcessHandler()
{
#ifdef Q_OS_WIN32
    mSharedMemory.detach();
#endif
    delete mDog;
}

bool QGenieSingleProcessHandler::tryCreate()
{
#ifdef Q_OS_WIN32
    if(mSharedMemory.create(2) )
#else
    QString file_to_lock = NETGEAR_DIR;//QApplication::applicationDirPath();
    file_to_lock += "/";
    file_to_lock += LAUNCH_SINGLEINSTANCE_LOCKFILE;

    if(
            already_running(file_to_lock.toStdString().c_str()) == 0
            )
#endif
    {
        qApp->processEvents();
        return true;
    }
    else
    {

        qDebug("NETGEARGenie is already running!");

        QLaunchSemaphore sem;
        sem.release(2);
        return false;
    }
    return false;
}

void QGenieSingleProcessHandler::startWatch()
{
    mDog->start();
}
