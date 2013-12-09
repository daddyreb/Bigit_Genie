#include "QGenieRepairThread.h"
#include "LanguageInternet.h"

#include <QDesktopServices>
#include <QUrl>
//#include "Genieplugin_Internet.h"

extern DiagnoseInterface *diagnose_inf;
#define INTERNET_STATE_OK_CODE      100
#define ROUTER_CONFIGURATION_PAGE   "http://routerlogin.net"

QGenieRepairThread::QGenieRepairThread():binternet_stateok(false)
        ,bneed_routercfg_gui(false)
        ,m_synmethodcall_mutex(0)
        ,m_synmethodcall_condition(0)
        ,m_bmessagebox_ret(false)
        ,m_shplugincable_dlg_result(0),m_shwlanoff_dlg_result(0)
        ,m_bis_repairok(false)
        ,m_shrestartrouter_dlg_result(false)
{
    setTerminationEnabled(true);
}


QGenieRepairThread::~QGenieRepairThread()
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

void QGenieRepairThread::run()
{
    if(0==diagnose_inf)
    {
        return;
    }
    Repair();
}

bool QGenieRepairThread::enable_nic(const NCard &niccard)
{
    bool bret      = false;
    int  interval  = 30;

    bret = diagnose_inf->NetCardStateChange(niccard.m_devguid,true);
    if(!bret)
    {
        goto fun_end;
    }

    do
    {
        if(diagnose_inf->IsNicDisabled(niccard.m_devguid)==0)
        {
            break;
        }
        QThread::msleep(1000);

    }while(--interval);

    if (diagnose_inf->IsNicDisabled(niccard.m_devguid)!=0)
    {
        bret = false;
    }
    else
    {
        //wait for nic enable complete
        //QThread::msleep(30000);
        bret = true;
    }

    fun_end:

    if(bret)
    {
        //emit_realtimemsg(tr("Enable Interface: %1 successfully").arg(niccard.m_devdesc));
        emit_realtime_msg2(L_INTERNET_RT_ENNIC_OK,niccard.m_devdesc);
        emit_pcrouter_link_unknown();
    }
    else
    {
        //emit_realtimemsg(tr("Enable Interface: %1 failed").arg(niccard.m_devdesc));
        emit_realtime_msg2(L_INTERNET_RT_ENNIC_FAILED,niccard.m_devdesc);
        emit_pcrouter_link_failed();
    }

    return bret;
}

//return code:1--->connected ;0-->not connected;-1-->not connected user canceled

