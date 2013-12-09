#include "plcdevice.h"
#include <QtDebug>
#include <QHostInfo>
#include <QHostAddress>
#include <QList>
#include <QtAlgorithms>


#ifndef _MSC_VER
const int PLCDevice::SCAN_FUNCTION_COUNT;
#endif

#define SOAP_PORT 80

PLCDevice::PLCDevice(const QString &deviceMac):
    m_tmpDevFieldValidMask(0)
  ,m_tmpDevFieldChangeMask(0)
  ,m_currentScanningEntryIndex(0)
  ,m_devFieldValidMask(0)
  ,m_devFieldChangeMask(0)
  ,m_wirelessSupport(false)
  ,m_usbSupport(false)
  ,m_devMAC(deviceMac)
  ,m_devLEDOn(false)

{
    InitScanFunctionArray();
}

void PLCDevice::InitScanFunctionArray()
{
    ScanFunArray[0] = &PLCDevice::ScanIP;
    ScanFunArray[1] = &PLCDevice::ScanModel;
    ScanFunArray[2] = &PLCDevice::ScanName;
    ScanFunArray[3] = &PLCDevice::ScanLEDState;
    ScanFunArray[4] = &PLCDevice::ScanFirmwareVersion;
    ScanFunArray[5] = &PLCDevice::ScanLinkRateTable;
    ScanFunArray[6] = &PLCDevice::ScanForwardTable;
    ScanFunArray[7] = &PLCDevice::ScanNetworkType;
    ScanFunArray[8] = &PLCDevice::ScanDriverModule;
    //    ScanFunArray[9] = &PLCDevice::ScanAttachDevice;
    //    ScanFunArray[10] = &PLCDevice::ScanAttachUsbDevice;
    //    ScanFunArray[11] = &PLCDevice::ScanWLANInfo;
}


void PLCDevice::ProcessScanning(MainPipeInterface *mainPipeInf)
{
    if(m_currentScanningEntryIndex < SCAN_FUNCTION_COUNT){
        (this->*(ScanFunArray[m_currentScanningEntryIndex]))(mainPipeInf);
        m_currentScanningEntryIndex = ((++m_currentScanningEntryIndex) % SCAN_FUNCTION_COUNT);
    }
}

void PLCDevice::WaitSoapEnd()
{
    m_soapThread.wait();
    m_devFieldValidMask |= m_tmpDevFieldValidMask;
    m_devFieldChangeMask |= m_tmpDevFieldChangeMask;
}

void PLCDevice::StartSoapProcess(MainPipeInterface *mainPipeInf)
{
    m_tmpDevFieldValidMask = 0;
    m_tmpDevFieldChangeMask = 0;
    m_soapThread.begin(this, mainPipeInf);
}

quint16 PLCDevice::DeviceChangeScanned() const
{
    return m_devFieldChangeMask;
}

quint16 PLCDevice::DeviceValidFileds() const
{
    return m_devFieldValidMask;
}

void PLCDevice::ResetScannedChangeFlag()
{
    m_devFieldChangeMask = 0;

    m_tmpDevFieldChangeMask = 0;
}

bool PLCDevice::ScanIP(MainPipeInterface *mainPipeInf)
{
    QString devIP;
    if(mainPipeInf->GetPLCDeviceIP(m_devMAC, devIP)){
        if(m_devIP != devIP){
            m_devIP = devIP;
            m_devFieldChangeMask |= PLC_DEVICE_IP_BIT;
        }
        m_devFieldValidMask |= PLC_DEVICE_IP_BIT;
        StartSoapProcess(mainPipeInf);
        return true;
    }

    QHostInfo hostInfo = QHostInfo::fromName(m_devName);
    QList<QHostAddress> addrList = hostInfo.addresses();
    if(!addrList.isEmpty()){
        foreach(QHostAddress addr, addrList){
            if(addr.isNull()){
                continue;
            }
            if(m_devIP != addr.toString()){
                m_devIP = addr.toString();
                m_devFieldChangeMask |= PLC_DEVICE_IP_BIT;
            }
            m_devFieldValidMask |= PLC_DEVICE_IP_BIT;
            StartSoapProcess(mainPipeInf);
            return true;
        }
    }

    return false;
}

bool PLCDevice::ScanName(MainPipeInterface *mainPipeInf)
{
    bool bResult = false;
    QString devName;
    if((bResult = mainPipeInf->PLCGetDeviceName(m_devMAC, devName))){
        m_devFieldValidMask |= PLC_DEVICE_NAME_BIT;
        if(m_devName != devName){
            m_devName = devName;
            m_devFieldChangeMask |= PLC_DEVICE_NAME_BIT;
        }
    };

    //qDebug() << "Get device name:" << (m_devFieldValidMask & PLC_DEVICE_NAME_BIT);
    //qDebug() << "PLC Device : " << m_devMAC << " device name: " << devName;

    return bResult;
}

