#ifndef RETRIVEPLCDEVICEIP_H
#define RETRIVEPLCDEVICEIP_H

#include <QString>
#include <QStringList>
#include <window.h>

class NetHelpRoutines
{
public:
    NetHelpRoutines();

public:
    static bool GetPLCDeviceIP(const QString &devMac ,QString &devIP);
    static bool GetGatewayMacs(QStringList &macList);

private:
    static bool SearchArpForIP(const QString &devMac ,QString &devIP);
    static bool GetGatewayIPs(QStringList &ipList);
};

#endif // RETRIVEPLCDEVICEIP_H
