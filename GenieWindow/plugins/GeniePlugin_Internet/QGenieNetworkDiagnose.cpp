#include "QGenieNetworkDiagnose.h"
#include "QGeniePluginCableWidget.h"
#include "QGenieSwitchonWlannicRadioWidget.h"
#include "QGenieRestartRouterWidget.h"
#include "QGenieVisibleNetworkWidget.h"
#include "Genieplugin_Internet.h"
#include "QGenieDialogContainerFrame.h"
#include "InternetDaemonDefs.h"
#include "IPCCommon.h"
#include "QGenieDisclaimerWidget.h"
#include "genie2_interface.h"

#include <QHeaderView>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QButtonGroup>
#include <QPluginLoader>
#include <QDateTime>
#include <QTimer>
#include <QApplication>
#include <QCursor>
#include <QDesktopServices>
#include <QDir>
#include <QtDebug>
#include <QUuid>
#include <QVariantList>

#ifdef GENIE_ENABLE_NET_MONITOR
#include "QGenieNetTrafficGraph.h"
#endif // GENIE_ENABLE_NET_MONITOR


#ifdef Q_OS_WIN
#include "DiagnoseInterface.h"
DiagnoseInterface                               *diagnose_inf = 0;
#elif defined Q_OS_MACX
#include "INetApiInterface.h"
#include "WiFiApiInterface.h"

INetApiInterface                                *inet_api = 0;
WiFiApiInterface                                *wifi_api = 0;

#include "QGenieChooseLocationWidget.h"
#include "VerifyDaemonHandler.h"
#endif

#define SHOW_TRAFFIC_TIME2DELAY           10000
#define DEFAULT_SITE2TEST                 "www.netgear.com"
#define SPEED_TEST_URL                    "http://www.speedtest.net"
#define FIX_DETAILS_CHECKBOX_KEY          "Internet/Details"

QGenieNetworkDiagnose::QGenieNetworkDiagnose(QWidget *parent)
    :QFrame(parent),
    repair_brief_label_lang_idx(-1),
    time2access_web(0),
    average_download_speed(0.0),
    testprompt_lb_lang_idx(L_INTERNET_SPEEDTEST_PROMPT_URL_INVALID),
    is_diagnose_or_repair(false),
    trafficwarnning_flag(false),
    bgohomepage(false),
    bwlanpluginout_after_repair(false),
    m_Internet_Status(int(INTERNET_CONNECTED))/*,
    m_bis_canceling_repairthread(false)*/
{
#ifdef Q_OS_WIN
    connect(&wzc_monitor_thread,SIGNAL(wzc_runningstate_changed(bool)),
            this,SLOT(wzc_runningstate_changed(bool)));
#endif

    connect(&m_Refresh_Status_Thread,SIGNAL(internet_state_changed(int)),
            this,SLOT(internet_state_changed(int)));
    //    connect(&m_Refresh_Status_Thread,SIGNAL(refresh_singleshot_complete(int)),
    //            this,SLOT(internetstate_refresh_singleshot_complete(int)));

    //ipc
    internet_msgprocessor = new InternetMsgProcessor(this);
    connect(internet_msgprocessor,SIGNAL(internetdaemon_finished())
            ,this,SLOT(internetdaemon_finished()));
    //ipc

    QString app_dir_path = qApp->applicationDirPath();
#ifdef Q_OS_WIN
    QPluginLoader loader(app_dir_path+QString("/DiagnosePlugin.dll"));
    diagnose_inf = qobject_cast<DiagnoseInterface*>(loader.instance());
#elif defined Q_OS_MACX
    QDir    plugins_dir(app_dir_path);
    plugins_dir.cdUp();
    plugins_dir.cd("PlugIns");
    QPluginLoader loader_inet(plugins_dir.absolutePath()+QString("/libINetApiPlugin.dylib"));
    inet_api = qobject_cast<INetApiInterface*>(loader_inet.instance());

    QPluginLoader loader_wifi(plugins_dir.absolutePath()+QString("/libWiFiApiPlugin.dylib"));
    wifi_api = qobject_cast<WiFiApiInterface*>(loader_wifi.instance());
#endif


    //speed test panel
    url2test_le             = new QLineEdit(this);
    test_speed_btn          = new QPushButton(this);
    disclaimer_btn          = new QPushButton(this);
    disclaimer_lb           = new QLabel(this);
    testprompt_lb           = new QLabel(this);
    speedtest_link_prompt   = new QLabel(this);
    speedtesturl_link       = new QLabel(this);

    url2test_le->installEventFilter(this);
    url2test_le->setMaxLength(255);
    url2test_le->setText(QString(DEFAULT_SITE2TEST));

    test_speed_btn->installEventFilter(this);

    testprompt_lb->setWordWrap(true);
    speedtesturl_link->setTextInteractionFlags(Qt::TextBrowserInteraction);
    speedtesturl_link->setText(QString("<a href=\"http://www.speedtest.net\">http://www.speedtest.net</a>"));
    speedtesturl_link->setFocusPolicy(Qt::NoFocus);

    disclaimer_lb->setTextInteractionFlags(Qt::TextBrowserInteraction);
    disclaimer_lb->setOpenExternalLinks(true);
    disclaimer_lb->setFocusPolicy(Qt::NoFocus);

    QHBoxLayout *speedtest_lebtn_layout = new QHBoxLayout();
#ifdef Q_OS_WIN
    speedtest_lebtn_layout->setSpacing(5);
#else
    speedtest_lebtn_layout->setSpacing(10);
#endif
    speedtest_lebtn_layout->setContentsMargins(0,0,0,0);
    speedtest_lebtn_layout->addWidget(url2test_le);
    speedtest_lebtn_layout->addWidget(test_speed_btn);
    speedtest_lebtn_layout->addWidget(speedtest_link_prompt);
    speedtest_lebtn_layout->addWidget(speedtesturl_link);
    //speedtest_lebtn_layout->addStretch(1);
    speedtest_lebtn_layout->setStretch(0,1);

    QHBoxLayout *prompt_linklb_layout = new QHBoxLayout();
    prompt_linklb_layout->setContentsMargins(0,0,0,0);
    prompt_linklb_layout->setSpacing(5);
    prompt_linklb_layout->addWidget(testprompt_lb);
    //prompt_linklb_layout->addWidget(speedtesturl_link);
    prompt_linklb_layout->setStretch(0,1);
    prompt_linklb_layout->setStretch(1,0);

    QHBoxLayout *disclaimer_lbbtn_layout = new QHBoxLayout();
    disclaimer_lbbtn_layout->setSpacing(2);
    disclaimer_lbbtn_layout->setContentsMargins(0,0,0,0);
    disclaimer_lbbtn_layout->addWidget(disclaimer_lb);
    disclaimer_lbbtn_layout->addWidget(disclaimer_btn);
    disclaimer_lbbtn_layout->setStretch(0,1);
    disclaimer_lbbtn_layout->setStretch(1,0);

    seedtest_panel        = new QWidget(this);
    QVBoxLayout *speedtest_panellayout = new QVBoxLayout(seedtest_panel);
    speedtest_panellayout->setSpacing(2);
    speedtest_panellayout->setContentsMargins(18,0,18,1);
    speedtest_panellayout->addLayout(speedtest_lebtn_layout);
    speedtest_panellayout->addLayout(prompt_linklb_layout);
    //speedtest_panellayout->addWidget(disclaimer_btn,0,Qt::AlignVCenter|Qt::AlignRight);
    speedtest_panellayout->addLayout(disclaimer_lbbtn_layout);
    //speed test panel end

    // state panel
    QFrame	*state_panel_container  =new QFrame(this);

	/**************2012-3-14 modify*****************/
    //state_panel_container->setFrameShape(QFrame::StyledPanel);
	state_panel_container->setFrameShape(QFrame::NoFrame);
	/********************END************************/

    state_panel_container->setFrameShadow(QFrame::Plain);
    state_panel				=new QGenieNetworkStatePanel(state_panel_container);
    QHBoxLayout	*state_panel_layout     =new QHBoxLayout(state_panel_container);
    state_panel_layout->addWidget(state_panel);
    //state panel end

    //state panel and speed test layout
    QVBoxLayout *statepanel_speedtest_layout = new QVBoxLayout();
    statepanel_speedtest_layout->addWidget(state_panel_container);
    statepanel_speedtest_layout->addWidget(seedtest_panel);
    //end

    repair_panel                        = new QFrame(this);
    opt_prompt_list			= new QTreeWidget(this);
    repair_detail_cb                    = new QCheckBox(this);
    repair_detail_cb->setFocusPolicy(Qt::NoFocus);
    repair_brief_label                  = new QLabel(this);
    repair_brief_panel                  = new QWidget(this);
    repair_brief_progress               = new QGenieProgressPanel(this);
    opt_btn			        = new QPushButton(this);
    opt_btn->setEnabled(false);

    repair_brief_progress->setFixedHeight(25);
    repair_brief_progress->setVisible(false);

    repair_brief_label->setTextInteractionFlags(Qt::TextBrowserInteraction);
    repair_brief_label->setOpenExternalLinks(true);
    repair_brief_label->setWordWrap(true);
    repair_brief_label->setAlignment(Qt::AlignCenter);
    repair_brief_label->setProperty("styleZiGreen","1");
    //SET_STYLE_PROPERTY(MAIN_LABEL,repair_brief_label);


    cancel_btn =new QPushButton(this);
    cancel_btn->setEnabled(false);
    opt_prompt_list->setRootIsDecorated(false);
    opt_prompt_list->setAlternatingRowColors(true);
    opt_prompt_list->setHeaderHidden(true);
    opt_prompt_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    opt_prompt_list->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    opt_prompt_list->setColumnCount(2);
    opt_prompt_list->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    opt_prompt_list->header()->setStretchLastSection(false);
    opt_prompt_list->resizeColumnToContents(0);
    opt_prompt_list->resizeColumnToContents(1);

    QSizePolicy szPolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    szPolicy.setHorizontalStretch(0);
    szPolicy.setVerticalStretch(0);
    state_panel_container->setSizePolicy(szPolicy);
    szPolicy.setVerticalStretch(1);
    opt_prompt_list->setSizePolicy(szPolicy);
    repair_brief_label->setSizePolicy(szPolicy);

    QHBoxLayout *horLayout=new QHBoxLayout();
    QSpacerItem *sp1      =new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Minimum);
    QSpacerItem *sp3      =new QSpacerItem(18,18,QSizePolicy::Fixed,QSizePolicy::Minimum);

    horLayout->addWidget(repair_detail_cb);
    horLayout->addItem(sp1);
    horLayout->addWidget(opt_btn);
    horLayout->addItem(sp3);
    horLayout->addWidget(cancel_btn);

    QVBoxLayout *brief_panel_layout = new QVBoxLayout(repair_brief_panel);
    brief_panel_layout->addWidget(repair_brief_progress);
    brief_panel_layout->addWidget(repair_brief_label);

    optpromptlist_brieflb_layout = new QStackedLayout();
    optpromptlist_brieflb_layout->setSpacing(0);
    optpromptlist_brieflb_layout->setMargin(0);
    optpromptlist_brieflb_layout->addWidget(opt_prompt_list);
    optpromptlist_brieflb_layout->addWidget(repair_brief_panel);

    QSpacerItem *sp_4 = new QSpacerItem(0,8,QSizePolicy::Minimum,QSizePolicy::Minimum);
    QVBoxLayout *repair_panel_layout = new QVBoxLayout(repair_panel);
    repair_panel_layout->setSpacing(0);
    repair_panel_layout->setMargin(0);
    //repair_panel_layout->addWidget(opt_prompt_list);
    repair_panel_layout->addLayout(optpromptlist_brieflb_layout);
    repair_panel_layout->addItem(sp_4);
    repair_panel_layout->addLayout(horLayout);

    promptlist_nettraffic_layout = new QStackedLayout();
    promptlist_nettraffic_layout->setMargin(0);
    promptlist_nettraffic_layout->addWidget(repair_panel);

