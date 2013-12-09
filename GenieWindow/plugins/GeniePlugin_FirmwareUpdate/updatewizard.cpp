#include <QtGui>

#include "updatewizard.h"

#include "ILanguageManager.h"
#include "LanguageFirmwareUpdate.h"

#define PLUGIN_ID "{c02fb5db-6c62-42e7-bf4a-f704247ed7b8}"

IManagerHandler *UpdateWizard::sHandler = NULL;

UpdateWizard::UpdateWizard(QWidget *parent) :
        WizardPage(parent)
{

    setTitle(tr("Update"));
    setExplain(tr("It's open your NetGear's Route Web interface When you clicked the 'Next' button."));
    NextButton->setText(tr("Update"));

//    setExplain(tr("The New Firmware!"));
    m_Widget = isSetArea();
    QVBoxLayout *mLayout = new QVBoxLayout(m_Widget);

    QHBoxLayout *viewLayout = new QHBoxLayout(m_Widget);
    viewTitle = new QLabel(m_Widget);
    viewTitle->setAlignment(Qt::AlignTop);
    viewTitle->setText(tr("Download Path:"));
    viewPath = new QLabel(m_Widget);
    viewPath->setAlignment(Qt::AlignTop);
    viewPath->setWordWrap(true);
    viewLayout->addWidget(viewTitle);
    viewLayout->addWidget(viewPath);
    viewLayout->addStretch();

    WebType = new QRadioButton(tr("turn to Internet Explorer"), m_Widget);
    WebType->click();
    SoapType = new QRadioButton(tr("turn to NETGEAR's SOAP"), m_Widget);
    SoapType->setDisabled(true);
    WebType->setHidden(true);
    SoapType->setHidden(true);

    TypeGroup = new QButtonGroup(m_Widget);
    TypeGroup->addButton(WebType, 0);
    TypeGroup->addButton(SoapType, 1);

    mLayout->addLayout(viewLayout);
    mLayout->addWidget(WebType);
    mLayout->addWidget(SoapType);


}

UpdateWizard::~UpdateWizard()
{

}

int UpdateWizard::isTypeCheck()
{
    return TypeGroup->checkedId();
}

void UpdateWizard::SetInfo(const QString &Info)
{
    viewPath->setText(Info);

//    ViewInfo->setHidden(false);
//    m_Widget->setHidden(false);

}

void UpdateWizard::changeEvent(QEvent *e)
{
    ILanguageManager *lanmanager = UpdateWizard::sHandler->getLanguageManager();
    QWidget::changeEvent(e);

    switch (e->type()){
    case QEvent::LanguageChange:
        UpdateWizard::viewTitle->setText(lanmanager->getText(L_UPGRADE_UPDATE_SAVEPATH, PLUGIN_ID));
        UpdateWizard::m_Title->setText(lanmanager->getText(L_UPGRADE_UPDATE_TITLE, PLUGIN_ID));
        UpdateWizard::m_Intro->setText(lanmanager->getText(L_UPGRADE_UPDATE_EXPLAIN, PLUGIN_ID));
        WizardPage::BackButton->setText(lanmanager->getText(L_UPGRADE_WIZARD_BACKBUT, PLUGIN_ID));
        WizardPage::NextButton->setText(lanmanager->getText(L_UPGRADE_WIZARD_NEXTBUT, PLUGIN_ID));
        WizardPage::CancelButton->setText(lanmanager->getText(L_UPGRADE_WIZARD_CANCELBUT, PLUGIN_ID));
        break;
    default:
        break;
    }

}


