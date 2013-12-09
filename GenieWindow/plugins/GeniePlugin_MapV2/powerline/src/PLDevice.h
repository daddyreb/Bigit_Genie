#ifndef DEVICE_H
#define DEVICE_H
#include <QMap>
#include <QVariant>
#include <QList>
#include "plctransaction.h"
#include "plctopologydata.h"

enum DEVICE_PROPERTY
{


    GDP_NAME,
    GDP_TYPE,//int
    GDP_CUSTOMNAME,
    GDP_CUSTOMTYPE,//int
    GDP_PHYSICALADDR,
    GDP_IP,
    GDP_OS,
    GDP_ISLOCALHOST,
    GDP_ISDEFAULTGATEWAY,
    GDP_VMFLAG,//virtual machine or not,bool
    GDP_STATUS,//online or not,bool
    GDP_ISWIRED,//wireless or wired physical link ,bool
    GDP_FIRMWARE,
    GDP_UPNP_DEVICETYPE,
    GDP_UPNP_DESCRIPTION,
    GDP_UPNP_MODEL,
    GDP_UPNP_MANUFACTURER,
    GDP_UPNP_MANUFACTURERURL,
    GDP_WIRELESS_SIGNALSTRENGTH,
    GDP_ATTACHDEVICE_LINKRATE,
    GDP_ATTACHDEVICE_BLOCKED,
    GDP_CURRENTSETTING_MODEL,

    GDP_ATTACH_DEV,
    GDP_VERSION,
    GDP_LED_STATUS,
    GDP_MODEL_NAME,
    GDP_NETWORK_TYPE,
    GDP_SUPPORT_USB,
	GDP_SUPPORT_WIRELESS,

    GDP_COUNT
};

enum DEVICE_MENUITEM
{
    GDMI_SETUP,
    GDMI_VIEWDETAIL,
    GDMI_BLOCK_ENABLE,
    GDMI_BLOCK_DISABLE,

    //powerline
    GDMI_DEVICENAME,
    GDMI_TURNLED_OFF,
    GDMI_SECURITY,
    GDMI_FACTORY_RESET,
	GDMI_WIRELESS_SET,
	GDMI_UPDATE,
	GDMI_HTTPIP,
    GDMI_COUNT
};
class PLDevice
{
public:
    PLDevice();
    PLDevice(const PLDevice &dev);
protected:
    QMap<int,QVariant> m_properties;
public:
    //QList<PLCNodeData *>    m_linkPlcNodes;    //powerline设备
    //QList<QPair<qint16,qint16> > m_linkPlcNodeLinkRates;

    QStringList     m_parent;
    QStringList     m_son;
	//QString			m_linkRate_send;
	//QString			m_linkRate_recv;

	QList<AttachDeviceEntry> m_attachDeviceList;
	QList<QPair<QString,QString> > m_attachUsbDevice;//<usbtype,usbdescription>
	WLANInfo m_wlanInfo;
	int			m_nID;			//身份标误识

	QMap<QString, QPair<qint16,qint16> >   m_linkRate;
public:
    PLDevice & operator = (const PLDevice &dev);
    const QVariant operator [](int property) const;
    QVariant &operator [](int property);

    void clear();

    int parseType() const;
public:
    QList<int> getDetailPropertyList(int devType) const;
    bool deviceIpShouldLink(int devType) const;
    QString getDetailPropertyLabel(int property) const;
    QString getDetailPropertyContent(int property) const;

    QList<int> getDeviceMenuItems() const;

};

Q_DECLARE_METATYPE(PLDevice)

#endif // DEVICE_H
