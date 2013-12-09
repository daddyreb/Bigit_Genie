#include "plcdevicescanner.h"
#include "mainpipeinterface.h"
#include "plcdevice.h"
#include "plctopologydata.h"
#include <QtAlgorithms>

#define MAXIMUM_INACTIVE_DEVICE_CACHE 10

PLCDeviceScanner::PLCDeviceScanner(MainPipeInterface *mainPipeInf, QObject *parent) :
    QObject(parent)
  ,m_mainPipeInf(mainPipeInf)
//  ,m_currentScanningDevIndex(0)
//  ,m_currentScanningEntrieIndex(0)
//  ,m_idleScanning(false)
  ,m_devListChanged(false)
  ,m_gatewayChanged(false)
{
}

PLCDeviceScanner::~PLCDeviceScanner()
{
    QList<PLCDevice *> plcDevices = m_mac2ActiveDeviceMap.values();
    foreach(PLCDevice *dev , plcDevices){
        delete dev;
    }

    plcDevices = m_mac2InactiveDeviceMap.values();
    foreach(PLCDevice *dev , plcDevices){
        delete dev;
    }
}

/*void PLCDeviceScanner::ProcessScanning(bool needReDiscovery)
{
    if(needReDiscovery || m_currentScanningDevList.empty()){
        DiscoverDevice();
        ParseScanningInfo();

        if(m_currentScanningDevList.empty()){
            m_idleScanning = true;
        }
        return;
    }

    if(m_idleScanning){
        DiscoverDevice();
    }

    if(m_currentScanningDevIndex < m_currentScanningDevList.size()){
        m_currentScanningDevList[m_currentScanningDevIndex]->ProcessScanning(m_mainPipeInf);

        if((++m_currentScanningDevIndex) >= m_currentScanningDevList.size()){
            m_currentScanningDevIndex = 0;

            if((++m_currentScanningEntrieIndex) == PLCDevice::SCAN_FUNCTION_COUNT){
                m_currentScanningDevList.clear();
                m_currentScanningEntrieIndex = 0;

                m_idleScanning = true;
            }
        }
    }else{
        m_idleScanning = true;
    }

    ParseScanningInfo();
}

bool PLCDeviceScanner::IdleScanning() const
{
    qDebug () << "PLC Device idle scanning :" << (m_idleScanning ? "YES" : "NO");
    return m_idleScanning;
}*/


void PLCDeviceScanner::DiscoverDevice()
{
    if(m_mac2InactiveDeviceMap.size() > MAXIMUM_INACTIVE_DEVICE_CACHE){//keep m_mac2InactiveDeviceMap from increasing all the time
        qDeleteAll(m_mac2InactiveDeviceMap.values());
        m_mac2InactiveDeviceMap.clear();
    }
    //m_devListChanged = false;

    QStringList devsDiscovered;
    m_mainPipeInf->PLCDiscovery(devsDiscovered);

    QStringList macKeys = m_mac2ActiveDeviceMap.keys();

    foreach(QString key, macKeys){
        if(!devsDiscovered.contains(key)){
            m_mac2InactiveDeviceMap[key] = m_mac2ActiveDeviceMap[key];
            m_mac2ActiveDeviceMap.remove(key);
            m_devListChanged = true;
        }
    }

    int devCount = devsDiscovered.count();
    for(int devIdx = 0 ; devIdx < devCount ; ++devIdx){
        if(!m_mac2ActiveDeviceMap.contains(devsDiscovered[devIdx])){
            if(m_mac2InactiveDeviceMap.contains(devsDiscovered[devIdx])){
                m_mac2ActiveDeviceMap[devsDiscovered[devIdx]] = m_mac2InactiveDeviceMap[devsDiscovered[devIdx]];
                m_mac2InactiveDeviceMap.remove(devsDiscovered[devIdx]);
            }else{
                m_mac2ActiveDeviceMap[devsDiscovered[devIdx]] = new PLCDevice(devsDiscovered[devIdx]);
            }

            m_devListChanged = true;
        }
    }

    QString curLocalDeviceMac;

    m_mainPipeInf->PLCGetLocalDevice(curLocalDeviceMac);

    if(m_currentLocalDeviceMac != curLocalDeviceMac){
        m_currentLocalDeviceMac = curLocalDeviceMac;
        m_devListChanged = true;
    }


    //update scanning list
    m_currentScanningDevList = m_mac2ActiveDeviceMap.values();

    //m_idleScanning = (m_devListChanged ? !m_devListChanged : m_idleScanning);

    //    QString activeMacs;
    //    foreach(PLCDevice *dev , m_currentScanningDevList){
    //        activeMacs += dev->deviceMac();
    //        activeMacs += " ";
    //    }

    //    qDebug() << "Active devices : " << activeMacs;
    //    qDebug() << "Current local device : " << m_currentLocalDeviceMac;
}

