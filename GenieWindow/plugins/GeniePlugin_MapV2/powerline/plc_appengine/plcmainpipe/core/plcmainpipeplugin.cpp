#include "plcmainpipeplugin.h"
#include "plcdriverexecutive.h"
#include "helperroutines.h"
#include "netgearsoapapi.h"
#include "nethelproutines.h"
#include "mainpipeapiguard.h"
#include<QMessageBox>
#include<QDebug>
#include <QRegExp>
#include <QMutexLocker>
#include <QSet>
//#include <QThread>

//class MiniSleep : public QThread
//{
//public:
//    static void msleep(unsigned long msecs){QThread::msleep(msecs);}
//};

//static const int API_CALL_INTERVAL_MS = 200;//200ms

#define API_REF_GUARD(countp,lockerp,cdtp,faileRetValue) \
    MainPipeApiGuard apiGuard(countp,lockerp,cdtp); \
    if(!apiGuard.CanProgress()){ \
    return (faileRetValue); \
    } /*MiniSleep::msleep(200);\*/
    /*{ \
        int time2sleep = m_plcApiCallInterval.restart(); \
        if(time2sleep <= 0){ \
            time2sleep = API_CALL_INTERVAL_MS; \
        }else if(time2sleep >= API_CALL_INTERVAL_MS){ \
            time2sleep = 0; \
        }else{ \
            time2sleep = (API_CALL_INTERVAL_MS - time2sleep); \
        } \
        if(time2sleep > 0){ \
            MiniSleep::msleep(time2sleep);\
        } \
    }*/


PLCMainPipePlugin::PLCMainPipePlugin(QObject *parent) :
    QObject(parent)
  ,m_refCount(0)
  ,m_eventMonitorInf(0)
  ,m_plcDriverExecutive(0)
  ,m_netgearSoapApi(0)
{
    QMutexLocker locker(&m_refCountMutex);
    m_refCount = 0;
}

bool PLCMainPipePlugin::Startup(const QString &driverDir, MainPipeEventMonitorInterface *monitorInf)
{
    if(m_plcDriverExecutive && m_netgearSoapApi){//indicate already startuped
        return true;
    }

    m_plcDriverExecutive = new PLCDriverExecutive(driverDir, monitorInf);
    m_netgearSoapApi = new NETGEARSoapApi();

    if(m_plcDriverExecutive != 0 && m_netgearSoapApi != 0){
        if(m_netgearSoapApi->Start()){
            m_refCountMutex.lock();
            m_refCount = 1;
            m_refCountMutex.unlock();

            m_plcApiCallInterval.start();

            return true;
        }
    }

    delete m_netgearSoapApi;
    m_netgearSoapApi = 0;

    delete m_plcDriverExecutive;
    m_plcDriverExecutive = 0;

    return false;
}

void PLCMainPipePlugin::StopProcess()
{
    m_refCountMutex.lock();

    if(m_netgearSoapApi){
        m_netgearSoapApi->Stop();
    }

    --m_refCount;
    while(m_refCount > 0){
        m_refCountCdt.wait(&m_refCountMutex);
    }

    m_refCountMutex.unlock();

}

void PLCMainPipePlugin::Cleanup()
{
    delete m_netgearSoapApi;
    m_netgearSoapApi = 0;

    delete m_plcDriverExecutive;
    m_plcDriverExecutive = 0;

}

QString PLCMainPipePlugin::GetDeviceDriverFilePath(const QString &DeviceMACAddress) const
{
    API_REF_GUARD(&m_refCount, &m_refCountMutex, &m_refCountCdt, QString());

    if(!IsMacAddrValid(DeviceMACAddress)){
        return QString();
    }
    QString normalDevMac = NormalizeValidMacAddrFormat(DeviceMACAddress);

    return m_plcDriverExecutive->GetPLCDriverPath(normalDevMac);
}

bool PLCMainPipePlugin::GetPLCDeviceIP(const QString &DeviceMACAddress, QString &IP) const
{
    API_REF_GUARD(&m_refCount, &m_refCountMutex, &m_refCountCdt, false);

    return NetHelpRoutines::GetPLCDeviceIP(DeviceMACAddress, IP);
}

bool PLCMainPipePlugin::GetGatewayPhysicalAddrs(QStringList &physiAddrs) const
{
    API_REF_GUARD(&m_refCount, &m_refCountMutex, &m_refCountCdt, false);

    return NetHelpRoutines::GetGatewayMacs(physiAddrs);
}

