#include "PLDevice.h"
#include "PowerlineDefine.h"
#include "Powerline_mapuitilities.h"


#include <QtGlobal>

/***********************************************************/
//the content of the arrays below must be in
//the DEVICE_PROPERTY enum
static const int ddv_hub_entries[] =
{
	GDP_NAME,
	GDP_TYPE
};
//static const int ddv_localhost_entries[] =
//{
//    GDP_NAME,
//    GDP_IP,
//    GDP_STATUS,
//    GDP_WIRELESS_SIGNALSTRENGTH,
//    GDP_ATTACHDEVICE_LINKRATE,
//    GDP_PHYSICALADDR,
//    GDP_OS,
//    GDP_COUNT//Terminator
//};
//
//static const int ddv_networkdev_entries[] =
//{
//    GDP_NAME,
//    GDP_TYPE,
//    GDP_IP,
//    GDP_STATUS,
//    GDP_WIRELESS_SIGNALSTRENGTH,
//    GDP_ATTACHDEVICE_LINKRATE,
//    GDP_PHYSICALADDR,
//    GDP_COUNT//Terminator
//};
//
//static const int ddv_windowspc_entries[] =
//{
//    GDP_NAME,
//    GDP_TYPE,
//    GDP_IP,
//    GDP_STATUS,
//    GDP_WIRELESS_SIGNALSTRENGTH,
//    GDP_ATTACHDEVICE_LINKRATE,
//    GDP_PHYSICALADDR,
//    GDP_OS,
//    GDP_COUNT//Terminator
//};
//
//static const int ddv_mediadevice_entries[] =
//{
//    GDP_NAME,
//    GDP_TYPE,
//    GDP_IP,
//    GDP_STATUS,
//    GDP_WIRELESS_SIGNALSTRENGTH,
//    GDP_ATTACHDEVICE_LINKRATE,
//    GDP_PHYSICALADDR,
//    GDP_UPNP_DESCRIPTION,
//    GDP_UPNP_MODEL,
//    GDP_UPNP_MANUFACTURER,
//    GDP_UPNP_MANUFACTURERURL,
//    GDP_COUNT//Terminator
//};

//static const int ddv_defaultgateway_entries[] =
//{
//    GDP_CURRENTSETTING_MODEL,
//    GDP_IP,
//    GDP_STATUS,
//    GDP_PHYSICALADDR,
//    GDP_FIRMWARE,
//    GDP_COUNT//Terminator
//};

/**********************************************************/


//the indics of the array below must be in
//the DEVICE_TYPE enum
static const int* ddv_device_details[GDT_DEVICETYPE_COUNT] =
{
 
	ddv_hub_entries
    /*ddv_mediadevice_entries,
    ddv_mediadevice_entries,
    ddv_mediadevice_entries*/
};

static const int ddv_ipcanlink_devices[] =
{
 /*   GDT_ROUTER,
    GDT_CAMERA,
    GDT_SCANNER,
    GDT_PRINTER,
    GDT_MEDIADEV,
    GDT_STORAGE,
    GDT_SWITCH,
    GDT_CABLESTB,
    GDT_OTHERSTB,
    GDT_SATELLITESTB,
    GDT_SLINGBOX,
    GDT_REPEATER,
    GDT_BRIDGE,*/
    GDT_DEVICETYPE_COUNT
};

//the indics of the array below must be in
//the DEVICE_PROPERTY enum
static const char* ddv_deviceproperty_texts[GDP_COUNT] =
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

PLDevice::PLDevice()
{
	m_nID = -1;
}

PLDevice::PLDevice(const PLDevice &dev)
{
    m_properties = dev.m_properties;
}

PLDevice & PLDevice::operator = (const PLDevice &dev)
{
    if(this != &dev)
    {
        m_properties = dev.m_properties;
    }

    return *this;
}

const QVariant PLDevice::operator [](int property) const
{
    Q_ASSERT( (property >= 0) && (property < GDP_COUNT) );

    return m_properties[property];
}

QVariant & PLDevice::operator [](int property)
{
    Q_ASSERT( (property >= 0) && (property < GDP_COUNT) );

    return m_properties[property];
}


void PLDevice::clear()
{
    m_properties.clear();
}

int PLDevice::parseType() const
{
    bool bok = false;
    int deviceType = m_properties[GDP_CUSTOMTYPE].toInt(&bok);

    if(!bok || (deviceType == GDT_DEVICETYPE_COUNT)/* ||
       (deviceType == GDT_NETWORKDEV)*/)
    {
        deviceType = m_properties[GDP_TYPE].toInt();
    }

    return deviceType;
}

