#include "diagnoseplugin.h"
#include "DiagnoseDll.h"
#include <QtPlugin>

DiagnosePlugin::DiagnosePlugin()
{
}

DiagnosePlugin::~DiagnosePlugin()
{

}

void DiagnosePlugin::enable_longtime_operation_interruptable(const int *flag_addr)
{
    ::enable_longtime_operation_interruptable(flag_addr);
}

void DiagnosePlugin::disble_longtime_operation_interruptable()
{
    ::disble_longtime_operation_interruptable();
}

bool  DiagnosePlugin::EnumInterfaces(QList<NCard> &nic_list)
{
    nic_list.clear();
    NICLIST *niclist=NULL;
    bool	bret	=false;
    if (::EnumInterfaces(&niclist))
    {
        for (DWORD i=0;i<niclist->dwCount;++i)
        {
            NCard ncard;
            ncard.m_id	 = int(niclist->lpNics[i].m_dwId);
            ncard.m_nictype	 = int(niclist->lpNics[i].m_dwNicType);
            ncard.m_disabled = niclist->lpNics[i].m_bDisabled!=FALSE;
            ncard.m_devguid	 = QString::fromWCharArray(niclist->lpNics[i].m_chDevGuid);
            ncard.m_devdesc	 = QString::fromWCharArray(niclist->lpNics[i].m_chDevDesc);
            nic_list.push_back(ncard);
        }
        ::FreeNicListBuffer(niclist);
        bret=true;
    }

    return bret;
}

bool  DiagnosePlugin::GetInterfaceDnsserver(const QString &nic_guid,QStringList &sl)
{
    StrList *strlist=NULL;
    bool	bret	=false;
    if (::GetInterfaceDnsserver(nic_guid.toStdWString().c_str(),&strlist))
    {
        for (DWORD i=0;i<strlist->dwCount;++i)
        {
            sl.push_back(QString::fromWCharArray(strlist->strs[i]));
        }
        ::FreeStrListBuffer(strlist);
        bret=true;
    }
    return bret;
}

bool  DiagnosePlugin::GetInterfaceGateway(const QString &nic_guid,QStringList &sl)
{
    StrList *strlist    = NULL;
    bool    bret	= false;

    if (::GetInterfaceGateway(nic_guid.toStdWString().c_str(),&strlist))
    {
        for (DWORD i=0;i<strlist->dwCount;++i)
        {
            sl.push_back(QString::fromWCharArray(strlist->strs[i]));
        }
        ::FreeStrListBuffer(strlist);
        bret=true;
    }
    return bret;
}

int  DiagnosePlugin::IsNicGatewaysOk(const QString &nic_guid,bool interrupt_flag)
{
    return ::IsNicGatewaysOk(nic_guid.toStdWString().c_str(),interrupt_flag?1:0);
}

int  DiagnosePlugin::IsNicDnsServersOk(const QString &nic_guid)
{
    return ::IsNicDnsServersOk(nic_guid.toStdWString().c_str());
}


bool  DiagnosePlugin::IsNicCablePluggedin(const QString &nic_guid)
{
    return ::IsNicCablePluggedin(nic_guid.toStdWString().c_str())!=FALSE;
}

int  DiagnosePlugin::IsNicDisabled(const QString &nic_guid)
{
    return ::IsNicDisabled(nic_guid.toStdWString().c_str());
}

int  DiagnosePlugin::IsWlanNicConnected(const QString &nic_guid)
{
    return ::IsWlanNicConnected(nic_guid.toStdWString().c_str());
}

int  DiagnosePlugin::IsWlanNicSwitchedOff(const QString &nic_guid)
{
    return ::IsWlanNicSwitchedOff(nic_guid.toStdWString().c_str());
}


bool  DiagnosePlugin::NetCardStateChange(const QString &nic_guid, bool enabled)
{
    return ::NetCardStateChange(nic_guid.toStdWString().c_str(),enabled?TRUE:FALSE)!=FALSE;
}

bool  DiagnosePlugin::SetInterfaceProfile(const QString &nic_guid,const QString &ssid,
                                          const QString &auth,const QString &cipher,const QString &pwd)
{
    return ::SetInterfaceProfile(nic_guid.toStdWString().c_str(),ssid.toStdWString().c_str(),
                                 auth.toStdWString().c_str(),cipher.toStdWString().c_str(),pwd.toStdWString().c_str())!=FALSE;
}

