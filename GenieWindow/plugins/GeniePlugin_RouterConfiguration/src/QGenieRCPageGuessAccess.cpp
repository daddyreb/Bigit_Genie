
#include "ui_QGenieRCPageGuessAccess.h"
//#include "QThreadIsNetgear.h"
#include "LanguageRouterConfiguration.h"
#include "ILanguageManager.h"
#include "IManagerHandler.h"
#include "mainwindow.h"
#include "common.h"
#include "QGenieMsgDialog.h"
#include "plugin_ui_def.h"
#include <QTimer>
#include <QVariant>
#include "ILogManager.h"
#include "QGenieRCSoapProxy.h"
#include "QGenieRCPageGuessAccess.h"
#include "QGenieRCLogic.h"
#include "IClickManager.h"

#define DEFAULT_SELECTION 1
#define GACC_DISABLE_TRYAGAIN_TIMER (60*60*1000)


QGenieRCPageGuessAccess::QGenieRCPageGuessAccess(QWidget *parent) :
    QWidget(parent)
    ,ui(new Ui::QGenieRCPageGuessAccess)
    ,mErrorIdx(-1)
    ,mEnableIdx(-1)
    ,mEnabledSaved(NULL)
    ,mWpaCur(NULL)
    ,mJustLogin(false)
{

    ui->setupUi(this);
    connect(&mRefreshTimer,SIGNAL(timeout()),this,SLOT(slot_timeout()));
    connect(ui->rdt_disable,SIGNAL(clicked()),this,SLOT(slot_SetApplyButtonState()));
    connect(ui->rdt_enable,SIGNAL(clicked()),this,SLOT(slot_SetApplyButtonState()));

    connect(ui->rdt_wpa2,SIGNAL(clicked()),this,SLOT(slot_SetApplyButtonState()));
    connect(ui->rdt_wpa_all,SIGNAL(clicked()),this,SLOT(slot_SetApplyButtonState()));
    connect(ui->rdt_wpa_none,SIGNAL(clicked()),this,SLOT(slot_SetApplyButtonState()));
    connect(ui->cob_time,SIGNAL(currentIndexChanged(int)),this,SLOT(slot_SetApplyButtonState()));

    connect(ui->edt_set_pwd,SIGNAL(textChanged(QString)),this,SLOT(slot_SetApplyButtonState()));
    connect(ui->edt_set_ssid,SIGNAL(textChanged(QString)),this,SLOT(slot_SetApplyButtonState()));

    connect(&mTimerToDisable,SIGNAL(timeout()),this,SLOT(slot_timeoutDisable()));

    connect(ui->but_info_gacc_refresh,SIGNAL(clicked()),this,SLOT(slot_refresh()));
    connect(ui->but_set_gacc_refresh,SIGNAL(clicked()),this,SLOT(slot_refresh()));
//    connect(&mApplyTimeLine,SIGNAL(frameChanged(int)),this,SLOT(slot_TimeLine(int)));

    connect(&mRefreshDelay,SIGNAL(timeout()),this,SLOT(slot_refreshDelayTimeout()));
    connect(&mApplyDelay,SIGNAL(timeout()),this,SLOT(slot_applyDelayTimeout()));
    SET_STYLE_PROPERTY(FLEX_BUTTON,ui->but_gacc_apply);
    SET_STYLE_PROPERTY(FLEX_BUTTON,ui->but_info_gacc_cancel);
    SET_STYLE_PROPERTY(FLEX_BUTTON,ui->but_set_gacc_cancel);
    SET_STYLE_PROPERTY(FLEX_BUTTON,ui->but_set_gacc_refresh);
    SET_STYLE_PROPERTY(FLEX_BUTTON,ui->but_info_gacc_refresh);
    SET_STYLE_PROPERTY(FLEX_BUTTON,ui->but_gacc_modify);

    ui->cob_time->setCurrentIndex(3);

}

QGenieRCPageGuessAccess::~QGenieRCPageGuessAccess()
{
    mRefreshTimer.stop();
    mTimerToDisable.stop();
    mApplyDelay.stop();
//    mApplyTimeLine.stop();
    delete ui;
}

void QGenieRCPageGuessAccess::refresh(int delay)
{
    if(delay>0)
    {
        clearText();
        startUiRefresh();
        mRefreshDelay.setSingleShot(true);
        mRefreshDelay.start(delay);
    }
    else
    {
        clearText();
        startUiRefresh();
        QGenieRCSoapProxy *soapproxy=QGenieRCSoapProxy::GetSingletonPtr();
        ASSERT(soapproxy);
        soapproxy->command(ESC_GUESTACCESS_ISENABLE);
    }
}

