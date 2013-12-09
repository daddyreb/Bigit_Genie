#include "networktransactionmanager.h"
#include "networktransaction.h"
#include "networkaccessproxy.h"

#ifndef _MSC_VER
const int NetworkTransactionManager::TranTimeOutIntervalMS;
#endif  // !_MSC_VER

NetworkTransactionManager::NetworkTransactionManager(QObject *parent) :
        QObject(parent)
        ,m_netAccessProxy(new NetworkAccessProxy())
{
    m_transactionTimeOutMonitorTimer.setInterval(TranTimeOutIntervalMS);
    connect(&m_transactionTimeOutMonitorTimer, SIGNAL(timeout()), SLOT(TransactionTimerFired()));
}

NetworkTransactionManager::~NetworkTransactionManager()
{
    m_transactionTimeOutMonitorTimer.stop();
    QSet<NetworkTransaction*>::iterator iter = m_transactionSet.begin();
    while(iter != m_transactionSet.end()){
        (*iter)->AbortTransaction();
        delete *iter;
        ++iter;
    }

    m_transactionSet.clear();

    delete m_netAccessProxy;
}

void NetworkTransactionManager::SubmitTransaction(const QString &module,
                                                  const QString &method,
                                                  const QMap<QString, QString> *param,
                                                  const QObject *resultReceiver,
                                                  const char *slot,
                                                  int timeoutSeconds)
{
    NetworkTransaction *transaction = m_netAccessProxy->PostRequest(module, method, param);
    connect(transaction, SIGNAL(ReturnResult(int, const QMap<QString,QString>*)), resultReceiver, slot);

    transaction->m_timeoutThresholdSeconds = timeoutSeconds;
    AddTransaction2Pool(transaction);
}

void NetworkTransactionManager::SubmitTransaction(const QString &host,
                                                  quint16 port,
                                                  const QString &module,
                                                  const QString &method,
                                                  const QMap<QString, QString> *param,
                                                  const QObject *resultReceiver,
                                                  const char *slot,
                                                  int timeoutSeconds)
{
    NetworkTransaction *transaction = m_netAccessProxy->PostRequest(host, port, module, method, param);
    connect(transaction, SIGNAL(ReturnResult(int, const QMap<QString,QString>*)), resultReceiver, slot);

    transaction->m_timeoutThresholdSeconds = timeoutSeconds;
    AddTransaction2Pool(transaction);
}

void NetworkTransactionManager::AddTransaction2Pool(NetworkTransaction *transaction)
{

    connect(transaction, SIGNAL(Finished(NetworkTransaction*)), SLOT(TransactionFinished(NetworkTransaction*)));
    m_transactionSet.insert(transaction);

    if(transaction->m_timeoutThresholdSeconds > 0){

        AddTransaction2TimeoutMonitorList(transaction);

        if(!m_transactionTimeOutMonitorTimer.isActive()){
            m_transactionTimeOutMonitorTimer.start();
        }
    }
}

void NetworkTransactionManager::AddTransaction2TimeoutMonitorList(NetworkTransaction *transaction)
{
    int count = m_timeoutMonitorList.count();
    int accuTimeoutValue = 0;
    int index = 0;

    for(; index < count ; ++index){
        accuTimeoutValue += m_timeoutMonitorList[index]->m_timeoutThresholdSeconds;
        if(transaction->m_timeoutThresholdSeconds > accuTimeoutValue){
            break;
        }
    }

    transaction->m_timeoutThresholdSeconds -= accuTimeoutValue;
    m_timeoutMonitorList.insert(index, transaction);

    int newCount = m_timeoutMonitorList.count();
    while((++index) < newCount){
        m_timeoutMonitorList[index]->m_timeoutThresholdSeconds -=  transaction->m_timeoutThresholdSeconds;
    }
}

void NetworkTransactionManager::RemoveTransactionfromTimeoutMonitorList(NetworkTransaction *transaction)
{
    int count = m_timeoutMonitorList.count();
    int index = 0;

    for( ; index < count ; ++index){
        if(m_timeoutMonitorList[index] == transaction){
            break;
        }
    }

    if(index < count){

        int idx = (index + 1);
        while(idx < count){
            m_timeoutMonitorList[idx]->m_timeoutThresholdSeconds +=  m_timeoutMonitorList[index]->m_timeoutThresholdSeconds;
            ++idx;
        }

        m_timeoutMonitorList.remove(index);
    }
}

void NetworkTransactionManager::TransactionFinished(NetworkTransaction *transaction)
{
    RemoveTransactionfromTimeoutMonitorList(transaction);
    m_transactionSet.remove(transaction);
    transaction->deleteLater();
}

void NetworkTransactionManager::TransactionTimerFired()
{
    QVector<NetworkTransaction *> timeoutTransactions;

    int countMonitor = m_timeoutMonitorList.count();
    if(countMonitor > 0){
        --m_timeoutMonitorList[0]->m_timeoutThresholdSeconds;
        for(int index = 0 ; (index < countMonitor) && (m_timeoutMonitorList[index]->m_timeoutThresholdSeconds == 0) ; ++index){
            timeoutTransactions.append(m_timeoutMonitorList[index]);
        }
    }else{
        m_transactionTimeOutMonitorTimer.stop();
    }

    int countTimeout = timeoutTransactions.count();
    m_timeoutMonitorList.remove(0, countTimeout);

    for(int idx = 0 ; idx < countTimeout ; ++idx){
        timeoutTransactions[idx]->TransactionTimeout();
    }
}