int QGenieRepairThread::connect_wlan(const QString &strnicguid)
{
    QString str_ssid;
    QString str_pwd;
    int	iret=0;

    QList<VisibleNetwork> temp_vnlist;
    if(!diagnose_inf->GetVisibleWlanNetworks(strnicguid,temp_vnlist) || 0==temp_vnlist.size())
    {
        return iret;
    }

    emit show_visiblenetworklist_dlg(strnicguid,&str_ssid,&str_pwd);
    //wait return
    //m_syschronize_methodcall.acquire(1);
    wait_synmethodcall();

    str_ssid = this->m_shvndlg_retssid;
    str_pwd  = this->m_shvndlg_retpwd;
    //return

    if (strnicguid.isEmpty()==false && str_ssid.isEmpty()==false)
    {
        emit_begin_connect();
        //emit_realtimemsg(tr("Try connect to network %1 please wait ...").arg(str_ssid));
        emit_realtime_msg2(L_INTERNET_RT_TRYCONNECT_WLAN,str_ssid);
        if (diagnose_inf->ConnectToWlan(strnicguid,str_ssid,str_pwd))
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


bool QGenieRepairThread::test_repair_wirelessradio(const NCard &niccard)
{
    diagnose_inf->SwitchWlanNic(niccard.m_devguid,true);
    QThread::msleep(2000);

    emit_pcrouter_link_unknown();
    //emit_realtimemsg(tr("Being test wireless radio state please wait ..."));
    emit_realtime_msg2(L_INTERNET_RT_TESTING_WIRELESSRADIO);

    bool   bret = true;

    int    iWirelessState = -1;
    int    trytimes       = 3;

    do
    {
        iWirelessState = diagnose_inf->IsWlanNicSwitchedOff(niccard.m_devguid);

        if(iWirelessState != -1)
        {
            break;
        }
        else
        {
            QThread::msleep(1000);
        }

    }while(--trytimes);

    if (iWirelessState==2)//software off
    {
        emit_pcrouter_link_failed();
        //emit_realtimemsg(tr("The wireless radio is software switched off,try to switch on it"));
        emit_realtime_msg2(L_INTERNET_RT_WIRELESSRADIO_SOFTOFF);

        if (diagnose_inf->SwitchWlanNic(niccard.m_devguid,true))
        {
            emit_pcrouter_link_unknown();
            //emit_realtimemsg(tr("Switch on the wireless radio of the interface successfully"));
            emit_realtime_msg2(L_INTERNET_RT_SOFT_SWITCHON_RADIO_OK);
        }
        else
        {
            bret = false;
            //emit_realtimemsg(tr("Failed to switch on the wireless radio of the interface"));
            emit_realtime_msg2(L_INTERNET_RT_SOFT_SWITCHON_RADIO_FAILED);
        }
    }
    else if (iWirelessState==1)//hardware off
    {
        //emit_realtimemsg(tr("Wireless radio hardware switched off"));
        emit_realtime_msg2(L_INTERNET_RT_WIRELESSRADIO_HARDOFF);
        emit_pcrouter_link_failed();

        int	    iMsgRet                = 0;
        bool        bradioHardwareoff      = true;

        do
        {
            emit show_wlanradiohwoff_dlg(&iMsgRet);
            //wait return
            //m_syschronize_methodcall.acquire(1);
            wait_synmethodcall();

            iMsgRet = m_shwlanoff_dlg_result;

            if (1==iMsgRet)
            {
                bradioHardwareoff=(1==diagnose_inf->IsWlanNicSwitchedOff(niccard.m_devguid));
            }
            else
            {
                break;
            }

        }while (bradioHardwareoff);

        if(bradioHardwareoff)
        {
            bret = false;
            //emit_realtimemsg(tr("Wireless radio is still hardware switched off"));
            emit_realtime_msg2(L_INTERNET_RT_WIRELESSRADIO_STILL_HARDOFF);
        }
        else
        {
            emit_pcrouter_link_unknown();
            //emit_realtimemsg(tr("Hardware switch on wireless radio successfully"));
            emit_realtime_msg2(L_INTERNET_RT_HARD_SWITCHON_RADIO_OK);
        }
        //------
    }
    else if (iWirelessState==-1)
    {
        //emit_realtimemsg(tr("The wireless radio state is unknown maybe switched off ,check it manually"));
        emit_realtime_msg2(L_INTERNET_RT_WIRELESSRADIO_STATE_UNKNOWN);
        //also try switch on
        diagnose_inf->SwitchWlanNic(niccard.m_devguid,true);
        /*continue;*/
    }
    else
    {
        //emit_realtimemsg(tr("The wireless radio is switched on"));
        emit_realtime_msg2(L_INTERNET_RT_WIRELESSRADIO_IS_ON);
    }

    return bret;
}

//return code:-1--->not connected ;-2-->connected and dhcp not enabled but gateway ,dns invalid
//-3 user canceled
//0-->connected and dhcp enabled but gateway ,dns invalid
//1-->connected gateway and dns are valid
//2-->connected gateway reachable but can't go outside
//INTERNET_STATE_OK_CODE-->internet state is ok now

int QGenieRepairThread::reconnect_wlan(const NCard &niccard)
{
    int    ires = -1;

    emit_begin_connect();
    //emit_realtimemsg(tr("Try to connect to wireless network with profile , please wait ..."));
    emit_realtime_msg2(L_INTERNET_RT_TRYCONNECT_WLAN_WITHPROFILE);

    if (diagnose_inf->ReConnectWlan(niccard.m_devguid))
    {
        if(1==diagnose_inf->IsDhcpEnabled(niccard.m_devguid))
        {
            ires = 0;
            //emit_realtimemsg(tr("Dhcp enabled on this interface ,waiting for dhcp complete ..."));
            emit_realtime_msg2(L_INTERNET_RT_DHCPEN_WAITTING);
            waitdhcp_forgateway(niccard.m_devguid);
        }
        else
        {
            ires = -2;
        }

        emit_end_connect();
        int temp_ret = test_gateway_dns(niccard);
        if(INTERNET_STATE_OK_CODE==temp_ret
           || 2==temp_ret)
        {
            return temp_ret;
        }

        if (1==temp_ret)
        {
            ires = 1;
        }
    }
    else
    {
        ires = -1;
        //emit_realtimemsg(tr("Failed to reconnect,maybe the wireless radio is switched off"));
        emit_realtime_msg2(L_INTERNET_RT_RECONNECT_FAILED);
        emit_end_connect();
    }

    if(ires!=1)
    {

        //emit_realtimemsg(tr("Try connect to available wireless network please wait ..."));
        emit_realtime_msg2(L_INTERNET_RT_TRYCONNECT_VISIBLE_NETWORK);
        int temp_ret = connect_wlan(niccard.m_devguid);
        if (1==temp_ret)
        {
            if(1==diagnose_inf->IsDhcpEnabled(niccard.m_devguid))
            {
                ires = 0;
                emit_begin_connect();
                //emit_realtimemsg(tr("Dhcp enabled on this interface ,waiting for dhcp complete ..."));
                emit_realtime_msg2(L_INTERNET_RT_DHCPEN_WAITTING);
                waitdhcp_forgateway(niccard.m_devguid);
                emit_end_connect();
            }
            else
            {
                ires = -2;
            }

            int temp_ret = test_gateway_dns(niccard);
            if(INTERNET_STATE_OK_CODE==temp_ret
               || 2==temp_ret)
            {
                return temp_ret;
            }

            if (1==temp_ret)
            {
                ires = 1;
            }
        }
        else
        {
            ires =(( -1==temp_ret)?-3:-1);
            //emit_realtimemsg(tr("Failed to connect to network,maybe the wireless radio is switched off"));
            emit_realtime_msg2(L_INTERNET_RT_CONNECT_VN_FAILED);
        }
    }

    return ires;
}

//return code:1-->gateway and dns are both ok,
//                  2-->gateway reachable but can't go outside
//                 INTERNET_STATE_OK_CODE -->internet is ok now
//                     0-->other
int QGenieRepairThread::test_gateway_dns(const NCard &niccard)
{
    if(binternet_stateok)
    {
        emit_pcrouter_link_ok();
        emit_routerinternet_link_ok();
        return INTERNET_STATE_OK_CODE;
    }

    int iGatewayOk =-1,iDnsOk = -1;
    //emit_realtimemsg(tr("Being test the gateway ..."));
    emit_realtime_msg2(L_INTERNET_RT_TESTING_GATEWAY);
    emit_begin_pcrouterlinkflash();
    iGatewayOk=diagnose_inf->RepairGatewayorDnsOk(niccard.m_devguid,true);
    emit_end_pcrouterlinkflash();

    switch(iGatewayOk)
    {
    case 1:
        emit_pcrouter_link_ok();
        //emit_realtimemsg(tr("The gateway is valid"));
        emit_realtime_msg2(L_INTERNET_RT_GATEWAY_VALID);
        break;
    case 0:
        emit_pcrouter_link_failed();
        //emit_realtimemsg(tr("The gateway is invalid"));
        emit_realtime_msg2(L_INTERNET_RT_GATEWAY_INVALID);
        break;
    case 2:
        //emit_pcrouter_link_failed();
        emit_pcrouter_link_ok();
        emit_routerinternet_link_failed();
        //emit_realtimemsg(tr("The gateway's outside port doesn't work"));
        emit_realtime_msg2(L_INTERNET_RT_GATEWAY_OUTSIDEPORT_INVALID);
        break;
    default:
        emit_pcrouter_link_unknown();
        //emit_realtimemsg(tr("The state of the gateway is unknown"));
        emit_realtime_msg2(L_INTERNET_RT_GATEWAY_STATE_UNKNOWN);
    }

    if(binternet_stateok)
    {
        emit_pcrouter_link_ok();
        emit_routerinternet_link_ok();
        return INTERNET_STATE_OK_CODE;
    }

    if (1==iGatewayOk)
    {
        //emit_realtimemsg(tr("Being test the dns ..."));
        emit_realtime_msg2(L_INTERNET_RT_TESTING_DNS);
        emit_begin_routerinternetlinkflash();
        iDnsOk=diagnose_inf->RepairGatewayorDnsOk(niccard.m_devguid,false);
        emit_end_routerinternetlinkflash();

        switch(iDnsOk)
        {
        case 1:
            emit_routerinternet_link_ok();
            //emit_realtimemsg(tr("The dns is valid"));
            emit_realtime_msg2(L_INTERNET_RT_DNS_VALID);
            break;
        case 0:
            emit_routerinternet_link_failed();
            //emit_realtimemsg(tr("The dns is invalid"));
            emit_realtime_msg2(L_INTERNET_RT_DNS_INVALID);
            break;
        default:
            emit_routerinternet_link_unknown();
            //emit_realtimemsg(tr("The state of the dns is unknown"));
            emit_realtime_msg2(L_INTERNET_RT_DNS_STATE_UNKNOWN);
        }
    }
    else
    {
        //emit_realtimemsg(tr("The gateway is invalid ,dns test can not go on"));
        emit_realtime_msg2(L_INTERNET_RT_DNS_TEST_CANNOTDO);
        if(iGatewayOk!=2)
        {
            emit_routerinternet_link_unknown();
        }

    }

    return ((1==iGatewayOk)&&(1==iDnsOk))?1:(2==iGatewayOk?2:0);
}


bool QGenieRepairThread::plugin_cable(const NCard &niccard)
{
    int	   iMsgRet        =0;
    bool        bPluggedin	=false;
    do
    {
        emit show_plugincable_dlg(&iMsgRet);
        //wait return
        //m_syschronize_methodcall.acquire(1);
        wait_synmethodcall();

        iMsgRet = m_shplugincable_dlg_result;

        if (1==iMsgRet)
        {
            bPluggedin=diagnose_inf->IsNicCablePluggedin(niccard.m_devguid);
        }
        else
        {
            break;
        }

    } while (!bPluggedin);

    if (!bPluggedin)
    {
        //emit_realtimemsg(tr("The cable of the wired interface is still not plugged in"));
        emit_realtime_msg2(L_INTERNET_RT_CABLE_STILLNOTIN);
        emit_pcrouter_link_failed();
    }
    else
    {
        //emit_realtimemsg(tr("The cable is plugged in now"));
        emit_realtime_msg2(L_INTERNET_RT_CABLE_PLUGINNOW);
        emit_pcrouter_link_unknown();
    }

    return bPluggedin;
}

int QGenieRepairThread::repair_gateway(const NCard &niccard)
{
    int iGatewayOk = -1;

    emit_begin_routerflash();
    emit_begin_pcrouterlinkflash();

    if (diagnose_inf->IsDhcpEnabled(niccard.m_devguid)!=1)
    {
        //emit_realtimemsg(tr("DHCP is disabled ,try to enable DHCP and get a valid gateway"));
        emit_realtime_msg2(L_INTERNET_RT_DHCPDIS_TRY_ENABLEIT);
        if (diagnose_inf->EnableDhcp(niccard.m_devguid))
        {
            //emit_realtimemsg(tr("Enable DHCP successfully and try to get a valid gateway , please wait ..."));
            emit_realtime_msg2(L_INTERNET_RT_ENDHCP_OK);
            waitdhcp_forgateway(niccard.m_devguid);

            emit_begin_pcrouterlinkflash();
            iGatewayOk=diagnose_inf->RepairGatewayorDnsOk(niccard.m_devguid,true);
            emit_end_pcrouterlinkflash();

            if (1==iGatewayOk)
            {
                //emit_realtimemsg(tr("Get a valid gateway from DHCP successfully"));
                emit_realtime_msg2(L_INTERNET_RT_GETVALID_GATEWAY_DHCP);
                emit_pcrouter_link_ok();
            }
            else if (2==iGatewayOk)
            {
                //emit_realtimemsg(tr("Get a gateway successfully,but its outside port doesn't work"));
                emit_realtime_msg2(L_INTERNET_RT_GETOUTSIDE_INVALID_GATEWAY_DHCP);
                //emit_pcrouter_link_unknown();
                emit_pcrouter_link_ok();
                emit_routerinternet_link_failed();
            }
            else
            {
                //emit_realtimemsg(tr("Failed to get a valid gateway from DHCP"));
                emit_realtime_msg2(L_INTERNET_RT_FAILED_GETVALIDGATEWAY_DHCP);
                emit_pcrouter_link_failed();
            }
        }
        else
        {
            //emit_realtimemsg(tr("Failed to enable DHCP on this interface"));
            emit_realtime_msg2(L_INTERNET_RT_ENABLEDHCP_FAILED);
            emit_pcrouter_link_failed();
        }
    }//if
    else //Dhcp is Enabled
    {
        //emit_realtimemsg(tr("DHCP is enabled,but no valid DHCP server found"));
        emit_realtime_msg2(L_INTERNET_RT_DHCPISEN_NODHCPSERVER);
        emit_pcrouter_link_failed();
    }

    emit_end_pcrouterlinkflash();
    emit_end_routerflash();

    return iGatewayOk;
}

bool QGenieRepairThread::repair_dns(const NCard &niccard)
{
    int iDNSOk = -1;
    emit_begin_internetflash();
    emit_begin_routerinternetlinkflash();

    //emit_realtimemsg(tr("Try to get dns with DHCP"));
    emit_realtime_msg2(L_INTERNET_RT_TRY_GETDNS_WITHDHCP);
    if (diagnose_inf->EnableDnsFromDhcp(niccard.m_devguid))
    {
        //emit_realtimemsg(tr("Enable dns from DHCP successfully"));
        //emit_realtimemsg(tr("Try to get dns with DHCP , please wait ..."));
        emit_realtime_msg2(L_INTERNET_RT_ENABLE_DNS_FROMDHCP_OK);
        emit_realtime_msg2(L_INTERNET_RT_TRYGETDNS_FROMDHCP);
        //QThread::msleep(60000);
        waitdhcp_fordns(niccard.m_devguid);

        iDNSOk=diagnose_inf->RepairGatewayorDnsOk(niccard.m_devguid,false);
        if (1==iDNSOk)
        {
            //emit_realtimemsg(tr("Get a valid dns from DHCP"));
            emit_realtime_msg2(L_INTERNET_RT_GETVALIDDNS_FROMDHCP);
            emit_routerinternet_link_ok();
        }
        else
        {
            //emit_realtimemsg(tr("Failed to get valid dns from DHCP"));
            emit_realtime_msg2(L_INTERNET_RT_GETDNS_FROMDHCP_FAILED);
            emit_routerinternet_link_failed();
        }
    }
    else
    {
        //emit_realtimemsg(tr("Failed to enable getting dns from DHCP"));
        emit_realtime_msg2(L_INTERNET_RT_ENABLE_DNS_FROMDHCP_FAILED);
        emit_routerinternet_link_failed();
    }

    if(iDNSOk!=1)
    {
        //emit_realtimemsg(tr("Try to set default dns , please wait ..."));
        emit_realtime_msg2(L_INTERNET_RT_TRY_SETDEFAULT_DNS);
        if (diagnose_inf->ConfigDNSManually(niccard.m_devguid))
        {
            iDNSOk=1;
            //emit_realtimemsg(tr("set default dns ok"));
            emit_realtime_msg2(L_INTERNET_RT_SETDEFAULT_DNS_OK);
            emit_routerinternet_link_ok();
        }
        else
        {
            //emit_realtimemsg(tr("set default dns failed"));
            emit_realtime_msg2(L_INTERNET_RT_SETDEFAULT_DNS_FAILED);
            emit_routerinternet_link_failed();
        }
    }

    emit_end_routerinternetlinkflash();
    emit_end_internetflash();

    return 1==iDNSOk;
}


void QGenieRepairThread::Repair()
{
    //GeniePlugin_Internet::output_log(QString("begin repair network ..."));
    m_bis_repairok = false;

    emit_realtime_msg2(L_INTERNET_RT_BRIEFPROMPT_RUNNINNG_INTERNET_FIX);

    nic_list.clear();

    QList<int>          index_list;
    int    i            =  0;

    if(binternet_stateok)
    {
        emit_realtime_msg2(L_INTERNET_RT_NETWORK_ISOK_NOW);
        emit_pcrouter_link_ok();
        emit_routerinternet_link_ok();
        m_bis_repairok = true;
        goto repair_end;
    }

    //emit_realtimemsg(tr("Try to repair the network ..."));
    emit_realtime_msg2(L_INTERNET_RT_TRYTO_REPAIRNETWORK);

    if (!diagnose_inf->EnumInterfaces(nic_list) || nic_list.size()==0)
    {
        //emit_realtimemsg(tr("Get network interfaces failed,may be no interface"));
        emit_realtime_msg2(L_INTERNET_RT_GETNICS_FAILED);
        goto repair_end;
    }


    for(int k=0;k<nic_list.size();++k)
    {
        if(2==nic_list[k].m_nictype)
        {
            index_list.prepend(k);
        }
        else if(1==nic_list[k].m_nictype)
        {
            index_list.append(k);
        }
        //other type interfaces is ignored
    }


    for (int j=0;j<index_list.size();++j)
    {
        bneed_routercfg_gui = false;

        if(binternet_stateok)
        {
            emit_realtime_msg2(L_INTERNET_RT_NETWORK_ISOK_NOW);
            emit_pcrouter_link_ok();
            emit_routerinternet_link_ok();
            m_bis_repairok = true;
            break;
        }

        i = index_list[j];
        bool bhas_auto_enabled = false;

        emit_begin_pcflash();

        //emit_realtimemsg(tr("Try to repair Interface:%1 , please wait ...").arg(nic_list[i].m_devdesc));
        emit_realtime_msg2(L_INTERNET_RT_TRYREPAIR_NIC_WAIT,nic_list[i].m_devdesc);

        if(diagnose_inf->IsNicDisabled(nic_list[i].m_devguid)!=1)
        {
            diagnose_inf->NetCardStateChange(nic_list[i].m_devguid,false);
            //waitting for internface disabled
            int disable_wait_times = 10;
            do
            {
                if(1==diagnose_inf->IsNicDisabled(nic_list[i].m_devguid))
                {
                    break;
                }
                QThread::msleep(1000);
            }while(--disable_wait_times);
        }

        if (1==diagnose_inf->IsNicDisabled(nic_list[i].m_devguid))
        {
            //emit_realtimemsg(tr("Interface being disabled so try to enable it , please wait ..."));
            emit_realtime_msg2(L_INTERNET_RT_NICDIS_TRYEN_WAIT);
            if(enable_nic(nic_list[i]))
            {
                bhas_auto_enabled = true;
            }
            else
            {
                //emit_realtimemsg(tr("Repair interface:%1 failed").arg(nic_list[i].m_devdesc));
                emit_realtime_msg2(L_INTERNET_RT_REPAIR_NIC_FAILED,nic_list[i].m_devdesc);
                emit_end_pcflash();

                if(this->messagebox(L_INTERNET_RT_RECUR_TRY_REPAIR_MSGBOX_PROMPT
                                    ,L_INTERNET_RT_RECUR_TRY_REPAIR_MSGBOX_OTHER_CASE,true))
                {
                    --j;//repeat current adpter repairing
                }
                continue;
            }
        }

        diagnose_inf->IpRenewInterface(nic_list[i].m_devguid);

        if (nic_list[i].m_nictype==2)//wireless
        {
            if(!test_repair_wirelessradio(nic_list[i]))
            {
                //emit_realtimemsg(tr("Repair interface:%1 failed").arg(nic_list[i].m_devdesc));
                emit_realtime_msg2(L_INTERNET_RT_REPAIR_NIC_FAILED,nic_list[i].m_devdesc);
                emit_end_pcflash();
                if(this->messagebox(L_INTERNET_RT_RECUR_TRY_REPAIR_MSGBOX_PROMPT
                                    ,L_INTERNET_RT_RECUR_TRY_REPAIR_MSGBOX_OTHER_CASE,true))
                {
                    --j;//repeat current adpter repairing
                }
                continue;
            }

            if(bhas_auto_enabled)
            {
                int times=30;
                do
                {
                    if(diagnose_inf->IsWlanNicConnected(nic_list[i].m_devguid)==1)
                    {
                        break;
                    }
                    QThread::msleep(1000);
                }while(--times);

                if(diagnose_inf->IsWlanNicConnected(nic_list[i].m_devguid)==1)
                {
                    if (1==diagnose_inf->IsDhcpEnabled(nic_list[i].m_devguid))//等待DHCP获取IP
                    {
                        //emit_realtimemsg(tr("Dhcp enabled on this interface ,waiting for dhcp complete ..."));
                        emit_realtime_msg2(L_INTERNET_RT_DHCPEN_WAITTING);
                        if(waitdhcp_forgateway(nic_list[i].m_devguid))
                        {
                            //                            if(wait_internet_ok())
                            //                            {
                            //                                emit_realtime_msg2(L_INTERNET_RT_NETWORK_ISOK_NOW);
                            //                                emit_end_pcflash();
                            //                                break;
                            //                            }
                        }
                    }

                    int temp_ret = test_gateway_dns(nic_list[i]);
                    if (1==temp_ret
                        || INTERNET_STATE_OK_CODE==temp_ret)
                    {
                        //emit_realtimemsg(tr("The network is ok now"));
                        emit_realtime_msg2(L_INTERNET_RT_NETWORK_ISOK_NOW);
                        emit_end_pcflash();
                        m_bis_repairok = true;
                        break;
                    }
                    else if(2==temp_ret)
                    {
                        emit_realtime_msg2(L_INTERNET_RT_ROUTER_WLANCABLE_MAY_PLUGINOUT);
                        emit_end_pcflash();
                        bneed_routercfg_gui = true;
                        break;
                    }
                }
            }

            if (diagnose_inf->IsWlanNicConnected(nic_list[i].m_devguid)!=1)
            {
                //emit_realtimemsg(tr("This interface is not connected"));
                emit_realtime_msg2(L_INTERNET_RT_NIC_NOT_CONNECTED);
                int iresult = reconnect_wlan(nic_list[i]);
                if(1==iresult)
                {
                    //emit_realtimemsg(tr("Repair interface:%1 successfully").arg(nic_list[i].m_devdesc));
                    emit_realtime_msg2(L_INTERNET_RT_REPAIR_NIC_OK,nic_list[i].m_devdesc);
                    emit_end_pcflash();
                    m_bis_repairok = true;
                    break;
                }
                else if(2==iresult)
                {
                    emit_realtime_msg2(L_INTERNET_RT_ROUTER_WLANCABLE_MAY_PLUGINOUT);
                    emit_end_pcflash();
                    bneed_routercfg_gui = true;
                    break;
                }
                //not connected or connected but gateway or dns is invalid with dhcp enabled,user canceled
                else if(-1==iresult || 0==iresult || -3==iresult)
                {
                    int temp_ret = recur_tryrepair_wireless(nic_list[i],iresult,(j+1)<index_list.size());
                    if(1==temp_ret)
                    {
                        //emit_realtimemsg(tr("Repair interface:%1 successfully").arg(nic_list[i].m_devdesc));
                        emit_realtime_msg2(L_INTERNET_RT_REPAIR_NIC_OK,nic_list[i].m_devdesc);
                        emit_end_pcflash();
                        m_bis_repairok = true;
                        break;
                    }
                    else if(INTERNET_STATE_OK_CODE==temp_ret)
                    {
                        emit_realtime_msg2(L_INTERNET_RT_NETWORK_ISOK_NOW);
                        emit_end_pcflash();
                        m_bis_repairok = true;
                        break;
                    }
                    else if(2==temp_ret)
                    {
                        emit_realtime_msg2(L_INTERNET_RT_ROUTER_WLANCABLE_MAY_PLUGINOUT);
                        emit_end_pcflash();
                        bneed_routercfg_gui = true;
                        break;
                    }
                    else
                    {
                        //emit_realtimemsg(tr("Repair interface:%1 failed.").arg(nic_list[i].m_devdesc));
                        emit_realtime_msg2(L_INTERNET_RT_REPAIR_NIC_FAILED,nic_list[i].m_devdesc);
                        emit_end_pcflash();
                        continue;
                    }
                }
                else if(-2==iresult)//dhcp is not enabled
                {
                    //connected but gateway or dns is invalid  goto next step:repair gateway and dns
                }
            }
        }//if
        else  if(nic_list[i].m_nictype==1 || nic_list[i].m_nictype==5)//wired
        {
            bool bcable_plugin  = false;
            bool iscable_in     = false;
            int times = 5;

            do
            {
                iscable_in = diagnose_inf->IsNicCablePluggedin(nic_list[i].m_devguid);
                if(iscable_in)
                {
                    break;
                }

                QThread::msleep(1000);

            }while((!iscable_in) && (--times));

            if (!iscable_in)
            {
                //emit_realtimemsg(tr("The cable of the wired interface is not plugged in,please plug it in"));
                emit_realtime_msg2(L_INTERNET_RT_CABLE_NOPLUGIN_PLUGINPROMPT);
                emit_pcrouter_link_failed();
                if(plugin_cable(nic_list[i]))
                {
                    bcable_plugin = true;
                }
                else
                {
                    //emit_realtimemsg(tr("Repair interface:%1 failed").arg(nic_list[i].m_devdesc));
                    emit_realtime_msg2(L_INTERNET_RT_REPAIR_NIC_FAILED,nic_list[i].m_devdesc);
                    emit_end_pcflash();
                    if(this->messagebox(L_INTERNET_RT_RECUR_TRY_REPAIR_MSGBOX_PROMPT
                                        ,L_INTERNET_RT_RECUR_TRY_REPAIR_MSGBOX_OTHER_CASE,true))
                    {
                        --j;//repeat current adpter repairing
                    }
                    continue;
                }
            }

            if(bhas_auto_enabled || bcable_plugin)
            {
                if (1==diagnose_inf->IsDhcpEnabled(nic_list[i].m_devguid))//等待DHCP获取IP
                {
                    //emit_realtimemsg(tr("Dhcp enabled on this interface ,waiting for dhcp complete ..."));
                    emit_realtime_msg2(L_INTERNET_RT_DHCPEN_WAITTING);
                    if(waitdhcp_forgateway(nic_list[i].m_devguid))
                    {
                        //                        if(wait_internet_ok())
                        //                        {
                        //                            emit_realtime_msg2(L_INTERNET_RT_NETWORK_ISOK_NOW);
                        //                            emit_end_pcflash();
                        //                            break;
                        //                        }
                    }
                }

                int temp_ret = test_gateway_dns(nic_list[i]);
                if (1==temp_ret
                    || INTERNET_STATE_OK_CODE==temp_ret)
                {
                    //emit_realtimemsg(tr("The network is ok now"));
                    emit_realtime_msg2(L_INTERNET_RT_NETWORK_ISOK_NOW);
                    emit_end_pcflash();
                    m_bis_repairok = true;
                    break;
                }
                else if(2==temp_ret)
                {
                    emit_realtime_msg2(L_INTERNET_RT_ROUTER_WLANCABLE_MAY_PLUGINOUT);
                    emit_end_pcflash();
                    bneed_routercfg_gui = true;
                    break;
                }
            }

        }
        emit_end_pcflash();

        //------------------------------repair gateway and dns --------------
        if(binternet_stateok)
        {
            emit_realtime_msg2(L_INTERNET_RT_NETWORK_ISOK_NOW);
            emit_pcrouter_link_ok();
            emit_routerinternet_link_ok();
            m_bis_repairok = true;
            break;
        }

        bool bgateway_ok = false,bdns_ok = false;
        int testgateway_ret = diagnose_inf->RepairGatewayorDnsOk(nic_list[i].m_devguid,true);
        if(testgateway_ret!=1)
        {
            if(2==testgateway_ret)
            {
                emit_realtime_msg2(L_INTERNET_RT_ROUTER_WLANCABLE_MAY_PLUGINOUT);
                emit_pcrouter_link_ok();
                emit_routerinternet_link_failed();
                bneed_routercfg_gui = true;
                break;
            }
            else
            {
                emit_pcrouter_link_failed();
                emit_routerinternet_link_unknown();
            }

            //emit_realtimemsg(tr("The gateway of the interface is invalid ,try to fix it ..."));
            emit_realtime_msg2(L_INTERNET_RT_NICGATEWAY_INVALID_TRYFIXIT);
            int temp_ret = repair_gateway(nic_list[i]);
            if(1==temp_ret)
            {
                bgateway_ok = true;
            }
            else if(2==temp_ret)
            {
                emit_realtime_msg2(L_INTERNET_RT_ROUTER_WLANCABLE_MAY_PLUGINOUT);
                bneed_routercfg_gui = true;
                break;
            }
        }
        else
        {
            bgateway_ok = true;
            emit_pcrouter_link_ok();
        }

        if(binternet_stateok)
        {
            emit_realtime_msg2(L_INTERNET_RT_NETWORK_ISOK_NOW);
            emit_pcrouter_link_ok();
            emit_routerinternet_link_ok();
            m_bis_repairok = true;
            break;
        }

        if(diagnose_inf->RepairGatewayorDnsOk(nic_list[i].m_devguid,false)!=1)
        {
            emit_routerinternet_link_failed();
            //emit_realtimemsg(tr("The dns of the interface is invalid"));
            emit_realtime_msg2(L_INTERNET_RT_THEDNS_ISINVALID);
            if(bgateway_ok)
            {
                //emit_realtimemsg(tr("Try to fix the dns of the interface ..."));
                emit_realtime_msg2(L_INTERNET_RT_TRYTOFIX_DNS);
                if(repair_dns(nic_list[i]))
                {
                    bdns_ok = true;
                    emit_routerinternet_link_ok();
                }
            }
            else
            {
                //emit_realtimemsg(tr("However ,can not fix it because the gateway is invalid"));
                emit_realtime_msg2(L_INTERNET_RT_CANNOTFIXDNS_GATEWAYISINVALID);
            }
        }
        else
        {
            bdns_ok = true;
            emit_routerinternet_link_ok();
        }
        //----------------------------repair gateway and dns end---------------------------------------

        if(bgateway_ok && bdns_ok)
        {
            //emit_realtimemsg(tr("Repair interface:%1 successfully").arg(nic_list[i].m_devdesc));
            emit_realtime_msg2(L_INTERNET_RT_REPAIR_NIC_OK,nic_list[i].m_devdesc);
            m_bis_repairok = true;
            break;
        }
        else
        {
            if(2==nic_list[i].m_nictype)//wireless
            {
                //ihint==0 hint:wireless connected but gateway or dns is invalid with dhcp enalbed
                int temp_ret = recur_tryrepair_wireless(nic_list[i],0,(j+1)<index_list.size());
                if(1==temp_ret)
                {
                    emit_realtime_msg2(L_INTERNET_RT_REPAIR_NIC_OK,nic_list[i].m_devdesc);
                    m_bis_repairok = true;
                    break;
                }
                else if(INTERNET_STATE_OK_CODE==temp_ret)
                {
                    emit_realtime_msg2(L_INTERNET_RT_NETWORK_ISOK_NOW);
                    m_bis_repairok = true;
                    break;
                }
                else if(2==temp_ret)
                {
                    emit_realtime_msg2(L_INTERNET_RT_ROUTER_WLANCABLE_MAY_PLUGINOUT);
                    bneed_routercfg_gui = true;
                    break;
                }
            }
            else
            {
                if(1==diagnose_inf->IsDhcpEnabled(nic_list[i].m_devguid))
                {
                    //try renew
                    int retcode = iprenew_interface(nic_list[i]);
                    if(INTERNET_STATE_OK_CODE==retcode
                       || 1==retcode || 2==retcode)
                    {
                        break;
                    }
                }
            }
            emit_realtime_msg2(L_INTERNET_RT_REPAIR_NIC_FAILED,nic_list[i].m_devdesc);
        }
    }//for

    repair_end:
    int fixstate = 0;
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
            //end
            fixstate = 2;
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

    //GeniePlugin_Internet::output_log(QString("repair network end"));
}

int QGenieRepairThread::iprenew_interface(const NCard &niccard)
{
    emit_realtime_msg2(L_INTERNET_RT_DHCPENABLE_TRY_RENEW_IP);
    if(diagnose_inf->IpRenewInterface(niccard.m_devguid))
    {
        emit_realtime_msg2(L_INTERNET_RT_RENEW_IP_OK);
        emit_realtime_msg2(L_INTERNET_RT_WAITGETIP_AFTER_IPRENEW);
        waitdhcp_forgateway(niccard.m_devguid);
        int temp_ret = test_gateway_dns(niccard);
        if(1==temp_ret)
        {
            emit_realtime_msg2(L_INTERNET_RT_REPAIR_NIC_OK,niccard.m_devdesc);
            m_bis_repairok = true;
        }
        else if(INTERNET_STATE_OK_CODE==temp_ret)
        {
            emit_realtime_msg2(L_INTERNET_RT_NETWORK_ISOK_NOW);
            m_bis_repairok = true;
        }
        else if(2==temp_ret)
        {
            emit_realtime_msg2(L_INTERNET_RT_ROUTER_WLANCABLE_MAY_PLUGINOUT);
            bneed_routercfg_gui = true;
        }

        return temp_ret;
    }
    else
    {
        emit_realtime_msg2(L_INTERNET_RT_RENEW_IP_FAILED);
        return -1;
    }
}

//return code:1--->ok;0-->failed;INTERNET_STATE_OK_CODE-->internet is ok now
//2-->connected gateway reachable but can't go outside
int QGenieRepairThread::recur_tryrepair_wireless(const NCard &niccard,int ihint,bool bmoreadapter)
{
    int iret = 0;
    do
    {
        if(binternet_stateok)
        {
            emit_realtime_msg2(L_INTERNET_RT_NETWORK_ISOK_NOW);
            emit_pcrouter_link_ok();
            emit_routerinternet_link_ok();
            iret = INTERNET_STATE_OK_CODE;
            break;
        }

        int prompt_text_id = 0;

        int   iWirelessState =  diagnose_inf->IsWlanNicSwitchedOff(niccard.m_devguid);
        if(1==iWirelessState || 2==iWirelessState)//wireless radio off
        {
            ihint = -4;
        }
        else if(0 == iWirelessState)//wireless radion on
        {
            if(-4==ihint || -1==ihint)
            {
                ihint = 0;
            }
        }
        else if(-4==ihint)
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
                    QThread::msleep(3000);
                }
            }

            if(0==ihint)
            {
                //try renew
                int retcode = iprenew_interface(niccard);
                if(INTERNET_STATE_OK_CODE==retcode
                   || 1==retcode || 2==retcode)
                {
                    iret = retcode;
                    break;
                }

                emit_realtime_msg2(L_INTERNET_RT_TRYCONNECT_VISIBLE_NETWORK);
                int temp_ret = connect_wlan(niccard.m_devguid);
                if (1==temp_ret)
                {
                    if(1==diagnose_inf->IsDhcpEnabled(niccard.m_devguid))
                    {
                        emit_begin_connect();
                        emit_realtime_msg2(L_INTERNET_RT_DHCPEN_WAITTING);
                        waitdhcp_forgateway(niccard.m_devguid);
                        emit_end_connect();
                    }
                    
                    int inner_ret = test_gateway_dns(niccard);
                    if (inner_ret!=0)
                    {
                        iret = inner_ret;
                        break;
                    }
                }
                else if(0==temp_ret)
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

                if(1==ihint || INTERNET_STATE_OK_CODE==ihint || 2==ihint)
                {
                    iret = ihint;
                    break;
                }
            }
        }
        else
        {
            break;
        }
    }while(true);

    return iret;
}

