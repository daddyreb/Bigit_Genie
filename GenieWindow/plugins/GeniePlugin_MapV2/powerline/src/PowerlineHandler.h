#ifndef POWERLINEHANDLER_H
#define POWERLINEHANDLER_H

#include <QObject>
#include "plctransaction.h"
#include "plctopologydata.h"

class PLCAppEngine;

class PowerlineHandler : public QObject
{
    Q_OBJECT
public:
    explicit PowerlineHandler(QObject *parent = 0);
    virtual ~PowerlineHandler();

public:
    bool backToFactory(const QString &strMac);
    bool turnLedOff(const QString &strMac, int nState = 0); //0:off  other:on
    bool modifyDevName(const QString &strMac, const QString &name);
    bool applySecurity(const QString &targetDevMac, const QString &securityKey, const QString &devPwd);
	bool applySecurityAll(const QStringList &targetDevMacs, const QString &securityKey, const QStringList &devPwds);
    bool setDeviceWLANInfo(const QString &targetDevIP , const WLANInfo &wlanInfo, bool autowep2wap = true);
    bool getDeviceWLANInfo(const QString &targetDevIP, unsigned long tagNumber);
	bool chkUpdate(const QString &strMac);
    void refreshSecurity();

	bool refreshScanDev();
protected:
    PLCAppEngine        *m_appEngine;
private:
    bool init();

signals:
    void OnLCDScanned(TopologyDataTransfer topData);
    void OnTransFinished(TransactionPointer transaction);
private slots:
    void OnTransactionFinished(TransactionPointer transaction);
    void OnPLCDeviceChangeScanned(TopologyDataTransfer topData);

};

#endif // POWERLINEHANDLER_H
