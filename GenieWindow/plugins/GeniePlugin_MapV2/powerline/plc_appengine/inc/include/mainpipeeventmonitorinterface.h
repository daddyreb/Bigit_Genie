#ifndef MAINPIPEEVENTMONITORINTERFACE_H
#define MAINPIPEEVENTMONITORINTERFACE_H

#include <QString>

class MainPipeEventMonitorInterface
{
public:
    virtual ~MainPipeEventMonitorInterface(){}

public:
    virtual void OnNewPLCDriver(const QString &driverPath) = 0;
};

#endif // MAINPIPEEVENTMONITORINTERFACE_H