bool QGenieRepairThread::waitdhcp_forgateway(const QString& nicguid)
{
    bool bret = false;
    int times = 90;
    QStringList gateway_list;
    do
    {
        gateway_list.clear();
        if(diagnose_inf->GetInterfaceGateway(nicguid,gateway_list))
        {
            if(gateway_list.count()!=0 && gateway_list[0]!=QString("0.0.0.0"))
            {
                bret = true;
                break;
            }     
        }
        QThread::msleep(1000);
    }while(--times);

    return bret;
}

bool QGenieRepairThread::waitdhcp_fordns(const QString &nicguid)
{
    bool bret = false;
    int times = 90;
    QStringList dns_list;
    do
    {
        dns_list.clear();
        if(diagnose_inf->GetInterfaceDnsserver(nicguid,dns_list))
        {
            if(dns_list.count()!=0)
            {
                bret = true;
                break;
            }
        }
        QThread::msleep(1000);
    }while(--times);

    return bret;
}

//bool QGenieRepairThread::wait_internet_ok()
//{
//    int times = 30;
//    do
//    {
//        if(binternet_stateok)
//        {
//            break;
//        }
//        QThread::msleep(1000);
//    }while(--times);
//
//    if(binternet_stateok)
//    {
//        emit_pcrouter_link_ok();
//        emit_routerinternet_link_ok();
//    }
//
//    return binternet_stateok;
//}

