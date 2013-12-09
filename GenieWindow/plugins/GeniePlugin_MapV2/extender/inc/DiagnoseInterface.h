#ifndef DIAGNOSEINTERFACE_H
#define DIAGNOSEINTERFACE_H
#include <QString>
#include <QStringList>
#include <QList>

typedef struct  
{
    int			m_id;
    bool		m_disabled;
    int			m_nictype;//1 有线网卡,2无线网卡
    QString		m_devdesc;
    QString		m_devguid;
}NCard;

typedef struct  
{
    int		wlanSignalQuality;
    bool        bSecurityEnabled;
    bool        bcurConnected;
    bool        bhasProfile;
    bool        bNetworkConnectable;
    QString	strProfileName;
    QString	strSsid;
    QString	strBssType;
    QString	strdot11DefaultAuthAlgorithm;
    QString	strdot11DefaultCipherAlgorithm;
}VisibleNetwork;

class DiagnoseInterface
{
public:
    virtual ~DiagnoseInterface() { }
public:
    virtual void enable_longtime_operation_interruptable(const int *flag_addr) = 0;
    virtual void disble_longtime_operation_interruptable() = 0;
public:

    virtual bool  EnumInterfaces(QList<NCard> &nic_list)=0;

    virtual bool  GetInterfaceDnsserver(const QString &nic_guid,QStringList &sl)=0;
    virtual bool  GetInterfaceGateway(const QString &nic_guid,QStringList &sl)=0;

    virtual int  IsNicGatewaysOk(const QString &nic_guid,bool interrupt_flag)=0;
    virtual int  IsNicDnsServersOk(const QString &nic_guid)=0;


    virtual bool  IsNicCablePluggedin(const QString &nic_guid)=0;
    virtual int  IsNicDisabled(const QString &nic_guid)=0;
    virtual int  IsWlanNicConnected(const QString &nic_guid)=0;
    virtual int  IsWlanNicSwitchedOff(const QString &nic_guid)=0;


    virtual bool  NetCardStateChange(const QString &nic_guid, bool enabled)=0;

    virtual bool  SetInterfaceProfile(const QString &nic_guid,const QString &ssid,
                                      const QString &auth,const QString &cipher,const QString &pwd)=0;

    virtual int  IsDhcpEnabled(const QString &nic_guid)=0;
    virtual bool  EnableDhcp(const QString &nic_guid,bool interrupt_flag)=0;

    virtual bool  EnableDnsFromDhcp(const QString &nic_guid,bool interrupt_flag)=0;
    virtual bool  SetInterfaceDnses(const QString &nic_guid,const QStringList &dnses,bool interrupt_flag)=0;

    virtual bool  SwitchWlanNic(const QString &nic_guid,bool switch_on)=0;

    virtual int  RepairGatewayorDnsOk(const QString &nic_guid,bool gatewayordns,bool interrupt_flag)=0;
    virtual bool  ConfigDNSManually(const QString &nic_guid,bool interrupt_flag)=0;
    virtual bool  ReConnectWlan(const QString &nic_guid,bool interrupt_flag)=0;
    virtual bool UiEditProfile(const QString &nic_guid,const QString &profile_name)=0;
    virtual bool ConnectToWlan(const QString &nic_guid,const QString &ssid,const QString &pwd,bool interrupt_flag)=0;
    virtual bool ReConnectCurWlan(const QString &nic_guid,const QString &new_pwd,bool interrupt_flag)=0;
    virtual int  GetWlanChannel(const QString &nic_guid,const QString &ssid,
                                const QString &dot11BssType,bool bSecurityEnabled)=0;
    virtual bool  GetVisibleWlanNetworks(const QString &nic_guid,QList<VisibleNetwork> &vn_list,bool interrupt_flag)=0;
    virtual bool  IsWzcServiceRunning()=0;
    virtual int   IsWzcServiceRunning2()=0;
    virtual bool  StartWzcService()=0;
    virtual bool  Disconnect(const QString &nic_guid)=0;
    virtual bool  GetVisibleWlanNetworks2(const QString &nic_guid,QList<VisibleNetwork> &vn_list,bool brescan,bool interrupt_flag)=0;
    virtual bool  IpRenewInterface(const QString &nic_guid) = 0;
    virtual int   IsWlanAutoConfigEnabled(const QString &nic_guid)=0;
    virtual bool  EableWlanAutoConfig(const QString &nic_guid)=0;
    virtual bool  MakeWlanProfileForInterfaces(const QString &ssid,const QString &Auth,
                                               const QString &Cipher,const QString pwd)=0;
    virtual bool TryConnect2WlanWithProfile(const QString &profile)=0;
    virtual bool IsSsidConnected(const QString &ssid)=0;
    virtual int  NicConnectedNetworkSignal(const QString &nicguid)=0;
    virtual bool GetWlanNicCurConnectedProfile(const QString &nicguid,QString &strProfile)=0;
    virtual bool GetWlanNicCurConnectedSsid(const QString &nicguid,QString &strSsid)=0;
    virtual bool TryConnect2WlanWithProfile2(const QString &nicguid,const QString &strProfile) = 0;
    virtual bool IsHostReachble(const QString& strDestIp,unsigned long timeoutms) = 0;
    virtual bool IsThisPhysicalNic(const QString &nicguid) = 0;

    virtual bool GetWlanProfileList(const QString &nicguid, QStringList &sl) = 0;
    virtual bool DeleteWlanProfile(const QString &nicguid, const QString &strProfileName) = 0;
    virtual bool SetWlanProfilePosition(const QString &nicguid, const QString &strProfileName, unsigned long pos) = 0;
    virtual bool SetWlanProfileList(const QString &nicguid, const QStringList &sl) = 0;
    virtual bool SetWlanInterfaceProfile(const QString &nic_guid,const QString &ssid,
                                         const QString &auth,const QString &cipher,
                                         const QString &pwd, bool bnonebroadcast) = 0;
    virtual bool GetWlanInterfaceProfile(const QString &nic_guid,const QString &profileName,QString &profileXml) = 0;
    virtual bool GetWiFiInterfaces(QStringList &sl) = 0;
};

Q_DECLARE_INTERFACE(DiagnoseInterface,"com.dragonflow.DiagnosePlugin.DiagnoseInterface/1.0")
#endif // DIAGNOSEINTERFACE_H
