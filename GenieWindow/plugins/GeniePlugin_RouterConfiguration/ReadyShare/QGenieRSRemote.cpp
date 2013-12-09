#include <QtCore>
#include <QtGui>
#include "QGenieRSRemote.h"
#include "genie2_interface.h"
#include "language_RouterConfig.h"
#include "pcpform.h"
#include <QUiLoader>

QGenieRSRemote::QGenieRSRemote()
    :mW(NULL)
    ,mRegisterId(-1)
    ,mUnRegisterId(-1)
    ,mErrorNum(-1)
{

    //    strRouterAdminPassword="password";
    http=new GenieHttp(this);

    timerHttp=new QTimer(this);
    timerHttp->setInterval(20*1000);
    timerHttp->setSingleShot(true);
    connect (timerHttp,SIGNAL(timeout()),this,SLOT(httpTimeout()));

    //void QHttp::authenticationRequired ( const QString & hostname, quint16 port, QAuthenticator * authenticator ) ;
    connect (http,SIGNAL(authenticationRequired(QString,quint16,QAuthenticator*)),this,SLOT(authenticationRequired(QString,quint16,QAuthenticator*)));
    connect (http,SIGNAL(readyRead(QNetworkReply *)),this,SLOT(readyRead(QNetworkReply *)));
    connect (http,SIGNAL(requestFinished(int,bool)),this,SLOT(requestFinished(int,bool)));
    //    connect (http,SIGNAL(requestStarted(int)),this,SLOT(requestStarted(int)));
}

QGenieRSRemote::~QGenieRSRemote()
{
    GENIE2_SAFEDELETE(mW);
    GENIE2_SAFEDELETE(http);
    GENIE2_SAFEDELETE(timerHttp);
}

void QGenieRSRemote::httpTimeout()
{
    http->abort();
    mStack->setEnabled(true);

    showError(L_READY_SHARE_TIMEOUT);
    mPi->stopAnimation();


    mRegisterId=-1;

    timerHttp->stop();
}


//void	QGenieRSRemote::requestStarted ( int id )
//{
//    iCurId=id;
//}


void	QGenieRSRemote::requestFinished ( int id, bool error )
{
    qDebug()<<"=======requestFinished id ="<<id<<" error -"<<error;
    if ((mRegisterId==id || mUnRegisterId==id)&& error==true && http->error()!=GenieHttp::Aborted)
    {
        //"HTTP request failed"
        showError(L_READY_SHARE_TIMEOUT);

        mStack->setEnabled(true);

        mPi->stopAnimation();
        timerHttp->stop();

        mRegisterId=-1;
    }
}



void QGenieRSRemote::authenticationRequired ( const QString & hostname, quint16 port, QAuthenticator * authenticator )
{
    Q_UNUSED(port)
    Q_UNUSED(hostname)
    PCPForm *pcpform=PCPForm::GetSingletonPtr();
    Q_ASSERT(pcpform);
    authenticator->setUser(pcpform->mUsername);
    authenticator->setPassword(pcpform->mPassword);
}

QWidget * QGenieRSRemote::getUi()
{
    if(!mW)
    {
        QUiLoader loader;
        QFile f(GENIE2_RES("ui/readyshareremote.ui"));
        bool btemp=f.open(QFile::ReadOnly);
        Q_ASSERT(btemp);
        QWidget *w=loader.load(&f, NULL);
        Q_ASSERT(w);

        //step1 find all member pointer
        mEdtError=w->findChild<QLabel *>("lab_error");
        Q_ASSERT( mEdtError);
        mTxtUser=w->findChild<QLineEdit *>("edt_usr");
        mTxtPwd=w->findChild<QLineEdit *>("edt_pwd");
        Q_ASSERT(mTxtUser && mTxtPwd );
        mStack=w->findChild<QStackedWidget *>("stack_register");
        Q_ASSERT(mStack);
        QFrame *fr_pi=w->findChild<QFrame *>("fr_pi");
        Q_ASSERT(fr_pi);
        mPi=new QProgressIndicator();
        QHBoxLayout *layout=new QHBoxLayout(fr_pi);
		mPi->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        layout->setContentsMargins(0,0,0,0);
        layout->addWidget(mPi);


        //step2 connect signal&slot
        connect(mTxtUser,SIGNAL(textChanged(const QString)),this,SLOT(textAnyChanged(const QString)));
        connect(mTxtPwd,SIGNAL(textChanged(const QString)),this,SLOT(textAnyChanged(const QString)));
        QList<QPushButton *> bts=w->findChildren<QPushButton *>();
        int c=bts.count();
        for(int i=0;i<c;i++)
        {
            connect(bts[i],SIGNAL(clicked()),this,SLOT(slot_butClicked()));
        }

        const char *cPara[]={"lab_recount","lab_step1","lab_step3"};
        for(int i=0;i<sizeof(cPara)/sizeof(cPara[0]);i++)
        {
            QLabel *lab=w->findChild<QLabel *>(cPara[i]);
            Q_ASSERT(lab);
            connect(lab,SIGNAL(linkActivated(QString)),this,SLOT(slot_linkActivated(QString)));

        }


        mW=w;
        retranslateUi();
    }
    return mW;
}
///
//void QGenieRSRemote::CountClickR16( const QString &count )
//{
//    //"click here" links
//    GENIE2_STATISTICS("R16");
//    QDesktopServices::openUrl( count );
//}
///
//void QGenieRSRemote::CountClickR17( const QString &count )
//{
//    //"readyshare.netgear.com" links
//    GENIE2_STATISTICS("R17");
//    QDesktopServices::openUrl( count );
//}
void	QGenieRSRemote::textAnyChanged ( const QString & text )
{
    Q_UNUSED(text)
    QLineEdit *edt=qobject_cast<QLineEdit *>(sender());
    Q_ASSERT(edt==mTxtPwd || edt==mTxtUser);
    QPushButton *but_register=mW->findChild<QPushButton *>("but_register");
    Q_ASSERT(but_register);
    but_register->setEnabled(!mTxtPwd->text().isEmpty() && !mTxtUser->text().isEmpty() );

}

