#include "plcdriverexecutive.h"
#include "helperroutines.h"
#include "plcdrivermanager.h"
#include "plcdriver.h"
#include "mainpipeeventmonitorinterface.h"

#include <QtDebug>
#include <QTextStream>
#include <QMutexLocker>
#include <QNetworkInterface>
#include <QSet>

PLCDriverExecutive::PLCDriverExecutive(const QString &driverDirPath, MainPipeEventMonitorInterface *monitorInf) :
    m_eventMonitor(monitorInf)
  ,m_plcDriverManager(new PLCDriverManager(driverDirPath))
  ,m_filesystemMonitor(new FileSystemMonitor(driverDirPath))
{
    //The object of class PLCDriverExecutive may belong to a thread without event loop so
    //we have to set the Qt::DirectConnection to let the file system monitor thread
    //invoke the OnNewPLCDriver slot directly
    connect(m_filesystemMonitor, SIGNAL(newPLCDriver(QString)) ,
            SLOT(OnNewPLCDriver(QString)), Qt::DirectConnection);
}

PLCDriverExecutive::~PLCDriverExecutive()
{
    QMutexLocker locker(&m_execMutex);
    delete m_filesystemMonitor;
    delete m_plcDriverManager;
}


void PLCDriverExecutive::OnNewPLCDriver(const QString &driverPath)
{
    QMutexLocker locker(&m_execMutex);

    //qDebug() << "PLC Driver Executive new driver found:" << driverPath;

    PLCDriver *driver = m_plcDriverManager->AddPLCDriver(driverPath);

    Q_UNUSED(driver);

    if(m_eventMonitor){
        m_eventMonitor->OnNewPLCDriver(driverPath);
    }

    //    if(!driver || !(driver->PLCDiscovery)){
    //        return;
    //    }

    //    BOOL bResult = FALSE;
    //    int devNumber = 0;
    //    PLC_DiscoveryDeviceList devMacAddrList;

    //    bResult = driver->PLCDiscovery(&devNumber,&devMacAddrList);

    //    QString traceMsg;
    //    QTextStream traceStream(&traceMsg);
    //    traceStream << "PLC Driver : " << driver->GetLibraryPathName() << " -->discover  device " << (bResult ? "OK" : "FAILED")
    //                << ",device number = " << devNumber;
    //    qDebug() << traceMsg;


    //    if(bResult){
    //        if(devNumber > 0){
    //            qDebug() << "Discovered device macs: ";
    //        }

    //        for(int idx = 0 ; idx < devNumber ; ++idx){
    //            qDebug() << "<" << PLCDeviceMacAddr2String(devMacAddrList.PLC_DeviceMACAddr[idx]) << ">";

    //            //associate plc device with plc driver
    //            m_plcDriverManager->AssociatePLCDriver(PLCDeviceMacAddr2String(devMacAddrList.PLC_DeviceMACAddr[idx]), driver);
    //        }
    //    }

    //    qDebug() <<  "\r\n";
}

QString PLCDriverExecutive::GetPLCDriverPath(const QString &devMac) const
{
    QMutexLocker locker(&m_execMutex);

    PLCDriver *plcDriver = m_plcDriverManager->GetPLCDriver(devMac);
    if(plcDriver){
        return plcDriver->GetLibraryPathName();
    }
    return QString();
}

BOOL PLCDriverExecutive::PLCDiscovery(int *DeviceNumber, PLC_DiscoveryDeviceList *DeviceMACAddrList)  const
{
    QMutexLocker locker(&m_execMutex);

    *DeviceNumber = 0;

    BOOL bDiscoverResult = FALSE;
    BOOL bResult = FALSE;
    int devNumber = 0;
    PLC_DiscoveryDeviceList devMacAddrList;

    QVector<PLCDriver *> drivers = m_plcDriverManager->GetPLCDriverList();
    QVector<PLCDriver *>::const_iterator iter = drivers.begin();

    for( ; iter != drivers.end() ; ++iter){

//        if(!(* iter)->PLCDiscovery){
//            continue;
//        }

        bResult = (* iter)->PLCDiscovery(&devNumber,&devMacAddrList);
        bDiscoverResult = (bDiscoverResult || bResult);


        QString traceMsg;
        QTextStream traceStream(&traceMsg);
        traceStream << "PLC Driver : " << (*iter)->GetLibraryPathName() << " -->discover  device " << (bResult ? "OK" : "FAILED")
                    << ",device number = " << devNumber;
        qDebug() << traceMsg;

        if(bResult){
            if(devNumber > 0){
                qDebug() << "Discovered device macs: ";
            }

            for(int idx = 0 ; (idx < devNumber) && (idx < PLC_MAX_DEVICES); ++idx){
                qDebug() << "<" << PLCDeviceMacAddr2String(devMacAddrList.PLC_DeviceMACAddr[idx]) << ">";

                memcpy(DeviceMACAddrList->PLC_DeviceMACAddr[*DeviceNumber + idx]
                       , devMacAddrList.PLC_DeviceMACAddr[idx]
                       ,sizeof(devMacAddrList.PLC_DeviceMACAddr[idx]));

                //associate plc device with plc driver
                m_plcDriverManager->AssociatePLCDriver(PLCDeviceMacAddr2String(devMacAddrList.PLC_DeviceMACAddr[idx]), *iter);
            }

            *DeviceNumber += devNumber;
        }
        qDebug() <<  "\r\n";

    }

    return  (bDiscoverResult ? PLC_GET_SUCCESS : PLC_GET_FAIL);
}

