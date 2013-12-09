#include "QGenieMacxFixThread.h"
#include "language_Internet.h"
#include <QDesktopServices>
#include <QUrl>
#include <assert.h>
#include <QtDebug>

#include "INetApiInterface.h"
#include "WiFiApiInterface.h"
extern INetApiInterface *inet_api;
extern WiFiApiInterface *wifi_api;

#define INTERNET_STATE_OK_CODE      100
#define ROUTER_CONFIGURATION_PAGE   "http://routerlogin.net"


QGenieMacxFixThread::QGenieMacxFixThread(QObject *parent) :
        QThread(parent)
        ,binternet_stateok(false)
        ,m_ifix_canceled(0)
        ,bneed_routercfg_gui(false)
        ,m_synmethodcall_mutex(0)
        ,m_synmethodcall_condition(0)
        ,m_bmessagebox_ret(false)
        ,m_shplugincable_dlg_result(0),m_shwlanoff_dlg_result(0)
        ,m_bis_repairok(false)
        ,m_shrestartrouter_dlg_result(false)
{
    //setTerminationEnabled(true);
}


QGenieMacxFixThread::~QGenieMacxFixThread()
{
    this->wait();

    if(m_synmethodcall_mutex)
    {
        delete m_synmethodcall_mutex;
        m_synmethodcall_mutex = 0;
    }

    if(m_synmethodcall_condition)
    {
        delete m_synmethodcall_condition;
        m_synmethodcall_condition = 0;
    }
}

void QGenieMacxFixThread::run()
{
    if( (0 == inet_api) || (0 == wifi_api) )
    {
        return;
    }

    //capture fix cancel interrupt
    try{
        fixit();
    }catch(...)
    {
        emit_end_pcflash();
        emit_end_routerflash();
        emit_end_internetflash();
        emit_end_pcrouterlinkflash();
        emit_end_routerinternetlinkflash();
        emit_end_connect();

        qDebug() << "catch a cancel interrupt!!!";

        emit repair_completed(2011);//2011 fix canceled
    }
}


bool QGenieMacxFixThread::turnup_nic(const QString &nic)
{
    //interrupt ?
    AutoInterrupt(this);

    bool  bret      = false;
    int   interval  = 30;

    bret = inet_api->TurnNicLinkUporDown(nic,true);

    if(!bret)
    {
        goto fun_end;
    }

    do
    {
        if( inet_api->IsNicLinkUp(nic) == 1 )
        {
            break;
        }

        fixthread_msleep_interruptable(1000);

    }while(--interval);

    if ( inet_api->IsNicLinkUp(nic) != 1 )
    {
        bret = false;
    }
    else
    {
        //wait for nic turn up complete
        fixthread_msleep_interruptable(3000);
        bret = true;
    }

    fun_end:

    if(bret)
    {
        emit_pcrouter_link_unknown();
    }
    else
    {
        emit_pcrouter_link_failed();
    }

    emit_realtime_msg2(bret?MACX_L_INTERNET_INTERFACE_LINK_TURNUP_OK
        :MACX_L_INTERNET_INTERFACE_LINK_TURNUP_FAILED);

    return bret;
}

//return code:1--->connected ;0-->not connected;-1-->not connected user canceled
int QGenieMacxFixThread::connect_wlan(const QString &nic_name)
{
    //interrupt ?
    AutoInterrupt(this);

    QString str_ssid;
    QString str_pwd;
    int     iret        = 0;

    m_shvndlg_retssid.clear();
    m_shvndlg_retpwd.clear();

    emit show_visiblenetworklist_dlg(nic_name,&str_ssid,&str_pwd);
    //wait return
    wait_synmethodcall();

    str_ssid = this->m_shvndlg_retssid;
    str_pwd  = this->m_shvndlg_retpwd;
    //return

    if ( !str_ssid.isEmpty() )
    {
        emit_begin_connect();
        emit_realtime_msg2(L_INTERNET_RT_TRYCONNECT_WLAN,str_ssid);
        if (wifi_api->AssociateToNetwork(nic_name,str_ssid,str_pwd))
        {
            iret=1;
        }

        emit_end_connect();
    }
    else
    {
        iret = -1;
    }

    return iret;
}

