#ifndef PLCDRIVERMANAGER_H
#define PLCDRIVERMANAGER_H

#include <QVector>
#include <QString>
#include <QStringList>
#include <QMap>

#include "plcdriverinterface.h"

class PLCDriver;
class QLibrary;

class PLCDriverManager
{
public:
    explicit PLCDriverManager(const QString &driverDirPath);
    ~PLCDriverManager();

public:
    QVector<PLCDriver *> GetPLCDriverList() const;
    PLCDriver* AddPLCDriver(const QString &driverPath);

    PLCDriver* GetPLCDriver(const QString &devMAC);
    void AssociatePLCDriver(const QString &devMAC, PLCDriver* plcDriver);

private:
    QStringList GetAllDriverModules(const QString &dirName) const;
    void LoadAllDriverModules(const QString &dirName);
    bool ValidPLCDriverModule(QLibrary *libDriver) const;
    PLCDriver * ConstructDriverObject(QLibrary *libDriver) const;

private:
    QVector<PLCDriver *>    m_driverModules;
    QMap<QString , PLCDriver *> m_plcDevDriverMap;
};

#endif // PLCDRIVERMANAGER_H
