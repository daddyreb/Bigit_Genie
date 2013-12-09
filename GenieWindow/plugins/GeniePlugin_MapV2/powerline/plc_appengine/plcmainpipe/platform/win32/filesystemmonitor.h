#ifndef FILESYSTEMMONITOR_H
#define FILESYSTEMMONITOR_H

#include <QString>
#include <QThread>
#include <windows.h>


class FileSystemMonitor : private QThread
{
    friend class PLCDriverExecutive;

    Q_OBJECT

public:
    explicit FileSystemMonitor(const QString &monitorDirPath);
    ~FileSystemMonitor();

signals:
    void newPLCDriver(const QString & driverModulePath);

protected:
    void run();

private:
    bool m_isExiting;
    QString m_monitorDirPath;
    HANDLE m_hExitEvent;
};

#endif // FILESYSTEMMONITOR_H
