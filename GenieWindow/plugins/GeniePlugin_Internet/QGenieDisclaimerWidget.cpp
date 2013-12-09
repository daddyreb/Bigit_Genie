#include "QGenieDisclaimerWidget.h"
#include "Genieplugin_Internet.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>


QGenieDisclaimerWidget::QGenieDisclaimerWidget(QWidget *parent)
    :QGenieDialogInnerWidget(parent)
{
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    prompt_label_te = new QTextEdit(this);
    ok_btn          = new QPushButton(this);

    prompt_label_te->setTextInteractionFlags(Qt::TextBrowserInteraction);
    prompt_label_te->setFocusPolicy(Qt::NoFocus);
    prompt_label_te->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    prompt_label_te->setReadOnly(true);
    prompt_label_te->setAlignment(Qt::AlignCenter);
    prompt_label_te->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    QVBoxLayout *ver_layout=new QVBoxLayout(this);
    ver_layout->setContentsMargins(2,9,2,2);
    ver_layout->setSpacing(8);
    ver_layout->addWidget(prompt_label_te);
    ver_layout->addWidget(ok_btn,0,Qt::AlignCenter);

    //setSizeIncrement(0,0);

    retranslateUi();

    connect(ok_btn,SIGNAL(clicked(bool)),this,SLOT(ok_btn_clicked(bool)));

}

QGenieDisclaimerWidget::~QGenieDisclaimerWidget()
{

}

void QGenieDisclaimerWidget::retranslateUi()
{
    prompt_label_te->setText(GeniePlugin_Internet::translateText(L_INTERNET_SPEEDTEST_DISCLAIMERWIDGET_CONTENT));
    ok_btn->setText(GeniePlugin_Internet::translateText(L_INTERNET_SPEEDTEST_DISCLAIMERWIDGET_OK_BTN_TEXT));
    this->setWindowTitle(GeniePlugin_Internet::translateText(L_INTERNET_SPEEDTEST_DISCLAIMERWIDGET_TITLE));
}

void QGenieDisclaimerWidget::changeEvent(QEvent *event)
{
    if(event->type()==QEvent::LanguageChange)
        retranslateUi();
    QGenieDialogInnerWidget::changeEvent(event);
}

void QGenieDisclaimerWidget::resizeEvent(QResizeEvent *event)
{
    setFixedSize(event->size());
}

void QGenieDisclaimerWidget::ok_btn_clicked(bool checked)
{
    accept();
}


QString QGenieDisclaimerWidget::get_dlg_title()
{
    return GeniePlugin_Internet::translateText(L_INTERNET_SPEEDTEST_DISCLAIMERWIDGET_TITLE);
}
