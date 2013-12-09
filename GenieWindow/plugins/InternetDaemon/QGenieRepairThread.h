#ifndef QGENIEREPAIRTHREAD_H
#define QGENIEREPAIRTHREAD_H
#include "QGenieDiagnoseCommon.h"
#include "DiagnoseInterface.h"
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

class QGenieRepairThread : public QThread
{
    Q_OBJECT
public:
    QGenieRepairThread();
    ~QGenieRepairThread();
protected:
    volatile bool       binternet_stateok;
    bool                bneed_routercfg_gui;
    QList<NCard>	nic_list;
private:
    QMutex              *m_synmethodcall_mutex;
    QWaitCondition      *m_synmethodcall_condition;

    volatile bool   m_bmessagebox_ret;
    QString         m_shvndlg_retssid;
    QString         m_shvndlg_retpwd;
    int             m_shplugincable_dlg_result;
    int             m_shwlanoff_dlg_result;
    bool            m_bis_repairok;
    //for restart router
    bool            m_shrestartrouter_dlg_result;
    //end
protected:
    void run();
private:
        void Repair();
        bool enable_nic(const NCard &niccard);
        int connect_wlan(const QString &strnicguid);
        bool test_repair_wirelessradio(const NCard &niccard);
        int reconnect_wlan(const NCard &niccard);
        int test_gateway_dns(const NCard &niccard);
        bool plugin_cable(const NCard &niccard);
        int repair_gateway(const NCard &niccard);
        bool repair_dns(const NCard &niccard);
        bool waitdhcp_forgateway(const QString& nicguid);
        bool waitdhcp_fordns(const QString &nicguid);
        int recur_tryrepair_wireless(const NCard &niccard,int ihint,bool bmoreadapter);
        int iprenew_interface(const NCard &niccard);
signals:
	void realtime_message(const QString &msg);
        void realtime_msg2(int idx,const QString &param);
	void show_visiblenetworklist_dlg(const QString &strnicguid,QString *strssid,QString *strpwd);
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
        void try_restartrouter(const NCard &nic);
        //end
public:
        void cancel_fix_process(int code);
        //for restart router
        void show_restartrouter_dlg_return(int result);
        //end
};

#endif // QGENIEREPAIRTHREAD_H
