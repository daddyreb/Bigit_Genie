#include "QGeniePlugin_Not_Surport_ReadyShare.h"
#include <QEvent>
#include "ILanguageManager.h"
//#include "LanguageReadyShare.h"
#include "LanguageRouterConfiguration.h"
//#include "ILanguageManager.h"

#include <QEvent>
#include "plugin_ui_def.h"
#include "IClickManager.h"
#include "IUiManager.h"
//#include "IUiManager.h"

extern QUuid g_readyShare_Uuid;

IManagerHandler *QGeniePlugin_Not_Surport_ReadyShare::sHandler = NULL;
QStackedLayout* QGeniePlugin_Not_Surport_ReadyShare::stackLayout = NULL;

QGeniePlugin_Not_Surport_ReadyShare::QGeniePlugin_Not_Surport_ReadyShare(QWidget *parent) :
    QWidget(parent)
{
    setupUi();
     btnBack->setEnabled(false);
}

void QGeniePlugin_Not_Surport_ReadyShare::setupUi()
{
    lblErrorString=new QLabel(this);
    lblErrorString->setWordWrap(false);
    lblErrorString->setStyleSheet("color: rgb(255, 0, 0);");


    grpSwitchType=new QGroupBox(this);
    //grpSwitchType->setTitle("Switch Type");
    grpSwitchType->setMinimumWidth(580);



    rdoBasic=new QRadioButton(grpSwitchType);
    rdoBasic->setText("Basic");
    rdoBasic->setChecked(true);
    rdoBasic->setFixedWidth(20);

    rdoRemote=new QRadioButton(grpSwitchType);
    rdoRemote->setChecked(false);
    rdoRemote->setFixedWidth(20);


    //rdoRemote->set
    //rdoRemote->


    rdoPrinting=new QRadioButton(grpSwitchType);
    rdoPrinting->setChecked(false);
    rdoPrinting->setFixedWidth(20);



    rdoBasic->setText("");
    rdoRemote->setText("");
    rdoPrinting->setText("");

    rdoBasic->setEnabled(false);
    rdoRemote->setEnabled(false);
    rdoPrinting->setEnabled(false);

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
   // btnContinue->setText("Continue");
    btnContinue->setEnabled(false);

    SET_STYLE_PROPERTY(FLEX_BUTTON,btnContinue);
    //connect(btnContinue,SIGNAL(clicked( bool)),this,SLOT(BtnContinueClicked( bool)));


    btnBack=new QPushButton(this);
    SET_STYLE_PROPERTY(FLEX_BUTTON,btnBack);
   // btnBack->setText("Back");
    btnBack->setEnabled(false);



    //connect(btnBack,SIGNAL(clicked( bool)),this,SLOT(BtnBackClicked( bool)));


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

void QGeniePlugin_Not_Surport_ReadyShare::BtnBackClicked( bool checked  )
{
    IUiManager *mUiPage = QGeniePlugin_Not_Surport_ReadyShare::sHandler->getUiManager();
    Q_ASSERT(mUiPage);
    mUiPage->changeStackIdx(0);
}


void QGeniePlugin_Not_Surport_ReadyShare::changeEvent(QEvent *e)
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


void QGeniePlugin_Not_Surport_ReadyShare::retranslateUi()
{
    ILanguageManager *lanmanager=QGeniePlugin_Not_Surport_ReadyShare::sHandler->getLanguageManager();
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
    lblErrorString->setText(" ");

}