//void QGenieRSRemote::BtnContinueClicked( bool checked  )
//{
//    Q_UNUSED(checked)

//    showError(-1);

//    QString strRegUser=mTxtUser->text();
//    QString strRegPwd=mTxtPwd->text();
//    QString strData=QString("submit_flag=register_user&TXT_remote_login=%1&TXT_remote_password=%2&BTN_reg=Register").arg(strRegUser).arg(strRegPwd);
//    qDebug()<<strData;

//    mStack->setEnabled(false);

//    mPi->startAnimation();
//    timerHttp->start(10000);


//    http->abort();
//    const QString &strIp=PCPForm::GetSingletonPtr()->Genie_IP();
//    http->setHost(strIp);
//    QString strUrl=QString("/cgi-bin/RMT_invite.cgi?/cgi-bin/RMT_invite.htm");
//    QByteArray POSTdata=strData.toUtf8();

//    QHttpRequestHeader header("POST", strUrl);

//    header.setValue("Accept","image/gif, image/jpeg, image/pjpeg, image/pjpeg, application/x-shockwave-flash, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, application/x-silverlight, application/x-ms-application");
//    header.setValue("Referer","http://" + strIp+ "/cgi-bin/RMT_invite.htm?unreg_ok");
//    header.setValue("Accept-Language","zh-cn");
//    header.setValue("User-Agent","Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022; CIBA; InfoPath.2)");
//    header.setValue("Content-Type", "application/x-www-form-urlencoded");
//    header.setValue("Accept-Encoding","gzip, deflate");
//    header.setValue("Host",strIp);
//    header.setValue("Content-Length","99");
//    header.setValue("Connection","Keep-Alive");
//    header.setValue("Cache-Control","no-cache");
//    mCurId=http->request(header,POSTdata);

//    ///
//    GENIE2_STATISTICS("R18");

//}



void	QGenieRSRemote::readyRead ( QNetworkReply *reply )
{
    Q_UNUSED(reply);
    //qDebug()<<"QHttpResponseHeader is " +resp.toString();///kai.yan resp.toString() can not transform

    QByteArray bta = http->readAll();
    qDebug()<<"http->readAll() is " <<bta.data();

    QString strTemp(bta.data());

    if (strTemp.indexOf("Timeout")>-1)
    {
        showError(L_READY_SHARE_TIMEOUT);
    }
    else if (strTemp.indexOf("Register to Server Successfully")>-1
             || strTemp.indexOf("RMT_invite.htm?register_ok") >-1)
    {
        //register ok
        qDebug()<<"=====Register OK: ";
        setRegisterState(true,mTxtUser->text(),mTxtPwd->text());
    }
    else if (strTemp.indexOf("Unregister to Server Successfully")>-1
             ||strTemp.indexOf("cgi-bin/RMT_invite.htm?unreg_ok") > -1)
    {
        setRegisterState(false,"","");
    }
    else if (strTemp.indexOf("Invalid User Name or Password")>-1
             ||strTemp.indexOf("RMT_invite.htm?register_fail") > -1)
    {
        //register fail
        showError(L_READY_SHARE_INVALID_USER);
    }
    else if(strTemp.indexOf("RMT_invite.htm?unreg_fail") > -1)
    {
        showError(L_READY_SHARE_TIMEOUT);
    }
    mPi->stopAnimation();
    timerHttp->stop();
    //add by caiyujun
    //request over should close socket,otherwise would not send SYN request in next request
//    http->abort();
    //add end
    mStack->setEnabled(true);
}

