#ifndef INTERNETDAEMONMSGPROCESSOR_H
#define INTERNETDAEMONMSGPROCESSOR_H

#include <QObject>
#include <QTimer>

#include "IChannelMessageProcessor.h"
#include "ChannelProducer.h"
#include "ChannelConsumer.h"

#ifdef Q_OS_WIN
//#include "QGenieRepairThread.h"
#include "QGenieWinFixThread.h"
#elif defined Q_OS_MACX
#include "QGenieMacxFixThread.h"
#endif

//for restart the router
#include "netgearsoapschedule.h"
//end

class InternetDaemonMsgProcessor : public QObject
        ,public IChannelMessageProcessor
{
Q_OBJECT
public:
    explicit InternetDaemonMsgProcessor(QObject *parent = 0);
    ~InternetDaemonMsgProcessor();
private:
#ifdef Q_OS_WIN
    //QGenieRepairThread  m_repair_thread;
    QGenieWinFixThread  m_repair_thread;
#elif defined Q_OS_MACX
    QGenieMacxFixThread  m_repair_thread;
#endif

    ChannelProducer    *m_channel_p;
    ChannelConsumer    *m_channel_c;
    QTimer              m_internet_keepalive_timer;
    //for restart the router
    NetgearSoapSchedule *m_soapapi;
    //end
private:
    int process_invoke(int sender,int code,const QStringList &str_list);
    int process_return(int sender,int code,const QStringList &str_list);
public:
    virtual int process_message(int sender, int code, int flag, const QStringList &str_list);
signals:
    void internet_keepalive_signal();
private:
    void connec_repairthread_signal();
private slots:
    //slots response to repair thread
    void realtime_message(const QString &msg);
    void realtime_msg2(int idx,const QString &param);
    void show_visiblenetworklist_dlg(const QString &strnicguid,QString *strssid,QString *strpwd);
#ifdef Q_OS_MACX
    void show_chooselocation_dlg(const QStringList &sets);
#endif
    void repair_completed(int fixstate);

    void show_plugincable_dlg(int *result);
    void show_wlanradiohwoff_dlg(int *result);
    void show_restartrouter_dlg();
    void end_show_restartrouter_dlg();
    void begin_pcflash();
    void end_pcflash();
    void begin_routerflash();
    void end_routerflash();
    void begin_internetflash();
    void end_internetflash();
    void begin_pcrouterlinkflash();
    void end_pcrouterlinkflash();
    void begin_routerinternetlinkflash();
    void end_routerinternetlinkflash();
    void begin_connect();
    void end_connect();
    void pcrouter_link_ok();
    void pcrouter_link_failed();
    void pcrouter_link_unknown();

    void routerinternet_link_ok();
    void routerinternet_link_failed();
    void routerinternet_link_unknown();

    void show_messagebox(int title_id,int text_id,bool byesorno,bool *result);
    //slots response to repair thread end
    void internet_keepalive_timeout();
    void internet_keepalive_slot();

    void repairthread_finished();

    //for restart the router
    void reboot_router();
    //end

private:
    void send(int sender,int code,int flag,const QStringList &str_list);
    void exit_internetdaemon();
};

#endif // INTERNETDAEMONMSGPROCESSOR_H
