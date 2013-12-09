#include "plcdriver.h"
#include <QLibrary>

PLCDriver::PLCDriver(QLibrary *libDriver):
    m_libDriver(libDriver)
{
    ResolveSymbols();
}

PLCDriver::~PLCDriver()
{
    if(m_libDriver != NULL){
        //m_libDriver->unload();
        delete m_libDriver;
    }
}

void PLCDriver::ResolveSymbols()
{
#ifndef DUMP_LAYER2_API
    PLCDiscovery = (PLCDiscovery_t)  m_libDriver->resolve("PLCDiscovery");

    PLCGetLocalDevice = (PLCGetLocalDevice_t) m_libDriver->resolve("PLCGetLocalDevice");
    PLCGetDeviceModelName = (PLCGetDeviceModelName_t) m_libDriver->resolve("PLCGetDeviceModelName");
    PLCGetDeviceFWVersion = (PLCGetDeviceFWVersion_t) m_libDriver->resolve("PLCGetDeviceFWVersion");
    PLCGetDeviceName = (PLCGetDeviceName_t) m_libDriver->resolve("PLCGetDeviceName");
    PLCGetNetworkType = (PLCGetNetworkType_t) m_libDriver->resolve("PLCGetNetworkType");
    PLCGetDeviceLedStatus = (PLCGetDeviceLedStatus_t) m_libDriver->resolve("PLCGetDeviceLedStatus");
    PLCGetDeviceLinkRate = (PLCGetDeviceLinkRate_t) m_libDriver->resolve("PLCGetDeviceLinkRate");
    PLCGetDeviceQoSStatus = (PLCGetDeviceQoSStatus_t) m_libDriver->resolve("PLCGetDeviceQoSStatus");
    PLCGetDeviceForwardtable = (PLCGetDeviceForwardtable_t) m_libDriver->resolve("PLCGetDeviceForwardtable");
    PLCGetDeviceFWUpgradeVersion = (PLCGetDeviceFWUpgradeVersion_t) m_libDriver->resolve("PLCGetDeviceFWUpgradeVersion");
    PLCGetDeviceFWUpgradeRegion = (PLCGetDeviceFWUpgradeRegion_t) m_libDriver->resolve("PLCGetDeviceFWUpgradeRegion");

    PLCSetDeviceName = (PLCSetDeviceName_t) m_libDriver->resolve("PLCSetDeviceName");
    PLCSetDeviceLedStatus = (PLCSetDeviceLedStatus_t) m_libDriver->resolve("PLCSetDeviceLedStatus");
    PLCSetDeviceSecurityKey = (PLCSetDeviceSecurityKey_t) m_libDriver->resolve("PLCSetDeviceSecurityKey");
    PLCSetDeviceQoS = (PLCSetDeviceQoS_t) m_libDriver->resolve("PLCSetDeviceQoS");
    PLCSetDeviceResetDefault = (PLCSetDeviceResetDefault_t) m_libDriver->resolve("PLCSetDeviceResetDefault");
    PLCCheckDevicePassword = (PLCCheckDevicePassword_t) m_libDriver->resolve("PLCCheckDevicePassword");
#else
    PLCDiscovery_f = (PLCDiscovery_t)  m_libDriver->resolve("PLCDiscovery");

    PLCGetLocalDevice_f = (PLCGetLocalDevice_t) m_libDriver->resolve("PLCGetLocalDevice");
    PLCGetDeviceModelName_f = (PLCGetDeviceModelName_t) m_libDriver->resolve("PLCGetDeviceModelName");
    PLCGetDeviceFWVersion_f = (PLCGetDeviceFWVersion_t) m_libDriver->resolve("PLCGetDeviceFWVersion");
    PLCGetDeviceName_f = (PLCGetDeviceName_t) m_libDriver->resolve("PLCGetDeviceName");
    PLCGetNetworkType_f = (PLCGetNetworkType_t) m_libDriver->resolve("PLCGetNetworkType");
    PLCGetDeviceLedStatus_f = (PLCGetDeviceLedStatus_t) m_libDriver->resolve("PLCGetDeviceLedStatus");
    PLCGetDeviceLinkRate_f = (PLCGetDeviceLinkRate_t) m_libDriver->resolve("PLCGetDeviceLinkRate");
    PLCGetDeviceQoSStatus_f = (PLCGetDeviceQoSStatus_t) m_libDriver->resolve("PLCGetDeviceQoSStatus");
    PLCGetDeviceForwardtable_f = (PLCGetDeviceForwardtable_t) m_libDriver->resolve("PLCGetDeviceForwardtable");
    PLCGetDeviceFWUpgradeVersion_f = (PLCGetDeviceFWUpgradeVersion_t) m_libDriver->resolve("PLCGetDeviceFWUpgradeVersion");
    PLCGetDeviceFWUpgradeRegion_f = (PLCGetDeviceFWUpgradeRegion_t) m_libDriver->resolve("PLCGetDeviceFWUpgradeRegion");

    PLCSetDeviceName_f = (PLCSetDeviceName_t) m_libDriver->resolve("PLCSetDeviceName");
    PLCSetDeviceLedStatus_f = (PLCSetDeviceLedStatus_t) m_libDriver->resolve("PLCSetDeviceLedStatus");
    PLCSetDeviceSecurityKey_f = (PLCSetDeviceSecurityKey_t) m_libDriver->resolve("PLCSetDeviceSecurityKey");
    PLCSetDeviceQoS_f = (PLCSetDeviceQoS_t) m_libDriver->resolve("PLCSetDeviceQoS");
    PLCSetDeviceResetDefault_f = (PLCSetDeviceResetDefault_t) m_libDriver->resolve("PLCSetDeviceResetDefault");
    PLCCheckDevicePassword_f = (PLCCheckDevicePassword_t) m_libDriver->resolve("PLCCheckDevicePassword");
#endif
}