void QGenieRSRemote::setRegisterState(bool registered, const QString &username, const QString &password)
{
    mStack->setCurrentIndex(registered?1:0);
    if(registered)
    {
        QLabel *lab_registered_usr=mW->findChild<QLabel *>("lab_registered_usr");
        Q_ASSERT(lab_registered_usr);
        lab_registered_usr->setText(username);
        mUsername=username;
        mPassword=password;
    }
}

void QGenieRSRemote::showError(int lan)
{
    mErrorNum=lan;
    mEdtError->setText((mErrorNum<0)?"":GENIE2_GET_TEXT(mErrorNum));
}

void QGenieRSRemote::registerRS()
{
    qDebug()<<"======registerRS";
    showError(-1);
    QString strRegUser=mTxtUser->text();
    QString strRegPwd=mTxtPwd->text();
    Q_ASSERT(!strRegPwd.isEmpty()||!strRegUser.isEmpty());
    QString strData=QString("submit_flag=register_user&TXT_remote_login=%1&TXT_remote_password=%2&BTN_reg=Register").arg(strRegUser).arg(strRegPwd);
    qDebug()<<strData;

    const QString &strIp=PCPForm::GetSingletonPtr()->Genie_IP();
    http->setHost(strIp);
    QString strUrl=QString("/cgi-bin/RMT_invite.cgi?/cgi-bin/RMT_invite.htm");
    QByteArray POSTdata=strData.toUtf8();

    GenieHttpRequestHeader header("POST", strUrl.toLatin1());

    header.setValue("Accept","image/gif, image/jpeg, image/pjpeg, image/pjpeg, application/x-shockwave-flash, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, application/x-silverlight, application/x-ms-application");
    header.setValue("Referer","http://" + strIp+ "/cgi-bin/RMT_invite.htm?unreg_ok");
    header.setValue("Accept-Language","zh-cn");
    header.setValue("User-Agent","Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022; CIBA; InfoPath.2)");
    header.setValue("Content-Type", "application/x-www-form-urlencoded");
    header.setValue("Accept-Encoding","gzip, deflate");
    header.setValue("Host", strIp);
    header.setValue("Content-Length","99");
    header.setValue("Connection","Keep-Alive");
    header.setValue("Cache-Control","no-cache");


    mRegisterId=http->post(header, POSTdata);
    qDebug()<<"====R post id: "<<mRegisterId;
    ///
    GENIE2_STATISTICS("R18");
}

void QGenieRSRemote::unregisterRS()
{
    //timerHttp->start(40000);
    showError(-1);

    const QString &strIp=PCPForm::GetSingletonPtr()->Genie_IP();
    http->setHost(strIp);
    QString strUrl=QString("/cgi-bin/RMT_invite.cgi?/cgi-bin/RMT_invite.htm");
    QString strData=QString("submit_flag=unregister_user&TXT_remote_login=%1&TXT_remote_password=%2&BTN_unreg=Unregister")
                    .arg(mUsername).arg(mPassword);

    QByteArray POSTdata=strData.toUtf8();

    GenieHttpRequestHeader header("POST", strUrl.toLatin1());

    header.setValue("Accept","image/gif, image/jpeg, image/pjpeg, image/pjpeg, application/x-shockwave-flash, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, application/x-silverlight, application/x-ms-application");
    header.setValue("Referer","http://" + strIp+ "/cgi-bin/RMT_invite.htm");
    header.setValue("Accept-Language","zh-cn");
    header.setValue("User-Agent","Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022; CIBA; InfoPath.2)");
    header.setValue("Content-Type", "application/x-www-form-urlencoded");
    header.setValue("Accept-Encoding","gzip, deflate");
    header.setValue("Host",strIp);
    header.setValue("Content-Length","105");
    header.setValue("Connection","Keep-Alive");
    header.setValue("Cache-Control","no-cache");

    mUnRegisterId=http->post(header, POSTdata);
    qDebug()<<"====unR post id: "<<mUnRegisterId;
    ///
    GENIE2_STATISTICS("R19");
}

//void QGenieRSRemote::BtnBackClicked( bool checked  )
//{
//    Q_UNUSED(checked)
//    showError(-1);

//    PCPForm::GetSingletonPtr()->showPreFrom();

//}

