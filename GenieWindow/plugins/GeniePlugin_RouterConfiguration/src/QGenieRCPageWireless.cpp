#include "QGenieRCPageWireless.h"
#include "QGenieExportSettingExchangeInf.h"
#include "GeniePluginTest.h"
#include "inc/genie_interface/IManagerHandler.h"
#include "inc/multilanguage_def/LanguageRouterConfiguration.h"
#include "QRCManagerHandler.h"
#include "QGenieRCSoapProxy.h"
#include "plugin_ui_def.h"
#include "ui_QGenieRCPageGuessAccess.h"
#include "mainwindow.h"
#include "IClickManager.h"
#ifdef ENABLE_READYSHARE
#include "IUiManager.h"
#endif
#include "ILogManager.h"
#include <QDebug>
#include "QGenieRCLogic.h"

#define DEFAULT_SELECTION 1
#define TAB_MOD "  %1  "

#ifdef ENABLE_READYSHARE
    #define GACC_IDX 2
#else
    #define GACC_IDX 1
#endif
#define WIRELESS_IDX 0

QGenieRCPageWireless::QGenieRCPageWireless(QWidget* parent)
    :QWidget(parent)
    ,m_wirelessexport_widget(NULL)
    ,m_TrafficMeter_widget(NULL)
#ifdef ENABLE_READY_SHARE
    ,m_ReadyShare_Widget(NULL)
#endif
    ,m_RouterUpdate_widget(NULL)
    ,mChannelIndex(0)
    ,mModeIndex(0)
    ,mErrStrIdx(-1)
    ,mCurrentWpaRdt(NULL)
    //    ,mPendingRequest(0)
    ,mNotifyWpa("")
    ,mNotifyRZ("")
    ,mNotifySsid("")
    ,mNotifyPwd("")
    //    ,mApplyTimeLine(REFRESH_WAITING_MSECS,this)
{
    mUi.setupUi(this);
    mUi.but_export->hide();


    mUuid =UUID_OF_THIS;


    SET_STYLE_PROPERTY(FLEX_BUTTON,mUi.but_info_refresh);
    SET_STYLE_PROPERTY(FLEX_BUTTON,mUi.but_info_cancel);
    SET_STYLE_PROPERTY(FLEX_BUTTON,mUi.but_modify);
    SET_STYLE_PROPERTY(FLEX_BUTTON,mUi.but_export);

    SET_STYLE_PROPERTY(FLEX_BUTTON,mUi.but_set_refresh);
    SET_STYLE_PROPERTY(FLEX_BUTTON,mUi.but_apply);
    SET_STYLE_PROPERTY(FLEX_BUTTON,mUi.but_set_cancel);



    connect(mUi.tabWidget,SIGNAL(currentChanged(int)),this,SLOT(slot_TabIdxChanged(int)));
    //    connect(&mTimers[EP_INFO],SIGNAL(timeout()),this,SLOT(slot_Timeout()));
    connect(&mTimer,SIGNAL(timeout()),this,SLOT(slot_Timeout()));
    connect(mUi.but_info_refresh,SIGNAL(clicked()),this,SLOT(slot_refresh()));
    connect(mUi.but_set_refresh,SIGNAL(clicked()),this,SLOT(slot_refresh()));

    connect(mUi.edt_pwd,SIGNAL(textChanged(QString)),this,SLOT(slot_EnableApply()));
    connect(mUi.edt_set_ssid,SIGNAL(textChanged(QString)),this,SLOT(slot_EnableApply()));
    connect(mUi.rbt_all,SIGNAL(clicked()),this,SLOT(slot_EnableApply()));
    connect(mUi.rbt_none,SIGNAL(clicked()),this,SLOT(slot_EnableApply()));
    connect(mUi.rbt_wpa2,SIGNAL(clicked()),this,SLOT(slot_EnableApply()));
    connect(mUi.cob_channel,SIGNAL(currentIndexChanged(int)),this,SLOT(slot_EnableApply()));
    connect(mUi.cob_mode,SIGNAL(currentIndexChanged(int)),this,SLOT(slot_EnableApply()));

    connect(&mRefreshDelay,SIGNAL(timeout()),this,SLOT(slot_refreshDelayTimeout()));

    retranslateUi();
#ifdef   NOT_SET_MODE
    mUi.label_set_mode_title->hide();
    mUi.cob_mode->hide();

#endif
}