int  DiagnosePlugin::IsDhcpEnabled(const QString &nic_guid)
{
    return ::IsDhcpEnabled(nic_guid.toStdWString().c_str());
}

bool  DiagnosePlugin::EnableDhcp(const QString &nic_guid, bool interrupt_flag)
{
    return ::EnableDhcp(nic_guid.toStdWString().c_str(),interrupt_flag?1:0)!=FALSE;
}

bool  DiagnosePlugin::EnableDnsFromDhcp(const QString &nic_guid,bool interrupt_flag)
{
    return ::EnableDnsFromDhcp(nic_guid.toStdWString().c_str(),interrupt_flag?1:0)!=FALSE;
}

bool  DiagnosePlugin::SetInterfaceDnses(const QString &nic_guid,const QStringList &dnses,bool interrupt_flag)
{
    bool bret=false;
    if (dnses.size()==0)
    {
        return bret;
    }
    TCHAR **pdnses=new TCHAR *[dnses.size()];
    for (int i=0;i<dnses.size();++i)
    {
        pdnses[i]=new TCHAR[dnses[i].length()+1];
        dnses[i].toWCharArray(pdnses[i]);
        *(pdnses[i]+dnses[i].length())=0;
    }
    bret=::SetInterfaceDnses(nic_guid.toStdWString().c_str(),dnses.size(),pdnses,interrupt_flag?1:0)!=FALSE;
    for (int j=0;j<dnses.size();++j)
    {
        delete [] pdnses[j];
    }
    delete [] pdnses;

    return bret;
}

bool  DiagnosePlugin::SwitchWlanNic(const QString &nic_guid,bool switch_on)
{
    return ::SwitchWlanNic(nic_guid.toStdWString().c_str(),switch_on?TRUE:FALSE)!=FALSE;
}

int  DiagnosePlugin::RepairGatewayorDnsOk(const QString &nic_guid,bool gatewayordns,bool interrupt_flag)
{
    return ::RepairGatewayorDnsOk(nic_guid.toStdWString().c_str(),gatewayordns?TRUE:FALSE,interrupt_flag?1:0);
}

bool  DiagnosePlugin::ConfigDNSManually(const QString &nic_guid, bool interrupt_flag)
{
    return ::ConfigDNSManually(nic_guid.toStdWString().c_str(),interrupt_flag?1:0)!=FALSE;
}

bool  DiagnosePlugin::ReConnectWlan(const QString &nic_guid,bool interrupt_flag)
{
    return ::ReConnectWlan(nic_guid.toStdWString().c_str(),interrupt_flag?1:0)!=FALSE;
}

bool  DiagnosePlugin::UiEditProfile(const QString &nic_guid,const QString &profile_name)
{
    return ::UiEditProfile(nic_guid.toStdWString().c_str(),profile_name.toStdWString().c_str())!=FALSE;
}

bool  DiagnosePlugin::ConnectToWlan(const QString &nic_guid,const QString &ssid,const QString &pwd,bool interrupt_flag)
{
    return ::ConnectToWlan(nic_guid.toStdWString().c_str(),
                           ssid.toStdWString().c_str(),pwd.toStdWString().c_str(),interrupt_flag?1:0)!=FALSE;
}

bool  DiagnosePlugin::ReConnectCurWlan(const QString &nic_guid,const QString &new_pwd,bool interrupt_flag)
{
    return ::ReConnectCurWlan(nic_guid.toStdWString().c_str(),new_pwd.toStdWString().c_str(),interrupt_flag?1:0)!=FALSE;
}

int  DiagnosePlugin::GetWlanChannel(const QString &nic_guid,const QString &ssid,
                                    const QString &dot11BssType,bool bSecurityEnabled)
{
    return ::GetWlanChannel(nic_guid.toStdWString().c_str(),ssid.toStdWString().c_str(),
                            dot11BssType.toStdWString().c_str(),bSecurityEnabled?TRUE:FALSE);
}

bool  DiagnosePlugin::GetVisibleWlanNetworks(const QString &nic_guid,QList<VisibleNetwork> &vn_list,bool interrupt_flag)
{
    return this->GetVisibleWlanNetworks2(nic_guid,vn_list,true,interrupt_flag);
}

bool  DiagnosePlugin::IsWzcServiceRunning()
{
    return ::IsWzcServiceRunning()!=FALSE;
}

