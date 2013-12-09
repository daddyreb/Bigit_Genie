#include "QGeniePlugin_Remote_Choosed_ReadyShare.h"
#include <QEvent>
#include "ILanguageManager.h"
//#include "LanguageReadyShare.h"
#include "LanguageRouterConfiguration.h"

#include <QEvent>
#include "plugin_ui_def.h"
#include "IClickManager.h"
#include "IUiManager.h"
#include <QFont>


#include <QUuid>
extern QUuid g_readyShare_Uuid;


IManagerHandler* QGeniePlugin_Remote_Choosed_ReadyShare::sHandler = NULL;
QStackedLayout* QGeniePlugin_Remote_Choosed_ReadyShare::stackLayout = NULL;

QGeniePlugin_Remote_Choosed_ReadyShare::QGeniePlugin_Remote_Choosed_ReadyShare(QWidget *parent) :
        QWidget(parent)
{
    iErrorNum=0;
    strHost="192.168.1.1";
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
    iCurId=-1;
}

void QGeniePlugin_Remote_Choosed_ReadyShare::httpTimeout()
{
    //timerHttp->setInterval(30000);
    iTimerCount=iTimerCount+1;
    if (iTimerCount==4)
    {
        http->abort();

        btnRegister->setEnabled(true);
        btnBack->setEnabled(true);
        txtUser->setEnabled(true);
        txtPwd->setEnabled(true);

        iErrorNum=1;
        lblErrorString->setText(strTimeout);

        qPrg->stopAnimation();


        iCurId=-1;

        timerHttp->stop();
        iTimerCount=0;
    }
}


void	QGeniePlugin_Remote_Choosed_ReadyShare::requestStarted ( int id )
{
    iCurId=id;
}


void	QGeniePlugin_Remote_Choosed_ReadyShare::requestFinished ( int id, bool error )
{
    if (iCurId==id && error==true && http->errorString()!="Request aborted")
    {
        //"HTTP request failed"
        qDebug()<<http->errorString();

        iErrorNum=1;
        lblErrorString->setText(strTimeout);


        btnRegister->setEnabled(true);
        btnBack->setEnabled(true);
        txtUser->setEnabled(true);
        txtPwd->setEnabled(true);

        qPrg->stopAnimation();
        timerHttp->stop();
        iTimerCount=0;

        iCurId=-1;
    }
}



void QGeniePlugin_Remote_Choosed_ReadyShare::authenticationRequired ( const QString & hostname, quint16 port, QAuthenticator * authenticator )
{
    authenticator->setUser(QString("admin"));
    authenticator->setPassword(strRouterAdminPassword);
}

