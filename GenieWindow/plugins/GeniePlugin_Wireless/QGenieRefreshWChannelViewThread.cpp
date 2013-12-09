#include "QGenieRefreshWChannelViewThread.h"
#include "QGenieWirelessCommon.h"
#include "QGenieWiFiUtilities.h"
#include "genie2_interface.h"
#include "genie2_netgear.h"

#ifdef Q_OS_WIN
#include <QMutex>
extern QMutex  scanwireless_lock;//declared in QGenieRefreshVisibleNetworkThread.h
#include "DiagnoseInterface.h"
extern DiagnoseInterface *diagnose_inf ;
#elif defined Q_OS_MACX
#include "WiFiApiGlobal.h"
#include "WiFiApiInterface.h"
extern WiFiApiInterface  *wifiApi_inf;
#endif


QGenieRefreshWChannelViewThread::QGenieRefreshWChannelViewThread(QObject *parent) :
        QThread(parent),
        m_vntState(VNT_WAIT),
        m_bisnetgearrouter(false),
        m_bgetchannel_fromsoap(false),
        m_bgetchannel_fromsoap_complete(false),
        m_bgetchannel_fromapi_complete(false),
        m_bgetchannel_fromapi_ok(false),
        m_sessionWireless(-1)
{
    connect(this,SIGNAL(scanwifi_complete(bool)),this,SLOT(on_scanwifi_complete(bool)));
}

QGenieRefreshWChannelViewThread::~QGenieRefreshWChannelViewThread()
{
    this->wait();
}

void QGenieRefreshWChannelViewThread::begin_refresh(bool netgearrouter)
{

    m_mutex.lock();
    if(m_vntState == VNT_RUN)
    {
        m_mutex.unlock();
        return;
    }
    m_mutex.unlock();

    m_bisnetgearrouter              = netgearrouter;
    m_bgetchannel_fromsoap          = false;
    m_bgetchannel_fromsoap_complete = false;
    m_bgetchannel_fromapi_complete  = false;
    m_bgetchannel_fromapi_ok        = false;

    this->start();
}


void QGenieRefreshWChannelViewThread::start(Priority priority)
{
    QThread::start(priority);
    m_mutex.lock();
    m_vntState = VNT_RUN;
    m_condition.wakeAll();
    m_mutex.unlock();
}

void QGenieRefreshWChannelViewThread::quit()
{
    QThread::quit();
    m_mutex.lock();
    m_vntState = VNT_QUIT;
    m_condition.wakeAll();
    m_mutex.unlock();
}

bool QGenieRefreshWChannelViewThread::scanWiFiChannel()
{
    bool bscanok   = false;
    int  channel   = 0;

    m_wifichanneldata.reset();

#ifdef Q_OS_WIN
    QList<VisibleNetwork>           vn_list;
#elif defined Q_OS_MACX
    QList<QMap<QString,QString> >   vn_maplist;
#endif

    QString nicguid = getWiFiNicUsable();
    if(nicguid.isEmpty())
    {
        goto done;
    }

#ifdef Q_OS_WIN

    scanwireless_lock.lock();
    bscanok = diagnose_inf->GetVisibleWlanNetworks(nicguid,vn_list,false);
    scanwireless_lock.unlock();

    if(bscanok)
    {
        for(int k = 0;k < vn_list.size();++k)
        {
            channel = diagnose_inf->GetWlanChannel(nicguid,vn_list[k].strSsid,
                                                   vn_list[k].strBssType,vn_list[k].bSecurityEnabled);
            if(channel > 0)
            {
                if(!m_wifichanneldata.m_ssidsPerChannel.contains(channel))
                {
                    m_wifichanneldata.m_ssidsPerChannel.insert(channel,QStringList());
                }

                m_wifichanneldata.m_ssidsPerChannel[channel].append(vn_list[k].strSsid);

                if(vn_list[k].bcurConnected)
                {
                    m_wifichanneldata.m_curChannel = channel;
                    m_wifichanneldata.m_curSSidIndex = m_wifichanneldata.m_ssidsPerChannel[channel].count() - 1;
                }
            }
        }//inner for
    }//if

#elif defined Q_OS_MACX
    if((bscanok = wifiApi_inf->ScanForNetworks(nicguid,vn_maplist)))
    {
        for(int k = 0;k < vn_maplist.size();++k)
        {
            channel = vn_maplist[k][WLAN_NETWORK_CHANNEL].toInt();
            if(channel > 0)
            {
                if(!m_wifichanneldata.m_ssidsPerChannel.contains(channel))
                {
                    m_wifichanneldata.m_ssidsPerChannel.insert(channel,QStringList());
                }

                m_wifichanneldata.m_ssidsPerChannel[channel].append(vn_maplist[k][WLAN_NETWORK_SSID]);

                if(vn_maplist[k][WLAN_NETWORK_CUR_CONNECTED].toInt() !=0)
                {
                    m_wifichanneldata.m_curChannel = channel;
                    m_wifichanneldata.m_curSSidIndex = m_wifichanneldata.m_ssidsPerChannel[channel].count() - 1;
                }
            }

        }//inner for
    }//if

#endif

    done:
    return bscanok;
}

