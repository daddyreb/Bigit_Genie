#include "QGenieWirelessTabWidget.h"
#include "GeniePlugin_Wireless.h"
#include "QGenieRefreshWirelessStateThread.h"
#include "genie2_interface.h"

#include <QPluginLoader>
#include <QButtonGroup>
#include <QtDebug>
#include <QApplication>
#include <QDir>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QUuid>
#include <QTimer>
#include <assert.h>


#ifdef Q_OS_WIN
#include "DiagnoseInterface.h"
DiagnoseInterface                          *diagnose_inf = 0;
#define   WIFIAPI_PLUGIN                    QString("DiagnosePlugin.dll")

#elif defined Q_OS_MACX
#include "WiFiApiInterface.h"
#include "WiFiApiGlobal.h"

WiFiApiInterface                          *wifiApi_inf = 0;
#define   WIFIAPI_PLUGIN                    QString("libWiFiApiPlugin.dylib")
#endif

extern QGenieRefreshWirelessStateThread    g_Refresh_WirelessStatus_Thread;
extern WIRELESS_STATE                      g_Wireless_Status;

QGenieWirelessTabWidget::QGenieWirelessTabWidget(QWidget *parent) :
    QWidget(parent),
    m_wchannel_view(0),
    m_wificonnect_widget(0),
    m_psudo_view(0),
    m_routerconfiguration_widget(0),
    m_cur_tab_index(-1),
    m_wireless_state(WIRELESS_CONNECTED)
{
#ifdef Q_OS_WIN
    connect(&m_wzcmonitor_thread,SIGNAL(wzc_runningstate_changed(bool)),
            this,SLOT(wzc_runningstate_changed(bool)));
    connect(&m_wzcmonitor_thread,SIGNAL(wzc_autoconfstate_changed(QString,bool)),
            this,SLOT(wzc_autoconfstate_changed(QString,bool)));
#endif

    QString app_dir_path = qApp->applicationDirPath();
#ifdef Q_OS_WIN
    QPluginLoader loader(app_dir_path+"/"+WIFIAPI_PLUGIN);
    diagnose_inf = qobject_cast<DiagnoseInterface*>(loader.instance());
#elif defined Q_OS_MACX
    QDir plugins_dir(app_dir_path);
    plugins_dir.cdUp();
    plugins_dir.cd("PlugIns");
    QPluginLoader loader(plugins_dir.absolutePath()+"/"+WIFIAPI_PLUGIN);
    wifiApi_inf = qobject_cast<WiFiApiInterface*>(loader.instance());
#endif

    m_tabwidget = new QTabWidget(this);
    QHBoxLayout  *tab_layout = new QHBoxLayout(this);
#ifdef Q_OS_MAC
    tab_layout->setContentsMargins(12,20,12,12);
#endif
    tab_layout->addWidget(m_tabwidget);
    m_tabwidget->setFocusPolicy(Qt::NoFocus);

    init_tab();

#ifdef Q_OS_WIN
    m_wzcmonitor_thread.start();
#endif
    retranslateUi();

    connect(m_tabwidget,SIGNAL(currentChanged(int)),this,SLOT(currentChanged(int)));

}

QGenieWirelessTabWidget::~QGenieWirelessTabWidget()
{
#ifdef Q_OS_WIN
    m_wzcmonitor_thread.end_monitor();
#endif

    g_Refresh_WirelessStatus_Thread.end_refresh();

#ifdef Q_OS_WIN
    m_wzcmonitor_thread.wait();
#endif

    g_Refresh_WirelessStatus_Thread.wait();

    qDebug()<<"wireless tabwidget destructor end";
}

void QGenieWirelessTabWidget::init_tab()
{

    if(0 == m_tabwidget)
        return;

    m_wificonnect_widget  = new  QGenieWiFiConnectWidget(this);
    m_tabwidget->addTab(m_wificonnect_widget,QString());

    m_wchannel_view = new QGenieWirelessChannelView(this);
    m_tabwidget->addTab(m_wchannel_view,QString());

    m_psudo_view = new QWidget(this);
    m_tabwidget->addTab(m_psudo_view,QString());
}