int QGenieMacxFixThread::verify_gateway()
{
    //interrupt ?
    AutoInterrupt(this);

    emit_realtime_msg2(L_INTERNET_RT_TESTING_GATEWAY);
    emit_begin_pcrouterlinkflash();

    fixthread_msleep_interruptable(20000);

    //enable long time interrupt
    inet_api->enable_longtime_operation_interruptable((const int*)&m_ifix_canceled);
    int igateway = inet_api->IsPrimaryServiceRouterOk(true);
    inet_api->disable_longtime_operation_interruptable();

    //interrupted ?
    process_cancel_interrupt();

    emit_end_pcrouterlinkflash();

    switch(igateway)
    {
    case 1:
        emit_pcrouter_link_ok();
        emit_realtime_msg2(L_INTERNET_RT_GATEWAY_VALID);
        break;
    case 0:
        emit_pcrouter_link_failed();
        emit_realtime_msg2(L_INTERNET_RT_GATEWAY_INVALID);
        break;
    case 2:
        emit_pcrouter_link_ok();
        emit_routerinternet_link_failed();
        bneed_routercfg_gui = true;
        emit_realtime_msg2(L_INTERNET_RT_GATEWAY_OUTSIDEPORT_INVALID);
        break;
    default:
        emit_pcrouter_link_unknown();
        emit_realtime_msg2(L_INTERNET_RT_GATEWAY_STATE_UNKNOWN);
    }

    return igateway;
}

//
int QGenieMacxFixThread::verify_dns()
{
    //interrupt ?
    AutoInterrupt(this);

    emit_realtime_msg2(L_INTERNET_RT_TESTING_DNS);
    emit_begin_routerinternetlinkflash();

    //enable long time interrupt
    inet_api->enable_longtime_operation_interruptable((const int*)&m_ifix_canceled);
    int idnsstate = inet_api->IsPrimaryServiceDnsOk(true);
    inet_api->disable_longtime_operation_interruptable();

    //interrupted ?
    process_cancel_interrupt();

    emit_end_routerinternetlinkflash();

    switch(idnsstate)
    {
    case 1:
        emit_routerinternet_link_ok();
        emit_realtime_msg2(L_INTERNET_RT_DNS_VALID);
        break;
    case 0:
        emit_routerinternet_link_failed();
        emit_realtime_msg2(L_INTERNET_RT_DNS_INVALID);
        break;
    default:
        emit_routerinternet_link_unknown();
        emit_realtime_msg2(L_INTERNET_RT_DNS_STATE_UNKNOWN);
    }

    return idnsstate;
}

int QGenieMacxFixThread::verify_gateway_dns(const QString &nic_name)
{
    //interrupt ?
    AutoInterrupt(this);

    int iret = 0;

    if( verify_gateway() == 1 )
    {
        iret = 2;
        if( verify_dns() == 1 )
        {
            iret = 1;
        }
    }
    else
    {
        emit_realtime_msg2(L_INTERNET_RT_DNS_TEST_CANNOTDO);
    }

    return iret;
}

QString QGenieMacxFixThread::get_primary_interface()
{
    //interrupt ?
    AutoInterrupt(this);

    QString str_inf,str_service;

    inet_api->GetPrimaryInterfaceAndPrimaryService(str_inf,str_service);

    return str_inf;
}

QString QGenieMacxFixThread::get_primary_service()
{
    //interrupt ?
    AutoInterrupt(this);

    QString str_inf,str_service;

    inet_api->GetPrimaryInterfaceAndPrimaryService(str_inf,str_service);

    return str_service;
}