QString PLCDriver::GetLibraryPathName() const
{
    if(m_libDriver != NULL){
        return m_libDriver->fileName();
    }else{
        return QString();
    }
}

#ifdef DUMP_LAYER2_API
#include <QTextStream>
#include <QFile>
#include <QDir>
#include <QDesktopServices>
#include <QDateTime>
#include "helperroutines.h"

void PLCDriver::dumpMsg(const QString &msg) const
{
    const QString dumpFileName = "plc_layer2api_dump.txt";
    const QString netgearGenieDirName = "NETGEARGenie";
    const QString strAppDataDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation);


    if(!QDir(strAppDataDir + QDir::separator() + netgearGenieDirName).exists()){
        QDir appDataDir(strAppDataDir);
        if(!appDataDir.mkdir(netgearGenieDirName)){
            return;
        }
    }

    //QFile file(strAppDataDir + QDir::separator() + netgearGenieDirName + QDir::separator() + dumpFileName);
    QFile file(strAppDataDir + QDir::separator() + dumpFileName);
    if(file.size() > 200*1024){//200KB
        file.resize(0);
    }

    file.open(QFile::WriteOnly | QFile::Text | QFile::Append);
    QTextStream dumpStream(&file);

    dumpStream << "[" + QDateTime::currentDateTime().toString("dd.MM.yyyy-hh:mm:ss.zzz")+ "]" + " <driver:" + GetLibraryPathName() + "> " + msg << "\r\n";

    dumpStream.flush();
    file.close();
}

BOOL PLCDriver::PLCDiscovery(int *count, PLC_DiscoveryDeviceList *devList)
{
    BOOL retCode = PLCDiscovery_f(count, devList);
    QString tmp;
    QString msg = "PLCDiscovery return code = ";
    msg += tmp.setNum(retCode);

    if(retCode){
        msg += ",count = ";
        msg += tmp.setNum(*count);
        msg += ",Device List = [";
        for(int idx = 0 ; idx < *count ; ++idx){
            msg += PLCDeviceMacAddr2String(devList->PLC_DeviceMACAddr[idx]);
            if(idx != (*count - 1)){
                msg += ",";
            }
        }
        msg += "].";
    }else{
        msg += ".";
    }

    dumpMsg(msg);

    return retCode;
}

BOOL PLCDriver::PLCGetLocalDevice(PLC_MACAddress mac)
{
    BOOL retCode = PLCGetLocalDevice_f(mac);
    QString tmp;
    QString msg = "PLCGetLocalDevice return code = ";
    msg += tmp.setNum(retCode);

    if(retCode){
        msg += ", local device mac = ";
        msg += PLCDeviceMacAddr2String(mac);
    }

    msg += ".";

    dumpMsg(msg);

    return retCode;
}

BOOL PLCDriver::PLCGetDeviceModelName(PLC_MACAddress mac, char *model)
{
    BOOL retCode = PLCGetDeviceModelName_f(mac, model);
    QString tmp;
    QString msg = "PLCGetDeviceModelName return code = ";
    msg += tmp.setNum(retCode);
    msg += ",target dev mac = ";
    msg += PLCDeviceMacAddr2String(mac);

    if(retCode){
        msg += ", Device Model Name = ";
        msg += model;
    }

    msg += ".";

    dumpMsg(msg);

    return retCode;

}

