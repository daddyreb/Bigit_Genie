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

    virtual  bool  EnumInterfaces(QList<NCard> &nic_list)=0;

    virtual  bool  GetInterfaceDnsserver(const QString &nic_guid,QStringList &sl)=0;
    virtual  bool  GetInterfaceGateway(const QString &nic_guid,QStringList &sl)=0;

    virtual int  IsNicGatewaysOk(const QString &nic_guid)=0;
    virtual int  IsNicDnsServersOk(const QString &nic_guid)=0;


    virtual  bool  IsNicCablePluggedin(const QString &nic_guid)=0;
    virtual  int  IsNicDisabled(const QString &nic_guid)=0;
    virtual   int  IsWlanNicConnected(const QString &nic_guid)=0;
    virtual int  IsWlanNicSwitchedOff(const QString &nic_guid)=0;


    virtual bool  NetCardStateChange(const QString &nic_guid, bool enabled)=0;

    virtual bool  SetInterfaceProfile(const QString &nic_guid,const QString &ssid,
                                      const QString &auth,const QString &cipher,const QString &pwd)=0;

    virtual  int  IsDhcpEnabled(const QString &nic_guid)=0;
    virtual  bool  EnableDhcp(const QString &nic_guid)=0;

    virtual  bool  EnableDnsFromDhcp(const QString &nic_guid)=0;
    virtual bool  SetInterfaceDnses(const QString &nic_guid,const QStringList &dnses)=0;

    virtual bool  SwitchWlanNic(const QString &nic_guid,bool switch_on)=0;

    virtual int  RepairGatewayorDnsOk(const QString &nic_guid,bool gatewayordns)=0;
    virtual bool  ConfigDNSManually(const QString &nic_guid)=0;
    virtual bool  ReConnectWlan(const QString &nic_guid)=0;
    virtual  bool  UiEditProfile(const QString &nic_guid,const QString &profile_name)=0;
    virtual  bool  ConnectToWlan(const QString &nic_guid,const QString &ssid,const QString &pwd)=0;
    virtual bool  ReConnectCurWlan(const QString &nic_guid,const QString &new_pwd)=0;
    virtual int  GetWlanChannel(const QString &nic_guid,const QString &ssid,
                                const QString &dot11BssType,bool bSecurityEnabled)=0;
    virtual bool  GetVisibleWlanNetworks(const QString &nic_guid,QList<VisibleNetwork> &vn_list)=0;
    virtual bool  IsWzcServiceRunning()=0;
    virtual int     IsWzcServiceRunning2()=0;
    virtual bool  StartWzcService()=0;
    virtual bool  Disconnect(const QString &nic_guid)=0;
    virtual bool  GetVisibleWlanNetworks2(const QString &nic_guid,QList<VisibleNetwork> &vn_list,bool brescan)=0;
};

Q_DECLARE_INTERFACE(DiagnoseInterface,"com.dragonflow.DiagnosePlugin.DiagnoseInterface/1.0")
#endif // DIAGNOSEINTERFACE_H