void QGenieWirelessTabWidget::retranslateUi()
{
    m_tabwidget->setTabText(m_tabwidget->indexOf(m_wchannel_view),/*tr("Channel")*//*tr("Wireless Channel")*/
                            GeniePlugin_Wireless::translateText(L_WIRELESS_WIRELESS_CHANNEL));
    m_tabwidget->setTabText(m_tabwidget->indexOf(m_wificonnect_widget),/*tr("Connect")*//*tr("Connect Wireless")*/
                            GeniePlugin_Wireless::translateText(L_WIRELESS_WIRELESS_CONNECT));

    if(m_tabwidget->indexOf(m_psudo_view) >= 0){
        m_tabwidget->setTabText(m_tabwidget->indexOf(m_psudo_view),
                                GeniePlugin_Wireless::translateText(L_WIRELESS_WIRELESS_GUESTACCESS));
    }
}

void QGenieWirelessTabWidget::changeEvent(QEvent *event)
{
    if(event->type()==QEvent::LanguageChange)
    {
        retranslateUi();
        update_bigbutton_ui();
    }

    QWidget::changeEvent(event);
}

#ifdef Q_OS_WIN
void QGenieWirelessTabWidget::wzc_runningstate_changed(bool brunning)
{
    m_wchannel_view->setEnabled(brunning);
    m_wificonnect_widget->setEnabled(brunning);
    GeniePlugin_Wireless::output_log(QString("wzc runningstate changed"));
}

void QGenieWirelessTabWidget::wzc_autoconfstate_changed(const QString &nic_guid,bool enabled)
{
    if(m_wificonnect_widget)
    {
        m_wificonnect_widget->wzc_autoconfstate_changed(nic_guid,enabled);
    }
}

#endif

void QGenieWirelessTabWidget::wirelessstate_changed(const QString &nic, int newstate,int signallevel)
{
    assert ( (signallevel >=0) && (signallevel <=5) );

    m_wireless_state = newstate;

    update_bigbutton_ui();

    if(/*g_uimanager &&*/ GeniePlugin_Wireless::single_instance())
    {
        QString signalImage("Wireless%1");
        signalImage = signalImage.arg(signallevel);
        //g_uimanager->setBigButtonIcon(signalImage,QUuid("{e722b972-b05e-4d71-8cd9-676ebd1535b9}"));
        GENIE2_UI_COMMAND(UICMD_BIGBUTTON_ICON,signalImage);
    }

    QString str_log("wireless state changed :state = %1,signal level = %2");
    QString str_tmp;
    str_log =  str_log.arg((m_wireless_state == WIRELESS_CONNECTED?"good":"bad"),str_tmp.setNum(signallevel));
    GeniePlugin_Wireless::output_log(str_log);
    qDebug()<<str_log.toStdString().c_str();
}

void QGenieWirelessTabWidget::update_bigbutton_ui()
{
    if(/*g_uimanager && */GeniePlugin_Wireless::single_instance())
    {
        //QString stylesheet;

        switch(m_wireless_state)
        {
        case WIRELESS_CONNECTED:
            //stylesheet = BBT_MODE_GOOD;
            //stylesheet = stylesheet.arg(GeniePlugin_Wireless::translateText(L_WIRELESS_STATUS_USER2_LEFT),
            //GeniePlugin_Wireless::translateText(L_WIRELESS_STATUS_GOOD_RIGHT));
            GENIE2_UI_COMMAND(UICMD_BIGBUTTION_BUTLINE_GOOD,GeniePlugin_Wireless::translateText(L_WIRELESS_STATUS_GOOD_RIGHT));

            break;
        case WIRELESS_NOTCONNECTED:
            //stylesheet = BBT_MODE_ERROR;
            //stylesheet = stylesheet.arg(GeniePlugin_Wireless::translateText(L_WIRELESS_STATUS_USER2_LEFT),
            //GeniePlugin_Wireless::translateText(L_WIRELESS_STATUS_USER_RIGHT));
            GENIE2_UI_COMMAND(UICMD_BIGBUTTION_BUTLINE_ERROR,GeniePlugin_Wireless::translateText(L_WIRELESS_STATUS_USER_RIGHT));

            break;
        case WIRELESS_NOTENABLED:
            //stylesheet = BBT_MODE_ERROR;
            //stylesheet = stylesheet.arg(GeniePlugin_Wireless::translateText(L_WIRELESS_STATUS_USER2_LEFT),
            //GeniePlugin_Wireless::translateText(L_WIRELESS_STATUS_USER2_RIGHT));
            GENIE2_UI_COMMAND(UICMD_BIGBUTTION_BUTLINE_ERROR,GeniePlugin_Wireless::translateText(L_WIRELESS_STATUS_USER2_RIGHT));

            break;
        default:
            qDebug () << "Invalid wireless state code!!!";
            assert(0);
        }

        //g_uimanager->setBigButtonUserStateText(stylesheet,QUuid("{e722b972-b05e-4d71-8cd9-676ebd1535b9}"));
    }
}

