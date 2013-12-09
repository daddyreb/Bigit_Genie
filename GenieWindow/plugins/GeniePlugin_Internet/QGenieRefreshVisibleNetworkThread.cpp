#include "QGenieRefreshVisibleNetworkThread.h"
#include <assert.h>

#ifdef Q_OS_WIN
extern DiagnoseInterface *diagnose_inf;
#elif defined Q_OS_MACX
#include "WiFiApiInterface.h"
extern WiFiApiInterface *wifi_api;
#endif

QGenieRefreshVisibleNetworkThread::QGenieRefreshVisibleNetworkThread(QObject *parent) :
    QThread(parent),
#ifdef Q_OS_WIN
    m_lpvisible_network(0)
#elif defined Q_OS_MACX
    m_lpvisible_networkmaplist(0)
#endif
{

}

QGenieRefreshVisibleNetworkThread::~QGenieRefreshVisibleNetworkThread()
{
    this->wait();
}

bool QGenieRefreshVisibleNetworkThread::begin_refresh()
{
   if((! this->isRunning())
#ifdef Q_OS_WIN
       && (m_lpvisible_network != 0)
#elif defined Q_OS_MACX
       && (m_lpvisible_networkmaplist != 0)
#endif
       && (!m_strnic_guid.isEmpty()))
   {
        this->start();
        return true;
   }
   else
   {
        return false;
   }
}

#ifdef Q_OS_WIN
void QGenieRefreshVisibleNetworkThread::set_out_vnlist(QList<VisibleNetwork>  *pvn_list)
{
    assert ( pvn_list != 0 );
    m_lpvisible_network = pvn_list;
}

#elif defined Q_OS_MACX

void QGenieRefreshVisibleNetworkThread::set_out_vnmaplist(QList<QMap<QString,QString> > *pvn_maplist)
{
    assert ( pvn_maplist != 0 );

    m_lpvisible_networkmaplist = pvn_maplist;
}
#endif

void QGenieRefreshVisibleNetworkThread::set_nicguid(const QString &nicguid)
{
    if(nicguid.isEmpty())
        return;
    m_strnic_guid = nicguid;
}

void QGenieRefreshVisibleNetworkThread::run()
{
#ifdef Q_OS_WIN
    if(0 == m_lpvisible_network || m_strnic_guid.isEmpty() || 0 == diagnose_inf)
    {
         emit refresh_complete(false);
         return;
    }

    m_lpvisible_network->clear();

    if(diagnose_inf->GetVisibleWlanNetworks(m_strnic_guid,*m_lpvisible_network,false))
    {
        emit refresh_complete(true);
    }
    else
    {
        emit refresh_complete(false);
    }
#elif defined Q_OS_MACX
    if(0 == m_lpvisible_networkmaplist || m_strnic_guid.isEmpty() || 0 == wifi_api)
    {
         emit refresh_complete(false);
         return;
    }

    m_lpvisible_networkmaplist->clear();

    if(wifi_api->ScanForNetworks(m_strnic_guid,*m_lpvisible_networkmaplist))
    {
        emit refresh_complete(true);
    }
    else
    {
        emit refresh_complete(false);
    }
#endif
}