bool PLCMainPipePlugin::PLCDiscovery(QStringList &DeviceMACAddrList) const
{
    API_REF_GUARD(&m_refCount, &m_refCountMutex, &m_refCountCdt, false);

    int DeviceNumber = 0;
    PLC_DiscoveryDeviceList discoverDevList;

    if(PLC_GET_SUCCESS == m_plcDriverExecutive->PLCDiscovery(&DeviceNumber, &discoverDevList)){
        QSet<QString> devMacSet;
        for(int idx = 0 ; idx < DeviceNumber ; ++idx){
            qDebug()<<PLCDeviceMacAddr2String(discoverDevList.PLC_DeviceMACAddr[idx]);
            devMacSet.insert(PLCDeviceMacAddr2String(discoverDevList.PLC_DeviceMACAddr[idx]));
        }
        DeviceMACAddrList = devMacSet.toList();
        return true;
    }
    return false;
}

bool PLCMainPipePlugin::PLCGetLocalDevice(QString &DeviceMACAddress) const
{
    API_REF_GUARD(&m_refCount, &m_refCountMutex, &m_refCountCdt, false);

    PLC_MACAddress localDevMac;
    if(PLC_GET_SUCCESS == m_plcDriverExecutive->PLCGetLocalDevice(localDevMac)) {
        DeviceMACAddress = PLCDeviceMacAddr2String(localDevMac);
        return true;
    }
    return false;
}

bool PLCMainPipePlugin::PLCGetDeviceModelName(const QString &DeviceMACAddress, QString &DeviceModelName) const
{
    API_REF_GUARD(&m_refCount, &m_refCountMutex, &m_refCountCdt, false);

    if(!IsMacAddrValid(DeviceMACAddress)){
        return false;
    }
    QString normalDevMac = NormalizeValidMacAddrFormat(DeviceMACAddress);

    char devModelName[256];
    memset(devModelName ,0 ,sizeof(devModelName));

    BYTE deviceMac[PLC_MAC_ADDRESS_SIZE];

    if(PLC_GET_SUCCESS == m_plcDriverExecutive->PLCGetDeviceModelName(PLCDeviceMacAddrFromString(normalDevMac, deviceMac),devModelName)){
        DeviceModelName = QString(devModelName);
        return true;
    }
    return false;
}

bool PLCMainPipePlugin::PLCGetDeviceFWVersion(const QString &DeviceMACAddress, QString &DeviceFWVersion) const
{
    API_REF_GUARD(&m_refCount, &m_refCountMutex, &m_refCountCdt, false);

    if(!IsMacAddrValid(DeviceMACAddress)){
        return false;
    }
    QString normalDevMac = NormalizeValidMacAddrFormat(DeviceMACAddress);

    char devFWVersion[256];
    memset(devFWVersion ,0 ,sizeof(devFWVersion));

    BYTE deviceMac[PLC_MAC_ADDRESS_SIZE];

    if(PLC_GET_SUCCESS == m_plcDriverExecutive->PLCGetDeviceFWVersion(PLCDeviceMacAddrFromString(normalDevMac, deviceMac),devFWVersion)){
        DeviceFWVersion = QString(devFWVersion);
        return true;
    }
    return false;
}

bool PLCMainPipePlugin::PLCGetDeviceName(const QString &DeviceMACAddress, QString &DeviceName) const
{
    API_REF_GUARD(&m_refCount, &m_refCountMutex, &m_refCountCdt, false);

    if(!IsMacAddrValid(DeviceMACAddress)){
        return false;
    }
    QString normalDevMac = NormalizeValidMacAddrFormat(DeviceMACAddress);

    char devName[256];
    memset(devName ,0 ,sizeof(devName));

    BYTE deviceMac[PLC_MAC_ADDRESS_SIZE];

    if(PLC_GET_SUCCESS == m_plcDriverExecutive->PLCGetDeviceName(PLCDeviceMacAddrFromString(normalDevMac, deviceMac),devName)){
        DeviceName = QString(devName);
        return true;
    }
    return false;
}