bool PLCDevice::ScanModel(MainPipeInterface *mainPipeInf)
{
    bool bResult = false;

    QString devModel;
    if((bResult = mainPipeInf->PLCGetDeviceModelName(m_devMAC, devModel))){
        m_devFieldValidMask |= PLC_DEVICE_MODEL_BIT;
        if(m_devModel != devModel){
            m_devModel = devModel;
            m_devFieldChangeMask |= PLC_DEVICE_MODEL_BIT;
        }
    };

    //qDebug() << "Get device model:" << (m_devFieldValidMask & PLC_DEVICE_MODEL_BIT);
    //qDebug() << "PLC Device : " << m_devMAC << " device model: " << devModel;

    return bResult;
}

bool PLCDevice::ScanLEDState(MainPipeInterface *mainPipeInf)
{
    bool bResult = false;

    int ledState = 0;
    if((bResult = mainPipeInf->PLCGetDeviceLedStatus(m_devMAC, &ledState))){
        m_devFieldValidMask |= PLC_DEVICE_LEDSTATE_BIT;
        if(m_devLEDOn != bool(ledState)){
            m_devLEDOn = bool(ledState);
            m_devFieldChangeMask |= PLC_DEVICE_LEDSTATE_BIT;
        }
    };

    //qDebug() << "Get device LED state:" << (m_devFieldValidMask & PLC_DEVICE_LEDSTATE_BIT);
    //qDebug() << "PLC Device : " << m_devMAC << " device  LED : " << (ledState ? "ON" : "OFF");

    return bResult;
}

bool PLCDevice::ScanFirmwareVersion(MainPipeInterface *mainPipeInf)
{
    bool bResult = false;

    QString devFWVersion;
    if((bResult = mainPipeInf->PLCGetDeviceFWVersion(m_devMAC, devFWVersion))){
        m_devFieldValidMask |= PLC_DEVICE_FWVERSION_BIT;
        if(m_devFWVersion != devFWVersion){
            m_devFWVersion = devFWVersion;
            m_devFieldChangeMask |= PLC_DEVICE_FWVERSION_BIT;
        }
    };

    //qDebug() << "Get device firmware version:" << (m_devFieldValidMask & PLC_DEVICE_FWVERSION_BIT);
    //qDebug() << "PLC Device : " << m_devMAC << " device  firmware version: " << devFWVersion;

    return bResult;
}

bool PLCDevice::ScanLinkRateTable(MainPipeInterface *mainPipeInf)
{
    bool bResult = false;

    QMap<QString,QPair<qint16,qint16> > linkRateTable;

    if((bResult = mainPipeInf->PLCGetDeviceLinkRate(m_devMAC, linkRateTable))){
        m_devFieldValidMask |= PLC_DEVICE_LINKRATETABLE_BIT;
        if(m_linkRateTable != linkRateTable){
            m_linkRateTable = linkRateTable;
            m_devFieldChangeMask |= PLC_DEVICE_LINKRATETABLE_BIT;
        }
    };

    //qDebug() << "Get device link rate table:" << (m_devFieldValidMask & PLC_DEVICE_LINKRATETABLE_BIT);
    //qDebug() << "PLC Device : " << m_devMAC << " device  link rate table : " << linkRateTable;

    return bResult;
}

bool PLCDevice::ScanForwardTable(MainPipeInterface *mainPipeInf)
{
    bool bResult = false;

    QStringList forwardTable;

    if((bResult = mainPipeInf->PLCGetDeviceForwardtable(m_devMAC, forwardTable))){
        m_devFieldValidMask |= PLC_DEVICE_FORWARDTABLE_BIT;
        if(m_forwardTable != forwardTable){
            m_forwardTable = forwardTable;
            m_devFieldChangeMask |= PLC_DEVICE_FORWARDTABLE_BIT;
        }
    };

    //qDebug() << "Get device forward table:" << (m_devFieldValidMask & PLC_DEVICE_FORWARDTABLE_BIT);
    //qDebug() << "PLC Device : " << m_devMAC << " device  forward table : " << forwardTable;

    return bResult;
}

