#ifndef PLCAPPENGINE_H
#define PLCAPPENGINE_H

#include <QObject>
#include "plctransaction.h"
#include "plctopologydata.h"

#include "mainpipedefines.h"

class PLCAppEngineExecutive;

class PLCAppEngine : public QObject
{
    Q_OBJECT
public:
    explicit PLCAppEngine(QObject *parent = 0);
    ~PLCAppEngine();

public:
    bool Startup(const QString &mainPipePath, const QString &plcDriverDir);
    void Cleanup();

public:
    bool ReScanPLCDevice();

public:
    bool T_SetDeviceName(const QString &targetDevMac, const QString &newName);
    bool T_SetDeviceLED(const QString &targetDevMac, int state);
    bool T_SetDeviceSecurityKey(const QString &targetDevMac, const QString &securityKey, const QString &devPwd);
    bool T_BatchSetDevicesSecurityKey(const QStringList &targetDevMacs, const QString &securityKey, const QStringList &devPwds);
    bool T_DeviceResetDefault(const QString &targetDevMac);
    bool T_SetDeviceWLANInfo(const QString &targetDevIP , const WLANInfo &wlanInfo, bool autowep2wpapsk = true);
    bool T_GetDeviceWLANInfo(const QString &targetDevIP, unsigned long tagNumber);
    bool T_CheckFirmwareUpgrade(const QString &targetDevMac);

signals:
    void TransactionFinished(TransactionPointer transaction);
    void PLCDeviceChangeScanned(TopologyDataTransfer topData);

public slots:
    
private:
    PLCAppEngineExecutive *m_appEngineExe;
};

#endif // PLCAPPENGINE_H
