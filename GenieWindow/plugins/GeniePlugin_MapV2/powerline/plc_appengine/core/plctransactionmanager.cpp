#include "plctransactionmanager.h"
#include "plctransaction.h"
#include <QMutexLocker>


PLCTransactionManager::PLCTransactionManager(QObject *parent) :
        QObject(parent)
{
}

PLCTransactionManager::~PLCTransactionManager()
{
    AbortPendingTransaction();
}

void PLCTransactionManager::AbortPendingTransaction()
{
    QMutexLocker locker(&m_transactionMutex);
    Q_UNUSED(locker);

    while(!m_plcTransactions.empty()){
        PLCTransaction *transaction = m_plcTransactions.takeFirst();
        delete transaction;
    }
}

void PLCTransactionManager::PutTransaction(PLCTransaction *transaction)
{
    QMutexLocker locker(&m_transactionMutex);
    Q_UNUSED(locker);

    m_plcTransactions.append(transaction);

    m_transactionCdt.wakeAll();
}

bool PLCTransactionManager::WaitForTransaction(unsigned long time) const
{
    m_transactionMutex.lock();
    if(m_plcTransactions.empty()){
        m_transactionCdt.wait(&m_transactionMutex, time);
    }
    m_transactionMutex.unlock();

    return !m_plcTransactions.empty();
}

void PLCTransactionManager::CancelWaitForTransaction() const
{
    m_transactionCdt.wakeAll();
}

bool PLCTransactionManager::ProcessTransaction(MainPipeInterface *mainPipeInf, PLCDeviceInfoInterface *infoInf)
{
    m_transactionMutex.lock();

    bool isModifyTransaction = false;

    while(!m_plcTransactions.empty()){
        PLCTransaction *transaction = m_plcTransactions.takeFirst();
        m_transactionMutex.unlock();

        isModifyTransaction = (isModifyTransaction || (transaction->Transaction() <= PLCTransaction::TC_MODIFY_CODE));

        transaction->Execute(mainPipeInf, infoInf);

        if(transaction->NeedEmitNotifySignal()){
            emit TransactionFinished(TransactionPointer(transaction));
        }else{
            delete transaction;
        }

        m_transactionMutex.lock();
    }

    m_transactionMutex.unlock();

    return isModifyTransaction;
}

bool PLCTransactionManager::NeedProcessTransaction() const
{
    QMutexLocker locker(&m_transactionMutex);
    Q_UNUSED(locker);

    return !m_plcTransactions.empty();
}

