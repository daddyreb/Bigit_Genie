#include "helperroutines.h"
#include <stdio.h>
#include <QStringList>

QString PLCDeviceMacAddr2String(BYTE DeviceMAC[PLC_MAC_ADDRESS_SIZE])
{
    char macStrBuf[24];

    sprintf(macStrBuf, "%02X:%02X:%02X:%02X:%02X:%02X"
            , DeviceMAC[0], DeviceMAC[1]
            , DeviceMAC[2], DeviceMAC[3]
            , DeviceMAC[4], DeviceMAC[5]);

    return QString(macStrBuf);
}

BYTE* PLCDeviceMacAddrFromString(const QString &strDevMac, BYTE DeviceMAC[PLC_MAC_ADDRESS_SIZE])
{
    QStringList macParts = strDevMac.split(":");
    int partsCount = macParts.count();
    for(int idx = 0 ; (idx < partsCount) && (idx < PLC_MAC_ADDRESS_SIZE) ; ++idx){
        DeviceMAC[idx] = (BYTE)macParts[idx].toUShort(0, 16);
    }

    return DeviceMAC;
}

bool IsMacAddrValid(const QString &mac)
{
    //valid mac,such as: 00:11:22:33:44:FF,00-11-22-33-44-AE,4455667788EE
    QRegExp macPattern("^(\\d|[a-f]|[A-F]){2}(((-((\\d|[a-f]|[A-F]){2})){5})|((:((\\d|[a-f]|[A-F]){2})){5})|(((\\d|[a-f]|[A-F]){2}){5}))$");
    return macPattern.exactMatch(mac);
}

QString NormalizeValidMacAddrFormat(const QString &mac)
{
    //paramter mac must be a valid MacAddr
    if(!IsMacAddrValid(mac)){
        return QString();
    }

    if(mac.contains(":")){
        return mac.toUpper();
    }else if(mac.contains("-")){
        QString normalMac = mac;
        normalMac.replace("-", ":");
        return normalMac.toUpper();
    }else{
        QString normalMac = mac.left(2);
        for(int idx = 1 ; idx <= 5 ; ++idx){
            normalMac += ":";
            normalMac += mac.mid(2*idx,2);
        }
        return normalMac.toUpper();
    }
}


/*detect the machine byte order
,little endian return none zero otherwise return zero*/
static bool little_endian()
{
    unsigned long magic = 0x12345678UL;
    char * p = (char*) &magic;
    return (p[0] == 0x78);
}

static quint64 bytes_swap(quint64 source)
{
    return 0
            | ((source & 0x00000000000000ffULL) << 56)
            | ((source & 0x000000000000ff00ULL) << 40)
            | ((source & 0x0000000000ff0000ULL) << 24)
            | ((source & 0x00000000ff000000ULL) << 8)
            | ((source & 0x000000ff00000000ULL) >> 8)
            | ((source & 0x0000ff0000000000ULL) >> 24)
            | ((source & 0x00ff000000000000ULL) >> 40)
            | ((source & 0xff00000000000000ULL) >> 56);
}

void Add2MacAddrValue(const BYTE DeviceMAC[PLC_MAC_ADDRESS_SIZE], BYTE ResultMac[PLC_MAC_ADDRESS_SIZE], int value)
{
    qint64 macValue = 0;
    BYTE *pByte = (BYTE*)&macValue;

    if(little_endian()){
        for(int i = 0 ; i < PLC_MAC_ADDRESS_SIZE; ++i){
            pByte[i] = DeviceMAC[PLC_MAC_ADDRESS_SIZE - i - 1];
        }
        macValue += value;
        macValue = bytes_swap(macValue);
    }else{
        for(int i = 0 ; i < PLC_MAC_ADDRESS_SIZE; ++i){
            pByte[i + 2] = DeviceMAC[i];
        }
        macValue += value;
    }

    memcpy(ResultMac, pByte + 2, PLC_MAC_ADDRESS_SIZE);
}

