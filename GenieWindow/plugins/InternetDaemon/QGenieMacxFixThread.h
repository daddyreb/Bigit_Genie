#ifndef QGENIEMACXFIXTHREAD_H
#define QGENIEMACXFIXTHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

class AutoInterrupt;

class QGenieMacxFixThread : public QThread
{
    friend class AutoInterrupt;

    Q_OBJECT
public:
    QGenieMacxFixThread(QObject *parent = 0);
    ~QGenieMacxFixThread();
protected:
    volatile bool       binternet_stateok;
    volatile int        m_ifix_canceled;
    bool                bneed_routercfg_gui;
private:
    QMutex              *m_synmethodcall_mutex;
    QWaitCondition      *m_synmethodcall_condition;

    volatile bool   m_bmessagebox_ret;
    QString         m_shvndlg_retssid;
    QString         m_shvndlg_retpwd;
    QString         m_chose_setid;
    int             m_shplugincable_dlg_result;
    int             m_shwlanoff_dlg_result;
    bool            m_bis_repairok;
    //for restart router
    bool            m_shrestartrouter_dlg_result;
    //end
protected:
    void run();
private:
        void fixit();
        void sort_niclist(QStringList &nic_list);
        bool verify_location();
        bool verify_activeservice_exist(const QString &nic_name);
        bool verify_link(const QString &nic_name);
        bool verify_cable(const QString &nic_name);
        bool verify_power(const QString &nic_name);
        bool turnup_nic(const QString &nic);
        int connect_wlan(const QString &strnicguid);
        int verify_gateway();
        int verify_dns();
        int verify_gateway_dns(const QString &nic_name);
        QString get_primary_interface();
        QString get_primary_service();

        bool verify_services_ofnic(const QString &nic_name);

        bool iprenew_service(const QString &svrid);

        bool remove_service_setupdns(const QString &svrid);
        bool config_service_setupdns(const QString &svrid);
        bool enable_service_dhcp(const QString &svrid);

        bool make_servicefirst_in_serviceorder(const QString &svrid);

        bool set_service_active(const QString &svrid);
        bool set_serviceipv4_active(const QString &svrid);
signals:
        void realtime_message(const QString &msg);
        void realtime_msg2(int idx,const QString &param);
        void show_visiblenetworklist_dlg(const QString &strnicguid,QString *strssid,QString *strpwd);
#ifdef Q_OS_MACX
        void show_chooselocation_dlg(const QStringList &sets);
#endif
        //fix state 1:ok,2:wlan port not connected,0:failed
        void repair_completed(int fixstate);

        //for restart the router
        void reboot_router();
        //end
private:
        void emit_realtimemsg(const QString &msg);
        void emit_realtime_msg2(int idx,const QString &param=QString());
signals:
        void show_plugincable_dlg(int *result);
        void show_wlanradiohwoff_dlg(int *result);
        //for restart router
        void show_restartrouter_dlg();
        void end_show_restartrouter_dlg();
        //end

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
private:
        void emit_begin_pcflash();
        void emit_end_pcflash();
        void emit_begin_routerflash();
        void emit_end_routerflash();
        void emit_begin_internetflash();
        void emit_end_internetflash();
        void emit_begin_pcrouterlinkflash();
        void emit_end_pcrouterlinkflash();
        void emit_begin_routerinternetlinkflash();
        void emit_end_routerinternetlinkflash();
        void emit_begin_connect();
        void emit_end_connect();
        void emit_pcrouter_link_ok();
        void emit_pcrouter_link_failed();
        void emit_pcrouter_link_unknown();

        void emit_routerinternet_link_ok();
        void emit_routerinternet_link_failed();
        void emit_routerinternet_link_unknown();

        bool messagebox(int title_id,int text_id,bool byesorno);
public:
        void update_internetstate(bool bok);
public:
        //after syschronized remote call return
        void show_messagebox_return(bool ret);
        void show_visiblenetworklist_dlg_return
                (const QString &ssid,const QString &pwd);
#ifdef Q_OS_MACX
        void show_chooselocation_dlg_return(const QString &setname,const QString &setid);
#endif
        void show_plugincable_dlg_return(int ret);
        void show_wlanoff_dlg_return(int ret);
public:
        void restore_nicsstate();
public:
        void start(Priority priority = InheritPriority );
private:
        void wait_synmethodcall();
        void synmethodcall_return();

        //for restart router
        void try_restartrouter(const QString &nic);
        //end
public slots:
        void cancel_fix_process(int code = 0);
        //for restart router
        void show_restartrouter_dlg_return(int result);
        //end
protected:
        void process_cancel_interrupt();
        void fixthread_msleep_interruptable(unsigned long ms);
};

class AutoInterrupt
{
protected:
    QGenieMacxFixThread* m_fixthread;
public:
    AutoInterrupt(QGenieMacxFixThread* fixthread):m_fixthread(fixthread){m_fixthread->process_cancel_interrupt();}
    ~AutoInterrupt(){m_fixthread->process_cancel_interrupt();}
};

#endif // QGENIEMACXFIXTHREAD_H
