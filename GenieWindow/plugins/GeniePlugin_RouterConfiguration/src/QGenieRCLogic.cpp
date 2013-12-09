#include "QGenieRCPageLogin.h"
#include "mainwindow.h"
#include "QGenieRCPageGuessAccess.h"
#include "QGenieRCLogic.h"
#include "QGenieRCPageWireless.h"
#include "GeniePluginTest.h"
#include "info_defs.h"
#include "ILogManager.h"
#include "LanguageRouterConfiguration.h"
#include "ui_QGenieRCPageGuessAccess.h"
#include "QGeniePlugin_Remote_Choosed_ReadyShare.h"
#include "QGeniePlugin_Remote_Unregister_ReadyShare.h"

quint32 QGenieRCLogic::mPendingCmd=0;
bool QGenieRCLogic::sInApplyProcess=false;
QString QGenieRCLogic::sRouteType;
#define TEXT_EDIT_MAX_LENGTH 32

QGenieRCLogic::QGenieRCLogic()
    :mInitOk(false)
{
    mMain =MainWindow::GetSingletonPtr();
    mGacc=QGenieRCPageGuessAccess::GetSingletonPtr();
    mWireless=QGenieRCPageWireless::GetSingletonPtr();
    mLogin=QGenieRCPageLogin::GetSingletonPtr();
    mInf=GeniePlugin_RouterConfiguration::GetSingletonPtr();
    mSoapProxy =QGenieRCSoapProxy::GetSingletonPtr();
    if(mMain && mGacc && mWireless && mLogin && mInf && mSoapProxy)
    {
        mInitOk=true;
    }
}