void QGenieRCPageGuessAccess::startUiRefresh()
{
#ifdef LOCK_WHEN_REFRESHING
    MainWindow::GetSingleton().setDisabled(true);
#endif
#ifdef USE_TEXT_WAIT
        showError(L_RTCT_MSG_PLEASEWAIT);
#else
        hideError();
#endif
        if(!mRefreshTimer.isActive())
        {
            mRefreshTimer.setSingleShot(true);
            mRefreshTimer.start(REFRESH_WAITING_MSECS);
        }
        ui->w_pi->startAnimation();
        ui->w_pi0->startAnimation();

        ui->edt_set_ssid->setDisabled(true);
        ui->rdt_disable->setDisabled(true);
        ui->rdt_enable->setDisabled(true);
        ui->rdt_wpa2->setDisabled(true);
        ui->rdt_wpa_all->setDisabled(true);
        ui->rdt_wpa_none->setDisabled(true);
        ui->but_gacc_apply->setDisabled(true);
        ui->but_info_gacc_refresh->setDisabled(true);
        ui->but_set_gacc_refresh->setDisabled(true);
        ui->cob_time->setDisabled(true);
        ui->edt_set_pwd->setDisabled(true);
        ui->but_gacc_modify->setDisabled(true);
#ifdef HIDE_PWD_WHEN_REFRESH
        ui->label_info_pwd_title->hide();
        ui->label_info_pwd->hide();
#endif


//        clearText();

}

void QGenieRCPageGuessAccess::stopUiRefresh(bool haveError)
{
#ifdef LOCK_WHEN_REFRESHING
    MainWindow::GetSingleton().setDisabled(false);
#endif

    mRefreshTimer.stop();
    mRefreshDelay.stop();
    ui->w_pi0->stopAnimation();
    ui->w_pi->stopAnimation();
    if(!haveError)
    {
        hideError();
        ui->rdt_disable->setDisabled(false);
        ui->rdt_enable->setDisabled(false);

//        if(mEnabledSaved==ui->rdt_enable && ui->rdt_enable->isChecked())
        {
            ui->edt_set_ssid->setDisabled(false);
            ui->rdt_wpa2->setDisabled(false);
            ui->rdt_wpa_all->setDisabled(false);
            ui->rdt_wpa_none->setDisabled(false);
//            ui->but_gacc_apply->setDisabled(false);
            ui->cob_time->setDisabled(false);
            ui->edt_set_pwd->setDisabled(false);

        }
        ui->but_gacc_modify->setDisabled(false);


    }

    ui->but_info_gacc_refresh->setDisabled(false);
    ui->but_set_gacc_refresh->setDisabled(false);

    MainWindow *main=MainWindow::GetSingletonPtr();
    ASSERT(main);

    QGenieRCLogic logic;
    logic.processPendingCmd();

    logic.tryApplyEnd();

//    mPendingRequest=0;
}

void QGenieRCPageGuessAccess::showError(int lanidx )
{
    mErrorIdx=lanidx;
    ILanguageManager *lanmanager=MainWindow::sHandler->getLanguageManager();
    ASSERT(lanmanager);
    QString mode=ERROR_MODE;
    QString message =mode.arg(lanmanager->getText(lanidx,QUuid(UUID_OF_THIS)));

    ui->label_set_err->setText(message);
    ui->label_info_err->setText(message);
}

void QGenieRCPageGuessAccess::hideError()
{
    ui->label_set_err->setText("");
    ui->label_info_err->setText("");
    mErrorIdx=-1;
}


void QGenieRCPageGuessAccess::clearText()
{
     ui->edt_set_ssid->setText("");
     ui->edt_set_pwd->setText("");
     ui->label_info_pwd->setText("N/A");
     ui->label_info_ssid->setText("N/A");
     ui->label_info_enable->setText("N/A");
     ui->label_info_wpa->setText("N/A");
}

void QGenieRCPageGuessAccess::whenLogin()
{
//    mPendingRequest=0;
    clearText();
    mJustLogin=true;
    mMac="";
    ui->stackedWidget->setCurrentIndex(0);
}

void QGenieRCPageGuessAccess::apply(int delay)
{
    if(delay>0)
    {
        mApplyDelay.stop();
        mApplyDelay.setSingleShot(true);
        mApplyDelay.start(delay);
    }
    else
    {
      WHEN_APPLY(MainWindow::AT_GACC_ENABLE_AND_SETTING);
        setGuestAccessPara();
    }
}

