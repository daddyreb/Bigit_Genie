#include "QGeniePlugin_Basic_Remote_Printing_ReadyShare.h"
#include <QEvent>
#include "ILanguageManager.h"
//#include "LanguageReadyShare.h"
#include "LanguageRouterConfiguration.h"

#include <QEvent>
#include "plugin_ui_def.h"
#include "IClickManager.h"
#include "IUiManager.h"

#include <QUuid>
extern QUuid g_readyShare_Uuid;

IManagerHandler* QGeniePlugin_Basic_Remote_Printing_ReadyShare::sHandler = NULL;
QStackedLayout* QGeniePlugin_Basic_Remote_Printing_ReadyShare::stackLayout = NULL;

QGeniePlugin_Basic_Remote_Printing_ReadyShare::QGeniePlugin_Basic_Remote_Printing_ReadyShare(QWidget *parent) :
    QWidget(parent)
{
    iErrorNum=0;
    strHost="192.168.1.1";
    strIpInUsing="192.168.1.1";
    strRouterAdminPassword="password";
    http=new QHttp(this);

    timerHttp=new QTimer(this);
    timerHttp->setInterval(10000);
    timerHttp->stop();
    connect (timerHttp,SIGNAL(timeout()),this,SLOT(httpTimeout()));
    iTimerCount=0;
    //void QHttp::authenticationRequired ( const QString & hostname, quint16 port, QAuthenticator * authenticator ) ;
    connect (http,SIGNAL(authenticationRequired(QString,quint16,QAuthenticator*)),this,SLOT(authenticationRequired(QString,quint16,QAuthenticator*)));
    connect (http,SIGNAL(readyRead(QHttpResponseHeader)),this,SLOT(readyRead(QHttpResponseHeader)));
    connect (http,SIGNAL(requestFinished(int,bool)),this,SLOT(requestFinished(int,bool)));
    connect (http,SIGNAL(requestStarted(int)),this,SLOT(requestStarted(int)));
    setupUi();
    btnBack->setEnabled(false);
    iCurId=-1;
}

void QGeniePlugin_Basic_Remote_Printing_ReadyShare::httpTimeout()
{
    //timerHttp->setInterval(30000);
    iTimerCount=iTimerCount+1;
    if (iTimerCount==4)
    {
        http->abort();
        btnContinue->setEnabled(true);
        btnBack->setEnabled(false);
        iErrorNum=1;
        lblErrorString->setText(strTimeout);

        qPrg->stopAnimation();
        rdoBasic->setEnabled(true);
        rdoRemote->setEnabled(true);
        rdoPrinting->setEnabled(true);

        iCurId=-1;

        timerHttp->stop();
        iTimerCount=0;
    }
}

void	QGeniePlugin_Basic_Remote_Printing_ReadyShare::requestStarted ( int id )
{
    iCurId=id;
}

void QGeniePlugin_Basic_Remote_Printing_ReadyShare::authenticationRequired ( const QString & hostname, quint16 port, QAuthenticator * authenticator )
{
    authenticator->setUser(QString("admin"));
    authenticator->setPassword(strRouterAdminPassword);

}

QString QGeniePlugin_Basic_Remote_Printing_ReadyShare::getUser(QString str)
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

QString QGeniePlugin_Basic_Remote_Printing_ReadyShare::getPassword(QString str)
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