#ifdef GENIE_ENABLE_NET_MONITOR
    netView = new NetTrafficView(this);
    //netView->setContentsMargins(0, 0, 17, 0);
    netView->setMonitor(GeniePlugin_Internet::getNetMonitor());
    promptlist_nettraffic_layout->addWidget(netView);
#endif // GENIE_ENABLE_NET_MONITOR

    QVBoxLayout *verLayout=new QVBoxLayout();
    verLayout->setSpacing(0);
    verLayout->setContentsMargins(6,6,6,6);
    //verLayout->addWidget(state_panel_container);
    verLayout->addLayout(statepanel_speedtest_layout);
    //    verLayout->addWidget(opt_prompt_list);
    verLayout->addLayout(promptlist_nettraffic_layout);
    //verLayout->addItem(sp_4);
    // verLayout->addLayout(horLayout);

    this->setLayout(verLayout);


    retranslateUi();

    connect(url2test_le,SIGNAL(textChanged(QString)),
            this,SLOT(url2test_le_textchanged(QString)));
    connect(&speed_test_act,SIGNAL(speedtest_complete(int,double,bool)),
            this,SLOT(speedtest_complete(int,double,bool)));
    connect(test_speed_btn,SIGNAL(clicked()),
            this,SLOT(testspeed_btn_clicked()));
    connect(disclaimer_btn,SIGNAL(clicked()),this,SLOT(disclaimer_btn_clicked()));

    connect(repair_detail_cb,SIGNAL(toggled(bool)),
            this,SLOT(detailcb_toggle(bool)));
    connect(opt_btn,SIGNAL(clicked(bool)),this,SLOT(opt_btn_clicked(bool)));
    connect(cancel_btn,SIGNAL(clicked(bool)),this,SLOT(cancel_btn_clicked(bool)));

    connect(internet_msgprocessor,SIGNAL(repair_completed(int)),this,SLOT(repair_completed(int)));
    //connect(&internet_msgprocessor,SIGNAL(realtime_message(const QString&)),this,SLOT(append_realtime_msg(const QString&)));
    connect(internet_msgprocessor,SIGNAL(realtime_msg2(int,const QString&)),this,SLOT(append_realtime_msg2(int,const QString&)));
    connect(internet_msgprocessor,SIGNAL(show_visiblenetworklist_dlg(const QString&,QString*,QString*)),
            this,SLOT(show_visiblenetworklist_dlg(const QString&,QString*,QString*))/*,Qt::BlockingQueuedConnection*/);
    connect(internet_msgprocessor,SIGNAL(show_chooselocation_dlg(const QStringList&)),this,SLOT(show_chooselocation_dlg(const QStringList &)));
    //connect(internet_msgprocessor,SIGNAL(show_inputpassword_dlg(const QString &,bool*,QString*)),
    //       this,SLOT(show_inputpassword_dlg(const QString &,bool*,QString*)),Qt::BlockingQueuedConnection);
    connect(internet_msgprocessor,SIGNAL(show_plugincable_dlg(int*)),this,SLOT(show_plugincable_dlg(int*))/*,Qt::BlockingQueuedConnection*/);
    connect(internet_msgprocessor,SIGNAL(show_wlanradiohwoff_dlg(int*)),this,SLOT(show_wlanradiohwoff_dlg(int*))/*,Qt::BlockingQueuedConnection*/);
    connect(internet_msgprocessor,SIGNAL(show_restart_router_dlg()),this,SLOT(show_restart_router_dlg()));
    connect(internet_msgprocessor,SIGNAL(end_show_restart_router_dlg()),
            this,SLOT(end_show_restart_router_dlg()));
    connect(internet_msgprocessor,SIGNAL(show_messagebox(int,int,bool,bool*))
            ,this,SLOT(show_messagebox(int,int,bool,bool*))/*,Qt::BlockingQueuedConnection*/);

    connect(internet_msgprocessor,SIGNAL(daemon_repairthread_exit()),
            this,SLOT(daemon_repairthread_exit())/*,Qt::BlockingQueuedConnection*/);

    connect(speedtesturl_link,SIGNAL(linkActivated(QString)),this,SLOT(speedtestlinkActivated(QString)));
    //detail_cb settings
    init_detail_cb();
    //end

    connect_optthread_statepanel();

