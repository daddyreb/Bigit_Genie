#ifndef QGENIEWINFIXTHREAD_H
#define QGENIEWINFIXTHREAD_H
#include "QGenieDiagnoseCommon.h"
#include "DiagnoseInterface.h"
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

class QGenieWinFixThread : public QThread
{
    friend class AutoInterrupt;
    Q_OBJECT
protected:
    enum FixStatus{ERROR_STATUS,NEED_ROUTERCFG_WEBPAGE,FIX_OK};
    FixStatus m_fix_status;
public:
    QGenieWinFixThread();
    ~QGenieWinFixThread();
protected:
    //-------------------interruptable api wrapper----------------------------
    int IsNicGatewaysOk(const QString &nic_guid);
    bool EnableDhcp(const QString &nic_guid);
    bool EnableDnsFromDhcp(const QString &nic_guid);
    bool SetInterfaceDnses(const QString &nic_guid,const QStringList &dnses);
    int  RepairGatewayorDnsOk(const QString &nic_guid,bool gatewayordns);
    bool ConfigDNSManually(const QString &nic_guid);
    bool ReConnectWlan(const QString &nic_guid);
    bool ConnectToWlan(const QString &nic_guid,const QString &ssid,const QString &pwd);
    bool ReConnectCurWlan(const QString &nic_guid,const QString &new_pwd);
    bool GetVisibleWlanNetworks(const QString &nic_guid,QList<VisibleNetwork> &vn_list);
    bool GetVisibleWlanNetworks2(const QString &nic_guid,QList<VisibleNetwork> &vn_list,bool brescan);
    //-------------------------end--------------------------------------------
protected:
    QList<NCard>	nic_list;
private:
    QMutex              *m_synmethodcall_mutex;
    QWaitCondition      *m_synmethodcall_condition;

    volatile bool   m_bmessagebox_ret;
    QString         m_shvndlg_retssid;
    QString         m_shvndlg_retpwd;
    int             m_shplugincable_dlg_result;
    int             m_shwlanoff_dlg_result;
    int             m_ifix_canceled;
    int             m_cancel_fix_process_code;
    //for restart router
    bool            m_shrestartrouter_dlg_result;
    //end
protected:
    void run();
private:
        void fixit();
        bool process_wired_nic(const NCard &nic,bool bauto_enabled,bool &need_repeat);
        bool process_wireless_nic(const NCard &nic,bool bauto_enabled,bool bhasmore_nic,bool &need_repeat);
        bool process_gateway_dns(const NCard &nic);
        void process_end_fix(const NCard &nic,bool bhas_morenic);

        bool verify_link(const NCard &nic,bool &bautoen);
        bool verify_cablein(const NCard &nic,bool &bplugin_manually);
        bool test_after_enable_or_plugincale(const NCard &nic);
        bool enable_nic(const NCard &niccard);
        int connect_wlan(const QString &strnicguid);
        bool test_repair_wirelessradio(const NCard &niccard);
        int reconnect_wlan(const NCard &niccard);
        void test_gateway_dns(const NCard &niccard);
        bool plugin_cable(const NCard &niccard);
        int repair_gateway(const NCard &niccard);
        bool repair_dns(const NCard &niccard);
        bool waitdhcp_forgateway(const QString& nicguid);
        bool waitdhcp_fordns(const QString &nicguid);
        void recur_tryrepair_wireless(const NCard &niccard,int ihint,bool bmoreadapter);
        void iprenew_interface(const NCard &niccard);

        bool enum_interfaces();
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
        //after syschronized remote call return
        void show_messagebox_return(bool ret);
        void show_visiblenetworklist_dlg_return
                (const QString &ssid,const QString &pwd);
        void show_plugincable_dlg_return(int ret);
        void show_wlanoff_dlg_return(int ret);
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
protected:
        void fixthread_msleep_interruptable(unsigned long ms);
        void process_cancel_interrupt();
        void restore_nicsstate();

};

class AutoInterrupt
{
protected:
    QGenieWinFixThread* m_fixthread;
public:
    AutoInterrupt(QGenieWinFixThread* fixthread):m_fixthread(fixthread){m_fixthread->process_cancel_interrupt();}
    ~AutoInterrupt(){m_fixthread->process_cancel_interrupt();}
};
#endif // QGENIEWINFIXTHREAD_H
