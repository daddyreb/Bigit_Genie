#include "QGenieVisibleNetworkWidget.h"
#include "QGeniePluginCableWidget.h"
#include "QGenieWlanKeyPersistent.h"
#include "Genieplugin_Internet.h"
#include "QGenieDialogContainerFrame.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QStringList>
#include <QSpacerItem>
#include <QList>
#include <QMessageBox>

#ifdef Q_OS_WIN
extern DiagnoseInterface *diagnose_inf;
#elif defined Q_OS_MACX
#include "WiFiApiInterface.h"
#include "WiFiApiGlobal.h"
extern WiFiApiInterface *wifi_api;
#endif

QGenieVisibleNetworkWidget::QGenieVisibleNetworkWidget(const QString &strnicguid,QWidget *parent)
    :/*QDialog*/QGenieDialogInnerWidget(parent),m_optprompt_text_idx(-1),m_strnicguid(strnicguid)
{
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    m_progress_panel              = new QGenieProgressPanel(this);
    m_progress_panel->setFixedSize(25,25);

    optprompt_lb		  =new QLabel(this);
    optprompt_lb->setObjectName(QString("UpdateTitle"));

    optprompt_lb->setWordWrap(true);
    optprompt_lb->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);

    QHBoxLayout *optprompt_progress_layout = new QHBoxLayout();
    optprompt_progress_layout->setMargin(0);
    optprompt_progress_layout->setSpacing(0);
    optprompt_progress_layout->addWidget(optprompt_lb);
    //optprompt_progress_layout->addStretch(1);
    optprompt_progress_layout->addWidget(m_progress_panel);

    available_network_list              =new QGenieAvailableNetworkList(this);

    connect_btn				=new QPushButton(this);
    refresh_btn				=new QPushButton(this);
    close_btn                           =new QPushButton(this);
    connect_btn->setEnabled(false);
    connect_btn->setDefault(true);

    connect_btn->setFocusPolicy(Qt::NoFocus);
    refresh_btn->setFocusPolicy(Qt::NoFocus);
    close_btn->setFocusPolicy(Qt::NoFocus);

    inputpassword_widget = new QGenieInputPasswordWidget(0);
    this->setFocusProxy(inputpassword_widget);
    inputpassword_widget->setParent(this,Qt::Widget);
    inputpassword_widget->setVisible(false);
    inputpassword_widget->setObjectName("InnerDialog");

    QHBoxLayout *hor_layout=new QHBoxLayout();
    hor_layout->addWidget(connect_btn);
    hor_layout->addWidget(refresh_btn);
    hor_layout->addWidget(close_btn);


    QSpacerItem *sp=new QSpacerItem(0,0,QSizePolicy::Expanding);

    QGridLayout *grid_layout = new QGridLayout(this);
    grid_layout->addLayout(optprompt_progress_layout,0,0,1,-1);
    grid_layout->addWidget(available_network_list,1,0,1,-1);
    grid_layout->addWidget(inputpassword_widget,2,0,2,1);
    grid_layout->addItem(sp,2,1,2,1);
    grid_layout->addLayout(hor_layout,3,2,1,1);

    resize(500,400);
    retranslateUi();

    connect(&m_refreshvn_thread,SIGNAL(refresh_complete(bool)),this,SLOT(refresh_complete(bool)));
    connect(connect_btn,SIGNAL(clicked(bool)),this,SLOT(connect_btn_clicked(bool)));
    connect(refresh_btn,SIGNAL(clicked(bool)),this,SLOT(refresh_btn_clicked(bool)));
    connect(close_btn,SIGNAL(clicked(bool)),this,SLOT(close_btn_clicked(bool)));
    connect(available_network_list,SIGNAL(itemSelectionChanged()),this,SLOT(networklist_itemselectionchanged()));
    connect(available_network_list,SIGNAL(itemDoubleClicked ( QListWidgetItem*)),this,SLOT(networklist_itemdoubleclicked(QListWidgetItem*)));

#ifdef Q_OS_WIN
    if(0 == diagnose_inf || !diagnose_inf->IsWzcServiceRunning())
#elif defined Q_OS_MACX
    if( 0 == wifi_api )
#endif
        this->setEnabled(false);
    else
    {
        refresh_btn_clicked(true);
    }
}

