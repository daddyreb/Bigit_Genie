#ifndef DEVICEINFO_H
#define DEVICEINFO_H

#include "common.h"
#include "log.h"
#include <QReadWriteLock>

class DevicesInfo
{
public:
    DevicesInfo();
    static DevicesInfo &instance();
    void getData(QMap<QString , QMap<QString, QString> *> *);
    void addData(QMap<QString , QMap<QString, QString> *> &);
    void deleteData(QList<QString> &);
    void displayData();

private:

    static QMap<QString, QMap<QString, QString> *> m_devicesinfo;

    static QReadWriteLock rwlock;
};

#endif // DEVICEINFO_H