#ifdef GENIE_ENABLE_NET_MONITOR
    connect(netView, SIGNAL(trafficAlert(bool)), SLOT(TrafficAlert(bool)));
#endif // GENIE_ENABLE_NET_MONITOR

#ifdef Q_OS_WIN
    wzc_monitor_thread.start();
#endif

}

void QGenieNetworkDiagnose::init_detail_cb()
{

    QString value = GENIE2_GET_SETTING(FIX_DETAILS_CHECKBOX_KEY).toString();
    GeniePlugin_Internet::output_log(QString("init_detail_cb() read configuration value:%1")
                                     .arg(value));
    bool bok = false;
    int ivalue = value.toInt(&bok);
    if(value.isEmpty() || !bok)
    {
        repair_detail_cb->setChecked(false);
        GENIE2_SET_SETTING(FIX_DETAILS_CHECKBOX_KEY,QString("0"));
        GeniePlugin_Internet::output_log(QString("init_detail_cb() set checked"));
    }
    else
    {
        repair_detail_cb->setChecked(ivalue!=0);
        GeniePlugin_Internet::output_log(QString("init_detail_cb() set unchecked"));
    }

    if(repair_detail_cb->isChecked())
    {
        optpromptlist_brieflb_layout->setCurrentWidget(opt_prompt_list);
    }
    else
    {
        optpromptlist_brieflb_layout->setCurrentWidget(repair_brief_panel);
    }
}

QGenieNetworkDiagnose::~QGenieNetworkDiagnose()
{
#ifdef Q_OS_WIN
    wzc_monitor_thread.end_monitor();
    wzc_monitor_thread.wait();
#endif
    m_Refresh_Status_Thread.end_refresh();
    m_Refresh_Status_Thread.wait();

    //ipc
    internet_msgprocessor->cancel_fix_process(1);
    //ipc
}


void QGenieNetworkDiagnose::show_internet_state()
{
    GeniePlugin_Internet::output_log(QString("recv :internet state changed event"));

    //update speed test panel
    seedtest_panel->setVisible(m_Internet_Status == INTERNET_CONNECTED);
    if(m_Internet_Status == INTERNET_CONNECTED)
    {
        if(L_INTERNET_SPEEDTEST_PROMPT_RUNNING_WAITTING != testprompt_lb_lang_idx)
        {
            testprompt_lb_lang_idx = L_INTERNET_SPEEDTEST_PROMPT_URL_INVALID;
        }
        retranslateUi();
    }

    if(is_diagnose_or_repair)
    {
        if(INTERNET_CONNECTED == m_Internet_Status)
        {
            cancel_repair(0);
        }
    }

    //update statepanel and prompt list ui
    GeniePlugin_Internet::output_log(QString("before :update_statepanel_and_promptlist"));
    update_statepanel_and_promptlist();
    GeniePlugin_Internet::output_log(QString("after :update_statepanel_and_promptlist"));


    if(!is_diagnose_or_repair)
    {
        if(INTERNET_DISCONNECTED == m_Internet_Status)
        {
            //            if(g_uimanager)
            //            {
            //                g_uimanager->changeStackIdx(0);
            //            }
            QVariantList lst;
            lst<<-1<<0;
            GENIE2_UI_COMMAND(UICMD_CHECK_LEFTBUT,lst);
        }

        switch_repair_traffic_panel();
    }

    //ipc
    internet_msgprocessor->update_internetstate(INTERNET_CONNECTED == m_Internet_Status);
    //ipc

    //update ui
    if(m_Internet_Status != INTERNET_CONNECTED)
    {
        if(
#ifdef Q_OS_WIN
                IsWzcServiceRunning() &&
#endif
                has_nicinterfaces()
                && (!is_diagnose_or_repair)
                && (!bwlanpluginout_after_repair))
        {
            opt_btn->setEnabled(true);
        }
    }
    else
    {
        opt_btn->setEnabled(false);
        if(!is_diagnose_or_repair)
        {
            cancel_btn->setEnabled(false);
        }
    }

    //notify mainframe the status
    GeniePlugin_Internet::output_log(QString("before :push_status_2mainframe"));
    push_status_2mainframe();
    GeniePlugin_Internet::output_log(QString("after :push_status_2mainframe"));

}

void QGenieNetworkDiagnose::internet_state_changed(int internet_state)
{
    GeniePlugin_Internet::output_log(QString("Internet state changed state = %1").arg(internet_state));

    m_Internet_Status = internet_state;
    show_internet_state();
}

void QGenieNetworkDiagnose::cancel_repair(int code)
{
    //ipc
    internet_msgprocessor->cancel_fix_process(code);
    //ipc

    cancel_modaldlg_and_msgbox();

    repair_brief_progress->setVisible(false);
    repair_brief_progress->stop();

    //    if(g_traymanager)
    //    {
    //        g_traymanager->stopMovie();
    //    }

    state_panel->stop_all_animate();
    is_diagnose_or_repair = false;
    bgohomepage = true;

    //    if(0 == code)
    //    {
    //        //internet ok interrupted repair process,and messages from internet daemon
    //        //are disabled so call repair_completed() with interrupted code 2011 manually
    //        repair_completed(2011);
    //    }
}