BOOL PLCDriver::PLCGetDeviceFWVersion(PLC_MACAddress mac, char *Fwversion)
{
    BOOL retCode = PLCGetDeviceFWVersion_f(mac, Fwversion);
    QString tmp;
    QString msg = "PLCGetDeviceFWVersion return code = ";
    msg += tmp.setNum(retCode);
    msg += ",target dev mac = ";
    msg += PLCDeviceMacAddr2String(mac);

    if(retCode){
        msg += ", Firmware Version = ";
        msg += Fwversion;
    }

    msg += ".";

    dumpMsg(msg);

    return retCode;
}

BOOL PLCDriver::PLCGetDeviceName(PLC_MACAddress mac, char *devName)
{
    BOOL retCode = PLCGetDeviceName_f(mac, devName);
    QString tmp;
    QString msg = "PLCGetDeviceName return code = ";
    msg += tmp.setNum(retCode);
    msg += ",target dev mac = ";
    msg += PLCDeviceMacAddr2String(mac);

    if(retCode){
        msg += ", Device Name = ";
        msg += devName;
    }

    msg += ".";

    dumpMsg(msg);

    return retCode;
}

BOOL PLCDriver::PLCGetNetworkType(PLC_MACAddress mac, char *netType)
{
    BOOL retCode = PLCGetNetworkType_f(mac, netType);
    QString tmp;
    QString msg = "PLCGetNetworkType return code = ";
    msg += tmp.setNum(retCode);
    msg += ",target dev mac = ";
    msg += PLCDeviceMacAddr2String(mac);

    if(retCode){
        msg += ", Network Type = ";
        msg += netType;
    }

    msg += ".";

    dumpMsg(msg);

    return retCode;

}

BOOL PLCDriver::PLCGetDeviceLedStatus(PLC_MACAddress mac, int *ledStatus)
{
    BOOL retCode = PLCGetDeviceLedStatus_f(mac, ledStatus);
    QString tmp;
    QString msg = "PLCGetDeviceLedStatus return code = ";
    msg += tmp.setNum(retCode);
    msg += ",target dev mac = ";
    msg += PLCDeviceMacAddr2String(mac);

    if(retCode){
        msg += ", LED Status = ";
        msg += tmp.setNum(*ledStatus);
    }

    msg += ".";

    dumpMsg(msg);

    return retCode;

}

BOOL PLCDriver::PLCGetDeviceLinkRate(PLC_MACAddress mac, int *count, PLC_DeviceLinkRateTable *linkRateTable)
{
    BOOL retCode = PLCGetDeviceLinkRate_f(mac, count, linkRateTable);
    QString tmp;
    QString msg = "PLCGetDeviceLinkRate return code = ";
    msg += tmp.setNum(retCode);
    msg += ",target dev mac = ";
    msg += PLCDeviceMacAddr2String(mac);

    if(retCode){
        msg += ",count = ";
        msg += tmp.setNum(*count);
        msg += ",Link Rate Table = [";
        for(int idx = 0 ; idx < *count ; ++idx){
            msg += "(mac:";
            msg += PLCDeviceMacAddr2String(linkRateTable->PLC_DeviceMACAddr[idx]);
            msg += " Rx:";
            msg += tmp.setNum(linkRateTable->PLC_DeviceRXRate[idx]);
            msg += " Tx:";
            msg += tmp.setNum(linkRateTable->PLC_DeviceTXRate[idx]);
            msg += ")";
            if(idx != (*count - 1)){
                msg += ",";
            }
        }
        msg += "].";
    }else{
        msg += ".";
    }

    dumpMsg(msg);

    return retCode;
}

BOOL PLCDriver::PLCGetDeviceQoSStatus(PLC_MACAddress mac, PLC_DeviceQoSStatus *qosStatus)
{
    return PLCGetDeviceQoSStatus_f(mac, qosStatus);
}

BOOL PLCDriver::PLCGetDeviceForwardtable(PLC_MACAddress mac, int *count, PLC_DeviceForwardTableList *forwardTable)
{
    BOOL retCode = PLCGetDeviceForwardtable_f(mac, count, forwardTable);
    QString tmp;
    QString msg = "PLCGetDeviceForwardtable return code = ";
    msg += tmp.setNum(retCode);
    msg += ",target dev mac = ";
    msg += PLCDeviceMacAddr2String(mac);

    if(retCode){
        msg += ",count = ";
        msg += tmp.setNum(*count);
        msg += ",Forward Table = [";
        for(int idx = 0 ; idx < *count ; ++idx){
            msg += PLCDeviceMacAddr2String(forwardTable->PLC_DeviceForwardMACAddr[idx]);
            if(idx != (*count - 1)){
                msg += ",";
            }
        }
        msg += "].";
    }else{
        msg += ".";
    }

    dumpMsg(msg);

    return retCode;
}