//void QGenieRSRemote::changeEvent(QEvent *e)
//{
//    switch (e->type()) {
//    case QEvent::LanguageChange:
//        retranslateUi();
//        break;
//    default:
//        break;
//    }
//}


void QGenieRSRemote::retranslateUi()
{
    if(!mW)
    {
        return;
    }
    const struct _{const char *objname;int lan;int type;const char *mode;}
    cPara[]={
        {"lab_usr", L_READY_SHARE_REMOTE_USERNAME, 0,NULL}
        ,{"lab_pwd",L_READY_SHARE_REMOTE_PASSWORD,0,NULL}
        ,{"lab_step1",L_READY_SHARE_REMOTE_STEPS_ONE,0,NULL}
        ,{"lab_step2",L_READY_SHARE_REMOTE_STEPS_TWO,0,NULL}
        ,{"lab_step3",L_READY_SHARE_REMOTE_STEPS_THREE,0,NULL}
        ,{"lab_step4",L_READY_SHARE_REMOTE_STEPS_FOUR,0,NULL}
        ,{"lab_note",L_READY_SHARE_REMOTE_STEPS_NOTE,0,NULL}
        ,{"lab_start",L_READY_SHARE_REMOTE_START_NOW,0,NULL}
        ,{"lab_desc",L_READY_SHARE_REMOTE_DESC,0,NULL}
        ,{"lab_how",L_READY_SHARE_REMOTE_HOW_WORKS,0,"<b>%1</b>"}
        ,{"lab_readyshare",L_READY_SHARE_REMOTE,0,"<b>%1</b>"}
        ,{"lab_recount",L_READY_SHARE_REMOTE_START_NOW_DESC,0,NULL}
        ,{"but_register",L_READY_SHARE_REGISTER,1,NULL}
        ,{"but_unregister",L_READY_SHARE_UNREGISTER,1,NULL}
        ,{"but_back",L_READY_SHARE_BACK,1,NULL}
        ,{"but_back2",L_READY_SHARE_BACK,1,NULL}
		,{"lab_buttom1", L_READY_SHARE_REMOTE_REGESTER_USER, 0, NULL}
		,{"lab_buttom2", L_READY_SHARE_REMOTE_REGESTER_ANYWAY, 0, NULL}
		,{"lab_buttom3", L_READY_SHARE_REMOTE_REGESTER_DIFFERENT, 0, NULL}
    };

    for(int i=0;i<sizeof(cPara)/sizeof(cPara[0]);i++)
    {
        QString s=GENIE2_GET_TEXT(cPara[i].lan);
        if(cPara[i].mode)
        {
            s=QString(cPara[i].mode).arg(s);
        }
        if(0==cPara[i].type)
        {
            QLabel *lab=mW->findChild<QLabel*>(cPara[i].objname);
            Q_ASSERT(lab);
            lab->setText(s);
        }
        else if(1==cPara[i].type)
        {
            QPushButton *but=mW->findChild<QPushButton*>(cPara[i].objname);
            Q_ASSERT(but);
            but->setText(s);
        }

    }
    mEdtError->setText((mErrorNum<0)?"":GENIE2_GET_TEXT(mErrorNum));
}

void QGenieRSRemote::slot_butClicked()
{
    QPushButton *but=qobject_cast<QPushButton *>(sender());
    Q_ASSERT(but);
    qDebug()<<"====slot_butClicked: "<<but->objectName();
    if(but->objectName()=="but_register")
    {
        registerRS();
        mStack->setEnabled(false);

        mPi->startAnimation();
        timerHttp->start(40000);
    }
    else if(but->objectName()=="but_back" || but->objectName()=="but_back2")
    {
        mPi->stopAnimation();
        http->abort();
        timerHttp->stop();
        showError(-1);
        PCPForm *pcpform=PCPForm::GetSingletonPtr();
        Q_ASSERT(pcpform);
        pcpform->mStack->setCurrentWidget(pcpform->mManagerObj->getUi());
       // GENIE2_STATISTICS("R18");
    }
    else if(but->objectName()=="but_unregister")
    {
        unregisterRS();
        mStack->setEnabled(false);

        mPi->startAnimation();
        timerHttp->start(40000);
       // GENIE2_STATISTICS("R19");
    }
}

void QGenieRSRemote::slot_linkActivated(const QString &url)
{
    QLabel *label=qobject_cast<QLabel *>(sender());
    QString objname=label->objectName();
    if(label && (objname=="lab_step1" || objname=="lab_recount"))
    {
        GENIE2_STATISTICS("R16");
    }
    else if(label && (label->objectName()=="lab_step3"))
    {
        GENIE2_STATISTICS("R17");
    }
    QDesktopServices::openUrl(url);
}