bool QGenieMacxFixThread::verify_services_ofnic(const QString &nic_name)
{
    //interrupt ?
    AutoInterrupt(this);

    QStringList     services_list;
    QString         service_name;
    QString         service_dhcp_on_and_setupdns;
    bool            bok             = false;
    bool            svr_dhcp_exsit  = false;

    emit_realtime_msg2(MACX_L_INTERNET_VERIFY_SERVICES_ON_INTERFACE,nic_name);

    if( !inet_api->GetPortSetServiceIDs(nic_name,NULL,services_list) )
    {
        emit_realtime_msg2(MACX_L_INTERNET_GET_SERVICES_ON_INTERFACE_FAILED);
        return false;
    }

    foreach (QString svrid,services_list)
    {

        //interrupt ?
        AutoInterrupt(this);

        service_name.clear();
        inet_api->ServiceIdtoServiceName(svrid,service_name);
        emit_realtime_msg2(MACX_L_INTERNET_BEGIN_DIAGNOSE_SERVICE,service_name);

        bool active             = false;
        bool is_anychange       = false;
        bool service_dhcp_on    = (inet_api->IsServiceIPv4EntityCfgMethodSetupDhcp(NULL,svrid) == 1);
        bool service_setupdns   = (inet_api->IsServiceExsitSetupDnses(NULL,svrid) == 1);

        if( !svr_dhcp_exsit && service_dhcp_on )
        {
            svr_dhcp_exsit = service_dhcp_on;
        }

        if( service_dhcp_on && service_setupdns && service_dhcp_on_and_setupdns.isEmpty() )
        {
            service_dhcp_on_and_setupdns = svrid;
        }

        if( inet_api->IsServiceActive(NULL,svrid) != 1 )
        {
            active = is_anychange = set_service_active(svrid);
        }
        else
        {
            active = true;
            emit_realtime_msg2(MACX_L_INTERNET_SERVICE_IS_ACTIVE);
        }

        if(active)
        {
            active = false;
            if( inet_api->IsServiceIPv4Active(NULL,svrid) != 1 )
            {
                active = is_anychange = set_serviceipv4_active(svrid);
            }
            else
            {
                active = true;
                emit_realtime_msg2(MACX_L_INTERNET_SERVICEIPV4_IS_ACTIVE);
            }
        }

        if(active)
        {
            if( get_primary_service() != svrid )
            {
                active = false;

                if(make_servicefirst_in_serviceorder(svrid))
                {
                    //ok
                    active = true;
                    is_anychange = true;
                }
            }
        }

        if(active)
        {
            if(is_anychange || (service_dhcp_on && (!service_setupdns) && iprenew_service(svrid)))
            {
                if(is_anychange)
                {
                    fixthread_msleep_interruptable(5000);
                }

                if( verify_gateway_dns(nic_name) == 1 )
                {
                    bok = true;
                    break;
                }
            }
        }

    }//for

    if( !bok )
    {
        QString priSvrid;

        if(svr_dhcp_exsit && (!service_dhcp_on_and_setupdns.isEmpty()))
        {
            priSvrid = service_dhcp_on_and_setupdns;
        }
        else
        {
            priSvrid = services_list[0];
        }

        //enable dhcp ,and remove dns setup manually on this service
        if((!enable_service_dhcp(priSvrid)) ||
           (!remove_service_setupdns(priSvrid)))
        {
            priSvrid.clear();
        }

        if(!priSvrid.isEmpty())
        {
            int icode = verify_gateway_dns(nic_name);
            if( icode == 1 )
            {
                bok = true;
            }
            else if(icode == 2)
            {
                if(config_service_setupdns(priSvrid))
                {
                    bok = (verify_gateway_dns(nic_name) == 1);
                }
            }
        }
    }

    return bok;
}

bool QGenieMacxFixThread::set_service_active(const QString &svrid)
{
    //interrupt ?
    AutoInterrupt(this);

    assert (!svrid.isEmpty());
    bool bresult = false;

    emit_realtime_msg2(MACX_L_INTERNET_SERVICE_IS_NOTACTIVE);
    //service is not active
    if( inet_api->SetServiceActive(NULL,svrid,true) )
    {
        bresult = true;

        fixthread_msleep_interruptable(5000);
        emit_realtime_msg2(MACX_L_INTERNET_MAKE_SERVICE_ACTIVE_OK);
    }
    else
    {
        emit_realtime_msg2(MACX_L_INTERNET_MAKE_SERVICE_ACTIVE_FAILED);
    }

    return bresult;
}

bool QGenieMacxFixThread::set_serviceipv4_active(const QString &svrid)
{
    //interrupt ?
    AutoInterrupt(this);

    assert (!svrid.isEmpty());
    bool bresult = false;

    emit_realtime_msg2(MACX_L_INTERNET_SERVICEIPV4_IS_NOTACTIVE);
    if( inet_api->SetServiceIPv4Active(NULL,svrid,true) )
    {
        bresult = true;
        fixthread_msleep_interruptable(5000);
        emit_realtime_msg2(MACX_L_INTERNET_MAKE_SERVICEIPV4_ACTIVE_OK);
    }
    else
    {
        emit_realtime_msg2(MACX_L_INTERNET_MAKE_SERVICEIPV4_ACTIVE_FAILED);
    }

    return bresult;
}

