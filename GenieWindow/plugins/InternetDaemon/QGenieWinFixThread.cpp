#include "QGenieWinFixThread.h"
#include "language_Internet.h"
#include <QDesktopServices>
#include <QUrl>
#include <assert.h>
#include <QtDebug>

extern DiagnoseInterface *diagnose_inf;

//long time operation interrupt exception code
#define LONGTIME_OPERATION_INTERRUPT 2011
#define ROUTER_CONFIGURATION_PAGE   "http://routerlogin.net"

QGenieWinFixThread::QGenieWinFixThread():
    m_fix_status(ERROR_STATUS),
    m_synmethodcall_mutex(0),
    m_synmethodcall_condition(0),
    m_bmessagebox_ret(false),
    m_shplugincable_dlg_result(0),
    m_shwlanoff_dlg_result(0),
    m_ifix_canceled(0),
    m_cancel_fix_process_code(0),
    m_shrestartrouter_dlg_result(false)
{
}


QGenieWinFixThread::~QGenieWinFixThread()
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

//---------------------interruptable low level api wrapper--------------
int  QGenieWinFixThread::IsNicGatewaysOk(const QString &nic_guid)
{
    assert (diagnose_inf);

    //interrupt ?
    AutoInterrupt(this);

    diagnose_inf->enable_longtime_operation_interruptable(&m_ifix_canceled);
    int ret = diagnose_inf->IsNicGatewaysOk(nic_guid,true);
    diagnose_inf->disble_longtime_operation_interruptable();

    return ret;
}

bool QGenieWinFixThread::EnableDhcp(const QString &nic_guid)
{
    assert (diagnose_inf);

    //interrupt ?
    AutoInterrupt(this);

    diagnose_inf->enable_longtime_operation_interruptable(&m_ifix_canceled);
    bool ret = diagnose_inf->EnableDhcp(nic_guid,true);

    fixthread_msleep_interruptable(10000);
    ret = (ret && diagnose_inf->EnableDnsFromDhcp(nic_guid,true));
    diagnose_inf->disble_longtime_operation_interruptable();

    return ret;
}

bool QGenieWinFixThread::EnableDnsFromDhcp(const QString &nic_guid)
{
    assert (diagnose_inf);

    //interrupt ?
    AutoInterrupt(this);

    diagnose_inf->enable_longtime_operation_interruptable(&m_ifix_canceled);
    bool ret = diagnose_inf->EnableDnsFromDhcp(nic_guid,true);
    diagnose_inf->disble_longtime_operation_interruptable();

    return ret;
}

bool QGenieWinFixThread::SetInterfaceDnses(const QString &nic_guid,const QStringList &dnses)
{
    assert (diagnose_inf);

    //interrupt ?
    AutoInterrupt(this);

    diagnose_inf->enable_longtime_operation_interruptable(&m_ifix_canceled);
    bool ret = diagnose_inf->SetInterfaceDnses(nic_guid,dnses,true);
    diagnose_inf->disble_longtime_operation_interruptable();

    return ret;
}

int QGenieWinFixThread::RepairGatewayorDnsOk(const QString &nic_guid,bool gatewayordns)
{
    assert (diagnose_inf);

    //interrupt ?
    AutoInterrupt(this);

    diagnose_inf->enable_longtime_operation_interruptable(&m_ifix_canceled);
    int ret = diagnose_inf->RepairGatewayorDnsOk(nic_guid,gatewayordns,true);
    diagnose_inf->disble_longtime_operation_interruptable();

    return ret;
}

bool QGenieWinFixThread::ConfigDNSManually(const QString &nic_guid)
{
    assert (diagnose_inf);

    //interrupt ?
    AutoInterrupt(this);

    diagnose_inf->enable_longtime_operation_interruptable(&m_ifix_canceled);
    bool ret = diagnose_inf->ConfigDNSManually(nic_guid,true);
    diagnose_inf->disble_longtime_operation_interruptable();

    return ret;
}

bool QGenieWinFixThread::ReConnectWlan(const QString &nic_guid)
{
    assert (diagnose_inf);

    //interrupt ?
    AutoInterrupt(this);

    diagnose_inf->enable_longtime_operation_interruptable(&m_ifix_canceled);
    bool ret = diagnose_inf->ReConnectWlan(nic_guid,true);
    diagnose_inf->disble_longtime_operation_interruptable();

    return ret;
}

bool QGenieWinFixThread::ConnectToWlan(const QString &nic_guid,const QString &ssid,const QString &pwd)
{
    assert (diagnose_inf);

    //interrupt ?
    AutoInterrupt(this);

    diagnose_inf->enable_longtime_operation_interruptable(&m_ifix_canceled);
    bool ret = diagnose_inf->ConnectToWlan(nic_guid,ssid,pwd,true);
    diagnose_inf->disble_longtime_operation_interruptable();

    return ret;
}

bool QGenieWinFixThread::ReConnectCurWlan(const QString &nic_guid,const QString &new_pwd)
{
    assert (diagnose_inf);

    //interrupt ?
    AutoInterrupt(this);

    diagnose_inf->enable_longtime_operation_interruptable(&m_ifix_canceled);
    bool ret = diagnose_inf->ReConnectCurWlan(nic_guid,new_pwd,true);
    diagnose_inf->disble_longtime_operation_interruptable();

    return ret;
}

bool QGenieWinFixThread::GetVisibleWlanNetworks(const QString &nic_guid,QList<VisibleNetwork> &vn_list)
{
    assert (diagnose_inf);

    //interrupt ?
    AutoInterrupt(this);

    diagnose_inf->enable_longtime_operation_interruptable(&m_ifix_canceled);
    bool ret = diagnose_inf->GetVisibleWlanNetworks(nic_guid,vn_list,true);
    diagnose_inf->disble_longtime_operation_interruptable();

    return ret;
}

bool QGenieWinFixThread::GetVisibleWlanNetworks2(const QString &nic_guid,QList<VisibleNetwork> &vn_list,bool brescan)
{
    assert (diagnose_inf);

    //interrupt ?
    AutoInterrupt(this);

    diagnose_inf->enable_longtime_operation_interruptable(&m_ifix_canceled);
    bool ret = diagnose_inf->GetVisibleWlanNetworks2(nic_guid,vn_list,brescan,true);
    diagnose_inf->disble_longtime_operation_interruptable();

    return ret;
}
//---------------------------------end----------------------------------

