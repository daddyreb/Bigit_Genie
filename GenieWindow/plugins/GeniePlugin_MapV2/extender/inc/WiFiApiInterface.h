#ifndef WIFIAPIINTERFACE_H
#define WIFIAPIINTERFACE_H

#include <QString>
#include <QList>
#include <QMap>
#include <QStringList>


class WiFiApiInterface
{
public:
    virtual ~WiFiApiInterface() { }
public:
    virtual bool IsWlanNicPowerOn(const QString &nicname,bool *err = NULL) = 0;
    virtual bool SetWlanNicPowerOn(const QString &nicname,bool bon) = 0;

    virtual bool IsWlanNicActive(const QString &nicname,bool *err = NULL) = 0;

    virtual int  RunningStateWlanNicSignalLevel(const QString &nicname) = 0;

    virtual bool IsWLanNicConnected(const QString &nicname) = 0;

    virtual bool ScanForNetworks(const QString &nicname,QList<QMap<QString,QString> > & result) = 0;
    virtual bool AssociateToNetwork(const QString &nicname,const QString & ssid,const QString & pwd) = 0;

    virtual void Disassociate(const QString &nicname) = 0;

    virtual bool AllWLanInterfaces(QStringList & nic_list) = 0;

    virtual bool GetWLanInterfaceCurSsid(const QString &nicname,QString &ssid) = 0;
};

Q_DECLARE_INTERFACE(WiFiApiInterface,"com.dragonflow.WiFiApiPlugin.WiFiApiInterface/1.0")

#endif // WIFIAPIINTERFACE_H
