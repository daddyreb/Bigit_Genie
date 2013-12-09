#ifndef QGENIEINTERFACE2SUBSCRIBEDMESSAGEMANAGER_H
#define QGENIEINTERFACE2SUBSCRIBEDMESSAGEMANAGER_H
#include "Singleton.h"

class QGenieInterface2SubscribedMessageManager:public Singleton<QGenieInterface2SubscribedMessageManager>
{
public:
    QGenieInterface2SubscribedMessageManager();
    void subscrib(int message,int uuid);
    void notify(const QVariant &paras);
    //message <--> uuid list
    QMap<int,QList<int> > mDataWho;
    QMap<int,QVariant> mDataCache;
    QVariant getCache(const QVariant &paras);
};

#endif // QGENIEINTERFACE2SUBSCRIBEDMESSAGEMANAGER_H