void QGenieVisibleNetworkWidget::retranslateUi()
{
    connect_btn->setText(/*tr("Connect")*/
            GeniePlugin_Internet::translateText(L_INTERNET_VNW_CONNECT_BTN_TEXT));
    refresh_btn->setText(/*tr("Refresh")*/
            GeniePlugin_Internet::translateText(L_INTERNET_VNW_REFRESH_BTN_TEXT));
    close_btn->setText(/*tr("Close")*/
            GeniePlugin_Internet::translateText(L_INTERNET_VNW_CLOSE_BTN_TEXT));
    this->setWindowTitle(/*tr("Select a network to connect")*/
            GeniePlugin_Internet::translateText(L_INTERNET_VNW_TITLE));

    optprompt_lb->setText(GeniePlugin_Internet::translateText(m_optprompt_text_idx));
}

void QGenieVisibleNetworkWidget::changeEvent(QEvent *event)
{
    if(event->type()==QEvent::LanguageChange)
        retranslateUi();
    /*QDialog*/QGenieDialogInnerWidget::changeEvent(event);
}

void QGenieVisibleNetworkWidget::refresh_availablenetwork_list(const QString &strNicGuid)
{
    optprompt_lb->setText(/*tr("Being refreshing wireless network ,please wait ...")*/
            GeniePlugin_Internet::translateText(L_INTERNET_VNW_OPTPRMPT_LB_REFRESHING_NETWORK));
    m_optprompt_text_idx = L_INTERNET_VNW_OPTPRMPT_LB_REFRESHING_NETWORK;
    m_progress_panel->start();

#ifdef Q_OS_WIN
    if (0==diagnose_inf || strNicGuid.isEmpty())
#elif defined Q_OS_MACX
    if (0 == wifi_api || strNicGuid.isEmpty())
#endif
    {
        optprompt_lb->setText(/*tr("Refresh wireless network list failed, try again ?")*/
                GeniePlugin_Internet::translateText(L_INTERNET_VNW_OPTPRMPT_LB_REFRESH_NETWORK_FAILED));
        m_optprompt_text_idx = L_INTERNET_VNW_OPTPRMPT_LB_REFRESH_NETWORK_FAILED;
        m_progress_panel->stop();
        refresh_btn->setEnabled(true);
        close_btn->setEnabled(true);
        return;
    }

    m_refreshvn_thread.set_nicguid(strNicGuid);
#ifdef Q_OS_WIN
    m_refreshvn_thread.set_out_vnlist(&m_visible_network);
#elif defined Q_OS_MACX
    m_refreshvn_thread.set_out_vnmaplist(&m_visible_networkmaplist);
#endif
    if(!m_refreshvn_thread.begin_refresh())
    {
        optprompt_lb->setText(/*tr("Refresh wireless network list failed, try again ?")*/
                GeniePlugin_Internet::translateText(L_INTERNET_VNW_OPTPRMPT_LB_REFRESH_NETWORK_FAILED));
        m_optprompt_text_idx = L_INTERNET_VNW_OPTPRMPT_LB_REFRESH_NETWORK_FAILED;

        m_progress_panel->stop();
        refresh_btn->setEnabled(true);
        close_btn->setEnabled(true);
    }

}

//void QGenieVisibleNetworkWidget::connect_btn_clicked(bool checked)
//{
//
//    QGenieAvailableNetworkItem *item=available_network_list->get_selected_item();
//    if (0==item)
//    {
//        QMessageBox::information(this,/*tr("Prompt")*/
//                                 GeniePlugin_Internet::translateText(L_INTERNET_VNW_MBOX_TITLE_PROMPT),
//                                 /*tr("Please select a wireless network.")*/
//                                 GeniePlugin_Internet::translateText(L_INTERNET_VNW_MBOX_TEXT_SELECTNETWORK));
//    }
//    else
//    {
//        m_strssid=item->get_ssid();
//        if (item->get_sec_enabled())
//        {
//            if (inputpassword_widget->input_password_ok()==false
//                || !valid_password(item->get_auth_algorithm(),item->get_encry_algorithm(),
//                                   inputpassword_widget->password()))
//            {
//                QMessageBox::information(this,/*tr("Prompt")*/GeniePlugin_Internet::translateText(L_INTERNET_VNW_MBOX_TITLE_PROMPT),
//                                         GeniePlugin_Internet::translateText(L_INTERNET_VNW_MBOX_TEXT_PWDINVALID)
//                                         /*tr("Password is invalid")*/
//                                         );
//            }
//            else
//            {
//                m_strPwd=inputpassword_widget->password();
//                if(!m_strPwd.isEmpty())
//                    QGenieWlanKeyPersistent::save_key(m_strssid,m_strPwd);
//                accept();
//            }
//        }
//        else
//        {
//            m_strPwd.clear();
//            accept();
//        }
//    }
//}