QGenieRCPageWireless::~QGenieRCPageWireless()
{
    mTimer.stop();
    mRefreshDelay.stop();
}
//Change Wireless Page Language
void QGenieRCPageWireless::changeEvent(QEvent* event)
{
    QWidget::changeEvent(event);
    switch(event->type())
    {
    case QEvent::LanguageChange:
        retranslateUi();
        break;
    default:
        break;
    }
}
//Set Wireless Page text
void QGenieRCPageWireless::retranslateUi()
{
    lanmanager=MainWindow::sHandler->getLanguageManager();
    QString stemp=QString(TAB_MOD).arg(lanmanager->getText(L_RTCF_TAB_TITLE,mUuid));
    mUi.tabWidget->setTabText(0, stemp);
    stemp=QString(TAB_MOD).arg(lanmanager->getText(L_GACC_BIGBT,mUuid));
#ifdef ENABLE_READYSHARE
    mUi.tabWidget->setTabText(2,stemp);
#else
    mUi.tabWidget->setTabText(1,stemp);
#endif

    GeniePlugin_RouterConfiguration *inf=GeniePlugin_RouterConfiguration::GetSingletonPtr();
    ASSERT(inf);
    IPluginInf *pi=inf->mPlugins[GeniePlugin_RouterConfiguration::SP_TRAFFICMETER];
    if(m_TrafficMeter_widget && pi)
    {
        stemp=QString(TAB_MOD).arg(pi->text());

        mUi.tabWidget->setTabText(mUi.tabWidget->indexOf(m_TrafficMeter_widget),stemp);
    }

#ifdef ENABLE_READYSHARE
    pi = inf->mPlugins[GeniePlugin_RouterConfiguration::SP_READYSHARE];
    if(m_ReadyShare_Widget && pi)
    {
//        QStringList text_list = pi->text().split(":");
//        bool        bconver   = false;

//        Q_ASSERT(text_list.size() >= 2);
//        int langidx = text_list[1].toInt(&bconver);
//        Q_ASSERT(bconver);

        stemp = QString(TAB_MOD).arg(lanmanager->getText(L_READY_SHARE_BIGBT,mUuid/*langidx,inf->getReadyShareUuid()*/));

        mUi.tabWidget->setTabText(mUi.tabWidget->indexOf(m_ReadyShare_Widget),stemp);
    }

#endif

    pi=inf->mPlugins[GeniePlugin_RouterConfiguration::SP_ROUTERUPDATE];
    if(m_RouterUpdate_widget && pi)
    {
        stemp=QString(TAB_MOD).arg(pi->text());
        mUi.tabWidget->setTabText(mUi.tabWidget->indexOf(m_RouterUpdate_widget),stemp);
    }


    //    mUi.label_set_wn->setText(lanmanager->getText(L_RTCF_SETTING_WIRELESS,mUuid));

    mUi.label_set_ssid_title->setText(lanmanager->getText(L_RTCF_SETTING_SSID,mUuid));
    //    mUi.label_set_region_title->setText(lanmanager->getText(L_RTCF_SETTING_REGION,uuid));
    mUi.label_set_channel_title->setText(lanmanager->getText(L_RTCF_SETTING_CHANNEL,mUuid));
    mUi.label_set_mode_title->setText(lanmanager->getText(L_RTCF_SETTING_MODE,mUuid));
    mUi.label_set_secure->setText(lanmanager->getText(L_RTCF_SETTING_SECURITY,mUuid));

    mUi.label_set_pwd_title->setText(lanmanager->getText(L_RTCF_INFO_PASSWOED,mUuid));
    mUi.but_apply->setText(lanmanager->getText(L_RTCF_SETTING_APPLY,mUuid));
    mUi.but_set_cancel->setText(lanmanager->getText(L_RTCF_CANCEL,mUuid));



    mUi.label_info_ssid_title->setText(lanmanager->getText(L_RTCF_SETTING_SSID,mUuid));
    mUi.label_info_chanel_title->setText(lanmanager->getText(L_RTCF_INFO_CHANNEL,mUuid));
    mUi.label_info_wpa_title->setText(lanmanager->getText(L_RTCF_INFO_CHOICE,mUuid));
    mUi.label_info_pwd_title->setText(lanmanager->getText(L_RTCF_INFO_PASSWOED,mUuid));
    mUi.but_modify->setText(lanmanager->getText(L_RTCF_INFO_MODIFY,mUuid));
    mUi.but_export->setText(lanmanager->getText(L_RTCF_INFO_EXPORT,mUuid));
    mUi.but_info_cancel->setText(lanmanager->getText(L_RTCF_INFO_LOGINOUT,mUuid));
    mUi.but_info_refresh->setText(lanmanager->getText(L_RTCF_INFO_REFRESH,mUuid));
    mUi.but_set_refresh->setText(lanmanager->getText(L_RTCF_INFO_REFRESH,mUuid));

    if(mErrStrIdx >=0)
    {
        QString mode =ERROR_MODE;
        QString err=lanmanager->getText(mErrStrIdx,mUuid);
        mUi.label_info_error->setText(mode.arg(err));
        mUi.label_set_error->setText(mode.arg(err));
    }
    mUi.label_note->setText(lanmanager->getText(L_RTCF_SETTING_NOTE,mUuid));
    translateRouteType();
}
//Load WirelessExport
void QGenieRCPageWireless::loadSubPlugins()
{
    Load_WirelessExport();
    Load_TrafficMeter();
#ifdef ENABLE_READYSHARE
    Load_ReadyShare();
#endif
    Load_RouterUpdate();
}

