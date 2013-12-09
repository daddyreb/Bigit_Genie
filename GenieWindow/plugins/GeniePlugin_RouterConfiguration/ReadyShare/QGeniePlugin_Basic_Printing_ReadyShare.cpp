#include "QGeniePlugin_Basic_Printing_ReadyShare.h"
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

IManagerHandler* QGeniePlugin_Basic_Printing_ReadyShare::sHandler = NULL;
QStackedLayout* QGeniePlugin_Basic_Printing_ReadyShare::stackLayout = NULL;

QGeniePlugin_Basic_Printing_ReadyShare::QGeniePlugin_Basic_Printing_ReadyShare(QWidget *parent) :
    QWidget(parent)
{
    setupUi();
     btnBack->setEnabled(false);
}

void QGeniePlugin_Basic_Printing_ReadyShare::setupUi()
{

    strReadyShare="ReadyShare";
    strDesc="The router does not support ReadyShare at all!";

   // lblReadyShare=new QLabel(strReadyShare,this,0);
   // lblReadyShare->setWordWrap(false);
   // SET_STYLE_PROPERTY(PARENT_LABEL1 ,lblReadyShare) ;
    grpSwitchType=new QGroupBox(this);

    grpSwitchType->setMinimumWidth(580);

    //grpSwitchType->setTitle("Switch Type");



    rdoBasic=new QRadioButton(grpSwitchType);
    rdoBasic->setText("Basic");
    rdoBasic->setChecked(true);
    rdoBasic->setFixedWidth(20);



    rdoPrinting=new QRadioButton(grpSwitchType);
    rdoPrinting->setChecked(false);
    rdoPrinting->setFixedWidth(20);



    rdoBasic->setText("");

    rdoPrinting->setText("");

    txtBasic=new     QLabel(grpSwitchType);
    txtBasic->setWordWrap(true);
    txtBasic->setFixedWidth(550);
    //txtBasic->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);



    txtPrinting=new     QLabel(grpSwitchType);
    txtPrinting->setWordWrap(true);
    txtPrinting->setFixedWidth(550);
   // txtPrinting->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);


    QGridLayout *vbox = new QGridLayout(this);

    vbox->addWidget(rdoBasic,0,0,1,1,Qt::AlignLeft|Qt::AlignVCenter);

    vbox->addWidget(rdoPrinting,1,0,1,1,Qt::AlignLeft|Qt::AlignVCenter);


    vbox->addWidget(txtBasic,0,1,1,1,Qt::AlignLeft|Qt::AlignVCenter);

    vbox->addWidget(txtPrinting,1,1,1,1,Qt::AlignLeft|Qt::AlignVCenter);


    vbox->setColumnStretch(1,1);


    grpSwitchType->setLayout(vbox);

    btnContinue=new QPushButton(this);
    btnContinue->setText("Continue");
    SET_STYLE_PROPERTY(FLEX_BUTTON,btnContinue);


    btnBack=new QPushButton(this);
    SET_STYLE_PROPERTY(FLEX_BUTTON,btnBack);
    btnBack->setText("Back");

    connect(btnBack,SIGNAL(clicked( bool)),this,SLOT(BtnBackClicked( bool)));
    connect(btnContinue,SIGNAL(clicked( bool)),this,SLOT(BtnContinueClicked( bool)));


    gridAll=new QGridLayout(this);

    gridAll->setContentsMargins(30,30,20,20);

  //  gridAll->addWidget(lblReadyShare,0,0,1,1,Qt::AlignTop|Qt::AlignLeft);

    gridAll->addWidget(grpSwitchType,1,0,1,4,Qt::AlignLeft);


    gridAll->addWidget(btnContinue,3,2,1,1,Qt::AlignBottom|Qt::AlignRight);

    gridAll->addWidget(btnBack,3,3,1,1,Qt::AlignBottom|Qt::AlignRight);

    gridAll->setRowStretch(2,280);
    gridAll->setColumnStretch(1,280);

    this->setLayout(gridAll);

    retranslateUi();
}

void QGeniePlugin_Basic_Printing_ReadyShare::BtnContinueClicked( bool checked  )
{
    if (rdoPrinting->isChecked()==true)
    {
        stackLayout->setCurrentIndex(7);
    }

    if (rdoBasic->isChecked()==true)
    {
        stackLayout->setCurrentIndex(5);
    }



    emit preIndexSend(3);
}

void QGeniePlugin_Basic_Printing_ReadyShare::BtnBackClicked( bool checked  )
{
    IUiManager *mUiPage = QGeniePlugin_Basic_Printing_ReadyShare::sHandler->getUiManager();
    Q_ASSERT(mUiPage);
    mUiPage->changeStackIdx(0);
}

void QGeniePlugin_Basic_Printing_ReadyShare::changeEvent(QEvent *e)
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


void QGeniePlugin_Basic_Printing_ReadyShare::retranslateUi()
{
    ILanguageManager *lanmanager=QGeniePlugin_Basic_Printing_ReadyShare::sHandler->getLanguageManager();
   // lblReadyShare->setText(lanmanager->getText(L_READY_SHARE_LIVE,g_readyShare_Uuid));
    grpSwitchType->setTitle(lanmanager->getText(L_READY_SHARE_SWITCH_TYPE,g_readyShare_Uuid));
    txtBasic->setText(lanmanager->getText(L_READY_SHARE_FUN_ONE,g_readyShare_Uuid));
    txtPrinting->setText(lanmanager->getText(L_READY_SHARE_FUN_THREE,g_readyShare_Uuid));
    btnBack->setText(lanmanager->getText(L_READY_SHARE_BACK,g_readyShare_Uuid));
    btnContinue->setText(lanmanager->getText(L_READY_SHARE_CONTINUE,g_readyShare_Uuid));
}