void QGenieRCPageGuessAccess::setGuestAccessPara()
{
    QMap<QString,QString> paras;
    const QString stemp=ui->edt_set_pwd->text();
    QRadioButton *cRdbts[]={ui->rdt_wpa_none,ui->rdt_wpa2,ui->rdt_wpa_all};
    QString cModes[]={"None","WPA2-PSK","WPA-PSK"};
    QString cKeys[]={"",stemp,stemp};
    QString ssid=ui->edt_set_ssid->text();
    bool useEnabled2=(ui->rdt_enable->isChecked()
                      && (mEnabledSaved!=ui->rdt_enable));

    if(useEnabled2)
    {
            paras.insert("NewGuestAccessEnabled","1");
    }
    for(int i=0;i<3;i++)
    {
        if(cRdbts[i]->isChecked())
        {
            mWpaCur=cRdbts[i];

            paras.insert("NewSSID",ssid);
            paras.insert("NewSecurityMode",cModes[i]);
            paras.insert("NewKey1",cKeys[i]);
            paras.insert("NewKey2","0");
            paras.insert("NewKey3","0");
            paras.insert("NewKey4","0");
            break;
        }
    }
    QGenieRCSoapProxy *soapproxy=QGenieRCSoapProxy::GetSingletonPtr();
    ASSERT(soapproxy);
    if(useEnabled2)
    {
        soapproxy->command(ESC_GUESTACCESS_SETINFO2,&paras);
    }
    else
    {
        soapproxy->command(ESC_GUESTACCESS_SETINFO,&paras);
    }

    //save date message
    QDateTime curdate=QDateTime::currentDateTime();
    const int cAddHours[]={0,1,5,10,0,0};
    const int cAddDays[]={0,0,0,0,1,7};
    int idx=ui->cob_time->currentIndex();
    ASSERT(0<=idx && idx <6);
    if(0<idx && idx <6) //idx==0 means always
    {
        ISettings *isettings=MainWindow::sHandler->getSettings();

        isettings->setValue(ISETTING_GACC_MAC,mMac);


        QDateTime qdt=curdate.addSecs(60*60*cAddHours[idx]).addDays(cAddDays[idx]);
#ifdef CHEET_CODE
        if(QDir::current().exists("cheetgacc.txt"))
        {
            int cheet;
            QFile f("cheetgacc.txt");
            if(f.open(QIODevice::ReadOnly))
            {
              QString s=f.readLine()  ;
              bool btemp;
              int itemp=s.toInt(&btemp);
              if(btemp)
              {
                  cheet=itemp;
              }
            }
            f.close();
            qdt=curdate.addSecs(cheet*60);
        }
#endif

        isettings->setValue(ISETTING_GACC_TIME,qdt.toString(DATE_FORMAT));
        int secs=curdate.secsTo(qdt);


        mTimerToDisable.stop();
        mTimerToDisable.setSingleShot(true);
//        secs=160;
        qDebug()<<"disable gacc within "<<secs<<"secs";
        mTimerToDisable.start(secs*1000);

    }
    else //if(idx == 0)
    {
        mTimerToDisable.stop();
    }
}

void QGenieRCPageGuessAccess::setUiEnableState(bool enable)
{
    ILanguageManager *lanmanager=MainWindow::sHandler->getLanguageManager();
    if(!enable)
    {
        ui->rdt_disable->setChecked(true);
        ui->frame_setting->hide();
        ui->label_info_wpa->hide();
        ui->label_info_wpa_title->hide();
#ifdef HIDE_GACC_INFO_WHEN_DISABLED
        ui->label_info_pwd->hide();
        ui->label_info_pwd_title->hide();
        ui->label_info_ssid->hide();
        ui->label_info_ssid_title->hide();
#else
        ui->label_info_pwd->setText("N/A");
        ui->label_info_ssid->setText("N/A");
#endif

        mEnabledSaved=ui->rdt_disable;
//                soapproxy->command(ESC_GUESTACCESS_INFO);
        mEnableIdx=L_GACC_NOTENABLED;
        ui->label_info_enable->setText(lanmanager->getText(mEnableIdx,QUuid(UUID_OF_THIS)));

    }
    else
    {
        ui->rdt_enable->setChecked(true);
        ui->frame_setting->show();
        ui->label_info_wpa->show();
        ui->label_info_wpa_title->show();
#ifdef HIDE_GACC_INFO_WHEN_DISABLED
        ui->label_info_pwd->show();
        ui->label_info_pwd_title->show();
        ui->label_info_ssid->show();
        ui->label_info_ssid_title->show();
#endif
        mEnabledSaved=ui->rdt_enable;

        mEnableIdx=L_GACC_ENABLED;
        ui->label_info_enable->setText(lanmanager->getText(mEnableIdx,QUuid(UUID_OF_THIS)));

      }

}