void QGenieRCPageWireless::Load_WirelessExport()
{
    GeniePlugin_RouterConfiguration *inf=GeniePlugin_RouterConfiguration::GetSingletonPtr();
    ASSERT(inf);
    QWidget *w=inf->loadSubPluginWidget(GeniePlugin_RouterConfiguration::SP_EXPORT);
    if(w)
    {
        m_wirelessexport_widget=w;
        mUi.stackedWidget->addWidget(w);
        mUi.but_export->show();
    }
}
//Load GuessAccess

//Load TrafficMeter
void QGenieRCPageWireless::Load_TrafficMeter()
{
    GeniePlugin_RouterConfiguration *inf=GeniePlugin_RouterConfiguration::GetSingletonPtr();
    ASSERT(inf);
    QWidget *w=inf->loadSubPluginWidget(GeniePlugin_RouterConfiguration::SP_TRAFFICMETER);
    if(w)
    {
        m_TrafficMeter_widget=w;
        mUi.tabWidget->addTab(w,QString());
    }
}

#ifdef ENABLE_READYSHARE
//Load ReadyShare from local not from plugin
void QGenieRCPageWireless::Load_ReadyShare()
{

    GeniePlugin_RouterConfiguration *inf=GeniePlugin_RouterConfiguration::GetSingletonPtr();
    ASSERT(inf);
    QWidget *w = inf->loadSubPluginWidget(GeniePlugin_RouterConfiguration::SP_READYSHARE);
    if(w)
    {
        m_ReadyShare_Widget = w;
        mUi.tabWidget->insertTab(1,w,QString());
    }
}

#endif

//Load RouterUpdate
void QGenieRCPageWireless::Load_RouterUpdate()
{
    GeniePlugin_RouterConfiguration *inf=GeniePlugin_RouterConfiguration::GetSingletonPtr();
    ASSERT(inf);
    QWidget *w=inf->loadSubPluginWidget(GeniePlugin_RouterConfiguration::SP_ROUTERUPDATE);
    if(w)
    {
        m_RouterUpdate_widget = w;
        mUi.tabWidget->addTab(w,QString());
    }

}

void QGenieRCPageWireless::startSettingMode()
{
    mUi.stackedWidget->setCurrentIndex(1);

    mUi.edt_set_ssid->setText(mSsid);
    mUi.edt_set_ssid->setCursorPosition(0);
    mUi.edt_pwd->setText(mPwd);

    mUi.cob_channel->setCurrentIndex(mChannelIndex);
    mUi.cob_mode->setCurrentIndex(mModeIndex);

    if(mCurrentWpaRdt)
    {
        mCurrentWpaRdt->setChecked(true);
    }

    if(mCurrentWpaRdt==mUi.rbt_none)
    {
        mUi.edt_pwd->hide();
        mUi.label_set_pwd_title->hide();
        mUi.label_note->hide();
    }
    else
    {
        mUi.edt_pwd->show();
        mUi.label_set_pwd_title->show();
        mUi.label_note->show();
    }
    mUi.but_apply->setDisabled(true);
    setBackgroundImg();

}



void QGenieRCPageWireless::endSettingMode()
{
    mUi.stackedWidget->setCurrentIndex(0);
    setBackgroundImg();
}