BOOL PLCDriverExecutive::PLCGetLocalDevice(PLC_MACAddress DeviceMACAddress) const
{
    QMutexLocker locker(&m_execMutex);

    //detect local interface hardware addresses
    QList<QNetworkInterface> nicinfs = QNetworkInterface::allInterfaces();
    QSet<QString> nicmacs;
    foreach(QNetworkInterface inf, nicinfs)
    {
        if(!inf.isValid()
                || (inf.flags() & QNetworkInterface::IsLoopBack)
                || !(inf.flags() & QNetworkInterface::IsUp)
                || !(inf.flags() & QNetworkInterface::IsRunning)
                || !(inf.flags() & QNetworkInterface::CanBroadcast)){

            continue;
        }

        if(IsMacAddrValid(inf.hardwareAddress())){
            nicmacs.insert(NormalizeValidMacAddrFormat(inf.hardwareAddress()));
        }
    }

    qDebug () << "local device mac set:" << nicmacs;
    //end

    QVector<PLCDriver *> drivers = m_plcDriverManager->GetPLCDriverList();
    QVector<PLCDriver *>::const_iterator iter = drivers.begin();

    PLC_MACAddress localDeviceMAC;
    bool localDevMacEmpty = true;

    for( ; iter != drivers.end() ; ++iter){
//        if(!(*iter)->PLCGetLocalDevice){
//            continue;
//        }

        if(PLC_GET_SUCCESS == (*iter)->PLCGetLocalDevice(localDeviceMAC)){
            if(localDevMacEmpty){
                memcpy(DeviceMACAddress, localDeviceMAC ,sizeof(PLC_MACAddress));
                localDevMacEmpty = false;
            }

            qDebug () << "Driver:" << (*iter)->GetLibraryPathName() <<"-->local device:" << PLCDeviceMacAddr2String(localDeviceMAC);

//            if(!(*iter)->PLCGetDeviceForwardtable){
//                continue;
//            }

            int num = 0;
            PLC_DeviceForwardTableList forwardTable;
            PLC_MACAddress nicMacPLCFormat;

            if(PLC_GET_SUCCESS == (*iter)->PLCGetDeviceForwardtable(localDeviceMAC, &num, &forwardTable)){
                foreach(QString nicmac, nicmacs){
                    PLCDeviceMacAddrFromString(nicmac, nicMacPLCFormat);
                    for(int idx = 0 ; idx < num ; ++idx){
                        if(!memcmp(nicMacPLCFormat, forwardTable.PLC_DeviceForwardMACAddr[idx],
                                   sizeof(PLC_MACAddress))){

                            memcpy(DeviceMACAddress, localDeviceMAC ,sizeof(PLC_MACAddress));
                            return PLC_GET_SUCCESS;
                        }
                    }
                }
            }
        }else{
            qDebug () << "Driver:" << (*iter)->GetLibraryPathName() <<"-->local device:" << "false";
        }
    }

    return (localDevMacEmpty ? PLC_GET_FAIL : PLC_GET_SUCCESS);
}

BOOL PLCDriverExecutive::PLCGetDeviceModelName(PLC_MACAddress DeviceMACAddress, char *DeviceModelName) const
{
    QMutexLocker locker(&m_execMutex);

    PLCDriver *plcDriver = m_plcDriverManager->GetPLCDriver(PLCDeviceMacAddr2String(DeviceMACAddress));
    if(plcDriver/* && plcDriver ->PLCGetDeviceModelName*/){
        return plcDriver ->PLCGetDeviceModelName(DeviceMACAddress, DeviceModelName);
    }

    return PLC_GET_FAIL;
}