void QGenieNetworkDiagnose::update_statepanel_and_promptlist()
{
    switch(m_Internet_Status)
    {
        //    case EPS_WAITTING:
        //        state_panel->pcrouter_link_unknown();
        //        state_panel->routerinternet_link_unknown();
        //        clear_prompt_list();

        //        append_realtime_msg2(L_INTERNET_GETTING_NETSTATE);
        //        break;
    case INTERNET_DISCONNECTED:
        state_panel->pcrouter_link_unknown();
        state_panel->routerinternet_link_failed();
        clear_prompt_list();
        append_realtime_msg2(L_INTERNET_NETWORK_DISCONNECTED);

#ifdef Q_OS_WIN
        if(0 == diagnose_inf)
#elif defined Q_OS_MACX
            if( (0 == inet_api) || (0 == wifi_api) )
#endif
            {
            append_realtime_msg2(L_INTERNET_OS_NOT_SUPPORT);
        }

        break;
    case INTERNET_CONNECTED:
        state_panel->pcrouter_link_ok();
        state_panel->routerinternet_link_ok();
        clear_prompt_list();

        append_realtime_msg2(L_INTERNET_NETWORK_OK);

        break;
    default:
        state_panel->pcrouter_link_unknown();
        state_panel->routerinternet_link_unknown();
        clear_prompt_list();

        append_realtime_msg2(L_INTERNET_GETTING_NETSTATE);
    }
}

void QGenieNetworkDiagnose::switch_repair_traffic_panel()
{
    if(INTERNET_CONNECTED == m_Internet_Status)
    {
        QTimer::singleShot(SHOW_TRAFFIC_TIME2DELAY,this,SLOT(showtraffic_singleshot()));
    }
    else
    {
        promptlist_nettraffic_layout->setCurrentWidget(repair_panel);
        opt_btn->setVisible(true);
        cancel_btn->setVisible(true);
    }
}

void QGenieNetworkDiagnose::push_status_2mainframe()
{
    if((INTERNET_CONNECTED == m_Internet_Status) && trafficwarnning_flag){
        GENIE2_NOTIFY_MESSAGE(SCCMD_CONNECTION_STATE,INTERNET_OVERFLOW);

//        GENIE2_UI_COMMAND(UICMD_TRAY_ICO,":/images/ico_tray_error.ico");
//        GENIE2_UI_COMMAND(UICMD_TRAY_MESSAGE,L_INTERNET_TRAY_TOO_FLOW);
    }
    else {

        if(m_Internet_Status == INTERNET_CONNECTED) {
            GENIE2_NOTIFY_MESSAGE(SCCMD_CONNECTION_STATE,INTERNET_CONNECTED);

//            GENIE2_UI_COMMAND(UICMD_TRAY_ICO,":/images/ico_tray_good.ico");
//            GENIE2_UI_COMMAND(UICMD_TRAY_TOOLTIP,L_INTERNET_TRAY_TOOLTIP_CONNECT);
//            GENIE2_UI_COMMAND(UICMD_TRAY_MESSAGE,L_INTERNET_TRAY_MESSAGE_CONNECT);
        }
        else {
            GENIE2_NOTIFY_MESSAGE(SCCMD_CONNECTION_STATE,INTERNET_DISCONNECTED);

//            GENIE2_UI_COMMAND(UICMD_TRAY_ICO,":/images/ico_tray_error.ico");
//            GENIE2_UI_COMMAND(UICMD_TRAY_TOOLTIP,L_INTERNET_TRAY_TOOLTIP_DISCONNECT);
//            GENIE2_UI_COMMAND(UICMD_TRAY_MESSAGE,L_INTERNET_TRAY_MESSAGE_DISCONNECT);
        }
    }

    //update big button
    update_bigbutton_ui();
}

void QGenieNetworkDiagnose::update_bigbutton_ui()
{
    if(GeniePlugin_Internet::single_instance())
    {
        if((INTERNET_CONNECTED == m_Internet_Status) && trafficwarnning_flag)
        {
            GENIE2_UI_COMMAND(UICMD_BIGBUTTION_BUTLINE_OVERFLOW,GeniePlugin_Internet::translateText(L_INTERNET_STATUS_WAITING_RIGH));
        }
        else
        {
            if(INTERNET_CONNECTED == m_Internet_Status)
            {
                GENIE2_UI_COMMAND(UICMD_BIGBUTTION_BUTLINE_GOOD,GeniePlugin_Internet::translateText(L_INTERNET_STATUS_GOOD));

            }
            else if(INTERNET_DISCONNECTED == m_Internet_Status)
            {
                GENIE2_UI_COMMAND(UICMD_BIGBUTTION_BUTLINE_FIXIT,GeniePlugin_Internet::translateText(L_INTERNET_CLICKHERE_FIX));

            }
        }

    }
}

bool QGenieNetworkDiagnose::has_nicinterfaces()
{
#ifdef Q_OS_WIN
    if(0 == diagnose_inf)
#elif defined Q_OS_MACX
        if( (0 == inet_api) || (0 == wifi_api) )
#endif
        {
        return false;
    }

#ifdef Q_OS_WIN
    QList<NCard> nic_list;
    if (!diagnose_inf->EnumInterfaces(nic_list) || (nic_list.size()==0))
    {
        return false;
    }
#elif defined Q_OS_MACX
    QStringList nic_list;
    if (!inet_api->EnumInterfaces(nic_list) || (nic_list.size()==0))
    {
        return false;
    }
#endif

    return true;
}

void QGenieNetworkDiagnose::connect_optthread_statepanel()
{
    //connect repair thread to state panel
    connect(internet_msgprocessor,SIGNAL(begin_pcflash()),state_panel,SLOT(begin_pcflash()));
    connect(internet_msgprocessor,SIGNAL(end_pcflash()),state_panel,SLOT(end_pcflash()));
    connect(internet_msgprocessor,SIGNAL(begin_routerflash()),state_panel,SLOT(begin_routerflash()));
    connect(internet_msgprocessor,SIGNAL(end_routerflash()),state_panel,SLOT(end_routerflash()));
    connect(internet_msgprocessor,SIGNAL(begin_internetflash()),state_panel,SLOT(begin_internetflash()));
    connect(internet_msgprocessor,SIGNAL(end_internetflash()),state_panel,SLOT(end_internetflash()));
    connect(internet_msgprocessor,SIGNAL(begin_pcrouterlinkflash()),state_panel,SLOT(begin_pcrouterlinkflash()));
    connect(internet_msgprocessor,SIGNAL(end_pcrouterlinkflash()),state_panel,SLOT(end_pcrouterlinkflash()));
    connect(internet_msgprocessor,SIGNAL(begin_routerinternetlinkflash()),state_panel,SLOT(begin_routerinternetlinkflash()));
    connect(internet_msgprocessor,SIGNAL(end_routerinternetlinkflash()),state_panel,SLOT(end_routerinternetlinkflash()));
    connect(internet_msgprocessor,SIGNAL(begin_connect()),state_panel,SLOT(begin_connect()));
    connect(internet_msgprocessor,SIGNAL(end_connect()),state_panel,SLOT(end_connect()));
    connect(internet_msgprocessor,SIGNAL(pcrouter_link_ok()),state_panel,SLOT(pcrouter_link_ok()));
    connect(internet_msgprocessor,SIGNAL(pcrouter_link_failed()),state_panel,SLOT(pcrouter_link_failed()));
    connect(internet_msgprocessor,SIGNAL(pcrouter_link_unknown()),state_panel,SLOT(pcrouter_link_unknown()));
    connect(internet_msgprocessor,SIGNAL(routerinternet_link_ok()),state_panel,SLOT(routerinternet_link_ok()));
    connect(internet_msgprocessor,SIGNAL(routerinternet_link_failed()),state_panel,SLOT(routerinternet_link_failed()));
    connect(internet_msgprocessor,SIGNAL(routerinternet_link_unknown()),state_panel,SLOT(routerinternet_link_unknown()));

}

