#include <QtCore>
#include "QGenieWatchDog.h"
#include "QLaunchSemaphore.h"

QGenieWatchDog::QGenieWatchDog()
    :mOk(true)
    ,mSemaphore(new QLaunchSemaphore())
{

}

QGenieWatchDog::~QGenieWatchDog()
{
    if(mOk)
    {
        stopit();
    }
    delete mSemaphore;
}

void QGenieWatchDog::run()
{
  //  bool btemp;
//    static bool sbFirstLaunch=true;
    QDateTime dt=QDateTime::currentDateTime();
    bool canWakeup=true;
    while(true)
    {
        if(!mOk)
        {
            break;
        }
        canWakeup=mSemaphore->aquire();
        if(canWakeup && mOk && dt.addSecs(4) <QDateTime::currentDateTime())
        {
            dt=QDateTime::currentDateTime();
            emit dogwakeup();
            qDebug()<<"WatchDog Worked!!!!!";
        }
    }
    qDebug()<<"dog quit";

}

void QGenieWatchDog::stopit()
{
    mOk=false;
    mSemaphore->release(SEMAPHORE_MAX);
    this->wait();
}
