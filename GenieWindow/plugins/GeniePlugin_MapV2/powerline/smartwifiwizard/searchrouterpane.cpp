#include "searchrouterpane.h"
#include "ui_searchrouterpane.h"

#include <QEvent>
#include "language_i18n.h"

SearchRouterPane::SearchRouterPane(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchRouterPane)
{
    ui->setupUi(this);
    connect(ui->m_searchRouterBtn, SIGNAL(clicked()),
            this, SIGNAL(searchRouterClicked()));

    retranslateUi();
}

SearchRouterPane::~SearchRouterPane()
{
    delete ui;
}

void SearchRouterPane::ControlSearchRouterUi(bool startOrStop)
{
    if(startOrStop){
        ui->m_searchingIndicator->startAnimation();

    }else{
        ui->m_searchingIndicator->stopAnimation();
    }

    ui->m_searchRouterBtn->setEnabled(!startOrStop);
}

void SearchRouterPane::SetDeviceModel(const QString &model)
{
    m_devModel = model;
    retranslateUi();
}

void SearchRouterPane::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange){
        retranslateUi();
    }

    QWidget::changeEvent(event);
}

void SearchRouterPane::retranslateUi()
{
    I18N_TEXT(ui->m_searchRouterPromptLB, Text, L_PLC_WIFI_WIZARD_SEARCHROUTER_TIP);
    I18N_TEXT(ui->m_searchRouterBtn, Text, L_PLC_WIFI_WIZARD_SEARCHROUTER_BTN_TEXT);

    QString tmpText = ui->m_searchRouterPromptLB->text().replace("XAVN2001", m_devModel);
    ui->m_searchRouterPromptLB->setText(tmpText);
}
