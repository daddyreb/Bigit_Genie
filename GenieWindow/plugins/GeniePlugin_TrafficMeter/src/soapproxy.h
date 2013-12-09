#ifndef SOAPPROXY_H
#define SOAPPROXY_H

#include "soap/netgearsoapschedule.h"
#include "soap/NetgearSoapAPI.h"
#include "common.h"

#define GET_INTERVAL 1000 * 60
#define SET_INTERVAL 1000 * 60
class SoapProxy : public QObject
{
    Q_OBJECT

public:
    SoapProxy();
    ~SoapProxy();
    static IManagerHandler * sHandler;

private:
    ISoapManager *soapmanager;
    NetgearSoapSchedule *soapapi;
    QUuid uuid;
    QMap<QString,int> idstrmap;
    settrafficmeter_t data;
    void sendsettrafficmeter();
    QTimer * timer;
    int id;
    QString TrafficMeterEnable;

    QString getid(QString &);

signals:
    void response(bool , int);
    void response(bool , int, gettrafficmeter_t &);
    void response(bool , int, settrafficmeter_t &);

public slots:
    void settrafficmeter(settrafficmeter_t &);
    void settrafficmeterenable(settrafficmeter_t &);
    void get();
    void gettrafficmeteroptions();

private slots:
    void GetResponseGetEnable(QtSoapMessage&,QUuid&,QString&);
    void GetResponseSetOptions(QtSoapMessage&,QUuid&,QString&);
    void GetResponseSet(QtSoapMessage&,QUuid&,QString&);
    void GetResponseStatistics(QtSoapMessage&,QUuid&,QString&);
    void GetResponseGetOptions(QtSoapMessage&,QUuid&,QString&);
    void timeout();
    void starttimerget();
    void starttimerset();
    void stoptimer();
    void gettrafficmeterenable();
    void gettrafficmeter();
    void updateid();
};

#endif // SOAPPROXY_H