void QGenieRCPageGuessAccess::parseSoapResponse(int cmd,QtSoapMessage &message)
{
    qDebug()<<"QGenieRCPageGuessAccess::parseSoapResponse"<<cmd<<message.isFault()<<message.returnResponseCode().toString();
    bool bOk=(!message.isFault() &&(message.returnResponseCode().toString().toInt() == 0));
    QGenieRCSoapProxy *soapproxy=QGenieRCSoapProxy::GetSingletonPtr();
    ASSERT(soapproxy);
    ILanguageManager *lanmanager=MainWindow::sHandler->getLanguageManager();
    QGenieRCLogic logic;
    bool haveerror=!bOk;
    QString errReson;

    static bool sEnabled=false;
    if(cmd == ESC_GUESTACCESS_ISENABLE)
    {
        const QtSoapType &res = message.returnValue();

        if(bOk)
        {
            int guestenable = res["NewGuestAccessEnabled"].value().toInt();
            qDebug()<<"NewGuestAccessEnabled"<<guestenable;
            sEnabled=(guestenable==1);

            //added at 2011.7.18,number 2 means not guest access supported
            haveerror=(guestenable==2);
            if(!haveerror)
            {
                soapproxy->command(ESC_GUESTACCESS_INFO);
            }
        }
    }
    else if(cmd == ESC_GUESTACCESS_INFO)
    {
        if(bOk)
        {
            const QtSoapType &res = message.returnValue();
            QString ssid=res["NewSSID"].value().toString();
            QString mode=res["NewSecurityMode"].value().toString();
            QString key=res["NewKey"].value().toString();
            ui->edt_set_ssid->setText(ssid);
            ui->edt_set_ssid->setCursorPosition(0);
            ui->label_info_ssid->setText(ssid);
            ui->edt_set_pwd->setText(key);
            ui->label_info_pwd->setText(key);
            ui->label_info_wpa->setText(mode);
            mSsidSaved=ssid;
            mKeySaved=key;

#if DEFAULT_SELECTION ==2
            QString modstr=(mode=="None")?"None":((mode == "WPA2-PSK")?"WPA2-PSK[AES]":"WPA-PSK/WPA2-PSK");
            mWpaCur=(mode=="None")?(ui->rdt_wpa_none):((mode == "WPA2-PSK")?(ui->rdt_wpa2):(ui->rdt_wpa_all));

#elif DEFAULT_SELECTION ==1
            QString modstr=(mode=="None")?"None":((mode == "WPA-PSK")?"WPA-PSK/WPA2-PSK":"WPA2-PSK[AES]");
            mWpaCur=(mode=="None")?(ui->rdt_wpa_none):((mode == "WPA-PSK")?(ui->rdt_wpa_all):(ui->rdt_wpa2));

#endif

            ui->label_info_wpa->setText(modstr);


            mWpaCur->setChecked(true);

            if(mWpaCur ==ui->rdt_wpa_none)
            {
                ui->label_info_pwd->setText("");

                ui->edt_set_pwd->hide();
                ui->label_set_pwd_title->hide();
                ui->label_note->hide();
               // ui->label_info_pwd_title->hide();
              //  ui->label_info_pwd->hide();
            }
            else{
                ui->edt_set_pwd->show();
                ui->label_set_pwd_title->show();
                ui->label_note->show();
              //  ui->label_info_pwd_title->show();
              //  ui->label_info_pwd->show();
#ifdef HIDE_PWD_WHEN_REFRESH
        ui->label_info_pwd_title->show();
        ui->label_info_pwd->show();
#endif
            }
            setUiEnableState(sEnabled);

            stopUiRefresh(false);
        }
    }
    else if(cmd ==ESC_GUESTACCESS_SETENABLE)
    {
        qDebug()<<"soap request returned "<<"ESC_GUESTACCESS_SETENABLE";

    }
    else if(cmd == ESC_GUESTACCESS_SETINFO2)
    {
        if(!message.isFault() && message.returnResponseCode().toString().toInt() == 401)
        {
            MainWindow::sHandler->getLogManager()
                    ->writeLog("disable gacc timer because soap 401",QUuid(UUID_OF_THIS));
            mTimerToDisable.stop();
        }

    }
    else if(cmd ==ESC_GUESTACCESS_ISENABLE_BEFORE_DISABLE_IT)
    {
        const QtSoapType &res = message.returnValue();

        if(bOk)
        {

//            int guestenable = res["NewGuestAccessEnabled"].value().toInt();
              int guestenable = (res["NewGuestAccessEnabled"].value().toInt()==1);

            qDebug()<<"NewGuestAccessEnabled"<<guestenable;
            if(guestenable)
            {
                askWhenGaccTimeoutThenSoapReturn();
            }
            else
            {
                logic.writeLog("gacc timeout ,and gacc already disabled when soap returned");
               // mTimerToDisable.stop();
            }
        }
    }
    else if(cmd==ESC_GUESTACCESS_ISMACOK_BEFORE_DISABLE_IT)
    {
        if(bOk)
        {
            const QtSoapType &res = message.returnValue();
            QString mac = res["NewWLANMACAddress"].value().toString();
            ISettings *isetings=MainWindow::sHandler->getSettings();
            ASSERT(isetings);
            QString iniMac=isetings->value(ISETTING_GACC_MAC,"").toString();
            if(iniMac==mac)
            {
                logic.writeLog("gacc timeout,mac is the same ,then check if it already disabled");
                QGenieRCSoapProxy *soapproxy=QGenieRCSoapProxy::GetSingletonPtr();
                Q_ASSERT(soapproxy);
                soapproxy->command(ESC_GUESTACCESS_ISENABLE_BEFORE_DISABLE_IT);
            }
            else{
                errReson=QString("can not disable gacc because mac not the same(%1!=%2),so try again 1hour later").arg(iniMac,mac);
                haveerror=true;
            }

        }

    }
    if(haveerror)
    {
        whenSoapError(cmd,errReson);
    }
}

