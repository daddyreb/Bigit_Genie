#include <QtGui>

#include "checkwizard.h"

#include "ILanguageManager.h"
#include "LanguageFirmwareUpdate.h"

#define PLUGIN_ID "{c02fb5db-6c62-42e7-bf4a-f704247ed7b8}"

IManagerHandler *CheckWizard::sHandler = NULL;

CheckWizard::CheckWizard(QWidget *parent) :
        WizardPage(parent)
{

    //m_CheckVersion = new FirmwareCheck();

    //setTitle(tr("Checked Firmware"));
    //setExplain(tr("Go to the NETGEAR web site to get new versions of the router software. If new version is found, you can select 'Next' to download that version."));

    m_Widget = isSetArea();
    QVBoxLayout *mLayout = new QVBoxLayout(m_Widget);

    QHBoxLayout *modeLayout = new QHBoxLayout(m_Widget);
    modelTitleLab = new QLabel(m_Widget);
    modelTitleLab->setText(tr("Router Model:"));
    modelLab = new QLabel(m_Widget);
    //modelLab->setHidden(true);
    modeLayout->addWidget(modelTitleLab);
    modeLayout->addWidget(modelLab);

    QHBoxLayout *nowLayout = new QHBoxLayout(m_Widget);
    nowTitleLab = new QLabel(m_Widget);
    nowTitleLab->setText(tr("Current Firmware Version:"));
    nowVersionLab = new QLabel(m_Widget);
    //nowVersionLab->setHidden(true);
    nowLayout->addWidget(nowTitleLab);
    nowLayout->addWidget(nowVersionLab);

    QHBoxLayout *newLayout = new QHBoxLayout(m_Widget);
    newTitleLab = new QLabel(m_Widget);
    newTitleLab->setText(tr("Lastest Firmware Version:"));
    newVersionLab = new QLabel(m_Widget);
    //newVersionLab->setHidden(true);
    newLayout->addWidget(newTitleLab);
    newLayout->addWidget(newVersionLab);

    mLayout->addLayout(modeLayout);
    mLayout->addLayout(nowLayout);
    mLayout->addLayout(newLayout);

    m_Widget->setHidden(true);

    //connect(m)
    //WizardPage::NextButton->set
    //WizardPage::connect(NextButton, SIGNAL(clicked), this, SLOT(TurnNext()));
    //connect(NextButton, SIGNAL(clicked), this, SLOT(TurnNext()));


}

CheckWizard::~CheckWizard()
{

}

void CheckWizard::SetInfo(const QStringList &Info)
{
    modelLab->setText(Info.value(0));
    nowVersionLab->setText(Info.value(1));
    newVersionLab->setText(Info.value(2));

    m_Widget->setHidden(false);

}

void CheckWizard::changeEvent(QEvent *e)
{
    ILanguageManager *lanmanager = CheckWizard::sHandler->getLanguageManager();
    QWidget::changeEvent(e);

    switch (e->type()){
    case QEvent::LanguageChange:
        CheckWizard::m_Title->setText(lanmanager->getText(L_UPGRADE_CHECK_TITLE, PLUGIN_ID));
        CheckWizard::m_Intro->setText(lanmanager->getText(L_UPGRADE_CHECK_EXPLAIN, PLUGIN_ID));
        CheckWizard::modelTitleLab->setText(lanmanager->getText(L_UPGRADE_CHECK_MODEL, PLUGIN_ID));
        CheckWizard::nowTitleLab->setText(lanmanager->getText(L_UPGRADE_CHECK_NOWVERSION, PLUGIN_ID));
        CheckWizard::newTitleLab->setText(lanmanager->getText(L_UPGRADE_CHECK_NEWVERSION, PLUGIN_ID));
        WizardPage::BackButton->setText(lanmanager->getText(L_UPGRADE_WIZARD_BACKBUT, PLUGIN_ID));
        WizardPage::NextButton->setText(lanmanager->getText(L_UPGRADE_WIZARD_NEXTBUT, PLUGIN_ID));
        WizardPage::CancelButton->setText(lanmanager->getText(L_UPGRADE_WIZARD_CANCELBUT, PLUGIN_ID));
        break;
    default:
        break;
    }

}

