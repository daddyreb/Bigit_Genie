#include "plcappengine.h"
#include "plcappengineexecutive.h"
#include "transactions.h"
#include <QMetaType>


PLCAppEngine::PLCAppEngine(QObject *parent) :
    QObject(parent)
  ,m_appEngineExe(new PLCAppEngineExecutive())
{
    qRegisterMetaType<TransactionPointer>("TransactionPointer");
    qRegisterMetaType<TopologyDataTransfer>("TopologyDataTransfer");

    connect(m_appEngineExe, SIGNAL(TransactionFinished(TransactionPointer)),
            this, SIGNAL(TransactionFinished(TransactionPointer)));
    connect(m_appEngineExe, SIGNAL(PLCDeviceChangeScanned(TopologyDataTransfer)),
            this, SIGNAL(PLCDeviceChangeScanned(TopologyDataTransfer)));
}

PLCAppEngine::~PLCAppEngine()
{
    delete m_appEngineExe;
}

bool PLCAppEngine::Startup(const QString &mainPipePath, const QString &plcDriverDir)
{
    return m_appEngineExe->Startup(mainPipePath, plcDriverDir);
}

void PLCAppEngine::Cleanup()
{
    m_appEngineExe->Cleanup();
}

bool PLCAppEngine::ReScanPLCDevice()
{
    PLC_TReSynchScan *transaction = new PLC_TReSynchScan();
    return m_appEngineExe->SubmitTransaction(transaction);
}


bool PLCAppEngine::T_SetDeviceName(const QString &targetDevMac, const QString &newName)
{
    PLC_TSetDeviceName *transaction = new PLC_TSetDeviceName(targetDevMac, newName);
    return m_appEngineExe->SubmitTransaction(transaction);
}

bool PLCAppEngine::T_SetDeviceLED(const QString &targetDevMac, int state)
{
    PLC_TSetDeviceLED *transaction = new PLC_TSetDeviceLED(targetDevMac, state);
    return m_appEngineExe->SubmitTransaction(transaction);
}

bool PLCAppEngine::T_SetDeviceSecurityKey(const QString &targetDevMac, const QString &securityKey, const QString &devPwd)
{
    PLC_TSetDeviceSecurityKey *transaction = new PLC_TSetDeviceSecurityKey(targetDevMac, securityKey, devPwd);
    return m_appEngineExe->SubmitTransaction(transaction);
}

bool PLCAppEngine::T_BatchSetDevicesSecurityKey(const QStringList &targetDevMacs, const QString &securityKey, const QStringList &devPwds)
{
    PLC_TBatchSetDevicesSecurityKey *transaction = new PLC_TBatchSetDevicesSecurityKey(targetDevMacs, securityKey, devPwds);
    return m_appEngineExe->SubmitTransaction(transaction);
}

bool PLCAppEngine::T_DeviceResetDefault(const QString &targetDevMac)
{
    PLC_TDeviceResetDefault *transaction = new PLC_TDeviceResetDefault(targetDevMac);
    return m_appEngineExe->SubmitTransaction(transaction);
}

bool PLCAppEngine::T_SetDeviceWLANInfo(const QString &targetDevIP ,const WLANInfo &wlanInfo, bool autowep2wpapsk)
{
    PLC_TSetDeviceWLANInfo *transaction = new PLC_TSetDeviceWLANInfo(targetDevIP, wlanInfo, autowep2wpapsk);
    return m_appEngineExe->SubmitTransaction(transaction);
}

bool PLCAppEngine::T_GetDeviceWLANInfo(const QString &targetDevIP, unsigned long tagNumber)
{
    PLC_TGetDeviceWLANInfo *transaction = new PLC_TGetDeviceWLANInfo(targetDevIP, tagNumber);
    return m_appEngineExe->SubmitTransaction(transaction);
}

bool PLCAppEngine::T_CheckFirmwareUpgrade(const QString &targetDevMac)
{
    PLC_TCheckFirmwareUpgrade *transaction = new PLC_TCheckFirmwareUpgrade(targetDevMac);
    return m_appEngineExe->SubmitTransaction(transaction);
}