void QGenieRCLogic::parseCmd(int cmd,const QString &para0,const QString &para1)
 {
    /////////////////
    if(!mInitOk){return;}
    if(mInf->mPlugins[GeniePlugin_RouterConfiguration::SP_ROUTERUPDATE])
    {
        mInf->mPlugins[GeniePlugin_RouterConfiguration::SP_ROUTERUPDATE]->command(cmd,para0,para1);
    }

    if(cmd ==COMMAND_GETPWD)
    {
        if(!isLogin()&&mLogin->mUi.edt_pwd->text()!="")
        {
            mLogin->tryStartLogin(SLP_GETPWD,SLP_ALL&~SLP_GETPWD);
        }
        else
        {
            if(isRefreshing())
            {
                mPendingCmd |=SLP_GETPWD;
            }
            else
            {
                mWireless->SetWirelessPage();
                mWireless->endSettingMode();
            }
        }
    }
    else if(cmd == COMMAND_CHANGECHANNEL)
    {
        if(!isLogin()&&mLogin->mUi.edt_pwd->text()!="")
        {
            mLogin->tryStartLogin(SLP_CHANGE_CHANNEL,SLP_ALL&~SLP_CHANGE_CHANNEL);
        }
        else
        {
            if(isRefreshing())
            {
                mPendingCmd |= SLP_CHANGE_CHANNEL;
            }
            else
            {
                mWireless->SetWirelessPage();
                mWireless->startSettingMode();
                mWireless->mUi.cob_channel->setFocus();
            }
        }
    }
    else if(cmd == COMMAND_NOTIFY_FUPDATE)
    {
        if(!isLogin()&&mLogin->mUi.edt_pwd->text()!="")
        {
            mLogin->tryStartLogin(SLP_FIRMWARE_UPDATE,SLP_ALL&~SLP_FIRMWARE_UPDATE);
        }
        else
        {
            if(isRefreshing())
            {
                mPendingCmd |= SLP_FIRMWARE_UPDATE;
            }
            else
            {
#ifdef ENABLE_READYSHARE
                mWireless->mUi.tabWidget->setCurrentIndex(4);
#else
                mWireless->mUi.tabWidget->setCurrentIndex(3);
#endif
            }
        }
    }
    else if(cmd ==COMMAND_TURNTOGACC)
    {
        if(!isLogin()&&mLogin->mUi.edt_pwd->text()!="")
        {
            mLogin->tryStartLogin(SLP_TURNTOGACC,SLP_ALL&~SLP_TURNTOGACC);
        }
        else
        {
            if(isRefreshing())
            {
                mPendingCmd |= SLP_TURNTOGACC;
            }
            else
            {
#ifdef ENABLE_READYSHARE
                mWireless->mUi.tabWidget->setCurrentIndex(2);
#else
                mWireless->mUi.tabWidget->setCurrentIndex(1);
#endif
#ifdef CHANGE_PAGE_WHEN_TURNTO_GACC
                mGacc->ui->stackedWidget->setCurrentIndex(0);
#endif
            }
        }
    }
#ifdef ENABLE_READYSHARE
    else if(cmd == COMMAND_TURNTOREADYSHARE)
    {
        if(!isLogin()&&mLogin->mUi.edt_pwd->text()!="")
        {
            mLogin->tryStartLogin(SLP_TURNTOREADYSHARE,SLP_ALL&~SLP_TURNTOREADYSHARE);
        }
        else
        {
            if(isRefreshing())
            {
                mPendingCmd |= SLP_TURNTOREADYSHARE;
            }
            else
            {
                mWireless->mUi.tabWidget->setCurrentWidget(mWireless->m_ReadyShare_Widget);
            }
        }
    }
#endif
    else if(cmd ==COMMAND_NOTIFYROUTETYPE)
    {
      //  sRouteType="";
        IInfoManager *infom=MainWindow::sHandler->getInfoManager();
        ILanguageManager *lanm=MainWindow::sHandler->getLanguageManager();


        writeLog("parseCmd COMMAND_NOTIFYROUTETYPE");
        if(mPendingCmd& SLP_LOGIN)
        {
            processPendingCmd(SLP_LOGIN);
        }


        if(sRouteType!=para0)
        {
            if(isLogin()&& !QGenieRCLogic::sInApplyProcess )
            {
                if(QGenieRCLogic::sRouteType!="")
                {
                    if(isRefreshing())
                    {
                        mPendingCmd |= SLP_LOGOUT;
                    }
                    else
                    {
                        logout();
                    }
                }
            }


            infom->notifyPluginMessage(INFO_NOTIFY_REGION_CHANGE,"");
            infom->notifyPluginMessage(INFO_NOTIFY_CHANNEL_CHANGE,"");

            sRouteType=para0;
        }

        if(!QGenieRCLogic::sInApplyProcess)
        {
            mWireless->translateRouteType();
        }
    }
    else if(cmd == COMMAND_NOTIFYGATEWAY)
    {
        static QString savedgateway="";
        if(savedgateway!=para0)
        {
            if(isLogin() &&(savedgateway !=""))
            {
                if(isRefreshing())
                {
                    mPendingCmd |= SLP_LOGOUT;
                }
                else
                {
                    logout();
                }
            }
            savedgateway=para0;
            ILogManager *logm=MainWindow::sHandler->getLogManager();
            Q_ASSERT(logm);
            QString log="route type or gateway changed to"+para0;
            logm->writeLog(log,QUuid(UUID_OF_THIS));

            IInfoManager *infom=MainWindow::sHandler->getInfoManager();
            infom->notifyPluginMessage(INFO_NOTIFY_REGION_CHANGE,"");
            infom->notifyPluginMessage(INFO_NOTIFY_CHANNEL_CHANGE,"");
        }
    }
}
static QString sRouteTypeWhenApplyStarted;

void QGenieRCLogic::applyStart()
{
    writeLog("QGenieRCLogic::applyStart()");
    QGenieRCLogic::sInApplyProcess=true;
    sRouteTypeWhenApplyStarted=QGenieRCLogic::sRouteType;
}

