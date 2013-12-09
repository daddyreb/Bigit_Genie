#ifndef INTERNETMSGPROCESSOR_H
#define INTERNETMSGPROCESSOR_H
#include "IChannelMessageProcessor.h"
#include "ChannelConsumer.h"
#include "ChannelProducer.h"

#include <QObject>
#include <QTimer>
#include <QSemaphore>
#ifdef Q_OS_WIN
#include <QProcess>
#elif defined Q_OS_MACX
#include "QGeniePrivilegeProcess.h"
#endif
class InternetMsgProcessor : public QObject,public IChannelMessageProcessor
{
Q_OBJECT
public:
    explicit InternetMsgProcessor(QObject *parent = 0);
    ~InternetMsgProcessor();
private:
    int process_invoke(int sender,int code,const QStringList &str_list);
    int process_return(int sender,int code,const QStringList &str_list);
public:
    virtual int process_message(int sender, int code,
                                int flag, const QStringList &str_list);
private:
    QTimer              m_internet_keepalive_timer;
    ChannelProducer     *m_channel_p;
    ChannelConsumer     *m_channel_c;
#ifdef Q_OS_WIN
    QProcess            m_internetdaemon;
#elif defined Q_OS_MACX
    QGeniePrivilegeProcess m_internetdaemon;
#endif
    volatile bool       m_brepairthreadrunning;//repair thread is running in internet daemon flag
    volatile bool       brecv_on;

    volatile bool       m_isStartingFix;
    volatile bool       m_pendingCancel;
private:
    void reset_channel();
public:
    bool start();
    bool update_internetstate(bool bok);
    bool cancel_fix_process(int code = 0);
    bool isRunning();
signals:
    void internetdaemon_finished();
    //signals send to networkdiagnose panel
    void realtime_message(const QString &msg);
    void realtime_msg2(int idx,const QString &param);
    void show_visiblenetworklist_dlg(const QString &strnicguid,QString *strssid,QString *strpwd);
    void show_chooselocation_dlg(const QStringList &sets);
    void repair_completed(int fixstate);

    void show_plugincable_dlg(int *result);
    void show_wlanradiohwoff_dlg(int *result);
    void show_restart_router_dlg();
    void end_show_restart_router_dlg();

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

    void daemon_repairthread_exit();

    void show_messagebox(int title_id,int text_id,bool byesorno,bool *result);

    void mainthread_process_message_signal(int sender, int code,
                                    int flag, const QStringList &str_list);
private:
    bool send(int sender,int code,int flag,const QStringList &str_list);
    //emit signal method
    void emit_realtimemsg(const QStringList &str_list);
    void emit_realtime_msg2(const QStringList &str_list);
    void emit_show_visiblenetworklist_dlg(const QStringList &str_list);
    void emit_show_chooselocation_dlg(const QStringList &sets);
    void messagebox(const QStringList &str_list);
private slots:
    void internetdaemon_finished(int exitcode,QProcess::ExitStatus exitstatus);
    void internetdaemon_started();
    void internet_keepalive_timeout();
    void sendmsg_timeout();

    void mainthread_process_message_slot(int sender, int code,
                                    int flag, const QStringList &str_list);
public slots:
    //send message
    void sendto(int sender,int code,int flag,const QStringList &str_list,bool &ret);
};

#endif // INTERNETMSGPROCESSOR_H
