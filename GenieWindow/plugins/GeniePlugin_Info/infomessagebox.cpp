#include "infomessagebox.h"

#include "plugin_ui_def.h"

//#include "IUiManager.h"
//#include "ILanguageManager.h"
#include "LanguageInfo.h"


IManagerHandler *InfoMessageBox::sHandler = NULL;

InfoMessageBox::InfoMessageBox(QString message, QString title)
{
    m_dlg_width = 383;
    m_dlg_height = 212;

    //this->resize(228, 117);
    this->resize(m_dlg_width, m_dlg_height);
    this->setModal(true);
    this->setWindowFlags(Qt::Dialog| Qt::FramelessWindowHint | Qt::ToolTip);
    this->setAttribute(Qt::WA_TranslucentBackground);



    m_dlgtitle_lb_height                 = 32;
    m_dlgtitle_lb_spaceitemw        = 60;
    m_frame_lr_margin                   = 12;
    m_frame_tb_margin                   = 11;
    m_framecontainer_style = QString::fromUtf8("#frame_container{background-image: url(:/images/bg_dialog_L.png);color:rgb(0,0,0);font-size:12px;}");


//    m_framecontainer = new QFrame(this);
//    m_framecontainer->setObjectName(QString("frame_container"));
//    m_framecontainer->setAutoFillBackground(true);
//    m_framecontainer->setStyleSheet(m_framecontainer_style);

//    QVBoxLayout  *dlg_layout = new QVBoxLayout(this);
//    dlg_layout->setContentsMargins(0,0,0,0);
//    dlg_layout->addWidget(m_framecontainer);
//
//    m_dlgtitle_lb = new QLabel(m_framecontainer);
//    init_dlgtitle();
//
//
//    QSpacerItem  *title_spitem = new QSpacerItem(m_dlgtitle_lb_spaceitemw,0,QSizePolicy::Fixed);
//    QHBoxLayout *title_layout  = new QHBoxLayout();
//    title_layout->addItem(title_spitem);
//    title_layout->addWidget(m_dlgtitle_lb);
//
//
//    QVBoxLayout   *container_layout = new QVBoxLayout(m_framecontainer);
//    container_layout->setContentsMargins(m_frame_lr_margin,m_frame_tb_margin,m_frame_lr_margin,m_frame_tb_margin);
//    container_layout->setSpacing(1);
//    container_layout->addLayout(title_layout);
//    //m_inner_widget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
//    //container_layout->addWidget(m_inner_widget);
//
//    container_layout->setStretch(0,0);
//    container_layout->setStretch(1,10);
//
////    resize(m_dlg_width,m_dlg_height);






//    gridLayout = new QGridLayout(this);
//    gridLayout->setSpacing(0);
//    gridLayout->setContentsMargins(0, 0, 0, 0);

    mFrame = new QFrame(this);
    mFrame->setObjectName(QString("Input_frame"));
    mFrame->setFrameShape(QFrame::StyledPanel);
    mFrame->setFrameShadow(QFrame::Raised);
    QPixmap pix;
    pix.load(":/images/bg_dialog_L.png",0,Qt::AvoidDither | Qt::ThresholdDither | Qt::ThresholdAlphaDither);
    QSize sizeImage(m_dlg_width, m_dlg_height);
    QPixmap pixmap(pix.scaled(sizeImage));
    QPalette mPalette = palette();
    mPalette.setBrush(QPalette::Window, QBrush(pixmap));
    mFrame->setPalette(mPalette);

    QVBoxLayout *mGridLayout = new QVBoxLayout(this);
    //mGridLayout->setSpacing(0);
    mGridLayout->setContentsMargins(0, 0, 0, 0);


    titleLabel = new QLabel(mFrame);
    titleLabel->setObjectName(QString("dlg_title_lb"));
    titleLabel->setFixedHeight(m_dlgtitle_lb_height);
    titleLabel->setAlignment(Qt::AlignVCenter);
    QString stylesheet = QString("titleLabel{color:rgb(255,255,255);%1;font-weight:bold;}");
    stylesheet = stylesheet.arg("font-size:14px");
    titleLabel->setStyleSheet(stylesheet);
    titleLabel->setText("Error");

    QSpacerItem  *title_spitem = new QSpacerItem(m_dlgtitle_lb_spaceitemw,0,QSizePolicy::Fixed);

    QHBoxLayout *title_layout  = new QHBoxLayout();
    title_layout->addItem(title_spitem);
    title_layout->addWidget(titleLabel);


    mGridLayout->addWidget(mFrame);






//    gridLayout_2 = new QGridLayout(mFrame);
//
//    horizontalLayout = new QHBoxLayout();
//
//    label = new QLabel(mFrame);
//    label->setMinimumSize(QSize(38, 0));
//    label->setMaximumSize(QSize(38, 16777215));
//    label->setText("Error");
//
//    horizontalLayout->addWidget(label);
//
//    label_2 = new QLabel(mFrame);
//    QFont font;
//    font.setBold(true);
//    font.setWeight(75);
//    label_2->setFont(font);
//    label_2->setText(title);
//    label_2->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);"));
//    label_2->setProperty("overstriking",1);
//
//    horizontalLayout->addWidget(label_2);
//
//    gridLayout_2->addLayout(horizontalLayout, 0, 0, 0, 0);
//
//    label_3 = new QLabel(mFrame);
//    label_3->setWordWrap(true);
//    label_3->setText(message);
//    label_3->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
//
//    gridLayout_2->addWidget(label_3, 1, 0, 1, 3);
//
//    horizontalSpacer = new QSpacerItem(60, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
//
//    gridLayout_2->addItem(horizontalSpacer, 2, 0, 1, 1);





    pushButton = new QPushButton(mFrame);
    pushButton->setFocusPolicy(Qt::NoFocus);
    SET_STYLE_PROPERTY(NORMAL_BUTTON ,pushButton);

    //gridLayout_2->addWidget(pushButton, 2, 1, 1, 1);

    //horizontalSpacer_2 = new QSpacerItem(60, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    //gridLayout_2->addItem(horizontalSpacer_2, 2, 2, 1, 1);

    title_layout->addWidget(pushButton);


    //gridLayout->addWidget(mFrame, 0, 0, 1, 1);

    //mGridLayout->addLayout(title_layout);

    uuid ="{f9b01961-e3b2-474c-8521-42371d2cab61}";

    connect(pushButton,SIGNAL(clicked()),this,SLOT(ChangePage()));

    retranslateUi();




}