void PLCDeviceScanner::DiscoverGatewayPhysicalAddrs()
{
    QStringList macs;
    if(m_mainPipeInf->GetGatewayPhysicalAddrs(macs)){
        qSort(macs);
        qSort(m_gatewayPhysicalAddrs);

        if(m_gatewayPhysicalAddrs != macs){
            m_gatewayPhysicalAddrs = macs;
            m_gatewayChanged = true;
        }
    }
}

void PLCDeviceScanner::ParseScanningInfo(bool notifyChangeOnly)
{
    bool devChangeScanned = (m_devListChanged || m_gatewayChanged);
    int devCount = m_currentScanningDevList.count();

    for(int idx = 0 ; (!devChangeScanned) && (idx < devCount) ; ++idx){
        devChangeScanned = (devChangeScanned || m_currentScanningDevList[idx]->DeviceChangeScanned());
    }

    if(notifyChangeOnly && !devChangeScanned){//no change scanned
        return;
    }

    PLCTopologyData *topData = new PLCTopologyData();
    topData->ParseScannedData(m_currentScanningDevList, m_gatewayPhysicalAddrs, m_currentLocalDeviceMac);

    //reset change scanned so far
    m_devListChanged = false;
    m_gatewayChanged = false;

    foreach(PLCDevice *dev, m_currentScanningDevList){
        dev->ResetScannedChangeFlag();
    }

    qDebug () << "PLCDeviceScanner::ParseScanningInfo notify new data!!!";
    emit PLCDeviceChangeScanned(TopologyDataTransfer(topData));

}


//---------------implement device information interface-------------------------

bool PLCDeviceScanner::GetDeviceName(const QString &devMac ,QString &devName)
{
    bool optOk = false;

    if((optOk = m_mac2ActiveDeviceMap.contains(devMac))){
        if(!(m_mac2ActiveDeviceMap[devMac]->DeviceValidFileds() & PLCDevice::PLC_DEVICE_NAME_BIT)){
            optOk = m_mac2ActiveDeviceMap[devMac]->ScanName(m_mainPipeInf);
        }

        devName = m_mac2ActiveDeviceMap[devMac]->deviceName();
    }

    return optOk;
}

bool PLCDeviceScanner::ReSynchScan()
{
    DiscoverDevice();
    DiscoverGatewayPhysicalAddrs();

    int devCount = m_currentScanningDevList.size();
    for(int idx = 0 ; idx < PLCDevice::SCAN_FUNCTION_COUNT ; ++idx){
        for(int index = 0 ; index < devCount ; ++index){
            m_currentScanningDevList[index]->ProcessScanning(m_mainPipeInf);
        }
    }

//    m_currentScanningDevIndex = 0;
//    m_currentScanningEntrieIndex = 0;
//    m_idleScanning = true;

    for(int index = 0 ; index < devCount ; ++index){
        m_currentScanningDevList[index]->WaitSoapEnd();
    }

    ParseScanningInfo(false);

    return true;
}

QString PLCDeviceScanner::CurrentLocalDevice()
{
    return m_currentLocalDeviceMac;
}