void QGenieWinFixThread::run()
{
    if(0 == diagnose_inf)
    {
        qDebug() << "Win fix low level api not available!" ;
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

        //m_cancel_fix_process_code != 0 indicate user canceled this process
        if((m_fix_status != FIX_OK) && m_cancel_fix_process_code != 0)
        {
            restore_nicsstate();
        }

        emit repair_completed(2011);//2011 fix canceled
    }

}

bool QGenieWinFixThread::enable_nic(const NCard &niccard)
{
    //interrupt ?
    AutoInterrupt(this);

    bool bret      = false;
    int  interval  = 30;

    int trytimes = 3;
    while(1){
        bret = diagnose_inf->NetCardStateChange(niccard.m_devguid,true);

        --trytimes;

        if(bret || trytimes <= 0){
            break;
        }else{
            fixthread_msleep_interruptable(3000);
        }
    }

    if(!bret)
    {
        goto fun_end;
    }

    do
    {
        if(diagnose_inf->IsNicDisabled(niccard.m_devguid) == 0)
        {
            break;
        }
        fixthread_msleep_interruptable(1000);

    }while(--interval);

    if (diagnose_inf->IsNicDisabled(niccard.m_devguid) != 0)
    {
        bret = false;
    }
    else
    {
        //wait for nic enable complete
        //fixthread_msleep_interruptable(30000);
        bret = true;
    }

fun_end:

    if(bret)
    {
        emit_realtime_msg2(L_INTERNET_RT_ENNIC_OK,niccard.m_devdesc);
        emit_pcrouter_link_unknown();
    }
    else
    {
        emit_realtime_msg2(L_INTERNET_RT_ENNIC_FAILED,niccard.m_devdesc);
        emit_pcrouter_link_failed();
    }

    return bret;
}

//return code:1--->connected ;0-->not connected;-1-->not connected user canceled

int QGenieWinFixThread::connect_wlan(const QString &strnicguid)
{
    //interrupt ?
    AutoInterrupt(this);

    QString str_ssid;
    QString str_pwd;
    int	iret = 0;

    QList<VisibleNetwork> temp_vnlist;
    if(!GetVisibleWlanNetworks(strnicguid,temp_vnlist) || 0 == temp_vnlist.size())
    {
        return iret;
    }

    emit show_visiblenetworklist_dlg(strnicguid,&str_ssid,&str_pwd);
    //wait return
    wait_synmethodcall();

    str_ssid = this->m_shvndlg_retssid;
    str_pwd  = this->m_shvndlg_retpwd;
    //return

    if (strnicguid.isEmpty() == false && str_ssid.isEmpty() == false)
    {
        emit_begin_connect();
        emit_realtime_msg2(L_INTERNET_RT_TRYCONNECT_WLAN,str_ssid);
        if (ConnectToWlan(strnicguid,str_ssid,str_pwd))
        {
            iret = 1;
        }
        emit_end_connect();
    }
    else
    {
        iret = -1;
    }

    return iret;
}


bool QGenieWinFixThread::test_repair_wirelessradio(const NCard &niccard)
{
    //interrupt ?
    AutoInterrupt(this);

    diagnose_inf->SwitchWlanNic(niccard.m_devguid,true);
    fixthread_msleep_interruptable(2000);

    emit_pcrouter_link_unknown();
    emit_realtime_msg2(L_INTERNET_RT_TESTING_WIRELESSRADIO);

    bool   bret             = true;
    int    iWirelessState   = -1;
    int    trytimes         = 3;

    do
    {
        iWirelessState = diagnose_inf->IsWlanNicSwitchedOff(niccard.m_devguid);

        if(iWirelessState != -1)
        {
            break;
        }
        else
        {
            fixthread_msleep_interruptable(1000);
        }

    }while(--trytimes);

    if (iWirelessState == 2)//software off
    {
        emit_pcrouter_link_failed();
        emit_realtime_msg2(L_INTERNET_RT_WIRELESSRADIO_SOFTOFF);

        if (diagnose_inf->SwitchWlanNic(niccard.m_devguid,true))
        {
            emit_pcrouter_link_unknown();
            emit_realtime_msg2(L_INTERNET_RT_SOFT_SWITCHON_RADIO_OK);
        }
        else
        {
            bret = false;
            emit_realtime_msg2(L_INTERNET_RT_SOFT_SWITCHON_RADIO_FAILED);
        }
    }
    else if (iWirelessState == 1)//hardware off
    {
        emit_realtime_msg2(L_INTERNET_RT_WIRELESSRADIO_HARDOFF);
        emit_pcrouter_link_failed();

        int	    iMsgRet                = 0;
        bool        bradioHardwareoff      = true;

        do
        {
            emit show_wlanradiohwoff_dlg(&iMsgRet);
            //wait return
            wait_synmethodcall();

            iMsgRet = m_shwlanoff_dlg_result;

            if (1 == iMsgRet)
            {
                bradioHardwareoff = (1 == diagnose_inf->IsWlanNicSwitchedOff(niccard.m_devguid));
            }
            else
            {
                break;
            }

        }while (bradioHardwareoff);

        if(bradioHardwareoff)
        {
            bret = false;
            emit_realtime_msg2(L_INTERNET_RT_WIRELESSRADIO_STILL_HARDOFF);
        }
        else
        {
            emit_pcrouter_link_unknown();
            emit_realtime_msg2(L_INTERNET_RT_HARD_SWITCHON_RADIO_OK);
        }
        //------
    }
    else if (iWirelessState == -1)
    {
        emit_realtime_msg2(L_INTERNET_RT_WIRELESSRADIO_STATE_UNKNOWN);
        //also try switch on
        diagnose_inf->SwitchWlanNic(niccard.m_devguid,true);
    }
    else
    {
        emit_realtime_msg2(L_INTERNET_RT_WIRELESSRADIO_IS_ON);
    }

    return bret;
}

