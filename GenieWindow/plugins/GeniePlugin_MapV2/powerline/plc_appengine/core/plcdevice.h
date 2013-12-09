#ifndef PLCDEVICE_H
#define PLCDEVICE_H

#include "mainpipeinterface.h"

#include <QString>
#include <QStringList>
#include <QMap>
#include <QPair>
#include "plcdevicesoapthread.h"

class MainPipeInterface;

class PLCDevice
{
public:
    static const int SCAN_FUNCTION_COUNT = 9;

public:
    enum{
        PLC_DEVICE_NAME_BIT = ((1<<0)&0xFFFF)
        ,PLC_DEVICE_MODEL_BIT = ((1<<1)&0xFFFF)
        ,PLC_DEVICE_FWVERSION_BIT = ((1<<2)&0xFFFF)
        ,PLC_DEVICE_LEDSTATE_BIT = ((1<<3)&0xFFFF)
        ,PLC_DEVICE_LINKRATETABLE_BIT = ((1<<4)&0xFFFF)
        ,PLC_DEVICE_FORWARDTABLE_BIT = ((1<<5)&0xFFFF)
        ,PLC_DEVICE_IP_BIT = ((1<<6)&0xFFFF)
        ,PLC_DEVICE_NETWORKTYPE_BIT = ((1<<7)&0xFFFF)
        ,PLC_DEVICE_DRIVERMODULE_BIT = ((1<<8)&0xFFFF)
        ,PLC_DEVICE_ATTACHDEVICE_BIT = ((1<<9)&0xFFFF)
        ,PLC_DEVICE_ATTACHUSBDEVICE_BIT = ((1<<10)&0xFFFF)
        ,PLC_DEVICE_WLANINFO_BIT = ((1<<11)&0xFFFF)
    };
private:
    PLCDeviceSoapThread m_soapThread;
    quint16 m_tmpDevFieldValidMask;
    quint16 m_tmpDevFieldChangeMask;

    void StartSoapProcess(MainPipeInterface *mainPipeInf);

private:
    typedef bool (PLCDevice::*ScanFunType)(MainPipeInterface *mainPipeInf);
    ScanFunType ScanFunArray[SCAN_FUNCTION_COUNT];
    void InitScanFunctionArray();

public:
    explicit PLCDevice(const QString &deviceMac);

public:
    void ProcessScanning(MainPipeInterface *mainPipeInf);
    void WaitSoapEnd();

    quint16 DeviceChangeScanned() const;
    quint16 DeviceValidFileds() const;
    void ResetScannedChangeFlag();

public:
    bool ScanIP(MainPipeInterface *mainPipeInf);
    bool ScanName(MainPipeInterface *mainPipeInf);
    bool ScanModel(MainPipeInterface *mainPipeInf);
    bool ScanLEDState(MainPipeInterface *mainPipeInf);
    bool ScanFirmwareVersion(MainPipeInterface *mainPipeInf);
    bool ScanLinkRateTable(MainPipeInterface *mainPipeInf);
    bool ScanForwardTable(MainPipeInterface *mainPipeInf);
    bool ScanNetworkType(MainPipeInterface *mainPipeInf);
    bool ScanDriverModule(MainPipeInterface *mainPipeInf);

    bool ScanAttachDevice(MainPipeInterface *mainPipeInf);
    bool ScanAttachUsbDevice(MainPipeInterface *mainPipeInf);
    bool ScanWLANInfo(MainPipeInterface *mainPipeInf);

public:
    bool IsWirelessSupport() const;
    bool IsUsbSupport() const;

public:
    QString deviceMac() const;
    QString deviceIP() const;
    bool deviceLEDState() const;
    QString deviceName() const;

    QString deviceModel() const;
    QString deviceFWVersion() const;
    QString deviceDriverPath() const;
    QMap<QString,QPair<qint16,qint16> > deviceLinkRateTable() const;
    QStringList deviceForwardTable() const;
    QString deviceNetworkType() const;

    QList<AttachDeviceEntry> deviceAttachDevices() const;
    QList<QPair<QString,QString> > deviceAttachUsbDevices() const;

    WLANInfo deviceWLANInfo() const;

private:
    bool IsContentEqual(const QStringList &left, const QStringList &right) const;
    bool IsAttachDeviceEqual(const QList<AttachDeviceEntry> &left, const QList<AttachDeviceEntry> &right) const;
    bool IsWLANInfoEqual(const WLANInfo &left, const WLANInfo &right) const;

private:
    int m_currentScanningEntryIndex;

private:
    quint16 m_devFieldValidMask;
    quint16 m_devFieldChangeMask;

private:
    bool m_wirelessSupport;
    bool m_usbSupport;

private:
    QString m_devMAC;
    QString m_devIP;

    bool m_devLEDOn;
    QString m_devName;
    QString m_devModel;
    QString m_devFWVersion;
    QMap<QString,QPair<qint16,qint16> > m_linkRateTable;
    QStringList m_forwardTable;
    QString m_networkType;
    QString m_driverModule;

    QList<AttachDeviceEntry> m_attachDeviceList;
    QStringList m_usbTypes;
    QStringList m_usbDescs;
    QList<QPair<QString,QString> > m_attachUsbDeviceList;

    WLANInfo m_wlanInfo;
};

#endif // PLCDEVICE_H