void	QGeniePlugin_Basic_Remote_Printing_ReadyShare::readyRead ( const QHttpResponseHeader & resp )
{
    qDebug()<<"QHttpResponseHeader is " +resp.toString();
    QByteArray bta=http->readAll();
    qDebug()<<"http->readAll() is " <<bta.data();

    QString strTemp(bta.data());
    if (strTemp.indexOf("Timeout")>-1)
    {
        btnContinue->setEnabled(true);
        btnBack->setEnabled(false);
        qPrg->stopAnimation();
        timerHttp->stop();
        iTimerCount=0;

        rdoBasic->setEnabled(true);
        rdoRemote->setEnabled(true);
        rdoPrinting->setEnabled(true);


        iErrorNum=1;
        lblErrorString->setText(strTimeout);

        return;
    }
    if (strTemp.indexOf("unregister_user")>-1)
    {
        //register ok
        strRegUser=getUser(strTemp);
        strRegPwd=getPassword(strTemp);
        //strTemp

        btnContinue->setEnabled(true);
        btnBack->setEnabled(false);
        qPrg->stopAnimation();
        timerHttp->stop();
        iTimerCount=0;

        rdoBasic->setEnabled(true);
        rdoRemote->setEnabled(true);
        rdoPrinting->setEnabled(true);

        emit RegisterOk( strRegUser, strRegPwd);
    }
    if (strTemp.indexOf("value=\"register_user\"")>-1)
    {
        btnContinue->setEnabled(true);
        btnBack->setEnabled(false);

        qPrg->stopAnimation();
        timerHttp->stop();
        iTimerCount=0;

        rdoBasic->setEnabled(true);
        rdoRemote->setEnabled(true);
        rdoPrinting->setEnabled(true);

        emit RegisterNotYet();
    }
}

 void	QGeniePlugin_Basic_Remote_Printing_ReadyShare::requestFinished ( int id, bool error )
 {
    if (iCurId==id && error==true && http->errorString()!="Request aborted")
    {

        qDebug()<<http->errorString();
        btnContinue->setEnabled(true);
        btnBack->setEnabled(false);

        iErrorNum=1;
        lblErrorString->setText(strTimeout);

        qPrg->stopAnimation();
        rdoBasic->setEnabled(true);
        rdoRemote->setEnabled(true);
        rdoPrinting->setEnabled(true);

        iCurId=-1;
        timerHttp->stop();
        iTimerCount=0;
    }
 }

void QGeniePlugin_Basic_Remote_Printing_ReadyShare::getInviteHtm()
{


    iErrorNum=0;
    lblErrorString->setText(" ");

    btnContinue->setEnabled(false);
    btnBack->setEnabled(false);

    http->abort();
    http->setHost(strIpInUsing);
    QString strUrl=QString("/cgi-bin/RMT_invite.htm");

    QHttpRequestHeader header("GET", strUrl);

    header.setValue("Accept","image/gif, image/jpeg, image/pjpeg, image/pjpeg, application/x-shockwave-flash, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, application/x-silverlight, application/x-ms-application");
    header.setValue("Referer","http://"+ strIpInUsing + "/USB_basic.htm");
    header.setValue("Accept-Language","zh-cn");
    header.setValue("User-Agent","Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022; CIBA; InfoPath.2)");

    header.setValue("Accept-Encoding","gzip, deflate");
    header.setValue("Host",strIpInUsing);
    header.setValue("Connection","Keep-Alive");
    http->request(header);

}