BOOL PLCDriverExecutive::PLCGetDeviceFWVersion(PLC_MACAddress DeviceMACAddress, char *DeviceFWVersion) const
{

    QMutexLocker locker(&m_execMutex);

    PLCDriver *plcDriver = m_plcDriverManager->GetPLCDriver(PLCDeviceMacAddr2String(DeviceMACAddress));
    if(plcDriver/* && plcDriver ->PLCGetDeviceFWVersion*/){
        return plcDriver ->PLCGetDeviceFWVersion(DeviceMACAddress, DeviceFWVersion);
    }

    return PLC_GET_FAIL;
}

BOOL PLCDriverExecutive::PLCGetDeviceName(PLC_MACAddress DeviceMACAddress, char *DeviceName) const
{

    QMutexLocker locker(&m_execMutex);

    PLCDriver *plcDriver = m_plcDriverManager->GetPLCDriver(PLCDeviceMacAddr2String(DeviceMACAddress));
    if(plcDriver/* && plcDriver ->PLCGetDeviceName*/){
        return plcDriver ->PLCGetDeviceName(DeviceMACAddress, DeviceName);
    }

    return PLC_GET_FAIL;
}


BOOL PLCDriverExecutive::PLCGetNetworkType(PLC_MACAddress DeviceMACAddress, char *DeviceNetworkType) const
{

    QMutexLocker locker(&m_execMutex);

    PLCDriver *plcDriver = m_plcDriverManager->GetPLCDriver(PLCDeviceMacAddr2String(DeviceMACAddress));
    if(plcDriver/* && plcDriver ->PLCGetNetworkType*/){
        return plcDriver ->PLCGetNetworkType(DeviceMACAddress, DeviceNetworkType);
    }

    return PLC_GET_FAIL;
}


BOOL PLCDriverExecutive::PLCGetDeviceLedStatus(PLC_MACAddress DeviceMACAddress, int *DeviceLedStatus) const
{

    QMutexLocker locker(&m_execMutex);

    PLCDriver *plcDriver = m_plcDriverManager->GetPLCDriver(PLCDeviceMacAddr2String(DeviceMACAddress));
    if(plcDriver/* && plcDriver ->PLCGetDeviceLedStatus*/){
        return plcDriver ->PLCGetDeviceLedStatus(DeviceMACAddress, DeviceLedStatus);
    }

    return PLC_GET_FAIL;
}


BOOL PLCDriverExecutive::PLCGetDeviceLinkRate(
        PLC_MACAddress DeviceMACAddress,
        int *NumOfTable,
        PLC_DeviceLinkRateTable *DeviceLinkRateTable) const
{

    QMutexLocker locker(&m_execMutex);

    PLCDriver *plcDriver = m_plcDriverManager->GetPLCDriver(PLCDeviceMacAddr2String(DeviceMACAddress));
    if(plcDriver/* && plcDriver ->PLCGetDeviceLinkRate*/){
        return plcDriver ->PLCGetDeviceLinkRate(DeviceMACAddress, NumOfTable, DeviceLinkRateTable);
    }

    return PLC_GET_FAIL;
}


BOOL PLCDriverExecutive::PLCGetDeviceQoSStatus(PLC_MACAddress DeviceMACAddress, PLC_DeviceQoSStatus *QoSStatus) const
{

    QMutexLocker locker(&m_execMutex);

    PLCDriver *plcDriver = m_plcDriverManager->GetPLCDriver(PLCDeviceMacAddr2String(DeviceMACAddress));
    if(plcDriver/* && plcDriver ->PLCGetDeviceQoSStatus*/){
        return plcDriver ->PLCGetDeviceQoSStatus(DeviceMACAddress, QoSStatus);
    }

    return PLC_GET_FAIL;
}


BOOL PLCDriverExecutive::PLCGetDeviceForwardtable(
        PLC_MACAddress DevMACAddr,
        int *NumOfEntries,
        PLC_DeviceForwardTableList *ForwardTable) const
{

    QMutexLocker locker(&m_execMutex);

    PLCDriver *plcDriver = m_plcDriverManager->GetPLCDriver(PLCDeviceMacAddr2String(DevMACAddr));
    if(plcDriver/* && plcDriver ->PLCGetDeviceForwardtable*/){
        return plcDriver ->PLCGetDeviceForwardtable(DevMACAddr, NumOfEntries, ForwardTable);
    }

    return PLC_GET_FAIL;
}


BOOL PLCDriverExecutive::PLCGetDeviceFWUpgradeVersion (PLC_MACAddress DeviceMACAddress, char *DeviceFWUpgradeVersion) const
{
    QMutexLocker locker(&m_execMutex);

    PLCDriver *plcDriver = m_plcDriverManager->GetPLCDriver(PLCDeviceMacAddr2String(DeviceMACAddress));
    if(plcDriver/* && plcDriver ->PLCGetDeviceFWUpgradeVersion*/){
        return plcDriver ->PLCGetDeviceFWUpgradeVersion(DeviceMACAddress, DeviceFWUpgradeVersion);
    }

    return PLC_GET_FAIL;
}