void QGenieRCPageWireless::SetExportPage()
{
    GeniePlugin_RouterConfiguration *inf=GeniePlugin_RouterConfiguration::GetSingletonPtr();
    ASSERT(inf);
    IPluginInf *pi=inf->mPlugins[GeniePlugin_RouterConfiguration::SP_EXPORT];
    if(m_wirelessexport_widget && pi)
    {
        pi->command(INFO_CMD_ROUTECONFIG_EXPORT_AUTHOR,mAuthentation,mEncryption);
        pi->command(INTO_CMD_ROUTECONFIG_EXPORT_PASSWORD,mSsid,mPwd);
        mUi.stackedWidget->setCurrentWidget(m_wirelessexport_widget);
        setBackgroundImg();
    }
    update();
}



void QGenieRCPageWireless::SetWirelessPage()
{
    mUi.tabWidget->setCurrentIndex(0);

}

void QGenieRCPageWireless::showError(int lanidx)
{
    mErrStrIdx=lanidx;
    QString mode=ERROR_MODE;
    ILanguageManager *lanmanager=MainWindow::sHandler->getLanguageManager();
    QString msg=lanmanager->getText(lanidx,QUuid(UUID_OF_THIS));
    mUi.label_set_error->setText(mode.arg(msg));
    mUi.label_info_error->setText(mode.arg(msg));
}

void QGenieRCPageWireless::hideError()
{
    mUi.label_info_error->setText("");
    mUi.label_set_error->setText("");
    mErrStrIdx=-1;
}

