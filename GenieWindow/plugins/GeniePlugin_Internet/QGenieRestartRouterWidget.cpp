#include "QGenieRestartRouterWidget.h"
#include "Genieplugin_Internet.h"
#include "language_Internet.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>


QGenieRestartRouterWidget::QGenieRestartRouterWidget(QWidget *parent)
    :QGenieDialogInnerWidget(parent),
    prompt_text_idx(L_INTERNET_RT_RESTARTROUTER_PROMPT_CONFIRM_TEXT)
{
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    prompt_label        = new QLabel(this);
    ok_btn              = new QPushButton(this);
    cancel_btn          = new QPushButton(this);
    m_progress_panel    = new QGenieProgressPanel(this);
    m_progress_panel->setFixedSize(25,25);
    m_progress_panel->setVisible(false);

    prompt_label->setAlignment(Qt::AlignCenter);
    prompt_label->setWordWrap(true);
    prompt_label->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);

    QHBoxLayout	*hor_layout = new QHBoxLayout();
    QSpacerItem *sp_item = new QSpacerItem(0,0,QSizePolicy::Expanding);
    hor_layout->addItem(sp_item);
    hor_layout->addWidget(ok_btn);
    hor_layout->addWidget(cancel_btn);

    QVBoxLayout *ver_layout2 = new QVBoxLayout();
    ver_layout2->setAlignment(m_progress_panel,Qt::AlignCenter);
    ver_layout2->addWidget(m_progress_panel);
    ver_layout2->addWidget(prompt_label);
    ver_layout2->setAlignment(m_progress_panel,Qt::AlignCenter);

    QVBoxLayout *ver_layout = new QVBoxLayout(this);
    ver_layout->addStretch(1);
    ver_layout->addLayout(ver_layout2);
    ver_layout->addStretch(1);
    ver_layout->addLayout(hor_layout);

    setSizeIncrement(0,0);

    retranslateUi();

    connect(ok_btn,SIGNAL(clicked(bool)),this,SLOT(ok_btn_clicked(bool)));
    connect(cancel_btn,SIGNAL(clicked(bool)),this,SLOT(cancel_btn_clicked(bool)));

}

QGenieRestartRouterWidget::~QGenieRestartRouterWidget()
{
    m_progress_panel->stop();
}

void QGenieRestartRouterWidget::retranslateUi()
{

    prompt_label->setText(GeniePlugin_Internet::translateText(prompt_text_idx));
    ok_btn->setText(GeniePlugin_Internet::translateText(L_INTERNET_IMB_YES_TEXT));
    cancel_btn->setText(GeniePlugin_Internet::translateText(L_INTERNET_IMB_NO_TEXT));
    this->setWindowTitle(GeniePlugin_Internet::translateText(L_INTERNET_RT_RESTARTROUTER_DLG_TITLE));

}

void QGenieRestartRouterWidget::changeEvent(QEvent *event)
{
    if(event->type()==QEvent::LanguageChange)
        retranslateUi();
    QGenieDialogInnerWidget::changeEvent(event);
}

void QGenieRestartRouterWidget::resizeEvent(QResizeEvent *event)
{
    setFixedSize(event->size());
}

void QGenieRestartRouterWidget::ok_btn_clicked(bool checked)
{
    ok_btn->setEnabled(false);
    cancel_btn->setEnabled(false);

    prompt_text_idx = L_INTERNET_RT_RESTARTROUTER_PROMPT_RESTARTINGROUTER;
    retranslateUi();
    m_progress_panel->setVisible(true);
    m_progress_panel->start();
    emit show_restartrouter_dlg_result(1);
}

void QGenieRestartRouterWidget::cancel_btn_clicked(bool checked)
{
    emit show_restartrouter_dlg_result(0);
    reject();
}

void QGenieRestartRouterWidget::end_showdlg()
{
    reject();
}

QString QGenieRestartRouterWidget::get_dlg_title()
{
    return GeniePlugin_Internet::translateText(L_INTERNET_RT_RESTARTROUTER_DLG_TITLE);
}
