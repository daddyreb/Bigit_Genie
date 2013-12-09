#include "QGeniePlugin_Basic_Choosed_ReadyShare.h"
#include <QEvent>
#include "ILanguageManager.h"
//#include "LanguageReadyShare.h"
#include "LanguageRouterConfiguration.h"

#include <QEvent>
#include "plugin_ui_def.h"
#include "IClickManager.h"
#include "IUiManager.h"
#include <QFont>
#include <QUrl>
#include <QDesktopServices>

#include <QUuid>
extern QUuid g_readyShare_Uuid;

IManagerHandler* QGeniePlugin_Basic_Choosed_ReadyShare::sHandler = NULL;
QStackedLayout* QGeniePlugin_Basic_Choosed_ReadyShare::stackLayout = NULL;

QGeniePlugin_Basic_Choosed_ReadyShare::QGeniePlugin_Basic_Choosed_ReadyShare(QWidget *parent) :
    QWidget(parent)
{
    setupUi();
}

void QGeniePlugin_Basic_Choosed_ReadyShare::setupUi()
{

    //strReadyShare="ReadyShare";
    //strDesc="The router does not support ReadyShare at all!";
    strRouterIp="";
    //strUrl="file:///192.168.1.1";
    lblReadyShare=new QLabel(this);
    lblReadyShare->setWordWrap(false);
    SET_STYLE_PROPERTY(PARENT_LABEL1 ,lblReadyShare) ;

     lblDevice=new QLabel(this);
     lblDevice->setWordWrap(false);
     lblDevice->setText("Network/Device Name:");

     lblDevicePash=new QLabel(this);
     lblDevicePash->setWordWrap(false);

     lblDevicePash->setTextFormat(Qt::RichText);
     lblDevicePash->setOpenExternalLinks(true);
     lblDevicePash->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
     QString str=QString("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"
                 "<html>"
                     "<head>"
                         "<meta name=\"qrichtext\" content=\"1\" />"
                             "<style type=\"text/css\">"
                                 "p, li { white-space: pre-wrap; }"
                             "</style>"
                     "</head>"
                     "<body style=\" font-family:'ËÎÌו'; font-size:12pt; font-weight:400; font-style:normal;\">"
                         "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
                             "<a href=\"file:\\\\%1\\\">"
                                 "<span style=\" text-decoration: underline; color:#0000ff;\">\\\\%2"
                                 "</span>"
                              "</a>"
                         "</p>"
                     "</body>"
                 "</html>").arg(strRouterIp).arg(strRouterIp);

     lblDevicePash->setText(str);


     QFont serifFont("Times", 12, QFont::Bold);
     serifFont.setUnderline(true);
     serifFont.setBold(false);

     lblDevicePash->setFont(serifFont);


    btnBack=new QPushButton(this);
    SET_STYLE_PROPERTY(FLEX_BUTTON,btnBack);
    btnBack->setText("Back");

    connect(btnBack,SIGNAL(clicked( bool)),this,SLOT(BtnBackClicked( bool)));

    gridAll=new QGridLayout(this);


    gridAll->setContentsMargins(30,30,20,20);

    gridAll->addWidget(lblReadyShare,0,0,1,4,Qt::AlignTop|Qt::AlignLeft);


    gridAll->addWidget(lblDevice,1,0,1,1,Qt::AlignTop|Qt::AlignLeft);
    gridAll->addWidget(lblDevicePash,1,2,1,1,Qt::AlignTop|Qt::AlignLeft);
    gridAll->addWidget(btnBack,3,3,1,1,Qt::AlignBottom|Qt::AlignRight);
    gridAll->setRowStretch(2,280);
    gridAll->setColumnStretch(2,280);
    this->setLayout(gridAll);
    retranslateUi();
}

void QGeniePlugin_Basic_Choosed_ReadyShare::linkActivated ( const QString & link )
{
    //QDesktopServices::openUrl(QUrl(strUrl, QUrl::TolerantMode));
}

void QGeniePlugin_Basic_Choosed_ReadyShare::ipChanged(QString strIp)
{
    strRouterIp=strIp;
   // strUrl="file:///" + strIp;//192.168.1.1"
    QString str=QString("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"
                "<html>"
                    "<head>"
                        "<meta name=\"qrichtext\" content=\"1\" />"
                            "<style type=\"text/css\">"
                                "p, li { white-space: pre-wrap; }"
                            "</style>"
                    "</head>"
                    "<body style=\" font-family:'ËÎÌו'; font-size:12pt; font-weight:400; font-style:normal;\">"
                        "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
                            "<a href=\"file:\\\\%1\\\">"
                                "<span style=\" text-decoration: underline; color:#0000ff;\">\\\\%2"
                                "</span>"
                             "</a>"
                        "</p>"
                    "</body>"
                "</html>").arg(strIp).arg(strIp);

    lblDevicePash->setText(str);
}

void QGeniePlugin_Basic_Choosed_ReadyShare::BtnContinueClicked( bool checked  )
{

}

void QGeniePlugin_Basic_Choosed_ReadyShare::BtnBackClicked( bool checked  )
{

    emit ShowPreForm();
}

void QGeniePlugin_Basic_Choosed_ReadyShare::changeEvent(QEvent *e)
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


void QGeniePlugin_Basic_Choosed_ReadyShare::retranslateUi()
{
    ILanguageManager *lanmanager=QGeniePlugin_Basic_Choosed_ReadyShare::sHandler->getLanguageManager();
    lblReadyShare->setText("<b>"+lanmanager->getText(L_READY_SHARE_FUN_ONE,g_readyShare_Uuid)+"</b>");

    lblDevice->setText(lanmanager->getText(L_READY_SHARE_NETWORK_NAME,g_readyShare_Uuid));
    btnBack->setText(lanmanager->getText(L_READY_SHARE_BACK,g_readyShare_Uuid));

}