void QGenieRCPageGuessAccess::whenSoapError(int cmd,const QString& reson )
{
    QGenieRCLogic logic;

    if(cmd == ESC_GUESTACCESS_ISENABLE)
    {
        showError(L_RTCT_MSG_GACC_NOTSUPPORTED);
        stopUiRefresh(true);
    }
    else if(cmd == ESC_GUESTACCESS_INFO)
    {

        showError(L_RTCT_MSG_SOAPERROR);
        stopUiRefresh(true);
    }

    else if(cmd ==ESC_GUESTACCESS_ISENABLE_BEFORE_DISABLE_IT)
    {
        mTimerToDisable.setSingleShot(true);
        mTimerToDisable.start(GACC_DISABLE_TRYAGAIN_TIMER);

    }
    else if(cmd==ESC_GUESTACCESS_ISMACOK_BEFORE_DISABLE_IT)
    {
        mTimerToDisable.setSingleShot(true);
        mTimerToDisable.start(GACC_DISABLE_TRYAGAIN_TIMER);
    }
    if(reson!="")
    {
        logic.writeLog(reson);
    }
}

void QGenieRCPageGuessAccess::askWhenGaccTimeoutThenSoapReturn()
{

    MainWindow *main=MainWindow::GetSingletonPtr();
    //ASSERT(main);
    QGenieMsgDialog *msgdialog=QGenieMsgDialog::GetSingletonPtr();
    ASSERT(msgdialog);

    QGenieRCLogic logic;
    if(logic.isRefreshing() || msgdialog->isVisible())
    {
        logic.mPendingCmd |= SLP_DISABLE_GACC;
        return;
    }


    ADD_CLICK("R9");
    msgdialog->showMsg(L_GACC_BIGBT,L_RTCT_MSG_GACC);
    if(msgdialog->mResult==ER_OK)
    {
        if(logic.isLogin())
        {
            QGenieRCPageWireless *wireless=QGenieRCPageWireless::GetSingletonPtr();
            ASSERT(wireless);
            wireless->mUi.tabWidget->setCurrentWidget(this);
            WHEN_APPLY(MainWindow::AT_GACC_TIMEOUT_DISABLE);
        }

        QMap<QString,QString> paras;
        paras.clear();
        paras.insert("NewGuestAccessEnabled","0");
//        mPendingRequest=PR_ENABLE;



        QGenieRCSoapProxy *soapproxy=QGenieRCSoapProxy::GetSingletonPtr();
        ASSERT(soapproxy);
        soapproxy->command(ESC_GUESTACCESS_SETENABLE,&paras);
        ISettings *isettings=MainWindow::sHandler->getSettings();
        //clear settings
        isettings->setValue(ISETTING_GACC_MAC,"");
        isettings->setValue(ISETTING_GACC_TIME,"");
    }
    else
    {
        logic.writeLog("restart gacc diable timer when user click cancel");
        mTimerToDisable.setSingleShot(true);
        mTimerToDisable.start(GACC_DISABLE_TRYAGAIN_TIMER);
    }
}

//