bool QGenieMacxFixThread::make_servicefirst_in_serviceorder(const QString &svrid)
{
    //interrupt ?
    AutoInterrupt(this);

    assert (!svrid.isEmpty());

    bool bresult = false;

    emit_realtime_msg2(MACX_L_INTERNET_MAKE_SERVICE_PRIMARY);
    //try to make it the primary service
    if( inet_api->MoveSetServiceFirstInServiceOrder(NULL,svrid) )
    {
        fixthread_msleep_interruptable(5000);
        //ok
        bresult = true;
        emit_realtime_msg2(MACX_L_INTERNET_MAKE_SERVICE_PRIMARY_OK);
    }
    else
    {
        //failed
        emit_realtime_msg2(MACX_L_INTERNET_MAKE_SERVICE_PRIMARY_FAILED);
    }

    return bresult;
}

bool QGenieMacxFixThread::remove_service_setupdns(const QString &svrid)
{
    //interrupt ?
    AutoInterrupt(this);

    assert (!svrid.isEmpty());

    bool btemp = false;

    emit_realtime_msg2(MACX_L_INTERNET_SETUPDNS_EXSIT_TRY_REMOVE_THEM);
    if( inet_api->SetServiceSetupDnses(NULL,svrid,NULL) )
    {
        emit_realtime_msg2(MACX_L_INTERNET_REMOVE_SETUPDNS_OK);

        if( get_primary_service() != svrid )
        {
            btemp = make_servicefirst_in_serviceorder(svrid);
        }
        else
        {
            btemp = true;
        }
    }
    else
    {
        //failed
        emit_realtime_msg2(MACX_L_INTERNET_REMOVE_SETUPDNS_FAILED);
    }

    return btemp;
}

bool QGenieMacxFixThread::config_service_setupdns(const QString &svrid)
{
    //interrupt ?
    AutoInterrupt(this);

    assert (!svrid.isEmpty());

    bool btemp = false;

    emit_realtime_msg2(L_INTERNET_RT_TRY_SETDEFAULT_DNS);

    QStringList opendnslist;
    opendnslist << "208.67.222.222" << "208.67.220.220";

    if( (btemp = inet_api->SetServiceSetupDnses(NULL,svrid,&opendnslist)) )
    {
        emit_realtime_msg2(L_INTERNET_RT_SETDEFAULT_DNS_OK);
    }
    else
    {
        //failed
        emit_realtime_msg2(L_INTERNET_RT_SETDEFAULT_DNS_FAILED);
    }

    return btemp;
}

bool QGenieMacxFixThread::enable_service_dhcp(const QString &svrid)
{
    //interrupt ?
    AutoInterrupt(this);

    assert (!svrid.isEmpty());

    bool btemp = false;

    emit_realtime_msg2(L_INTERNET_RT_DHCPDIS_TRY_ENABLEIT);
    if( inet_api->SetServiceIPv4EntityCfgMethod2Dhcp(NULL,svrid) )
    {
        emit_realtime_msg2(L_INTERNET_RT_ENDHCP_OK);

        if( get_primary_service() != svrid )
        {
            btemp = make_servicefirst_in_serviceorder(svrid);
        }
        else
        {
            btemp = true;
        }
    }
    else
    {
        //failed
        emit_realtime_msg2(L_INTERNET_RT_ENABLEDHCP_FAILED);
    }

    return btemp;
}

bool QGenieMacxFixThread::iprenew_service(const QString &svrid)
{
    //interrupt ?
    AutoInterrupt(this);

    emit_realtime_msg2(L_INTERNET_RT_DHCPENABLE_TRY_RENEW_IP);

    if( inet_api->IpRenew(svrid) )
    {
        emit_realtime_msg2(L_INTERNET_RT_RENEW_IP_OK);
        return true;
    }
    else
    {
        emit_realtime_msg2(L_INTERNET_RT_RENEW_IP_FAILED);
        return false;
    }
}

void QGenieMacxFixThread::sort_niclist(QStringList &nic_list)
{
    int     count       = nic_list.size();
    bool    wireless    = false;

    for(int i = 0;i < count;++i)
    {
        assert (inet_api->IsNicWireless(nic_list[i],&wireless) == 0);

        if(wireless)
        {
            nic_list.prepend(nic_list[i]);
            nic_list.removeAt(i+1);
        }
    }
}