void QGenieVisibleNetworkWidget::connect_btn_clicked(bool checked)
{
    if(m_refreshvn_thread.isRunning())
    {
        return;
    }

    QGenieAvailableNetworkItem *item=available_network_list->get_selected_item();
    if (0==item)
    {
        QGenieDialogContainerFrame::messageBox(this,/*tr("Prompt")*/
                                 GeniePlugin_Internet::translateText(L_INTERNET_VNW_MBOX_TITLE_PROMPT),
                                 /*tr("Please select a wireless network.")*/
                                 GeniePlugin_Internet::translateText(L_INTERNET_VNW_MBOX_TEXT_SELECTNETWORK),false);
    }
    else
    {
        m_strssid=item->get_ssid();
        if (item->get_sec_enabled())
        {
            QGenieInputPasswordWidget *ipw=new QGenieInputPasswordWidget(item->get_auth_algorithm(),
                                          item->get_encry_algorithm(),this,true);            
            ipw->set_show_password(QGenieWlanKeyPersistent::load_key(item->get_ssid()));
            QGenieDialogContainerFrame dcf(QGenieDialogContainerFrame::Normal,ipw,this);
            QGenieDialogContainerFrame::m_dcfs_list.append(&dcf);
            if(/*ipw.exec()*/dcf.exec()==QDialog::Accepted)
            {
                m_strPwd=ipw->password();
                if(!m_strPwd.isEmpty())
                    QGenieWlanKeyPersistent::save_key(m_strssid,m_strPwd);
                accept();
            }
            QGenieDialogContainerFrame::m_dcfs_list.removeAll(&dcf);
            //ipw->deleteLater();
            delete ipw;
        }
        else
        {
            if(QDialog::Accepted==QGenieDialogContainerFrame::messageBox(this,/*tr("Prompt")*/
                                                          GeniePlugin_Internet::translateText(L_INTERNET_VNW_MBOX_TITLE_PROMPT)
                                                          ,/*tr("Connect to this network is not security,be sure to continue ?")*/
                                                          GeniePlugin_Internet::translateText(L_INTERNET_VNW_MBOX_TEXT_BESURE_CONNECTNETWORK)
                                                          ,true))
            {
                //to connect
                m_strPwd.clear();
                accept();
            }
        }
    }
}

void QGenieVisibleNetworkWidget::networklist_itemdoubleclicked(QListWidgetItem * item )
{
    connect_btn_clicked(true);
}

void QGenieVisibleNetworkWidget::refresh_btn_clicked(bool checked)
{
    if(m_refreshvn_thread.isRunning())
    {
        return;
    }

    refresh_btn->setEnabled(false);
    connect_btn->setEnabled(false);
    close_btn->setEnabled(false);
    inputpassword_widget->setVisible(false);
    refresh_availablenetwork_list(m_strnicguid);
}

void QGenieVisibleNetworkWidget::close_btn_clicked(bool checked)
{
    reject();
}

void QGenieVisibleNetworkWidget::networklist_itemselectionchanged()
{
    QGenieAvailableNetworkItem  *item=available_network_list->get_selected_item();
    if (item!=0)
    {
        //        if(item->get_sec_enabled())
        //        {
        //            inputpassword_widget->set_show_password(QGenieWlanKeyPersistent::load_key(item->get_ssid()));
        //        }
        //        else
        //        {
        //            inputpassword_widget->set_show_password(QString());
        //        }
        //        inputpassword_widget->setVisible(item->get_sec_enabled());
        connect_btn->setEnabled(true);
    }
    else
    {
        //inputpassword_widget->setVisible(false);
        connect_btn->setEnabled(false);
    }
}

bool QGenieVisibleNetworkWidget::get_connect_detail(QString &strSsid,QString &strPwd)
{
    if (m_strnicguid.isEmpty() || m_strssid.isEmpty())
    {
        return false;
    }
    else
    {
        strSsid		=m_strssid;
        strPwd		=m_strPwd;
        return true;
    }
}