void QGenieNetworkDiagnose::retranslateUi()
{
    speedtest_link_prompt->setText(GeniePlugin_Internet::translateText(L_INTERNET_SPEEDTEST_SPEEDTESTLINK_PROMPT));
    disclaimer_lb->setText(GeniePlugin_Internet::translateText(L_INTERNET_SPEEDTEST_DISCLAIMER_PROMPT));

    if(repair_brief_label_lang_idx!=-1)
    {
        if(L_INTERNET_RT_OPENCONFIGURATION_GUI == repair_brief_label_lang_idx)
        {
            repair_brief_label->setText(GeniePlugin_Internet::translateText(L_INTERNET_RT_ROUTER_WLANCABLE_MAY_PLUGINOUT)+"\n"+
                                        GeniePlugin_Internet::translateText(repair_brief_label_lang_idx));
        }
        else
        {
            repair_brief_label->setText(GeniePlugin_Internet::translateText(repair_brief_label_lang_idx));
        }
    }
    else
    {
        repair_brief_label->setText("");
    }

    if(L_INTERNET_SPEEDTEST_PROMPT_NORMALRESULT==testprompt_lb_lang_idx)
    {
        QString temp_str1 = QString("%1").arg(average_download_speed,0,'f',2);
        QString temp_str2 = temp_str1;
        testprompt_lb->setText(GeniePlugin_Internet::translateText(testprompt_lb_lang_idx)
                               .arg(temp_str1,temp_str2));
    }
    else if(-1!=testprompt_lb_lang_idx)
    {
        testprompt_lb->setText(GeniePlugin_Internet::translateText(testprompt_lb_lang_idx));
    }

    test_speed_btn->setText(GeniePlugin_Internet::translateText(L_INTERNET_SPEEDTEST_BTN_TEXT));
    disclaimer_btn->setText(GeniePlugin_Internet::translateText(L_INTERNET_SPEEDTEST_DISCLAIMER_BTN_TEXT));
    repair_detail_cb->setText(GeniePlugin_Internet::translateText(L_INTERNET_FIX_SHOWDETAILS_CB_TEXT));
    opt_btn->setText(GeniePlugin_Internet::translateText(L_INTERNET_OPT_BTN_TEXT));
    cancel_btn->setText(GeniePlugin_Internet::translateText(L_INTERNET_CANCEL_BTN_TEXT));
    retranslate_promptlist();

    update_bigbutton_ui();
}

void QGenieNetworkDiagnose::retranslate_promptlist()
{
    int count = opt_prompt_list->topLevelItemCount();
    QTreeWidgetItem *item=0;
    QString param,msg;

    if(count>promptlist_lang_idx_list.count())
    {
        count=promptlist_lang_idx_list.count();
    }

    if(count>prompt_time_list.count())
    {
        count = prompt_time_list.count();
    }

    for(int i=0;i<count;++i)
    {
        msg = GeniePlugin_Internet::translateText(promptlist_lang_idx_list[i]);

        param = idx_param_map.value(promptlist_lang_idx_list[i],QString());
        if(!param.isEmpty())
        {
            msg =msg.arg(param);
        }

        item = opt_prompt_list->topLevelItem(i);
        item->setText(0,prompt_time_list[i]);
        item->setText(1,msg);
    }
}

void QGenieNetworkDiagnose::clear_prompt_list()
{
    opt_prompt_list->clear();
    promptlist_lang_idx_list.clear();
    idx_param_map.clear();
    prompt_time_list.clear();
}


void QGenieNetworkDiagnose::changeEvent(QEvent *event)
{
    if(event->type()==QEvent::LanguageChange)
        retranslateUi();
    QFrame::changeEvent(event);
}

void QGenieNetworkDiagnose::try_begin_fixit()
{
    if(internet_msgprocessor->isRunning())
    {
        qDebug() << "opt btn clicked internet repair is running";
        return;
    }

    bgohomepage                 = false;
    bwlanpluginout_after_repair = false;

    opt_btn->setEnabled(false);
    cancel_btn->setEnabled(true);
    clear_prompt_list();

    if((is_diagnose_or_repair = internet_msgprocessor->start()))
    {
        repair_brief_progress->setVisible(true);
        repair_brief_progress->start();
        //        if(g_traymanager)
        //        {
        //            g_traymanager->startMovie();
        //        }
    }
    else
    {
        if(
#ifdef Q_OS_WIN
                IsWzcServiceRunning() &&
#endif
                has_nicinterfaces()
                && (!is_diagnose_or_repair)
                && (!bwlanpluginout_after_repair))
        {
            opt_btn->setEnabled(true);
        }

        cancel_btn->setEnabled(false);
        append_realtime_msg2(L_INTERNET_START_REPAIR_FAILED);
    }
}

void QGenieNetworkDiagnose::opt_btn_clicked(bool checked)
{
    //statistics
    GeniePlugin_Internet::function_usage_statistics("I5");
    //end
    try_begin_fixit();
}

void QGenieNetworkDiagnose::cancel_btn_clicked(bool checked)
{
    cancel_btn->setEnabled(false);
    //    m_bis_canceling_repairthread = true;

    //ipc
    internet_msgprocessor->cancel_fix_process(1);
    //ipc
}

void QGenieNetworkDiagnose::repair_completed(int fixstate)
{
    is_diagnose_or_repair = false;
    state_panel->stop_all_animate();

    repair_brief_progress->setVisible(false);
    repair_brief_progress->stop();

    int old_status = m_Internet_Status;

    if( 1 == fixstate )
    {
        m_Internet_Status = INTERNET_CONNECTED;
    }
    else
    {

        if(2 == fixstate)
        {
            m_Internet_Status = INTERNET_DISCONNECTED;
            bwlanpluginout_after_repair = true;
        }
        else if(2011 == fixstate)
        {
            if(m_Internet_Status == INTERNET_DISCONNECTED)
            {
                append_realtime_msg2(L_INTERNET_OPT_CANCELED);
            }

        }
        else
        {
            m_Internet_Status = INTERNET_DISCONNECTED;
        }
    }


    bgohomepage = (m_Internet_Status == INTERNET_CONNECTED);

    if(m_Internet_Status != INTERNET_CONNECTED
#ifdef Q_OS_WIN
       && IsWzcServiceRunning()
#endif
       && has_nicinterfaces()
       && (!bwlanpluginout_after_repair))
    {
        opt_btn->setEnabled(true);
    }

    cancel_btn->setEnabled(false);

    //    if(g_traymanager)
    //    {
    //        g_traymanager->stopMovie();
    //    }

    if(old_status != m_Internet_Status)
    {
        show_internet_state();
    }

    //statistics
    GeniePlugin_Internet::function_usage_statistics(bgohomepage?"I8":"I9");
    //end
}