bool PLCMainPipePlugin::PLCGetNetworkType(const QString &DeviceMACAddress, QString &DeviceNetworkType) const
{
    API_REF_GUARD(&m_refCount, &m_refCountMutex, &m_refCountCdt, false);

    if(!IsMacAddrValid(DeviceMACAddress)){
        return false;
    }
    QString normalDevMac = NormalizeValidMacAddrFormat(DeviceMACAddress);

    char devNetworkType[256];
    memset(devNetworkType ,0 ,sizeof(devNetworkType));

    BYTE deviceMac[PLC_MAC_ADDRESS_SIZE];

    if(PLC_GET_SUCCESS == m_plcDriverExecutive->PLCGetNetworkType(PLCDeviceMacAddrFromString(normalDevMac, deviceMac),devNetworkType)){
        DeviceNetworkType = QString(devNetworkType);
        return true;
    }
    return false;
}

bool PLCMainPipePlugin::PLCGetDeviceLedStatus(const QString &DeviceMACAddress, int *DeviceLedStatus) const
{
    API_REF_GUARD(&m_refCount, &m_refCountMutex, &m_refCountCdt, false);

    if(!IsMacAddrValid(DeviceMACAddress)){
        return false;
    }
    QString normalDevMac = NormalizeValidMacAddrFormat(DeviceMACAddress);

    BYTE deviceMac[PLC_MAC_ADDRESS_SIZE];

    return (PLC_GET_SUCCESS == m_plcDriverExecutive->PLCGetDeviceLedStatus(PLCDeviceMacAddrFromString(normalDevMac, deviceMac),DeviceLedStatus));
}

bool PLCMainPipePlugin::PLCGetDeviceLinkRate(const QString &DeviceMACAddress, QMap<QString, QPair<qint16, qint16> > &DeviceLinkRateTable) const
{
    API_REF_GUARD(&m_refCount, &m_refCountMutex, &m_refCountCdt, false);

    if(!IsMacAddrValid(DeviceMACAddress)){
        return false;
    }
    QString normalDevMac = NormalizeValidMacAddrFormat(DeviceMACAddress);

    PLC_DeviceLinkRateTable devLinkRateTable;
    int devLinkRateNumber = 0;
    BYTE deviceMac[PLC_MAC_ADDRESS_SIZE];
    QString devMac;
    QPair<qint16, qint16> linkRatePair;

    if(PLC_GET_SUCCESS == m_plcDriverExecutive->PLCGetDeviceLinkRate(PLCDeviceMacAddrFromString(normalDevMac, deviceMac),
                                                                     &devLinkRateNumber, &devLinkRateTable)){
        DeviceLinkRateTable.clear();

        for(int idx = 0 ; idx < devLinkRateNumber ; ++idx){
            devMac = PLCDeviceMacAddr2String(devLinkRateTable.PLC_DeviceMACAddr[idx]);
            linkRatePair.first = devLinkRateTable.PLC_DeviceTXRate[idx];
            linkRatePair.second = devLinkRateTable.PLC_DeviceRXRate[idx];
            DeviceLinkRateTable[devMac] = linkRatePair;
        }//for

        return true;
    }//if

    return false;
}

bool PLCMainPipePlugin::PLCGetDeviceQoSStatus(const QString &DeviceMACAddress 
                                              , QVector<QPair<QString, int> > &qosMacs
                                              , QVector<QPair<quint16, int> > &qosPorts
                                              , bool &checkVLAN
                                              , bool &checkTOS ) const
{
    API_REF_GUARD(&m_refCount, &m_refCountMutex, &m_refCountCdt, false);

    if(!IsMacAddrValid(DeviceMACAddress)){
        return false;
    }
    QString normalDevMac = NormalizeValidMacAddrFormat(DeviceMACAddress);

    BYTE deviceMac[PLC_MAC_ADDRESS_SIZE];
    PLC_DeviceQoSStatus devQoSStatus;
    QString devMac;

    if(PLC_GET_SUCCESS == m_plcDriverExecutive->PLCGetDeviceQoSStatus(PLCDeviceMacAddrFromString(normalDevMac, deviceMac),&devQoSStatus)){

        qosMacs.clear();
        qosPorts.clear();

        checkVLAN = (devQoSStatus.PLC_VLANStatus == ENABLE);
        checkTOS = (devQoSStatus.PLC_TOSStatus == ENABLE);

        for(BYTE macIdx = 0 ; macIdx < devQoSStatus.PLC_QoSMACListItem ; ++macIdx){
            devMac = PLCDeviceMacAddr2String(devQoSStatus.PLC_MACAddrList[macIdx].PLC_QOS_MACAddr);
            qosMacs.append(QPair<QString,int>(devMac, devQoSStatus.PLC_MACAddrList[macIdx].PLC_QOS_MACAddr_Priority));
        }

        for(BYTE portIdx = 0 ; portIdx < devQoSStatus.PLC_QoSPortListItem ; ++portIdx){
            qosPorts.append(QPair<quint16, int>(devQoSStatus.PLC_PortList[portIdx].PLC_PortNumber, devQoSStatus.PLC_PortList[portIdx].PLC_PortNumber_Priority));
        }

        return true;
    }

    return false;
}


