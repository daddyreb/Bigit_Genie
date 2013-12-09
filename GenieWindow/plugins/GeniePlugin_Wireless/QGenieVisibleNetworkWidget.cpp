#include "QGenieVisibleNetworkWidget.h"
#include "QGenieWlanKeyPersistent.h"
#include "GeniePlugin_Wireless.h"
#include "QGenieDialogContainerFrame.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QStringList>
#include <QSpacerItem>
#include <QList>
#include <QMessageBox>
#include <QTextCodec>
#include "genie2_interface.h"

#ifdef Q_OS_WIN
#include "DiagnoseInterface.h"
extern DiagnoseInterface *diagnose_inf ;
#elif defined Q_OS_MACX
#include "WiFiApiGlobal.h"
#include "WiFiApiInterface.h"
extern WiFiApiInterface  *wifiApi_inf;
#endif


QGenieVisibleNetworkWidget::QGenieVisibleNetworkWidget(QWidget *parent)
    :QWidget(parent),m_bconnect_or_disconnect(true),m_bconnecting_or_refreshing(false),
#ifdef Q_OS_WIN
    m_optprompt_index((0 == diagnose_inf)?L_WIRELESS_OS_NOT_SUPPORT:L_WIRELESS_CCW_CHANNEL_LB_NOINFORMATION)
#elif defined Q_OS_MACX
    m_optprompt_index((0 == wifiApi_inf)?L_WIRELESS_OS_NOT_SUPPORT:L_WIRELESS_CCW_CHANNEL_LB_NOINFORMATION)
#endif

{
    m_cur_toconnect_item          = 0;
    m_progress_panel              = new QGenieProgressPanel(this);
    m_progress_panel->setFixedSize(25,25);

    optprompt_lb                  = new QLabel(this);
    optprompt_lb->setWordWrap(true);
    optprompt_lb->setObjectName(QString("WirelessTitle"));
    optprompt_lb->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);

    QHBoxLayout *optprompt_progress_layout = new QHBoxLayout();
    optprompt_progress_layout->setMargin(0);
    optprompt_progress_layout->setSpacing(0);
    optprompt_progress_layout->addWidget(optprompt_lb);
    optprompt_progress_layout->addWidget(m_progress_panel);

    available_network_list      = new QGenieAvailableNetworkList(this);

    manager_btn                 = new QPushButton(this);
    connect_btn			= new QPushButton(this);
    refresh_btn			= new QPushButton(this);


#ifndef ENABLE_WIFIPROFILE_MANAGER
    manager_btn->setVisible(false);
#endif

    connect_btn->setEnabled(false);
    connect_btn->setDefault(true);
    //manager_btn->setEnabled(false);

    connect_btn->setFocusPolicy(Qt::NoFocus);
    refresh_btn->setFocusPolicy(Qt::NoFocus);
    manager_btn->setFocusPolicy(Qt::NoFocus);


    QHBoxLayout *hor_layout=new QHBoxLayout();
    hor_layout->addWidget(manager_btn);
    hor_layout->addStretch(10);
    hor_layout->addWidget(connect_btn);
    hor_layout->addWidget(refresh_btn);


    QSpacerItem *sp = new QSpacerItem(0,0,QSizePolicy::Expanding);

    QGridLayout *grid_layout = new QGridLayout(this);
    grid_layout->addLayout(optprompt_progress_layout,0,0,1,-1);
    grid_layout->addWidget(available_network_list,1,0,1,-1);
    grid_layout->addItem(sp,2,0,2,1);
    grid_layout->addLayout(hor_layout,3,0,1,-1);


    resize(500,400);
    retranslateUi();

    connect(&m_refreshvn_thread,SIGNAL(refreshCompleted(bool,QString)),
            this,SLOT(onRefreshCompleted(bool,QString)));

    connect(connect_btn,SIGNAL(clicked()),this,SLOT(connectBtnClicked()));
    connect(refresh_btn,SIGNAL(clicked()),this,SLOT(refreshBtnClicked()));
    connect(available_network_list,SIGNAL(itemSelectionChanged()),this,SLOT(networklist_itemselectionchanged()));
    connect(available_network_list,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(networklist_itemdoubleclicked(QListWidgetItem*)));
    connect(&m_connectwlan_thread,SIGNAL(connect_complete(bool)),this,SLOT(connect_complete(bool)));
    connect(manager_btn,SIGNAL(clicked()),this,SIGNAL(Manager()));

