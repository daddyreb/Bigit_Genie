#include "QGenieConfirmMessageBox.h"
#include "GeniePlugin_Wireless.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

QGenieConfirmMessageBox::QGenieConfirmMessageBox(QWidget *parent) :
    GeniePopupDialog(parent)
{
    m_ok_btn    = new QPushButton(this);
    m_no_btn    = new QPushButton(this);
    m_prompt_lb = new QLabel(this);

    QHBoxLayout *btn_layout = new QHBoxLayout();
    btn_layout->setSpacing(16);
    btn_layout->addStretch(1);
    btn_layout->addWidget(m_ok_btn);
    btn_layout->addWidget(m_no_btn);
    btn_layout->addStretch(1);

    QVBoxLayout *rootlayout = new QVBoxLayout(this);
    rootlayout->setContentsMargins(0,5,0,0);
    rootlayout->setSpacing(10);
    rootlayout->addWidget(m_prompt_lb);
    rootlayout->addLayout(btn_layout);

    rootlayout->setStretch(0,1);

    connect(m_ok_btn,SIGNAL(clicked()),SLOT(accept()));
    connect(m_no_btn,SIGNAL(clicked()),this,SLOT(reject()));

    retranslateUi();
}

void QGenieConfirmMessageBox::retranslateUi()
{
    m_ok_btn->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_IMB_YES_TEXT));
   m_no_btn->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_IMB_NO_TEXT));
   m_prompt_lb->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_CMB_CONTENT)/*"Be sure to delete this profile ?"*/);
   setWindowTitle(GeniePlugin_Wireless::translateText(L_WIRELESS_CMB_TITLE)/*"Delete profile"*/);
}

void QGenieConfirmMessageBox::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange)
    {
        retranslateUi();
    }

    GeniePopupDialog::changeEvent(event);
}
