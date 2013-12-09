#include "QGenieRSPrint.h"
#include <QEvent>

#include <QUuid>
#include <QUiLoader>
#include "pcpform.h"
#include "genie2_interface.h"
#include "language_RouterConfig.h"
#include "QProgressIndicator.h"

QString QGenieRSManager::wrapStr(const QString &s)
{
    const int lineW=75;
    if(s.length()<=lineW)
    {
        return s;
    }

    int idx=s.left(lineW).lastIndexOf(" ");
    QString ret=s;
    ret.insert(idx,"\n");
    return ret;
}


QGenieRSManager::QGenieRSManager()
    :mW(NULL)
//    ,btnBack(NULL)
    ,mErrorNum(-1)
{

//    strRouterAdminPassword = "password";
    http = new GenieHttp(this);

    mTimer = new QTimer(this);
    mTimer->setInterval(20*1000);
    mTimer->setSingleShot(true);

//    iTimerCount = 0;

    connect(mTimer, SIGNAL(timeout()), this, SLOT(slot_HttpTimeout()));
    connect (http,SIGNAL(authenticationRequired(QString,quint16,QAuthenticator*)),this,SLOT(authenticationRequired(QString,quint16,QAuthenticator*)));
    connect (http,SIGNAL(readyRead(QNetworkReply*)),this,SLOT(readyRead(QNetworkReply*)));
    connect (http,SIGNAL(requestFinished(int,bool)),this,SLOT(requestFinished(int,bool)));
//    connect (http,SIGNAL(requestStarted(int)),this,SLOT(requestStarted(int)));
//    btnBack->setEnabled(false);QNetWorkReply
    mCurId=-1;
}

QGenieRSManager::~QGenieRSManager()
{
    GENIE2_SAFEDELETE(mW);
    GENIE2_SAFEDELETE(http);
    GENIE2_SAFEDELETE(mTimer);
}



void QGenieRSManager::updateTitleButton(quint32 flag)
{
	QString path;
    if((flag&(1<<3)) && !mPcpForm->getPath(path))
    {
        GENIE2_UI_COMMAND(UICMD_TITLE_UPDATE,"valt_on");
    }
    else
    {
        GENIE2_UI_COMMAND(UICMD_TITLE_UPDATE,"valt_off");
    }
}

void QGenieRSManager::slot_HttpTimeout()
{
    http->abort();
    Q_ASSERT(mW);
    mW->setEnabled(true);
    showError(L_READY_SHARE_TIMEOUT);
    mPi->stopAnimation();
    mCurId=-1;
    mTimer->stop();
}
//void QGenieRSManager::requestStarted(int id)
//{
//    mCurId = id;
//}
void QGenieRSManager::authenticationRequired(const QString &hostname, quint16 port, QAuthenticator *authenticator)
{
    Q_UNUSED(port)
    Q_UNUSED(hostname)

    authenticator->setUser(mPcpForm->mUsername);
    authenticator->setPassword(mPcpForm->mPassword);
    qDebug()<<"mPcpForm->mUsername:"<<mPcpForm->mUsername;
    qDebug()<<"mPcpForm->mPassword:"<<mPcpForm->mPassword;
}
QString QGenieRSManager::getUser(const QString &str)
{
    int iValue1=str.indexOf("input type=\"hidden\" value=\"",0);
    int iName=str.indexOf("\" name=\"TXT_remote_login\"",0);
    if (iValue1>-1 && iName>-1 && iName>iValue1)
    {
        QString strUserName=str.mid(iValue1+27,iName-27-iValue1);
        qDebug()<<"strusername is "<<strUserName;
        return strUserName;
    }
    else
    {
        return "";
    }
    return "";
}
QString QGenieRSManager::getPassword(const QString &str)
{
    int iValue1=str.indexOf("input type=\"hidden\" value=\"",0);
    if (iValue1>0)
    {
        int iValue2=str.indexOf("input type=\"hidden\" value=\"",iValue1+5);
        int iPassword=str.indexOf("\" name=\"TXT_remote_password\"",0);
        if (iValue2>-1 && iPassword>-1 && iPassword>iValue2)
        {
            QString strPwd=str.mid(iValue2+27,iPassword-27-iValue2);
            qDebug()<<"strpwd is "<<strPwd;
            return strPwd;
        }
        else
        {
            return "";
        }
    }
    else
    {
        return "";
    }
    return "";
}

