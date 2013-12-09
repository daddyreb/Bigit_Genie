#ifndef TOPOLOGYLIB_H
#define TOPOLOGYLIB_H

#include "common.h"
#include "sqlite.h"
#include "devicesinfo.h"

class EXPORT TopologyLib
{
public:
    TopologyLib();
    ~TopologyLib();

    bool getDevicesInfo(QMap<QString, QMap<QString, QString> *> *);

private:
    Sqlite sqlite;
    static int flag;
    QProcess * myProcess;
};

#endif // TOPOLOGYLIB_H
