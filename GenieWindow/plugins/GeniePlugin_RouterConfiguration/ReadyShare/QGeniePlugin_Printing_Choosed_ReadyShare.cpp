#include "QGeniePlugin_Printing_Choosed_ReadyShare.h"
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

IManagerHandler* QGeniePlugin_Printing_Choosed_ReadyShare::sHandler = NULL;
QStackedLayout* QGeniePlugin_Printing_Choosed_ReadyShare::stackLayout = NULL;

QGeniePlugin_Printing_Choosed_ReadyShare::QGeniePlugin_Printing_Choosed_ReadyShare(QWidget *parent) :
    QWidget(parent)
{
    setupUi();
}

void QGeniePlugin_Printing_Choosed_ReadyShare::setupUi()
{

    strReadyShare="ReadyShare";
    strDesc="The router does not support ReadyShare at all!";

    lblReadyShare=new QLabel(strReadyShare,this,0);
    lblReadyShare->setWordWrap(false);
    SET_STYLE_PROPERTY(PARENT_LABEL1 ,lblReadyShare) ;

    lblDesc=new QLabel(this);
    lblDesc->setWordWrap(true);
    lblHow=new QLabel(this);
    lblStep1=new QLabel(this);
    lblStep1->setWordWrap(true);
    lblStep2=new QLabel(this);
    lblStep2->setWordWrap(true);
    lblStep3=new QLabel(this);
    lblStep3->setWordWrap(true);
    lblStep4=new QLabel(this);
    lblStep4->setWordWrap(true);

    lblDesc->setMinimumWidth(580);
    lblStep1->setMinimumWidth(580);
    lblStep2->setMinimumWidth(580);
    lblStep3->setMinimumWidth(580);
    lblStep4->setMinimumWidth(580);

    lblStep3->setTextFormat(Qt::RichText);
    lblStep3->setOpenExternalLinks(true);
    lblStep3->setTextInteractionFlags(Qt::LinksAccessibleByMouse);



    btnBack=new QPushButton(this);
    SET_STYLE_PROPERTY(FLEX_BUTTON,btnBack);
    btnBack->setText("Back");

    connect(btnBack,SIGNAL(clicked( bool)),this,SLOT(BtnBackClicked( bool)));

    gridAll=new QGridLayout(this);
    gridAll->setContentsMargins(30,30,20,20);
    gridAll->addWidget(lblReadyShare,0,0,1,3,Qt::AlignTop|Qt::AlignLeft);
    gridAll->addWidget(lblDesc,2,0,1,3,Qt::AlignTop|Qt::AlignLeft);
    gridAll->addWidget(lblHow,4,0,1,1,Qt::AlignTop|Qt::AlignLeft);
    gridAll->addWidget(lblStep1,6,0,1,3,Qt::AlignTop|Qt::AlignLeft);
    gridAll->addWidget(lblStep2,7,0,1,3,Qt::AlignTop|Qt::AlignLeft);
    gridAll->addWidget(lblStep3,8,0,1,3,Qt::AlignTop|Qt::AlignLeft);
    gridAll->addWidget(lblStep4,10,0,1,3,Qt::AlignTop|Qt::AlignLeft);
    gridAll->addWidget(btnBack,12,2,1,1,Qt::AlignBottom|Qt::AlignRight);

    gridAll->setRowStretch(1,1);
    gridAll->setRowStretch(3,1);
    gridAll->setRowStretch(5,1);
    gridAll->setRowStretch(9,1);
    gridAll->setRowStretch(11,1);

    gridAll->setColumnStretch(1,1);

    this->setLayout(gridAll);
    retranslateUi();
}

void QGeniePlugin_Printing_Choosed_ReadyShare::BtnContinueClicked( bool checked  )
{

}

void QGeniePlugin_Printing_Choosed_ReadyShare::BtnBackClicked( bool checked  )
{

    emit ShowPreForm();
}

void QGeniePlugin_Printing_Choosed_ReadyShare::changeEvent(QEvent *e)
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


void QGeniePlugin_Printing_Choosed_ReadyShare::retranslateUi()
{
/*

    QString str=QString("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"
                "<html>"
                    "<head>"
                        "<meta name=\"qrichtext\" content=\"1\" />"
                            "<style type=\"text/css\">"
                                "p, li { white-space: pre-wrap; }"
                            "</style>"
                    "</head>"
                    "<body style=\" font-size:10pt; \">"
                        "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
                            "%1"
                        "</p>"
                    "</body>"
                "</html>");
    */
    ILanguageManager *lanmanager=QGeniePlugin_Printing_Choosed_ReadyShare::sHandler->getLanguageManager();

    lblReadyShare->setText("<b>" + lanmanager->getText(L_READY_SHARE_PRINTING,g_readyShare_Uuid)+ "</b>");
    lblDesc->setText(lanmanager->getText(L_READY_SHARE_PRINTING_DESC,g_readyShare_Uuid));
    lblHow->setText("<b>"+lanmanager->getText(L_READY_SHARE_PRINTING_HOW_WORKS,g_readyShare_Uuid)+"</b>");
    lblStep1->setText(lanmanager->getText(L_READY_SHARE_PRINTING_STEPS_ONE,g_readyShare_Uuid));
    lblStep2->setText(lanmanager->getText(L_READY_SHARE_PRINTING_STEPS_TWO,g_readyShare_Uuid));

    QString strTextStep3=lanmanager->getText(L_READY_SHARE_PRINTING_STEPS_THREE,g_readyShare_Uuid);
    lblStep3->setText(strTextStep3);


    lblStep4->setText(lanmanager->getText(L_READY_SHARE_PRINTING_STEPS_FOUR,g_readyShare_Uuid));

    btnBack->setText(lanmanager->getText(L_READY_SHARE_BACK,g_readyShare_Uuid));

}
