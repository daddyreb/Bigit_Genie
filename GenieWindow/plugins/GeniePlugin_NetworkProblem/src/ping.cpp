#include "ping.h"
#include "mainwindow.h"

Ping::Ping(QWidget* parent)
    :QWidget(parent)
{
    gridLayout_5 = new QGridLayout(this);

    groupBox = new QGroupBox(this);
    groupBox->setProperty("styleGroupbox",99);

    gridLayout_6 = new QGridLayout(groupBox);
    gridLayout_6->setVerticalSpacing(6);

    frame = new QFrame(groupBox);
    frame->setFrameShape(QFrame::NoFrame);
    frame->setFrameShadow(QFrame::Plain);

    verticalLayout = new QVBoxLayout(frame);
    verticalLayout->setSpacing(9);
    verticalLayout->setContentsMargins(-1, 1, -1, 0);

    label_4 = new QLabel(frame);
    label_4->setWordWrap(true);
    label_4->setProperty("stylehintplugin",99);

    verticalLayout->addWidget(label_4);

    label_2 = new QLabel(frame);
    label_2->setWordWrap(true);
    label_2->setProperty("stylehintplugin",99);

    verticalLayout->addWidget(label_2);

    gridLayout_6->addWidget(frame, 0, 0, 1, 1);

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

    gridLayout_6->addWidget(frame_2, 1, 0, 1, 1);

    frame_3 = new QFrame(groupBox);
    frame_3->setFrameShape(QFrame::NoFrame);
    frame_3->setFrameShadow(QFrame::Plain);

    gridLayout = new QGridLayout(frame_3);
    gridLayout->setSpacing(0);
    gridLayout->setContentsMargins(0, 15, 0, 0);

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
    textBrowser->setObjectName("pingview");
    //textBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    textBrowser->setWordWrapMode(QTextOption::WordWrap);
	textBrowser->setProperty("styleTextBrowser", 1);
    //textBrowser->setWordWrapMode();

	textBrowser->setFrameShadow(QFrame::Plain);
	textBrowser->setFrameShape(QFrame::NoFrame);

    gridLayout_4->addWidget(textBrowser, 0, 0, 1, 1);

    gridLayout->addWidget(widget_2, 1, 0, 1, 1);

    gridLayout_6->addWidget(frame_3, 2, 0, 1, 1);

    verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout_6->addItem(verticalSpacer, 3, 0, 1, 1);

    gridLayout_5->addWidget(groupBox, 1, 0, 1, 1);

    uuid ="{5ddd2596-5211-4d83-980b-71f3516d0b5c}";

    widget->hide();
    widget_2->hide();

    groupBox->setTitle(QString());
    label_4->setText(tr("Use <B>PING</B> to text whether your computer can reach a particular host or website."));
    label_2->setText(tr("Type the name or IP address of the host and click the arrow."));
    label_3->setText(tr("Host:"));
    pushButton->setText(tr("Ping"));
    label->setText(tr("Please wait a few seconds..."));

    dragon= new DragonNetTool();
    connect(pushButton,SIGNAL(clicked()),this,SLOT(pingTest()));
    connect(dragon,SIGNAL(ReadPingData()),this,SLOT(ViewResult()));
    connect(dragon,SIGNAL(PingFinish()),this,SLOT(pingfinished()));
    connect(lineEdit,SIGNAL(returnPressed()),this,SLOT(linechange()));

//    connect(&g_thread,SIGNAL(PingFinish()),this,SLOT(ViewResult()));

    retranslateUi();

}
Ping::~Ping()
{
//    delete DataList;
}
void Ping::changeEvent(QEvent* event)
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
void Ping::retranslateUi()
{
    groupBox->setTitle(QString());
    label_4->setText(GENIE2_GET_TEXT(L_NETWORKPROBLEM_TOOLS_BTNPING_TITLE));//lanmanager->getText(L_NETWORKPROBLEM_TOOLS_BTNPING_TITLE,uuid));
    label_2->setText(GENIE2_GET_TEXT(L_NETWORKPROBLEM_TOOLS_BTNPING_TITLE_1));//lanmanager->getText(L_NETWORKPROBLEM_TOOLS_BTNPING_TITLE_1,uuid));
    label_3->setText(GENIE2_GET_TEXT(L_NETWORKPROBLEM_TOOLS_BTNPING_HOST));//lanmanager->getText(L_NETWORKPROBLEM_TOOLS_BTNPING_HOST,uuid));
    pushButton->setText(GENIE2_GET_TEXT(L_NETWORKPROBLEM_TOOLS_BTNPING_PING));//lanmanager->getText(L_NETWORKPROBLEM_TOOLS_BTNPING_PING,uuid));
    label->setText(GENIE2_GET_TEXT(L_NETWORKPROBLEM_TOOLS_BTNPING_WAIT));//lanmanager->getText(L_NETWORKPROBLEM_TOOLS_BTNPING_WAIT,uuid));
}
void Ping::linechange()
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
void Ping::keyPressEvent(QKeyEvent *event)
{
    if(event->key() ==Qt::Key_Return)
    {
        pingTest();
    }
}
void Ping::pingTest()
{
    //////told server to const
    GENIE2_STATISTICS("N2");

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

//        DataList = 0;
//        g_thread.begin_ping(host,&DataList);
        DataList = new QStringList();
        dragon->Ping(host,&DataList);

        //QString s ="h";
        //IClickManager* clickmanage =MainWindow::sHandler->getClickManager();
        //clickmanage->click(s);

#ifdef USE_LOG
#define GENIE_LOG(s) do{\
        ILogManager *logmanger=MainWindow::sHandler->getLogManager();\
        logmanger->writeLog("writelog",uuid);\
        }while(0)