void QGenieRCPageGuessAccess::notifyMac(QString mac)
{
#if 1
//
//           st0                         st1
//           getmac first time 0         have pending request 1
//           mac is the same   1          do not have         0
//           mac not the same  2
//           can not get mac   3
    const int F_NOTHING=0;
    const int F_ASSERT=1<<0;
    const int F_REFRESH=1<<1;
    const int F_LOGOUT=1<<2;
//    const int F_CLEARPR_T=1<<3;//clear pending request
//    const int F_CLEARPR_F=1<<4;
    const int F_RESETTIMER=1<<5;
    const int F_NOTIFY=1<<6;

    const int cFunctionMap[]={
      F_NOTHING|F_RESETTIMER|F_NOTIFY,                //00
      F_NOTHING|F_RESETTIMER,                //10
      F_LOGOUT|F_NOTIFY,                 //20
      F_NOTHING                //30

    };
    int st0,st1;
    st0=(mac=="")?3:((mMac=="")?0:((mac==mMac)?1:2));
    if(mac!="") mMac=mac;
    int idx=st0;
    ASSERT(idx <(sizeof(cFunctionMap)/sizeof(cFunctionMap[0])));
    int funcs=cFunctionMap[idx];
    if((funcs &F_LOGOUT )!=0)
    {
		QGenieRCLogic logic;
		logic.logout(true);
    }
    if((funcs & F_ASSERT )!=0)
    {
        ILogManager *logm=MainWindow::sHandler->getLogManager();
        QString log=QString("assert in QGenieRCPageGuessAccess::notifyMac %1").arg(QString::number(idx));
        logm->writeLog(log,QUuid(UUID_OF_THIS));
    }
    if(funcs &F_REFRESH)
    {
        refresh();
    }
    if(funcs & F_NOTIFY)
    {
        IInfoManager *infom=MainWindow::sHandler->getInfoManager();
        ASSERT(infom);
        infom->notifyPluginMessage(INFO_NOTIFY_MAC_CHANGE,mac);
    }
    if(funcs & F_RESETTIMER)
    {
        ISettings *isetings=MainWindow::sHandler->getSettings();
        ASSERT(isetings);
        QString iniMac=isetings->value(ISETTING_GACC_MAC,"").toString();
        if(iniMac == mMac)
        {
            QString timestr=isetings->value(ISETTING_GACC_TIME,"").toString();
            QDateTime dti=QDateTime::fromString(timestr,DATE_FORMAT);
            if(dti.isValid())
            {
                QGenieRCSoapProxy *soapproxy=QGenieRCSoapProxy::GetSingletonPtr();
                Q_ASSERT(soapproxy);
                soapproxy->ignoreRequest(ESC_GUESTACCESS_ISENABLE_BEFORE_DISABLE_IT);
                soapproxy->ignoreRequest(ESC_GUESTACCESS_ISMACOK_BEFORE_DISABLE_IT);

                QDateTime cti=QDateTime::currentDateTime();
                if(cti.addSecs(60) >= dti)
                {
                    slot_timeoutDisable();
                }
                else
                {
                    mTimerToDisable.stop();
                    mTimerToDisable.setSingleShot(true);
                    int msecs=cti.secsTo(dti)*1000;
                    mTimerToDisable.start(msecs);

                    ILogManager *logmanager=MainWindow::sHandler->getLogManager();
                    logmanager->writeLog(QString("gaccdisable at %1").arg(dti.toString(DATE_FORMAT)),
                                         QUuid(UUID_OF_THIS));
                }
            }
        }
//        else
//        {
//            mTimerToDisable.stop();
//        }
    }
#endif
}

void QGenieRCPageGuessAccess::refreshWhenChangeIdxJustAfterLogin()
{
    if(mJustLogin)
    {
        refresh();
        mJustLogin=false;
    }
}


