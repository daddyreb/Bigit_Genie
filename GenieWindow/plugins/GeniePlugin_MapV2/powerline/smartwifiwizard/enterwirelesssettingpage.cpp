#include "enterwirelesssettingpage.h"
#include "ui_enterwirelesssettingpage.h"

#include <QEvent>

#include "language_i18n.h"

EnterWirelessSettingPage::EnterWirelessSettingPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EnterWirelessSettingPage)
{
    ui->setupUi(this);

    connect(ui->m_searchRouterPane, SIGNAL(searchRouterClicked()),
            this ,SIGNAL(searchRouterClicked()));

    connect(ui->m_wirelessSettingPane, SIGNAL(settingValidityUpdate(bool)), SLOT(onSettingValidityUpdate(bool)));

    connect(ui->m_operatePane, SIGNAL(applyClicked()), SLOT(onApplySetting()));
    connect(ui->m_operatePane, SIGNAL(cancelClicked()), this, SIGNAL(cancelClicked()));

    retranslateUi();
}

EnterWirelessSettingPage::~EnterWirelessSettingPage()
{
    delete ui;
}

void EnterWirelessSettingPage::initWlanInfo(const QString &model, const WLANInfo & wlanInfo)
{
    m_devModel = model;
    ui->m_wirelessSettingPane->initWlanInfo(wlanInfo);
    ui->m_searchRouterPane->SetDeviceModel(m_devModel);
    retranslateUi();
}

void EnterWirelessSettingPage::ControlSearchRouterUi(bool startOrStop)
{
    ui->m_searchRouterPane->ControlSearchRouterUi(startOrStop);
}

void EnterWirelessSettingPage::onSettingValidityUpdate(bool valid)
{
    ui->m_operatePane->enableApply(valid);
}

void EnterWirelessSettingPage::onApplySetting()
{
    WLANInfo wlanInfo;
    ui->m_wirelessSettingPane->retriveSetting(wlanInfo);
    emit requestApplySetting(&wlanInfo);
}

void EnterWirelessSettingPage::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        retranslateUi();
    }

    QWidget::changeEvent(event);
}

void EnterWirelessSettingPage::retranslateUi()
{
    I18N_TEXT(ui->m_groupBox, Title, L_PLC_WIFI_WIZARD_TITLE);
    I18N_TEXT(ui->m_enterSettingPromptLB, Text, L_PLC_WIFI_WIZARD_ENTER_WIRELESSSETTING);

    QString enterText = ui->m_enterSettingPromptLB->text();
    enterText.replace("%model",/*"XAVN2001"*/m_devModel);
    ui->m_enterSettingPromptLB->setText(enterText);
}