//current location changed return true ,otherwise return false
bool QGenieMacxFixThread::verify_location()
{
    //interrupt ?
    AutoInterrupt(this);

    qDebug () << "verify_location";
    emit_realtime_msg2(MACX_L_INTERNET_BEGIN_VERIFY_LOCATION);

    QStringList sets;
    QStringList param_list;
    QString     set_name;
    QString     cur_setid;
    bool        bret        = false;
    int         cursetidx   = -1;

    if( !inet_api->GetAllSets(sets) || (sets.size() < 2) || !inet_api->GetCurrentSet(cur_setid) )
    {
        goto fun_end;
    }

    foreach(QString setid,sets)
    {
        assert (inet_api->SetIdtoSetName(setid,set_name));
        if( setid != cur_setid )
        {
            param_list.append(set_name);
            param_list.append(setid);
        }
        else//current set postion first
        {
            param_list.prepend(setid);
            param_list.prepend(set_name);
        }

        set_name.clear();
    }

    assert ( param_list.size() != 0 );

    m_chose_setid.clear();

    emit show_chooselocation_dlg(param_list);

    //wait return
    wait_synmethodcall();

    if( (m_chose_setid.isEmpty()) || (m_chose_setid == cur_setid) )
    {
        goto fun_end;
    }

    qDebug() << "user choose location:" << m_chose_setid;
    qDebug() << "current location:" <<cur_setid;

    bret = inet_api->SetCurrentSet(m_chose_setid);

    fun_end:
    if(bret)
    {
        //set id to set name
        cursetidx = param_list.indexOf(m_chose_setid);
        assert ( cursetidx > 0 );
        emit_realtime_msg2(MACX_L_INTERNET_SWITCH_TO_LOCATION_OK,param_list[cursetidx-1]);
    }
    else
    {
        emit_realtime_msg2(MACX_L_INTERNET_SWITCH_TO_LOCATION_FAILED);
    }

    return bret;
}

bool QGenieMacxFixThread::verify_activeservice_exist(const QString &nic_name)
{
    //interrupt ?
    AutoInterrupt(this);

    bool        bret    = false;
    QStringList svrids;
    QString     svrid;
    QString     svrname;

    qDebug () << "verify_service_exist";
    emit_realtime_msg2(MACX_L_INTERNET_VERIFY_SERVICE_EXIST,nic_name);

    inet_api->GetPortSetServiceIDs(nic_name,NULL,svrids);

    if(svrids.isEmpty())
    {
        qDebug() << "interface :" << nic_name << "has no service";
        emit_realtime_msg2(MACX_L_INTERNET_NO_SERVICE_EXIST_TRYCREATE);
        if( bret = inet_api->CreateService(nic_name,NULL,svrid) )
        {
            bret = true;

            fixthread_msleep_interruptable(2000);
            qDebug() << "create service" << svrid << "on interface " << nic_name;

            svrname.clear();
            inet_api->ServiceIdtoServiceName(svrid,svrname);
            emit_realtime_msg2(MACX_L_INTERNET_CREATE_SERVICE_OK,svrname);
        }
        else
        {
            emit_realtime_msg2(MACX_L_INTERNET_CREATE_SERVICE_FAILED);
        }

    }
    else
    {
        if( inet_api->IsServiceActive(NULL,svrids[0]) != 1 )
        {
            bret = set_service_active(svrids[0]);
        }
        else
        {
            bret  = true;
        }

    }

    emit_realtime_msg2(bret?MACX_L_INTERNET_VERIFY_SERVICE_EXIST_OK:MACX_L_INTERNET_VERIFY_SERVICE_EXIST_FAILED);

    return bret;
}

bool QGenieMacxFixThread::verify_link(const QString &nic_name)
{
    //interrupt ?
    AutoInterrupt(this);

    qDebug () << "verify_link";
    emit_realtime_msg2(MACX_L_INTERNET_VERIFY_INTERFACE_LINK,nic_name);
    bool bret = false;
    int  link = inet_api->IsNicLinkUp(nic_name);

    if( 1 == link )//up state
    {
        bret = true;
        //output prompt
        emit_realtime_msg2(MACX_L_INTERNET_INTERFACE_LINK_UP);
    }
    else if( 0 == link )//down state
    {
        //try to turn it up
        emit_realtime_msg2(MACX_L_INTERNET_INTERFACE_LINK_DOWN);
        bret = turnup_nic(nic_name);
    }
    else//get link state failed
    {
        assert (0);
    }

    return bret;
}