QList<int> PLDevice::getDetailPropertyList(int devType) const
{
    Q_ASSERT(devType >=0 && devType < GDT_DEVICETYPE_COUNT);

    const int *dps = NULL;

    //if(m_properties[GDP_ISLOCALHOST].toBool())
    //{
    //    dps = ddv_localhost_entries;
    //}
    //else if(m_properties[GDP_ISDEFAULTGATEWAY].toBool())
    //{
    //    dps = ddv_defaultgateway_entries;
    //}
    /*else*/
    {
        dps = ddv_device_details[devType];
    }

    /*dps = (dps ? dps : ddv_device_details[GDT_NETWORKDEV]);*/

    Q_ASSERT (dps);

    QList<int> proList;
    int idx = 0;

    while(dps[idx] != GDP_COUNT)
    {
        if(((dps[idx] == GDP_WIRELESS_SIGNALSTRENGTH) &&
            (!m_properties.contains(GDP_WIRELESS_SIGNALSTRENGTH) ||
             m_properties[GDP_WIRELESS_SIGNALSTRENGTH].toInt() == -1)) ||
           ((GDP_ATTACHDEVICE_LINKRATE == dps[idx]) && (m_properties[GDP_ISWIRED].toBool() || m_properties[GDP_ATTACHDEVICE_LINKRATE].toString().isEmpty())))
        {
            ++idx;
            continue;
        }

        proList.append(dps[idx++]);
    }

    return proList;
}

bool PLDevice::deviceIpShouldLink(int devType) const
{
    Q_ASSERT(devType >=0 && devType < GDT_DEVICETYPE_COUNT);

    int idx = 0;

    while((ddv_ipcanlink_devices[idx] != GDT_DEVICETYPE_COUNT) &&
          (ddv_ipcanlink_devices[idx] != devType))
    {
        ++idx;
    }

    return (ddv_ipcanlink_devices[idx] != GDT_DEVICETYPE_COUNT);
}

QString PLDevice::getDetailPropertyLabel(int property) const
{
    Q_ASSERT (property >= 0 && property < GDP_COUNT);

    return QString(ddv_deviceproperty_texts[property] ?
                       ddv_deviceproperty_texts[property] : "Undefine:");
}

QString PLDevice::getDetailPropertyContent(int property) const
{
    Q_ASSERT (property >= 0 && property < GDP_COUNT);

    if(property == GDP_STATUS)
    {
        return (m_properties[property].toBool() ? "On Line" : "Off Line");
    }
    else if((property == GDP_TYPE) || (property == GDP_CUSTOMTYPE))
    {
     /*   return getDeviceTypeText(m_properties[property].toInt());*/
    }
    else if(property == GDP_WIRELESS_SIGNALSTRENGTH)
    {
        return m_properties[property].toString() + "%";
    }
    else if(property == GDP_ATTACHDEVICE_LINKRATE)
    {
        QString linkRate = m_properties[property].toString();
        return (linkRate.isEmpty() ? linkRate : (linkRate + "Mbps"));
    }

    return m_properties[property].toString();
}

QList<int> PLDevice::getDeviceMenuItems() const
{
    int devType = m_properties[GDP_TYPE].toInt();
    devType = ((devType != GDT_DEVICETYPE_COUNT) ? devType : m_properties[GDP_TYPE].toInt());

	QList<int> items;
	QString strDevModel = m_properties[GDP_MODEL_NAME].toString();
	if(strDevModel == POWERLINE_UNKNOWN_DEV)
	{
		items << GDMI_SECURITY;

		return items;
	}


    if(deviceIpShouldLink(devType))
    {
        items << GDMI_SETUP;
    }


    if(devType == GDT_POWERLINE)
    {
        items << GDMI_SECURITY;
        items << GDMI_DEVICENAME;
        items << GDMI_TURNLED_OFF;
        items << GDMI_FACTORY_RESET;
		items << GDMI_WIRELESS_SET;
		items << GDMI_UPDATE;
		items << GDMI_HTTPIP;


        return items;
    }

    //if(devType != GDT_INTERNET)
    //{
    //    items << GDMI_VIEWDETAIL;
    //}


    //int blocked=m_properties[GDP_ATTACHDEVICE_BLOCKED].toInt();
    //bool isself=m_properties[GDP_ISLOCALHOST].toBool();
    //if(!isself)
    //{
    //    if(blocked==1)
    //    {
    //        items << GDMI_BLOCK_ENABLE;
    //    }
    //    else if(blocked==0)
    //    {
    //        items << GDMI_BLOCK_DISABLE;
    //    }
    //}
    return items;
}
