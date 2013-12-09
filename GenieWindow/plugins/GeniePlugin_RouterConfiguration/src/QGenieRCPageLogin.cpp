#include "QGenieRCPageLogin.h"
#include "mainwindow.h"
#include "inc/plugin_ui_def/plugin_ui_def.h"
#include "inc/genie_interface/IManagerHandler.h"
#include "inc/genie_interface/IInfoManager.h"
#include "inc/multilanguage_def/LanguageRouterConfiguration.h"
#include "setting_def.h"
#include "QGenieRCSoapProxy.h"
#include <QDebug>
#include <QThread>
#include <QUrl>
#include "QGenieRCLogic.h"
#include "common.h"
#include "IClickManager.h"


#define INI_SECTION  "WirelessSettings"


QGenieRCPageLogin::QGenieRCPageLogin(QWidget* parent)
    :QWidget(parent)
    ,mErrorTextIdx(-1)
    ,mLoginTimes(0)
#ifdef HTTP_LOGIN
    ,mCurHttpRequestId(-1)
#endif

{
    mUi.setupUi(this);


    g_settings =MainWindow::sHandler->getSettings();

    ReloadPassword();

    uuid ="{6c1c9b2f-5f5e-45f8-8dad-1633ff446377}";

    mLoginTimer = new QTimer(this);

    connect(mLoginTimer,SIGNAL(timeout()),this,SLOT(slot_LoginTimeout()));
//    connect(mUi.but_login,SIGNAL(clicked()),this,SLOT(slot_LoginIn()));
    connect(mUi.but_cancel,SIGNAL(clicked()),this,SLOT(slot_Cancel()));
    //connect(mUi.chk_save,SIGNAL(clicked()),this,SLOT(JudgePassword()));
    connect(mUi.edt_pwd,SIGNAL(textEdited(QString)),this,SLOT(PasswordLineEdit(QString)));
#ifdef HTTP_LOGIN
    connect(&mLoginHttp,SIGNAL(done(bool)),this,SLOT(slot_HttpDone(bool)));
    connect(&mLoginHttp,SIGNAL(requestFinished(int,bool)),this,SLOT(slot_HttpRequestFinished(int,bool)));
#endif
    SET_STYLE_PROPERTY(FLEX_BUTTON,mUi.but_login);
    SET_STYLE_PROPERTY(FLEX_BUTTON,mUi.but_cancel);
    retranslateUi();
    hideError();
//    mThreadIsNetgear=new QThreadIsNetgear(this);
 //   connect(mThreadIsNetgear,SIGNAL(finished()),this,SLOT(slot_IsNetgearDone()));
}
QGenieRCPageLogin::~QGenieRCPageLogin()
{
    mLoginTimer->stop();
//    mThreadIsNetgear->wait();
#ifdef HTTP_LOGIN
    mLoginHttp.abort();
#endif
}
//Change QGenieRCPageLogin Page Language
void QGenieRCPageLogin::changeEvent(QEvent* event)
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
//Set QGenieRCPageLogin Page Widget text
void QGenieRCPageLogin::retranslateUi()
{
    lanmanager=MainWindow::sHandler->getLanguageManager();
//    groupBox->setTitle(QString());
    mUi.label_user->setText(lanmanager->getText(L_RTCF_LOGIN_USER,uuid));
    mUi.label_pwd->setText(lanmanager->getText(L_RTCF_PASSWORD,uuid));
    mUi.but_login->setText(lanmanager->getText(L_RTCF_LOGIN_BUT_LEFT,uuid));
    mUi.but_cancel->setText(lanmanager->getText(L_RTCF_CANCEL,uuid));
    mUi.label_title->setText(lanmanager->getText(L_RTCF_LOGIN_TITLE,uuid));

    if(mErrorTextIdx >=0)
    {
        QString mode=ERROR_MODE;
        QString message =lanmanager->getText(mErrorTextIdx,uuid);
        mUi.label_error->setText(mode.arg(message));
    }

    mUi.chk_save->setText(lanmanager->getText(L_RTCF_LOGIN_REMEMBER,uuid));
    mUi.label_default->setText(lanmanager->getText(L_RTCF_DEFAULT,uuid));
#if 0
    QString sub1=lanmanager->getText(L_RTCF_BIGBT,uuid);
    QString sub2=lanmanager->getText(L_GACC_BIGBT,uuid);
    QString sub3=lanmanager->getText(L_RTCF_TRAFFICMETTER,uuid);
    QString sub4=lanmanager->getText(L_RTCF_FUPDATE,uuid);
    QString lst=QString(".%1\n.%2\n.%3\n.%4\n").arg(sub1,sub2,sub3,sub4);
#else
    QString lst=lanmanager->getText(L_RTCF_LOGIN_LIST,uuid);
#endif
    mUi.label_list->setText(lst);


    mUi.label_list_title->setText(lanmanager->getText(L_RTCF_LIST_TITLE,uuid));
}
//ReLoad the Rouer password,if you have saved
void QGenieRCPageLogin::ReloadPassword()
{
    load_key();
    bool bsave=GET_SETTING_BOOL(PLUGIN_ROUTECONF_SAVEPWD);
    if(bsave)
    {
        mUi.chk_save->setChecked(true);
//        ckb_savepwd->setChecked(true);
    }
}
//Load The Router Password
void QGenieRCPageLogin::load_key()
{
    qDebug()<<"QGenieRCPageLogin::Load_key(const QString &user)";
    if(g_settings)
    { 
        QString usrname=GET_SETTING_STR(PLUGIN_ROUTECONF_USER);
        mUi.edt_user->setText(usrname);

        QString pwd=GET_SETTING_STR(PLUGIN_ROUTECONF_PWD);
        mUi.edt_pwd->setText(pwd);
    }

}
//Save The Router Password
void QGenieRCPageLogin::save_key(const QString &user, const QString &pwd)
{
    qDebug()<<"QGenieRCPageLogin::save_key"<<user<<pwd;
    if(!g_settings)
    {
        return;
    }
    SET_SETTING_VALUE(PLUGIN_ROUTECONF_USER,user);
    SET_SETTING_VALUE(PLUGIN_ROUTECONF_PWD,pwd);

}