//return code:-1--->not connected ;-2-->connected and dhcp not enabled but gateway ,dns invalid
//-3 user canceled
//0-->connected and dhcp enabled but gateway ,dns invalid
//1-->connected gateway and dns are valid
//2-->connected gateway reachable but can't go outside

int QGenieWinFixThread::reconnect_wlan(const NCard &niccard)
{
    //interrupt ?
    AutoInterrupt(this);

    int    ires = -1;

    emit_begin_connect();
    emit_realtime_msg2(L_INTERNET_RT_TRYCONNECT_WLAN_WITHPROFILE);

    if (ReConnectWlan(niccard.m_devguid))
    {
        if(1 == diagnose_inf->IsDhcpEnabled(niccard.m_devguid))
        {
            ires = 0;
            emit_realtime_msg2(L_INTERNET_RT_DHCPEN_WAITTING);
            waitdhcp_forgateway(niccard.m_devguid);
        }
        else
        {
            ires = -2;
        }

        emit_end_connect();
        test_gateway_dns(niccard);
        if(NEED_ROUTERCFG_WEBPAGE == m_fix_status)
        {
            ires = 2;
            return ires;
        }
        else if (FIX_OK == m_fix_status)
        {
            ires = 1;
        }
        //----------added later---------
        else
        {

            if(!process_gateway_dns(niccard))
            {
                ires = (m_fix_status == FIX_OK?1:2);
                return ires;
            }

        }
        //----------------------------
    }
    else
    {
        ires = -1;
        emit_realtime_msg2(L_INTERNET_RT_RECONNECT_FAILED);
        emit_end_connect();
    }

    if(ires != 1)
    {
        emit_realtime_msg2(L_INTERNET_RT_TRYCONNECT_VISIBLE_NETWORK);
        int temp_ret = connect_wlan(niccard.m_devguid);
        if (1 == temp_ret)
        {
            if(1 == diagnose_inf->IsDhcpEnabled(niccard.m_devguid))
            {
                ires = 0;
                emit_begin_connect();
                emit_realtime_msg2(L_INTERNET_RT_DHCPEN_WAITTING);
                waitdhcp_forgateway(niccard.m_devguid);
                emit_end_connect();
            }
            else
            {
                ires = -2;
            }

            test_gateway_dns(niccard);
            if(NEED_ROUTERCFG_WEBPAGE == m_fix_status)
            {
                ires = 2;
                return ires;
            }

            if (FIX_OK == m_fix_status)
            {
                ires = 1;
            }
            //----------added later---------
            else
            {

                if(!process_gateway_dns(niccard))
                {
                    ires = (m_fix_status == FIX_OK?1:2);
                    return ires;
                }

            }
            //----------------------------
        }
        else
        {
            ires =(( -1 == temp_ret)?-3:-1);
            emit_realtime_msg2(L_INTERNET_RT_CONNECT_VN_FAILED);
        }
    }

    return ires;
}


void QGenieWinFixThread::test_gateway_dns(const NCard &niccard)
{
    //interrupt ?
    AutoInterrupt(this);

    int iGatewayOk = -1,iDnsOk = -1;

    emit_realtime_msg2(L_INTERNET_RT_TESTING_GATEWAY);
    emit_begin_pcrouterlinkflash();

    iGatewayOk = RepairGatewayorDnsOk(niccard.m_devguid,true);

    emit_end_pcrouterlinkflash();

    switch(iGatewayOk)
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
        emit_realtime_msg2(L_INTERNET_RT_GATEWAY_OUTSIDEPORT_INVALID);
        break;
    default:
        emit_pcrouter_link_unknown();
        emit_realtime_msg2(L_INTERNET_RT_GATEWAY_STATE_UNKNOWN);
    }

    if (1 == iGatewayOk)
    {
        emit_realtime_msg2(L_INTERNET_RT_TESTING_DNS);
        emit_begin_routerinternetlinkflash();
        iDnsOk = RepairGatewayorDnsOk(niccard.m_devguid,false);
        emit_end_routerinternetlinkflash();

        switch(iDnsOk)
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
    }
    else
    {
        emit_realtime_msg2(L_INTERNET_RT_DNS_TEST_CANNOTDO);
        if(iGatewayOk != 2)
        {
            emit_routerinternet_link_unknown();
        }

    }

    if((1 == iGatewayOk) && (1 == iDnsOk))
    {
        emit_realtime_msg2(L_INTERNET_RT_REPAIR_NIC_OK,niccard.m_devdesc);
        m_fix_status = FIX_OK;
    }
    else if(/*((1 == iGatewayOk) && (iDnsOk != 1)) || */(2 == iGatewayOk))
    {
        emit_realtime_msg2(L_INTERNET_RT_ROUTER_WLANCABLE_MAY_PLUGINOUT);
        m_fix_status = NEED_ROUTERCFG_WEBPAGE;
    }
    else
    {
        //this case :m_fix_status == ERROR_STATUS
    }
}


