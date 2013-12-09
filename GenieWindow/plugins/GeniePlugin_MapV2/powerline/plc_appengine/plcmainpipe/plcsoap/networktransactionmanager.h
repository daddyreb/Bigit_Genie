#ifndef NETWORKTRANSACTIONMANAGER_H
#define NETWORKTRANSACTIONMANAGER_H

#include <QObject>
#include <QMap>
#include <QSet>
#include <QVector>
#include <QTimer>


class NetworkTransaction;
class NetworkAccessProxy;


class NetworkTransactionManager : public QObject
{
    Q_OBJECT
public:
    explicit NetworkTransactionManager(QObject *parent = 0);
    ~NetworkTransactionManager();

public slots:
    void SubmitTransaction(const QString &module,
                           const QString &method,
                           const QMap<QString, QString> *param,
                           const QObject *resultReceiver,
                           const char *slot,
                           int timeoutSeconds);
    void SubmitTransaction(const QString &host,
                           quint16 port,
                           const QString &module,
                           const QString &method,
                           const QMap<QString, QString> *param,
                           const QObject *resultReceiver,
                           const char *slot,
                           int timeoutSeconds);

private:
    void AddTransaction2Pool(NetworkTransaction *transaction);
    void AddTransaction2TimeoutMonitorList(NetworkTransaction *transaction);
    void RemoveTransactionfromTimeoutMonitorList(NetworkTransaction *transaction);


private slots:
    void TransactionFinished(NetworkTransaction *transaction);
    void TransactionTimerFired();

private:
    static const int TranTimeOutIntervalMS = 1000;

private:
    NetworkAccessProxy *m_netAccessProxy;
    QSet<NetworkTransaction*> m_transactionSet;
    QTimer m_transactionTimeOutMonitorTimer;
    QVector<NetworkTransaction*> m_timeoutMonitorList;

};

#endif // NETWORKTRANSACTIONMANAGER_H