void QGeniePlugin_Basic_Remote_Printing_ReadyShare::rdobasicchecked()
{
    rdoBasic->setChecked(true);
    rdoRemote->setChecked(false);
    rdoPrinting->setChecked(false);
}
void QGeniePlugin_Basic_Remote_Printing_ReadyShare::rdoremotechecked()
{
    rdoBasic->setChecked(false);
    rdoRemote->setChecked(true);
    rdoPrinting->setChecked(false);
}
void QGeniePlugin_Basic_Remote_Printing_ReadyShare::rdoprintingchecked()
{
    rdoBasic->setChecked(false);
    rdoRemote->setChecked(false);
    rdoPrinting->setChecked(true);
}
void QGeniePlugin_Basic_Remote_Printing_ReadyShare::setupUi()
{

    strReadyShare="ReadyShare";
    strDesc="The router does not support ReadyShare at all!";

   // lblReadyShare=new QLabel(strReadyShare,this,0);
   // lblReadyShare->setWordWrap(false);
   // SET_STYLE_PROPERTY(PARENT_LABEL1 ,lblReadyShare) ;


    lblErrorString=new QLabel(this);
    lblErrorString->setWordWrap(false);
    lblErrorString->setStyleSheet("color: rgb(255, 0, 0);");


    grpSwitchType=new QGroupBox(this);

    //grpSwitchType->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    grpSwitchType->setMinimumWidth(580);

    //grpSwitchType->setTitle("Switch Type");


    rdoBasic=new QRadioButton(grpSwitchType);
    connect(rdoBasic,SIGNAL(clicked()),this,SLOT(rdobasicchecked()));
    rdoBasic->setText("Basic");
    rdoBasic->setChecked(true);
    rdoBasic->setFixedWidth(13);
    rdoBasic->setFixedHeight(13);

    rdoRemote=new QRadioButton(grpSwitchType);
    connect(rdoRemote,SIGNAL(clicked()),this,SLOT(rdoremotechecked()));
    rdoBasic->setText("Basic");
    rdoRemote->setChecked(false);
    rdoRemote->setFixedWidth(13);
    rdoRemote->setFixedHeight(13);
    //rdoRemote->set
    //rdoRemote->


    rdoPrinting=new QRadioButton(grpSwitchType);
     connect(rdoPrinting,SIGNAL(clicked()),this,SLOT(rdoprintingchecked()));
    rdoPrinting->setChecked(false);
    rdoPrinting->setFixedWidth(13);
    rdoPrinting->setFixedHeight(13);



    rdoBasic->setText("");
    rdoRemote->setText("");
    rdoPrinting->setText("");

    txtBasic=new     QLabel(grpSwitchType);
    txtBasic->setWordWrap(true);
    txtBasic->setFixedWidth(550);
    //txtBasic->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);


    txtRemote=new     QLabel(grpSwitchType);
    txtRemote->setWordWrap(true);
    txtRemote->setFixedWidth(550);
    //txtRemote->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);


    txtPrinting=new     QLabel(grpSwitchType);
    txtPrinting->setWordWrap(true);
    txtPrinting->setFixedWidth(550);
   // txtPrinting->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);


    QGridLayout *vbox = new QGridLayout(this);

    vbox->addWidget(rdoBasic,0,0,1,1,Qt::AlignLeft|Qt::AlignVCenter);
    vbox->addWidget(rdoRemote,1,0,1,1,Qt::AlignLeft|Qt::AlignVCenter);
    vbox->addWidget(rdoPrinting,2,0,1,1,Qt::AlignLeft|Qt::AlignVCenter);


    vbox->addWidget(txtBasic,0,1,1,1,Qt::AlignLeft|Qt::AlignVCenter);
    vbox->addWidget(txtRemote,1,1,1,1,Qt::AlignLeft|Qt::AlignVCenter);
    vbox->addWidget(txtPrinting,2,1,1,1,Qt::AlignLeft|Qt::AlignVCenter);


    vbox->setColumnStretch(1,1);


    grpSwitchType->setLayout(vbox);


    btnContinue=new QPushButton(this);
    btnContinue->setText("Continue");
    SET_STYLE_PROPERTY(FLEX_BUTTON,btnContinue);
    connect(btnContinue,SIGNAL(clicked( bool)),this,SLOT(BtnContinueClicked( bool)));


    btnBack=new QPushButton(this);
    SET_STYLE_PROPERTY(FLEX_BUTTON,btnBack);
    btnBack->setText("Back");


    connect(btnBack,SIGNAL(clicked( bool)),this,SLOT(BtnBackClicked( bool)));


    qPrg=new QProgressIndicator(this);
    qPrg->setFixedSize(35,35);
    qPrg->setDisplayedWhenStopped(false);

    gridAll=new QGridLayout(this);

    gridAll->setContentsMargins(30,30,20,20);

  //  gridAll->addWidget(lblReadyShare,0,0,1,1,Qt::AlignTop|Qt::AlignLeft);

    gridAll->addWidget(lblErrorString,0,1,1,1,Qt::AlignTop|Qt::AlignRight);
    gridAll->addWidget(qPrg,0,3,1,1,Qt::AlignLeft);

    gridAll->addWidget(grpSwitchType,1,0,1,4,Qt::AlignLeft);

    gridAll->addWidget(btnContinue,3,2,1,1,Qt::AlignBottom|Qt::AlignRight);
    gridAll->addWidget(btnBack,3,3,1,1,Qt::AlignBottom|Qt::AlignRight);


    gridAll->setRowStretch(2,280);
    gridAll->setColumnStretch(1,280);

    this->setLayout(gridAll);

    retranslateUi();
}

