#ifndef INETAPIINTERFACE_H
#define INETAPIINTERFACE_H

#include <QStringList>

//apis that has a long time operation being interrupted return code as follow
#include "INetConstants.h"

class INetApiInterface{
public:
    virtual ~INetApiInterface(){}
public:
    virtual void enable_longtime_operation_interruptable(const int *pflag) = 0;
    virtual void disable_longtime_operation_interruptable() = 0;
public:
    virtual bool  EnumInterfaces(QStringList &nic_list) = 0;
    virtual bool GetInterfaceHardwareAddr(const QString &nic_name,QString &hwaddr) = 0;

    virtual int IsNicWireless(const QString &nic_name,bool *bwireless) = 0;

    virtual int IsNicLinkUp(const QString &nic_name) = 0;
    virtual bool TurnNicLinkUporDown(const QString &nic_name,bool bup) = 0;

    virtual bool  IsNicCablePluggedin(const QString &nic_name) = 0;
/*
    virtual  int  IsNicDisabled(const QString &nic_guid) = 0;

    virtual  bool  GetInterfaceDnsserver(const QString &nic_guid,QStringList &sl) = 0;
    virtual  bool  GetInterfaceGateway(const QString &nic_guid,QStringList &sl) = 0;*/

    virtual bool GetPrimaryServiceRouter(QString &router) = 0;
    virtual bool GetPrimaryServiceDNS(QStringList &Dnslist) = 0;

    virtual int IsPrimaryServiceRouterOk(bool binterruptable = false) = 0;
    virtual int IsPrimaryServiceDnsOk(bool binterruptable = false) = 0;

    virtual bool MoveSetServiceFirstInServiceOrder(const QString *setId,const QString &svrId) = 0;

    virtual bool GetPrimaryInterfaceAndPrimaryService(QString &priInf,QString &priSvr) = 0;

    virtual int IsServiceActive(const QString *setId,const QString &svrId) = 0;

    virtual bool SetServiceActive(const QString *setId,const QString &svrId,bool bactive) = 0;

    virtual int IsServiceIPv4Active(const QString *setId,const QString &svrId) = 0;

    virtual bool SetServiceIPv4Active(const QString *setId,const QString &svrId,bool bactive) = 0;

    virtual bool GetPortSetServiceIDs(const QString &nic_name,const QString *setId,QStringList &svr_list) = 0;

    virtual int IsServiceIPv4EntityCfgMethodSetupDhcp(const QString *setId,const QString &svrId) = 0;

    virtual bool SetServiceIPv4EntityCfgMethod2Dhcp(const QString *setId,const QString &svrId) = 0;

    virtual int IsServiceExsitSetupDnses(const QString *setId,const QString &svrId) = 0;

    virtual bool CopyServiceSetupDnses(const QString *setId,const QString &svrId,QStringList &dns_list) = 0;

    virtual bool SetServiceSetupDnses(const QString *setId,const QString &svrId,const QStringList *dns_list) = 0;

    virtual bool IpRenew(const QString &svrId) = 0;

    virtual bool GetAllSets(QStringList &sets) = 0;
    virtual bool GetCurrentSet(QString &curset) = 0;
    virtual bool SetCurrentSet(const QString &new_curset) = 0;
    virtual bool SetIdtoSetName(const QString setid,QString &setname) = 0;

    virtual bool CreateService(const QString &nic_name,const QString *setId,QString &svrId) = 0;
    virtual bool ServiceIdtoServiceName(const QString &svrid,QString &svrname) = 0;
};

Q_DECLARE_INTERFACE(INetApiInterface,"com.dragonflow.INetApiPlugin.INetApiInterface/1.0")
#endif // INETAPIINTERFACE_H
