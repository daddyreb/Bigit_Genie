#ifndef PLC_TSETDEVICESECURITYKEY_H
#define PLC_TSETDEVICESECURITYKEY_H

#include "plctransaction.h"
#include <QString>

class PLC_TSetDeviceSecurityKey : public PLCTransaction
{
public:
    PLC_TSetDeviceSecurityKey(const QString &targetDevMac, const QString &securityKey, const QString &devPwd);

public:
    int Execute(MainPipeInterface *mainPipeInf, PLCDeviceInfoInterface *infoInf);

private:
    QString m_targetDevMac;
    QString m_securityKey;
    QString m_devicePassword;
};

#endif // PLC_TSETDEVICESECURITYKEY_H
