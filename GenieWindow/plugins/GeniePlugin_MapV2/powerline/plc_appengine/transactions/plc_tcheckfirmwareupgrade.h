#ifndef PLC_TCHECKFIRMWAREUPGRADE_H
#define PLC_TCHECKFIRMWAREUPGRADE_H

#include "plctransaction.h"

#include <QString>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

class PLC_TCheckFirmwareUpgrade :  public PLCTransaction
{

public:
    explicit PLC_TCheckFirmwareUpgrade(const QString &targetDevMac);

public:
    int Execute(MainPipeInterface *mainPipeInf, PLCDeviceInfoInterface *infoInf);

public:
    bool GetUpgradeUrl(QString &downloadUrl) const;

private:
    bool VersionGreat(const QString &newVersion, const QString oldVersion) const;

private:
    QString m_targetDevMac;
    QString m_upgradeUrl;
    bool m_newversionAvlaible;
};


class CheckFirmwareUpgradeHandler : public QThread
{
    Q_OBJECT
public:
    explicit CheckFirmwareUpgradeHandler(QObject *parent = 0);
    ~CheckFirmwareUpgradeHandler();

public:
    bool Check(const QString &model, const QString &region, QString &versionOnServer, QString &firmwareUrl);

private slots:
    void finished(QNetworkReply *reply);

private:
    void run();

private:
    QString m_model;
    QString m_region;
    QString m_downloadUrl;
    QString m_versionOnServer;
    bool m_dataValid;
};

#endif // PLC_TCHECKFIRMWAREUPGRADE_H
