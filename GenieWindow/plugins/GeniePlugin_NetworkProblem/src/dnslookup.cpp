#include "dnslookup.h"
#include "mainwindow.h"

DNSLookup::DNSLookup(QWidget* parent)
    :QWidget(parent)
{
    gridLayout_5 = new QGridLayout(this);

    groupBox = new QGroupBox(this);
    groupBox->setProperty("styleGroupbox",99);

    gridLayout_6 = new QGridLayout(groupBox);
    gridLayout_6->setVerticalSpacing(0);

    frame = new QFrame(groupBox);
	frame->setFrameShape(QFrame::NoFrame);
    frame->setFrameShadow(QFrame::Plain);

    verticalLayout = new QVBoxLayout(frame);
    verticalLayout->setSpacing(0);
    verticalLayout->setContentsMargins(-1, 1, -1, 0);

    label_4 = new QLabel(frame);
    label_4->setWordWrap(true);
    label_4->setProperty("stylehintplugin",99);

    verticalLayout->addWidget(label_4);

    //label_2 = new QLabel(frame);
    //label_2->setWordWrap(true);
    //label_2->setProperty("stylehintplugin",99);

    //verticalLayout->addWidget(label_2);

    gridLayout_6->addWidget(frame, 0, 0, 1, 2);

    frame_2 = new QFrame(groupBox);
    frame_2->setFrameShape(QFrame::NoFrame);
    frame_2->setFrameShadow(QFrame::Plain);

    gridLayout_7 = new QGridLayout(frame_2);

    label_3 = new QLabel(frame_2);

    gridLayout_7->addWidget(label_3, 0, 0, 1, 1);

    lineEdit = new QLineEdit(frame_2);
    lineEdit->setText(QString(tr("www.netgear.com")));

    gridLayout_7->addWidget(lineEdit, 0, 1, 1, 1);

    pushButton = new QPushButton(frame_2);
    pushButton->setFocusPolicy(Qt::NoFocus);
    pushButton->setProperty("styleButton",88);

    gridLayout_7->addWidget(pushButton, 0, 2, 1, 1);

    gridLayout_6->addWidget(frame_2, 1, 0, 1, 2);

    frame_3 = new QFrame(groupBox);
    frame_3->setFrameShape(QFrame::NoFrame);
    frame_3->setFrameShadow(QFrame::Plain);

    gridLayout = new QGridLayout(frame_3);
    gridLayout->setSpacing(0);
    gridLayout->setContentsMargins(0, 0, 0, 0);

    widget = new QWidget(frame_3);
    widget->setMinimumSize(QSize(0, 80));
    widget->setMaximumSize(QSize(16777215, 80));

    gridLayout_2 = new QGridLayout(widget);
    gridLayout_2->setSpacing(0);
    gridLayout_2->setContentsMargins(0, 0, 0, 0);

    widget_3 = new QWidget(widget);
    widget_3->setMinimumSize(QSize(0, 50));
    widget_3->setMaximumSize(QSize(16777215, 50));

    stackedLayout =new QStackedLayout(widget_3);
    pi =new QProgressIndicator(widget_3);
    stackedLayout->addWidget(pi);

    gridLayout_2->addWidget(widget_3, 0, 0, 1, 1);

    widget_4 = new QWidget(widget);

    gridLayout_3 = new QGridLayout(widget_4);
    gridLayout_3->setSpacing(0);
    gridLayout_3->setContentsMargins(0, 0, 0, 0);

    label = new QLabel(widget_4);
    label->setObjectName(QString::fromUtf8("label"));

    gridLayout_3->addWidget(label, 0, 1, 1, 1);

    horizontalSpacer_2 = new QSpacerItem(221, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout_3->addItem(horizontalSpacer_2, 0, 2, 1, 1);

    horizontalSpacer = new QSpacerItem(199, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout_3->addItem(horizontalSpacer, 0, 0, 1, 1);

    gridLayout_2->addWidget(widget_4, 1, 0, 1, 1);

    gridLayout->addWidget(widget, 0, 0, 1, 1);

    widget_2 = new QWidget(frame_3);

    gridLayout_4 = new QGridLayout(widget_2);
    gridLayout_4->setSpacing(0);
    gridLayout_4->setContentsMargins(0, 0, 0, 0);

    textBrowser = new QTextBrowser(widget_2);
    textBrowser->setObjectName("dnslookupview");
	textBrowser->setFrameShape(QFrame::NoFrame);
	textBrowser->setFrameShadow(QFrame::Plain);
	textBrowser->setProperty("styleTextBrowser", 1);

    gridLayout_4->addWidget(textBrowser, 0, 0, 1, 1);

    gridLayout->addWidget(widget_2, 1, 0, 1, 1);

    gridLayout_6->addWidget(frame_3, 2, 0, 1, 2);

    verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout_6->addItem(verticalSpacer, 3, 1, 1, 1);

    frame->raise();
    frame_2->raise();
    frame_3->raise();
    textBrowser->raise();

    gridLayout_5->addWidget(groupBox, 0, 0, 1, 1);

    uuid ="{5ddd2596-5211-4d83-980b-71f3516d0b5c}";

    widget->hide();
    widget_2->hide();

    groupBox->setTitle(QString());
    label_4->setText(tr("Use <B>DNS LOOKUP</B> to find the IP address of a hostname.The host could be any network-arrached device with name,typically a website.Type the name of the host and click the arrow."));
    //label_2->setText(tr("Alternatively,to find the hostname of an IP address,tick the reverse Lookup filed.Type the TP address and click the arrow.Not all IP address have a hostname."));
    label_3->setText(tr("Host:"));
    pushButton->setText(tr("DNS Lookup"));
    label->setText(tr("Please Wait a few seconds..."));

    connect(pushButton,SIGNAL(clicked()),this,SLOT(dnslookupTest()));

    dragon = new DragonNetTool();
    connect(dragon, SIGNAL(DnsLookupFinish()), this, SLOT(ViewResult()));
    connect(lineEdit,SIGNAL(returnPressed()),this,SLOT(linechange()));

    retranslateUi();
}
DNSLookup::~DNSLookup()
{
//    delete DataList;
}
void DNSLookup::changeEvent(QEvent* event)
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
void DNSLookup::retranslateUi()
{
    groupBox->setTitle(QString());
    label_4->setText(GENIE2_GET_TEXT(L_NETWORKPROBLEM_TOOLS_BTNDNS_TITLE));//lanmanager->getText(L_NETWORKPROBLEM_TOOLS_BTNDNS_TITLE,uuid));
    //label_2->setText(lanmanager->getText(L_NETWORKPROBLEM_TOOLS_BTNDNS_TITLE_1,uuid));
    label_3->setText(GENIE2_GET_TEXT(L_NETWORKPROBLEM_TOOLS_BTNDNS_HOST));//lanmanager->getText(L_NETWORKPROBLEM_TOOLS_BTNDNS_HOST,uuid));
    pushButton->setText(GENIE2_GET_TEXT(L_NETWORKPROBLEM_TOOLS_BTNDNS_DNSLOOKUP));//lanmanager->getText(L_NETWORKPROBLEM_TOOLS_BTNDNS_DNSLOOKUP,uuid));
    label->setText(GENIE2_GET_TEXT(L_NETWORKPROBLEM_TOOLS_BTNDNS_WAIT));//lanmanager->getText(L_NETWORKPROBLEM_TOOLS_BTNDNS_WAIT,uuid));
}
void DNSLookup::linechange()
{
    if(lineEdit->text().isEmpty())
    {
        pushButton->setDisabled(true);
    }
    else
    {
        if(widget->isHidden())
        {
            pushButton->setDisabled(false);
        }
        else
        {
            pushButton->setDisabled(true);
        }
    }
}

void DNSLookup::keyPressEvent(QKeyEvent *event)
{
    if(event->key() ==Qt::Key_Return){
        dnslookupTest();
    }
}

void DNSLookup::dnslookupTest()
{
    //////told server to const
    GENIE2_STATISTICS("N4");

    QString host =lineEdit->text();
    if(!host.isEmpty())
    {
        widget->show();
        if(widget_2->isHidden())
        {}
        else
        {
            widget_2->hide();
        }
        pushButton->setDisabled(true);
        stackedLayout->setCurrentWidget(pi);
        pi->startAnimation();
        pi->setAnimationDelay(50);
        DataList = new QStringList();
        dragon->DnsLookup(host,&DataList);

        //QString s ="h";
        //IClickManager* clickmanage =MainWindow::sHandler->getClickManager();
        //clickmanage->click(s);
    }
}
void DNSLookup::ViewResult()
{
    textBrowser->clear();
    QString ViewData;
    QString blank ="    ";
    ViewData = "Resolved IP Address:\n";

    if (!DataList->empty()){
        ViewData = ViewData + blank + "Primary DNS:   " +DataList->value(0) + "\n";
        ViewData = ViewData + blank + "Secondary DNS: " +DataList->value(0) + "\n";
        for (int i=1; i<DataList->size(); i++)
        {
            //ViewData = ViewData + blank + "Primary DNS:   " +DataList->value(i) + "\n";
            //ViewData = ViewData + blank + "Secondary DNS: " +DataList->value(i) + "\n";
            ViewData = ViewData + blank + "               " +DataList->value(i) + "\n";
        }
    }

    textBrowser->setText(ViewData);

    //pi->stopAnimation();
    //pi->setHidden(true);
    widget->hide();

    widget_2->show();
    pushButton->setDisabled(false);
}
