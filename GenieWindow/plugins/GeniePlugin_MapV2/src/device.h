#ifndef DEVICE_H
#define DEVICE_H

#include <QMap>
#include <QVariant>
#include <QList>


enum DEVICE_PROPERTY
{
    DP_NAME,
    DP_TYPE,//int
    DP_CUSTOMNAME,
    DP_CUSTOMTYPE,//int
    DP_CUSTOMWIRED,
    DP_PHYSICALADDR,
    DP_IP,
    DP_OS,
    DP_ISLOCALHOST,
    DP_ISDEFAULTGATEWAY,
    DP_VMFLAG,//virtual machine or not,bool
    DP_STATUS,//online or not,bool
    DP_ISWIRED,//wireless or wired physical link ,bool


    DP_FIRMWARE,
    DP_UPNP_DEVICETYPE,
    DP_UPNP_DESCRIPTION,
    DP_UPNP_MODEL,
    DP_UPNP_MANUFACTURER,
    DP_UPNP_MANUFACTURERURL,
    DP_WIRELESS_SIGNALSTRENGTH,
    DP_ATTACHDEVICE_LINKRATE,
    DP_ATTACHDEVICE_BLOCKED,
    DP_CURRENTSETTING_MODEL,
#ifdef USE_RDP	
    DP_RDP_ENABLE,
#endif
#ifdef USE_PUTTY
    DP_PUTTY_ENABLE,
    DP_PUTTY_HINT,
    DP_WINSCP_HINT,
#endif
    //add by caiyujun
    DP_TURBOFLAG,
    //add end
    DP_COUNT
};

enum DEVICE_MENUITEM
{
    DMI_SETUP,
    DMI_VIEWDETAIL,
    DMI_BLOCK_ENABLE,
    DMI_BLOCK_DISABLE,
#ifdef USE_RDP
    DMI_RDT,
#endif
#ifdef USE_PUTTY
    DMI_PUTTY,
    DMI_WINSCP,
#endif
    DMI_COUNT,
    //add by caiyujun
    DMI_SEND_FILE,
    DMI_SEND_FOLDER,
    DMI_EXTENDER_MAP,
	DMI_NETWORK_MAP
    //add end

};


class Device
{
public:
    Device();
    Device(const Device &dev);
protected:
    QMap<int,QVariant> m_properties;

public:
    Device & operator = (const Device &dev);
    const QVariant operator [](int property) const;
    QVariant &operator [](int property);

    void clear();

    int parseType() const;
    int parseWiredInCB() const;
    int parseWiredInShowing() const;
public:
    QList<int> getDetailPropertyList(int devType) const;
    bool deviceIpShouldLink(int devType) const;
    QString getDetailPropertyLabel(int property) const;
    QString getDetailPropertyContent(int property) const;

    QList<int> getDeviceMenuItems() const;
    //add by caiyujun
    bool isDevice(QString strIp);
    //add end

};

Q_DECLARE_METATYPE(Device)

#endif // DEVICE_H