void QGeniePlugin_Remote_Choosed_ReadyShare::setupUi()
{

    strReadyShare="ReadyShare";
    strDesc="The router does not support ReadyShare at all!";
    strIpInUsing="192.168.1.1";

    scrollArea = new QScrollArea(this);

    scrollArea->setFrameShape(QFrame::WinPanel);
    scrollArea->setFrameShadow(QFrame::Sunken);
    scrollArea->setWidgetResizable(true);
    scrollArea->setAttribute(Qt::WA_TranslucentBackground,false);

    scrollAreaWidgetContents = new QWidget(scrollArea);
    scrollAreaWidgetContents->setAttribute(Qt::WA_TranslucentBackground,false);

    scrollAreaWidgetContents->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    scrollAreaWidgetContents->setStyleSheet("background-color: qlineargradient(spread:pad, x1:1, y1:1, x2:1, y2:0, stop:0 rgba(214, 214, 214, 253), stop:1 rgba(253, 253, 253, 253));");

    verticalLayout_2 = new QVBoxLayout(scrollAreaWidgetContents);
    verticalLayout_2->setSizeConstraint(QLayout::SetNoConstraint);
    verticalLayout_2->setSpacing(0);
    verticalLayout_2->setContentsMargins(0, 0, 0, 0);


    lblReadyShare=new QLabel(strReadyShare,this,0);
    lblReadyShare->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);


    lblErrorString=new QLabel(this);
    lblErrorString->setWordWrap(false);
    lblErrorString->setStyleSheet("color: rgb(255, 0, 0);");

    lblReadyShare->setWordWrap(false);
    SET_STYLE_PROPERTY(PARENT_LABEL1 ,lblReadyShare) ;

    lblDesc=new QLabel(scrollAreaWidgetContents);
    //lblDesc->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);


    lblDesc->setWordWrap(true);
    lblDesc->setAttribute(Qt::WA_TranslucentBackground,true);

    lblHow=new QLabel(scrollAreaWidgetContents);
    lblHow->setAttribute(Qt::WA_TranslucentBackground,true);
    lblHow->setWordWrap(true);
    lblHow->setTextFormat(Qt::RichText);
   // lblHow->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);


    lblStep1=new QLabel(scrollAreaWidgetContents);
    //lblHow->setWordWrap(true);


    lblStep1->setWordWrap(true);
    lblStep1->setAttribute(Qt::WA_TranslucentBackground,true);
    lblStep1->setTextFormat(Qt::RichText);
    lblStep1->setOpenExternalLinks(true);
    lblStep1->setTextInteractionFlags(Qt::LinksAccessibleByMouse);

    lblStep2=new QLabel(scrollAreaWidgetContents);
    lblStep2->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    lblStep2->setAttribute(Qt::WA_TranslucentBackground,true);
    lblStep2->setWordWrap(true);
    lblStep2->setTextFormat(Qt::RichText);
    lblStep2->setOpenExternalLinks(true);
    lblStep2->setTextInteractionFlags(Qt::LinksAccessibleByMouse);

    lblStep3=new QLabel(scrollAreaWidgetContents);
    lblStep3->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    lblStep3->setAttribute(Qt::WA_TranslucentBackground,true);
    lblStep3->setWordWrap(true);
    lblStep3->setTextFormat(Qt::RichText);
    lblStep3->setOpenExternalLinks(true);
    lblStep3->setTextInteractionFlags(Qt::LinksAccessibleByMouse);

    lblStep4=new QLabel(scrollAreaWidgetContents);
    lblStep4->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    lblStep4->setAttribute(Qt::WA_TranslucentBackground,true);
    lblStep4->setWordWrap(true);
    lblStep4->setTextFormat(Qt::RichText);
    lblStep4->setOpenExternalLinks(true);
    lblStep4->setTextInteractionFlags(Qt::LinksAccessibleByMouse);

    lblNote=new QLabel(scrollAreaWidgetContents);
    lblNote->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    lblNote->setAttribute(Qt::WA_TranslucentBackground,true);
    lblNote->setWordWrap(true);
    lblNote->setTextFormat(Qt::RichText);
    lblNote->setOpenExternalLinks(true);
    lblNote->setTextInteractionFlags(Qt::LinksAccessibleByMouse);


    lblStart=new QLabel(this);
    lblStart->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    lblStart->setAttribute(Qt::WA_TranslucentBackground,true);
    lblGetRecount=new QLabel(this);
    lblGetRecount->setAttribute(Qt::WA_TranslucentBackground,true);
    lblUser=new QLabel(this);
    lblUser->setAttribute(Qt::WA_TranslucentBackground,true);
    lblPwd=new QLabel(this);
    lblPwd->setAttribute(Qt::WA_TranslucentBackground,true);



    lblGetRecount->setWordWrap(true);
    lblGetRecount->setAttribute(Qt::WA_TranslucentBackground,true);
    lblGetRecount->setTextFormat(Qt::RichText);
    lblGetRecount->setOpenExternalLinks(true);
    lblGetRecount->setTextInteractionFlags(Qt::LinksAccessibleByMouse);


    lblDesc->setMinimumWidth(580);
    lblHow->setMinimumWidth(580);
    lblStep1->setMinimumWidth(580);
    lblStep2->setMinimumWidth(580);
    lblStep3->setMinimumWidth(580);
    lblStep4->setMinimumWidth(580);
    lblNote->setMinimumWidth(580);
    lblGetRecount->setMinimumWidth(580);
    lblStart->setMinimumWidth(580);

    txtUser=new QLineEdit(this);
    txtUser->setFixedWidth(150);
    txtPwd=new QLineEdit(this);
    txtPwd->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    txtPwd->setFixedWidth(150);

    connect(txtUser,SIGNAL(textChanged(const QString)),this,SLOT(textAnyChanged(const QString)));
    connect(txtPwd,SIGNAL(textChanged(const QString)),this,SLOT(textAnyChanged(const QString)));

    txtUser->setText("");
    txtPwd->setText("");


    btnRegister=new QPushButton(this);
    btnRegister->setText("Continue");
    SET_STYLE_PROPERTY(FLEX_BUTTON,btnRegister);
    btnRegister->setEnabled(false);
    connect(btnRegister,SIGNAL(clicked( bool)),this,SLOT(BtnContinueClicked( bool)));

    btnBack=new QPushButton(this);
    SET_STYLE_PROPERTY(FLEX_BUTTON,btnBack);
    btnBack->setText("Back");

    connect(btnBack,SIGNAL(clicked( bool)),this,SLOT(BtnBackClicked( bool)));


    qPrg=new QProgressIndicator(this);
    qPrg->setFixedSize(35,35);
    qPrg->setDisplayedWhenStopped(false);

    gridAll=new QGridLayout(this);
    gridAll->setContentsMargins(5,5,5,10);

    gridAll->addWidget(lblReadyShare,0,0,1,2,Qt::AlignVCenter|Qt::AlignLeft);
    gridAll->addWidget(lblErrorString,0,2,1,2,Qt::AlignVCenter|Qt::AlignRight);


    gridAll->addWidget(qPrg,0,4,1,1,Qt::AlignLeft);


    //    QFrame* frameLine1;
    //    QFrame* frameLine2;
    frameLine1=new QFrame(this);
    frameLine1->setFrameShape(QFrame::HLine);
    frameLine1->setFrameShadow(QFrame::Sunken);
    frameLine1->setFixedHeight(2);
    frameLine1->setFixedWidth(608);
    frameLine1->setStyleSheet("background:qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgb( 176, 176, 176), stop:1 rgba( 255, 255, 255));");

    frameLine2=new QFrame(this);
    frameLine2->setFrameShape(QFrame::HLine);
    frameLine2->setFrameShadow(QFrame::Sunken);
    frameLine2->setFixedWidth(608);
    frameLine2->setFixedHeight(2);
    frameLine2->setStyleSheet("background:qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgb( 176, 176, 176), stop:1 rgba( 255, 255, 255));");


    gridAll->addWidget(frameLine1,1,0,1,5,Qt::AlignLeft);


    //verticalLayout_2->addWidget(frameLine1);
    verticalLayout_2->addWidget(lblDesc,1,Qt::AlignTop|Qt::AlignLeft);
    verticalLayout_2->addWidget(lblHow,1,Qt::AlignTop|Qt::AlignLeft);
    verticalLayout_2->addWidget(lblStep1,1,Qt::AlignTop|Qt::AlignLeft);
    verticalLayout_2->addWidget(lblStep2,1,Qt::AlignTop|Qt::AlignLeft);
    verticalLayout_2->addWidget(lblStep3,1,Qt::AlignTop|Qt::AlignLeft);
    verticalLayout_2->addWidget(lblStep4,1,Qt::AlignTop|Qt::AlignLeft);
    verticalLayout_2->addWidget(lblNote,1,Qt::AlignTop|Qt::AlignLeft);
    //verticalLayout_2->addWidget(frameLine2);

    scrollAreaWidgetContents->setLayout(verticalLayout_2);
    scrollArea->setWidget(scrollAreaWidgetContents);




    gridAll->addWidget(/*widgetScroll*/scrollArea,2,0,1,5/*,Qt::AlignCenter*/);
    gridAll->setRowStretch(2,100);

    gridAll->addWidget(frameLine2,3,0,1,5,Qt::AlignLeft);




    gridAll->addWidget(lblStart,4,0,1,5,Qt::AlignTop|Qt::AlignLeft);

    gridAll->addWidget(lblGetRecount,5,0,1,5,Qt::AlignTop|Qt::AlignLeft);

    gridAll->addWidget(lblUser,6,0,1,1,Qt::AlignTop|Qt::AlignLeft);
    gridAll->addWidget(txtUser,6,1,1,1,Qt::AlignTop|Qt::AlignLeft);
    gridAll->addWidget(lblPwd,7,0,1,1,Qt::AlignTop|Qt::AlignLeft);
    gridAll->addWidget(txtPwd,7,1,1,1,Qt::AlignTop|Qt::AlignLeft);
    gridAll->addWidget(btnRegister,9,3,1,1,Qt::AlignBottom|Qt::AlignRight);
    gridAll->addWidget(btnBack,9,4,1,1,Qt::AlignBottom|Qt::AlignRight);


    gridAll->setRowStretch(8,1);
    gridAll->setColumnStretch(2,1);

    this->setLayout(gridAll);

    retranslateUi();
}

