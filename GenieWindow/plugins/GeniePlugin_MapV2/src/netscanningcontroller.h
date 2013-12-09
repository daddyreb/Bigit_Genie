#ifndef NETSCANNINGCONTROLLER_H
#define NETSCANNINGCONTROLLER_H

#include <QObject>

#include "topology.h"

#include <QList>
#include <QMap>
#include <QString>
#include "genie2_interface.h"

class Device;

/*********************************************
The objects of class TopologyView and class
NetScanningController must run in the same thread
***********************************************/

class NetScanningController;
class NetworkMapView;


class NetScanningController : public QObject
{
    Q_OBJECT
public:
    explicit NetScanningController(QObject *parent = 0);
    ~NetScanningController();
    inline void setExitFlag()
    {
        m_scp.setExitFlag();
    }

protected:
    QList<Device *>                     m_subscribeDevsList;
    SVT_ControlProxy                    m_scp;

    QMap<QString , QList<QString> >     *m_customDataCacheMap;

    int                                 m_poolNumber;
    int                                 m_curAdpterId;
    bool                                m_isDataValid;

#ifdef POWERLINE_REVISION
    int                                 m_curStartIdx;
    int                                 m_curTotalPowerLineInSubDevsList;
    int                                 m_curTotalPowerLine;
#endif

private:
    bool storeCustomDataCacheMap();
    bool loadCustomDataCacheMap();

private:
    void clearSubscribeDevsList();
    void resetSubscribeDevsList();
    void deviceFromRawData(Device &dev,const QMap<int,QString> &data ,int idx);

    QString parseDeviceName(const QMap<int,QString> &data);
    int parseDeviceType(const QMap<int,QString> &data);
    int parseUpnpDeviceType(const QMap<int,QString> &data);
    int parseDeviceTypeWithAttachDevice(const QMap<int,QString> &data);

    QString parseDeviceOS(const QMap<int,QString> &data);
    int parseDeivceSignalStrenth(const QMap<int,QString> &data);
    bool parseDeivceWiredProperty(const QMap<int,QString> &data);

    QString loadCustomName(const QString &key);
    int loadCustomDeviceType(const QString &key);
    int loadCustomConnectionType(const QString &key);
    bool saveCustomData(const QString &key, const QString &customName
                        ,int customType
                        ,int customConnectionType);
signals:
    void newData(const QList<Device *> &data);
    void infInformation(SVT_NotifyType data);
    void deviceCountChanged(int count);
	//add by caiyujun
	void otherDeviceCountChanged(QString deviceIp, int count);
	//add end
    void newDeviceNotify(SVT_NotifyType data);
    void routerTypeChanged();
//    void currentSettingData(const QMap<int,QString> &data);
    void gatewayChanged(const QString &data);


private slots:
    void onNewData(SVT_NotifyType data);
    void onInfInformation(SVT_NotifyType data);
    void onCountChanged(int count);
	//add by caiyujun
	void onOtherDeviceCountChanged(QString deviceIp, int count);
	//add end

#ifdef POWERLINE_REVISION
    void onPowerLineStateChanged(int totalPowerLine);
    void updatePowerLineInSubDevsList();
#endif

    //process sginals from NetworkMapView
    void onNode2Device(int idx,Device *dev);
    //add by caiyujun
    void onIp2Device(QString strIp, Device *device);
    void onBonjourTypeChanged(const Device &dev);
    //add end
    void onDeviceDataChanged(const Device &dev);
    void onRefresh();
	//modify by caiyujun
    //void onSubscribeData(int adpIdx,int startIdx,int count);
	void onSubscribeData(int adpIdx,int startIdx,int count,bool bExtendMap,unsigned long extenderIp);
	//modify end
//    void onNetgearInfo(const QMap<int,QString> &data);
//    void onGatewayChanged(QString data);
public slots:
    void startScan();
//    void refreshNetgearInfo();
    friend class BlockDeviceManager;
};

#endif // NETSCANNINGCONTROLLER_H
