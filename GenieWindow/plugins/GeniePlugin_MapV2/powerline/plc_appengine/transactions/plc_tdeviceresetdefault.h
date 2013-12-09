#ifndef PLC_TDEVICERESETDEFAULT_H
#define PLC_TDEVICERESETDEFAULT_H

#include "plctransaction.h"

#include <QString>

class PLC_TDeviceResetDefault : public PLCTransaction
{
public:
    explicit PLC_TDeviceResetDefault(const QString &targetDevMac);

public:
    int Execute(MainPipeInterface *mainPipeInf, PLCDeviceInfoInterface *infoInf);

private:
    QString m_targetDevMac;
};

#endif // PLC_TDEVICERESETDEFAULT_H