void	QGeniePlugin_Remote_Choosed_ReadyShare::textAnyChanged ( const QString & text )
{
    if (txtPwd->text()=="" || txtUser->text()=="" )
    {
        btnRegister->setEnabled(false);
    }
    else
    {
        btnRegister->setEnabled(true);
    }
}

void QGeniePlugin_Remote_Choosed_ReadyShare::BtnContinueClicked( bool checked  )
{

    iErrorNum=0;
    lblErrorString->setText(" ");

    strRegUser=txtUser->text();
    strRegPwd=txtPwd->text();
    QString strData=QString("submit_flag=register_user&TXT_remote_login=%1&TXT_remote_password=%2&BTN_reg=Register").arg(strRegUser).arg(strRegPwd);
    qDebug()<<strData;

    btnRegister->setEnabled(false);
    btnBack->setEnabled(false);

    txtUser->setEnabled(false);
    txtPwd->setEnabled(false);

    qPrg->startAnimation();
    iTimerCount=0;
    timerHttp->start(10000);


    http->abort();
    http->setHost(strIpInUsing);
    QString strUrl=QString("/cgi-bin/RMT_invite.cgi?/cgi-bin/RMT_invite.htm");
    QByteArray POSTdata=strData.toUtf8();

    QHttpRequestHeader header("POST", strUrl);

    header.setValue("Accept","image/gif, image/jpeg, image/pjpeg, image/pjpeg, application/x-shockwave-flash, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, application/x-silverlight, application/x-ms-application");
    header.setValue("Referer","http://" + strIpInUsing+ "/cgi-bin/RMT_invite.htm?unreg_ok");
    header.setValue("Accept-Language","zh-cn");
    header.setValue("User-Agent","Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022; CIBA; InfoPath.2)");
    header.setValue("Content-Type", "application/x-www-form-urlencoded");
    header.setValue("Accept-Encoding","gzip, deflate");
    header.setValue("Host",strIpInUsing);
    header.setValue("Content-Length","99");
    header.setValue("Connection","Keep-Alive");
    header.setValue("Cache-Control","no-cache");
    http->request(header,POSTdata);


}