#ifdef Q_OS_WIN
    if(0==diagnose_inf || !diagnose_inf->IsWzcServiceRunning())
        this->setEnabled(false);
#elif defined Q_OS_MACX
    this->setEnabled(wifiApi_inf != 0);
#endif
}

QGenieVisibleNetworkWidget::~QGenieVisibleNetworkWidget()
{
    m_refreshvn_thread.quit();
}

void QGenieVisibleNetworkWidget::retranslateUi()
{
    optprompt_lb->setText(GeniePlugin_Wireless::translateText(m_optprompt_index));
    if(m_bconnect_or_disconnect)
    {

        connect_btn->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_VNW_CONNECT_BTN_CONNECT));
    }
    else
    {
        connect_btn->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_VNW_CONNECT_BTN_DISCONNECT));
    }

    refresh_btn->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_VNW_REFRESH_BTN_TEXT));
    this->setWindowTitle(GeniePlugin_Wireless::translateText(L_WIRELESS_VNW_WND_TITLE));

    manager_btn->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_VNM_MANAGE_BTN_TEXT)/*"Manage"*/);
}

void QGenieVisibleNetworkWidget::changeEvent(QEvent *event)
{
    if(event->type()==QEvent::LanguageChange)
        retranslateUi();
    QWidget::changeEvent(event);
}

void  QGenieVisibleNetworkWidget::showEvent ( QShowEvent * event )
{
    if(
#ifdef Q_OS_WIN
            0 == diagnose_inf /*|| !diagnose_inf->IsWzcServiceRunning()*/
#elif defined Q_OS_MACX
            0 == wifiApi_inf
#endif
            )
    {
        this->setEnabled(false);
    }
    else
    {
        this->setEnabled(true);
        beginRefresh();
    }

    QWidget::showEvent(event);
}



void QGenieVisibleNetworkWidget::connectBtnClicked()
{
    if(m_bconnecting_or_refreshing)
    {
        return;
    }
    m_cur_toconnect_item=available_network_list->get_selected_item();
    if (0==m_cur_toconnect_item)
    {
        QGenieDialogContainerFrame::messageBox(this,GeniePlugin_Wireless::translateText(L_WIRELESS_VNW_MBOX_TITLE),
                                               GeniePlugin_Wireless::translateText(L_WIRELESS_VNW_MBOX_SEL_NETWORK),false);
    }
    else
    {
        if(m_cur_toconnect_item->get_curconnected())
        {
            if(QDialog::Accepted==QGenieDialogContainerFrame::messageBox(this,GeniePlugin_Wireless::translateText(L_WIRELESS_VNW_MBOX_TITLE)
                ,GeniePlugin_Wireless::translateText(L_WIRELESS_VNW_MBOX_BESURE_DISCONNECT),true))
                {
                //to disconnect
                do_connect_disconnect(QString(),QString());
            }
        }
        else
        {
            QString ssid_loadkey = QGenieWlanKeyPersistent::load_key(m_cur_toconnect_item->get_ssid());
            if ( m_cur_toconnect_item->get_sec_enabled() &&
#ifdef Q_OS_WIN
                 (!m_cur_toconnect_item->get_hasprofile()) &&
#endif
                 ssid_loadkey.isEmpty())
            {
                QGenieInputPasswordWidget *ipw=new QGenieInputPasswordWidget(m_cur_toconnect_item->get_auth_algorithm(),
                                                                             m_cur_toconnect_item->get_encry_algorithm(),this,true);
                ipw->set_show_password(ssid_loadkey);
                QGenieDialogContainerFrame dcf(QGenieDialogContainerFrame::Normal,ipw,this);
                if(dcf.exec()==QDialog::Accepted)
                {
                    do_connect_disconnect(m_cur_toconnect_item->get_ssid(),ipw->password());
                }
                delete ipw;
            }
            else
            {
                if(!m_cur_toconnect_item->get_sec_enabled())
                {
                    if(QDialog::Accepted==QGenieDialogContainerFrame::messageBox(this,/*tr("Prompt")*/
                                                                                 GeniePlugin_Wireless::translateText(L_WIRELESS_VNW_MBOX_TITLE)
                                                                                 ,/*tr("Connect to this network is not security,be sure to continue ?")*/
                                                                                 GeniePlugin_Wireless::translateText(L_WIRELESS_VNW_MBOX_BESURE_CONNECT),true))
                    {
                        //to connect
                        do_connect_disconnect(m_cur_toconnect_item->get_ssid(),QString());
                    }
                }
#ifdef Q_OS_WIN
                else if(m_cur_toconnect_item->get_hasprofile())
                {
                    do_connect_disconnect(m_cur_toconnect_item->get_ssid(),QString(),true);
                }
#endif
                else
                {
                    do_connect_disconnect(m_cur_toconnect_item->get_ssid(),ssid_loadkey);
                }


            }
        }
    }
}