void QGenieVisibleNetworkWidget::set_nic_interface(const QString &strnicguid)
{
    if (strnicguid.isEmpty())
    {
        return;
    }
    m_strnicguid=strnicguid;
    m_strssid.clear();
    m_strPwd.clear();
    refresh_availablenetwork_list(m_strnicguid);
}

void QGenieVisibleNetworkWidget::refresh_complete(bool bok)
{
    m_progress_panel->stop();
    available_network_list->clear();
    if(bok)
    {
#ifdef Q_OS_WIN
        for(int i=0;i<m_visible_network.size();++i)
        {
            available_network_list->add_networkitem(m_visible_network[i].strSsid,
                                                    m_visible_network[i].strBssType, m_visible_network[i].strdot11DefaultAuthAlgorithm,m_visible_network[i].strdot11DefaultCipherAlgorithm
                                                    ,m_visible_network[i].bSecurityEnabled,m_visible_network[i].wlanSignalQuality,
                                                    diagnose_inf->GetWlanChannel(m_strnicguid,m_visible_network[i].strSsid,
                                                                                 m_visible_network[i].strBssType,m_visible_network[i].bSecurityEnabled ));
        }
#elif defined Q_OS_MACX
        for(int i = 0;i < m_visible_networkmaplist.size();++i)
        {
            available_network_list->add_networkitem(m_visible_networkmaplist[i][WLAN_NETWORK_SSID],
                                                    m_visible_networkmaplist[i][WLAN_NETWORK_IS_IBSS],
                                                    m_visible_networkmaplist[i][WLAN_NETWORK_SECURITY_MODE],
                                                    m_visible_networkmaplist[i][WLAN_NETWORK_SECURITY_MODE],
                                                    (m_visible_networkmaplist[i][WLAN_NETWORK_SECURITY_MODE].compare(QString("OPEN"),Qt::CaseInsensitive)),
                                                    /*m_visible_networkmaplist[i][WLAN_NETWORK_CUR_CONNECTED].toInt(),*/
                                                    m_visible_networkmaplist[i][WLAN_NETWORK_SIGNAL_LEVEL].toInt(),
                                                    m_visible_networkmaplist[i][WLAN_NETWORK_CHANNEL].toInt());
        }
#endif
        optprompt_lb->setText(/*tr("Please select a wireless network and connect to it.")*/
                GeniePlugin_Internet::translateText(L_INTERNET_VNW_OPTPRMPT_LB_SELECT_NETWORK_2CONNECT));
        m_optprompt_text_idx = L_INTERNET_VNW_OPTPRMPT_LB_SELECT_NETWORK_2CONNECT;
        networklist_itemselectionchanged();
        assert_connect_btn_state();

    }
    else
    {
        optprompt_lb->setText(/*tr("Refresh wireless network list failed, try again ?")*/
                GeniePlugin_Internet::translateText(L_INTERNET_VNW_OPTPRMPT_LB_REFRESH_NETWORK_FAILED));
        m_optprompt_text_idx = L_INTERNET_VNW_OPTPRMPT_LB_REFRESH_NETWORK_FAILED;
    }
    refresh_btn->setEnabled(true);
    close_btn->setEnabled(true);
}

void QGenieVisibleNetworkWidget::assert_connect_btn_state()
{
    QGenieAvailableNetworkItem  *item=available_network_list->get_selected_item();
    if (item!=0)
    {
        connect_btn->setEnabled(true);
    }
    else
    {
        connect_btn->setEnabled(false);
    }
}

//bool QGenieVisibleNetworkWidget::valid_password(const QString &auth,const QString &cipher,const QString &key)
//{
//    // Network key
//    // WEP keys are of 5 or 13 characters (ASCII) or of 10 or 26 characters (hexadecimal)
//    // WPAPSK and WPA2PSK keys are 8-63 characters (ASCII) or 64 characters (hexadecimal)
//    if ( ( (cipher== QString("WEP")) && ( key.length()!=5 ) && ( key.length()!=13 ) && ( key.length()!=10 ) && ( key.length()!=26 ) )
//        ||
//                ( ( (auth==QString("WPAPSK")) || (auth==QString("WPA2PSK"))) && ( ( (key.length() < 8 ) || (key.length() > 64 ) ) ) ) )
//
//        {
//        return false;
//    }
//    else
//    {
//        return true;
//    }
//}

QString QGenieVisibleNetworkWidget::get_dlg_title()
{
    return GeniePlugin_Internet::translateText(L_INTERNET_VNW_TITLE);
}