void QGenieNetworkDiagnose::show_plugincable_dlg(int *result)
{
    int i_ret = 0;
    QGeniePluginCableWidget *pcw = new QGeniePluginCableWidget(this);
    QGenieDialogContainerFrame dcf(QGenieDialogContainerFrame::Medium,pcw,this);
    QGenieDialogContainerFrame::m_dcfs_list.append(&dcf);
    if (dcf.exec() == QDialog::Accepted)
    {
        i_ret = 1;
    }
    else
    {
        i_ret = 0;
    }

    if(result)
    {
        *result = i_ret;
    }
    QGenieDialogContainerFrame::m_dcfs_list.removeAll(&dcf);
    delete pcw;

    bool bret = false;
    QStringList str_list;
    QString     str_arg;
    str_arg.setNum(i_ret);
    str_list.append(str_arg);
    internet_msgprocessor->sendto(MESSAGE_SENDER_INVALID
                                  ,INTERNET_SHOW_PLUGINCABLE_DLG,MESSAGE_FLAG_RETURN,str_list,bret);
}

void QGenieNetworkDiagnose::show_wlanradiohwoff_dlg(int *result)
{
    int i_ret = 0;
    QGenieSwitchonWlannicRadioWidget *swrw = new QGenieSwitchonWlannicRadioWidget(this);
    QGenieDialogContainerFrame dcf(QGenieDialogContainerFrame::Medium,swrw,this);
    QGenieDialogContainerFrame::m_dcfs_list.append(&dcf);
    if (dcf.exec() == QDialog::Accepted)
    {
        i_ret = 1;
    }
    else
    {
        i_ret = 0;
    }

    if(result)
    {
        *result = i_ret;
    }
    QGenieDialogContainerFrame::m_dcfs_list.removeAll(&dcf);
    delete swrw;

    bool bret = false;
    QStringList str_list;
    QString     str_arg;
    str_arg.setNum(i_ret);
    str_list.append(str_arg);
    internet_msgprocessor->sendto(MESSAGE_SENDER_INVALID
                                  ,INTERNET_SHOW_WLANRADIOOFF_DLG,MESSAGE_FLAG_RETURN,str_list,bret);
}

void QGenieNetworkDiagnose::show_restart_router_dlg()
{
    QGenieRestartRouterWidget *restartrouter = new QGenieRestartRouterWidget(this);
    connect(restartrouter,SIGNAL(show_restartrouter_dlg_result(int)),this,SLOT(show_restartrouter_dlg_result(int)));
    connect(this,SIGNAL(end_show_restart_router_dlg_signal()),
            restartrouter,SLOT(end_showdlg()));

    QGenieDialogContainerFrame dcf(QGenieDialogContainerFrame::Normal,restartrouter,this);
    QGenieDialogContainerFrame::m_dcfs_list.append(&dcf);

    dcf.exec();

    disconnect(restartrouter,SIGNAL(show_restartrouter_dlg_result(int))
               ,this,SLOT(show_restartrouter_dlg_result(int)));
    disconnect(this,SIGNAL(end_show_restart_router_dlg_signal()),
               restartrouter,SLOT(end_showdlg()));

    QGenieDialogContainerFrame::m_dcfs_list.removeAll(&dcf);
    delete restartrouter;

    //statistics
    GeniePlugin_Internet::function_usage_statistics("I7");
    //end
}

void QGenieNetworkDiagnose::show_visiblenetworklist_dlg(const QString &strnicguid,QString *strssid,QString *strpwd)
{
    QString ssid,pwd;

    QGenieVisibleNetworkWidget *visible_network_list=new QGenieVisibleNetworkWidget(strnicguid,this);
    QGenieDialogContainerFrame dcf(QGenieDialogContainerFrame::Large,visible_network_list,this);
    QGenieDialogContainerFrame::m_dcfs_list.append(&dcf);
    if (QDialog::Accepted == dcf.exec())
    {
        visible_network_list->get_connect_detail(ssid,pwd);
        if(strssid)
        {
            *strssid = ssid;
        }

        if(strpwd)
        {
            *strpwd = pwd;
        }
    }
    else
    {
        if(strssid)
        {
            strssid->clear();
        }

    }
    QGenieDialogContainerFrame::m_dcfs_list.removeAll(&dcf);
    delete visible_network_list;

    bool bret = false;
    QStringList str_list;
    str_list.append(ssid);
    str_list.append(pwd);
    internet_msgprocessor->sendto(MESSAGE_SENDER_INVALID
                                  ,INTERNET_SHOW_VNDLG,MESSAGE_FLAG_RETURN,str_list,bret);
}

#ifdef Q_OS_MACX
void QGenieNetworkDiagnose::show_chooselocation_dlg(const QStringList &sets)
{
    QString sel_setid;
    QGenieChooseLocationWidget *clw = new QGenieChooseLocationWidget(this);
    clw->init_location_list(sets);

    QGenieDialogContainerFrame dcf(QGenieDialogContainerFrame::Normal,clw,this);
    QGenieDialogContainerFrame::m_dcfs_list.append(&dcf);
    if (QDialog::Accepted == dcf.exec())
    {
        sel_setid = clw->get_location();
    }

    qDebug () << "internet user selected set = " << sel_setid;
    QGenieDialogContainerFrame::m_dcfs_list.removeAll(&dcf);
    delete clw;

    bool bret = false;
    QStringList str_list;
    if(!sel_setid.isEmpty())
    {
        str_list.append(sel_setid);
    }

    internet_msgprocessor->sendto(MESSAGE_SENDER_INVALID
                                  ,INTERNET_SHOW_CHOOSELOCACTION_DLG,MESSAGE_FLAG_RETURN,str_list,bret);
}
#endif

void QGenieNetworkDiagnose::show_inputpassword_dlg(const QString &str_prompt,bool *result,QString *strpwd)
{
    QGenieInputPasswordWidget *ipw=new QGenieInputPasswordWidget(this,true);
    QGenieDialogContainerFrame dcf(QGenieDialogContainerFrame::Medium,ipw,this);
    ipw->set_prompt_text(str_prompt);
    QGenieDialogContainerFrame::m_dcfs_list.append(&dcf);
    if (QDialog::Accepted == dcf.exec())
    {
        *result=true;
        *strpwd=ipw->password();
    }
    else
    {
        *result=false;
    }
    QGenieDialogContainerFrame::m_dcfs_list.removeAll(&dcf);
    delete ipw;
}


