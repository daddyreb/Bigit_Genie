#include "QGenieWelcomePage.h"
#include "GeniePlugin_WSetupPlugin.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QPixmap>
#include <QLabel>

QGenieWelcomePage::QGenieWelcomePage(QWidget *parent) :
        QFrame(parent)
{
    welcome_image_lb        = new QLabel(this);
    welcome_text_lb         = new QLabel(this);
    introduction_lb         = new QLabel(this);
    opt_prompt_lb           = new QLabel(this);
    opt_next_btn            = new QPushButton(this);
    opt_cancel_btn          = new QPushButton(this);

    welcome_text_lb->setObjectName(QString("UpdateTitle"));
    opt_prompt_lb->setObjectName(QString("UpdateIntro"));
    welcome_text_lb->setWordWrap(true);

    introduction_lb->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    introduction_lb->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    introduction_lb->setWordWrap(true);

    welcome_text_lb->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);

    welcome_image_lb->setPixmap(QPixmap(":/image/Welcome.png"));

    QVBoxLayout     *ver_layout = new QVBoxLayout();

    QSpacerItem     *spitem0 = new QSpacerItem(0,30,QSizePolicy::Minimum,QSizePolicy::Fixed);
    ver_layout->addItem(spitem0);
    ver_layout->addWidget(welcome_text_lb);
    ver_layout->addWidget(introduction_lb);
    ver_layout->addWidget(opt_prompt_lb);
    QSpacerItem       *spitem2 = new QSpacerItem(0,30,QSizePolicy::Minimum,QSizePolicy::Fixed);
    ver_layout->addItem(spitem2);

    QFrame *ver_line = new QFrame(this);
    ver_line->setFixedHeight(207);
    ver_line->setFrameShape(QFrame::VLine);
    ver_line->setFrameShadow(QFrame::Sunken);
    ver_line->setFixedWidth(2);
    ver_line->setObjectName("Splitline1");

    QHBoxLayout   *hor_layout = new QHBoxLayout();
    hor_layout->setSpacing(10);
    hor_layout->addWidget(welcome_image_lb);
    hor_layout->addWidget(ver_line);
    hor_layout->addLayout(ver_layout);

    QHBoxLayout  *opt_btn_layout = new QHBoxLayout();
#ifdef Q_OS_MAC
    opt_btn_layout->setSpacing(10);
#else
    opt_btn_layout->setSpacing(20);
#endif


    opt_btn_layout->addStretch(1);
    opt_btn_layout->addWidget(opt_next_btn);

    opt_btn_layout->addWidget(opt_cancel_btn);

    QVBoxLayout   *page_layout = new QVBoxLayout(this);
    page_layout->addLayout(hor_layout);
    page_layout->addLayout(opt_btn_layout);



    connect(opt_next_btn,SIGNAL(clicked()),this,SIGNAL(welcomepage_next_btn_clicked()));
    connect(opt_cancel_btn,SIGNAL(clicked()),this,SIGNAL(welcomepage_cancel_btn_clicked()));
    retranslateUi();
}


void QGenieWelcomePage::changeEvent(QEvent *event)
{
    if(event->type()==QEvent::LanguageChange)
    {
        retranslateUi();
    }
    QFrame::changeEvent(event);
}

void QGenieWelcomePage:: retranslateUi()
{
    welcome_text_lb->setText(/*tr("Welcome to use wireless export wizard.")*/
            GeniePlugin_WSetupPlugin::translateText(L_WIRELESSEXPORT_WELCOME_LB_TEXT));
    introduction_lb->setText(GeniePlugin_WSetupPlugin::translateText(L_WIRELESSEXPORT_INTRODUCTION_LB_TEXT));
    opt_prompt_lb->setText(/*tr("Click \"Next\" to continue.")*/
            GeniePlugin_WSetupPlugin::translateText(L_WIRELESSEXPORT_OPT_PROMPT_LB_TEXT));
    opt_next_btn->setText(/*tr("Next")*/
            GeniePlugin_WSetupPlugin::translateText(L_WIRELESSEXPORT_NEXT_BTN_TEXT));
    opt_cancel_btn->setText(GeniePlugin_WSetupPlugin::translateText(L_WIRELESSEXPORT_FDP_CANCEL_BTN_TEXT));
}
