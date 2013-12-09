#ifndef HELPERROUTINES_H
#define HELPERROUTINES_H

#include <QString>

#include "plcdriverinterface.h"

QString PLCDeviceMacAddr2String(BYTE DeviceMAC[PLC_MAC_ADDRESS_SIZE]);

BYTE* PLCDeviceMacAddrFromString(const QString &strDevMac, BYTE DeviceMAC[PLC_MAC_ADDRESS_SIZE]);

bool IsMacAddrValid(const QString &mac);

QString NormalizeValidMacAddrFormat(const QString &mac);

void Add2MacAddrValue(const BYTE DeviceMAC[], BYTE ResultMac[], int value);

#endif // HELPERROUTINES_H
