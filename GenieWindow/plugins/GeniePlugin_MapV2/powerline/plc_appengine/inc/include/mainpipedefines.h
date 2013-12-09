#ifndef MAINPIPEDEFINES_H
#define MAINPIPEDEFINES_H
#include <QString>

#define SOAP_NOT_INITIALIZED    -2
#define SOAP_UNKNOW_ERROR       -1
#define SOAP_OK      0
#define SOAP_REBOOT_REQUIRED     1
#define SOAP_FUN_NOT_SUPPORT     2
#define SOAP_INVALID_ACTION      3
#define SOAP_INVALID_ARGS        4
#define SOAP_ACTION_FAILED       5

#define SOAP_FAILED      6
#define SOAP_TIMEOUT     7
#define SOAP_ABORT       8

typedef struct AttachDeviceEntry_tag
{
    QString m_devIP;
    QString m_devName;
    QString m_devMac;
    bool m_isWired;
    int m_linkSpeed;
    int m_signalStrength;
    bool m_allowOrBlock;

    bool operator== (const AttachDeviceEntry_tag &right) const
    {
        return m_devIP == right.m_devIP &&
                m_devName == right.m_devName &&
                m_devMac == right.m_devMac &&
                m_isWired == right.m_isWired &&
                m_linkSpeed == right.m_linkSpeed &&
                m_signalStrength == right.m_signalStrength &&
                m_allowOrBlock == right.m_allowOrBlock;
    }

    QString toString() const
    {
        QString tmp;
        QString str("[");
        str += m_devIP;
        str += ",";
        str += m_devName;
        str += ",";
        str += m_devMac;
        str += ",";
        str += (m_isWired ? "Wire" : "Wireless");
        str += ",";
        str += tmp.setNum(m_linkSpeed);
        str += "Mbps,";
        str += tmp.setNum(m_signalStrength);
        str += "%,";
        str += (m_allowOrBlock ? "Allow" : "Blocked");
        str += "]";

        return str;
    }

}AttachDeviceEntry;

typedef struct WLANInfo_tag
{
    enum {
        WPS_CONFIGURED,
        WPS_NOT_CONFIGURED,
        WPS_STATE_UNKNOWN
    };

    bool m_enabled;
    bool m_ssidBroadcast;
    bool m_statusUp;
    int m_wpsConfigured;
    QString m_ssid;
    QString m_region;
    QString m_channel;
    QString m_wirelessMode;
    QString m_basicEncryptionModes;
    QString m_wepAuthType ;
    QString m_wepKey;
    QString m_wpaEncryptionModes;
    QString m_wlanMACAddress;
    QString m_wlanPassphrase;

    QString toString() const
    {
        QString str("(");
        str += (m_enabled ? "Enable" : "Disable");
        str += (m_ssidBroadcast ? "\nBroadcast" : "\nNot Broadcast");
        str += (m_statusUp ? "\nStatus Up" : "\nStatus Down");
        str += (m_wpsConfigured == WPS_CONFIGURED ? "\nWPS Configured" : (m_wpsConfigured == WPS_NOT_CONFIGURED ? "\nWPS Not Configured" : "\nWPS State Unknown"));
        str += "\nSSID:";
        str += m_ssid;
        str += "\nRegion:";
        str += m_region;
        str += "\nChannel:";
        str += m_channel;
        str += "\nWireless mode:";
        str += m_wirelessMode;
        str += "\nBasic Encryption Mode:";
        str += m_basicEncryptionModes;
        str += "\nWEP Auth Type:";
        str += m_wepAuthType;
        str += "\nWEP Key:";
        str += m_wepKey;
        str += "\nWPA Encryption Mode:";
        str += m_wpaEncryptionModes;
        str += "\nPassphrase:";
        str += m_wlanPassphrase;
        str += "\nWLAN MAC:";
        str += m_wlanMACAddress;
        str += ")";

        return str;
    }

}WLANInfo;

#endif // MAINPIPEDEFINES_H
