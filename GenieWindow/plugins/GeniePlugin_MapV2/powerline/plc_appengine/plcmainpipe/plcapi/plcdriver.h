#ifndef PLCDRIVER_H
#define PLCDRIVER_H

#include "plcdriverinterface.h"
#include <QString>

class QLibrary;

class PLCDriver
{
public:
    explicit PLCDriver(QLibrary *libDriver);
    ~PLCDriver();
public:
    QString GetLibraryPathName() const;
private:
    void ResolveSymbols();
private:
    QLibrary *m_libDriver;

private:
    typedef BOOL (*PLCDiscovery_t)(int *, PLC_DiscoveryDeviceList *);

    typedef BOOL (*PLCGetLocalDevice_t)(PLC_MACAddress);
    typedef BOOL (*PLCGetDeviceModelName_t)(PLC_MACAddress , char *);
    typedef BOOL (*PLCGetDeviceFWVersion_t)(PLC_MACAddress , char *);
    typedef BOOL (*PLCGetDeviceName_t)(PLC_MACAddress , char *);
    typedef BOOL (*PLCGetNetworkType_t)(PLC_MACAddress , char *);
    typedef BOOL (*PLCGetDeviceLedStatus_t)(PLC_MACAddress , int *);
    typedef BOOL (*PLCGetDeviceLinkRate_t)(PLC_MACAddress , int *, PLC_DeviceLinkRateTable *);
    typedef BOOL (*PLCGetDeviceQoSStatus_t)(PLC_MACAddress , PLC_DeviceQoSStatus *);
    typedef BOOL (*PLCGetDeviceForwardtable_t)(PLC_MACAddress , int *, PLC_DeviceForwardTableList *);
    typedef BOOL (*PLCGetDeviceFWUpgradeVersion_t)(PLC_MACAddress , char* );
    typedef BOOL (*PLCGetDeviceFWUpgradeRegion_t)(PLC_MACAddress , char* );

    typedef BOOL (*PLCSetDeviceName_t)(PLC_MACAddress , char []);
    typedef BOOL (*PLCSetDeviceLedStatus_t)(PLC_MACAddress , int );
    typedef int	(*PLCSetDeviceSecurityKey_t)( PLC_MACAddress , char [], char []);
    typedef BOOL (*PLCSetDeviceQoS_t)(PLC_MACAddress , PLC_DeviceQoSStatus* );
    typedef BOOL (*PLCSetDeviceResetDefault_t)(PLC_MACAddress );
    typedef BOOL (*PLCCheckDevicePassword_t)(PLC_MACAddress, char []);

public:
#ifndef DUMP_LAYER2_API
    PLCDiscovery_t PLCDiscovery;

    PLCGetLocalDevice_t PLCGetLocalDevice;
    PLCGetDeviceModelName_t PLCGetDeviceModelName;
    PLCGetDeviceFWVersion_t PLCGetDeviceFWVersion;
    PLCGetDeviceName_t PLCGetDeviceName;
    PLCGetNetworkType_t PLCGetNetworkType;
    PLCGetDeviceLedStatus_t PLCGetDeviceLedStatus;
    PLCGetDeviceLinkRate_t PLCGetDeviceLinkRate;
    PLCGetDeviceQoSStatus_t PLCGetDeviceQoSStatus;
    PLCGetDeviceForwardtable_t PLCGetDeviceForwardtable;
    PLCGetDeviceFWUpgradeVersion_t PLCGetDeviceFWUpgradeVersion;
    PLCGetDeviceFWUpgradeRegion_t PLCGetDeviceFWUpgradeRegion;

    PLCSetDeviceName_t PLCSetDeviceName;
    PLCSetDeviceLedStatus_t PLCSetDeviceLedStatus;
    PLCSetDeviceSecurityKey_t PLCSetDeviceSecurityKey;
    PLCSetDeviceQoS_t PLCSetDeviceQoS;
    PLCSetDeviceResetDefault_t PLCSetDeviceResetDefault;
    PLCCheckDevicePassword_t PLCCheckDevicePassword;

#else
private:
    PLCDiscovery_t PLCDiscovery_f;

    PLCGetLocalDevice_t PLCGetLocalDevice_f;
    PLCGetDeviceModelName_t PLCGetDeviceModelName_f;
    PLCGetDeviceFWVersion_t PLCGetDeviceFWVersion_f;
    PLCGetDeviceName_t PLCGetDeviceName_f;
    PLCGetNetworkType_t PLCGetNetworkType_f;
    PLCGetDeviceLedStatus_t PLCGetDeviceLedStatus_f;
    PLCGetDeviceLinkRate_t PLCGetDeviceLinkRate_f;
    PLCGetDeviceQoSStatus_t PLCGetDeviceQoSStatus_f;
    PLCGetDeviceForwardtable_t PLCGetDeviceForwardtable_f;
    PLCGetDeviceFWUpgradeVersion_t PLCGetDeviceFWUpgradeVersion_f;
    PLCGetDeviceFWUpgradeRegion_t PLCGetDeviceFWUpgradeRegion_f;

    PLCSetDeviceName_t PLCSetDeviceName_f;
    PLCSetDeviceLedStatus_t PLCSetDeviceLedStatus_f;
    PLCSetDeviceSecurityKey_t PLCSetDeviceSecurityKey_f;
    PLCSetDeviceQoS_t PLCSetDeviceQoS_f;
    PLCSetDeviceResetDefault_t PLCSetDeviceResetDefault_f;
    PLCCheckDevicePassword_t PLCCheckDevicePassword_f;

    void dumpMsg(const QString &msg) const;

public:
    BOOL PLCDiscovery(int *count, PLC_DiscoveryDeviceList *devList);

    BOOL PLCGetLocalDevice(PLC_MACAddress mac);
    BOOL PLCGetDeviceModelName(PLC_MACAddress mac, char *model);
    BOOL PLCGetDeviceFWVersion(PLC_MACAddress mac, char *Fwversion);
    BOOL PLCGetDeviceName(PLC_MACAddress mac, char *devName);
    BOOL PLCGetNetworkType(PLC_MACAddress mac, char *netType);
    BOOL PLCGetDeviceLedStatus(PLC_MACAddress mac, int *ledStatus);
    BOOL PLCGetDeviceLinkRate(PLC_MACAddress mac, int *count, PLC_DeviceLinkRateTable *linkRateTable);
    BOOL PLCGetDeviceQoSStatus(PLC_MACAddress mac, PLC_DeviceQoSStatus *qosStatus);
    BOOL PLCGetDeviceForwardtable(PLC_MACAddress mac, int *count, PLC_DeviceForwardTableList *forwardTable);
    BOOL PLCGetDeviceFWUpgradeVersion(PLC_MACAddress mac, char* FwUpVersion);
    BOOL PLCGetDeviceFWUpgradeRegion(PLC_MACAddress mac, char* FwUpRegion);

    BOOL PLCSetDeviceName(PLC_MACAddress mac, char devName[]);
    BOOL PLCSetDeviceLedStatus(PLC_MACAddress mac, int ledStatus);
    BOOL PLCSetDeviceSecurityKey(PLC_MACAddress , char key[], char pwd[]);
    BOOL PLCSetDeviceQoS(PLC_MACAddress mac, PLC_DeviceQoSStatus* qos);
    BOOL PLCSetDeviceResetDefault(PLC_MACAddress mac);
    BOOL PLCCheckDevicePassword(PLC_MACAddress mac, char pwd[]);

#endif

};

#endif // PLCDRIVER_H