void QGenieRCPageWireless::parseSoapResponse(int cmd,QtSoapMessage &message)
{

    QGenieRCSoapProxy *soapproxy=QGenieRCSoapProxy::GetSingletonPtr();
    ASSERT(soapproxy);
    bool bOk=(!message.isFault() && message.returnResponseCode().toString().toInt() == 0);
    QGenieRCPageGuessAccess *gacc=QGenieRCPageGuessAccess::GetSingletonPtr();
    ASSERT(gacc);
    const QtSoapType &res = message.returnValue();
    if(cmd == ESC_ROUTE_INFO)
    {
        if(bOk)
        {
            mSsid=res["NewSSID"].value().toString();
            mUi.label_info_ssid->setText(mSsid);
            mUi.edt_set_ssid->setText(mSsid);
            mUi.edt_set_ssid->setCursorPosition(0);

            QString channel=res["NewChannel"].value().toString();

            IInfoManager *infom=MainWindow::sHandler->getInfoManager();
            ASSERT(infom);
            infom->notifyPluginMessage(INFO_NOTIFY_CHANNEL_CHANGE,channel.toLower());


            if(channel==""){
                channel="Auto";
            }
            mUi.label_info_channel->setText(channel);
            bool btemp;
            int itemp=channel.toInt(&btemp,10);
            if(!btemp ||itemp>=mUi.cob_channel->count())
            {
                itemp=0;
            }
            mUi.cob_channel->setCurrentIndex(itemp);
            mChannelIndex=itemp;

            QString mac = res["NewWLANMACAddress"].value().toString();
            notifyMacChange(mac);
            gacc->notifyMac(mac);

            QString geniesecurity = res["NewBasicEncryptionModes"].value().toString();

#ifndef NO_EXPORT_INFO
            if(geniesecurity=="WEP" || geniesecurity =="None")
            {
                mEncryption=geniesecurity;
                if(mEncryption=="None")
                {
                    mEncryption="none";
                }
                QString weptype=res["NewWEPAuthType"].value().toString().toLower();
                mAuthentation=(geniesecurity=="WEP")?(weptype):"open";
            }
            else
            {
                mAuthentation=geniesecurity;
                mAuthentation.replace("-","");
                if((mAuthentation!="WPAPSK") &&(mAuthentation!="WPA2PSK"))
                {
                    mAuthentation="WPA2PSK";
                }

#if DEFAULT_SELECTION ==2
                mEncryption=(geniesecurity =="WPA2-PSK")?"AES":"TKIP";
#elif DEFAULT_SELECTION ==1
                mEncryption=(geniesecurity =="WPA-PSK/WPA2-PSK")?"TKIP":"AES";
#endif

            }
#endif
            if(geniesecurity =="None")
            {
                mUi.label_info_wpa->setText("None");
                mUi.label_info_pwd->setText("");

                mUi.label_set_pwd_title->hide();
                mUi.edt_pwd->hide();
                mUi.label_note->hide();
                mCurrentWpaRdt=mUi.rbt_none;
                stopUiRefresh();
            }
            else
            {

                mUi.label_set_pwd_title->show();
                mUi.edt_pwd->show();
                mUi.label_note->show();

#if DEFAULT_SELECTION ==2
                QString stemp=(geniesecurity =="WPA2-PSK")?"WPA2-PSK[AES]":"WPA-PSK/WPA2-PSK";
                mUi.label_info_wpa->setText(stemp);
                mCurrentWpaRdt=(geniesecurity =="WPA2-PSK")?mUi.rbt_wpa2:mUi.rbt_all;
#elif DEFAULT_SELECTION ==1
                QString stemp=(geniesecurity =="WPA-PSK/WPA2-PSK")?"WPA-PSK/WPA2-PSK":"WPA2-PSK[AES]";
                mUi.label_info_wpa->setText(stemp);
                mCurrentWpaRdt=(geniesecurity =="WPA-PSK/WPA2-PSK")?mUi.rbt_all:mUi.rbt_wpa2;

#endif
                QGenieRCLogic logic;
                if(logic.isLogin())
                {
                    soapproxy->command(ESC_INFO_SECURITY_KEY);
                }
#ifdef HIDE_PWD_WHEN_REFRESH
                mUi.label_info_pwd->show();
                mUi.label_info_pwd_title->show();
#endif
            }
            mPwd="";
            mCurrentWpaRdt->setChecked(true);

            QString geniemode =res["NewWirelessMode"].value().toString();
#ifndef NOT_SET_MODE
            int wirelessmode =geniemode.left(geniemode.length()-4).toInt();
            const int cModParas[]={54,145,300};
            int tempidx=0;
            for(int i=0;i<(sizeof(cModParas)/sizeof(cModParas[0]));i++)
            {
                if(wirelessmode>cModParas[i])
                {
                    tempidx++;
                }
            }
            mUi.cob_mode->setCurrentIndex(tempidx);
            mModeIndex=tempidx;
#else
            mModeStr=geniemode;
#endif

            mRegion=res["NewRegion"].value().toString();


            infom->notifyPluginMessage(INFO_NOTIFY_REGION_CHANGE,mRegion);
            if(mRegion.toUpper()=="UNITED STATES" ||mRegion.toUpper()=="US")
            {
                mUi.cob_channel->setMaxCount(12);
            }
            else{
                for(int i=mUi.cob_channel->count();i<14;i++)
                {
                    mUi.cob_channel->addItem(QString::number(i));
                }
            }

        }else{
            whenSoapError(cmd,"");
        }

    }// if(cmd == ESC_ROUTE_INFO)
    else if(cmd == ESC_INFO_SECURITY_KEY)
    {
        if(bOk)
        {
            mPwd =res["NewWPAPassphrase"].value().toString();
            mUi.label_info_pwd->setText(mPwd);
            mUi.edt_pwd->setText(mPwd);

            GeniePlugin_RouterConfiguration *inf=GeniePlugin_RouterConfiguration::GetSingletonPtr();
            ASSERT(inf);
            stopUiRefresh(true);
        }
        else
        {
            whenSoapError(cmd,"");
        }
    }

}



void QGenieRCPageWireless::whenSoapError(int cmd,const QString& reson )
{
    if(cmd == ESC_ROUTE_INFO)
    {
        showError(L_RTCT_MSG_SOAPERROR);
        stopUiRefresh(false);

        notifyMacChange("");

        QGenieRCPageGuessAccess *gacc=QGenieRCPageGuessAccess::GetSingletonPtr();
        ASSERT(gacc);
        gacc->notifyMac("");


    }// if(cmd == ESC_ROUTE_INFO)
    else if(cmd == ESC_INFO_SECURITY_KEY)
    {
        showError(L_RTCT_MSG_SOAPERROR);
        stopUiRefresh(false);
    }

}

void QGenieRCPageWireless::translateRouteType()
{
    IInfoManager *infom=MainWindow::sHandler->getInfoManager();
    ILanguageManager *lanm=MainWindow::sHandler->getLanguageManager();

    QString route=infom->getInfo(INFO_CMD_ROUTE_TYPE_IMMEDIATE);
    QString firmware=infom->getInfo(INFO_CMD_FIRMWARE_IMMEDIATE);
    QString route_head=lanm->getText(L_RTCF_ROUTETYPE,UUID_OF_THIS);
    QString firmware_head=lanm->getText(L_RTCF_FIRMWARE_VERSION,UUID_OF_THIS);
    mUi.rc_text_route->setText(route_head+" "+route);
    mUi.rc_text_firmware->setText(firmware_head+" "+firmware);

}