void QGenieRCPageGuessAccess::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    ILanguageManager *lanmanager=MainWindow::sHandler->getLanguageManager();
    QUuid uuid=QUuid(UUID_OF_THIS);
    if(e->type() ==QEvent::LanguageChange)
    {
        if(mErrorIdx >=0)
        {
            QString mode =ERROR_MODE;
            QString err=lanmanager->getText(mErrorIdx,uuid);
            ui->label_set_err->setText(mode.arg(err));
            ui->label_info_err->setText(mode.arg(err));
        }
        ui->label_info_enable_title->setText(lanmanager->getText(L_GACC_ENABLE_TITLE,uuid));
        if(mEnableIdx >=0)
        {
            ui->label_info_enable->setText(lanmanager->getText(mEnableIdx,uuid));
        }
        else
        {
            ui->label_info_enable->setText("N/A");
        }
        ui->rdt_disable->setText(lanmanager->getText(L_GACC_DISABLEGUEST,uuid));
        ui->rdt_enable->setText(lanmanager->getText(L_GACC_ENABLEGUEST,uuid));
        ui->label_set_ssid_title->setText(lanmanager->getText(L_GACC_GUESTWIRELESS_SSID,uuid));
        ui->label_info_ssid_title->setText(lanmanager->getText(L_GACC_GUESTWIRELESS_SSID,uuid));
        ui->label_info_wpa_title->setText(lanmanager->getText(L_RTCF_SETTING_SECURITY,uuid));
         ui->label_set_wpa_title->setText(lanmanager->getText(L_RTCF_SETTING_SECURITY,uuid));
         ui->label_set_time_title->setText(lanmanager->getText(L_GACC_WORKTIME,uuid));

        ui->but_info_gacc_refresh->setText(lanmanager->getText(L_RTCF_INFO_REFRESH,uuid));
        ui->but_set_gacc_refresh->setText(lanmanager->getText(L_RTCF_INFO_REFRESH,uuid));
        ui->but_info_gacc_cancel->setText(lanmanager->getText(L_RTCF_INFO_LOGINOUT,uuid));
        ui->but_set_gacc_cancel->setText(lanmanager->getText(L_RTCF_CANCEL,uuid));
        ui->but_gacc_apply->setText(lanmanager->getText(L_RTCF_SETTING_APPLY,uuid));
        ui->but_gacc_modify->setText(lanmanager->getText(L_RTCF_INFO_MODIFY,uuid));

        ui->label_info_pwd_title->setText(lanmanager->getText(L_RTCF_INFO_PASSWOED,uuid));
        ui->label_set_pwd_title->setText(lanmanager->getText(L_RTCF_INFO_PASSWOED,uuid));
        ui->label_note->setText(lanmanager->getText(L_RTCF_SETTING_NOTE,uuid));

        int tempidx=ui->cob_time->currentIndex();
        ui->cob_time->clear();
        QStringList templist;
        templist << lanmanager->getText(L_GACC_ALWAYS,uuid);
        templist << lanmanager->getText(L_GACC_1HOUR,uuid);
        templist << lanmanager->getText(L_GACC_5HOUR,uuid);
        templist << lanmanager->getText(L_GACC_10HOUR,uuid);
        templist << lanmanager->getText(L_GACC_1DAY,uuid);
        templist << lanmanager->getText(L_GACC_1WEEK,uuid);
        ui->cob_time->insertItems(0,templist);
        ui->cob_time->setCurrentIndex(tempidx);


    }
}

void QGenieRCPageGuessAccess::slot_timeout()
{
    qDebug()<<"QGenieRCPageGuessAccess::slot_timeout()";

    stopUiRefresh(true);
    showError(L_RTCF_MSG_TIMEOUT);


    QGenieRCSoapProxy *soapproxy=QGenieRCSoapProxy::GetSingletonPtr();
    ASSERT(soapproxy);
    soapproxy->ignoreRequest(ESC_GUESTACCESS_ISENABLE);
    soapproxy->ignoreRequest(ESC_GUESTACCESS_INFO);
    soapproxy->ignoreRequest(ESC_GUESTACCESS_SETENABLE);
    soapproxy->ignoreRequest(ESC_GUESTACCESS_SETINFO2);
    soapproxy->ignoreRequest(ESC_GUESTACCESS_SETINFO);
}

void QGenieRCPageGuessAccess::slot_refresh()
{
    refresh();
}
void QGenieRCPageGuessAccess::on_but_gacc_apply_clicked()
{
    qDebug()<<"QGenieRCPageGuessAccess::on_but_apply_clicked()";
    QGenieRCSoapProxy *soapproxy=QGenieRCSoapProxy::GetSingletonPtr();
    ASSERT(soapproxy);
    QGenieMsgDialog *msgdialog=QGenieMsgDialog::GetSingletonPtr();
    ASSERT(msgdialog);
    QGenieRCPageWireless *wireless=QGenieRCPageWireless::GetSingletonPtr();

    msgdialog->showMsg(L_GACC_BIGBT,L_RTCF_MSG_NOTIFYCUT);
     if(msgdialog->mResult==ER_OK)
    {
        if(
            ui->rdt_disable->isChecked()&&
            mEnabledSaved!=ui->rdt_disable
            )
        {
            QMap<QString,QString> paras;
            paras.clear();
            paras.insert("NewGuestAccessEnabled","0");
            WHEN_APPLY(MainWindow::AT_GACC_DISABLE);
            soapproxy->command(ESC_GUESTACCESS_SETENABLE,&paras);
            ADD_CLICK("R8");
        }
        else
        {
            apply(0);
            ADD_CLICK("R7");
        }
//        wireless->setNotifyParas();
     }
}