bool QGenieMacxFixThread::verify_cable(const QString &nic_name)
{
    //interrupt ?
    AutoInterrupt(this);

    int         iMsgRet             = 0;
    bool        bPluggedin          = false;

    if(inet_api->IsNicCablePluggedin(nic_name))
    {
        return true;
    }

    //output cable pugin out prompt

    do
    {
        //interrupt ?
        AutoInterrupt(this);

        emit show_plugincable_dlg(&iMsgRet);
        wait_synmethodcall();

        iMsgRet = m_shplugincable_dlg_result;

        if (1 == iMsgRet)
        {
            bPluggedin = inet_api->IsNicCablePluggedin(nic_name);
        }
        else
        {
            break;
        }

    } while (!bPluggedin);

    if (!bPluggedin)
    {
        emit_realtime_msg2(L_INTERNET_RT_CABLE_STILLNOTIN);
        emit_pcrouter_link_failed();
    }
    else
    {
        emit_realtime_msg2(L_INTERNET_RT_CABLE_PLUGINNOW);
        emit_pcrouter_link_unknown();
    }

    return bPluggedin;
}

bool QGenieMacxFixThread::verify_power(const QString &nic_name)
{
    //interrupt ?
    AutoInterrupt(this);

    emit_pcrouter_link_unknown();
    emit_realtime_msg2(L_INTERNET_RT_TESTING_WIRELESSRADIO);

    bool berr       = false;
    bool power      = false;
    bool bresult    = true;

    power = wifi_api->IsWlanNicPowerOn(nic_name,&berr);

    assert (!berr);

    if(power)//power on
    {
        emit_realtime_msg2(L_INTERNET_RT_WIRELESSRADIO_IS_ON);
    }
    else//power off
    {
        emit_pcrouter_link_failed();
        emit_realtime_msg2(L_INTERNET_RT_WIRELESSRADIO_SOFTOFF);

        if ( wifi_api->SetWlanNicPowerOn(nic_name,true) )
        {
            emit_pcrouter_link_unknown();
            emit_realtime_msg2(L_INTERNET_RT_SOFT_SWITCHON_RADIO_OK);
        }
        else
        {
            bresult = false;
            emit_realtime_msg2(L_INTERNET_RT_SOFT_SWITCHON_RADIO_FAILED);
        }
    }

    return bresult;
}

