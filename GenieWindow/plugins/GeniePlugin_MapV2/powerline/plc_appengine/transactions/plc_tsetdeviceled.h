#ifndef PLC_TSETDEVICELED_H
#define PLC_TSETDEVICELED_H

#include "plctransaction.h"
#include <QString>

class PLC_TSetDeviceLED : public PLCTransaction
{
public:
    PLC_TSetDeviceLED(const QString &targetDevMac, int LEDState);

public:
    int Execute(MainPipeInterface *mainPipeInf, PLCDeviceInfoInterface *infoInf);

private:
    QString m_targetDevMac;
    int m_newLEDState;
};

#endif // PLC_TSETDEVICELED_H