bool PLCDevice::ScanNetworkType(MainPipeInterface *mainPipeInf)
{
    bool bResult = false;

    QString networkType;
    if((bResult = mainPipeInf->PLCGetNetworkType(m_devMAC, networkType))){
        m_devFieldValidMask |= PLC_DEVICE_NETWORKTYPE_BIT;
        if(m_networkType != networkType){
            m_networkType = networkType;
            m_devFieldChangeMask |= PLC_DEVICE_NETWORKTYPE_BIT;
        }
    };

    //qDebug() << "Get device network type:" << (m_devFieldValidMask & PLC_DEVICE_NETWORKTYPE_BIT);
    //qDebug() << "PLC Device : " << m_devMAC << " device  network type: " << networkType;

    return bResult;
}

bool PLCDevice::ScanDriverModule(MainPipeInterface *mainPipeInf)
{
    bool bResult = false;

    QString driverModule = mainPipeInf->GetDeviceDriverFilePath(m_devMAC);
    if((bResult = !driverModule.isEmpty())){
        m_devFieldValidMask |= PLC_DEVICE_DRIVERMODULE_BIT;
        if(m_driverModule != driverModule){
            m_driverModule = driverModule;
            m_devFieldChangeMask |= PLC_DEVICE_DRIVERMODULE_BIT;
        }
    };

    return bResult;
}


bool PLCDevice::ScanAttachDevice(MainPipeInterface *mainPipeInf)
{
    if(m_devIP.isEmpty()){
        return false;
    }

    QList<AttachDeviceEntry> attachDevices;
    if(mainPipeInf->SoapGetAttachDevice(m_devIP, SOAP_PORT, attachDevices) != SOAP_OK){
        return false;
    }

    //m_devFieldValidMask |= PLC_DEVICE_ATTACHDEVICE_BIT;
    m_tmpDevFieldValidMask |= PLC_DEVICE_ATTACHDEVICE_BIT;

    if(!IsAttachDeviceEqual(m_attachDeviceList, attachDevices)){
        m_attachDeviceList = attachDevices;
        //m_devFieldChangeMask |= PLC_DEVICE_ATTACHDEVICE_BIT;
        m_tmpDevFieldChangeMask |= PLC_DEVICE_ATTACHDEVICE_BIT;
    }

    //    QString devsString;
    //    for(int idx = 0 ; idx < attachDevices.size() ; ++idx){
    //        if(0 == idx){
    //            devsString += "<";
    //        }

    //        devsString += attachDevices[idx].toString();

    //        if(idx == (attachDevices.size() - 1)){
    //            devsString += ">";
    //        }else{
    //            devsString += " ; ";
    //        }
    //    }

    //qDebug () << "PLC Device : " << m_devMAC << "Scan attach devices :" << devsString;

    return true;
}


bool PLCDevice::ScanAttachUsbDevice(MainPipeInterface *mainPipeInf)
{
    if(m_devIP.isEmpty()){
        return false;
    }

    QStringList usbTypes;
    if(mainPipeInf->SoapGetUSBType(m_devIP, SOAP_PORT, usbTypes) != SOAP_OK){
        return false;
    }

    QStringList usbDescs;
    if(mainPipeInf->SoapGetUSBDesc(m_devIP, SOAP_PORT, usbDescs) != SOAP_OK){
        return false;
    }

    m_usbSupport = true;

    if(usbTypes.size() != usbDescs.size()){
        qDebug () << "The number of Usb types not equal the number of usb descriptions";
        return false;
    }

    //m_devFieldValidMask |= PLC_DEVICE_ATTACHUSBDEVICE_BIT;
    m_tmpDevFieldValidMask |= PLC_DEVICE_ATTACHUSBDEVICE_BIT;

    if(!IsContentEqual(m_usbTypes, usbTypes) || !IsContentEqual(m_usbDescs, usbDescs)){
        m_usbTypes = usbTypes;
        m_usbDescs = usbDescs;
        int count = m_usbTypes.size();
        m_attachUsbDeviceList.clear();

        for(int idx = 0 ; idx < count ; ++idx){
            m_attachUsbDeviceList.append(QPair<QString,QString>(m_usbTypes[idx],m_usbDescs[idx]));
        }

        //m_devFieldChangeMask |= PLC_DEVICE_ATTACHUSBDEVICE_BIT;
        m_tmpDevFieldChangeMask |= PLC_DEVICE_ATTACHUSBDEVICE_BIT;
    }

    //qDebug () << "PLC Device : " << m_devMAC << "Scan attach usb device :" << m_attachUsbDeviceList;

    return true;
}