void QGenieMacxFixThread::fixit()
{
    QStringList nic_list;
    bool bwireless = false;
    int  i         = 0;
    int  wlan_conn = 0;

    m_bis_repairok  = false;
    m_ifix_canceled = 0;
    bneed_routercfg_gui = false;


    emit_realtime_msg2(L_INTERNET_RT_BRIEFPROMPT_RUNNINNG_INTERNET_FIX);
    emit_realtime_msg2(L_INTERNET_RT_TRYTO_REPAIRNETWORK);
    emit_begin_pcflash();

    if(verify_location())//choose location
    {
        fixthread_msleep_interruptable(5000);

        QString pri_inf = get_primary_interface();
        if(  !pri_inf.isEmpty() )
        {
            if( verify_gateway_dns(pri_inf) == 1 )
            {
                m_bis_repairok = true;
                goto for_fix_nic_end;
            }
        }
    }

    if (!inet_api->EnumInterfaces(nic_list) || nic_list.size() == 0)
    {
        emit_realtime_msg2(L_INTERNET_RT_GETNICS_FAILED);
        goto repair_end;
    }

    sort_niclist(nic_list);

    for(i = 0;i < nic_list.size();++i)
    {
        emit_realtime_msg2(L_INTERNET_RT_TRYREPAIR_NIC_WAIT,nic_list[i]);

        bwireless = false;
        bneed_routercfg_gui = false;

        if(!verify_activeservice_exist(nic_list[i]))
        {
            goto for_fix_nic_end;
        }

        if(!verify_link(nic_list[i]))
        {
            goto for_fix_nic_end;
        }

        assert ( inet_api->IsNicWireless(nic_list[i],&bwireless) == 0 );

        if(bwireless)
        {
            if(!verify_power(nic_list[i]))
            {
                goto for_fix_nic_end;
            }
            else
            {
                fixthread_msleep_interruptable(5000);
            }

            if( !wifi_api->IsWLanNicConnected(nic_list[i]) )
            {
                lbconnwlan:
                wlan_conn = connect_wlan(nic_list[i]);

                if( wlan_conn == -1 )
                {
                    goto for_fix_nic_end;
                }
                else if( wlan_conn == 0 )
                {
                    goto lbconnwlan;//connect failed repeat
                }
                //else connect wlan ok go ahead
            }

        }
        else//wired nic
        {
            if(!verify_cable(nic_list[i]))
            {
                goto for_fix_nic_end;
            }
        }

        fixthread_msleep_interruptable(5000);

        if( get_primary_interface() == nic_list[i] )
        {
            if( verify_gateway_dns(nic_list[i]) == 1 )
            {
                m_bis_repairok = true;
                goto for_fix_nic_end;
            }
        }

        if(bneed_routercfg_gui)
        {
            goto for_fix_nic_end;
        }

        if( verify_services_ofnic(nic_list[i]) )
        {
            m_bis_repairok = true;
            //goto for_fix_nic_end;
        }

        for_fix_nic_end:

        emit_realtime_msg2((m_bis_repairok?L_INTERNET_RT_REPAIR_NIC_OK:L_INTERNET_RT_REPAIR_NIC_FAILED),nic_list[i]);
        if(m_bis_repairok || bneed_routercfg_gui)
        {
            break;
        }

    }

    repair_end:
    int fixstate = 0;
    emit_end_pcflash();
    if(m_bis_repairok)
    {
        fixstate = 1;
        emit_realtime_msg2(L_INTERNET_RT_BRIEFPROMPT_FIX_OK);
    }
    else
    {
        if(bneed_routercfg_gui)
        {
            //restart the router
            try_restartrouter(nic_list[i]);
            if(m_bis_repairok)
            {
                goto repair_end;
            }
            //end

            fixstate = 2;
            emit_realtime_msg2(L_INTERNET_RT_ROUTER_WLANCABLE_MAY_PLUGINOUT);
            emit_realtime_msg2(L_INTERNET_RT_OPENCONFIGURATION_GUI);
            QDesktopServices::openUrl(QUrl(QString(ROUTER_CONFIGURATION_PAGE)));
        }
        else
        {
            fixstate = 0;
            emit_realtime_msg2(L_INTERNET_RT_BRIEFPROMPT_FIX_FAILED);
        }

    }

    emit repair_completed(fixstate);
}


void QGenieMacxFixThread::emit_realtimemsg(const QString &msg)
{
    emit realtime_message(msg);
}

void QGenieMacxFixThread::emit_realtime_msg2(int idx,const QString &param)
{
    emit realtime_msg2(idx,param);
}

void QGenieMacxFixThread::emit_begin_pcflash()
{
    emit begin_pcflash();
}

void QGenieMacxFixThread::emit_end_pcflash()
{
    emit end_pcflash();
}

void QGenieMacxFixThread::emit_begin_routerflash()
{
    emit begin_routerflash();
}

void QGenieMacxFixThread::emit_end_routerflash()
{
    emit end_routerflash();
}

void QGenieMacxFixThread::emit_begin_internetflash()
{
    emit begin_internetflash();
}

void QGenieMacxFixThread::emit_end_internetflash()
{
    emit end_internetflash();
}

void QGenieMacxFixThread::emit_begin_pcrouterlinkflash()
{
    emit begin_pcrouterlinkflash();
}

void QGenieMacxFixThread::emit_end_pcrouterlinkflash()
{
    emit end_pcrouterlinkflash();
}

void QGenieMacxFixThread::emit_begin_routerinternetlinkflash()
{
    emit begin_routerinternetlinkflash();
}

void QGenieMacxFixThread::emit_end_routerinternetlinkflash()
{
    emit end_routerinternetlinkflash();
}

void QGenieMacxFixThread::emit_begin_connect()
{
    emit begin_connect();
}

void QGenieMacxFixThread::emit_end_connect()
{
    emit end_connect();
}

void QGenieMacxFixThread::emit_pcrouter_link_ok()
{
    emit pcrouter_link_ok();
}
void QGenieMacxFixThread::emit_pcrouter_link_failed()
{
    emit pcrouter_link_failed();
}

void QGenieMacxFixThread::emit_pcrouter_link_unknown()
{
    emit pcrouter_link_unknown();
}

void QGenieMacxFixThread::emit_routerinternet_link_ok()
{
    emit routerinternet_link_ok();
}

void QGenieMacxFixThread::emit_routerinternet_link_failed()
{
    emit routerinternet_link_failed();
}