int  DiagnosePlugin::IsWzcServiceRunning2()
{
    return ::IsWzcServiceRunning2()!=FALSE;
}

bool  DiagnosePlugin::StartWzcService()
{
    return ::StartWzcService()!=FALSE;
}

bool  DiagnosePlugin::Disconnect(const QString &nic_guid)
{
    return ::Disconnect(nic_guid.toStdWString().c_str())!=FALSE;
}

bool  DiagnosePlugin::GetVisibleWlanNetworks2(const QString &nic_guid,QList<VisibleNetwork> &vn_list,bool brescan,bool interrupt_flag)
{
    bool            bret            = false;
    Visible_Network *vn             = NULL;
    DWORD           dwNum           = 0;

    if (::GetVisibleWlanNetworks2(nic_guid.toStdWString().c_str(),&vn,&dwNum,brescan!=false,interrupt_flag?1:0))
    {
        for (DWORD i=0;i<dwNum;++i)
        {
            VisibleNetwork Vn;
            Vn.bcurConnected					= vn[i].bcurConnected!=FALSE;
            Vn.bhasProfile					= vn[i].bhasProfile!=FALSE;
            Vn.bNetworkConnectable				= vn[i].bNetworkConnectable!=FALSE;
            Vn.bSecurityEnabled					= vn[i].bSecurityEnabled!=FALSE;
            Vn.strBssType					= QString::fromWCharArray(vn[i].strBssType);
            Vn.strdot11DefaultAuthAlgorithm                     = QString::fromWCharArray(vn[i].strdot11DefaultAuthAlgorithm);
            Vn.strdot11DefaultCipherAlgorithm                   = QString::fromWCharArray(vn[i].strdot11DefaultCipherAlgorithm);
            Vn.strProfileName					= QString::fromWCharArray(vn[i].strProfileName);
            Vn.strSsid						= QString::fromWCharArray(vn[i].strSsid);
            Vn.wlanSignalQuality				= int(vn[i].wlanSignalQuality);
            vn_list.push_back(Vn);
        }
        FreeVisibleWlanNetworks(vn);
        bret = true;
    }
    return bret;
}

bool DiagnosePlugin::IpRenewInterface(const QString &nic_guid)
{
    return ::IpRenewInterface(nic_guid.toStdWString().c_str())!=FALSE;
}

int DiagnosePlugin::IsWlanAutoConfigEnabled(const QString &nic_guid)
{
    return ::IsWlanAutoConfigEnabled(nic_guid.toStdWString().c_str());
}

bool  DiagnosePlugin::EableWlanAutoConfig(const QString &nic_guid)
{
    return ::EableWlanAutoConfig(nic_guid.toStdWString().c_str())!=FALSE;
}

bool  DiagnosePlugin::MakeWlanProfileForInterfaces(const QString &ssid,const QString &Auth,
                                                   const QString &Cipher,const QString pwd)
{
    return ::MakeWlanProfileForInterfaces(ssid.toStdWString().c_str(),Auth.toStdWString().c_str()
                                          ,Cipher.toStdWString().c_str(),pwd.toStdWString().c_str())!=FALSE;
}

bool DiagnosePlugin::TryConnect2WlanWithProfile(const QString &profile)
{
    return ::TryConnect2WlanWithProfile(profile.toStdWString().c_str())!=FALSE;
}

bool DiagnosePlugin::IsSsidConnected(const QString &ssid)
{
    return ::IsSsidConnected(ssid.toStdWString().c_str())!=FALSE;
}

int DiagnosePlugin::NicConnectedNetworkSignal(const QString &nicguid)
{
    return ::NicConnectedNetworkSignal(nicguid.toStdWString().c_str());
}

bool DiagnosePlugin::GetWlanNicCurConnectedProfile(const QString &nicguid,QString &strProfile)
{
    char buffer[512] = {0};
    bool bret = ::GetWlanNicCurConnectedProfile(nicguid.toStdWString().c_str(),(LPTSTR)buffer,512) != FALSE;
    if(bret)
        strProfile = QString::fromWCharArray((const wchar_t*)buffer);

    return bret;
}

bool DiagnosePlugin::GetWlanNicCurConnectedSsid(const QString &nicguid,QString &strSsid)
{
    char buffer[512] = {0};
    bool bret = ::GetWlanNicCurConnectedSsid(nicguid.toStdWString().c_str(),(LPTSTR)buffer,512) != FALSE;
    if(bret)
        strSsid = QString::fromWCharArray((const wchar_t*)buffer);

    return bret;
}

