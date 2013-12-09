#include "plcdrivermanager.h"
#include "plcdriver.h"

#include <QLibrary>
#include <QDir>
#include <QtDebug>
#include <QDir>


PLCDriverManager::PLCDriverManager(const QString &driverDirPath)
{
    LoadAllDriverModules(driverDirPath);
}

PLCDriverManager::~PLCDriverManager()
{
    foreach( PLCDriver *driver, m_driverModules){
        delete driver;
    }
    m_driverModules.clear();
}

QVector<PLCDriver *> PLCDriverManager::GetPLCDriverList() const
{
    return m_driverModules;
}

PLCDriver* PLCDriverManager::AddPLCDriver(const QString &driverPath)
{
    PLCDriver *plcDriver = NULL;
    QLibrary *libDriver = new QLibrary(driverPath);

    if(!libDriver->load()){
        qDebug () << libDriver->errorString();
        delete libDriver;
        return plcDriver;
    }

    if(ValidPLCDriverModule(libDriver)){
        plcDriver = ConstructDriverObject(libDriver);
        m_driverModules.append(plcDriver);
    }else{
        libDriver->unload();
        delete libDriver;
    }
    return plcDriver;
}

PLCDriver* PLCDriverManager::GetPLCDriver(const QString &devMAC)
{
    return m_plcDevDriverMap.value(devMAC);
}

void PLCDriverManager::AssociatePLCDriver(const QString &devMAC, PLCDriver* plcDriver)
{
    if(m_driverModules.contains(plcDriver)){
        m_plcDevDriverMap[devMAC] = plcDriver;
    }
}

QStringList PLCDriverManager::GetAllDriverModules(const QString &dirName) const
{
    QDir driverDir(dirName);
    QStringList modules = driverDir.entryList();
    for(int index = modules.count() - 1 ; index >= 0 ; --index){
        if(!QLibrary::isLibrary(driverDir.absoluteFilePath(modules[index]))){
            modules.removeAt(index);
        }
    }
    return modules;
}

void PLCDriverManager::LoadAllDriverModules(const QString & dirName)
{
    QStringList moduleNames = GetAllDriverModules(dirName);
    QDir driverDir(dirName);
    foreach(QString module, moduleNames){
        AddPLCDriver(driverDir.absoluteFilePath(module));
    }
}

bool PLCDriverManager::ValidPLCDriverModule(QLibrary * libDriver) const
{
    const char*  driverFns[] = {
        "PLCDiscovery"
        ,"PLCGetLocalDevice"
        ,"PLCGetDeviceModelName"
        ,"PLCGetDeviceFWVersion"
        ,"PLCGetDeviceName"
        ,"PLCGetNetworkType"
        ,"PLCGetDeviceLedStatus"
        ,"PLCGetDeviceLinkRate"
        ,"PLCGetDeviceQoSStatus"
        ,"PLCGetDeviceForwardtable"
        ,"PLCGetDeviceFWUpgradeVersion"
        ,"PLCGetDeviceFWUpgradeRegion"
        ,"PLCSetDeviceName"
        ,"PLCSetDeviceLedStatus"
        ,"PLCSetDeviceSecurityKey"
        ,"PLCSetDeviceQoS"
        ,"PLCSetDeviceResetDefault"
        ,"PLCCheckDevicePassword"
        ,NULL
    };

    int index = 0 ;
    while(driverFns[index]){
        if(!libDriver->resolve(driverFns[index])){
            qDebug() << "Driver module :  " << libDriver->fileName() << "is not a valid PLC Driver ,routine : " << driverFns[index] << "is not implemented!!!\r\n";
            break;
        }

        ++index;
    }

    return (driverFns[index] == NULL);
}

PLCDriver * PLCDriverManager::ConstructDriverObject(QLibrary *libDriver) const
{
    return new PLCDriver(libDriver);
}