void QGenieRepairThread::emit_realtimemsg(const QString &msg)
{
    emit realtime_message(msg);
}

void QGenieRepairThread::emit_realtime_msg2(int idx,const QString &param)
{
    emit realtime_msg2(idx,param);
}

void QGenieRepairThread::emit_begin_pcflash()
{
    emit begin_pcflash();
}

void QGenieRepairThread::emit_end_pcflash()
{
    emit end_pcflash();
}

void QGenieRepairThread::emit_begin_routerflash()
{
    emit begin_routerflash();
}

void QGenieRepairThread::emit_end_routerflash()
{
    emit end_routerflash();
}

void QGenieRepairThread::emit_begin_internetflash()
{
    emit begin_internetflash();
}

void QGenieRepairThread::emit_end_internetflash()
{
    emit end_internetflash();
}

void QGenieRepairThread::emit_begin_pcrouterlinkflash()
{
    emit begin_pcrouterlinkflash();
}

void QGenieRepairThread::emit_end_pcrouterlinkflash()
{
    emit end_pcrouterlinkflash();
}

void QGenieRepairThread::emit_begin_routerinternetlinkflash()
{
    emit begin_routerinternetlinkflash();
}

void QGenieRepairThread::emit_end_routerinternetlinkflash()
{
    emit end_routerinternetlinkflash();
}