void QGeniePlugin_Basic_Remote_Printing_ReadyShare::BtnContinueClicked( bool checked  )
{
    iErrorNum=0;
    lblErrorString->setText(" ");

    if (rdoBasic->isChecked()==true)
    {
        stackLayout->setCurrentIndex(5);
    }

    if (rdoPrinting->isChecked()==true)
    {
        stackLayout->setCurrentIndex(7);
    }
    if (rdoRemote->isChecked()==true)
    {

        qPrg->startAnimation();

        iTimerCount=0;
        timerHttp->start(10000);


        rdoBasic->setEnabled(false);
        rdoRemote->setEnabled(false);
        rdoPrinting->setEnabled(false);

        getInviteHtm();
    }

    emit preIndexSend(4);
}

void QGeniePlugin_Basic_Remote_Printing_ReadyShare::BtnBackClicked( bool checked  )
{
    iErrorNum=0;
    lblErrorString->setText(" ");

    IUiManager *mUiPage = QGeniePlugin_Basic_Remote_Printing_ReadyShare::sHandler->getUiManager();
    Q_ASSERT(mUiPage);

    mUiPage->changeStackIdx(0);
}

void QGeniePlugin_Basic_Remote_Printing_ReadyShare::changeEvent(QEvent *e)
{
   // ILanguageManager *lanmanager=WorkPanel::sHandler->getLanguageManager();
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        //ui->mainmenubut->setText(lanmanager->getText(L_TESTUI_MAINBUT));
        retranslateUi();
        break;
    default:
        break;
    }
}


void QGeniePlugin_Basic_Remote_Printing_ReadyShare::retranslateUi()
{
    ILanguageManager *lanmanager=QGeniePlugin_Basic_Remote_Printing_ReadyShare::sHandler->getLanguageManager();
   // lblReadyShare->setText(lanmanager->getText(L_READY_SHARE_LIVE,g_readyShare_Uuid));
    grpSwitchType->setTitle(lanmanager->getText(L_READY_SHARE_SWITCH_TYPE,g_readyShare_Uuid));
    txtBasic->setText(lanmanager->getText(L_READY_SHARE_FUN_ONE,g_readyShare_Uuid));
    QString strFunTwo="";
    //L_READY_SHARE_REMOTE
    //L_READY_SHARE_FUN_TWO
    strFunTwo=lanmanager->getText(L_READY_SHARE_REMOTE,g_readyShare_Uuid);
    strFunTwo=strFunTwo+"(";
    strFunTwo=strFunTwo+lanmanager->getText(L_READY_SHARE_FUN_TWO,g_readyShare_Uuid);
    strFunTwo=strFunTwo+")";
    txtRemote->setText(strFunTwo);

    txtPrinting->setText(lanmanager->getText(L_READY_SHARE_FUN_THREE,g_readyShare_Uuid));
    btnBack->setText(lanmanager->getText(L_READY_SHARE_BACK,g_readyShare_Uuid));
    btnContinue->setText(lanmanager->getText(L_READY_SHARE_CONTINUE,g_readyShare_Uuid));

    strTimeout=lanmanager->getText(L_READY_SHARE_TIMEOUT,g_readyShare_Uuid);
    if (iErrorNum==1)
    {
        lblErrorString->setText(strTimeout);
    }
    else
    {
         lblErrorString->setText(" ");
    }
}