//Keyboard enter events
void QGenieRCPageLogin::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Return)
    {
        tryStartLogin(SLP_NONE,SLP_ALL);
    }
}
//QGenieRCPageLogin Timer Out
void QGenieRCPageLogin::slot_LoginTimeout()
{
    QGenieRCLogic logic;
    logic.loginProcessEnd(false,L_RTCF_MSG_LOGINTIMEOUT);
}

void QGenieRCPageLogin::on_but_login_clicked()
{
    qDebug()<<"QGenieRCPageLogin::on_but_login_clicked()";
    tryStartLogin(SLP_NONE,SLP_ALL);
    if(mUi.edt_pwd->text()!=PLUGIN_ROUTECONF_PWD_DEFAULT)
    {
        ADD_CLICK("R1");
    }
}

void QGenieRCPageLogin::on_chk_save_clicked(bool ck)
{
    qDebug()<<"QGenieRCPageLogin::on_chk_save_clicked";
    SET_SETTING_VALUE(PLUGIN_ROUTECONF_SAVEPWD,ck);


    if(!ck)
    {
        QString username =mUi.edt_user->text().trimmed();
        QString password =mUi.edt_pwd->text().trimmed();
        password = "" ;
        save_key(username,password);
        mUi.edt_pwd->clear();
        ADD_CLICK("R2");
    }
}


void QGenieRCPageLogin::startUiWaitState()
{
    hideError();
    qDebug()<<"QGenieRCPageLogin::PiStart()";
    mUi.pi->startAnimation();
    mUi.but_login->setDisabled(true);
//    mUi.but_cancel->setDisabled(true);
    mUi.edt_pwd->setDisabled(true);
    mUi.edt_user->setDisabled(true);
    mUi.chk_save->setDisabled(true);
    mLoginTimer->start(40000);
}

void QGenieRCPageLogin::stopUiWaitState(bool bOk)
{
    if(bOk)
    {
        hideError();
    }
    else
    {
        QGenieRCLogic::mPendingCmd=SLP_NONE;
    }
    qDebug()<<"QGenieRCPageLogin::PiStop()";
    mLoginTimer->stop();
    mUi.pi->stopAnimation();
    mUi.but_login->setDisabled(false);
//    mUi.but_cancel->setDisabled(false);
    mUi.edt_pwd->setDisabled(false);
    mUi.chk_save->setDisabled(false);
    QGenieRCSoapProxy *soapproxy=QGenieRCSoapProxy::GetSingletonPtr();
    ASSERT(soapproxy);
    soapproxy->ignoreRequest(ESC_LOGIN_PWD);
}

