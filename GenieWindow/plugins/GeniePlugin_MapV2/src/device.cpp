#include "device.h"
#include "mapdevicetype.h"
#include "mapuitilities.h"
#include "puttyfuncs.h"

#include <QtGlobal>
#include "topology.h"

/***********************************************************/
//the content of the arrays below must be in
//the DEVICE_PROPERTY enum
extern quint32 sMapFlags;
static const int ddv_localhost_entries[] =
{
    DP_NAME,
    DP_IP,
    DP_STATUS,
    DP_WIRELESS_SIGNALSTRENGTH,
    DP_ATTACHDEVICE_LINKRATE,
    DP_PHYSICALADDR,
    DP_OS,
    DP_COUNT//Terminator
};

static const int ddv_networkdev_entries[] =
{
    DP_NAME,
    DP_TYPE,
    DP_ISWIRED,
    DP_IP,
    DP_STATUS,
    DP_WIRELESS_SIGNALSTRENGTH,
    DP_ATTACHDEVICE_LINKRATE,
    DP_PHYSICALADDR,
    DP_COUNT//Terminator
};

static const int ddv_windowspc_entries[] =
{
    DP_NAME,
    DP_TYPE,
    DP_ISWIRED,
    DP_IP,
    DP_STATUS,
    DP_WIRELESS_SIGNALSTRENGTH,
    DP_ATTACHDEVICE_LINKRATE,
    DP_PHYSICALADDR,
    DP_OS,
    DP_COUNT//Terminator
};

static const int ddv_mediadevice_entries[] =
{
    DP_NAME,
    DP_TYPE,
    DP_ISWIRED,
    DP_IP,
    DP_STATUS,
    DP_WIRELESS_SIGNALSTRENGTH,
    DP_ATTACHDEVICE_LINKRATE,
    DP_PHYSICALADDR,
    DP_UPNP_DESCRIPTION,
    DP_UPNP_MODEL,
    DP_UPNP_MANUFACTURER,
    DP_UPNP_MANUFACTURERURL,
    DP_COUNT//Terminator
};

static const int ddv_defaultgateway_entries[] =
{
    DP_CURRENTSETTING_MODEL,
    DP_IP,
    DP_STATUS,
    DP_PHYSICALADDR,
    DP_FIRMWARE,
    DP_COUNT//Terminator
};

/**********************************************************/


//the indics of the array below must be in
//the DEVICE_TYPE enum
static const int* ddv_device_details[MDT_DEVICETYPE_COUNT] =
{
    NULL,//for internet, just a dummy
    ddv_networkdev_entries,
    ddv_windowspc_entries,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    ddv_mediadevice_entries,
    ddv_mediadevice_entries,
    NULL,
    NULL,
    ddv_mediadevice_entries,
    ddv_mediadevice_entries,
    ddv_mediadevice_entries
};

static const int ddv_ipcanlink_devices[] =
{
    MDT_ROUTER,
    MDT_CAMERA,
    MDT_SCANNER,
    MDT_PRINTER,
    MDT_MEDIADEV,
    MDT_STORAGE,
    MDT_SWITCH,
    MDT_CABLESTB,
    MDT_OTHERSTB,
    MDT_SATELLITESTB,
    MDT_SLINGBOX,
    MDT_REPEATER,
    MDT_BRIDGE,
    MDT_POWERLINE,
    MDT_DEVICETYPE_COUNT
};

//the indics of the array below must be in
//the DEVICE_PROPERTY enum
static const char* ddv_deviceproperty_texts[DP_COUNT] =
{
    "Name:",
    "Type:",
    "CustomName:",
    "CustomType:",
    "MAC Address:",
    "IP Address:",
    "OS:",
    "Local Host:",
    "Default Gateway",
    "Virtual Machine:",
    "Status:",
    "Is Wired:",
    "Firmware Version:",
    "Upnp Device Type:",
    "Upnp Description:",
    "Upnp Model:",
    "Upnp Manufacturer:",
    "Upnp Manufacturer URL:",
    "Signal Strenth:",
    "Link Rate:",
    "Router Model:"
};

Device::Device()
{
}

Device::Device(const Device &dev)
{
    m_properties = dev.m_properties;
}

Device & Device::operator = (const Device &dev)
{
    if(this != &dev)
    {
        m_properties = dev.m_properties;
    }

    return *this;
}

const QVariant Device::operator [](int property) const
{
    Q_ASSERT( (property >= 0) && (property < DP_COUNT) );

    return m_properties[property];
}

QVariant & Device::operator [](int property)
{
    Q_ASSERT( (property >= 0) && (property < DP_COUNT) );

    return m_properties[property];
}


void Device::clear()
{
    m_properties.clear();
}

int Device::parseType() const
{
    bool bok = false;
    int deviceType = m_properties[DP_CUSTOMTYPE].toInt(&bok);

    if(!bok || (deviceType == MDT_DEVICETYPE_COUNT) ||
       (deviceType == MDT_NETWORKDEV))
    {
        deviceType = m_properties[DP_TYPE].toInt();
    }

    return deviceType;
}

int Device::parseWiredInCB() const
{  
    bool bcustomok=m_properties.contains(DP_CUSTOMWIRED);
//    bool btypeok = m_properties.contains(DP_ISWIRED);
    if(bcustomok )
    {
        int wiredType=m_properties[DP_CUSTOMWIRED].toInt();
		if(wiredType<0 || wiredType>=MCT_COUNT)
		{
			wiredType=MCT_AUTO;
		}
        return wiredType;
    }
    return MCT_AUTO;
}