void QGenieWirelessTabWidget::enableChangeChannel(bool enable)
{
    m_wchannel_view->enableChangeChannel(enable);
}

void QGenieWirelessTabWidget::enablegotoguestaccess(bool enable)
{
    //m_tabwidget->setTabEnabled(m_tabwidget->indexOf(m_psudo_view), enable);

    if(enable && m_tabwidget->indexOf(m_psudo_view) < 0){
        m_tabwidget->addTab(m_psudo_view,QString());

        retranslateUi();

    }

    if(!enable && m_tabwidget->indexOf(m_psudo_view) >= 0){
        m_tabwidget->removeTab(m_tabwidget->indexOf(m_psudo_view));
    }

}

void QGenieWirelessTabWidget::update_ssid_connected(const QString &ssid)
{
    
    if(ssid.compare(m_current_ssid))
    {
#ifdef ENBABLE_UPDATEWIRELESS_REALTIME
        //refresh the wireless network list
        m_wificonnect_widget->try_refreshnetwork();
#endif

        GENIE2_UI_COMMAND(UICMD_TITLE_SSID,ssid);
        GENIE2_NOTIFY_MESSAGE(SCCMD_SSID,ssid);
    }

    m_current_ssid = ssid;

}


void QGenieWirelessTabWidget::currentChanged ( int index )
{
    if(m_tabwidget->indexOf(m_psudo_view) == index)
    {

        GENIE2_TELL_OTHERPLUGIN(PLUGIN_UUID_NETCONFIG, EPCC_TURN_TO_GACC, 0);
        GENIE2_UI_COMMAND(UICMD_CHECK_LEFTBUT,PLUGIN_UUID_NETCONFIG);


        disconnect(m_tabwidget,SIGNAL(currentChanged(int)),this,SLOT(currentChanged(int)));
        m_tabwidget->setCurrentIndex((-1 == m_cur_tab_index)?0:m_cur_tab_index);
        connect(m_tabwidget,SIGNAL(currentChanged(int)),this,SLOT(currentChanged(int)));
    }
    else
    {
        m_cur_tab_index = index;
    }

    if(m_wificonnect_widget == m_tabwidget->currentWidget())
    {
        m_wificonnect_widget->onNetworkList();
    }

    //statistics
    QString fun_code;
    if(m_tabwidget->indexOf(m_wificonnect_widget) == index)
    {
        fun_code = "W1";
    }
    else if(m_tabwidget->indexOf(m_wchannel_view) == index)
    {
        fun_code = "W5";
    }
    else if(m_tabwidget->indexOf(m_psudo_view) == index)
    {
        fun_code = "W7";
    }

    GENIE2_STATISTICS(fun_code);
    //end
}

void QGenieWirelessTabWidget::notifyRaise()
{
    if(m_wificonnect_widget && (m_wificonnect_widget == m_tabwidget->currentWidget()))
    {
        m_wificonnect_widget->onNetworkList();
    }
}
