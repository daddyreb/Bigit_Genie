#ifndef MAINPIPEINTERFACE_H
#define MAINPIPEINTERFACE_H

#include <QtCore>
#include "mainpipedefines.h"
#include "mainpipeeventmonitorinterface.h"

class MainPipeInterface 
{
public:
    virtual ~MainPipeInterface(){}

public:
    //Startup ,StopProcess and Cleanup must be called from the same thread
    //StopProcess must be called before Cleanup can be called
    virtual bool Startup(const QString &driverDir, MainPipeEventMonitorInterface *monitorInf = 0) = 0;
    virtual void StopProcess() = 0;
    virtual void Cleanup() = 0;

    //the apis below are thread safe,however,they must be called after the call to Starup return successfully
    //and must not be called any more when the Cleanup is called,that is you must make sure that there is no thread
    //will make new call to these apis,before you make a call to Cleanup.

public:
    //other api
    virtual QString GetDeviceDriverFilePath(const QString &DeviceMACAddress) const = 0;
    virtual bool GetPLCDeviceIP(const QString &DeviceMACAddress, QString &IP) const = 0;
    virtual bool GetGatewayPhysicalAddrs(QStringList &physiAddrs) const = 0;

public:

    //adaptive layer api
	virtual bool PLCDiscovery(QStringList &DeviceMACAddrList) const = 0;

	virtual bool PLCGetLocalDevice(QString &DeviceMACAddress) const = 0;

	virtual bool PLCGetDeviceModelName(const QString &DeviceMACAddress, QString &DeviceModelName) const = 0;

	virtual bool PLCGetDeviceFWVersion(const QString &DeviceMACAddress, QString &DeviceFWVersion) const = 0;

	virtual bool PLCGetDeviceName(const QString &DeviceMACAddress, QString &DeviceName) const = 0;

	virtual bool PLCGetNetworkType(const QString &DeviceMACAddress, QString &DeviceNetworkType) const = 0;

	virtual bool PLCGetDeviceLedStatus(const QString &DeviceMACAddress, int *DeviceLedStatus) const = 0;

	virtual bool PLCGetDeviceLinkRate( const QString &DeviceMACAddress, QMap<QString,QPair<qint16,qint16> > &DeviceLinkRateTable) const = 0;

	virtual bool PLCGetDeviceQoSStatus(const QString &DeviceMACAddress 
			, QVector<QPair<QString, int> > &qosMacs
			, QVector<QPair<quint16, int> > &qosPorts
			, bool &checkVLAN
			, bool &checkTOS ) const = 0;

	virtual bool PLCGetDeviceForwardtable( const QString &DeviceMACAddress, QStringList &ForwardTable) const = 0;

    virtual bool PLCGetDeviceFWUpgradeVersion (const QString &DeviceMACAddress, QString &DeviceFWUpgradeVersion) const = 0;

    virtual bool PLCGetDeviceFWUpgradeRegion (const QString &DeviceMACAddress, QString &DeviceFWUpgradeResion) const = 0;



	virtual bool PLCSetDeviceName(const QString &DeviceMACAddress, const QString &DeviceName) const = 0;

	virtual bool PLCSetDeviceLedStatus(const QString &DeviceMACAddress, int DeviceLedStatus) const = 0;

    virtual bool PLCSetDeviceSecurityKey( const QString &DeviceMACAddress, const QString &SecurityKey,const QString &DevicePassword) const = 0;

	virtual bool PLCSetDeviceQoS(const QString &DeviceMACAddress
			, const QVector<QPair<QString, int> > &qosMacs
			, const QVector<QPair<quint16, int> > &qosPorts
			, bool checkVLAN
			, bool checkTOS ) const = 0;


	virtual bool PLCSetDeviceResetDefault(const QString &DeviceMACAddress) const = 0;

    virtual bool PLCCheckDevicePassword(const QString &DeviceMACAddress, const QString &dakcode) const = 0;
	
public:
	//soap api
    virtual int SoapGetAttachDevice(const QString &host ,quint16 port, QList<AttachDeviceEntry> &attachDevs) const = 0;
    virtual int SoapGetWPSConfigureState(const QString &host, quint16 port, bool &configured) const = 0;
	virtual int SoapGetUSBType(const QString &host, quint16 port, QStringList &usbTypes) const = 0;
    virtual int SoapGetUSBDesc(const QString &host, quint16 port, QStringList &usbDescs) const = 0;
    virtual int SoapGetWLANInfo(const QString &host, quint16 port, WLANInfo &wlanInfo) const = 0;
    virtual int SoapSetWLANInfo(const QString &host, quint16 port, const WLANInfo &wlanInfo) const = 0;
};

Q_DECLARE_INTERFACE(MainPipeInterface,"com.netgear.www.PowerLine.MainPipeInterface/1.0")

#endif // MAINPIPEINTERFACE_H