void QGenieRCPageGuessAccess::slot_SetApplyButtonState()
{
    qDebug()<<"QGenieRCPageGuessAccess::slot_SetApplyButtonState()";
#if 0
    QRadioButton *cRdts[]={ui->rdt_wpa2,ui->rdt_wpa_none,ui->rdt_wpa_all};
    for(int i=0;i<sizeof(cRdts)/sizeof(cRdts[0]);i++)
    {
        if(cRdts[i]->isChecked())
        {
            mWpaCur=cRdts[i];
            break;
        }
    }
    QString ssidCur=ui->edt_set_ssid->text();
    QString keyCur=ui->edt_set_pwd->text();
    bool disable=( mWpaCur==mWpaSaved
        && ssidCur == mSsidSaved
        && keyCur==mKeySaved);
#endif
    bool disabled;
    bool valueok=!((ui->edt_set_ssid->text()=="")||((ui->edt_set_pwd->text().length()<8))&&(!ui->rdt_wpa_none->isChecked()));
    if((ui->rdt_enable->isChecked() && mEnabledSaved!=ui->rdt_enable)
            ||(ui->rdt_disable->isChecked()&& mEnabledSaved!=ui->rdt_disable))
    {
        if(ui->rdt_disable->isChecked())
        {
            disabled=false;
        }
        else
        {
            disabled=!valueok;
        }
    }
    else if(ui->rdt_disable->isChecked())
    {
        disabled=true;
    }
    else
    {
        disabled=!valueok;
    }
    if(!mRefreshTimer.isActive())
    {
        ui->but_gacc_apply->setDisabled(disabled);
    }
}

void QGenieRCPageGuessAccess::on_rdt_enable_clicked(bool ck)
{

}

void QGenieRCPageGuessAccess::on_rdt_disable_clicked(bool ck)
{


}
void QGenieRCPageGuessAccess::startSettingMode()
{
    mEnabledSaved->setChecked(true);
    if(mEnabledSaved==ui->rdt_disable)
    {
        ui->frame_setting->hide();
    }
    else
    {
        ui->frame_setting->show();
    }

    ui->edt_set_ssid->setText(mSsidSaved);
    ui->edt_set_ssid->setCursorPosition(0);
    ui->edt_set_pwd->setText(mKeySaved);
    mWpaCur->setChecked(true);
    if(mWpaCur==ui->rdt_wpa_none)
    {
        ui->edt_set_pwd->hide();
        ui->label_note->hide();
        ui->label_set_pwd_title->hide();
    }
    else
    {
        ui->edt_set_pwd->show();
        ui->label_note->show();
        ui->label_set_pwd_title->show();
    }


    ui->stackedWidget->setCurrentIndex(1);
    ui->but_gacc_apply->setDisabled(true);
    QGenieRCPageWireless *wireless=QGenieRCPageWireless::GetSingletonPtr();
    ASSERT(wireless);
    wireless->setBackgroundImg();
}

void QGenieRCPageGuessAccess::endSettingMode()
{
    ui->stackedWidget->setCurrentIndex(0);
    QGenieRCPageWireless *wireless=QGenieRCPageWireless::GetSingletonPtr();
    ASSERT(wireless);
    wireless->setBackgroundImg();
}


void QGenieRCPageGuessAccess::on_but_gacc_modify_clicked()
{
    startSettingMode();
}

void QGenieRCPageGuessAccess::on_but_set_gacc_cancel_clicked()
{
    endSettingMode();
}

void QGenieRCPageGuessAccess::on_but_info_gacc_cancel_clicked()
{
	QGenieRCLogic logic;
	logic.logout(true);
}


void QGenieRCPageGuessAccess::slot_timeoutDisable()
{
    qDebug()<<"QGenieRCPageGuessAccess::slot_timeoutDisable()";

    QGenieRCSoapProxy *soapproxy=QGenieRCSoapProxy::GetSingletonPtr();
    Q_ASSERT(soapproxy);
    soapproxy->command(ESC_GUESTACCESS_ISMACOK_BEFORE_DISABLE_IT);

}

void QGenieRCPageGuessAccess::slot_refreshDelayTimeout()
{
    qDebug()<<"QGenieRCPageGuessAccess::slot_refreshDelayTimeout()";
    refresh();
}
void QGenieRCPageGuessAccess::slot_applyDelayTimeout()
{
    qDebug()<<"QGenieRCPageGuessAccess::slot_refreshDelayTimeout()";
    apply(0);
}