BOOL PLCDriverExecutive::PLCGetDeviceFWUpgradeRegion (PLC_MACAddress DeviceMACAddress, char *DeviceFWUpgradeResion) const
{
    QMutexLocker locker(&m_execMutex);

    PLCDriver *plcDriver = m_plcDriverManager->GetPLCDriver(PLCDeviceMacAddr2String(DeviceMACAddress));
    if(plcDriver/* && plcDriver ->PLCGetDeviceFWUpgradeRegion*/){
        return plcDriver ->PLCGetDeviceFWUpgradeRegion(DeviceMACAddress, DeviceFWUpgradeResion);
    }

    return PLC_GET_FAIL;
}

BOOL PLCDriverExecutive::PLCSetDeviceName(PLC_MACAddress DeviceMACAddress, const char DeviceName[]) const
{

    QMutexLocker locker(&m_execMutex);

    PLCDriver *plcDriver = m_plcDriverManager->GetPLCDriver(PLCDeviceMacAddr2String(DeviceMACAddress));
    if(plcDriver/* && plcDriver ->PLCSetDeviceName*/){
        return plcDriver ->PLCSetDeviceName(DeviceMACAddress , (char *)DeviceName);
    }

    return PLC_SET_FAIL;
}


BOOL PLCDriverExecutive::PLCSetDeviceLedStatus(PLC_MACAddress DeviceMACAddress, int DeviceLedStatus) const
{

    QMutexLocker locker(&m_execMutex);

    PLCDriver *plcDriver = m_plcDriverManager->GetPLCDriver(PLCDeviceMacAddr2String(DeviceMACAddress));
    if(plcDriver/* && plcDriver ->PLCSetDeviceLedStatus*/){
        return plcDriver ->PLCSetDeviceLedStatus(DeviceMACAddress , (DeviceLedStatus ? PLC_LED_ON : PLC_LED_OFF));
    }

    return PLC_SET_FAIL;
}



int PLCDriverExecutive::PLCSetDeviceSecurityKey(
        PLC_MACAddress DeviceMACAddress,
        const char SecurityKey[],
        const char DevicePassword[]) const
{

    QMutexLocker locker(&m_execMutex);

    PLCDriver *plcDriver = m_plcDriverManager->GetPLCDriver(PLCDeviceMacAddr2String(DeviceMACAddress));
    if(plcDriver/* && plcDriver ->PLCSetDeviceSecurityKey*/){
        return plcDriver ->PLCSetDeviceSecurityKey(DeviceMACAddress , (char*)SecurityKey, (char*)DevicePassword);
    }

    return PLC_SET_FAIL;
}


BOOL PLCDriverExecutive::PLCSetDeviceQoS(PLC_MACAddress DeviceMACAddress, PLC_DeviceQoSStatus* QoSStatus) const
{

    QMutexLocker locker(&m_execMutex);

    PLCDriver *plcDriver = m_plcDriverManager->GetPLCDriver(PLCDeviceMacAddr2String(DeviceMACAddress));
    if(plcDriver/* && plcDriver ->PLCSetDeviceQoS*/){
        return plcDriver ->PLCSetDeviceQoS(DeviceMACAddress , QoSStatus);
    }

    return PLC_SET_FAIL;
}


BOOL PLCDriverExecutive::PLCSetDeviceResetDefault(PLC_MACAddress DeviceMACAddress) const
{

    QMutexLocker locker(&m_execMutex);

    PLCDriver *plcDriver = m_plcDriverManager->GetPLCDriver(PLCDeviceMacAddr2String(DeviceMACAddress));
    if(plcDriver/* && plcDriver ->PLCSetDeviceResetDefault*/){
        return plcDriver ->PLCSetDeviceResetDefault(DeviceMACAddress);
    }

    return PLC_SET_FAIL;
}

BOOL PLCDriverExecutive::PLCCheckDevicePassword(PLC_MACAddress DeviceMACAddress, const char DevicePassword[]) const
{
    QMutexLocker locker(&m_execMutex);

    PLCDriver *plcDriver = m_plcDriverManager->GetPLCDriver(PLCDeviceMacAddr2String(DeviceMACAddress));
    if(plcDriver/* && plcDriver ->PLCCheckDevicePassword*/){
        return plcDriver->PLCCheckDevicePassword(DeviceMACAddress, (char*)DevicePassword);
    }

    return PLC_GET_FAIL;
}
