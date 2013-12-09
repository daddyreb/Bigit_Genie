#ifndef PLCDEVICEINFOINTERFACE_H
#define PLCDEVICEINFOINTERFACE_H

#include <QString>

class PLCDeviceInfoInterface
{
public:
    virtual ~PLCDeviceInfoInterface(){}
public:
    virtual bool GetDeviceName(const QString &devMac ,QString &devName) = 0;
    virtual QString CurrentLocalDevice() = 0;
    virtual bool ReSynchScan() = 0;
};

#endif // PLCDEVICEINFOINTERFACE_H