void QGenieVisibleNetworkWidget::do_connect_disconnect(const QString &ssid,const QString &pwd,bool buseprofile)
{
    GeniePlugin_Wireless::output_log(QString("do connect or disconnect operation"));

    connect_btn->setEnabled(false);
    refresh_btn->setEnabled(false);
    m_bconnecting_or_refreshing = true;

    if(ssid.isEmpty())
    {
#ifdef Q_OS_WIN
        if(diagnose_inf)
        {
            diagnose_inf->Disconnect(m_strnicguid);
            m_bconnecting_or_refreshing = false;
            beginRefresh();
            return;
        }
#elif defined Q_OS_MACX
        if(wifiApi_inf)
        {
            wifiApi_inf->Disassociate(m_strnicguid);
            m_bconnecting_or_refreshing = false;
            beginRefresh();
            return;
        }
#endif
    }
    else
    {
        m_strssid=ssid;
        if (!pwd.isEmpty())
        {

            m_strPwd=pwd;
            QGenieWlanKeyPersistent::save_key(m_strssid,m_strPwd);
            GeniePlugin_Wireless::output_log(QString("key saved"));

            if(!m_connectwlan_thread.begin_connect(m_strnicguid,m_strssid,m_strPwd))
            {
                optprompt_lb->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_VNW_CONNECT2_NETWORK_FAILED));
                m_optprompt_index = L_WIRELESS_VNW_CONNECT2_NETWORK_FAILED;

                m_bconnecting_or_refreshing = false;
                assert_connect_btn_state();
                refresh_btn->setEnabled(true);
            }
            else
            {
                optprompt_lb->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_VNW_CONNECTING_NETWORK));
                m_optprompt_index = L_WIRELESS_VNW_CONNECTING_NETWORK;

                m_progress_panel->start();
                return;
            }

        }
        else
        {
            m_strPwd.clear();
            GeniePlugin_Wireless::output_log(QString("password is empty"));

            if(!m_connectwlan_thread.begin_connect(m_strnicguid,m_strssid,m_strPwd,buseprofile))
            {
                optprompt_lb->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_VNW_CONNECT2_NETWORK_FAILED));
                m_optprompt_index = L_WIRELESS_VNW_CONNECT2_NETWORK_FAILED;

                m_bconnecting_or_refreshing = false;
                assert_connect_btn_state();
                refresh_btn->setEnabled(true);
            }
            else
            {
                optprompt_lb->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_VNW_CONNECTING_NETWORK));
                m_optprompt_index = L_WIRELESS_VNW_CONNECTING_NETWORK;

                m_progress_panel->start();
                return;
            }
        }
    }

    m_bconnecting_or_refreshing = false;
    assert_connect_btn_state();
    refresh_btn->setEnabled(true);
}

void QGenieVisibleNetworkWidget::refreshBtnClicked()
{
    beginRefresh();
}

void QGenieVisibleNetworkWidget::beginRefresh()
{
    if(m_bconnecting_or_refreshing)
    {
        return;
    }

    refresh_btn->setEnabled(false);
    connect_btn->setEnabled(false);
    m_bconnecting_or_refreshing = true;


    optprompt_lb->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_VNW_REFRESHING_NETWORK));
    m_optprompt_index = L_WIRELESS_VNW_REFRESHING_NETWORK;
    m_progress_panel->start();

    m_strnicguid.clear();

#ifdef Q_OS_WIN
    m_refreshvn_thread.set_out_vnlist(&m_visible_network);
#elif defined Q_OS_MACX
    m_refreshvn_thread.set_out_vn_maplist(&m_network_maplist);