void QGenieMacxFixThread::emit_routerinternet_link_unknown()
{
    emit routerinternet_link_unknown();
}

bool QGenieMacxFixThread::messagebox(int title_id,int text_id,bool byesorno)
{
    bool bresult = false;
    emit show_messagebox(title_id,text_id,byesorno,&bresult);
    //wait return
    wait_synmethodcall();

    bresult = this->m_bmessagebox_ret;
    //return
    return bresult;
}

void QGenieMacxFixThread::update_internetstate(bool bok)
{
    binternet_stateok = bok;
}

void QGenieMacxFixThread::show_messagebox_return(bool ret)
{
    m_bmessagebox_ret = ret;
    synmethodcall_return();
}

void QGenieMacxFixThread::show_visiblenetworklist_dlg_return
        (const QString &ssid,const QString &pwd)
{
    m_shvndlg_retssid = ssid;
    m_shvndlg_retpwd  = pwd;
    synmethodcall_return();
}

#ifdef Q_OS_MACX
void QGenieMacxFixThread::show_chooselocation_dlg_return(const QString &setname,const QString &setid)
{
    m_chose_setid = setid;
    synmethodcall_return();
}
#endif

void QGenieMacxFixThread::show_plugincable_dlg_return(int ret)
{
    m_shplugincable_dlg_result = ret;
    synmethodcall_return();
}

void QGenieMacxFixThread::show_wlanoff_dlg_return(int ret)
{
    m_shwlanoff_dlg_result = ret;
    synmethodcall_return();
}

void QGenieMacxFixThread::restore_nicsstate()
{

}

void QGenieMacxFixThread::start(Priority priority)
{
    if(m_synmethodcall_mutex)
    {
        delete m_synmethodcall_mutex;
    }

    m_synmethodcall_mutex = new QMutex();

    if(m_synmethodcall_condition)
    {
        delete m_synmethodcall_condition;
    }

    m_synmethodcall_condition = new QWaitCondition();

    QThread::start(priority);
}

void QGenieMacxFixThread::wait_synmethodcall()
{
    m_synmethodcall_mutex->lock();
    m_synmethodcall_condition->wait(m_synmethodcall_mutex);
    m_synmethodcall_mutex->unlock();
}

void QGenieMacxFixThread::synmethodcall_return()
{
    //m_synmethodcall_mutex->lock();
    m_synmethodcall_condition->wakeOne();
    //m_synmethodcall_mutex->unlock();
}

void QGenieMacxFixThread::cancel_fix_process(int code)
{
    m_ifix_canceled = 1;
}

void QGenieMacxFixThread::process_cancel_interrupt()
{
    if(m_ifix_canceled)
    {
        throw LONGTIME_OPERATION_INTERRUPT;
    }
}

void QGenieMacxFixThread::fixthread_msleep_interruptable(unsigned long ms)
{
    assert ( ms > 0 );

    unsigned long times = ms / 100;
    unsigned long remainder = ms % 100;

    if(times > 0)
    {
        do
        {
            process_cancel_interrupt();
            QThread::msleep(100);
        }while(--times > 0);
    }

    if(remainder > 0)
    {
        process_cancel_interrupt();
        QThread::msleep(remainder);
    }

}

//for restart router
void QGenieMacxFixThread::show_restartrouter_dlg_return(int result)
{
    m_shrestartrouter_dlg_result = (result != 0);
    synmethodcall_return();
}

void QGenieMacxFixThread::try_restartrouter(const QString &nic)
{
    emit show_restartrouter_dlg();
    wait_synmethodcall();

    if(!m_shrestartrouter_dlg_result)
    {
        return;
    }

    m_shrestartrouter_dlg_result = false;

    emit_realtime_msg2(L_INTERNET_RT_RESTARTROUTER_PROMPT_RESTARTINGROUTER);

    //reboot the router
    emit reboot_router();

    //wait for the router reboot
    QThread::msleep(70000 + 20000);//70 seconds + 20 seconds


    bool bwireless = false;
    assert ( inet_api->IsNicWireless(nic,&bwireless) == 0 );

    if(!bwireless || wifi_api->IsWLanNicConnected(nic))
    {
        if(1 == verify_gateway_dns(nic))
        {
            m_bis_repairok = true;
            emit_realtime_msg2(L_INTERNET_RT_REPAIR_NIC_OK,nic);
        }
    }

    emit end_show_restartrouter_dlg();
}
//end