bool PLCMainPipePlugin::PLCGetDeviceForwardtable(const QString &DeviceMACAddress, QStringList &ForwardTable) const
{
    API_REF_GUARD(&m_refCount, &m_refCountMutex, &m_refCountCdt, false);

    if(!IsMacAddrValid(DeviceMACAddress)){
        return false;
    }
    QString normalDevMac = NormalizeValidMacAddrFormat(DeviceMACAddress);

    PLC_DeviceForwardTableList devForwardTableList;
    int devForwardTableNumber = 0;
    BYTE deviceMac[PLC_MAC_ADDRESS_SIZE];

    if(PLC_GET_SUCCESS == m_plcDriverExecutive->PLCGetDeviceForwardtable(PLCDeviceMacAddrFromString(normalDevMac, deviceMac),
                                                                         &devForwardTableNumber, &devForwardTableList)){
        ForwardTable.clear();

        PLC_MACAddress devMACPlusOne, devMACMinusOne;
        Add2MacAddrValue(deviceMac, devMACPlusOne, 1);
        Add2MacAddrValue(deviceMac, devMACMinusOne, -1);

        for(int idx = 0 ; idx < devForwardTableNumber; ++idx){
            if(0 == memcmp(devForwardTableList.PLC_DeviceForwardMACAddr[idx], deviceMac, sizeof(PLC_MACAddress)) ||
                    0 == memcmp(devForwardTableList.PLC_DeviceForwardMACAddr[idx], devMACPlusOne, sizeof(PLC_MACAddress)) ||
                    0 == memcmp(devForwardTableList.PLC_DeviceForwardMACAddr[idx], devMACMinusOne, sizeof(PLC_MACAddress)) ){
                continue;
            }

            ForwardTable.append(PLCDeviceMacAddr2String(devForwardTableList.PLC_DeviceForwardMACAddr[idx]));
        }//for

        return true;
    }//if

    return false;
}


bool PLCMainPipePlugin::PLCGetDeviceFWUpgradeVersion (const QString &DeviceMACAddress, QString &DeviceFWUpgradeVersion) const
{
    API_REF_GUARD(&m_refCount, &m_refCountMutex, &m_refCountCdt, false);

    if(!IsMacAddrValid(DeviceMACAddress)){
        return false;
    }
    QString normalDevMac = NormalizeValidMacAddrFormat(DeviceMACAddress);

    char upgrageVersion[256];
    memset(upgrageVersion ,0 ,sizeof(upgrageVersion));

    BYTE deviceMac[PLC_MAC_ADDRESS_SIZE];

    if(PLC_GET_SUCCESS == m_plcDriverExecutive->PLCGetDeviceFWUpgradeVersion(PLCDeviceMacAddrFromString(normalDevMac, deviceMac),upgrageVersion)){
        DeviceFWUpgradeVersion = QString(upgrageVersion);
        return true;
    }
    return false;
}


bool PLCMainPipePlugin::PLCGetDeviceFWUpgradeRegion (const QString &DeviceMACAddress, QString &DeviceFWUpgradeResion) const
{
    API_REF_GUARD(&m_refCount, &m_refCountMutex, &m_refCountCdt, false);

    if(!IsMacAddrValid(DeviceMACAddress)){
        return false;
    }
    QString normalDevMac = NormalizeValidMacAddrFormat(DeviceMACAddress);

    char upgradeRegion[256];
    memset(upgradeRegion ,0 ,sizeof(upgradeRegion));

    BYTE deviceMac[PLC_MAC_ADDRESS_SIZE];

    if(PLC_GET_SUCCESS == m_plcDriverExecutive->PLCGetDeviceFWUpgradeRegion(PLCDeviceMacAddrFromString(normalDevMac, deviceMac),upgradeRegion)){
        DeviceFWUpgradeResion = QString(upgradeRegion);
        return true;
    }
    return false;
}

