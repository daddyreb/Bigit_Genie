#ifndef PLCMAINPIPEPLUGIN_H
#define PLCMAINPIPEPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QString>
#include <QVector>
#include <QPair>
#include <QMap>
#include <QMutex>
#include <QWaitCondition>
#include <QTime>

#include "mainpipeinterface.h"

class PLCDriverExecutive;
class NETGEARSoapApi;

class PLCMainPipePlugin : public QObject ,public MainPipeInterface
{
    Q_OBJECT
    Q_INTERFACES(MainPipeInterface)

public:
    explicit PLCMainPipePlugin(QObject *parent = 0);
    
signals:
    
public slots:

public:
    bool Startup(const QString &driverDir, MainPipeEventMonitorInterface *monitorInf);
    void StopProcess();
	void Cleanup();

public:
    //other api
    QString GetDeviceDriverFilePath(const QString &DeviceMACAddress) const;
    bool GetPLCDeviceIP(const QString &DeviceMACAddress, QString &IP) const;
    bool GetGatewayPhysicalAddrs(QStringList &physiAddrs) const;

public:

    //adaptive layer api
    bool PLCDiscovery(QStringList &DeviceMACAddrList) const;

    bool PLCGetLocalDevice(QString &DeviceMACAddress) const;

    bool PLCGetDeviceModelName(const QString &DeviceMACAddress, QString &DeviceModelName) const;

    bool PLCGetDeviceFWVersion(const QString &DeviceMACAddress, QString &DeviceFWVersion) const;

    bool PLCGetDeviceName(const QString &DeviceMACAddress, QString &DeviceName) const;

    bool PLCGetNetworkType(const QString &DeviceMACAddress, QString &DeviceNetworkType) const;

    bool PLCGetDeviceLedStatus(const QString &DeviceMACAddress, int *DeviceLedStatus) const;

    bool PLCGetDeviceLinkRate( const QString &DeviceMACAddress, QMap<QString,QPair<qint16,qint16> > &DeviceLinkRateTable) const;

	bool PLCGetDeviceQoSStatus(const QString &DeviceMACAddress 
					, QVector<QPair<QString, int> > &qosMacs
					, QVector<QPair<quint16, int> > &qosPorts
					, bool &checkVLAN
					, bool &checkTOS ) const;

    bool PLCGetDeviceForwardtable( const QString &DeviceMACAddress, QStringList &ForwardTable) const;

    bool PLCGetDeviceFWUpgradeVersion (const QString &DeviceMACAddress, QString &DeviceFWUpgradeVersion) const;

    bool PLCGetDeviceFWUpgradeRegion (const QString &DeviceMACAddress, QString &DeviceFWUpgradeResion) const;



    bool PLCSetDeviceName(const QString &DeviceMACAddress, const QString &DeviceName)const;

    bool PLCSetDeviceLedStatus(const QString &DeviceMACAddress, int DeviceLedStatus) const;

    bool PLCSetDeviceSecurityKey( const QString &DeviceMACAddress, const QString &SecurityKey,const QString &DevicePassword)const;

	bool PLCSetDeviceQoS(const QString &DeviceMACAddress
					, const QVector<QPair<QString, int> > &qosMacs
					, const QVector<QPair<quint16, int> > &qosPorts
					, bool checkVLAN
					, bool checkTOS ) const;

    bool PLCSetDeviceResetDefault(const QString &DeviceMACAddress) const;

    bool PLCCheckDevicePassword(const QString &DeviceMACAddress, const QString &dakcode) const;

public:
    //soap api
    int SoapGetAttachDevice(const QString &host ,quint16 port, QList<AttachDeviceEntry> &attachDevs) const;
    int SoapGetWPSConfigureState(const QString &host, quint16 port, bool &configured) const;
    int SoapGetUSBType(const QString &host, quint16 port, QStringList &usbTypes) const;
    int SoapGetUSBDesc(const QString &host, quint16 port, QStringList &usbDescs) const;
    int SoapGetWLANInfo(const QString &host, quint16 port, WLANInfo &wlanInfo) const;
    int SoapSetWLANInfo(const QString &host, quint16 port, const WLANInfo &wlanInfo) const;

private:
    mutable int m_refCount;
    mutable QMutex m_refCountMutex;
    mutable QWaitCondition m_refCountCdt;
    mutable QTime m_plcApiCallInterval;

    MainPipeEventMonitorInterface *m_eventMonitorInf;

	PLCDriverExecutive *m_plcDriverExecutive;
	NETGEARSoapApi *m_netgearSoapApi;
};

#endif // PLCMAINPIPEPLUGIN_H
