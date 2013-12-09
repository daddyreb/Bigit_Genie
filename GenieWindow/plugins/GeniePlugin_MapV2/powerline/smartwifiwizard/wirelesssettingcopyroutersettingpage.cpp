#include "wirelesssettingcopyroutersettingpage.h"
#include "ui_wirelesssettingcopyroutersettingpage.h"

#include "language_i18n.h"
#include <QEvent>

WirelessSettingCopyRouterSettingPage::WirelessSettingCopyRouterSettingPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WirelessSettingCopyRouterSettingPage)
{
    ui->setupUi(this);
    connect(ui->m_routerSettingPane, SIGNAL(copyRouterSettings()),SLOT(onCopyRouterSettings()));

    connect(ui->m_wirelessSettingPane, SIGNAL(settingValidityUpdate(bool)), SLOT(onSettingValidityUpdate(bool)));

    connect(ui->m_operatePane, SIGNAL(applyClicked()), SLOT(onApplySetting()));
    connect(ui->m_operatePane, SIGNAL(cancelClicked()), this , SIGNAL(cancelClicked()));

    retranslateUi();
}

WirelessSettingCopyRouterSettingPage::~WirelessSettingCopyRouterSettingPage()
{
    delete ui;
}

void WirelessSettingCopyRouterSettingPage::initWlanInfo(const QString &model, const WLANInfo & wlanInfo)
{
    m_devModel = model;
    retranslateUi();
    ui->m_wirelessSettingPane->initWlanInfo(wlanInfo);
}

void WirelessSettingCopyRouterSettingPage::initRouterSettings(const WLANInfo &wlanInfo)
{
    m_routerSettings = wlanInfo;
    ui->m_routerSettingPane->initRouterSettings(m_routerSettings);
}

void WirelessSettingCopyRouterSettingPage::onCopyRouterSettings()
{
    ui->m_wirelessSettingPane->initWlanInfo(m_routerSettings);
}

void WirelessSettingCopyRouterSettingPage::onSettingValidityUpdate(bool valid)
{
    ui->m_operatePane->enableApply(valid);
}

void WirelessSettingCopyRouterSettingPage::onApplySetting()
{
    WLANInfo wlanInfo;
    ui->m_wirelessSettingPane->retriveSetting(wlanInfo);
    emit requestApplySetting(&wlanInfo);
}

void WirelessSettingCopyRouterSettingPage::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        retranslateUi();
    }

    QWidget::changeEvent(event);
}

void WirelessSettingCopyRouterSettingPage::retranslateUi()
{
    I18N_TEXT(ui->m_copyGB, Title, L_PLC_WIFI_WIZARD_TITLE);
    I18N_TEXT(ui->m_plcGB, Title, L_PLC_WIFI_WIZARD_XAVN2001_SETTING);
    I18N_TEXT(ui->m_routerGB, Title, L_PLC_WIFI_WIZARD_YOUR_ROUTER_SETTING);
    I18N_TEXT(ui->m_routerFounedLB, Text, L_PLC_WIFI_WIZARD_NETGEARROUTER_FOUNDED_PROMPT_TEXT);

    QString tmpText = ui->m_plcGB->title();
    ui->m_plcGB->setTitle(tmpText.replace("XAVN2001", m_devModel));

    tmpText = ui->m_routerGB->title();
    ui->m_routerGB->setTitle(tmpText.replace("XAVN2001", m_devModel));

    tmpText = ui->m_routerFounedLB->text();
    ui->m_routerFounedLB->setText(tmpText.replace("XAVN2001", m_devModel));

}
