#include "support.h"
#include "mainwindow.h"

Support::Support(QWidget* parent)
    :QWidget(parent)
{
    gridLayout = new QGridLayout(this);

    widget = new QWidget(this);
    widget->setMinimumSize(QSize(0, 50));
    widget->setMaximumSize(QSize(16777215, 50));

    gridLayout_3 = new QGridLayout(widget);

    verticalSpacer = new QSpacerItem(20, 41, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout_3->addItem(verticalSpacer, 0, 0, 1, 1);

    label = new QLabel(widget);
    label->setWordWrap(true);

    gridLayout_3->addWidget(label, 1, 0, 1, 1);

    gridLayout->addWidget(widget, 0, 0, 1, 1);

    widget_2 = new QWidget(this);

    gridLayout_5 = new QGridLayout(widget_2);

    frame = new QFrame(widget_2);
    frame->setFrameShape(QFrame::NoFrame);
    frame->setFrameShadow(QFrame::Plain);

    gridLayout_4 = new QGridLayout(frame);
    gridLayout_4->setVerticalSpacing(20);

    verticalSpacer_2 = new QSpacerItem(20, 105, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout_4->addItem(verticalSpacer_2, 0, 1, 1, 1);

    pushButton = new PushButton(frame);
    pushButton->setFocusPolicy(Qt::NoFocus);
    //pushButton->setMaximumSize(191,34);
    //pushButton->setMinimumSize(191,34);
    //pushButton->setStyleSheet("border-image:url(:/images/plugin_gradient_but.png);");

    gridLayout_4->addWidget(pushButton, 1, 1, 1, 1);

    pushButton_2 = new PushButton(frame);
    pushButton_2->setFocusPolicy(Qt::NoFocus);
    //pushButton_2->setMaximumSize(191,34);
    //pushButton_2->setMinimumSize(191,34);
    //pushButton_2->setStyleSheet("border-image:url(:/images/plugin_gradient_but.png);");

    gridLayout_4->addWidget(pushButton_2, 2, 1, 1, 1);

    horizontalSpacer = new QSpacerItem(294, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout_4->addItem(horizontalSpacer, 3, 0, 1, 1);

    pushButton_3 = new PushButton(frame);
    pushButton_3->setFocusPolicy(Qt::NoFocus);
    //pushButton_3->setMaximumSize(191,34);
    //pushButton_3->setMinimumSize(191,34);
    //pushButton_3->setStyleSheet("border-image:url(:/images/plugin_gradient_but.png);");

    gridLayout_4->addWidget(pushButton_3, 3, 1, 1, 1);

    horizontalSpacer_2 = new QSpacerItem(294, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout_4->addItem(horizontalSpacer_2, 3, 2, 1, 1);

    pushButton_4 = new PushButton(frame);
    pushButton_4->setFocusPolicy(Qt::NoFocus);
    //pushButton_4->setMaximumSize(191,34);
    //pushButton_4->setMinimumSize(191,34);
    //pushButton_4->setStyleSheet("border-image:url(:/images/plugin_gradient_but.png);");

	/**************2012-3-20 add*******************/
	pushButton->setProperty("supportBtn", 1);
	pushButton_2->setProperty("supportBtn", 1);
	pushButton_3->setProperty("supportBtn", 1);
	pushButton_4->setProperty("supportBtn", 1);
	/*******************END************************/

    gridLayout_4->addWidget(pushButton_4, 4, 1, 1, 1);

    verticalSpacer_3 = new QSpacerItem(20, 104, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout_4->addItem(verticalSpacer_3, 5, 1, 1, 1);

    gridLayout_5->addWidget(frame, 0, 0, 1, 1);

    frame_2 = new QFrame(widget_2);
    frame_2->setFrameShape(QFrame::NoFrame);
    frame_2->setFrameShadow(QFrame::Plain);

    gridLayout_2 = new QGridLayout(frame_2);
    gridLayout_2->setVerticalSpacing(20);
    gridLayout_2->setContentsMargins(1,1,1,1);

    verticalSpacer_5 = new QSpacerItem(51, 6, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout_2->addItem(verticalSpacer_5, 0, 1, 1, 1);

    horizontalSpacer_4 = new QSpacerItem(305, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout_2->addItem(horizontalSpacer_4, 3, 0, 1, 1);

    horizontalSpacer_3 = new QSpacerItem(304, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout_2->addItem(horizontalSpacer_3, 3, 2, 1, 1);

    verticalSpacer_4 = new QSpacerItem(51, 6, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout_2->addItem(verticalSpacer_4, 5, 1, 1, 1);

    label_2 = new QLabel(frame_2);

    gridLayout_2->addWidget(label_2, 1, 1, 1, 1);

    label_3 = new QLabel(frame_2);

    gridLayout_2->addWidget(label_3, 2, 1, 1, 1);

    label_4 = new QLabel(frame_2);

    gridLayout_2->addWidget(label_4, 3, 1, 1, 1);

    label_5 = new QLabel(frame_2);

    gridLayout_2->addWidget(label_5, 4, 1, 1, 1);

    lineEdit = new QLineEdit(frame_2);
    lineEdit->setMinimumSize(QSize(370, 0));
    lineEdit->setMaximumSize(QSize(370, 16777215));
    lineEdit->setReadOnly(true);
    lineEdit->setDisabled(true);

    gridLayout_2->addWidget(lineEdit, 1, 2, 1, 1);

    lineEdit_2 = new QLineEdit(frame_2);
    lineEdit_2->setReadOnly(true);
    lineEdit_2->setDisabled(true);

    gridLayout_2->addWidget(lineEdit_2, 2, 2, 1, 1);

    lineEdit_3 = new QLineEdit(frame_2);
    lineEdit_3->setReadOnly(true);
    lineEdit_3->setDisabled(true);

    gridLayout_2->addWidget(lineEdit_3, 3, 2, 1, 1);

    lineEdit_4 = new QLineEdit(frame_2);
    lineEdit_4->setReadOnly(true);
    lineEdit_4->setDisabled(true);
    lineEdit_4->setAlignment(Qt::AlignLeft);
    lineEdit_4->setFrame(false);

    gridLayout_2->addWidget(lineEdit_4, 4, 2, 1, 1);

    pushButton_5 = new QPushButton(frame_2);
    pushButton_5->setProperty("SupportCopy",1);
    gridLayout_2->addWidget(pushButton_5, 3, 3, 1, 1);

    pushButton_6 = new QPushButton(frame_2);
    pushButton_6->setProperty("SupportCopy",1);

    gridLayout_2->addWidget(pushButton_6, 2, 3, 1, 1);

    pushButton_7 = new QPushButton(frame_2);
    pushButton_7->setProperty("SupportCopy",1);

    gridLayout_2->addWidget(pushButton_7, 1, 3, 1, 1);

    pushButton_8 = new QPushButton(frame_2);

    pushButton_8->setProperty("SupportCopy",1);

    gridLayout_2->addWidget(pushButton_8, 4, 3, 1, 1);

    gridLayout_5->addWidget(frame_2, 1, 0, 1, 1);

    gridLayout->addWidget(widget_2, 1, 0, 1, 1);

    checkBox = new QCheckBox(this);
    checkBox->setHidden(true);

    gridLayout->addWidget(checkBox, 2, 0, 1, 1);

    uuid ="{5ddd2596-5211-4d83-980b-71f3516d0b5c}";

    label->setText(tr("If you would like to know more about our NETGEAR products,threr links to the NETGEAR website may be of assistance."));
    pushButton->setText(QString(tr("Downloads")));
    pushButton_2->setText(QString(tr("Forums")));
    pushButton_3->setText(QString(tr("Register Your Product")));
    pushButton_4->setText(QString(tr("Premium Support")));
    checkBox->setText(QString(tr("The browser version is IE6, please select")));
    label_2->setText(QString(tr("<B>Downloads:</B>")));
    label_3->setText(QString(tr("<B>Forums:</B>")));
    label_4->setText(QString(tr("<B>Register Your Product:</B>")));
    label_5->setText(QString(tr("<B>Premium Support:</B>")));
    pushButton_5->setText(QString(tr("Copy")));
    pushButton_6->setText(QString(tr("Copy")));
    pushButton_7->setText(QString(tr("Copy")));
    pushButton_8->setText(QString(tr("Copy")));

    lineEdit->setText(QString(tr("http://kb.NetGear.com/app/products/list")));
    lineEdit_2->setText(QString(tr("http://forum1.NetGear.com/")));
    lineEdit_3->setText(QString(tr("https://my.NetGear.com/registration/login.aspx")));
    lineEdit_4->setText(QString(tr("http://kbserver.NetGear.com/kb_web_files/gearhead/home.html")));

    connect(pushButton,SIGNAL(clicked()),this,SLOT(Downloads()));
    connect(pushButton_2,SIGNAL(clicked()),this,SLOT(Forums()));
    connect(pushButton_3,SIGNAL(clicked()),this,SLOT(Register()));
    connect(pushButton_4,SIGNAL(clicked()),this,SLOT(Premium()));
    connect(pushButton_7,SIGNAL(clicked()),this,SLOT(copydownload()));
    connect(pushButton_6,SIGNAL(clicked()),this,SLOT(copyforums()));
    connect(pushButton_5,SIGNAL(clicked()),this,SLOT(copyregister()));
    connect(pushButton_8,SIGNAL(clicked()),this,SLOT(copysupport()));


    retranslateUi();

    frame_2->hide();

    //connect(checkBox,SIGNAL(clicked()),this,SLOT(changePage()));
}
Support::~Support()
{

}
void Support::changeEvent(QEvent* event)
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
void Support::retranslateUi()
{
    label->setText(GENIE2_GET_TEXT(L_NETWORKPROBLEM_SUPPORT_TITLE));//lanmanager->getText(L_NETWORKPROBLEM_SUPPORT_TITLE,uuid));
    pushButton->setText(GENIE2_GET_TEXT(L_NETWORKPROBLEM_SUPPORT_BTNDOWNLOADS));//lanmanager->getText(L_NETWORKPROBLEM_SUPPORT_BTNDOWNLOADS,uuid));
    pushButton_2->setText(GENIE2_GET_TEXT(L_NETWORKPROBLEM_SUPPORT_FORUMS));//lanmanager->getText(L_NETWORKPROBLEM_SUPPORT_FORUMS,uuid));
    pushButton_3->setText(GENIE2_GET_TEXT(L_NETWORKPROBLEM_SUPPORT_REGISTER));//lanmanager->getText(L_NETWORKPROBLEM_SUPPORT_REGISTER,uuid));
    pushButton_4->setText(GENIE2_GET_TEXT(L_NETWORKPROBLEM_SUPPORT_SUPPORT));//lanmanager->getText(L_NETWORKPROBLEM_SUPPORT_SUPPORT,uuid));
    pushButton_5->setText(GENIE2_GET_TEXT(L_NETWORKPROBLEM_SUPPORT_SUPPORT_COPY));//lanmanager->getText(L_NETWORKPROBLEM_SUPPORT_SUPPORT_COPY,uuid));
    pushButton_6->setText(GENIE2_GET_TEXT(L_NETWORKPROBLEM_SUPPORT_SUPPORT_COPY));//lanmanager->getText(L_NETWORKPROBLEM_SUPPORT_SUPPORT_COPY,uuid));
    pushButton_7->setText(GENIE2_GET_TEXT(L_NETWORKPROBLEM_SUPPORT_SUPPORT_COPY));//lanmanager->getText(L_NETWORKPROBLEM_SUPPORT_SUPPORT_COPY,uuid));
    pushButton_8->setText(GENIE2_GET_TEXT(L_NETWORKPROBLEM_SUPPORT_SUPPORT_COPY));//lanmanager->getText(L_NETWORKPROBLEM_SUPPORT_SUPPORT_COPY,uuid));
    checkBox->setText(GENIE2_GET_TEXT(L_NETWORKPROBLEM_SUPPORT_SUPPORT_SELETE));//lanmanager->getText(L_NETWORKPROBLEM_SUPPORT_SUPPORT_SELETE,uuid));
}
void Support::Downloads()
{
    mOper.clear();
    mOper = "NS1";
    GENIE2_STATISTICS("NS1");

    QString slan=GENIE2_HANDLER_CMD(HCMD_ASK_SUBSCRIBED_MESSAGE,SCCMD_LANGUAGE).toString();

    QUrl url((slan=="jpn")?"http://www.netgear.jp/supportInfo/":"http://kb.NetGear.com/app/products/list");
    QDesktopServices::openUrl(url);

    //clickmanage->click(s);

//    IUiManager* uimanager = MainWindow::sHandler->getUiManager();
//    uimanager->getClipBoard()->clear(QClipboard::Clipboard);
//    QClipboard* clipboard = uimanager->getClipBoard();
//    QApplication::clipboard()->clear(QClipboard::Clipboard);
//    QClipboard* clipboard = QApplication::clipboard();
//    clipboard->setText("http://kb.NetGear.com/app/products/list");
//    qDebug()<<clipboard;

//    pushButton->setCheckable(true);
//    pushButton->setChecked(true);
//    pushButton_2->setChecked(false);
//    pushButton_3->setChecked(false);
//    pushButton_4->setChecked(false);
//    pushButton->setStyleSheet("background-image:url(:/Images/an2.png);");
//    pushButton_2->setStyleSheet("background-image:url(:/Images/an1.png);");
//    pushButton_3->setStyleSheet("background-image:url(:/Images/an1.png);");
//    pushButton_4->setStyleSheet("background-image:url(:/Images/an1.png);");
}
void Support::Forums()
{
    mOper.clear();
    GENIE2_STATISTICS("NS2");

    QUrl url("http://forum1.NetGear.com/");
    QDesktopServices::openUrl(url);

    //clickmanage->click(s);

//    pushButton_2->setCheckable(true);
//    pushButton_2->setChecked(true);
//    pushButton->setChecked(false);
//    pushButton_3->setChecked(false);
//    pushButton_4->setChecked(false);
//    pushButton->setStyleSheet("background-image:url(:/Images/an1.png);");
//    pushButton_2->setStyleSheet("background-image:url(:/Images/an2.png);");
//    pushButton_3->setStyleSheet("background-image:url(:/Images/an1.png);");
//    pushButton_4->setStyleSheet("background-image:url(:/Images/an1.png);");
}
void Support::Register()
{
    mOper.clear();

    GENIE2_STATISTICS("NS3");

    QString slan=GENIE2_HANDLER_CMD(HCMD_ASK_SUBSCRIBED_MESSAGE,SCCMD_LANGUAGE).toString();

    QUrl url((slan=="jpn")?"https://www.netgear.jp/supportInfo/RegisterUser/"
        :"https://my.NetGear.com/registration/login.aspx");
    QDesktopServices::openUrl(url);

    //clickmanage->click(s);

//    pushButton_3->setCheckable(true);
//    pushButton_3->setChecked(true);
//    pushButton_2->setChecked(false);
//    pushButton->setChecked(false);
//    pushButton_4->setChecked(false);
//    pushButton->setStyleSheet("background-image:url(:/Images/an1.png);");
//    pushButton_2->setStyleSheet("background-image:url(:/Images/an1.png);");
//    pushButton_3->setStyleSheet("background-image:url(:/Images/an2.png);");
//    pushButton_4->setStyleSheet("background-image:url(:/Images/an1.png);");
}
void Support::Premium()
{
    mOper.clear();

    GENIE2_STATISTICS("NS4");


    QString slan=GENIE2_HANDLER_CMD(HCMD_ASK_SUBSCRIBED_MESSAGE,SCCMD_LANGUAGE).toString();

    QUrl url((slan=="jpn")?"https://www.netgear.jp/support/premiumsupport"
        :"http://kbserver.NetGear.com/kb_web_files/gearhead/home.html");
    QDesktopServices::openUrl(url);

    //clickmanage->click(s);

//    pushButton_4->setCheckable(true);
//    pushButton_4->setChecked(true);
//    pushButton_2->setChecked(false);
//    pushButton_3->setChecked(false);
//    pushButton->setChecked(false);
//    pushButton->setStyleSheet("background-image:url(:/Images/an1.png);");
//    pushButton_2->setStyleSheet("background-image:url(:/Images/an1.png);");
//    pushButton_3->setStyleSheet("background-image:url(:/Images/an1.png);");
//    pushButton_4->setStyleSheet("background-image:url(:/Images/an2.png);");
}
void Support::changePage()
{
    if(checkBox->isChecked())
    {
        frame_2->show();
        frame->hide();
    }
    else
    {
        frame->show();
        frame_2->hide();
    }
}
void Support::copydownload()
{
    QString text= lineEdit->text();
//    uimanager->setClipBoardString(text);
    QApplication::clipboard()->setText(text);
}
void Support::copyforums()
{
    QString text= lineEdit_2->text();
//    uimanager->setClipBoardString(text);
    QApplication::clipboard()->setText(text);
}
void Support::copyregister()
{
    QString text= lineEdit_3->text();
//    uimanager->setClipBoardString(text);
    QApplication::clipboard()->setText(text);
}
void Support::copysupport()
{
    QString text= lineEdit_4->text();
//    uimanager->setClipBoardString(text);
    QApplication::clipboard()->setText(text);
}
