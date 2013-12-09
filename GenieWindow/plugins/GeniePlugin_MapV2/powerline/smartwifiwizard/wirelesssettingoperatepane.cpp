#include "wirelesssettingoperatepane.h"
#include "ui_wirelesssettingoperatepane.h"

#include "language_i18n.h"
#include <QEvent>

WirelessSettingOperatePane::WirelessSettingOperatePane(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WirelessSettingOperatePane)
{
    ui->setupUi(this);

    connect(ui->m_applyBtn, SIGNAL(clicked()), this , SIGNAL(applyClicked()));
    connect(ui->m_cancelBtn, SIGNAL(clicked()), this, SIGNAL(cancelClicked()));

    retranslateUi();
}

WirelessSettingOperatePane::~WirelessSettingOperatePane()
{
    delete ui;
}


void WirelessSettingOperatePane::enableApply(bool enable)
{
    ui->m_applyBtn->setEnabled(enable);
}

void WirelessSettingOperatePane::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        retranslateUi();
    }

    QWidget::changeEvent(event);
}

void WirelessSettingOperatePane::retranslateUi()
{
    I18N_TEXT(ui->m_applyBtn, Text, L_PLC_WIFI_WIZARD_APPLY);
    I18N_TEXT(ui->m_cancelBtn, Text, L_PLC_WIFI_WIZARD_CANCEL);
}