bool QGenieWinFixThread::plugin_cable(const NCard &niccard)
{
    //interrupt ?
    AutoInterrupt(this);

    int	   iMsgRet      = 0;
    bool   bPluggedin	= false;

    do
    {
        //interrupt ?
        AutoInterrupt(this);

        emit show_plugincable_dlg(&iMsgRet);
        //wait return
        wait_synmethodcall();

        iMsgRet = m_shplugincable_dlg_result;

        if (1 == iMsgRet)
        {
            bPluggedin = diagnose_inf->IsNicCablePluggedin(niccard.m_devguid);
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

int QGenieWinFixThread::repair_gateway(const NCard &niccard)
{
    //interrupt ?
    AutoInterrupt(this);

    int iGatewayOk = -1;

    emit_begin_pcrouterlinkflash();

    if (diagnose_inf->IsDhcpEnabled(niccard.m_devguid) != 1)
    {
        emit_realtime_msg2(L_INTERNET_RT_DHCPDIS_TRY_ENABLEIT);
        if (EnableDhcp(niccard.m_devguid))
        {
            emit_realtime_msg2(L_INTERNET_RT_ENDHCP_OK);
            waitdhcp_forgateway(niccard.m_devguid);

            emit_begin_pcrouterlinkflash();
            iGatewayOk = RepairGatewayorDnsOk(niccard.m_devguid,true);
            emit_end_pcrouterlinkflash();

            if (1 == iGatewayOk)
            {
                emit_realtime_msg2(L_INTERNET_RT_GETVALID_GATEWAY_DHCP);
                emit_pcrouter_link_ok();
            }
            else if (2 == iGatewayOk)
            {
                emit_realtime_msg2(L_INTERNET_RT_GETOUTSIDE_INVALID_GATEWAY_DHCP);
                emit_pcrouter_link_ok();
                emit_routerinternet_link_failed();
            }
            else
            {
                emit_realtime_msg2(L_INTERNET_RT_FAILED_GETVALIDGATEWAY_DHCP);
                emit_pcrouter_link_failed();
            }
        }
        else
        {
            emit_realtime_msg2(L_INTERNET_RT_ENABLEDHCP_FAILED);
            emit_pcrouter_link_failed();
        }
    }//if
    else //Dhcp is Enabled
    {
        emit_realtime_msg2(L_INTERNET_RT_DHCPISEN_NODHCPSERVER);
        emit_pcrouter_link_failed();
    }

    emit_end_pcrouterlinkflash();

    return iGatewayOk;
}

bool QGenieWinFixThread::repair_dns(const NCard &niccard)
{
    //interrupt ?
    AutoInterrupt(this);

    int iDNSOk = -1;
    emit_begin_routerinternetlinkflash();

    emit_realtime_msg2(L_INTERNET_RT_TRY_GETDNS_WITHDHCP);
    if (EnableDnsFromDhcp(niccard.m_devguid))
    {

        emit_realtime_msg2(L_INTERNET_RT_ENABLE_DNS_FROMDHCP_OK);
        emit_realtime_msg2(L_INTERNET_RT_TRYGETDNS_FROMDHCP);
        waitdhcp_fordns(niccard.m_devguid);

        iDNSOk = RepairGatewayorDnsOk(niccard.m_devguid,false);
        if (1 == iDNSOk)
        {
            emit_realtime_msg2(L_INTERNET_RT_GETVALIDDNS_FROMDHCP);
            emit_routerinternet_link_ok();
        }
        else
        {
            emit_realtime_msg2(L_INTERNET_RT_GETDNS_FROMDHCP_FAILED);
            emit_routerinternet_link_failed();
        }
    }
    else
    {
        emit_realtime_msg2(L_INTERNET_RT_ENABLE_DNS_FROMDHCP_FAILED);
        emit_routerinternet_link_failed();
    }

#ifdef ENABLE_CONFIGDNS_MANUALLY
    if(iDNSOk != 1)
    {
        emit_realtime_msg2(L_INTERNET_RT_TRY_SETDEFAULT_DNS);
        if (ConfigDNSManually(niccard.m_devguid))
        {
            iDNSOk = 1;
            emit_realtime_msg2(L_INTERNET_RT_SETDEFAULT_DNS_OK);
            emit_routerinternet_link_ok();
        }
        else
        {
            emit_realtime_msg2(L_INTERNET_RT_SETDEFAULT_DNS_FAILED);
            emit_routerinternet_link_failed();
        }
    }
#endif

    emit_end_internetflash();

    return (1 == iDNSOk);
}

bool QGenieWinFixThread::enum_interfaces()
{
    //interrupt ?
    AutoInterrupt(this);

    emit_begin_pcflash();

    nic_list.clear();

    if (!diagnose_inf->EnumInterfaces(nic_list) || (nic_list.size() == 0))
    {
        emit_realtime_msg2(L_INTERNET_RT_GETNICS_FAILED);
    }

    //sort the interfaces list ,wireless first
    //only contain wireless and wired interface
    for(int k = nic_list.size()-1;k >= 0 ;--k)
    {
        if(2 == nic_list[k].m_nictype)
        {
            if( k != 0 )//wireless move first
            {
                nic_list.prepend(nic_list.takeAt(k));
            }
        }
        else if(nic_list[k].m_nictype != 1)
        {
            nic_list.removeAt(k);
        }
    }

    emit_end_pcflash();
    return (nic_list.size() > 0);
}

bool QGenieWinFixThread::verify_link(const NCard &nic,bool &bautoen)
{
    //interrupt ?
    AutoInterrupt(this);

    bool bret = true;

    emit_begin_pcflash();

    if(diagnose_inf->IsNicDisabled(nic.m_devguid) != 1)
    {
        diagnose_inf->NetCardStateChange(nic.m_devguid,false);
        //waitting for internface disabled
        int disable_wait_times = 10;
        do
        {
            if(1 == diagnose_inf->IsNicDisabled(nic.m_devguid))
            {
                break;
            }
            fixthread_msleep_interruptable(1000);
        }while(--disable_wait_times);
    }

    if (1 == diagnose_inf->IsNicDisabled(nic.m_devguid))
    {
        emit_realtime_msg2(L_INTERNET_RT_NICDIS_TRYEN_WAIT);
        if(enable_nic(nic))
        {
            bautoen = true;
        }
        else
        {
            emit_realtime_msg2(L_INTERNET_RT_REPAIR_NIC_FAILED,nic.m_devdesc);
            bret = false;
        }
    }

    diagnose_inf->IpRenewInterface(nic.m_devguid);

    emit_end_pcflash();

    return bret;
}

bool QGenieWinFixThread::verify_cablein(const NCard &nic,bool &bplugin_manually)
{
    //interrupt ?
    AutoInterrupt(this);

    bool iscable_in     = false;
    int  times          = 5;

    do
    {
        iscable_in = diagnose_inf->IsNicCablePluggedin(nic.m_devguid);

        if(iscable_in)
        {
            break;
        }

        fixthread_msleep_interruptable(1000);

    }while((!iscable_in) && (--times));

    if (!iscable_in)
    {
        emit_realtime_msg2(L_INTERNET_RT_CABLE_NOPLUGIN_PLUGINPROMPT);
        emit_pcrouter_link_failed();

        if(plugin_cable(nic))
        {
            bplugin_manually = true;//plugin manually now
            iscable_in = true;
        }
        else
        {
            emit_realtime_msg2(L_INTERNET_RT_REPAIR_NIC_FAILED,nic.m_devdesc);
        }
    }

    return iscable_in;
}

bool QGenieWinFixThread::test_after_enable_or_plugincale(const NCard &nic)
{
    //interrupt ?
    AutoInterrupt(this);

    if (1 == diagnose_inf->IsDhcpEnabled(nic.m_devguid))//µÈ´ýDHCP»ñÈ¡IP
    {
        emit_realtime_msg2(L_INTERNET_RT_DHCPEN_WAITTING);
        waitdhcp_forgateway(nic.m_devguid);
    }

    test_gateway_dns(nic);

    return (ERROR_STATUS == m_fix_status);
}

//return:true -> this nic need more process ;false -> otherwise
//nic:network card object
//bauto_enabled:indecate nic being auto enabled ,ture->yes,false->no
//need_repeat:out parameter ,true repeat fix this nic from scratch,when evaluted to true the
//return value must be false
bool QGenieWinFixThread::process_wired_nic(const NCard &nic,bool bauto_enabled,bool &need_repeat)
{
    //interrupt ?
    AutoInterrupt(this);

    emit_begin_pcflash();

    bool need_moreprocess   = true;
    bool bcable_plugin      = false;

    need_repeat             = false;

    if(!verify_cablein(nic,bcable_plugin))
    {
        if(this->messagebox(L_INTERNET_RT_RECUR_TRY_REPAIR_MSGBOX_PROMPT
                            ,L_INTERNET_RT_RECUR_TRY_REPAIR_MSGBOX_OTHER_CASE,true))
        {
            need_repeat = true;//repeat current adpter repairing
        }

        need_moreprocess = false;
        goto this_fun_end;


    }

    if(bauto_enabled || bcable_plugin)
    {
        if(!test_after_enable_or_plugincale(nic))
        {
            need_moreprocess = false;
        }
    }

this_fun_end:
    emit_end_pcflash();
    return need_moreprocess;
}

//return:true -> this nic need more process ;false -> otherwise
//nic:network card object
//bauto_enabled:indecate nic being auto enabled ,ture->yes,false->no
//need_repeat:out parameter ,true repeat fix this nic from scratch,when evaluted to true the
//return value must be false
bool QGenieWinFixThread::process_wireless_nic(const NCard &nic,bool bauto_enabled,bool bhasmore_nic,bool &need_repeat)
{
    //interrupt ?
    AutoInterrupt(this);

    bool need_moreprocess   = true;

    emit_begin_pcflash();

    need_repeat = false;

    if(!test_repair_wirelessradio(nic))
    {
        emit_realtime_msg2(L_INTERNET_RT_REPAIR_NIC_FAILED,nic.m_devdesc);
        if(this->messagebox(L_INTERNET_RT_RECUR_TRY_REPAIR_MSGBOX_PROMPT
                            ,L_INTERNET_RT_RECUR_TRY_REPAIR_MSGBOX_OTHER_CASE,true))
        {
            need_repeat = true;//repeat current adpter repairing
        }

        need_moreprocess = false;
        goto this_fun_end;
    }

    if(bauto_enabled)
    {
        int times = 30;
        do
        {
            if(diagnose_inf->IsWlanNicConnected(nic.m_devguid) == 1)
            {
                break;
            }
            fixthread_msleep_interruptable(1000);
        }while(--times);

        if(diagnose_inf->IsWlanNicConnected(nic.m_devguid) == 1)
        {
            if(!test_after_enable_or_plugincale(nic))
            {
                need_moreprocess = false;
                goto this_fun_end;
            }
        }
    }

    if (diagnose_inf->IsWlanNicConnected(nic.m_devguid) != 1)
    {
        emit_realtime_msg2(L_INTERNET_RT_NIC_NOT_CONNECTED);
        int iresult = reconnect_wlan(nic);
        if((1 == iresult) || (2 == iresult))
        {
            need_moreprocess    = false;
            goto this_fun_end;
        }
        //not connected or connected but gateway or dns is invalid with dhcp enabled,user canceled
        else if(-1 == iresult || 0 == iresult || -3 == iresult)
        {
            recur_tryrepair_wireless(nic,iresult,bhasmore_nic);
            need_moreprocess    = false;
            goto this_fun_end;
        }
        else if(-2 == iresult)//dhcp is not enabled
        {
            //connected but gateway or dns is invalid  goto next step:repair gateway and dns
        }
    }

this_fun_end:
    emit_end_pcflash();
    return need_moreprocess;
}

//return:true -> this nic need more process ;false -> otherwise
bool QGenieWinFixThread::process_gateway_dns(const NCard &nic)
{
    //interrupt ?
    AutoInterrupt(this);

    emit_begin_routerflash();

    bool need_moreprocess   = true;
    bool bgateway_ok        = false;
    bool bdns_ok            = false;

    int testgateway_ret = RepairGatewayorDnsOk(nic.m_devguid,true);

    if(testgateway_ret != 1)
    {
        if(2 == testgateway_ret)
        {
            emit_realtime_msg2(L_INTERNET_RT_ROUTER_WLANCABLE_MAY_PLUGINOUT);
            emit_pcrouter_link_ok();
            emit_routerinternet_link_failed();
            m_fix_status        = NEED_ROUTERCFG_WEBPAGE;
            need_moreprocess    = false;
            goto this_fun_end;
        }
        else
        {
            emit_pcrouter_link_failed();
            emit_routerinternet_link_unknown();
        }

        emit_realtime_msg2(L_INTERNET_RT_NICGATEWAY_INVALID_TRYFIXIT);
        int temp_ret = repair_gateway(nic);
        if(1 == temp_ret)
        {
            bgateway_ok = true;
        }
        else if(2 == temp_ret)
        {
            emit_realtime_msg2(L_INTERNET_RT_ROUTER_WLANCABLE_MAY_PLUGINOUT);
            m_fix_status        = NEED_ROUTERCFG_WEBPAGE;
            need_moreprocess    = false;
            goto this_fun_end;
        }
    }
    else
    {
        bgateway_ok = true;
        emit_pcrouter_link_ok();
    }

    emit_end_routerflash();
    emit_begin_internetflash();

    if(RepairGatewayorDnsOk(nic.m_devguid,false) != 1)
    {
        emit_routerinternet_link_failed();
        emit_realtime_msg2(L_INTERNET_RT_THEDNS_ISINVALID);
        if(bgateway_ok)
        {
            emit_realtime_msg2(L_INTERNET_RT_TRYTOFIX_DNS);
            if(repair_dns(nic))
            {
                bdns_ok = true;
                emit_routerinternet_link_ok();
            }
        }
        else
        {
            emit_realtime_msg2(L_INTERNET_RT_CANNOTFIXDNS_GATEWAYISINVALID);
        }
    }
    else
    {
        bdns_ok = true;
        emit_routerinternet_link_ok();
    }

    if(bgateway_ok && bdns_ok)
    {
        emit_realtime_msg2(L_INTERNET_RT_REPAIR_NIC_OK,nic.m_devdesc);
        m_fix_status = FIX_OK;
        need_moreprocess = false;
    }
    else if(bgateway_ok && (!bdns_ok))//for netgear router reset to factory default configuration,dns hajackt
    {
        emit_realtime_msg2(L_INTERNET_RT_ROUTER_WLANCABLE_MAY_PLUGINOUT);
        m_fix_status        = NEED_ROUTERCFG_WEBPAGE;
        need_moreprocess    = false;
    }

this_fun_end:
    emit_end_routerflash();
    emit_end_internetflash();
    return need_moreprocess;
}

void QGenieWinFixThread::process_end_fix(const NCard &nic,bool bhas_morenic)
{
    //interrupt ?
    AutoInterrupt(this);

    emit_begin_pcflash();

    if(2 == nic.m_nictype)//wireless
    {
        recur_tryrepair_wireless(nic,-3,bhas_morenic);
    }
    else
    {
        if(1 == diagnose_inf->IsDhcpEnabled(nic.m_devguid))
        {
            //try renew
            iprenew_interface(nic);
            if( ERROR_STATUS == m_fix_status )
            {
                emit_realtime_msg2(L_INTERNET_RT_REPAIR_NIC_FAILED,nic.m_devdesc);
            }
        }
    }

    emit_end_pcflash();
}

void QGenieWinFixThread::fixit()
{
    int i = 0;

    m_fix_status    = ERROR_STATUS;
    m_ifix_canceled = 0;

    emit_realtime_msg2(L_INTERNET_RT_BRIEFPROMPT_RUNNINNG_INTERNET_FIX);
    emit_realtime_msg2(L_INTERNET_RT_TRYTO_REPAIRNETWORK);

    if(!enum_interfaces())
    {
        goto fix_all_end;
    }

    for (i = 0;i < nic_list.size();++i)
    {
        bool bhas_auto_enabled  = false;
        bool bneed_moreprocess  = false;
        bool bneed_repeat       = false;

        m_fix_status = ERROR_STATUS;

        emit_realtime_msg2(L_INTERNET_RT_TRYREPAIR_NIC_WAIT,nic_list[i].m_devdesc);

        if(!verify_link(nic_list[i],bhas_auto_enabled))
        {
            emit_begin_pcflash();
            if(this->messagebox(L_INTERNET_RT_RECUR_TRY_REPAIR_MSGBOX_PROMPT
                                ,L_INTERNET_RT_RECUR_TRY_REPAIR_MSGBOX_OTHER_CASE,true))
            {
                --i;//repeat current adpter repairing
            }
            emit_end_pcflash();
            continue;
        }

        if (nic_list[i].m_nictype == 2)//wireless
        {
            bneed_moreprocess = process_wireless_nic(nic_list[i],bhas_auto_enabled,(i+1)<nic_list.size(),bneed_repeat);
        }//if
        else  if(nic_list[i].m_nictype == 1 || nic_list[i].m_nictype == 5)//wired
        {
            bneed_moreprocess = process_wired_nic(nic_list[i],bhas_auto_enabled,bneed_repeat);
        }

        if(!bneed_moreprocess)
        {
            if(bneed_repeat)
            {
                --i;
                continue;
            }
            else
            {
                goto single_nic_fix_end;
            }
        }

        if(!process_gateway_dns(nic_list[i]))
        {
            goto single_nic_fix_end;
        }

        process_end_fix(nic_list[i],(i+1)<nic_list.size());

single_nic_fix_end:
        if(FIX_OK == m_fix_status)
        {
            break;
        }
        else if((NEED_ROUTERCFG_WEBPAGE == m_fix_status) || (ERROR_STATUS == m_fix_status))
        {
            emit_begin_routerflash();
            int gateway_status = RepairGatewayorDnsOk(nic_list[i].m_devguid,true);
            emit_end_routerflash();

            //gate way status can be reboot router?
            if((1 == gateway_status) || (2 == gateway_status))
            {
                //restart the router
                try_restartrouter(nic_list[i]);
                //end
            }

            if(NEED_ROUTERCFG_WEBPAGE == m_fix_status)
                break;
        }
        else
        {
            qDebug() << "Unknown fix status code :" << m_fix_status;
            assert(0);
        }
    }//for

fix_all_end:
    int fixstate = 0;
    if(m_fix_status == FIX_OK)
    {
        fixstate = 1;
        emit_realtime_msg2(L_INTERNET_RT_BRIEFPROMPT_FIX_OK);
    }
    else if(m_fix_status == NEED_ROUTERCFG_WEBPAGE)
    {
        fixstate = 2;
        emit_realtime_msg2(L_INTERNET_RT_OPENCONFIGURATION_GUI);
        QDesktopServices::openUrl(QUrl(QString(ROUTER_CONFIGURATION_PAGE)));
    }
    else if(ERROR_STATUS == m_fix_status)
    {
        fixstate = 0;
        emit_realtime_msg2(L_INTERNET_RT_BRIEFPROMPT_FIX_FAILED);
    }
    else
    {
        qDebug() << "fix end Unknown fix status code :" << m_fix_status;
        assert(0);
    }

    emit repair_completed(fixstate);

}

void QGenieWinFixThread::iprenew_interface(const NCard &niccard)
{
    //interrupt ?
    AutoInterrupt(this);

    emit_realtime_msg2(L_INTERNET_RT_DHCPENABLE_TRY_RENEW_IP);
    if(diagnose_inf->IpRenewInterface(niccard.m_devguid))
    {
        emit_realtime_msg2(L_INTERNET_RT_RENEW_IP_OK);
        emit_realtime_msg2(L_INTERNET_RT_WAITGETIP_AFTER_IPRENEW);
        waitdhcp_forgateway(niccard.m_devguid);
        test_gateway_dns(niccard);
    }
    else
    {
        emit_realtime_msg2(L_INTERNET_RT_RENEW_IP_FAILED);
    }
}


void QGenieWinFixThread::recur_tryrepair_wireless(const NCard &niccard,int ihint,bool bmoreadapter)
{
    //interrupt ?
    AutoInterrupt(this);

    do
    {
        //interrupt ?
        AutoInterrupt(this);

        int prompt_text_id = 0;

        int   iWirelessState =  diagnose_inf->IsWlanNicSwitchedOff(niccard.m_devguid);
        if(1 == iWirelessState || 2 == iWirelessState)//wireless radio off
        {
            ihint = -4;
        }
        else if(0 == iWirelessState)//wireless radion on
        {
            if(-4 == ihint || -1 == ihint)
            {
                ihint = 0;
            }
        }
        else if(-4 == ihint)
        {
            ihint = -1;
        }

        switch(ihint)
        {
        case 0:
            prompt_text_id = L_INTERNET_RT_RECUR_TRY_REPAIR_MSGBOX_WIRELESSNETWORK_KEY_MAYWRONG;
            break;
        case -1:
            prompt_text_id = L_INTERNET_RT_RECUR_TRY_REPAIR_MSGBOX_WIRELESSRIDIO_MAYOFF;
            break;
        case -3:
            prompt_text_id = L_INTERNET_RT_RECUR_TRY_REPAIR_MSGBOX_OTHER_CASE;
            break;
        case -4:
            prompt_text_id = L_INTERNET_RT_RECUR_TRY_REPAIR_MSGBOX_WIRELESSRIDIO_ISOFF;
            break;
        default:
            prompt_text_id = L_INTERNET_RT_RECUR_TRY_REPAIR_MSGBOX_CHECK_WRADIO_NETWORKKEY_AND_OTHER;
        }



        if(this->messagebox(L_INTERNET_RT_RECUR_TRY_REPAIR_MSGBOX_PROMPT
                            ,prompt_text_id,true))
        {
            if( 2 == iWirelessState )
            {
                if(diagnose_inf->SwitchWlanNic(niccard.m_devguid,true))
                {
                    fixthread_msleep_interruptable(3000);
                }
            }

            if(0 == ihint)
            {
                if(1 == diagnose_inf->IsDhcpEnabled(niccard.m_devguid))
                {
                    //try renew
                    iprenew_interface(niccard);
                    if(FIX_OK == m_fix_status || NEED_ROUTERCFG_WEBPAGE == m_fix_status)
                    {
                        break;
                    }
                }

                emit_realtime_msg2(L_INTERNET_RT_TRYCONNECT_VISIBLE_NETWORK);
                int temp_ret = connect_wlan(niccard.m_devguid);
                if (1 == temp_ret)
                {
                    if(1 == diagnose_inf->IsDhcpEnabled(niccard.m_devguid))
                    {
                        emit_begin_connect();
                        emit_realtime_msg2(L_INTERNET_RT_DHCPEN_WAITTING);
                        waitdhcp_forgateway(niccard.m_devguid);
                        emit_end_connect();
                    }

                    test_gateway_dns(niccard);

                    if ((m_fix_status != ERROR_STATUS) || (!process_gateway_dns(niccard)))
                    {
                        break;
                    }
                }
                else if(0 == temp_ret)
                {
                    ihint = -1;
                }
                else
                {
                    ihint = -3;
                }

                emit_realtime_msg2(L_INTERNET_RT_CONNECT_VN_FAILED);
            }
            else
            {
                ihint = reconnect_wlan(niccard);

                if(1 == ihint || 2 == ihint)
                {
                    break;
                }
            }
        }
        else
        {
            emit_realtime_msg2(L_INTERNET_RT_REPAIR_NIC_FAILED,niccard.m_devdesc);
            break;
        }
    }while(true);

}

bool QGenieWinFixThread::waitdhcp_forgateway(const QString& nicguid)
{
    //interrupt ?
    AutoInterrupt(this);

    bool bret = false;
    int times = 90;
    QStringList gateway_list;
    do
    {
        gateway_list.clear();
        if(diagnose_inf->GetInterfaceGateway(nicguid,gateway_list))
        {
            if(gateway_list.count() != 0 && gateway_list[0] != QString("0.0.0.0"))
            {
                bret = true;
                break;
            }
        }
        fixthread_msleep_interruptable(1000);
    }while(--times);

    return bret;
}

bool QGenieWinFixThread::waitdhcp_fordns(const QString &nicguid)
{
    //interrupt ?
    AutoInterrupt(this);

    bool bret = false;
    int times = 90;
    QStringList dns_list;
    do
    {
        dns_list.clear();
        if(diagnose_inf->GetInterfaceDnsserver(nicguid,dns_list))
        {
            if(dns_list.count() != 0)
            {
                bret = true;
                break;
            }
        }
        fixthread_msleep_interruptable(1000);
    }while(--times);

    return bret;
}


void QGenieWinFixThread::emit_realtimemsg(const QString &msg)
{
    emit realtime_message(msg);
}

void QGenieWinFixThread::emit_realtime_msg2(int idx,const QString &param)
{
    emit realtime_msg2(idx,param);
}

void QGenieWinFixThread::emit_begin_pcflash()
{
    emit begin_pcflash();
}

void QGenieWinFixThread::emit_end_pcflash()
{
    emit end_pcflash();
}

void QGenieWinFixThread::emit_begin_routerflash()
{
    emit begin_routerflash();
}

void QGenieWinFixThread::emit_end_routerflash()
{
    emit end_routerflash();
}

void QGenieWinFixThread::emit_begin_internetflash()
{
    emit begin_internetflash();
}

void QGenieWinFixThread::emit_end_internetflash()
{
    emit end_internetflash();
}

void QGenieWinFixThread::emit_begin_pcrouterlinkflash()
{
    emit begin_pcrouterlinkflash();
}

void QGenieWinFixThread::emit_end_pcrouterlinkflash()
{
    emit end_pcrouterlinkflash();
}

void QGenieWinFixThread::emit_begin_routerinternetlinkflash()
{
    emit begin_routerinternetlinkflash();
}

void QGenieWinFixThread::emit_end_routerinternetlinkflash()
{
    emit end_routerinternetlinkflash();
}

void QGenieWinFixThread::emit_begin_connect()
{
    emit begin_connect();
}

void QGenieWinFixThread::emit_end_connect()
{
    emit end_connect();
}

void QGenieWinFixThread::emit_pcrouter_link_ok()
{
    emit pcrouter_link_ok();
}
void QGenieWinFixThread::emit_pcrouter_link_failed()
{
    emit pcrouter_link_failed();
}

void QGenieWinFixThread::emit_pcrouter_link_unknown()
{
    emit pcrouter_link_unknown();
}

void QGenieWinFixThread::emit_routerinternet_link_ok()
{
    emit routerinternet_link_ok();
}

void QGenieWinFixThread::emit_routerinternet_link_failed()
{
    emit routerinternet_link_failed();
}

void QGenieWinFixThread::emit_routerinternet_link_unknown()
{
    emit routerinternet_link_unknown();
}

bool QGenieWinFixThread::messagebox(int title_id,int text_id,bool byesorno)
{
    bool bresult = false;
    emit show_messagebox(title_id,text_id,byesorno,&bresult);
    //wait return
    wait_synmethodcall();

    bresult = this->m_bmessagebox_ret;
    //return
    return bresult;
}


void QGenieWinFixThread::show_messagebox_return(bool ret)
{
    m_bmessagebox_ret = ret;
    synmethodcall_return();
}

void QGenieWinFixThread::show_visiblenetworklist_dlg_return
(const QString &ssid,const QString &pwd)
{
    m_shvndlg_retssid = ssid;
    m_shvndlg_retpwd  = pwd;
    synmethodcall_return();
}

void QGenieWinFixThread::show_plugincable_dlg_return(int ret)
{
    m_shplugincable_dlg_result = ret;
    synmethodcall_return();
}

void QGenieWinFixThread::show_wlanoff_dlg_return(int ret)
{
    m_shwlanoff_dlg_result = ret;
    synmethodcall_return();
}

void QGenieWinFixThread::restore_nicsstate()
{
    qDebug () << "restore nics state";

    foreach(NCard nic,nic_list)
    {
        if(!nic.m_disabled)
        {
            diagnose_inf->NetCardStateChange(nic.m_devguid,true);
        }
    }
}

void QGenieWinFixThread::start(Priority priority)
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

void QGenieWinFixThread::wait_synmethodcall()
{
    m_synmethodcall_mutex->lock();
    m_synmethodcall_condition->wait(m_synmethodcall_mutex);
    m_synmethodcall_mutex->unlock();
}

void QGenieWinFixThread::synmethodcall_return()
{
    m_synmethodcall_condition->wakeOne();
}

void QGenieWinFixThread::cancel_fix_process(int code)
{
    m_ifix_canceled = 1;
    m_cancel_fix_process_code = code;
}

void QGenieWinFixThread::fixthread_msleep_interruptable(unsigned long ms)
{
    assert ( ms > 0 );

    unsigned long times = ms / 100;
    unsigned long remainder = ms % 100;

    while(times-- > 0)
    {
        process_cancel_interrupt();
        QThread::msleep(100);
    }

    if(remainder > 0)
    {
        process_cancel_interrupt();
        QThread::msleep(remainder);
    }

}

void QGenieWinFixThread::process_cancel_interrupt()
{
    if(m_ifix_canceled)
    {
        throw LONGTIME_OPERATION_INTERRUPT;
    }
}

//for restart router
void QGenieWinFixThread::show_restartrouter_dlg_return(int result)
{
    m_shrestartrouter_dlg_result = (result != 0);
    synmethodcall_return();
}

void QGenieWinFixThread::try_restartrouter(const NCard &nic)
{
    //interrupt ?
    AutoInterrupt(this);

    emit_begin_routerflash();

    emit show_restartrouter_dlg();
    wait_synmethodcall();

    if(!m_shrestartrouter_dlg_result)
    {
        return;
    }

    m_shrestartrouter_dlg_result = false;

    emit_realtime_msg2(L_INTERNET_RT_RESTARTROUTER_PROMPT_RESTARTINGROUTER);

    QString strProfile;
    if(nic.m_nictype == 2)
    {
        diagnose_inf->GetWlanNicCurConnectedProfile(nic.m_devguid,strProfile);
    }

    //reboot the router
    emit reboot_router();

    //wait for the router reboot
    fixthread_msleep_interruptable(70000);//70 seconds

    if(nic.m_nictype == 2 && !strProfile.isEmpty())
    {
        int times = 10;
        do
        {
            if((diagnose_inf->IsWlanNicConnected(nic.m_devguid) == 1) ||
                    diagnose_inf->TryConnect2WlanWithProfile2(nic.m_devguid,strProfile))
            {
                break;
            }
            else
            {
                fixthread_msleep_interruptable(1000);
            }
        }while(--times);


    }

    if(((nic.m_nictype != 2) || (diagnose_inf->IsWlanNicConnected(nic.m_devguid) == 1)) &&
            (1 == diagnose_inf->IsDhcpEnabled(nic.m_devguid)))
    {
        emit_realtime_msg2(L_INTERNET_RT_DHCPEN_WAITTING);
        if(waitdhcp_forgateway(nic.m_devguid))
        {
            test_gateway_dns(nic);
        }

    }

    emit end_show_restartrouter_dlg();

    emit_end_routerflash();
}
//end

