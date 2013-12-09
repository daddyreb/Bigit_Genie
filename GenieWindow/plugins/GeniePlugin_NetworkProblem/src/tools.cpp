#include "tools.h"
#include "mainwindow.h"

Tools::Tools(QWidget* parent)
    :QWidget(parent)
{
    this->setProperty("stylehintplugin",5);

    gridLayout_4 = new QGridLayout(this);
    gridLayout_4->setContentsMargins(0, 0, 0, 0);
    gridLayout_4->setSpacing(0);

    frame = new QFrame(this);
    frame->setFrameShape(QFrame::NoFrame);
    frame->setFrameShadow(QFrame::Plain);

    gridLayout = new QGridLayout(frame);
    gridLayout->setContentsMargins(20, 30, 0, 15);
    gridLayout->setHorizontalSpacing(0);

    label = new QLabel(frame);
    QFont font;
    font.setPointSize(10);
    font.setBold(true);
    font.setWeight(75);
    label->setFont(font);
    label->setWordWrap(true);
    
    gridLayout->addWidget(label, 1, 0, 1, 1);

    gridLayout_4->addWidget(frame, 0, 0, 1, 2);

    int framLenght = 210;
    int btnLenght = 191;
    frame_2 = new QFrame(this);
    frame_2->setMinimumSize(QSize(framLenght, 0));
    frame_2->setMaximumSize(QSize(framLenght, 16777215));
    //frame_2->setMinimumSize(QSize(210, 0));
    //frame_2->setMaximumSize(QSize(210, 16777215));
    frame_2->setFrameShape(QFrame::NoFrame);
    frame_2->setFrameShadow(QFrame::Plain);

    gridLayout_2 = new QGridLayout(frame_2);
    gridLayout_2->setVerticalSpacing(20);
    gridLayout_2->setContentsMargins(15, 0, 15, 0);

    verticalSpacer = new QSpacerItem(93, 129, QSizePolicy::Minimum, QSizePolicy::Expanding);
    
    gridLayout_2->addItem(verticalSpacer, 0, 1, 1, 1);
    
    pushButton = new PushButton(frame_2);
    //pushButton->setMaximumSize(btnLenght,34);
    //pushButton->setMinimumSize(btnLenght,34);
    pushButton->setFocusPolicy(Qt::NoFocus);

//    SET_FIX_SIZE(GRADIENT_BUTTON,pushButton);

    gridLayout_2->addWidget(pushButton, 1, 0, 1, 2);
    
    pushButton_2 = new PushButton(frame_2);
    //pushButton_2->setMaximumSize(btnLenght,34);
    //pushButton_2->setMinimumSize(btnLenght,34);
    pushButton_2->setFocusPolicy(Qt::NoFocus);

//    SET_FIX_SIZE(GRADIENT_BUTTON,pushButton_2);
    
    gridLayout_2->addWidget(pushButton_2, 2, 0, 1, 2);
    
    pushButton_3 = new PushButton(frame_2);
    //pushButton_3->setMaximumSize(191,34);
    //pushButton_3->setMinimumSize(191,34);
    //pushButton_3->setMaximumSize(btnLenght,34);
    //pushButton_3->setMinimumSize(btnLenght,34);
    pushButton_3->setFocusPolicy(Qt::NoFocus);

//    SET_FIX_SIZE(GRADIENT_BUTTON,pushButton_3);
    
    gridLayout_2->addWidget(pushButton_3, 3, 0, 1, 2);
    
    pushButton_4 = new PushButton(frame_2);
    //pushButton_4->setMaximumSize(btnLenght,34);
    //pushButton_4->setMinimumSize(btnLenght,34);
    pushButton_4->setFocusPolicy(Qt::NoFocus);

//    SET_FIX_SIZE(GRADIENT_BUTTON,pushButton_4);
    
	/**************2012-3-20 add*******************/
	pushButton->setProperty("toolsBtn", 1);
	pushButton_2->setProperty("toolsBtn", 1);
	pushButton_3->setProperty("toolsBtn", 1);
	pushButton_4->setProperty("toolsBtn", 1);
	/*******************END************************/

    gridLayout_2->addWidget(pushButton_4, 4, 0, 1, 2);
    
    verticalSpacer_2 = new QSpacerItem(93, 128, QSizePolicy::Minimum, QSizePolicy::Expanding);
    
    gridLayout_2->addItem(verticalSpacer_2, 5, 0, 1, 1);
    
    gridLayout_4->addWidget(frame_2, 1, 0, 1, 1);
    
    frame_3 = new QFrame(this);
    frame_3->setFrameShape(QFrame::NoFrame);
    frame_3->setFrameShadow(QFrame::Plain);


    stackedLayout = new QStackedLayout(frame_3);
    ping = new Ping(frame_3);
    route = new TraceRoute(frame_3);
    dnslookup = new DNSLookup(frame_3);
    pro = new Profile(frame_3);


    stackedLayout->addWidget(ping);
    stackedLayout->addWidget(route);
    stackedLayout->addWidget(dnslookup);
    stackedLayout->addWidget(pro);

    gridLayout_4->addWidget(frame_3, 1, 1, 1, 1);

    uuid ="{5ddd2596-5211-4d83-980b-71f3516d0b5c}";

    label->setText(QString(tr("If you would like to check the quality of your Internet Connection these tools may be of assistance.")));
    pushButton->setText(QString(tr("Ping")));
    pushButton_2->setText(QString(tr("Trace Route")));
    pushButton_3->setText(QString(tr("DNS Lookup")));
    pushButton_4->setText(QString(tr("Computer Profile")));

    connect(pushButton,SIGNAL(clicked()),this,SLOT(pingchange()));
    connect(pushButton_2,SIGNAL(clicked()),this,SLOT(routechange()));
    connect(pushButton_3,SIGNAL(clicked()),this,SLOT(dnschange()));
    connect(pushButton_4,SIGNAL(clicked()),this,SLOT(profilechange()));
    connect(this,SIGNAL(getinfo()),pro,SLOT(GetSysInfo()));

    retranslateUi();

    pingchange();
}
Tools::~Tools()
{

}
void Tools::changeEvent(QEvent* event)
{
    QWidget::changeEvent(event);
    switch(event->type())
    {
        case QEvent::LanguageChange:
            retranslateUi();
            break;
        default:
                break;
    }
}
void Tools::retranslateUi()
{
    label->setText(GENIE2_GET_TEXT(L_NETWORKPROBLEM_TOOLS_TITLE));//lanmanager->getText(L_NETWORKPROBLEM_TOOLS_TITLE,uuid));
    pushButton->setText(GENIE2_GET_TEXT(L_NETWORKPROBLEM_TOOLS_BTNPING));//lanmanager->getText(L_NETWORKPROBLEM_TOOLS_BTNPING,uuid));
    pushButton_2->setText(GENIE2_GET_TEXT(L_NETWORKPROBLEM_TOOLS_BTNROUTE));//lanmanager->getText(L_NETWORKPROBLEM_TOOLS_BTNROUTE,uuid));
    pushButton_3->setText(GENIE2_GET_TEXT(L_NETWORKPROBLEM_TOOLS_BTNDNS));//lanmanager->getText(L_NETWORKPROBLEM_TOOLS_BTNDNS,uuid));
    pushButton_4->setText(GENIE2_GET_TEXT(L_NETWORKPROBLEM_TOOLS_BTNPROFILE));//lanmanager->getText(L_NETWORKPROBLEM_TOOLS_BTNPROFILE,uuid));
}
void Tools::pingchange()
{
    stackedLayout->setCurrentWidget(ping);
    pushButton->setCheckable(true);
    pushButton->setChecked(true);
    pushButton_2->setChecked(false);
    pushButton_3->setChecked(false);
    pushButton_4->setChecked(false);
    pushButton->setStyleSheet("border-image:url(:/images/plugin_gradient_but_hover.png);");
    pushButton_2->setStyleSheet("border-image:url(:/images/plugin_gradient_but.png);");
    pushButton_3->setStyleSheet("border-image:url(:/images/plugin_gradient_but.png);");
    pushButton_4->setStyleSheet("border-image:url(:/images/plugin_gradient_but.png);");
}
void Tools::routechange()
{
    stackedLayout->setCurrentWidget(route);
    pushButton_2->setCheckable(true);
    pushButton_2->setChecked(true);
    pushButton->setChecked(false);
    pushButton_3->setChecked(false);
    pushButton_4->setChecked(false);
    pushButton->setStyleSheet("border-image:url(:/images/plugin_gradient_but.png);");
    pushButton_2->setStyleSheet("border-image:url(:/images/plugin_gradient_but_hover.png);");
    pushButton_3->setStyleSheet("border-image:url(:/images/plugin_gradient_but.png);");
    pushButton_4->setStyleSheet("border-image:url(:/images/plugin_gradient_but.png);");
}
void Tools::dnschange()
{
    stackedLayout->setCurrentWidget(dnslookup);
    pushButton_3->setCheckable(true);
    pushButton_3->setChecked(true);
    pushButton_2->setChecked(false);
    pushButton->setChecked(false);
    pushButton_4->setChecked(false);
    pushButton->setStyleSheet("border-image:url(:/images/plugin_gradient_but.png);");
    pushButton_2->setStyleSheet("border-image:url(:/images/plugin_gradient_but.png);");
    pushButton_3->setStyleSheet("border-image:url(:/images/plugin_gradient_but_hover.png);");
    pushButton_4->setStyleSheet("border-image:url(:/images/plugin_gradient_but.png);");
}
void Tools::profilechange()
{
    stackedLayout->setCurrentWidget(pro);
    emit getinfo();
    pushButton_4->setCheckable(true);
    pushButton_4->setChecked(true);
    pushButton_2->setChecked(false);
    pushButton_3->setChecked(false);
    pushButton->setChecked(false);
    pushButton->setStyleSheet("border-image:url(:/images/plugin_gradient_but.png);");
    pushButton_2->setStyleSheet("border-image:url(:/images/plugin_gradient_but.png);");
    pushButton_3->setStyleSheet("border-image:url(:/images/plugin_gradient_but.png);");
    pushButton_4->setStyleSheet("border-image:url(:/images/plugin_gradient_but_hover.png);");
}
void Tools::getinfoprofile()
{
    emit getinfo();
}
