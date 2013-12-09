#ifndef DIAGNOSEPLUGIN_H
#define DIAGNOSEPLUGIN_H

#include <QObject>
#include "DiagnoseInterface.h"

class DiagnosePlugin : public QObject,public DiagnoseInterface
{
	Q_OBJECT
	Q_INTERFACES(DiagnoseInterface)
    Q_PLUGIN_METADATA(IID "com.dragonflow.DiagnosePlugin.DiagnoseInterface/1.0" FILE "DiagnosePlugin.json")
public:
    DiagnosePlugin();
	~DiagnosePlugin();
public:
        virtual void enable_longtime_operation_interruptable(const int *flag_addr);
        virtual void disble_longtime_operation_interruptable();
public:
	virtual  bool  EnumInterfaces(QList<NCard> &nic_list);

	virtual  bool  GetInterfaceDnsserver(const QString &nic_guid,QStringList &sl);
	virtual  bool  GetInterfaceGateway(const QString &nic_guid,QStringList &sl);

        virtual int  IsNicGatewaysOk(const QString &nic_guid,bool interrupt_flag);
	virtual int  IsNicDnsServersOk(const QString &nic_guid);


	virtual  bool  IsNicCablePluggedin(const QString &nic_guid);
	virtual  int  IsNicDisabled(const QString &nic_guid);
	virtual   int  IsWlanNicConnected(const QString &nic_guid);
	virtual int  IsWlanNicSwitchedOff(const QString &nic_guid);


	virtual bool  NetCardStateChange(const QString &nic_guid, bool enabled);

	virtual bool  SetInterfaceProfile(const QString &nic_guid,const QString &ssid,
		const QString &auth,const QString &cipher,const QString &pwd);

	virtual  int  IsDhcpEnabled(const QString &nic_guid);
        virtual  bool  EnableDhcp(const QString &nic_guid,bool interrupt_flag);

        virtual  bool  EnableDnsFromDhcp(const QString &nic_guid,bool interrupt_flag);
        virtual bool  SetInterfaceDnses(const QString &nic_guid,const QStringList &dnses,bool interrupt_flag);

	virtual bool  SwitchWlanNic(const QString &nic_guid,bool switch_on);

        virtual int  RepairGatewayorDnsOk(const QString &nic_guid,bool gatewayordns,bool interrupt_flag);
        virtual bool  ConfigDNSManually(const QString &nic_guid,bool interrupt_flag);
        virtual bool  ReConnectWlan(const QString &nic_guid,bool interrupt_flag);
	virtual  bool  UiEditProfile(const QString &nic_guid,const QString &profile_name);
        virtual  bool  ConnectToWlan(const QString &nic_guid,const QString &ssid,const QString &pwd,bool interrupt_flag);
        virtual bool  ReConnectCurWlan(const QString &nic_guid,const QString &new_pwd,bool interrupt_flag);
	virtual int  GetWlanChannel(const QString &nic_guid,const QString &ssid,
		const QString &dot11BssType,bool bSecurityEnabled);
        virtual bool  GetVisibleWlanNetworks(const QString &nic_guid,QList<VisibleNetwork> &vn_list,bool interrupt_flag);
        virtual bool  IsWzcServiceRunning();
        virtual int   IsWzcServiceRunning2();
        virtual bool  StartWzcService();
        virtual bool  Disconnect(const QString &nic_guid);
        virtual bool  GetVisibleWlanNetworks2(const QString &nic_guid,QList<VisibleNetwork> &vn_list,bool brescan,bool interrupt_flag);
        virtual bool  IpRenewInterface(const QString &nic_guid);
        virtual int   IsWlanAutoConfigEnabled(const QString &nic_guid);
        virtual bool  EableWlanAutoConfig(const QString &nic_guid);
        virtual bool  MakeWlanProfileForInterfaces(const QString &ssid,const QString &Auth,
                                                   const QString &Cipher,const QString pwd);
        virtual bool TryConnect2WlanWithProfile(const QString &profile);
        virtual bool IsSsidConnected(const QString &ssid);
        virtual int  NicConnectedNetworkSignal(const QString &nicguid);
        virtual bool GetWlanNicCurConnectedProfile(const QString &nicguid,QString &strProfile);
        virtual bool GetWlanNicCurConnectedSsid(const QString &nicguid,QString &strSsid);
        virtual bool TryConnect2WlanWithProfile2(const QString &nicguid,const QString &strProfile);
        virtual bool IsHostReachble(const QString& strDestIp,unsigned long timeoutms);
        virtual bool IsThisPhysicalNic(const QString &nicguid);

        virtual bool GetWlanProfileList(const QString &nicguid, QStringList &sl);
        virtual bool DeleteWlanProfile(const QString &nicguid, const QString &strProfileName);
        virtual bool SetWlanProfilePosition(const QString &nicguid, const QString &strProfileName, unsigned long pos);
        virtual bool SetWlanProfileList(const QString &nicguid, const QStringList &sl);
        virtual bool SetWlanInterfaceProfile(const QString &nic_guid,const QString &ssid,
                                             const QString &auth,const QString &cipher,
                                             const QString &pwd, bool bnonebroadcast);
        virtual bool GetWlanInterfaceProfile(const QString &nic_guid,const QString &profileName,QString &profileXml);
        virtual bool GetWiFiInterfaces(QStringList &sl);
};

#endif // DIAGNOSEPLUGIN_H