void QGenieRCLogic::tryApplyEnd()
{
    if(QGenieRCLogic::sInApplyProcess)
    {
        mWireless->translateRouteType();
        QGenieRCLogic::sInApplyProcess=false;
        writeLog("QGenieRCLogic::tryApplyEnd()");
        if(QGenieRCLogic::sRouteType != sRouteTypeWhenApplyStarted
                &&QGenieRCLogic::sRouteType!="")
        {
            if(isRefreshing())
            {
                mPendingCmd |= SLP_LOGOUT;
            }
            else
            {
                logout();
            }
        }
#ifdef SUBMIT_FINISH_AFTER_REBOOT
        else{
            processPendingCmd(SLP_SUBMIT_SOAP_FINISH);
        }
#endif
    }
}

QString QGenieRCLogic::getDisableTimerStatus()
{
    ISettings *isetings=MainWindow::sHandler->getSettings();
    QString timestr=isetings->value(ISETTING_GACC_TIME,"").toString();

    if(!mInitOk){return "class init error";}
    if(!mGacc->mTimerToDisable.isActive() )
    {
        return "not active";
    }
    int interval=mGacc->mTimerToDisable.interval()/1000;
    QString ret=QString("interval %1:%2:%3").arg(interval/(60*60)).arg((interval%(60*60))/60).arg(interval%60);
    ret=ret+"--"+timestr;

    return ret;
}



void QGenieRCLogic::parseCmd(int cmd,const int &para)
{
    if(!mInitOk){return;}

    if(mInf->mPlugins[GeniePlugin_RouterConfiguration::SP_ROUTERUPDATE])
    {
        mInf->mPlugins[GeniePlugin_RouterConfiguration::SP_ROUTERUPDATE]->command(cmd,para);
    }
    if(cmd == COMMAND_NOTIFYNETSTATE)
    {
        qDebug()<<"GeniePlugin_RouterConfiguration::command"<<cmd;
        int cFlags[]={1,0,1};
        ASSERT(para>=0 && para<sizeof(cFlags)/cFlags[0]);
        static int sFlag=-1;
        bool bStateChanged=(sFlag!=cFlags[para]);
        sFlag=cFlags[para];
        if(sFlag==0)
        {
            mWireless->tryNotifyRouteChanges();
        }

    }
#if 0
    if(cmd == COMMAND_NOTIFY_SUBSTATE)
    {
        qDebug()<<"command COMMAND_NOTIFY_SUBSTATE";
        if(para==EBSS_BUT)
        {
            MainWindow::sHandler->getUiManager()->setBigButtonSubState(mInf,EBSS_REFRESH);
        }
    }
#endif
}