void	QGeniePlugin_Remote_Choosed_ReadyShare::readyRead ( const QHttpResponseHeader & resp )
{
    qDebug()<<"QHttpResponseHeader is " +resp.toString();
    QByteArray bta=http->readAll();
    qDebug()<<"http->readAll() is " <<bta.data();

    QString strTemp(bta.data());

    if (strTemp.indexOf("Timeout")>-1)
    {
        btnRegister->setEnabled(true);
        btnBack->setEnabled(true);

        txtUser->setEnabled(true);
        txtPwd->setEnabled(true);

        iErrorNum=1;
        lblErrorString->setText(strTimeout);

        qPrg->stopAnimation();
        timerHttp->stop();
        iTimerCount=0;
        return;
    }
    if (strTemp.indexOf("Register to Server Successfully")>-1)
    {
        //register ok
        btnRegister->setEnabled(true);
        btnBack->setEnabled(true);

        txtUser->setEnabled(true);
        txtPwd->setEnabled(true);

        qPrg->stopAnimation();
        timerHttp->stop();
        iTimerCount=0;
        emit RegisterOk( strRegUser, strRegPwd);

    }
    if (strTemp.indexOf("Invalid User Name or Password")>-1)
    {
        //register fail
        iErrorNum=2;
        lblErrorString->setText(strInvalidUser);

        qPrg->stopAnimation();
        timerHttp->stop();
        iTimerCount=0;
        btnRegister->setEnabled(true);
        btnBack->setEnabled(true);

        txtUser->setEnabled(true);
        txtPwd->setEnabled(true);
    }
}