void QGenieNetworkDiagnose::append_realtime_msg2(int idx,const QString &param)
{
    if(  L_INTERNET_RT_BRIEFPROMPT_RUNNINNG_INTERNET_FIX == idx ||
         L_INTERNET_RT_BRIEFPROMPT_FIX_OK == idx)//brief prompt
    {
        repair_brief_label_lang_idx = idx;
        repair_brief_label->setText(GeniePlugin_Internet::translateText(repair_brief_label_lang_idx));
    }
    else
    {
        if(L_INTERNET_OPT_CANCELED == idx ||
           L_INTERNET_GETTING_NETSTATE == idx ||
           L_INTERNET_NETWORK_DISCONNECTED == idx ||
           L_INTERNET_NETWORK_OK == idx ||
           L_INTERNET_RT_BRIEFPROMPT_FIX_FAILED == idx ||
           L_INTERNET_RT_OPENCONFIGURATION_GUI == idx ||
           L_INTERNET_OS_NOT_SUPPORT == idx)
        {
            repair_brief_label_lang_idx = idx;
            if(L_INTERNET_RT_OPENCONFIGURATION_GUI == repair_brief_label_lang_idx)
            {
                repair_brief_label->setText(GeniePlugin_Internet::translateText(L_INTERNET_RT_ROUTER_WLANCABLE_MAY_PLUGINOUT)+"\n"+
                                            GeniePlugin_Internet::translateText(repair_brief_label_lang_idx));
            }
            else
            {
                repair_brief_label->setText(GeniePlugin_Internet::translateText(repair_brief_label_lang_idx));
            }

        }

        if(idx == L_INTERNET_RT_BRIEFPROMPT_FIX_FAILED)
        {
            idx = L_INTERNET_RT_DETAILPROMPT_FIX_FAILED;
        }

        QDateTime cur_datetime = QDateTime::currentDateTime();
        QString   str_datetime = cur_datetime.toString(QString("hh:mm:ss"));
        QString   str_msg;

        prompt_time_list.append(str_datetime);

        str_msg +="  ";
        if(param.isEmpty())
        {
            str_msg +=GeniePlugin_Internet::translateText(idx);
        }
        else
        {
            str_msg +=GeniePlugin_Internet::translateText(idx).arg(param);
            idx_param_map.insert(idx,param);
        }
        promptlist_lang_idx_list.append(idx);

        QStringList item_strlist;
        item_strlist.append(str_datetime);
        item_strlist.append(str_msg);

        QTreeWidgetItem *twi=new QTreeWidgetItem(item_strlist);

        twi->setSizeHint(0,QSize(70,16));
        twi->setSizeHint(1,QSize(550,16));
        opt_prompt_list->addTopLevelItem(twi);
        opt_prompt_list->scrollToItem(twi);
    }


}

#ifdef Q_OS_WIN
void QGenieNetworkDiagnose::wzc_runningstate_changed(bool brunning)
{
    if(brunning)
    {
        if(m_Internet_Status != INTERNET_CONNECTED
           && has_nicinterfaces()
           && (!is_diagnose_or_repair)
           && (!bwlanpluginout_after_repair))
        {
            opt_btn->setEnabled(true);
        }
    }
    else
    {
        opt_btn->setEnabled(false);
        if(!is_diagnose_or_repair)
        {
            cancel_btn->setEnabled(false);
        }
    }
}

bool QGenieNetworkDiagnose::IsWzcServiceRunning()
{
    if(0==diagnose_inf)
    {
        return false;
    }
    else
    {
        return diagnose_inf->IsWzcServiceRunning2();
    }
}
#endif

void QGenieNetworkDiagnose::TrafficAlert(bool bAlert)
{
    if(bAlert != trafficwarnning_flag)
    {
        trafficwarnning_flag = bAlert;
        show_internet_state();
    }
}

bool QGenieNetworkDiagnose::is_trafficalert_on()
{
    return trafficwarnning_flag;
}

void QGenieNetworkDiagnose::showtraffic_singleshot()
{
    if(INTERNET_CONNECTED == m_Internet_Status)
    {
        //first go to home page
        if(bgohomepage/* && g_uimanager*/)
        {
            //g_uimanager->changeStackIdx(0);
            QVariantList lst;
            lst<<-1<<0;
            GENIE2_UI_COMMAND(UICMD_CHECK_LEFTBUT,lst);
        }

#ifdef GENIE_ENABLE_NET_MONITOR
	promptlist_nettraffic_layout->setCurrentWidget(netView);
#endif // GENIE_ENABLE_NET_MONITOR

        opt_btn->setVisible(false);
        cancel_btn->setVisible(false);
    }

    bgohomepage = false;
}

void QGenieNetworkDiagnose::show_messagebox(int title_id,int text_id,bool byesorno,bool  *result)
{
    int iret = QGenieDialogContainerFrame::messageBox(this,
                                                      GeniePlugin_Internet::translateText(title_id),
                                                      GeniePlugin_Internet::translateText(text_id),byesorno);
    bool b_ret =  (iret == QDialog::Accepted)?true:false;
    if(result)
    {
        *result = b_ret;
    }


    bool        bret = false;
    QStringList str_list;
    QString     str_arg;
    str_arg.setNum(b_ret?1:0);
    str_list.append(str_arg);
    internet_msgprocessor->sendto(MESSAGE_SENDER_INVALID
                                  ,INTERNET_SHOW_MESSAGEBOX,MESSAGE_FLAG_RETURN,str_list,bret);

    //statistics
    if(L_INTERNET_RT_RECUR_TRY_REPAIR_MSGBOX_WIRELESSNETWORK_KEY_MAYWRONG == text_id)
    {
        GeniePlugin_Internet::function_usage_statistics("I6");
    }
    //end

}

void QGenieNetworkDiagnose::update_routertype()
{
    state_panel->update_routerpixmap();
}

void QGenieNetworkDiagnose::internetdaemon_finished()
{
    //m_bis_canceling_repairthread = false;

    if(is_diagnose_or_repair)
    {
        //append_realtime_msg2(L_INTERNET_DAEMON_ABNORMAL_TERMINATION);
        cancel_modaldlg_and_msgbox();

        repair_completed((m_Internet_Status == INTERNET_CONNECTED)?1:0);
#if 0
        this->try_begin_fixit();
#endif
    }
}

void QGenieNetworkDiagnose::detailcb_toggle(bool checked)
{
    if(checked)
    {
        optpromptlist_brieflb_layout->setCurrentWidget(opt_prompt_list);
        GENIE2_SET_SETTING(FIX_DETAILS_CHECKBOX_KEY,QString("1"));
    }
    else
    {
        optpromptlist_brieflb_layout->setCurrentWidget(repair_brief_panel);
        GENIE2_SET_SETTING(FIX_DETAILS_CHECKBOX_KEY,QString("0"));
    }

    static bool first_time = true;
    if(!first_time)
    {
        //statistics
        GeniePlugin_Internet::function_usage_statistics("I4");
        //end
    }

    first_time = false;

}

