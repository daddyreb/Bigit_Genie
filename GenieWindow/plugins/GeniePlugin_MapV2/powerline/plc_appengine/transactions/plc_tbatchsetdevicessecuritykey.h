#ifndef PLC_TBATCHSETDEVICESSECURITYKEY_H
#define PLC_TBATCHSETDEVICESSECURITYKEY_H

#include "plctransaction.h"
#include <QString>
#include <QStringList>
#include <QMap>

class PLC_TBatchSetDevicesSecurityKey : public PLCTransaction
{
public:
    PLC_TBatchSetDevicesSecurityKey(const QStringList &targetDevMacs, const QString &securityKey, const QStringList &devPwds);

public:
    int Execute(MainPipeInterface *mainPipeInf, PLCDeviceInfoInterface *infoInf);

public:
    QMap<QString, QString> GetSuccessDeviceDetails() const;
    QMap<QString, QString> GetPasswordOkDeviceDetails() const;
    QStringList GetTargetDeviceMacs() const;
private:
    QStringList m_targetDevMacs;
    QString m_securityKey;
    QStringList m_devicePasswords;

    QMap<QString, QString> m_SuccessDevDetails;//<MAC,PASSWORD>
    QMap<QString, QString> m_PasswordOkDevDetails;//<MAC,PASSWORD>
};

#endif // PLC_TBATCHSETDEVICESSECURITYKEY_H
