#include "QGenieRefreshVisibleNetworkThread.h"
#include "QGenieWiFiUtilities.h"
#include <assert.h>

#ifdef Q_OS_WIN
#include <QMutex>
QMutex  scanwireless_lock;
#include "DiagnoseInterface.h"
extern DiagnoseInterface *diagnose_inf ;
#elif defined Q_OS_MACX
#include "WiFiApiGlobal.h"
#include "WiFiApiInterface.h"
extern WiFiApiInterface  *wifiApi_inf;
#endif


QGenieRefreshVisibleNetworkThread::QGenieRefreshVisibleNetworkThread(QObject *parent) :
        QThread(parent),
        m_vntState(VNT_WAIT),
#ifdef Q_OS_WIN
        m_lpvisible_network(0)
#elif defined Q_OS_MACX
        m_lpnetwork_maplist(0)
#endif
{

}

QGenieRefreshVisibleNetworkThread::~QGenieRefreshVisibleNetworkThread()
{
    this->wait();
}


#ifdef Q_OS_WIN
void QGenieRefreshVisibleNetworkThread::set_out_vnlist(QList<VisibleNetwork>  *pvn_list)
{
    assert ( pvn_list != 0 );

    m_lpvisible_network = pvn_list;
}
#elif defined Q_OS_MACX
void QGenieRefreshVisibleNetworkThread::set_out_vn_maplist(QList<QMap<QString,QString> >  *pvn_maplist)
{
    assert ( pvn_maplist != 0 );

    m_lpnetwork_maplist = pvn_maplist;
}
#endif


void QGenieRefreshVisibleNetworkThread::run()
{
    bool bresult = false;
    QString nicguid;

    for(;;)
    {
        m_mutex.lock();
        if(m_vntState == VNT_WAIT)
        {
            m_condition.wait(&m_mutex);
            m_mutex.unlock();
            continue;
        }
        else if(m_vntState == VNT_QUIT)
        {
            m_mutex.unlock();
            break;
        }
        m_mutex.unlock();

        bresult = scanNetworks(nicguid);

        m_mutex.lock();
        if (m_vntState == VNT_RUN)
        {
            m_vntState = VNT_WAIT;
        }

        emit refreshCompleted(bresult,nicguid);
        m_mutex.unlock();
    }
}

bool QGenieRefreshVisibleNetworkThread::scanNetworks(QString &nicguid)
{
    nicguid = getWiFiNicUsable();

#ifdef Q_OS_WIN
    if(0 == m_lpvisible_network || nicguid.isEmpty() || 0 == diagnose_inf)
    {
        return false;
    }

    m_lpvisible_network->clear();

    scanwireless_lock.lock();
    bool bscanok = diagnose_inf->GetVisibleWlanNetworks(nicguid,*m_lpvisible_network,false);
    scanwireless_lock.unlock();

    if(bscanok)
    {
        QStringList gateways;
        int         times = 20;
        again:
        if(diagnose_inf && diagnose_inf->GetInterfaceGateway(nicguid,gateways))
        {
            if((gateways.count()==0 || gateways[0] == QString("0.0.0.0")) && (times > 0) && (m_vntState != VNT_QUIT))
            {
                msleep(500);
                --times;
                goto again;
            }
        }
    }

    return bscanok;

#elif defined Q_OS_MACX
    if(0 == m_lpnetwork_maplist || nicguid.isEmpty() || 0 == wifiApi_inf)
    {
        return false;
    }

    m_lpnetwork_maplist->clear();
    return wifiApi_inf->ScanForNetworks(nicguid,*m_lpnetwork_maplist);
#endif
}


void QGenieRefreshVisibleNetworkThread::start(Priority priority)
{
    QThread::start(priority);
    m_mutex.lock();
    m_vntState = VNT_RUN;
    m_condition.wakeAll();
    m_mutex.unlock();
}

void QGenieRefreshVisibleNetworkThread::quit()
{
    QThread::quit();
    m_mutex.lock();
    m_vntState = VNT_QUIT;
    m_condition.wakeAll();
    m_mutex.unlock();
}