bool DiagnosePlugin::TryConnect2WlanWithProfile2(const QString &nicguid,const QString &strProfile)
{
    return ::TryConnect2WlanWithProfile2(nicguid.toStdWString().c_str(),strProfile.toStdWString().c_str()) != FALSE;
}

bool DiagnosePlugin::IsHostReachble(const QString& strDestIp,unsigned long timeoutms)
{
    return ::IsHostReachble(strDestIp.toStdString().c_str(),timeoutms) != FALSE;
}

bool DiagnosePlugin::IsThisPhysicalNic(const QString &nicguid)
{
    return ::IsThisPhysicalNic(nicguid.toStdWString().c_str()) != FALSE;
}

bool DiagnosePlugin::GetWlanProfileList(const QString &nicguid, QStringList &sl)
{
    bool    bret     = false;
    StrList *strlist = NULL;

    if(::GetWlanProfileList(nicguid.toStdWString().c_str(),&strlist))
    {
        for(DWORD i = 0;i < strlist->dwCount;++i)
        {
            sl.push_back(QString::fromWCharArray(strlist->strs[i]));
        }
        ::FreeStrListBuffer(strlist);

        bret = true;
    }

    return bret;
}

bool DiagnosePlugin::DeleteWlanProfile(const QString &nicguid, const QString &strProfileName)
{
    return (::DeleteWlanProfile(nicguid.toStdWString().c_str(),strProfileName.toStdWString().c_str()) != FALSE);
}

bool DiagnosePlugin::SetWlanProfilePosition(const QString &nicguid, const QString &strProfileName, unsigned long pos)
{
    return (::SetWlanProfilePosition(nicguid.toStdWString().c_str(),strProfileName.toStdWString().c_str(),pos) != FALSE);
}

bool DiagnosePlugin::SetWlanProfileList(const QString &nicguid, const QStringList &sl)
{
    bool bret = false;
    StrList strlist;
    strlist.dwCount = sl.count();
    strlist.strs    = NULL;

    if(strlist.dwCount)
    {
        strlist.strs = new TCHAR *[strlist.dwCount];
    }

    for (DWORD i = 0 ; i < strlist.dwCount ; ++i)
    {
        strlist.strs[i]=new TCHAR[sl[i].length()+1];
        sl[i].toWCharArray(strlist.strs[i]);
        *(strlist.strs[i]+sl[i].length())=0;
    }

    bret = (::SetWlanProfileList(nicguid.toStdWString().c_str(),&strlist) != FALSE);

    for (DWORD j = 0 ; j < strlist.dwCount ; ++j)
    {
        delete [] strlist.strs[j];
    }

    delete [] strlist.strs;

    return bret;
}

bool DiagnosePlugin::SetWlanInterfaceProfile(const QString &nic_guid,const QString &ssid,
                                                  const QString &auth,const QString &cipher,
                                                  const QString &pwd, bool bnonebroadcast)
{
    return ::SetWlanInterfaceProfile(nic_guid.toStdWString().c_str(),
                                     ssid.toStdWString().c_str(),
                                     auth.toStdWString().c_str(),
                                     cipher.toStdWString().c_str(),
                                     pwd.toStdWString().c_str(),
                                     bnonebroadcast?TRUE:FALSE) != FALSE;
}

bool DiagnosePlugin::GetWlanInterfaceProfile(const QString &nic_guid,const QString &profileName,QString &profileXml)
{
    StrList *sl = NULL;
    bool bret = (::GetWlanInterfaceProfile(nic_guid.toStdWString().c_str(),profileName.toStdWString().c_str(),&sl) != FALSE);
    if(bret && sl && (sl->dwCount == 1))
    {
        profileXml = QString::fromWCharArray(sl->strs[0]);
        FreeStrListBuffer(sl);
    }
    else
    {
        profileXml.clear();
    }

    return bret;
}

bool DiagnosePlugin::GetWiFiInterfaces(QStringList &sl)
{
    bool    bret     = false;
    StrList *strlist = NULL;
    sl.clear();

    if(::GetWiFiInterfaces(&strlist))
    {
        for(DWORD i = 0 ; i < strlist->dwCount ; ++i)
        {
            sl.push_back(QString::fromWCharArray(strlist->strs[i]));
        }
        ::FreeStrListBuffer(strlist);

        bret = true;
    }

    return bret;
}


