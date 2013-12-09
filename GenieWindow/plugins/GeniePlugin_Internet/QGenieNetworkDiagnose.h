#ifndef QGENIENETWORKDIAGNOSE_H
#define QGENIENETWORKDIAGNOSE_H

#include <QFrame>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QPushButton>
#include <QEvent>
#include <QShowEvent>
#include <QStackedLayout>
#include <QList>
#include <QMap>
#include <QStringList>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QLabel>


#include "QGenieNetworkStatePanel.h"
#include "QGenieRefreshStatusThread.h"
#include "InternetMsgProcessor.h"
#include "QGenieSpeedTest.h"
#include "QGenieProgressPanel.h"

#ifdef GENIE_ENABLE_NET_MONITOR
class NetTrafficView;
#endif // GENIE_ENABLE_NET_MONITOR

#ifdef Q_OS_WIN
#include "QGenieWzcMonitorThread.h"
#endif

class QGenieNetworkDiagnose : public QFrame
{
    Q_OBJECT
public:
    QGenieNetworkDiagnose(QWidget *parent=0);
    ~QGenieNetworkDiagnose();

#ifdef GENIE_ENABLE_NET_MONITOR
	NetTrafficView *netView;
#endif // GENIE_ENABLE_NET_MONITOR

protected:
    QGenieNetworkStatePanel			*state_panel;
    QTreeWidget					*opt_prompt_list;
    QCheckBox                                   *repair_detail_cb;
    QLabel                                      *repair_brief_label;
    QWidget                                     *repair_brief_panel;
    QGenieProgressPanel                         *repair_brief_progress;
    QPushButton					*opt_btn;
    QPushButton					*cancel_btn;
    QStackedLayout                              *promptlist_nettraffic_layout;
    int                                         repair_brief_label_lang_idx;
    //QGenie_qtNetTraffic                       *nettraffic_panel;
    QFrame                                      *repair_panel;
    QStackedLayout                              *optpromptlist_brieflb_layout;
    //speed test
    QGenieSpeedTest                             speed_test_act;
    QLineEdit                                   *url2test_le;
    QPushButton                                 *test_speed_btn;
    QPushButton                                 *disclaimer_btn;
    QLabel                                      *disclaimer_lb;
    QLabel                                      *testprompt_lb;
    QLabel                                      *speedtest_link_prompt;
    QLabel                                      *speedtesturl_link;
    QWidget                                     *seedtest_panel;
    int                                         time2access_web;
    double                                      average_download_speed;
    int                                         testprompt_lb_lang_idx;

    //end
protected:
    //ipc relative
    InternetMsgProcessor                        *internet_msgprocessor;
    //ipc relative end
    bool                                        is_diagnose_or_repair;//being diagnosing or repairing
#ifdef Q_OS_WIN
    QGenieWzcMonitorThread                      wzc_monitor_thread;
#endif
    QGenieRefreshStatusThread                   m_Refresh_Status_Thread;
    QList<int>                                  promptlist_lang_idx_list;
    QMap<int,QString>                           idx_param_map;
    QStringList                                 prompt_time_list;
    //net traffic warnning
    bool                                        trafficwarnning_flag;
    bool                                        bgohomepage;
    bool                                        bwlanpluginout_after_repair;
    int                                         m_Internet_Status;
    //bool                                        m_bis_canceling_repairthread;
protected:
    void retranslateUi();
    void retranslate_promptlist();
    void clear_prompt_list();

#ifdef Q_OS_WIN
    bool IsWzcServiceRunning();
#endif

protected:
    virtual void changeEvent(QEvent *event);
public:
    bool is_trafficalert_on();
    void notify_raise();
    void notify_lower();
    bool start_refresh_status_thread();
    int internet_status();
private:
    void try_begin_fixit();
public slots:
    void opt_btn_clicked(bool checked);
    void cancel_btn_clicked(bool checked);
    void repair_completed(int fixstate);
    void show_plugincable_dlg(int *result);
    void show_wlanradiohwoff_dlg(int *result);
    void show_restart_router_dlg();
    void show_visiblenetworklist_dlg(const QString &strnicguid,QString *strssid,QString *strpwd);
#ifdef Q_OS_MACX
    void show_chooselocation_dlg(const QStringList &sets);
#endif
    void show_inputpassword_dlg(const QString &str_prompt,bool *result,QString *strpwd);
    void append_realtime_msg2(int idx,const QString &param=QString());
    void show_internet_state();
    void internet_state_changed(int internet_state);
    bool has_nicinterfaces();

#ifdef Q_OS_WIN
    void wzc_runningstate_changed(bool brunning);
#endif

    //net traffic warnning slot
    void TrafficAlert(bool bAlert);

    void showtraffic_singleshot();

    void show_messagebox(int title_id,int text_id,bool byesorno,bool *result);
    void daemon_repairthread_exit();
    void refresh_internet_state_immediate();
private:
    void connect_optthread_statepanel();
    void assert_btn_state();
    void push_status_2mainframe();
    void update_bigbutton_ui();
    void switch_repair_traffic_panel();
    void update_statepanel_and_promptlist();
    void cancel_repair(int code);
    bool eventFilter ( QObject * watched, QEvent * event );
    void cancel_modaldlg_and_msgbox();
public slots:
    void update_routertype();
public:
    void init_detail_cb();
protected slots:
    void internetdaemon_finished();
    void detailcb_toggle(bool checked);
    void testspeed_btn_clicked();
    void disclaimer_btn_clicked();
    void speedtest_complete(int accesstime,double speed_kbps,bool ok);
    void url2test_le_textchanged(const QString & text);
    void internetstate_refresh_singleshot_complete(int state);

    void show_restartrouter_dlg_result(int result);
    void end_show_restart_router_dlg();
    void speedtestlinkActivated(const QString &link);
signals:
    void end_show_restart_router_dlg_signal();
};

#endif // QGENIENETWORKDIAGNOSE_H
