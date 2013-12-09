#include <QtCore>
#include "QGenieInterface2SubscribedMessageManager.h"
#include "QGenieInterface2Helper.h"
#include "interface2_cmds.h"

QGenieInterface2SubscribedMessageManager::QGenieInterface2SubscribedMessageManager()
{
}

void QGenieInterface2SubscribedMessageManager::subscrib(int message, int uuid)
{
    Q_ASSERT(0<=message && message< SCCMD_NUM);
    if(!mDataWho.contains(message))
    {
        mDataWho.insert(message,QList<int>());
    }
    if(!mDataWho[message].contains(uuid))
    {
        mDataWho[message].append(uuid);
    }
}

void QGenieInterface2SubscribedMessageManager::notify(const QVariant &paras)
{

    QList<QVariant> ltemp=paras.toList();
    int c=ltemp.count();
    if(c>=1)
    {
        bool btemp;
        int message=ltemp[0].toInt(&btemp);
        Q_ASSERT(btemp);
        if(c>1)
        {
            mDataCache.insert(message,ltemp[1]);
        }

        if(mDataWho.contains(message))
        {
            QGenieInterface2Helper *i2helper=QGenieInterface2Helper::GetSingletonPtr();

            if(i2helper)
            {
                foreach(int uuid,mDataWho[message])
                {
                    QList<QVariant> ltemp;
                    ltemp<<message<<paras;
                    i2helper->tellPluginMessage(uuid,I2CMD_NOTIFY_MESSAGE,paras);
                }
            }
        }
    }
}

QVariant QGenieInterface2SubscribedMessageManager::getCache(const QVariant &paras)
{
//    qDebug()<<paras;
//    qDebug()<<mDataCache;
    return (paras.type()==QVariant::Int && mDataCache.contains(paras.toInt()))
            ?mDataCache[paras.toInt()]:QVariant();
}