void  QGenieRCPageWireless::tryNotifyRouteChanges()
{
    if(mNotifySsid !="")
    {
        MainWindow::sHandler->getLogManager()->writeLog("notify route changes",QUuid(UUID_OF_THIS));
        MainWindow::sHandler->getInfoManager()->notifyPluginMessage(INFO_NOTIFY_SSID_CHANGE0,mNotifyWpa,mNotifyRZ);
        MainWindow::sHandler->getInfoManager()->notifyPluginMessage(INFO_NOTIFY_SSID_CHANGE,mNotifySsid,mNotifyPwd);
    }
    mNotifyWpa="";
    mNotifyRZ="";
    mNotifySsid="";
    mNotifyPwd="";
}

void QGenieRCPageWireless::slot_EnableApply()
{
    qDebug()<<"QGenieRCPageWireless::slot_EnableApply()";
    bool disabled=((mUi.edt_set_ssid->text()=="")||
                   ((mUi.edt_pwd->text().length()<8 )&&!mUi.rbt_none->isChecked())
                   );
    if(!mTimer.isActive())
    {

        mUi.but_apply->setDisabled(disabled);
    }
}


//Change tabWidget Page
void QGenieRCPageWireless::slot_TabIdxChanged(int index)
{
    qDebug()<<"Wireless::slot_TabIdxChanged(int index)";
    if(m_TrafficMeter_widget)
    {
        int trafficmeteridx=mUi.tabWidget->indexOf(m_TrafficMeter_widget);
        if(trafficmeteridx==index)
        {
            GeniePlugin_RouterConfiguration *inf=GeniePlugin_RouterConfiguration::GetSingletonPtr();
            ASSERT(inf);
            IPluginInf *pi=inf->mPlugins[GeniePlugin_RouterConfiguration::SP_TRAFFICMETER];


            ASSERT(pi);
            pi->notifyRaise();
            ADD_CLICK("R10");
        }
    }
    if(m_RouterUpdate_widget)
    {
        int rupdateidx=mUi.tabWidget->indexOf(m_RouterUpdate_widget);
        if(rupdateidx==index)
        {
            ADD_CLICK("R13");
        }
    }

#ifdef ENABLE_READYSHARE
    if(m_ReadyShare_Widget)
    {
        int readyshare_idx = mUi.tabWidget->indexOf(m_ReadyShare_Widget);

        if(readyshare_idx == index)
        {
            GeniePlugin_RouterConfiguration *inf = GeniePlugin_RouterConfiguration::GetSingletonPtr();
            ASSERT(inf);
            IPluginInf *pi = inf->mPlugins[GeniePlugin_RouterConfiguration::SP_READYSHARE];
            ASSERT(pi);
            pi->notifyRaise();
        }
    }
#endif


    if(index==GACC_IDX)
    {
        mUi.gacc->refreshWhenChangeIdxJustAfterLogin();
        ADD_CLICK("R6");
    }
    if(index==WIRELESS_IDX)
    {
        ADD_CLICK("R5");
    }
    setBackgroundImg();

#ifdef ENABLE_READYSHARE
    IUiManager *uimanager = MainWindow::sHandler->getUiManager();
    Q_ASSERT(uimanager);

    if(index == mUi.tabWidget->indexOf(m_ReadyShare_Widget))
    {
        uimanager->setLeftButtonChecked(GeniePlugin_RouterConfiguration::sSinglePtr,1);
    }
    else
    {
        uimanager->setLeftButtonChecked(GeniePlugin_RouterConfiguration::sSinglePtr,0);
    }
#endif

}


void QGenieRCPageWireless::notifyMacChange(QString mac)
{

}