#endif

    m_refreshvn_thread.start();
}

//void QGenieVisibleNetworkWidget::refresh_netgearrouter()
//{
//    IInfoManager *manager = 0;
//    QString         str_result;
//    if(g_manager_handler)
//    {
//        manager = g_manager_handler->getInfoManager();
//    }

//    if(manager)
//    {
//        str_result = manager->getInfo(INFO_CMD_NETGEAR);
//    }
//}


void QGenieVisibleNetworkWidget::networklist_itemselectionchanged()
{
    QGenieAvailableNetworkItem  *item=available_network_list->get_selected_item();
    if (item!=0)
    {
        m_bconnect_or_disconnect = !item->get_curconnected();
        retranslateUi();
        assert_connect_btn_state();
    }
    else
    {
        connect_btn->setEnabled(false);
    }
}

void QGenieVisibleNetworkWidget::networklist_itemdoubleclicked(QListWidgetItem *item )
{
    GENIE2_SET_FOCUS(this);
    connectBtnClicked();
}

void QGenieVisibleNetworkWidget::refresh_complete(bool bok)
{
    m_bconnecting_or_refreshing = false;
    m_progress_panel->stop();
    available_network_list->clear();
    manager_btn->setEnabled(bok);

    if(bok)
    {
        //statistics
        int sec_networkcount = 0;
        int wep_networkcount = 0;
        //end
#ifdef Q_OS_WIN
        int networkcount = m_visible_network.size();
#elif defined Q_OS_MACX
        int networkcount = m_network_maplist.size();
#endif
        for(int i=0;i<networkcount;++i)
        {
#ifdef Q_OS_WIN
            //statistics
            if(m_visible_network[i].bSecurityEnabled)
                ++sec_networkcount;

            if(m_visible_network[i].strdot11DefaultAuthAlgorithm.contains("WEP",Qt::CaseInsensitive))
            {
                ++wep_networkcount;
            }
            //end
            int connectstate = (m_visible_network[i].bcurConnected?1:0);
            if(1 == connectstate)
            {
                QStringList gateways;
                if(diagnose_inf && diagnose_inf->GetInterfaceGateway(m_strnicguid,gateways))
                {
                    if(gateways.count()==0 || gateways[0] == QString("0.0.0.0"))
                    {
                        connectstate = 2;
                    }
                }
            }

            available_network_list->
                    add_networkitem(m_visible_network[i].strSsid,
                                    m_visible_network[i].strBssType,
                                    m_visible_network[i].strdot11DefaultAuthAlgorithm,
                                    m_visible_network[i].strdot11DefaultCipherAlgorithm,
                                    m_visible_network[i].bSecurityEnabled,connectstate,
                                    m_visible_network[i].wlanSignalQuality,
                                    diagnose_inf->GetWlanChannel(m_strnicguid,m_visible_network[i].strSsid,
                                                                 m_visible_network[i].strBssType,m_visible_network[i].bSecurityEnabled ),
                                    m_visible_network[i].bhasProfile,m_visible_network[i].strProfileName);
#elif defined Q_OS_MACX
            //statistics
            if(m_network_maplist[i][WLAN_NETWORK_SECURITY_MODE].compare(QString("OPEN"),Qt::CaseInsensitive))
                ++sec_networkcount;
            if(m_network_maplist[i][WLAN_NETWORK_SECURITY_MODE].contains("WEP",Qt::CaseInsensitive))
            {
                ++wep_networkcount;
            }
            //end
            available_network_list->add_networkitem(m_network_maplist[i][WLAN_NETWORK_SSID],
                                                    m_network_maplist[i][WLAN_NETWORK_IS_IBSS],
                                                    m_network_maplist[i][WLAN_NETWORK_SECURITY_MODE],
                                                    m_network_maplist[i][WLAN_NETWORK_SECURITY_MODE],
                                                    (m_network_maplist[i][WLAN_NETWORK_SECURITY_MODE].compare(QString("OPEN"),Qt::CaseInsensitive)),
                                                    m_network_maplist[i][WLAN_NETWORK_CUR_CONNECTED].toInt(),
                                                    m_network_maplist[i][WLAN_NETWORK_SIGNAL_LEVEL].toInt(),
                                                    m_network_maplist[i][WLAN_NETWORK_CHANNEL].toInt());
#endif
        }
        optprompt_lb->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_VNW_SELNETWORK_2CONNECT));
        m_optprompt_index = L_WIRELESS_VNW_SELNETWORK_2CONNECT;

        networklist_itemselectionchanged();
        assert_connect_btn_state();

        //statistics
        GENIE2_STATISTICS_MAXVALUE("W2",networkcount);
        GENIE2_STATISTICS_MAXVALUE("W3",sec_networkcount);
        GENIE2_STATISTICS_MAXVALUE("W4",networkcount - sec_networkcount);
        GENIE2_STATISTICS_MAXVALUE("W8",wep_networkcount);
        //end
    }
    else
    {
#ifdef Q_OS_WIN
        m_optprompt_index = ((0 == diagnose_inf)?L_WIRELESS_OS_NOT_SUPPORT:L_WIRElESS_VNW_REFRESHNETWORK_FAILED);
#elif defined Q_OS_MACX
        m_optprompt_index = ((0 == wifiApi_inf)?L_WIRELESS_OS_NOT_SUPPORT:L_WIRElESS_VNW_REFRESHNETWORK_FAILED);
#endif
        optprompt_lb->setText(GeniePlugin_Wireless::translateText(m_optprompt_index));

    }
    refresh_btn->setEnabled(true);
}


