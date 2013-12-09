#ifndef PLCDEVICESCANNER_H
#define PLCDEVICESCANNER_H

#include <QObject>

#include <QMap>
#include <QList>
#include <QStringList>

#include "plcdeviceinfointerface.h"
#include "plctopologydata.h"

class PLCDevice;
class MainPipeInterface;

class PLCDeviceScanner : public QObject , public PLCDeviceInfoInterface
{
    Q_OBJECT
public:
    explicit PLCDeviceScanner(MainPipeInterface *mainPipeInf, QObject *parent = 0);
    ~PLCDeviceScanner();

//public:
//    void ProcessScanning(bool needReDiscovery = false);
//    bool IdleScanning() const;

private:
    void DiscoverDevice();
    void DiscoverGatewayPhysicalAddrs();
    void ParseScanningInfo(bool notifyChangeOnly = true);

signals:
    void PLCDeviceChangeScanned(TopologyDataTransfer topData);

public slots:

private:
    MainPipeInterface *m_mainPipeInf;

    //int m_currentScanningDevIndex;
    //int m_currentScanningEntrieIndex;
    //bool m_idleScanning;
    bool m_devListChanged;

    QStringList m_gatewayPhysicalAddrs;
    bool m_gatewayChanged;

    QMap<QString, PLCDevice *> m_mac2ActiveDeviceMap;
    QMap<QString, PLCDevice *> m_mac2InactiveDeviceMap;

    QList<PLCDevice *> m_currentScanningDevList;
    QString m_currentLocalDeviceMac;

    //---------------implement device information interface-------------------------
    bool GetDeviceName(const QString &devMac ,QString &devName);
    bool ReSynchScan();
    QString CurrentLocalDevice();
};

#endif // PLCDEVICESCANNER_H
