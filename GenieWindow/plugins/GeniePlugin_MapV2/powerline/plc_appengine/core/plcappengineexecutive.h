#ifndef PLCAPPENGINEEXECUTIVE_H
#define PLCAPPENGINEEXECUTIVE_H

#include <QThread>
#include <QString>
#include <QPluginLoader>
#include <QMutex>
#include <QWaitCondition>

#include "plctransaction.h"
#include "plctopologydata.h"
#include "mainpipeeventmonitorinterface.h"


class MainPipeInterface;
class PLCDeviceScanner;
class PLCTransactionManager;

class PLCAppEngineExecutive : private QThread ,public MainPipeEventMonitorInterface
{
    friend class PLCAppEngine;

    Q_OBJECT
public:
    explicit PLCAppEngineExecutive(QObject *parent = 0);
    ~PLCAppEngineExecutive();

public:
    bool Startup(const QString &mainPipePath, const QString &plcDriverDir);
    void Cleanup();
    bool SubmitTransaction(PLCTransaction *transaction);


signals:
    void TransactionFinished(TransactionPointer transaction);
    void PLCDeviceChangeScanned(TopologyDataTransfer topData);

private:
    void run();
    void Executive();
    bool IsExecutiveStartupOk();
    bool ContinueExe();

private:
    //main pipe event monitor method ,and these method may be called from another thread
    void OnNewPLCDriver(const QString &driverPath);

private:
    //QPluginLoader mainPipeLoader;

    bool m_exitFlag;
    QMutex m_exitFlagMutex;

    MainPipeInterface *m_mainPipeInf;

    PLCDeviceScanner *m_deviceScanner;
    PLCTransactionManager *m_transactionManager;

    bool m_isStartupOk;
    QMutex m_startMutex;
    QWaitCondition m_startCdt;

    bool m_newDriverFounded;
};

#endif // PLCAPPENGINEEXECUTIVE_H