void QGenieNetworkDiagnose::testspeed_btn_clicked()
{
    QString url2test = url2test_le->text();
    if(speed_test_act.is_url_valid(url2test))
    {
        test_speed_btn->setEnabled(false);
        url2test_le->setEnabled(false);

        if(!speed_test_act.test_speed(url2test))
        {
            test_speed_btn->setEnabled(speed_test_act.is_url_valid(url2test_le->text()));
            url2test_le->setEnabled(true);
            testprompt_lb_lang_idx = L_INTERNET_SPEEDTEST_PROMPT_ERROR_OCUR;
        }
        else
        {
            this->setCursor(QCursor(Qt::WaitCursor));
            testprompt_lb_lang_idx=L_INTERNET_SPEEDTEST_PROMPT_RUNNING_WAITTING;
        }

        retranslateUi();
    }

    //statistics
    GeniePlugin_Internet::function_usage_statistics("I1");
    //end
}

void QGenieNetworkDiagnose::disclaimer_btn_clicked()
{
    QGenieDisclaimerWidget *disclaimer = new QGenieDisclaimerWidget(this);
    QGenieDialogContainerFrame dcf(QGenieDialogContainerFrame::Normal,disclaimer,this);

    dcf.exec();

    delete disclaimer;
}

void QGenieNetworkDiagnose::speedtest_complete(
        int accesstime,double speed_kbps,bool ok)
{
    if(ok)
    {
        time2access_web         = accesstime;
        average_download_speed  = speed_kbps;
        testprompt_lb_lang_idx  = L_INTERNET_SPEEDTEST_PROMPT_NORMALRESULT;
    }
    else
    {
        if(speed_test_act.is_server_nofound())
        {
            testprompt_lb_lang_idx = L_INTERNET_SPEEDTEST_PROMPT_SITE_NOT_FOUND;
        }
        else
        {
            testprompt_lb_lang_idx = L_INTERNET_SPEEDTEST_PROMPT_ERROR_OCUR;
        }
    }

    retranslateUi();
    test_speed_btn->setEnabled(speed_test_act.is_url_valid(url2test_le->text()));
    url2test_le->setEnabled(true);

    this->unsetCursor();
}

void QGenieNetworkDiagnose::url2test_le_textchanged(const QString & text)
{
    bool valid = speed_test_act.is_url_valid(text);
    test_speed_btn->setEnabled(valid);
    //    if(valid)
    //    {
    //        testprompt_lb_lang_idx = L_INTERNET_SPEEDTEST_PROMPT_URL_VALID;
    //    }
    //    else
    //    {
    //        testprompt_lb_lang_idx = L_INTERNET_SPEEDTEST_PROMPT_URL_INVALID;
    //    }

    retranslateUi();
}

bool QGenieNetworkDiagnose::eventFilter (QObject * watched, QEvent * event )
{
    if(watched == url2test_le
       || watched == test_speed_btn)
    {
        if(event->type()==QEvent::KeyRelease)
        {
            QKeyEvent *key_event = static_cast<QKeyEvent *>(event);
            if(key_event->key()==Qt::Key_Enter ||
               key_event->key()==Qt::Key_Return)
            {
                if(test_speed_btn->isEnabled())
                {
                    testspeed_btn_clicked();
                }

                //return true;
            }
        }
    }

    return QFrame::eventFilter(watched,event);
}

void QGenieNetworkDiagnose::notify_raise()
{
    push_status_2mainframe();

    if((m_Internet_Status != INTERNET_CONNECTED)
        && (!is_diagnose_or_repair)
#ifdef Q_OS_WIN
        && IsWzcServiceRunning()
#endif
        && has_nicinterfaces())
        {
        try_begin_fixit();
        //statistics
        GeniePlugin_Internet::function_usage_statistics("H10");
        //end
    }

#ifdef Q_OS_MACX
    bool bveryOk = VerifyDaemonHandler::doVerify();
    qDebug () << "Verfy NETGEARGenieDaemon running " << (bveryOk ? "ok" : "failed");
#endif
}

void QGenieNetworkDiagnose::notify_lower()
{
#ifdef GENIE_ENABLE_NET_MONITOR
    netView->restoreStackPanel();
#endif
}

bool QGenieNetworkDiagnose::start_refresh_status_thread()
{
    if(!m_Refresh_Status_Thread.isRunning())
    {
        m_Refresh_Status_Thread.start();
        //m_Refresh_Status_Thread.begin_refresh();
        return true;
    }
    else
    {
        return false;
    }
}

void QGenieNetworkDiagnose::refresh_internet_state_immediate()
{
    //m_Refresh_Status_Thread.begin_refresh(true);
}

int QGenieNetworkDiagnose::internet_status()
{
    return m_Internet_Status;
}

void QGenieNetworkDiagnose::daemon_repairthread_exit()
{
    if(is_diagnose_or_repair)
    {
        is_diagnose_or_repair = false;
        cancel_btn->setEnabled(false);
        cancel_modaldlg_and_msgbox();
        state_panel->stop_all_animate();
        repair_brief_progress->setVisible(false);
        repair_brief_progress->stop();
        //        if(g_traymanager)
        //        {
        //            g_traymanager->stopMovie();
        //        }
    }

    //    if(m_bis_canceling_repairthread)
    //    {
    //        m_bis_canceling_repairthread = false;
    //        append_realtime_msg2(L_INTERNET_OPT_CANCELED);
    //    }

    if((m_Internet_Status != INTERNET_CONNECTED)
#ifdef Q_OS_WIN
        && IsWzcServiceRunning()
#endif
        && (!bwlanpluginout_after_repair)
        && has_nicinterfaces())
        {
        opt_btn->setEnabled(true);
    }
}

void QGenieNetworkDiagnose::cancel_modaldlg_and_msgbox()
{
    while(!QGenieDialogContainerFrame::m_dcfs_list.isEmpty())
    {
        QGenieDialogContainerFrame::m_dcfs_list.back()->reject();
        QGenieDialogContainerFrame::m_dcfs_list.removeLast();
    }
}

void QGenieNetworkDiagnose::internetstate_refresh_singleshot_complete(int state)
{
    GeniePlugin_Internet::output_log(QString("Internet state single shot refresh complete state = %1").arg(state));
    //    if(g_uimanager)
    //    {
    //        g_uimanager->setBigButtonSubState(GeniePlugin_Internet::single_instance(),EBSS_BUT);
    //    }
}

void QGenieNetworkDiagnose::show_restartrouter_dlg_result(int result)
{
    bool bret = false;
    QStringList arglist;
    QString     arg;
    arg.setNum(result);
    arglist << arg;
    internet_msgprocessor->sendto(MESSAGE_SENDER_INVALID
                                  ,INTERNET_SHOW_RESTARTROUTER_DLG,MESSAGE_FLAG_RETURN,arglist,bret);
}

void QGenieNetworkDiagnose::end_show_restart_router_dlg()
{
    emit end_show_restart_router_dlg_signal();
}

void QGenieNetworkDiagnose::speedtestlinkActivated(const QString &link)
{
    QDesktopServices::openUrl(QUrl(link));
    GeniePlugin_Internet::function_usage_statistics("I2");
}