void QGenieRCPageWireless::setBackgroundImg()
{
    QString img="";
    IUiManager* uimanager = MainWindow::sHandler->getUiManager();
    QGenieRCPageGuessAccess *gacc=QGenieRCPageGuessAccess::GetSingletonPtr();
    ASSERT(gacc);
    MainWindow *main=MainWindow::GetSingletonPtr();
    ASSERT(main);
    QGenieRCLogic logic;
    if(logic.isLogin())
    {
#ifdef ENABLE_READYSHARE
        if(
                (mUi.tabWidget->currentIndex()==0&&mUi.stackedWidget->currentIndex()==1  )
                ||(mUi.tabWidget->currentIndex()==2 && gacc->ui->stackedWidget->currentIndex()==1)
                ||(mUi.tabWidget->currentIndex()==3 && mTrafficMetterImg !="")
                )
#else
            if(
                    (mUi.tabWidget->currentIndex()==0&&mUi.stackedWidget->currentIndex()==1  )
                    ||(mUi.tabWidget->currentIndex()==1 && gacc->ui->stackedWidget->currentIndex()==1)
                    ||(mUi.tabWidget->currentIndex()==2 && mTrafficMetterImg !="")
                    )

#endif
        {
            img =":/bjtu.png";
        }
    }
    qDebug()<<"setBackgroundImg"<<img;
    uimanager->setBackGroundImg(mUuid,img);
}

void QGenieRCPageWireless::clearText()
{
    mUi.label_info_ssid->setText("N/A");
    mUi.label_info_channel->setText("N/A");
    mUi.label_info_wpa->setText("N/A");
    mUi.label_info_pwd->setText("N/A");

    mUi.edt_pwd->setText("N/A");
    mUi.edt_set_ssid->setText("N/A");
}

void QGenieRCPageWireless::startUiRefresh()
{
#ifdef LOCK_WHEN_REFRESHING
    MainWindow::GetSingleton().setDisabled(true);
#endif

    qDebug()<<"startUiRefresh";
#ifdef USE_TEXT_WAIT
    showError(L_RTCT_MSG_PLEASEWAIT);
#else
    hideError();
#endif



    mUi.pi_info->startAnimation();
    mUi.but_modify->setDisabled(true);
    mUi.but_export->setDisabled(true);
    mUi.but_info_refresh->setDisabled(true);

    mUi.pi_set->startAnimation();
    mUi.but_apply->setDisabled(true);
    //    mUi.but_set_cancel->setDisabled(true);
    mUi.but_set_refresh->setDisabled(true);
    mUi.cob_channel->setDisabled(true);
    mUi.cob_mode->setDisabled(true);

    if(!mTimer.isActive())
    {
        mTimer.setSingleShot(true);
        mTimer.start(REFRESH_WAITING_MSECS);
    }
#ifdef HIDE_PWD_WHEN_REFRESH
    mUi.label_info_pwd->hide();
    mUi.label_info_pwd_title->hide();
#endif
}



//Set Pi to Stop
void QGenieRCPageWireless::stopUiRefresh(bool bOk)
{
#ifdef LOCK_WHEN_REFRESHING
    MainWindow::GetSingleton().setDisabled(false);
#endif
    if(bOk)
    {
        hideError();
    }


    mUi.pi_info->stopAnimation();
    mUi.but_modify->setDisabled(!bOk);
    mUi.but_export->setDisabled(!bOk);
    mUi.but_info_refresh->setDisabled(false);


    mUi.pi_set->stopAnimation();
    mUi.but_apply->setDisabled(!bOk);
    mUi.cob_channel->setDisabled(!bOk);
    mUi.cob_mode->setDisabled(!bOk);
    //        mUi.but_set_cancel->setDisabled(!bOk);
    mUi.but_set_refresh->setDisabled(false);

    mTimer.stop();
    mRefreshDelay.stop();
    //    mPendingRequest=0;

    MainWindow *main=MainWindow::GetSingletonPtr();
    ASSERT(main);

    QGenieRCLogic logic;
    logic.processPendingCmd();

    logic.tryApplyEnd();

}

void QGenieRCPageWireless::slot_Timeout()
{
    stopUiRefresh(false);
    showError(L_RTCF_MSG_TIMEOUT);
    QGenieRCSoapProxy *soapproxy=QGenieRCSoapProxy::GetSingletonPtr();
    ASSERT(soapproxy);

    soapproxy->ignoreRequest(ESC_ROUTE_INFO);

    soapproxy->ignoreRequest(ESC_APPLY_CHANGES_NOSECURE);
    soapproxy->ignoreRequest(ESC_APPLY_CHANGES_WITHSECURE);
}
//void QGenieRCPageWireless::slot_NotifyTimeout()
//{
//    tryNotifyRouteChanges();
//}

void QGenieRCPageWireless::slot_refresh()
{
    refresh();
}

