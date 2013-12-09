#ifndef QGENIEREFRESHWCHANNELVIEWTHREAD_H
#define QGENIEREFRESHWCHANNELVIEWTHREAD_H

#include <QThread>
#include <QStringList>
#include <QMutex>
#include <QWaitCondition>

#include "WifiChannelData.h"

class QGenieRefreshWChannelViewThread : public QThread
{
Q_OBJECT
public:
    explicit QGenieRefreshWChannelViewThread(QObject *parent = 0);
    ~QGenieRefreshWChannelViewThread();
private:
    enum {VNT_WAIT,VNT_RUN,VNT_QUIT};
    int m_vntState;

    QMutex          m_mutex;
    QWaitCondition  m_condition;
private:
    bool                                m_bisnetgearrouter;
    bool                                m_bgetchannel_fromsoap;
    bool                                m_bgetchannel_fromsoap_complete;
    bool                                m_bgetchannel_fromapi_complete;
    bool                                m_bgetchannel_fromapi_ok;
    int                                 m_sessionWireless;

    QString                             m_channelfromsoap;
    QString                             m_channelfromsoap_prev;
private:
    WifiChannelData                     m_wifichanneldata;
private:
    void init_soapapi();
    void getcurchannel_fromsoap();
    void try_refreshchannel_allcomplete();

    bool scanWiFiChannel();
private slots:
    //void GetSoapResponse(QtSoapMessage &message,QUuid &uuid, QString &id);
    void slot_DataRreturned(int session,const QStringList& strList);
    void getchannelfromrouter_timeout();
    void on_scanwifi_complete(bool bok);

protected:
    void run();
public:
    void begin_refresh(bool netgearrouter);

signals:
    void refresh_complete(bool bok, WifiChannelData *data);
    void scanwifi_complete(bool bok);
public slots:
    void start(Priority priority = InheritPriority);
    void quit();
};

#endif // QGENIEREFRESHWCHANNELVIEWTHREAD_H
