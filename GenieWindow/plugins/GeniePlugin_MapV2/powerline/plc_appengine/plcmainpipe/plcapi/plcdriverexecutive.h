#ifndef PLCDRIVEREXECUTIVE_H
#define PLCDRIVEREXECUTIVE_H


#include "plcdriverinterface.h"
#include "filesystemmonitor.h"
#include <QString>
#include <QMap>
#include <QObject>
#include <QMutex>

class PLCDriverManager;
class MainPipeEventMonitorInterface;

class PLCDriverExecutive : public QObject
{
    Q_OBJECT

public:
    explicit PLCDriverExecutive(const QString &driverDirPath, MainPipeEventMonitorInterface *monitorInf = 0);
    ~PLCDriverExecutive();

private slots:
    void OnNewPLCDriver(const QString &driverPath);
public:
	QString GetPLCDriverPath(const QString &devMac) const;

public:
    BOOL PLCDiscovery(int *DeviceNumber, PLC_DiscoveryDeviceList *DeviceMACAddrList) const;

    BOOL PLCGetLocalDevice(PLC_MACAddress DeviceMACAddress) const;

    BOOL PLCGetDeviceModelName(PLC_MACAddress DeviceMACAddress, char *DeviceModelName) const;

    BOOL PLCGetDeviceFWVersion(PLC_MACAddress DeviceMACAddress, char *DeviceFWVersion) const;

    BOOL PLCGetDeviceName(PLC_MACAddress DeviceMACAddress, char *DeviceName) const;

    BOOL PLCGetNetworkType(PLC_MACAddress DeviceMACAddress, char *DeviceNetworkType) const;

    BOOL PLCGetDeviceLedStatus(PLC_MACAddress DeviceMACAddress, int *DeviceLedStatus) const;

    BOOL PLCGetDeviceLinkRate( PLC_MACAddress DeviceMACAddress, int *NumOfTable, PLC_DeviceLinkRateTable *DeviceLinkRateTable) const;

    BOOL PLCGetDeviceQoSStatus(PLC_MACAddress DeviceMACAddress, PLC_DeviceQoSStatus *QoSStatus) const;

    BOOL PLCGetDeviceForwardtable( PLC_MACAddress DevMACAddr, int *NumOfEntries, PLC_DeviceForwardTableList *ForwardTable) const;

    BOOL PLCGetDeviceFWUpgradeVersion (PLC_MACAddress DeviceMACAddress, char *DeviceFWUpgradeVersion) const;

    BOOL PLCGetDeviceFWUpgradeRegion (PLC_MACAddress DeviceMACAddress, char *DeviceFWUpgradeResion) const;



    BOOL PLCSetDeviceName(PLC_MACAddress DeviceMACAddress, const char DeviceName[]) const;

    BOOL PLCSetDeviceLedStatus(PLC_MACAddress DeviceMACAddress, int DeviceLedStatus) const;

    int PLCSetDeviceSecurityKey( PLC_MACAddress DeviceMACAddress, const char SecurityKey[], const char DevicePassword[]) const; 

    BOOL PLCSetDeviceQoS(PLC_MACAddress DeviceMACAddress, PLC_DeviceQoSStatus* QoSStatus) const;

    BOOL PLCSetDeviceResetDefault(PLC_MACAddress DeviceMACAddress) const;

    BOOL PLCCheckDevicePassword(PLC_MACAddress DeviceMACAddress, const char DevicePassword[]) const;


private:
    MainPipeEventMonitorInterface *m_eventMonitor;

    PLCDriverManager *m_plcDriverManager;
    FileSystemMonitor *m_filesystemMonitor;
    mutable QMutex m_execMutex;
};

#endif // PLCDRIVEREXECUTIVE_H
