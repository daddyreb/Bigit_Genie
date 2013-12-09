#ifndef PLC_TSETDEVICENAME_H
#define PLC_TSETDEVICENAME_H

#include "plctransaction.h"

#include <QString>

class PLC_TSetDeviceName : public PLCTransaction
{
public:
    PLC_TSetDeviceName(const QString &targeDevMac, const QString &newName);

public:
    int Execute(MainPipeInterface *mainPipeInf, PLCDeviceInfoInterface *infoInf);

private:
    QString m_targetDevMac;
    QString m_devNewName;
};

#endif // PLC_TSETDEVICENAME_H