void QGenieRepairThread::emit_begin_connect()
{
    emit begin_connect();
}

void QGenieRepairThread::emit_end_connect()
{
    emit end_connect();
}

void QGenieRepairThread::emit_pcrouter_link_ok()
{
    emit pcrouter_link_ok();
}
void QGenieRepairThread::emit_pcrouter_link_failed()
{
    emit pcrouter_link_failed();
}

void QGenieRepairThread::emit_pcrouter_link_unknown()
{
    emit pcrouter_link_unknown();
}

void QGenieRepairThread::emit_routerinternet_link_ok()
{
    emit routerinternet_link_ok();
}

void QGenieRepairThread::emit_routerinternet_link_failed()
{
    emit routerinternet_link_failed();
}

void QGenieRepairThread::emit_routerinternet_link_unknown()
{
    emit routerinternet_link_unknown();
}

bool QGenieRepairThread::messagebox(int title_id,int text_id,bool byesorno)
{
    bool bresult = false;
    emit show_messagebox(title_id,text_id,byesorno,&bresult);
    //wait return
    //m_syschronize_methodcall.acquire(1);
    wait_synmethodcall();

    bresult = this->m_bmessagebox_ret;
    //return
    return bresult;
}

void QGenieRepairThread::update_internetstate(bool bok)
{
    binternet_stateok = bok;
}

