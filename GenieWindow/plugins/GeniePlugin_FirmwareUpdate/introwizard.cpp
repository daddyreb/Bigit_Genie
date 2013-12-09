#include <QtGui>

#include "introwizard.h"

#include "ILanguageManager.h"
#include "LanguageFirmwareUpdate.h"
#include "IInfoManager.h"

#define PLUGIN_ID "{c02fb5db-6c62-42e7-bf4a-f704247ed7b8}"


IManagerHandler *IntroWizard::sHandler = NULL;

IntroWizard::IntroWizard(QWidget *parent) :
        WizardPage(parent)
{
    lanmanager = IntroWizard::sHandler->getLanguageManager();

    //intTitle = L_UPGRADE_INTRO_TITLE;
    //intExplain = L_UPGRADE_INTRO_EXPLAIN;
    //setTitle(tr("Introduction"));
    //setExplain(tr("You install new versions of the router's software using the Router Upgrade page."));


//    IInfoManager *mInfoManager = IntroWizard::sHandler->getInfoManager();
//    Q_ASSERT(mInfoManager);
//    routerType.clear();
//    routerType = mInfoManager->getInfo(INFO_CMD_ROUTE_TYPE_IMMEDIATE);
//    if (routerType.isEmpty()){
//        intTitle = L_UPGRADE_NOTNETGEAR_TITLE;
//        intExplain = L_UPGRADE_NOTNETGEAR_EXPLAIN;
//
//        NextButton->setDisabled(true);
//        CancelButton->setDisabled(true);
//    }else {
//        intTitle = L_UPGRADE_INTRO_TITLE;
//        intExplain = L_UPGRADE_INTRO_EXPLAIN;
//
//        CancelButton->setDisabled(true);
//    }



    BackButton->setDisabled(true);
//    //CancelButton->setHidden(true);
    CancelButton->setDisabled(true);
//    //SpaceButt->setHidden(false);
    NextButton->setDisabled(true);

    m_Widget = isSetArea();
    QVBoxLayout *vLayout = new QVBoxLayout(m_Widget);
    QHBoxLayout *mLayout = new QHBoxLayout(m_Widget);

    mProgress = new QProgressIndicator(m_Widget);
    //mProgress->se
    //mProgress->startAnimation();
    //mProgress->setAnimationDelay(50);
    mProgress->setHidden(true);
    mLayout->addStretch();
    mLayout->addWidget(mProgress);
    mLayout->addStretch();


    vPrompt = new QLabel(m_Widget);
    vPrompt->setAlignment(Qt::AlignHCenter);
    //vPrompt->setText(tr("Please Wait a few seconds..."));
    vPrompt->setHidden(true);

    vTimeOut = new QLabel(m_Widget);
    vTimeOut->setAlignment(Qt::AlignHCenter);
    //vPrompt->setText(tr("Please Wait a few seconds..."));
    vTimeOut->setHidden(true);

    vLayout->addSpacing(20);
    vLayout->addLayout(mLayout);
    vLayout->addWidget(vPrompt);
    vLayout->addWidget(vTimeOut);
    vLayout->addStretch();

    FirstSetForm();

}

IntroWizard::~IntroWizard()
{

}

void IntroWizard::SetInfo(const QString &Info)
{
    vTimeOut->clear();
    vTimeOut->setText(Info);

    vTimeOut->setHidden(false);

}

void IntroWizard::SetInfo(const int &intInfo)
{
    mExplain = intInfo;

    vTimeOut->clear();
    vTimeOut->setText(lanmanager->getText(mExplain, PLUGIN_ID));

    vTimeOut->setHidden(false);

}

void IntroWizard::ClearInfo()
{
    vTimeOut->clear();

    vTimeOut->setHidden(true);
}

void IntroWizard::FirstSetForm()
{
    intTitle = L_UPGRADE_NOTNETGEAR_TITLE;
    intExplain = L_UPGRADE_NOTNETGEAR_EXPLAIN;

    NextButton->setDisabled(true);
    CancelButton->setDisabled(true);

}

void IntroWizard::SetTitle(QString &type, bool setbtn)
{
    qDebug() << "Netgear router: " << type;
    if (type.isEmpty()){
        intTitle = L_UPGRADE_NOTNETGEAR_TITLE;
        intExplain = L_UPGRADE_NOTNETGEAR_EXPLAIN;

        NextButton->setDisabled(true);
        CancelButton->setDisabled(true);
    }else {
        intTitle = L_UPGRADE_INTRO_TITLE;
        //intTitle = L_UPGRADE_SUCCESS_TITLE;
        intExplain = L_UPGRADE_INTRO_EXPLAIN;

        if (setbtn){
            NextButton->setDisabled(false);
            CancelButton->setDisabled(true);
        }
    }

    IntroWizard::m_Title->setText(lanmanager->getText(intTitle, PLUGIN_ID));
    IntroWizard::m_Intro->setText(lanmanager->getText(intExplain, PLUGIN_ID));

}

void IntroWizard::openView()
{
    mProgress->startAnimation();
    mProgress->setAnimationDelay(50);

    mProgress->setHidden(false);
    vPrompt->setHidden(false);
}

void IntroWizard::closeView()
{
    mProgress->stopAnimation();

    mProgress->setHidden(true);
    vPrompt->setHidden(true);

    vTimeOut->clear();
    vTimeOut->setHidden(true);
}

void IntroWizard::isCancelBtn(bool isFlag)
{
    CancelButton->setDisabled(isFlag);
}

void IntroWizard::changeEvent(QEvent *e)
{
    //ILanguageManager *lanmanager = IntroWizard::sHandler->getLanguageManager();
    QWidget::changeEvent(e);

    switch (e->type()){
    case QEvent::LanguageChange:
        vTimeOut->setText(lanmanager->getText(mExplain, PLUGIN_ID));
        IntroWizard::m_Title->setText(lanmanager->getText(intTitle, PLUGIN_ID));
        IntroWizard::m_Intro->setText(lanmanager->getText(intExplain, PLUGIN_ID));
        IntroWizard::vPrompt->setText(lanmanager->getText(L_UPGRADE_INTRO_WAIT, PLUGIN_ID));
        WizardPage::BackButton->setText(lanmanager->getText(L_UPGRADE_WIZARD_BACKBUT, PLUGIN_ID));
        WizardPage::NextButton->setText(lanmanager->getText(L_UPGRADE_WIZARD_NEXTBUT, PLUGIN_ID));
        WizardPage::CancelButton->setText(lanmanager->getText(L_UPGRADE_WIZARD_CANCELBUT, PLUGIN_ID));
        break;
    default:
        break;
    }

}