bool  QGenieRCLogic::processPendingCmd(const quint32 &flag)
{
    if(!mInitOk){return false;}

    bool processed=false;

    if(flag & mPendingCmd & SLP_LOGOUT)
    {
        mPendingCmd &= ~SLP_LOGOUT;
        logout();
        //mPendingCmd become None,after logout()
        processed=true;
    }
    if(flag & mPendingCmd & SLP_TURNTOGACC)
    {
        Q_ASSERT(isLogin());
        mPendingCmd &=(~SLP_TURNTOGACC);
#ifdef ENABLE_READYSHARE
        mWireless->mUi.tabWidget->setCurrentIndex(2);
#else
        mWireless->mUi.tabWidget->setCurrentIndex(1);
#endif
#ifdef CHANGE_PAGE_WHEN_TURNTO_GACC
        mGacc->ui->stackedWidget->setCurrentIndex(0);
#endif
        processed=true;
    }

#ifdef ENABLE_READYSHARE
    if(flag & mPendingCmd & SLP_TURNTOREADYSHARE)
    {
        Q_ASSERT(isLogin());
        mPendingCmd &= (~SLP_TURNTOREADYSHARE);
        mWireless->mUi.tabWidget->setCurrentWidget(mWireless->m_ReadyShare_Widget);
        processed = true;
    }
#endif

    if(flag & mPendingCmd &SLP_GETPWD)
    {
        mPendingCmd &=~SLP_GETPWD;
        mWireless->SetWirelessPage();
        mWireless->endSettingMode();
        processed=true;
    }

    if(flag & mPendingCmd &SLP_FIRMWARE_UPDATE)
    {
        Q_ASSERT(isLogin());
        mPendingCmd &=~SLP_FIRMWARE_UPDATE;


        if(mInf->mPlugins[GeniePlugin_RouterConfiguration::SP_ROUTERUPDATE])
        {
#ifdef ENABLE_READYSHARE
            mWireless->mUi.tabWidget->setCurrentIndex(4);
#else
            mWireless->mUi.tabWidget->setCurrentIndex(3);
#endif
            mInf->mPlugins[GeniePlugin_RouterConfiguration::SP_ROUTERUPDATE]->command(COMMAND_NOTIFY_FUPDATE,"","");
        }

        processed=true;
    }


    if(flag & mPendingCmd & SLP_CHANGE_CHANNEL)
    {
        Q_ASSERT(isLogin());
        mPendingCmd &=(~SLP_CHANGE_CHANNEL);
        mWireless->SetWirelessPage();
        mWireless->startSettingMode();
        mWireless->mUi.cob_channel->setFocus();
        processed=true;
    }

    if(flag & mPendingCmd & SLP_DISABLE_GACC)
    {
        mPendingCmd &= ~SLP_DISABLE_GACC;
        mGacc->askWhenGaccTimeoutThenSoapReturn();
        processed=true;
    }
    if(flag & mPendingCmd & SLP_LOGIN)
    {
        mPendingCmd &= ~SLP_LOGIN;
        IInfoManager *infom=MainWindow::sHandler->getInfoManager();
        Q_ASSERT(infom);
        if(!isLogin())
        {
            if(infom->getInfo(INFO_CMD_ROUTE_TYPE_IMMEDIATE)!="")
            {
                mLogin->GoToLogin();
            }
            else
            {
                mLogin->showError(L_RTCF_MSG_NOTNETGEAR);
                mLogin->stopUiWaitState(false);
            }
        }
        processed=true;
    }
#ifdef SUBMIT_FINISH_AFTER_REBOOT
    if(flag & mPendingCmd & SLP_SUBMIT_SOAP_FINISH)
    {
        mPendingCmd &= ~SLP_SUBMIT_SOAP_FINISH;
        QGenieRCSoapProxy *soapproxy=QGenieRCSoapProxy::GetSingletonPtr();
        ASSERT(soapproxy);
        if(isLogin())
        {
            writeLog("submit soap finish command");
            soapproxy->submitFinishCommandAfterReboot();
        }

        processed=true;
    }
#endif

    return processed;

}

void QGenieRCLogic::logout(bool toframehome)
{
    if(!mInitOk){return;}

    writeLog("QGenieLogic::logout");
    mPendingCmd=SLP_NONE;
    QGenieMsgDialog *msg=QGenieMsgDialog::GetSingletonPtr();
    if(msg)
    {
        msg->hide();
    }


    mLogin->stopUiWaitState(true);
    mMain->mStack->setCurrentWidget(mLogin);
    mWireless->SetWirelessPage();

    //to stop timer
    mWireless->stopUiRefresh(true);

    mGacc->stopUiRefresh(true);

    mSoapProxy->ignoreAllRequest();


    if(mInf->mPlugins[GeniePlugin_RouterConfiguration::SP_TRAFFICMETER])
    {
        mInf->mPlugins[GeniePlugin_RouterConfiguration::SP_TRAFFICMETER]->command(INFO_CMD_ROUTECONFIG_LOGOUT,0);
    }

    if(mInf->mPlugins[GeniePlugin_RouterConfiguration::SP_ROUTERUPDATE])
    {
        mInf->mPlugins[GeniePlugin_RouterConfiguration::SP_ROUTERUPDATE]->command(INFO_CMD_ROUTECONFIG_LOGOUT,0);
    }
    mWireless->setBackgroundImg();
    mMain->mApplayType=MainWindow::AT_NONE;
    mMain->update();

    if(toframehome)
    {
        IUiManager *uimanager=MainWindow::sHandler->getUiManager();
        ASSERT(uimanager);
        uimanager->changeStackIdx(0);
    }

    mLogin->ReloadPassword();

}