void QGenieRepairThread::show_messagebox_return(bool ret)
{
    m_bmessagebox_ret = ret;
    //m_syschronize_methodcall.release(1);
    synmethodcall_return();
}

void QGenieRepairThread::show_visiblenetworklist_dlg_return
        (const QString &ssid,const QString &pwd)
{
    m_shvndlg_retssid = ssid;
    m_shvndlg_retpwd  = pwd;
    //m_syschronize_methodcall.release(1);
    synmethodcall_return();
}

void QGenieRepairThread::show_plugincable_dlg_return(int ret)
{
    m_shplugincable_dlg_result = ret;
    //m_syschronize_methodcall.release(1);
    synmethodcall_return();
}

void QGenieRepairThread::show_wlanoff_dlg_return(int ret)
{
    m_shwlanoff_dlg_result = ret;
    //m_syschronize_methodcall.release(1);
    synmethodcall_return();
}

void QGenieRepairThread::restore_nicsstate()
{
    foreach(NCard nic,nic_list)
    {
        if(!nic.m_disabled)
        {
            diagnose_inf->NetCardStateChange(nic.m_devguid,true);
        }
    }
}

void QGenieRepairThread::start(Priority priority)
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

void QGenieRepairThread::wait_synmethodcall()
{
    m_synmethodcall_mutex->lock();
    m_synmethodcall_condition->wait(m_synmethodcall_mutex);
    m_synmethodcall_mutex->unlock();
}

void QGenieRepairThread::synmethodcall_return()
{
    //m_synmethodcall_mutex->lock();
    m_synmethodcall_condition->wakeOne();
    //m_synmethodcall_mutex->unlock();
}

void QGenieRepairThread::cancel_fix_process(int code)
{
    terminate();
    restore_nicsstate();
}


//for restart router
void QGenieRepairThread::show_restartrouter_dlg_return(int result)
{
    m_shrestartrouter_dlg_result = (result != 0);
    synmethodcall_return();
}

void QGenieRepairThread::try_restartrouter(const NCard &nic)
{

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
    QThread::msleep(70000);//70 seconds

    if(nic.m_nictype == 2 && !strProfile.isEmpty())
    {
        int times = 60;
        do
        {
            if(diagnose_inf->IsWlanNicConnected(nic.m_devguid) != 1)
            {
                diagnose_inf->TryConnect2WlanWithProfile2(nic.m_devguid,strProfile);
                QThread::msleep(1000);
            }
            else
            {
                QThread::msleep(10000);
                break;
            }
        }while(--times);

    }

    emit end_show_restartrouter_dlg();
}
//end