int Device::parseWiredInShowing() const
{
    int temp_type=parseWiredInCB();
    temp_type=(temp_type==MCT_WIRED || temp_type==MCT_WIRELESS)?
                              (temp_type):
                              m_properties[DP_ISWIRED].toBool()?MCT_WIRED:MCT_WIRELESS;
    return temp_type;
}

QList<int> Device::getDetailPropertyList(int devType) const
{
    Q_ASSERT(devType >=0 && devType < MDT_DEVICETYPE_COUNT);

    const int *dps = NULL;

    if(m_properties[DP_ISLOCALHOST].toBool())
    {
        dps = ddv_localhost_entries;
    }
    else if(m_properties[DP_ISDEFAULTGATEWAY].toBool())
    {
        dps = ddv_defaultgateway_entries;
    }
    else
    {
        dps = ddv_device_details[devType];
    }

    dps = (dps ? dps : ddv_device_details[MDT_NETWORKDEV]);

    Q_ASSERT (dps);

    QList<int> proList;
    int idx = 0;

    while(dps[idx] != DP_COUNT)
    {
        if(((dps[idx] == DP_WIRELESS_SIGNALSTRENGTH) &&
            (!m_properties.contains(DP_WIRELESS_SIGNALSTRENGTH) ||
             m_properties[DP_WIRELESS_SIGNALSTRENGTH].toInt() == -1)) ||
           ((DP_ATTACHDEVICE_LINKRATE == dps[idx]) && (m_properties[DP_ISWIRED].toBool() || m_properties[DP_ATTACHDEVICE_LINKRATE].toString().isEmpty())))
        {
            ++idx;
            continue;
        }

        proList.append(dps[idx++]);
    }

    return proList;
}

bool Device::deviceIpShouldLink(int devType) const
{
    Q_ASSERT(devType >=0 && devType < MDT_DEVICETYPE_COUNT);

    int idx = 0;

    while((ddv_ipcanlink_devices[idx] != MDT_DEVICETYPE_COUNT) &&
          (ddv_ipcanlink_devices[idx] != devType))
    {
        ++idx;
    }

    return (ddv_ipcanlink_devices[idx] != MDT_DEVICETYPE_COUNT);
}

QString Device::getDetailPropertyLabel(int property) const
{
    Q_ASSERT (property >= 0 && property < DP_COUNT);

    return QString(ddv_deviceproperty_texts[property] ?
                   ddv_deviceproperty_texts[property] : "Undefine:");
}

QString Device::getDetailPropertyContent(int property) const
{
    Q_ASSERT (property >= 0 && property < DP_COUNT);

    if(property == DP_STATUS)
    {
        return (m_properties[property].toBool() ? "On Line" : "Off Line");
    }
    else if((property == DP_TYPE) || (property == DP_CUSTOMTYPE))
    {
        return getDeviceTypeText(m_properties[property].toInt());
    }
    else if(property == DP_WIRELESS_SIGNALSTRENGTH)
    {
        return m_properties[property].toString() + "%";
    }
    else if(property == DP_ATTACHDEVICE_LINKRATE)
    {
        QString linkRate = m_properties[property].toString();
        return (linkRate.isEmpty() ? linkRate : (linkRate + "Mbps"));
    }

    return m_properties[property].toString();
}

QList<int> Device::getDeviceMenuItems() const
{
    int devType = m_properties[DP_CUSTOMTYPE].toInt();
    devType = ((devType != MDT_DEVICETYPE_COUNT)&&(devType!=MDT_NETWORKDEV) )
            ? devType : m_properties[DP_TYPE].toInt();

    QList<int> items;
    if(deviceIpShouldLink(devType))
    {
        items << DMI_SETUP;
    }

    if(devType != MDT_INTERNET)
    {
        items << DMI_VIEWDETAIL;
    }

    int blocked=m_properties[DP_ATTACHDEVICE_BLOCKED].toInt();
    bool isself=m_properties[DP_ISLOCALHOST].toBool();
    if(!isself)
    {
        if(blocked==1)
        {
            items << DMI_BLOCK_ENABLE;
        }
        else if(blocked==0)
        {
            items << DMI_BLOCK_DISABLE;
        }
    }
#ifdef USE_RDP

    if(sMapFlags & MF_USE_RDP_SCAN)
    {
        bool online=m_properties[DP_STATUS].toBool();
        if(online && m_properties[DP_RDP_ENABLE].toBool() && !m_properties[DP_ISLOCALHOST].toBool())
        {
            items << DMI_RDT;
        }
    }
#endif
#ifdef USE_PUTTY
    if(sMapFlags & MF_USE_PUTTY_SCAN)
    {
        bool online=m_properties[DP_STATUS].toBool();
        if(online && !m_properties[DP_ISDEFAULTGATEWAY].toBool()&& m_properties[DP_PUTTY_ENABLE].toBool())
        {
            if(puttyExeExists())
            {
                items << DMI_PUTTY;
            }
            if(winscpExeExists())
            {
                items << DMI_WINSCP;
            }
        }
    }
#endif


    return items;
}

//add by caiyujun
bool Device::isDevice(QString strIp)
{
    if (m_properties[DP_IP].toString() == strIp)
    {
        return true;
    }
    else
    {
        return false;
    }
}
//add end