void QGenieRefreshWChannelViewThread::run()
{
    bool bresult = false;

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

        bresult = scanWiFiChannel();

        m_mutex.lock();
        if (m_vntState == VNT_RUN)
        {
            m_vntState = VNT_WAIT;
        }

        emit scanwifi_complete(bresult);

        m_mutex.unlock();
    }
}


void QGenieRefreshWChannelViewThread::on_scanwifi_complete(bool bok)
{
    m_bgetchannel_fromapi_complete = true;
    m_bgetchannel_fromapi_ok       = bok;

    if(bok && m_bisnetgearrouter && (m_wifichanneldata.m_curChannel <= CHANNELCOUNT_IN_2DOT4GHZFREQUENCY))
    {
        m_bgetchannel_fromsoap = true;
        QString tempchannel;

        /*if(g_manager_handler)
        {
            IInfoManager *infomanger = g_manager_handler->getInfoManager();
            if(infomanger)
            {
                tempchannel = infomanger->getInfo(INFO_CMD_CHANNEL,0);
            }
        }*/

        if(tempchannel.isEmpty())
        {
            if((!m_channelfromsoap_prev.isEmpty()) || m_channelfromsoap.isEmpty())
            {
                getcurchannel_fromsoap();
            }
            else
            {
                m_bgetchannel_fromsoap_complete = true;
            }

        }
        else
        {
            m_channelfromsoap = tempchannel;
            m_bgetchannel_fromsoap_complete = true;
        }

        m_channelfromsoap_prev = tempchannel;
    }

    try_refreshchannel_allcomplete();
}

void QGenieRefreshWChannelViewThread::getcurchannel_fromsoap()
{
    m_channelfromsoap.clear();

    /*QString  str_session("getcurchannel_fromsoap");
    QMap<QString,QString> search_param;
    QString  search_module("WLANConfiguration");
    QString  search_method("GetInfo");
    QUuid uuid_ssid("{e722b972-b05e-4d71-8cd9-676ebd1535b9}");
    m_soapapi->submitrequest(search_module,search_method , search_param,uuid_ssid,str_session);*/

    m_sessionWireless = QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                         ->netgearCommand(INetgearInterface::IC_GET_WIRELESS,QStringList()
                                          ,this,SLOT(slot_DataRreturned(int,QStringList)));

    QTimer::singleShot(30000,this,SLOT(getchannelfromrouter_timeout()));
}

/*void QGenieRefreshWChannelViewThread::GetSoapResponse(QtSoapMessage &message,
                                                      QUuid &uuid, QString &id)
{
    if(uuid != QUuid("{e722b972-b05e-4d71-8cd9-676ebd1535b9}") ||
       id != QString("getcurchannel_fromsoap") || m_bgetchannel_fromsoap_complete)
    {
        return;
    }

    m_channelfromsoap.clear();

    bool bOk = (!message.isFault() && message.returnResponseCode().toString().toInt() == 0);
    if(bOk)
    {
        const QtSoapType &res = message.returnValue();
        m_channelfromsoap = res["NewChannel"].value().toString();
        if(m_channelfromsoap == ""){
            m_channelfromsoap = "Auto";
        }
    }

    m_bgetchannel_fromsoap_complete = true;

    try_refreshchannel_allcomplete();

}*/

void QGenieRefreshWChannelViewThread::slot_DataRreturned(int session,const QStringList& strList)
{
    if(session != m_sessionWireless || m_bgetchannel_fromsoap_complete)
    {
        return;
    }

    m_channelfromsoap.clear();

    bool btemp = false;
    bool bOk =(strList.size() >= 3 && (0 == strList[0].toInt(&btemp)) && btemp);

    if(bOk)
    {
        m_channelfromsoap = strList[2];
        if((m_channelfromsoap == "") || m_channelfromsoap.contains("Auto",Qt::CaseInsensitive)){
            m_channelfromsoap = "Auto";
        }
    }

    m_bgetchannel_fromsoap_complete = true;

    try_refreshchannel_allcomplete();
}

void QGenieRefreshWChannelViewThread::getchannelfromrouter_timeout()
{
    if(!m_bgetchannel_fromsoap_complete)
    {
        m_bgetchannel_fromsoap_complete = true;
        try_refreshchannel_allcomplete();
    }
}

void QGenieRefreshWChannelViewThread::try_refreshchannel_allcomplete()
{
    bool needupdate = false;
    if(m_bgetchannel_fromsoap)
    {
        if(m_bgetchannel_fromsoap_complete && m_bgetchannel_fromapi_complete)
        {
            if(!m_channelfromsoap.compare(QString("Auto"),Qt::CaseInsensitive))
            {
                m_wifichanneldata.m_bautoChannel = true;
            }
            needupdate = true;
        }
    }
    else if(m_bgetchannel_fromapi_complete)
    {
        needupdate = true;
    }

    if(needupdate)
    {
        emit refresh_complete(m_bgetchannel_fromapi_ok, &m_wifichanneldata);
    }
}

