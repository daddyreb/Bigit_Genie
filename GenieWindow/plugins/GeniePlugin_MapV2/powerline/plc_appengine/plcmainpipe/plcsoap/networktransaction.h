#ifndef NETWORKTRANSACTION_H
#define NETWORKTRANSACTION_H

#include <QObject>
#include <QMap>



class QNetworkReply;
class QByteArray;

class NetworkTransaction : public QObject
{
    friend class NetworkTransactionManager;

    Q_OBJECT
public:
    NetworkTransaction(QNetworkReply *reply, QByteArray *requestData, QObject *parent = 0);
    ~NetworkTransaction();

private:
    void ParseResponseData(const QByteArray &xmlData, QMap<QString, QString> &parseResult);
    void TransactionTimeout();
    void AbortTransaction();
    void ProcessTransactionEnd(int soapCode, const QMap<QString, QString> *resultMap);
    int ParseResponseCode(const QString &responseCode);

signals:
    void Finished(NetworkTransaction *transaction);
    void ReturnResult(int soapCode, const QMap<QString, QString> *resultMap);

private slots:
    void RequestFinished();

private:
    static const int PORT_80 = 80;
    static const int PORT_5000 = 5000;

private:
    QNetworkReply *m_networkReply;
    QByteArray *m_requestData;
    int m_timeoutThresholdSeconds;
    int m_retryTimesRemain;
};

#endif // NETWORKTRANSACTION_H
