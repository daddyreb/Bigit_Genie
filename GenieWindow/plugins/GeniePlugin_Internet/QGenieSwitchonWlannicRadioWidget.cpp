#include "QGenieSwitchonWlannicRadioWidget.h"
#include "Genieplugin_Internet.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>


QGenieSwitchonWlannicRadioWidget::QGenieSwitchonWlannicRadioWidget(QWidget *parent)
    :QGenieDialogInnerWidget(parent)
{
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    gif_movie=new QMovie(":/image/WlanNicRadioOff.gif");
    gif_label=new QLabel(this);
    gif_label->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

    prompt_label = new QLabel(this);
    ok_btn	 = new QPushButton(this);
    cancel_btn	 = new QPushButton(this);


    prompt_label->setWordWrap(true);

    QHBoxLayout	*hor_layout=new QHBoxLayout();
    QSpacerItem *sp_item=new QSpacerItem(0,0,QSizePolicy::Expanding);
    hor_layout->addItem(sp_item);
    hor_layout->addWidget(ok_btn);
    hor_layout->addWidget(cancel_btn);

    QVBoxLayout *ver_layout=new QVBoxLayout(this);
    ver_layout->addWidget(prompt_label);
    ver_layout->addWidget(gif_label);
    ver_layout->addLayout(hor_layout);

    //setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    setSizeIncrement(0,0);

    retranslateUi();

    connect(ok_btn,SIGNAL(clicked(bool)),this,SLOT(ok_btn_clicked(bool)));
    connect(cancel_btn,SIGNAL(clicked(bool)),this,SLOT(cancel_btn_clicked(bool)));

    gif_label->setMovie(gif_movie);
    gif_movie->start();

}

QGenieSwitchonWlannicRadioWidget::~QGenieSwitchonWlannicRadioWidget()
{
    delete gif_movie;
}

void QGenieSwitchonWlannicRadioWidget::retranslateUi()
{

    prompt_label->setText(GeniePlugin_Internet::translateText(L_INTERNET_SWRW_PROMPT_LB_TEXT));
    ok_btn->setText(GeniePlugin_Internet::translateText(L_INTERNET_SWRW_OK_BTN_TEXT));
    cancel_btn->setText(GeniePlugin_Internet::translateText(L_INTERNET_SWRW_CANCEL_BTN_TEXT));
    this->setWindowTitle(GeniePlugin_Internet::translateText(L_INTERNET_SWRW_TITLE));

}

void QGenieSwitchonWlannicRadioWidget::changeEvent(QEvent *event)
{
    if(event->type()==QEvent::LanguageChange)
        retranslateUi();
    /*QDialog*/QGenieDialogInnerWidget::changeEvent(event);
}

void QGenieSwitchonWlannicRadioWidget::resizeEvent(QResizeEvent *event)
{
    setFixedSize(event->size());
}

void QGenieSwitchonWlannicRadioWidget::ok_btn_clicked(bool checked)
{
    accept();
}

void QGenieSwitchonWlannicRadioWidget::cancel_btn_clicked(bool checked)
{
    reject();
}

QString QGenieSwitchonWlannicRadioWidget::get_dlg_title()
{
    return GeniePlugin_Internet::translateText(L_INTERNET_SWRW_DLG_TITLE);
}