void QGeniePlugin_Remote_Choosed_ReadyShare::BtnBackClicked( bool checked  )
{

    iErrorNum=0;
    lblErrorString->setText(" ");

    emit ShowPreForm();
}

void QGeniePlugin_Remote_Choosed_ReadyShare::changeEvent(QEvent *e)
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


void QGeniePlugin_Remote_Choosed_ReadyShare::retranslateUi()
{


//    QString str=QString("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"
//                        "<html>"
//                        "<head>"
//                        "<meta name=\"qrichtext\" content=\"1\" />"
//                        "<style type=\"text/css\">"
//                        "p, li { white-space: pre-wrap; }"
//                        "</style>"
//                        "</head>"
//                        "<body style=\" font-size:10pt; \">"
//                        "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
//                        "%1"
//                        "</p>"
//                        "</body>"
//                        "</html>");



//    QString str=QString("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"
//                "<html>"
//                    "<head>"
//                        "<meta name=\"qrichtext\" content=\"1\" />"
//                            "<style type=\"text/css\">"
//                                "p, li { white-space: pre-wrap; }"
//                            "</style>"
//                    "</head>"
//                    "<body style=\" font-size:10pt; \">"
//                        "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
//                            "%1"
//                        "</p>"
//                    "</body>"
//                "</html>");

    ILanguageManager *lanmanager=QGeniePlugin_Remote_Choosed_ReadyShare::sHandler->getLanguageManager();

    lblReadyShare->setText("<b>"+lanmanager->getText(L_READY_SHARE_REMOTE,g_readyShare_Uuid)+ "</b>");
    lblDesc->setText(lanmanager->getText(L_READY_SHARE_REMOTE_DESC,g_readyShare_Uuid));
    lblHow->setText("<b>"+lanmanager->getText(L_READY_SHARE_REMOTE_HOW_WORKS,g_readyShare_Uuid)+"</b>");
    QString strTextStep1=lanmanager->getText(L_READY_SHARE_REMOTE_STEPS_ONE,g_readyShare_Uuid);
    lblStep1->setText(strTextStep1);
    lblStep2->setText(lanmanager->getText(L_READY_SHARE_REMOTE_STEPS_TWO,g_readyShare_Uuid));

    QString strTextStep3=lanmanager->getText(L_READY_SHARE_REMOTE_STEPS_THREE,g_readyShare_Uuid);
    lblStep3->setText(strTextStep3);

    lblStep4->setText(lanmanager->getText(L_READY_SHARE_REMOTE_STEPS_FOUR,g_readyShare_Uuid));
    lblStart->setText(lanmanager->getText(L_READY_SHARE_REMOTE_START_NOW,g_readyShare_Uuid));
    lblUser->setText(lanmanager->getText(L_READY_SHARE_REMOTE_USERNAME,g_readyShare_Uuid));
    lblPwd->setText(lanmanager->getText(L_READY_SHARE_REMOTE_PASSWORD,g_readyShare_Uuid));
    btnBack->setText(lanmanager->getText(L_READY_SHARE_BACK,g_readyShare_Uuid));
    btnRegister->setText(lanmanager->getText(L_READY_SHARE_REGISTER,g_readyShare_Uuid));

    QString strGetRecount=lanmanager->getText(L_READY_SHARE_REMOTE_START_NOW_DESC,g_readyShare_Uuid);
    lblGetRecount->setText(strGetRecount);

    QString strNote=lanmanager->getText(L_READY_SHARE_REMOTE_STEPS_NOTE,g_readyShare_Uuid);
    lblNote->setText(strNote);

    strTimeout=lanmanager->getText(L_READY_SHARE_TIMEOUT,g_readyShare_Uuid);
    strInvalidUser=lanmanager->getText(L_READY_SHARE_INVALID_USER,g_readyShare_Uuid);
    if (iErrorNum==1)
    {
        lblErrorString->setText(strTimeout);
    }
    if (iErrorNum==2)
    {
        lblErrorString->setText(strInvalidUser);
    }
    if (iErrorNum==0)
    {
        lblErrorString->setText(" ");
    }



}
