#ifndef QGENIESOAPUTIL_H
#define QGENIESOAPUTIL_H

#ifndef IN_STABLE_H
#include <QtCore>
#include "qtsoap.h"
#endif
enum soap_node_state
{
    SNS_READY,
    SNS_STARTSET,
    SNS_CODE,
    SNS_FINISHSET,
    SNS_TIMEOUT,
    SNS_ENDED,
    SNS_NUM
};


enum soap_node_option
{
    SNO_READ,
    SNO_WRITE,
    SNO_WRITE_FINISH,
    SNO_NUM
};

struct soap_node{
public:

    QtSoapHttpTransport soaphttp;
    QTimer timer;
    soap_node_state state;
    soap_node_option option;
    QUuid uuid_cache;
    QString id_cache;
    QString method_name;
    QString module_name;
    QMap<QString,QString> paras_cache;
    int retrytime;
    int retrytime_end;

};

#define SOAP_NODE_READ_COUNT 20
#define SOAP_NODE_WRITE_COUNT 2

class NetgearSoapSchedule:public QObject
{
    Q_OBJECT
public:
    NetgearSoapSchedule(QObject *parent=NULL);
    ~NetgearSoapSchedule();
    void submitrequest(const QString &modulename, const QString &methodname, QMap<QString, QString> &params
                       ,QUuid &uuid,QString &id);
    void iner_submitrequest(const QString &modulename, const QString &methodname, QMap<QString, QString> &params
                                                                    ,QUuid &uuid,QString &id,int times,int t_end);
    void real_submitrequest(soap_node *node);

    void sendFinishCmd();
    void stopAll();
    QString m_hostname;
//    quint16 m_port;
    int mRetryStartIdx;
protected:
    bool mEnabled;
    QMap<QTimer *,soap_node *> mTimerNodeMap;
    QMap<QtSoapHttpTransport *,soap_node *> mSoapNodeMap;
    soap_node mNodesForRead[SOAP_NODE_READ_COUNT];
    soap_node mNodesForWrite[SOAP_NODE_WRITE_COUNT];
protected:
    soap_node *getEndedNode(soap_node_option option);
    void doNextStep(soap_node *node);
    void setHost(const QString &host);
    void tryChangePortOrSendError(soap_node *node);
signals:
    void getResponse(QtSoapMessage &message, QUuid &uuid, QString &id);
    void signal_SoapError(QUuid &uuid,QString &id);
protected slots:
    void slot_nodeTimeout();
    void slot_getResponse();
};

#endif // QGENIESOAPUTIL_H