void QGenieVisibleNetworkWidget::connect_complete(bool bok)
{

    if(bok)
    {
        m_bconnecting_or_refreshing = false;
        m_progress_panel->stop();
        beginRefresh();
    }
    else
    {

        if(m_cur_toconnect_item && m_cur_toconnect_item->get_sec_enabled() && connect_wireless_tryagin())
        {


        }
        else
        {
            if(m_cur_toconnect_item && (!m_cur_toconnect_item->get_sec_enabled()))
            {
                QGenieDialogContainerFrame::messageBox(this,GeniePlugin_Wireless::translateText(L_WIRELESS_VNW_MBOX_TITLE_ERROR),/*tr("Connect to wlan failed.")*/
                                                       GeniePlugin_Wireless::translateText(L_WIRELESS_VNM_MBOX_CONNECT_FAILED),false);
            }

            m_bconnecting_or_refreshing = false;
            m_progress_panel->stop();

            beginRefresh();
        }

    }
}


void QGenieVisibleNetworkWidget::onRefreshCompleted(bool bok , const QString &nicguid)
{
    m_strnicguid = nicguid;
    refresh_complete(bok);
}

void QGenieVisibleNetworkWidget::assert_connect_btn_state()
{
    QGenieAvailableNetworkItem  *item=available_network_list->get_selected_item();
    if (item!=0 && !m_bconnecting_or_refreshing)
    {
        connect_btn->setEnabled(true);
    }
    else
    {
        connect_btn->setEnabled(false);
    }
}

#ifdef Q_OS_WIN
void QGenieVisibleNetworkWidget::wzc_autoconfstate_changed(const QString &nic_guid,bool enabled)
{
    if(m_strnicguid.compare(nic_guid,Qt::CaseInsensitive)==0)
    {
        if((enabled))
        {
            beginRefresh();
        }
        else
        {
            m_optprompt_index = L_WIRELESS_VNM_WZCAUTOCONF_DISABLED;
            retranslateUi();
        }
    }
}
#endif

bool QGenieVisibleNetworkWidget::connect_wireless_tryagin()
{
    bool bret = false;

    QString ssid_loadkey = QGenieWlanKeyPersistent::load_key(m_cur_toconnect_item->get_ssid());

    QGenieInputPasswordWidget *ipw=new QGenieInputPasswordWidget(m_cur_toconnect_item->get_auth_algorithm(),
                                                                 m_cur_toconnect_item->get_encry_algorithm(),this,true);
    ipw->set_show_password(ssid_loadkey);
    QGenieDialogContainerFrame dcf(QGenieDialogContainerFrame::Normal,ipw,this);
    if(dcf.exec()==QDialog::Accepted)
    {
        bret = true;
        do_connect_disconnect(m_cur_toconnect_item->get_ssid(),ipw->password());
    }

    delete ipw;

    return bret;
}

#ifdef ENBABLE_UPDATEWIRELESS_REALTIME
void QGenieVisibleNetworkWidget::try_refreshnetwork()
{
    beginRefresh();
}
#endif