void QGenieRCPageWireless::slot_refreshDelayTimeout()
{
    qDebug()<<"QGenieRCPageWireless::slot_refreshDelayTimeout()";
    refresh(0);
}

void QGenieRCPageWireless::refresh(int delay)
{
    qDebug()<<"slot_refresh()";
    //    mMacGeted=false;
    if(delay>0)
    {
        clearText();
        startUiRefresh();
        mRefreshDelay.setSingleShot(true);
        mRefreshDelay.start(delay);
    }
    else{
        clearText();
        startUiRefresh();
        QGenieRCSoapProxy *soapproxy=QGenieRCSoapProxy::GetSingletonPtr();
        ASSERT(soapproxy);
        soapproxy->command(ESC_ROUTE_INFO);
    }
}

void QGenieRCPageWireless::on_but_apply_clicked()
{
    QGenieMsgDialog *dialog=QGenieMsgDialog::GetSingletonPtr();
    ASSERT(dialog);
    dialog->showMsg(L_RTCF_TAB_TITLE,L_RTCF_MSG_NOTIFYCUT);
    if(dialog->mResult != ER_OK)
    {
        // stopUiRefresh(true);
        return;
    }
    //    mPendingRequest=1;
    WHEN_APPLY(MainWindow::AT_WIRELESS);

    QMap<QString,QString> paras;
    paras.clear();

    QString ssid = mUi.edt_set_ssid->text().trimmed();
    QString region;
    QString channel;
    QString mode,option,pwd;

    region = mRegion;
    int channelIdx=mUi.cob_channel->currentIndex();
    if(channelIdx ==0)
    {
        channel ="Auto";
    }
    else
    {
        char sbuffer[64];
        sprintf(sbuffer,"%02d",channelIdx);
        channel=sbuffer;
        qDebug()<<"when sumbmit channel is"<<channel;
    }
#ifndef NOT_SET_MODE
    const char *cModStrs[]={"50Mbps","130Mbps","270Mbps"};
    int modIdx=mUi.cob_mode->currentIndex();
    ASSERT(modIdx >=0 && modIdx <sizeof(cModStrs)/sizeof(cModStrs[0]));
    mode=cModStrs[modIdx];
#else
    mode=mModeStr;
#endif
    paras.insert("NewSSID",ssid);
    paras.insert("NewRegion",region);
    paras.insert("NewChannel",channel);
    paras.insert("NewWirelessMode",mode);


    int cmd=ESC_APPLY_CHANGES_NOSECURE;
    if(mUi.rbt_all->isChecked() || mUi.rbt_wpa2->isChecked())
    {
        cmd=ESC_APPLY_CHANGES_WITHSECURE;
        option =(mUi.rbt_all->isChecked())?"WPA-PSK/WPA2-PSK":"WPA2-PSK";


        pwd = mUi.edt_pwd->text().trimmed();
        paras.insert("NewWPAEncryptionModes",option);
    }
    paras.insert("NewWPAPassphrase",pwd);

    QGenieRCSoapProxy *soapproxy=QGenieRCSoapProxy::GetSingletonPtr();
    ASSERT(soapproxy);
    soapproxy->command(cmd,&paras);
    mSsid=ssid;
    mPwd=pwd;

    setNotifyParas();

}

void QGenieRCPageWireless::setNotifyParas()
{
    QString notifyWPA="open";
    QString notifyRZ="none";
    if(mUi.rbt_all->isChecked() || mUi.rbt_wpa2->isChecked())
    {
        notifyWPA="WPA2PSK";
        notifyRZ="AES";
    }
    setNotifyParas(notifyWPA,notifyRZ,mSsid,mPwd);
}

void QGenieRCPageWireless::on_but_export_clicked()
{
    qDebug()<<"QGenieRCPageWireless::on_but_export_clicked()";
    SetExportPage();
}

void QGenieRCPageWireless::on_but_modify_clicked()
{
    qDebug()<<"QGenieRCPageWireless::on_but_modify_clicked()";
    startSettingMode();
}

void QGenieRCPageWireless::on_but_info_cancel_clicked()
{
    qDebug()<<"QGenieRCPageWireless::on_but_info_cancel_clicked()";
    QGenieRCLogic logic;
    logic.logout();

}


void QGenieRCPageWireless::on_but_set_cancel_clicked()
{
    qDebug()<<"QGenieRCPageWireless::on_but_set_cancel_clicked()";
    mUi.stackedWidget->setCurrentIndex(0);
    setBackgroundImg();
}





