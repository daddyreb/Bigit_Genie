#ifndef __NETGEAR_SAOP_SCHEDULE_H__
#define __NETGEAR_SAOP_SCHEDULE_H__
#include <QtCore>
#include <QRunnable>
#include <QUuid>
#include <QVariant>

#include <netgearsoapapi.h>

#define GETINTERVAL 1000 * 20
#define SETINTERVAL 1000 * 50
#define POLLCOUNT 2 * 1
#define OPTCOOUNT 3

typedef enum {
    STARTED = 0,
    SETCODE,
    FINISHED,
    END
}OPTCODESTATUS_E;

class SoapOptcode
{
public:
    SoapOptcode();
    SoapOptcode(QString &modulename, QString &methodname, QMap<QString, QString> &params, QUuid &uuid, QString &id, OPTTYPE_E opttype=READ);
    ~SoapOptcode(){};
        void setSoapParams(QString &modulename, QString &methodname, QMap<QString, QString> &params, QUuid &uuid, QString &id, OPTTYPE_E opttype = READ);
        bool nextstatus();
        void getSoapParams(QString &modulename, QString &methodname, QMap<QString, QString> &params);
        void getUuid(QUuid &uuid){uuid = m_uuid;};
        void getId(QString &id){id = m_id;};
        OPTTYPE_E gettype(){return m_opttype;};
        OPTCODESTATUS_E getStatus(){return m_status;};

private:
        static QString m_modulenamedeviceconfig;
        static QString m_methodnamestart;
        static QString m_methodnamefinish;
        static QMap<QString, QString> m_startparams;
        static QMap<QString, QString> m_finishparams;

        QString m_modulename;
        QString m_methodname;
        QMap<QString, QString> m_params;
        QUuid m_uuid;
        QString m_id;
        OPTTYPE_E m_opttype;
        OPTCODESTATUS_E m_status;
};

class SoapOptcodeQueue : public  SoapOptcode
{
public:
    SoapOptcodeQueue();
    ~SoapOptcodeQueue();
    void insert(QString &modulename, QString &methodname, QMap<QString, QString> &params, QUuid &uuid, QString &id );//, OPTTYPE_E opttype=WRITE);
    bool nextOptcode();
    SoapOptcode * getSoapOptcode();

private:
    static QList<SoapOptcode *> freelist;
    QList<SoapOptcode *> optcodelist;
    QReadWriteLock  m_lock;
};

class NetgearSoapSchedule : public QObject//public QThread, public QRunnable
{
    Q_OBJECT

public:
    NetgearSoapSchedule();
    ~NetgearSoapSchedule();
    void run();
    void stop();
    void quitevent();

private:
    QEventLoop                      *m_eventloop;
    QTimer                          *m_timer;
    QTimer                          *m_timer2;
    NetgearSoapAPI                  *m_netgearsoap_80;
    NetgearSoapAPI                  *m_netgearsoap_5000;
    NetgearSoapAPI                  *m_netgearsoap;
    SoapOptcodeQueue                soapoptcodequeue;
    QReadWriteLock                    m_lock;

    int xorptr;
    void changeport();

signals:
    void getResponse(QtSoapMessage &message, QUuid &uuid, QString &id);
    void submitrequestsignal(QString &modulename, QString &methodname, QMap<QString, QString> &params, QUuid &uuid, QString &id);
    void submitrequesttosoapsignal(QString &modulename, QString &methodname, QMap<QString, QString> &params);
    void sendimmediatelysingal();

public slots:
    void submitrequest(QString &modulename, QString &methodname, QMap<QString, QString> &params, QUuid &uuid, QString &id);
    void getResponseslot(QtSoapMessage &message);
    void send();

private slots:
    void timeout();
};
#endif //__NETGEAR_SAOPAPI_SCHEDULE_H__
