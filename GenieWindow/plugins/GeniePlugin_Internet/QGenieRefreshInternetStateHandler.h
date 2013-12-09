#ifndef QGENIEREFRESHINTERNETSTATEHANDLER_H
#define QGENIEREFRESHINTERNETSTATEHANDLER_H

#include <QObject>
#include <QHostInfo>
#include "QGenieThreadTimer.h"

class QGenieRefreshInternetStateHandler : public QObject
{
Q_OBJECT
public:
    explicit QGenieRefreshInternetStateHandler(QObject *parent = 0);
    ~QGenieRefreshInternetStateHandler();
private:
#ifdef Q_OS_WIN
    bool verify_netgear_haj_win32(const QHostInfo & info);
#elif defined Q_OS_MACX
    bool verify_netgear_haj_macx(const QHostInfo & info);
#endif

protected:
    int                 m_ilookup_id;
    bool                m_blookingup;
    QGenieThreadTimer   *lookup_timeout_timer;
    int                 m_Internet_Status;
    int                 m_err_count;
    int                 m_cur_domain_idx;
    //bool                m_bsingleshot;
signals:
    void internet_state_changed(int  new_state);
    void refresh_singleshot_complete(int state);
public slots:
     void begin_refresh(bool bsingleshot = false);
protected slots:
    void lookup_completed(const QHostInfo & info);
    void lookup_timeout();
public:
    int internet_status();
protected:
    //bool is_dnsserver_reachable();
    void ok_process();
    void error_process();
    //void singleshot_process();
//protected:
////#ifdef Q_OS_WIN
//    int isPing(const QString &HostName);
////#endif
};

#endif // QGENIEREFRESHINTERNETSTATEHANDLER_H