bool PLCMainPipePlugin::PLCSetDeviceName(const QString &DeviceMACAddress, const QString &DeviceName)const
{
    API_REF_GUARD(&m_refCount, &m_refCountMutex, &m_refCountCdt, false);

    if(!IsMacAddrValid(DeviceMACAddress)){
        return false;
    }
    QString normalDevMac = NormalizeValidMacAddrFormat(DeviceMACAddress);

    BYTE deviceMac[PLC_MAC_ADDRESS_SIZE];

    return (PLC_SET_SUCCESS == m_plcDriverExecutive->PLCSetDeviceName(PLCDeviceMacAddrFromString(normalDevMac, deviceMac),DeviceName.toStdString().c_str()));
}

bool PLCMainPipePlugin::PLCSetDeviceLedStatus(const QString &DeviceMACAddress, int DeviceLedStatus) const
{
    API_REF_GUARD(&m_refCount, &m_refCountMutex, &m_refCountCdt, false);

    if(!IsMacAddrValid(DeviceMACAddress)){
        return false;
    }
    QString normalDevMac = NormalizeValidMacAddrFormat(DeviceMACAddress);

    BYTE deviceMac[PLC_MAC_ADDRESS_SIZE];

    return (PLC_SET_SUCCESS == m_plcDriverExecutive->PLCSetDeviceLedStatus(PLCDeviceMacAddrFromString(normalDevMac, deviceMac) ,DeviceLedStatus));
}

bool PLCMainPipePlugin::PLCSetDeviceSecurityKey( const QString &DeviceMACAddress, const QString &SecurityKey,const QString &DevicePassword) const
{
    API_REF_GUARD(&m_refCount, &m_refCountMutex, &m_refCountCdt, false);

    if(!IsMacAddrValid(DeviceMACAddress)){
        return false;
    }
    QString normalDevMac = NormalizeValidMacAddrFormat(DeviceMACAddress);

    BYTE deviceMac[PLC_MAC_ADDRESS_SIZE];

    return (PLC_SET_SUCCESS == m_plcDriverExecutive->PLCSetDeviceSecurityKey(PLCDeviceMacAddrFromString(normalDevMac, deviceMac)
                                                                             ,SecurityKey.toStdString().c_str(), DevicePassword.toStdString().c_str()));
}

bool PLCMainPipePlugin::PLCSetDeviceQoS(const QString &DeviceMACAddress
                                        , const QVector<QPair<QString, int> > &qosMacs
                                        , const QVector<QPair<quint16, int> > &qosPorts
                                        , bool checkVLAN
                                        , bool checkTOS ) const
{
    API_REF_GUARD(&m_refCount, &m_refCountMutex, &m_refCountCdt, false);

    if(!IsMacAddrValid(DeviceMACAddress)){
        return false;
    }
    QString normalDevMac = NormalizeValidMacAddrFormat(DeviceMACAddress);

    BYTE deviceMac[PLC_MAC_ADDRESS_SIZE];
    PLC_DeviceQoSStatus devQoSStatus;

    devQoSStatus.PLC_QoSMACListItem = static_cast<BYTE>(qosMacs.count());
    devQoSStatus.PLC_QoSPortListItem = static_cast<BYTE>(qosPorts.count());

    devQoSStatus.PLC_VLANStatus = (checkVLAN ? ENABLE : DISABLE);
    devQoSStatus.PLC_TOSStatus = (checkTOS ? ENABLE : DISABLE);

    for(BYTE macIdx = 0 ; macIdx < devQoSStatus.PLC_QoSMACListItem ; ++macIdx){
        PLCDeviceMacAddrFromString(qosMacs[macIdx].first, deviceMac);
        memcpy(devQoSStatus.PLC_MACAddrList[macIdx].PLC_QOS_MACAddr, deviceMac, sizeof(deviceMac));
        devQoSStatus.PLC_MACAddrList[macIdx].PLC_QOS_MACAddr_Priority = static_cast<BYTE>(qosMacs[macIdx].second);
    }

    for(BYTE portIdx = 0 ; portIdx < devQoSStatus.PLC_QoSPortListItem; ++portIdx){
        devQoSStatus.PLC_PortList[portIdx].PLC_PortNumber = static_cast<int>(qosPorts[portIdx].first);
        devQoSStatus.PLC_PortList[portIdx].PLC_PortNumber_Priority = static_cast<BYTE>(qosPorts[portIdx].second);
    }


    BYTE destDeviceMac[PLC_MAC_ADDRESS_SIZE];

    return (PLC_SET_SUCCESS == m_plcDriverExecutive->PLCSetDeviceQoS(PLCDeviceMacAddrFromString(normalDevMac,destDeviceMac),&devQoSStatus));
}

