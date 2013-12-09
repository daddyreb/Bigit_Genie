#include "getthread.h"

GetThread::GetThread(QObject *parent)
        :QThread(parent)
{
//    connect(&getDetails, SIGNAL(GetDetailsFinish()), this, SIGNAL(ThreadFinish()));
   // connect(&getDetails, SIGNAL(GetDetailsFinish()), this, SLOT(ThreadOver()));
}

GetThread::~GetThread()
{
    wait();
}

void GetThread::StartGetDetails(QMap<QString, QMap<QString, QStringList> >**OutData)
{
	paraMap = *OutData;
    paraMap->clear();
    start();
}

void GetThread::run()
{
    qDebug() << "Start Get System's Details!";
    //getDetails.SystemDetails(&paraMap);
}

void GetThread::ThreadOver()
{
    qDebug() << "Get System's Details is Over!";
}