BOOL PLCDriver::PLCGetDeviceFWUpgradeVersion(PLC_MACAddress mac, char* FwUpVersion)
{
    BOOL retCode = PLCGetDeviceFWUpgradeVersion_f(mac, FwUpVersion);
    QString tmp;
    QString msg = "PLCGetDeviceFWUpgradeVersion return code = ";
    msg += tmp.setNum(retCode);
    msg += ",target dev mac = ";
    msg += PLCDeviceMacAddr2String(mac);

    if(retCode){
        msg += ", Firmware Upgrade Version = ";
        msg += FwUpVersion;
    }

    msg += ".";

    dumpMsg(msg);

    return retCode;
}

BOOL PLCDriver::PLCGetDeviceFWUpgradeRegion(PLC_MACAddress mac, char* FwUpRegion)
{
    BOOL retCode = PLCGetDeviceFWUpgradeRegion_f(mac, FwUpRegion);

    QString tmp;
    QString msg = "PLCGetDeviceFWUpgradeRegion return code = ";
    msg += tmp.setNum(retCode);
    msg += ",target dev mac = ";
    msg += PLCDeviceMacAddr2String(mac);

    if(retCode){
        msg += ", Firmware Upgrade Region = ";
        msg += FwUpRegion;
    }

    msg += ".";

    dumpMsg(msg);

    return retCode;
}


BOOL PLCDriver::PLCSetDeviceName(PLC_MACAddress mac, char devName[])
{
    BOOL retCode = PLCSetDeviceName_f(mac, devName);

    QString tmp;
    QString msg = "PLCSetDeviceName return code = ";
    msg += tmp.setNum(retCode);
    msg += ",target dev mac = ";
    msg += PLCDeviceMacAddr2String(mac);

    msg += ", Device Name = ";
    msg += devName;
    msg += ".";

    dumpMsg(msg);

    return retCode;

}

BOOL PLCDriver::PLCSetDeviceLedStatus(PLC_MACAddress mac, int ledStatus)
{
    BOOL retCode = PLCSetDeviceLedStatus_f(mac, ledStatus);
    QString tmp;
    QString msg = "PLCSetDeviceLedStatus return code = ";
    msg += tmp.setNum(retCode);
    msg += ",target dev mac = ";
    msg += PLCDeviceMacAddr2String(mac);

    msg += ", LED Status = ";
    msg += tmp.setNum(ledStatus);
    msg += ".";

    dumpMsg(msg);

    return retCode;

}

BOOL PLCDriver::PLCSetDeviceSecurityKey( PLC_MACAddress mac, char key[], char pwd[])
{
    BOOL retCode = PLCSetDeviceSecurityKey_f(mac, key, pwd);

    QString tmp;
    QString msg = "PLCSetDeviceSecurityKey return code = ";
    msg += tmp.setNum(retCode);
    msg += ",target dev mac = ";
    msg += PLCDeviceMacAddr2String(mac);

    msg += ", Key = ";
    msg += key;
    msg += ", Password = ";
    msg += pwd;
    msg += ".";

    dumpMsg(msg);

    return retCode;
}

BOOL PLCDriver::PLCSetDeviceQoS(PLC_MACAddress mac, PLC_DeviceQoSStatus* qos)
{
    return PLCSetDeviceQoS(mac, qos);
}

BOOL PLCDriver::PLCSetDeviceResetDefault(PLC_MACAddress mac)
{
    BOOL retCode = PLCSetDeviceResetDefault_f(mac);

    QString tmp;
    QString msg = "PLCSetDeviceResetDefault return code = ";
    msg += tmp.setNum(retCode);
    msg += ",target dev mac = ";
    msg += PLCDeviceMacAddr2String(mac);
    msg += ".";
    dumpMsg(msg);

    return retCode;
}

BOOL PLCDriver::PLCCheckDevicePassword(PLC_MACAddress mac, char pwd[])
{
    if(!PLCCheckDevicePassword_f){
        dumpMsg("PLCCheckDevicePassword not implemented!!!");
        return PLC_GET_FAIL;
    }

    BOOL retCode = PLCCheckDevicePassword_f(mac, pwd);

    QString tmp;
    QString msg = "PLCCheckDevicePassword return code = ";
    msg += tmp.setNum(retCode);
    msg += ",target dev mac = ";
    msg += PLCDeviceMacAddr2String(mac);
    msg += ",DAK = ";
    msg += pwd;
    msg += ".";
    dumpMsg(msg);

    return retCode;
}
#endif