void QGenieRSManager::showError(int lan)
{
    Q_ASSERT(lblErrorString);
    mErrorNum=lan;
    lblErrorString->setText((mErrorNum<0)?"":GENIE2_GET_TEXT(lan));
}

void QGenieRSManager::readyRead(QNetworkReply* reply)
{
    //qDebug() << "QHttpResponseHeader is" + resp.toString();
    QByteArray bta = http->readAll();
    qDebug() << "http->readAll() is" << bta.data();
    QString strTemp(bta.data());
    if (strTemp.indexOf("Timeout")>-1)
    {
        mW->setEnabled(true);
        mPi->stopAnimation();
        mTimer->stop();
        showError(L_READY_SHARE_TIMEOUT);
        return;
    }
    if (strTemp.indexOf("unregister_user")>-1)
    {
        //register ok
        QString usrname=getUser(strTemp);
        QString password=getPassword(strTemp);
//        mPcpForm->mRSUser=usrname;
//        mPcpForm->mRSPwd=getPassword(strTemp);
        //strTemp
        mW->setEnabled(true);
        mPi->stopAnimation();
        mTimer->stop();
        mPcpForm->mStack->setCurrentWidget(mPcpForm->mRemoteObj->getUi());
        mPcpForm->mRemoteObj->setRegisterState(true,usrname,password);
//        emit RegisterOk( strRegUser, strRegPwd);
    }
    if (strTemp.indexOf("value=\"register_user\"")>-1)
    {
        mW->setEnabled(true);

        mPi->stopAnimation();
        mPcpForm->mStack->setCurrentWidget(mPcpForm->mRemoteObj->getUi());
        mPcpForm->mRemoteObj->setRegisterState(false,"","");
        mTimer->stop();

//        emit RegisterNotYet();
    }
	if (strTemp.indexOf("location.href=")>-1)
	{
		getInviteHtm(true);
	}
}

void QGenieRSManager::requestFinished(int id, bool error)
{
    if (mCurId==id && error==true && http->error()!= GenieHttp::Aborted)
    {

        qDebug()<<"======="<<error;
        mW->setEnabled(true);

        showError(L_READY_SHARE_TIMEOUT);
        mPi->stopAnimation();

        mCurId=-1;
        mTimer->stop();

    }
}
void QGenieRSManager::getInviteHtm(bool haslocation)
{
    qDebug()<<"===getInviteHtm";
    showError(-1);

    mW->setEnabled(false);

    http->abort();
    const QString &strIp=mPcpForm->Genie_IP();
    http->setHost(strIp);
	QString strUrl = "";
    if(haslocation)
		strUrl=QString("/RMT_remote.htm");
	else
		strUrl=QString("/cgi-bin/RMT_invite.htm");//RMT_remote.htm

    GenieHttpRequestHeader header("GET", strUrl);

    header.setValue("Accept","image/gif, image/jpeg, image/pjpeg, image/pjpeg, application/x-shockwave-flash, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, application/x-silverlight, application/x-ms-application");
    header.setValue("Referer","http://"+ strIp + "/USB_basic.htm");
    header.setValue("Accept-Language","en-US,zh-cn");
    header.setValue("User-Agent","Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022; CIBA; InfoPath.2)");

    header.setValue("Accept-Encoding","gzip, deflate");
    header.setValue("Host",strIp);
    header.setValue("Connection","Keep-Alive");
    qDebug()<<"ip:"<<strIp<<"getInviteHtm:\n";

    QList<QByteArray> list = header.rawHeaderList();
    foreach(QByteArray str, list)
        qDebug()<<str<<":"<<header.rawHeader(str);
    mCurId=http->request(header);

}

QWidget * QGenieRSManager::getUi()
{
    if(!mW)
    {
        QUiLoader loader;
        QFile f(GENIE2_RES("ui/readysharemanager.ui"));
        bool btemp=f.open(QFile::ReadOnly);
        Q_ASSERT(btemp);
        QWidget *w=loader.load(&f, NULL);
        Q_ASSERT(w);
        lblErrorString=w->findChild<QLabel *>("lab_err");
        grpSwitchType=w->findChild<QGroupBox *>("groupBox");
        rdoBasic=w->findChild<QRadioButton *>("rdt1");
        rdoRemote=w->findChild<QRadioButton *>("rdt2");
        rdoPrinting=w->findChild<QRadioButton *>("rdt3");
        rdoG9=w->findChild<QRadioButton *>("rdt4");
        QFrame *fr_pi=w->findChild<QFrame *>("fr_pi");
        btnContinue=w->findChild<QPushButton *>("but_continue");
        Q_ASSERT(lblErrorString && grpSwitchType && rdoBasic
                 && rdoRemote && rdoPrinting && fr_pi&& btnContinue);


        mPi=new QProgressIndicator();
        QHBoxLayout *layout=new QHBoxLayout(fr_pi);
		mPi->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        layout->setContentsMargins(0,0,0,0);
        layout->addWidget(mPi);

 
        


        connect(btnContinue,SIGNAL(clicked( bool)),this,SLOT(BtnContinueClicked( bool)));
        mW=w;
        retranslateUi();
    }
    return mW;



}

