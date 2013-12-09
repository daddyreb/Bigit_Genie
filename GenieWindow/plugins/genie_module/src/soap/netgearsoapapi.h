#ifndef NETGEARSOAPAPI_H
#define NETGEARSOAPAPI_H

#include <qtsoap.h>

typedef enum{
    READ,
    WRITE
}OPTTYPE_E;

class NetgearSoapAPI : public QObject {
    Q_OBJECT
public:
    NetgearSoapAPI();
    NetgearSoapAPI(QString &hostName);
    void setHost( const QString & hostName, quint16 port = 80);
    void abort(){http.abort();};
    QtSoapHttpTransport http;

signals:
    void getResponse(QtSoapMessage &message);

public slots:
    void submitrequest(QString &modulename, QString &methodname, QMap<QString, QString> &params);

private slots:
    void getresponse();

private: 
    //QtSoapHttpTransport http;
    QString hostName;
    QString modulename;
    QString methodname;
    QMap<QString, QString> params;

    void submitsoapmessage();
    void setmodulename(QString &modulename);
    void setmethodname(QString &methodname);
    void setparams(QMap<QString, QString> &params);

    void configurationstarted();
    void configurationfinished();

    bool setflag;
    quint16 m_port;
};

#endif // NETGEARSOAPAPI_H
