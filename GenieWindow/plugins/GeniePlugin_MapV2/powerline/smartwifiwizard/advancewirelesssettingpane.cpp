#include "advancewirelesssettingpane.h"
#include "ui_advancewirelesssettingpane.h"

#include "language_i18n.h"
#include <QEvent>
#include <QDesktopServices>

AdvanceWirelessSettingPane::AdvanceWirelessSettingPane(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdvanceWirelessSettingPane)
{
    ui->setupUi(this);

    connect(ui->m_advPromptLB, SIGNAL(linkActivated(QString)), SLOT(onLinkActivated(QString)));

    retranslateUi();
}

AdvanceWirelessSettingPane::~AdvanceWirelessSettingPane()
{
    delete ui;
}

void AdvanceWirelessSettingPane::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        retranslateUi();
    }

    QWidget::changeEvent(event);
}

void AdvanceWirelessSettingPane::retranslateUi()
{
    I18N_TEXT(ui->m_advGroupBox, Title, L_PLC_WIFI_WIZARD_ADV_WIRELESSSETTING_TITLE);
    I18N_TEXT(ui->m_advPromptLB, Text, L_PLC_WIFI_WIZARD_ADV_WIRELESSSETTING_TEXT);

    QString url = "<a href=\"http://%1\">%1</a> ";
    url = url.arg(m_devIP);
    QString advText = ui->m_advPromptLB->text();
    advText = advText.replace("%s", url);
    advText = advText.replace("XAVN2001", m_devModel);
    ui->m_advPromptLB->setText(advText);
}

void AdvanceWirelessSettingPane::onLinkActivated(const QString &url)
{
    QDesktopServices::openUrl(url);
}

void AdvanceWirelessSettingPane::setAdvanceSettingInfo(const QString &model, const QString &ip)
{
    m_devModel = model;
    m_devIP = ip;

    retranslateUi();
}
