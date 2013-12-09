#ifndef QGENIEREPAIRTHREAD_H
#define QGENIEREPAIRTHREAD_H
#include "QGenieDiagnoseCommon.h"
#include "DiagnoseInterface.h"
#include <QThread>

class QGenieRepairThread : public QThread
{
    Q_OBJECT
public:
    QGenieRepairThread();
    ~QGenieRepairThread();
protected:
    volatile bool binternet_stateok;
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
        bool wait_internet_ok();
signals:
	void realtime_message(const QString &msg);
        void realtime_msg2(int idx,const QString &param);
	void show_visiblenetworklist_dlg(const QString &strnicguid,QString *strssid,QString *strpwd);
        //void show_inputpassword_dlg(const QString &str_prompt,bool *result,QString *str_pwd);
	void repair_completed();
private:
	void emit_realtimemsg(const QString &msg);
        void emit_realtime_msg2(int idx,const QString &param=QString());
signals:
        void show_plugincable_dlg(int *result);
        void show_wlanradiohwoff_dlg(int *result);
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
};

#endif // QGENIEREPAIRTHREAD_H