void QGenieRCLogic::writeLog(const QString &str)
{
     if(!mInitOk){return;}
     ILogManager *logm=MainWindow::sHandler->getLogManager();
     ASSERT(logm);
     logm->writeLog(str,UUID_OF_THIS);
}

void QGenieRCLogic::limitTextLength()
{
     if(!mInitOk){return;}
     mLogin->mUi.edt_pwd->setMaxLength(TEXT_EDIT_MAX_LENGTH);
     mLogin->mUi.edt_user->setMaxLength(TEXT_EDIT_MAX_LENGTH);
     mWireless->mUi.edt_pwd->setMaxLength(TEXT_EDIT_MAX_LENGTH);
     mWireless->mUi.edt_set_ssid->setMaxLength(TEXT_EDIT_MAX_LENGTH);
     mGacc->ui->edt_set_pwd->setMaxLength(TEXT_EDIT_MAX_LENGTH);
     mGacc->ui->edt_set_ssid->setMaxLength(TEXT_EDIT_MAX_LENGTH);


}

void QGenieRCLogic::loginProcessEnd(bool ok,int reson_str,bool forcestop)
{
    //from loginok
    if(!mInitOk){return;}
    if(ok)
    {
#ifdef ENABLE_READYSHARE
        QGeniePlugin_Remote_Choosed_ReadyShare *rshare=
                QGeniePlugin_Remote_Choosed_ReadyShare::GetSingletonPtr();
        QGeniePlugin_Remote_Unregister_ReadyShare *rshare1=
                QGeniePlugin_Remote_Unregister_ReadyShare::GetSingletonPtr();

        Q_ASSERT(rshare);
        Q_ASSERT(rshare1);

        rshare->strRouterAdminPassword=mLogin->mUi.edt_pwd->text();
        rshare1->strRouterAdminPassword=mLogin->mUi.edt_pwd->text();
#endif
        if(mLogin->mUi.chk_save->isChecked())
        {
            QString username =mLogin->mUi.edt_user->text();
            QString password =mLogin->mUi.edt_pwd->text();
            mLogin->save_key(username,password);

        }
        mLogin->stopUiWaitState(true);

        mWireless->endSettingMode();
        mMain->mStack->setCurrentWidget(mWireless);
        mMain->mStack->setCurrentWidget(mWireless);
        mWireless->refresh();

        if(mInf->mPlugins[GeniePlugin_RouterConfiguration::SP_TRAFFICMETER])
        {
            mInf->mPlugins[GeniePlugin_RouterConfiguration::SP_TRAFFICMETER]->command(INFO_CMD_ROUTECONFIG_LOGIN,0);
        }
        mWireless->setBackgroundImg();
        mWireless->update();

    }
    else
    {
        const int cTotalCount=2;
        if(((++mLogin->mLoginTimes)!=cTotalCount)
                && !forcestop)
        {
            mLogin->tryStartLogin(SLP_NONE,SLP_NONE,mLogin->mLoginTimes);
        }
        else
        {
            mLogin->showError(reson_str);
            mLogin->stopUiWaitState(false);

        }
    }
}




bool QGenieRCLogic::isRefreshing()
{
    if(!mInitOk){return false;}
	qDebug()<<mMain->isEnabled()<<mLogin->isRefreshing()<<mWireless->isRefreshing()<<mGacc->isRefreshing();
    return (!mMain->isEnabled()||mLogin->isRefreshing()||mWireless->isRefreshing() ||mGacc->isRefreshing());

}

bool QGenieRCLogic::isLogin()
{
    if(!mInitOk){return false;}
    return (mMain->mStack->currentWidget()!=mMain->login);
}

bool QGenieRCLogic::isInApplyProcess()
{
    return mMain->mApplyTimeLine.state()==QTimeLine::Running;
}

#ifdef ENABLE_READYSHARE
void QGenieRCLogic::turnto_wirelesssettings()
{
    if(mWireless)
    {
        mWireless->SetWirelessPage();
    }
}
#endif