InfoMessageBox::~InfoMessageBox()
{

}

void InfoMessageBox::init_dlgtitle()
{
//    m_dlgtitle_lb->setObjectName(QString("dlg_title_lb"));
//    m_dlgtitle_lb->setFixedHeight(m_dlgtitle_lb_height);
//    m_dlgtitle_lb->setAlignment(Qt::AlignVCenter);
//    QString stylesheet =QString("#dlg_title_lb{color:rgb(255,255,255);%1;font-weight:bold;}");
//
//    stylesheet=stylesheet.arg("font-size:14px");
//
//    m_dlgtitle_lb->setStyleSheet(stylesheet);

}


















void InfoMessageBox::changeEvent(QEvent* event)
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

void InfoMessageBox::retranslateUi()
{
    lanmanager = InfoMessageBox::sHandler->getLanguageManager();
    //pushButton->setText(lanmanager->getText(L_UPGRADE_PRODUCT_FAILED,uuid));
}

void InfoMessageBox::resizeEvent(QResizeEvent *event)
{
//    mFrame->setStyleSheet(m_framecontainer_style);

    mFrame->setAutoFillBackground(true);
//    QPixmap pix;
//    //pix.load(":/Images/gai1.png",0,Qt::AvoidDither | Qt::ThresholdDither | Qt::ThresholdAlphaDither);
//    pix.load(":/images/bg_dialog_L.png",0,Qt::AvoidDither | Qt::ThresholdDither | Qt::ThresholdAlphaDither);
//    int ew = this->size().width();
//    int eh = this->size().height();
//    QSize size(ew,eh);
//    QPixmap pixmap(pix.scaled(size));
//    QPalette p = palette();
//    p.setBrush(QPalette::Window, QBrush(pixmap));
//    mFrame->setPalette(p);

}

void InfoMessageBox::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        QPoint newpos = event->globalPos();
        QPoint upleft = pos0 + newpos - last;
        this->move(upleft);
    }
}

void InfoMessageBox::mousePressEvent(QMouseEvent *event)
{
    last = event->globalPos();
    pos0 = event->globalPos() - event->pos();
}

void InfoMessageBox::labelchange(QString message, QString title)
{
//    label_2->setText(title);

//    label_3->setText(message);
}

void InfoMessageBox::ChangePage()
{

    this->close();

//    QPluginLoader loader_Router(QString("GeniePlugin_RouterConfiguration.dll"));
//    m_Router_plugin = qobject_cast<IPluginInf*>(loader_Router.instance());
//    if(m_Router_plugin)
//    {
//        m_Router_plugin->command(QString("GuestAccess_LoginPage"),111111);
//    }

}
