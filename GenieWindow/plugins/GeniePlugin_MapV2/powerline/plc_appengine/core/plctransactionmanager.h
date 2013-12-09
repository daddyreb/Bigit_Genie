#ifndef PLCTRANSACTIONMANAGER_H
#define PLCTRANSACTIONMANAGER_H

#include <QObject>
#include <QList>
#include <QMutex>
#include <QWaitCondition>

#include "transactions.h"

class PLCTransaction;
class MainPipeInterface;
class PLCDeviceInfoInterface;

class PLCTransactionManager : public QObject
{
    Q_OBJECT
public:
    explicit PLCTransactionManager(QObject *parent = 0);
    ~PLCTransactionManager();

public:
    void PutTransaction(PLCTransaction *transaction);

public:
    //Return: has pending transaction return true, otherwise return false
    bool WaitForTransaction(unsigned long time = ULONG_MAX) const;
    void CancelWaitForTransaction() const;

    bool ProcessTransaction(MainPipeInterface *mainPipeInf, PLCDeviceInfoInterface *infoInf);
    bool NeedProcessTransaction() const;

private:
    void AbortPendingTransaction();

signals:
    void TransactionFinished(TransactionPointer transaction);

private:
    QList<PLCTransaction *> m_plcTransactions;
    mutable QMutex m_transactionMutex;
    mutable QWaitCondition m_transactionCdt;

};

#endif // PLCTRANSACTIONMANAGER_H
