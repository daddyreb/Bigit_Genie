#include "QGeniePluginCableWidget.h"
#include "plugin_ui_def.h"
//#include "Genieplugin_Internet.h"
#include "LanguageParentalControl.h"
#include "pcpform.h"
#include "ILanguageManager.h"
#include <QUuid>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>


QGeniePluginCableWidget::QGeniePluginCableWidget(QWidget *parent)
    :/*QDialog*/QGenieDialogInnerWidget(parent)
{
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    gif_movie=new QMovie(":/image/PluginCable.gif");
    gif_label=new QLabel(this);
    gif_label->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

    prompt_label=new QLabel(this);
    ok_btn	=new QPushButton(this);
    cancel_btn	=new QPushButton(this);

    prompt_label->setWordWrap(true);
    //ok_btn->setFocusPolicy(Qt::NoFocus);
    //cancel_btn->setFocusPolicy(Qt::NoFocus);

    SET_FIX_SIZE(NORMAL_BUTTON,ok_btn);
    SET_FIX_SIZE(NORMAL_BUTTON,cancel_btn);

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

QGeniePluginCableWidget::~QGeniePluginCableWidget()
{
    delete gif_movie;
}

void QGeniePluginCableWidget::retranslateUi()
{


   // prompt_label->setText(/*tr("The network cable is not plugged in ,please plug it in first.")*/
   //         GeniePlugin_Internet::translateText(L_INTERNET_PLUG_PROMPT_LB_TEXT));
  //  ok_btn->setText(/*tr("Try")*/
   //         GeniePlugin_Internet::translateText(L_INTERNET_PCW_OK_BTN_TEXT));
   // cancel_btn->setText(/*tr("Cancel")*/
   //         GeniePlugin_Internet::translateText(L_INTERNET_PCW_CANCEL_BTN_TEXT));
  //  this->setWindowTitle(/*tr("Plug in cable")*/
   //         GeniePlugin_Internet::translateText(L_INTERNET_PCW_TITLE));

    /*
        L_PARENTAL_CONTROL_MSG_TITLE,
        L_PARENTAL_CONTROL_MSG_DESC,
        L_PARENTAL_CONTROL_MSG_OK,
        L_PARENTAL_CONTROL_MSG_CANCEL
    */
    /*
    ILanguageManager *lanmanager=PCPForm::sHandler->getLanguageManager();
    prompt_label->setText(lanmanager->getText(L_PARENTAL_CONTROL_MSG_DESC,QUuid("{ce944b2a-342f-49ca-b66b-b08766ad6599}")));
    ok_btn->setText(lanmanager->getText(L_PARENTAL_CONTROL_MSG_OK,QUuid("{ce944b2a-342f-49ca-b66b-b08766ad6599}")));
    cancel_btn->setText(lanmanager->getText(L_PARENTAL_CONTROL_MSG_CANCEL,QUuid("{ce944b2a-342f-49ca-b66b-b08766ad6599}")));
    this->setWindowTitle(lanmanager->getText(L_PARENTAL_CONTROL_MSG_TITLE,QUuid("{ce944b2a-342f-49ca-b66b-b08766ad6599}")));
    */
}

void QGeniePluginCableWidget::changeEvent(QEvent *event)
{
    if(event->type()==QEvent::LanguageChange)
        retranslateUi();
    /*QDialog*/QGenieDialogInnerWidget::changeEvent(event);
           }

void QGeniePluginCableWidget::resizeEvent(QResizeEvent *event)
{
    setFixedSize(event->size());
}

void QGeniePluginCableWidget::ok_btn_clicked(bool checked)
{
    accept();
}

void QGeniePluginCableWidget::cancel_btn_clicked(bool checked)
{
    reject();
}

QString QGeniePluginCableWidget::get_dlg_title()
{
   // return GeniePlugin_Internet::translateText(L_INTERNET_PCW_TITLE);
    return "";
}
