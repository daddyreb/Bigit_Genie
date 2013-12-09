
#include "stable.h"

QGenieInterfaceClickManager::QGenieInterfaceClickManager()
{
}

void QGenieInterfaceClickManager::click(const QString &key)
{
    IPluginInf *pi=getStatisticPi();
    if(pi)
    {
        pi->command(COMMAND_CLICK,key);
    }
}

void QGenieInterfaceClickManager::registMaxValue(const QString &key,int v)
{
    IPluginInf *pi=getStatisticPi();
     if(pi)
     {
         pi->command(COMMAND_REGMAXVALUE,key,QString::number(v));
     }
}

void QGenieInterfaceClickManager::rememberSearchStr(const QString &s)
{
    IPluginInf *pi=getStatisticPi();
     if(pi)
     {
         pi->command(COMMAND_REMEMBER_SEARCH,s);
     }
}


IPluginInf *QGenieInterfaceClickManager::getStatisticPi()
{
    QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
    ASSERT(proxymanager);
    plug_inf *p=proxymanager->getPluginInf(QUuid(PLUGIN_UUID_STATISTIC));
    if(p && p->pi)
    {
        return p->pi;
    }
    return NULL;
}

void QGenieInterfaceClickManager::notifySaveData()
{
    IPluginInf *pi=getStatisticPi();
     if(pi)
     {
         pi->command(COMMAND_SAVE_SEARCHDATA);
     }
}