bool PLCMainPipePlugin::PLCSetDeviceResetDefault(const QString &DeviceMACAddress) const
{
    API_REF_GUARD(&m_refCount, &m_refCountMutex, &m_refCountCdt, false);

    if(!IsMacAddrValid(DeviceMACAddress)){
        return false;
    }
    QString normalDevMac = NormalizeValidMacAddrFormat(DeviceMACAddress);

    BYTE deviceMac[PLC_MAC_ADDRESS_SIZE];

    return (PLC_SET_SUCCESS == m_plcDriverExecutive->PLCSetDeviceResetDefault(PLCDeviceMacAddrFromString(normalDevMac, deviceMac)));
}

bool PLCMainPipePlugin::PLCCheckDevicePassword(const QString &DeviceMACAddress, const QString &dakcode) const
{
    API_REF_GUARD(&m_refCount, &m_refCountMutex, &m_refCountCdt, false);

    if(!IsMacAddrValid(DeviceMACAddress)){
        return false;
    }
    QString normalDevMac = NormalizeValidMacAddrFormat(DeviceMACAddress);

    BYTE deviceMac[PLC_MAC_ADDRESS_SIZE];

    return (PLC_GET_SUCCESS == m_plcDriverExecutive->PLCCheckDevicePassword(PLCDeviceMacAddrFromString(normalDevMac, deviceMac), dakcode.toStdString().c_str()));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
int PLCMainPipePlugin::SoapGetAttachDevice(const QString &host ,quint16 port, QList<AttachDeviceEntry> &attachDevs) const
{
    API_REF_GUARD(&m_refCount, &m_refCountMutex, &m_refCountCdt, SOAP_NOT_INITIALIZED);

    return m_netgearSoapApi->GetAttachDevice(host,port,attachDevs);
}

int PLCMainPipePlugin::SoapGetWPSConfigureState(const QString &host, quint16 port, bool &configured) const
{
    API_REF_GUARD(&m_refCount, &m_refCountMutex, &m_refCountCdt, SOAP_NOT_INITIALIZED);

    return m_netgearSoapApi->GetWPSConfigureState(host, port, configured);
}

int PLCMainPipePlugin::SoapGetUSBType(const QString &host, quint16 port, QStringList &usbTypes) const
{
    API_REF_GUARD(&m_refCount, &m_refCountMutex, &m_refCountCdt, SOAP_NOT_INITIALIZED);

    return m_netgearSoapApi->GetUSBType(host, port, usbTypes);
}

int PLCMainPipePlugin::SoapGetUSBDesc(const QString &host, quint16 port, QStringList &usbDescs) const
{
    API_REF_GUARD(&m_refCount, &m_refCountMutex, &m_refCountCdt, SOAP_NOT_INITIALIZED);

    return m_netgearSoapApi->GetUSBDesc(host, port, usbDescs);
}

int PLCMainPipePlugin::SoapGetWLANInfo(const QString &host, quint16 port, WLANInfo &wlanInfo) const
{
    API_REF_GUARD(&m_refCount, &m_refCountMutex, &m_refCountCdt, SOAP_NOT_INITIALIZED);

    wlanInfo.m_wpsConfigured = WLANInfo::WPS_STATE_UNKNOWN;
    bool wpsConfigured = false;
    if(SOAP_OK == m_netgearSoapApi->GetWPSConfigureState(host, port, wpsConfigured)){
        wlanInfo.m_wpsConfigured = (wpsConfigured ? WLANInfo::WPS_CONFIGURED : WLANInfo::WPS_NOT_CONFIGURED);
    }

    return m_netgearSoapApi->GetWLANInfo(host, port, wlanInfo);
}

int PLCMainPipePlugin::SoapSetWLANInfo(const QString &host, quint16 port, const WLANInfo &wlanInfo) const
{
    API_REF_GUARD(&m_refCount, &m_refCountMutex, &m_refCountCdt, SOAP_NOT_INITIALIZED);

    return m_netgearSoapApi->SetWLANInfo(host, port, wlanInfo);
}

#ifdef BUILD_MAINPIPE_PLUGIN
Q_EXPORT_PLUGIN2(PLCMainPipePlugin, PLCMainPipePlugin)
#endif
