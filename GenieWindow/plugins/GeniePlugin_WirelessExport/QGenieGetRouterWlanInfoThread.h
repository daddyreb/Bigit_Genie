#ifndef QGENIEGETROUTERWLANINFOTHREAD_H
#define QGENIEGETROUTERWLANINFOTHREAD_H

#include <QThread>
#include <QString>
#include "netgearsoapschedule.h"
#include "qtsoap.h"

class QGenieGetRouterWlanInfoThread : public QThread
{
Q_OBJECT
public:
    explicit QGenieGetRouterWlanInfoThread(QObject *parent = 0);
    ~QGenieGetRouterWlanInfoThread();
protected:
    NetgearSoapSchedule         *m_soapapi;
    bool                                      m_bstart_ok;
    bool                                      m_bgetssid_ok;
    bool                                      m_bgetkey_ok;
    bool                                      m_bcontinue;

    QString                                 m_strnicguid;
    QString                                 m_strprofilename;
    QString                                 m_strssid;
    QString                                 m_strkey;
protected:
    void run();
public:
    bool begind_getwlaninfo();
signals:
    void getwlaninfo_complete(bool bok,const QString &ssid,const QString &key
                              ,const QString &nicguid,const QString &profilename);
public slots:
    void getResponse(QtSoapMessage&,QUuid&,QString&);

};

#endif // QGENIEGETROUTERWLANINFOTHREAD_H