void QGenieRSManager::BtnContinueClicked( bool checked  )
{
    Q_UNUSED(checked)
    showError(-1);
    qDebug()<<"BtnContinueClicked( bool checked  )";
    if (rdoBasic->isChecked()==true)
    {
        mPcpForm->mStack->setCurrentWidget(mPcpForm->mBasicObj->getUi());

        mPcpForm->mBasicObj->bFirstShow=true;
        const QString strip=mPcpForm->Genie_IP();
        mPcpForm->mBasicObj->ipChanged(strip);
    }

    if (rdoPrinting->isChecked()==true)
    {
        mPcpForm->mStack->setCurrentWidget(mPcpForm->mPrintObj->getUi());
    }
    if (rdoRemote->isChecked()==true)
    {
        qDebug()<<"Readshar Remote";
        mPi->startAnimation();


        mTimer->start(20*1000);
        mW->setEnabled(false);
        getInviteHtm(false);
    }
    /////////////////////////////////////

    if(rdoG9->isChecked() == true)
    {
        mPcpForm->runExe();
        return;
    }

}

//void QGenieRSManager::BtnBackClicked( bool checked  )
//{
//    //todo »Øµ½¿ò¼ÜµÄhome
//    Q_UNUSED(checked)
//}

//void QGenieRSManager::changeEvent(QEvent *e)
//{
////    QWidget::changeEvent(e);
//    switch (e->type()) {
//        case QEvent::LanguageChange:
//            retranslateUi();
//            break;
//        default:
//            break;
//    }
//}


void QGenieRSManager::retranslateUi()
{
    if(!mW)
    {
        return;
    }

    grpSwitchType->setTitle(GENIE2_GET_TEXT(L_READY_SHARE_SWITCH_TYPE));
    rdoBasic->setText(GENIE2_GET_TEXT(L_READY_SHARE_FUN_ONE));
    QString strFunTwo="";
    strFunTwo=GENIE2_GET_TEXT(L_READY_SHARE_REMOTE);
    strFunTwo=strFunTwo+"(";
    strFunTwo=strFunTwo+GENIE2_GET_TEXT(L_READY_SHARE_FUN_TWO);
    strFunTwo=strFunTwo+")";
    strFunTwo=wrapStr(strFunTwo);
    rdoRemote->setText(strFunTwo);

    rdoPrinting->setText(GENIE2_GET_TEXT(L_READY_SHARE_FUN_THREE));
    btnContinue->setText(GENIE2_GET_TEXT(L_READY_SHARE_CONTINUE));
    lblErrorString->setText((mErrorNum<0)?"":GENIE2_GET_TEXT(mErrorNum));
    /////////////////////////////////////////////

    rdoG9->setText("ReadySHARE Vault"); 

#ifdef TEST_MODE_CS_I8
    rdoG9->setText("");
    rdoG9->setText(GENIE2_GET_TEXT(L_READY_SHARE_TEST_MODE, g_readyShare_Uuid));
#endif

}

void QGenieRSManager::init(quint32 flag)
{
    QRadioButton *cRadio[4]={
      rdoBasic,rdoRemote,rdoPrinting,rdoG9
    };
    for(int i=0;i<sizeof(cRadio)/sizeof(cRadio[0]);i++)
    {
        cRadio[i]->setEnabled(flag&(1<<i));
        cRadio[i]->setVisible((flag&(1<<i))||!flag);
    }
	
    mW->setEnabled(flag);
	///kai.yan modify bug
	//When the router does not support ReadySHARE, we should add the message "Your router does not support ReadySHARE." in ReadySHARE page.
	if(!flag)
	{
		 showError(L_READY_SHARE_MSG_NOTSUPPORT);
	}else
	{
		showError(-1);
	}
#ifdef Q_OS_MAC
    rdoG9->setVisible(false);
#endif
//    btnContinue->setEnabled(flag);

}





