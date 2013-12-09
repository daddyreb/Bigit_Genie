#ifndef PLCTOPOLOGYDATA_H
#define PLCTOPOLOGYDATA_H

#include <QList>
#include <QString>
#include <QStringList>
#include <QPair>
#include <QMap>

#include "pointertransfer.h"
#include "mainpipedefines.h"

class PLCDevice;

struct PLCNodeData
{
    PLCNodeData():
        m_devLEDOn(false)
      ,m_withUSB(false)
      ,m_withWireless(false)
    {
    }

    QString m_devMAC;
    QString m_devIP;
    QString m_devName;
    QString m_devModel;
    QString m_devFWVersion;
    QString m_devNetworkType;//Public or Private
    QString m_devDriverPath;
    bool m_devLEDOn;
    QStringList m_forwardTable;
    QList<AttachDeviceEntry> m_attachDeviceList;

    bool m_withUSB;//devcie support USB is true ,otherwise false
    QList<QPair<QString,QString> > m_attachUsbDevice;//<usbtype,usbdescription>
    QMap<QString, QPair<qint16,qint16> > m_devLinkRates;//link rates to other devices
    bool m_withWireless;//device support wireless is true ,otherwise false
    WLANInfo m_wlanInfo;//device support wireless should has valid values in this structure

    QString toString() const;
};

class PLCTopologyData
{
    friend class PLCDeviceScanner;

public:
    PLCTopologyData();
    ~PLCTopologyData();

public:
	bool IsRouterLocated() const;
    QString LocalDeviceMacAddr() const {return m_localDevMac;}
    QList<PLCNodeData *> GetTopologyData() const;

public:
    void dump() const;

private:
    void ParseScannedData(const QList<PLCDevice *> &devList, const QStringList& gatewayMacList, const QString &localDeviceMac);
    void ParseAttachDevice(PLCNodeData *node, const PLCDevice *dev);

private:
    bool m_routerLocated;
    QStringList m_routerMacList;
    QString m_localDevMac;
    QList<PLCNodeData *> m_plcNodes;//the first element is local device
};

typedef PointerTransfer<PLCTopologyData> TopologyDataTransfer;

#endif // PLCTOPOLOGYDATA_H