#else
#define GENIE_LOG(s) do{}while(0)
#endif
    }
}
void Ping::ViewResult()
{
//    QMessageBox::about(NULL,"Test","test");
    textBrowser->clear();
    QString ViewData;
    QString strTemp;
    for (int i=0; i<DataList->size(); i++)
    {
        strTemp = DataList->value(i);
        if ( strTemp != "over" ){
//        ViewData ="Host name or IP address resolved  =   " + DataList->value(i) + "\n";
            ViewData = ViewData + strTemp + "\n";
        }else {
            ViewData = ViewData + "\n" + GENIE2_GET_TEXT(L_NETWORKPROBLEM_TOOLS_BTNROUTE_TRACEROUTEOVER);//lanmanager->getText(L_NETWORKPROBLEM_TOOLS_BTNROUTE_TRACEROUTEOVER,uuid);
        }
    }
    textBrowser->setText(ViewData);
    textBrowser->moveCursor(QTextCursor::End);

    //pi->stopAnimation();
    //pi->setHidden(true);
    widget->hide();

    widget_2->show();
}

void Ping::pingfinished()
{
    //////2011-03-08
    //QString strOver;
    //ILanguageManager *lanmanager = MainWindow::sHandler->getLanguageManager();
    //strOver = "\n" + lanmanager->getText(L_NETWORKPROBLEM_TOOLS_BTNROUTE_TRACEROUTEOVER,uuid);
    //DataList->append(strOver);
    ViewResult();

    //pi->stopAnimation();
    //pi->setHidden(true);
    widget->hide();

    pushButton->setDisabled(false);
}

MyThread::MyThread(QObject *parent)
    :QThread(parent)
{
//    DataList = 0;
//    m_strhost.clear();
//    connect(&dragon,SIGNAL(PingFinish()),this,SIGNAL(PingFinish()));
//    connect(&dragon,SIGNAL(PingFinish()),this,SLOT(on_PingFinish()));
}
MyThread::~MyThread()
{
//    this->wait();
}
void MyThread::begin_ping(const QString &host,QStringList **list)
{
//    if(host.isEmpty() || 0==list)
//        return;
//    else
//    {
//        m_strhost = host;
//        DataList = list;
//        this->start();
//    }
}
void MyThread::run()
{
//    if(m_strhost.isEmpty() || 0==DataList)
//        return;
//    dragon.Ping(m_strhost,DataList);
//    qDebug()<<"test";
}

void MyThread::on_PingFinish()
{
//    qDebug()<<"on_PingFinish";
}