//Go to Login
void QGenieRCPageLogin::GoToLogin()
{

    hideError();

    QGenieRCPageGuessAccess *gacc=QGenieRCPageGuessAccess::GetSingletonPtr();
    ASSERT(gacc);
    gacc->whenLogin();

    // startUiWaitState();
    //mLoginTimes=0;

#ifndef HTTP_LOGIN
    qDebug()<<"QGenieRCPageLogin::GoToLogin()";
    // emit SendLoginNo(0);
    QGenieRCSoapProxy *soapproxy=QGenieRCSoapProxy::GetSingletonPtr();
    ASSERT(soapproxy);
    QMap<QString,QString> paras;
    paras.insert("NewUsername",mUi.edt_user->text());
    paras.insert("NewPassword",mUi.edt_pwd->text());
    soapproxy->command(ESC_LOGIN_PWD,&paras);
#else
    QUrl url=QUrl("http://routerlogin.com/index.htm");
    qDebug()<<url.host()<<url.path();
    //const QString cHost="routerlogin.com";
  //  mLoginHttp.setHost(,QHttp::ConnectionModeHttp);
    QHttpRequestHeader header=QHttpRequestHeader("GET",url.path());
    header.setValue("Connection","Keep-Alive");
    header.setValue("Host",url.host());
    QString key=QString("%1:%2").arg(edt_username->text(),edt_pwd->text());
    qDebug()<<"login key is "<<key;
    key="Basic "+key.toAscii().toBase64();

    header.setValue("Authorization",key.toAscii());

    mLoginHttp.abort();
    mLoginHttp.setHost(url.host(),80);
    mCurHttpRequestId=mLoginHttp.request(header,0,0);
#endif
}
//Clear the password
void QGenieRCPageLogin::slot_Cancel()
{
    qDebug()<<"QGenieRCPageLogin::Clearup()";
    QGenieRCLogic logic;
    logic.logout();
}
//Get Router Password
void QGenieRCPageLogin::parseSoapResponse(int cmd,QtSoapMessage &message)
{
    qDebug()<<"QGenieRCPageLogin::parseSoapResponse ";
    QGenieRCLogic logic;
#ifdef FAKE_PWD
    const bool bFack=true;
#else
    const bool bFack=false;
#endif
    if (!message.isFault() || bFack)
    {
        const QtSoapType &res = message.returnValue();
        if( message.returnResponseCode().toString().toInt() == 0 ||bFack)
        {
            logic.loginProcessEnd(true,0);
        }
        else if(message.returnResponseCode().toString().toInt() == 401)
        {
            logic.loginProcessEnd(false,L_RTCF_MSG_PWDWRONG,true);
        }
        else
        {
            logic.loginProcessEnd(false,L_RTCT_MSG_SOAPERROR_LOGIN);
        }
    }
    else
    {
        logic.loginProcessEnd(false,L_RTCT_MSG_SOAPERROR_LOGIN);
    }
}
//To Set Timer Stop

void QGenieRCPageLogin::whenSoapError(int cmd,const QString& reson )
{
    QGenieRCLogic logic;
    logic.loginProcessEnd(false,L_RTCT_MSG_SOAPERROR_LOGIN);
}

//When Password lineEdit is Changed,Do this
void QGenieRCPageLogin::PasswordLineEdit(QString pwd)
{
    qDebug()<<"QGenieRCPageLogin::PasswordLineEdit "<<pwd;
//    mUi.but_cancel->setDisabled(false);
}


bool QGenieRCPageLogin::isPwdEmpty()
{
    qDebug()<<"Login::isPwdEmpty()";
    if(mUi.edt_pwd->text()=="")
    {
        return true;
    }
    return false;

}

void QGenieRCPageLogin::tryStartLogin(PENDING_CMD pcmd,quint32 clearPending,int trytime)
{
    MainWindow *main=MainWindow::GetSingletonPtr();
    ASSERT(main);



    QGenieRCLogic::mPendingCmd &= ~clearPending;
    QGenieRCLogic::mPendingCmd |= pcmd;

    IInfoManager *infom=MainWindow::sHandler->getInfoManager();
    Q_ASSERT(infom);

     if(0==trytime && mLoginTimer->isActive())
     {
         return;
     }
    mLoginTimes=0;
     startUiWaitState();
     QGenieRCLogic::mPendingCmd |= (SLP_LOGIN);

     QGenieRCSoapProxy *soapproxy=QGenieRCSoapProxy::GetSingletonPtr();
     ASSERT(soapproxy);
     soapproxy->ignoreAllRequest();

     infom->getInfo(INFO_CMD_NETGEAR_REFRESH);
     //now wait for COMMAND_NOTIFYROUTETYPE message

}




void QGenieRCPageLogin::showError(int idx)
{
    QString mode=ERROR_MODE;
    QString message =mode.arg(lanmanager->getText(idx,uuid));
    mErrorTextIdx=idx;
    mUi.label_error->setText(message);
    mUi.label_error->show();
}
void QGenieRCPageLogin::hideError()
{
    mUi.label_error->hide();
}
#ifdef HTTP_LOGIN
void QGenieRCPageLogin::slot_HttpDone(bool err)
{
    qDebug()<<"QGenieRCPageLogin::slot_HttpDone"<<err;
}

void QGenieRCPageLogin::slot_HttpRequestFinished(int id,bool error)
{
    qDebug()<<" QGenieRCPageLogin::slot_HttpRequestFinished"<<id<<error<<mCurHttpRequestId;
    if(mCurHttpRequestId == id)
    {
        int code=mLoginHttp.lastResponse().statusCode();
        qDebug()<<code;
        loginOk(code==200);
    }


}
#endif