bool PLCDevice::ScanWLANInfo(MainPipeInterface *mainPipeInf)
{
    if(m_devIP.isEmpty()){
        return false;
    }

    WLANInfo wlanInfo;
    if(mainPipeInf->SoapGetWLANInfo(m_devIP, SOAP_PORT, wlanInfo) != SOAP_OK){
        return false;
    }

    m_wirelessSupport = !wlanInfo.m_ssid.isEmpty();

    //m_devFieldValidMask |= PLC_DEVICE_WLANINFO_BIT;
    m_tmpDevFieldValidMask |= PLC_DEVICE_WLANINFO_BIT;

    if(!IsWLANInfoEqual(m_wlanInfo, wlanInfo)){
        m_wlanInfo = wlanInfo;
        //m_devFieldChangeMask |= PLC_DEVICE_WLANINFO_BIT;
        m_tmpDevFieldChangeMask |= PLC_DEVICE_WLANINFO_BIT;
    }

    //qDebug () << "PLC Device : " << m_devMAC << "Scan WLAN info :" << m_wlanInfo.m_ssid;

    return true;
}

bool PLCDevice::IsWirelessSupport() const
{
    return m_wirelessSupport;
}

bool PLCDevice::IsUsbSupport() const
{
    return m_usbSupport;
}

bool PLCDevice::IsContentEqual(const QStringList &left, const QStringList &right) const
{
    bool equal = (left.size() == right.size());
    if(equal){
        foreach(QString item, left){
            if(!right.contains(item)){
                equal = false;
                break;
            }
        }
    }//if

    return equal;
}

static bool AttachDeviceEntryPtrLess(const AttachDeviceEntry *left, const AttachDeviceEntry *right )
{
    return left->m_devMac < right->m_devMac;
}

bool PLCDevice::IsAttachDeviceEqual(const QList<AttachDeviceEntry> &left, const QList<AttachDeviceEntry> &right) const
{
    if(left.size() != right.size()){
        return false;
    }

    QList<const AttachDeviceEntry *> leftps;
    QList<const AttachDeviceEntry *> rightps;

    const int psCount = left.size();

    for(int l_idx = 0 ; l_idx < psCount ; ++l_idx){
        leftps.append(&left[l_idx]);
    }

    for(int r_idx = 0 ; r_idx < psCount ; ++r_idx){
        rightps.append(&right[r_idx]);
    }

    //sort
    qSort(leftps.begin(), leftps.end(), AttachDeviceEntryPtrLess);
    qSort(rightps.begin(), rightps.end(), AttachDeviceEntryPtrLess);

    for(int idx = 0 ; idx < psCount ; ++idx){
        if(!(*(leftps[idx]) == *(rightps[idx]))){
            return false;
        }
    }

    return true;
}

bool PLCDevice::IsWLANInfoEqual(const WLANInfo &left, const WLANInfo &right) const
{
    return (left.m_enabled == right.m_enabled &&
            left.m_basicEncryptionModes == right.m_basicEncryptionModes &&
            left.m_channel == right.m_channel &&
            left.m_region == right.m_region &&
            left.m_ssid == right.m_ssid &&
            left.m_ssidBroadcast == right.m_ssidBroadcast &&
            left.m_statusUp == right.m_statusUp &&
            left.m_wpsConfigured == right.m_wpsConfigured &&
            left.m_wepAuthType == right.m_wepAuthType &&
            left.m_wirelessMode == right.m_wirelessMode &&
            left.m_wlanMACAddress == right.m_wlanMACAddress &&
            left.m_wpaEncryptionModes == right.m_wpaEncryptionModes &&
            left.m_wlanPassphrase == right.m_wlanPassphrase &&
            left.m_wepKey == right.m_wepKey);
}

//---------------------------------------------------------------------------------
QString PLCDevice::deviceMac() const
{
    return m_devMAC;
}

QString PLCDevice::deviceIP() const
{
    return m_devIP;
}

bool PLCDevice::deviceLEDState() const
{
    return m_devLEDOn;
}

QString PLCDevice::deviceName() const
{
    return m_devName;
}

QString PLCDevice::deviceModel() const
{
    return m_devModel;
}

QString PLCDevice::deviceFWVersion() const
{
    return m_devFWVersion;
}

QMap<QString,QPair<qint16,qint16> > PLCDevice::deviceLinkRateTable() const
{
    return m_linkRateTable;
}

QStringList PLCDevice::deviceForwardTable() const
{
    return m_forwardTable;
}

QString PLCDevice::deviceNetworkType() const
{
    return m_networkType;
}

QString PLCDevice::deviceDriverPath() const
{
    return m_driverModule;
}

QList<AttachDeviceEntry> PLCDevice::deviceAttachDevices() const
{
    return m_attachDeviceList;
}

QList<QPair<QString,QString> > PLCDevice::deviceAttachUsbDevices() const
{
    return m_attachUsbDeviceList;
}

WLANInfo PLCDevice::deviceWLANInfo() const
{
    return m_wlanInfo;
}
