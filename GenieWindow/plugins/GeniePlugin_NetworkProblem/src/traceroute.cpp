#include "traceroute.h"
#include "mainwindow.h"

TraceRoute::TraceRoute(QWidget* parent)
    :QWidget(parent)
{
    this->setProperty("stylehintplugin",5);

    gridLayout_5 = new QGridLayout(this);

    groupBox = new QGroupBox(this);
    //groupBox->setProperty("styleGroupbox",99);

    gridLayout_6 = new QGridLayout(groupBox);
    gridLayout_6->setVerticalSpacing(0);

    frame = new QFrame(groupBox);
    frame->setMaximumSize(QSize(16777215, 100));
    frame->setMaximumSize(QSize(16777215, 200));
    frame->setFrameShape(QFrame::NoFrame);
    frame->setFrameShadow(QFrame::Plain);

    verticalLayout = new QVBoxLayout(frame);
    verticalLayout->setSpacing(9);
    verticalLayout->setContentsMargins(-1, 1, -1, 0);

    label_4 = new QLabel(frame);
    //label_4->setMinimumSize(QSize(20, 20));
    //label_4->setMaximumSize(QSize(16777215, 16777215));
    label_4->setWordWrap(true);
    label_4->setProperty("stylehintplugin",99);

    verticalLayout->addWidget(label_4);

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
    //SET_FIX_SIZE(NORMAL_BUTTON ,pushButton);
    pushButton->setProperty("styleButton",88);

    gridLayout_7->addWidget(pushButton, 0, 2, 1, 1);

    gridLayout_6->addWidget(frame_2, 1, 0, 1, 1);

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

    widget_3 = new QWidget(widget);
    widget_3->setMinimumSize(QSize(0, 50));
    widget_3->setMaximumSize(QSize(16777215, 50));

    stackedLayout =new QStackedLayout(widget_3);
    pi =new QProgressIndicator(widget_3);
    stackedLayout->addWidget(pi);

    gridLayout_2->addWidget(widget_3, 0, 0, 1, 1);

    gridLayout->addWidget(widget, 0, 0, 1, 1);

    gridLayout_6->addWidget(frame_3, 2, 0, 1, 1);

    widget_2 = new QWidget(groupBox);

    gridLayout_4 = new QGridLayout(widget_2);
    gridLayout_4->setSpacing(0);
    gridLayout_4->setContentsMargins(0, 0, 0, 0);

    //textBrowser = new QTextEdit(widget_2);
    textBrowser = new QTextBrowser(widget_2);
    textBrowser->setObjectName("tracerouteview");
    textBrowser->verticalScrollBar()->setSliderDown(true);
	textBrowser->setFrameShape(QFrame::NoFrame);
	textBrowser->setFrameShadow(QFrame::Plain);
	textBrowser->setProperty("styleTextBrowser", 1);
//    textBrowser->moveCursor(MoveOperation::End);

    gridLayout_4->addWidget(textBrowser, 0, 0, 1, 1);

    gridLayout_6->addWidget(widget_2, 3, 0, 1, 1);

    verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout_6->addItem(verticalSpacer, 4, 0, 1, 1);

    gridLayout_5->addWidget(groupBox, 1, 0, 1, 1);

    uuid ="{5ddd2596-5211-4d83-980b-71f3516d0b5c}";

    widget->hide();
    widget_2->hide();

    groupBox->setTitle(QString());
    label_4->setText(tr("Use <B>TRACE ROUTE</B> to identify the route taken by data packets from your computer to the host.Trace route helps the trained eye locate routeing problems or firewalls that may be blocking your computer's access to the host.Type the name or IP address of the host and click the arrow."));
    label_3->setText(tr("Host:"));
    pushButton->setText(tr("Trace Route"));
    label->setText(tr("Please wait a few seconds..."));

    dragon = new DragonNetTool();
    connect(dragon, SIGNAL(ReadTraceRouteData()), this, SLOT(ViewResult()));
    connect(dragon,SIGNAL(TraceRouteFinish()),this,SLOT(tracefinish()));

    connect(lineEdit,SIGNAL(returnPressed()),this,SLOT(linechange()));
    connect(pushButton,SIGNAL(clicked()),this,SLOT(routeTest()));

    retranslateUi();

//    cmd = new QProcess;
//
//    connect(cmd,SIGNAL(readyRead()),this,SLOT(readOutput()));
//    connect(cmd,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(ProcessFinish(int,QProcess::ExitStatus)));
}
TraceRoute::~TraceRoute()
{
//    delete DataList;
}
void TraceRoute::changeEvent(QEvent* event)
{
    QWidget::changeEvent(event);
    switch(event->type())
    {
        case QEvent::LanguageChange:
        {
//            //ViewResult();
//            //QString ViewData;
//            QString strTemp;
//            for (int k=0; k<DataList->size(); k++)
//            {
//                strTemp = DataList->value(k);
//                //if ( strTemp != "over" ){
//                    //ViewData = ViewData + strTemp + "\n";
//                //}else {
//                    //ILanguageManager *lanmanager = MainWindow::sHandler->getLanguageManager();
//                    //ViewData = ViewData + "\n";
//                    //ViewData = ViewData + lanmanager->getText(L_NETWORKPROBLEM_TOOLS_BTNROUTE_TRACEROUTEOVER,uuid);
//                //}
//            }
//            textBrowser->setText("afdasf");

            retranslateUi();
            break;
        }
        default:
                break;
    }
}
void TraceRoute::retranslateUi()
{
    groupBox->setTitle(QString());
    label_4->setText(GENIE2_GET_TEXT(L_NETWORKPROBLEM_TOOLS_BTNROUTE_TITLE));//lanmanager->getText(L_NETWORKPROBLEM_TOOLS_BTNROUTE_TITLE,uuid));
    label_3->setText(GENIE2_GET_TEXT(L_NETWORKPROBLEM_TOOLS_BTNROUTE_HOST));//lanmanager->getText(L_NETWORKPROBLEM_TOOLS_BTNROUTE_HOST,uuid));
    pushButton->setText(GENIE2_GET_TEXT(L_NETWORKPROBLEM_TOOLS_BTNROUTE_TRACEROUTE));//lanmanager->getText(L_NETWORKPROBLEM_TOOLS_BTNROUTE_TRACEROUTE,uuid));
    label->setText(GENIE2_GET_TEXT(L_NETWORKPROBLEM_TOOLS_BTNROUTE_WAIT));//lanmanager->getText(L_NETWORKPROBLEM_TOOLS_BTNROUTE_WAIT,uuid));

//    //ViewResult();
//    QString ViewData;
//    QString strTemp;
//    for (int k=0; k<DataList->size(); k++)
//    {
//        strTemp = DataList->value(k);
//        //if ( strTemp != "over" ){
//            //ViewData = ViewData + strTemp + "\n";
//        //}else {
//            //ILanguageManager *lanmanager = MainWindow::sHandler->getLanguageManager();
//            //ViewData = ViewData + "\n";
//            //ViewData = ViewData + lanmanager->getText(L_NETWORKPROBLEM_TOOLS_BTNROUTE_TRACEROUTEOVER,uuid);
//        //}
//    }
//    textBrowser->setText(lanmanager->getText(L_NETWORKPROBLEM_TOOLS_BTNROUTE_TRACEROUTEOVER,uuid));
}

void TraceRoute::linechange()
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
void TraceRoute::keyPressEvent(QKeyEvent *event)
{
    if(event->key() ==Qt::Key_Return)
    {
        routeTest();
    }
}
void TraceRoute::routeTest()
{
    //////told server to const
    GENIE2_STATISTICS("N3");

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
        textBrowser->clear();
        pushButton->setDisabled(true);
        stackedLayout->setCurrentWidget(pi);
        pi->startAnimation();
        pi->setAnimationDelay(50);

        DataList = new QStringList();
        dragon->TraceRoute(host,&DataList);

        //QString s ="h";
        //IClickManager* clickmanage =MainWindow::sHandler->getClickManager();
        //clickmanage->click(s);
    }
}
void TraceRoute::ViewResult()
{
	textBrowser->clear();
    QString ViewData;
    QString strTemp;
    for (int i=0; i<DataList->size(); i++)
    {
        strTemp = DataList->value(i);
        if ( strTemp != "over" ){
            ViewData = ViewData + strTemp + "\n";
        }else {
            ViewData = ViewData + "\n" + GENIE2_GET_TEXT(L_NETWORKPROBLEM_TOOLS_BTNROUTE_TRACEROUTEOVER);//lanmanager->getText(L_NETWORKPROBLEM_TOOLS_BTNROUTE_TRACEROUTEOVER,uuid);
        }
    }
    textBrowser->setText(ViewData);
    //textBrowser->verticalScrollBar()->setSliderDown(true);
    textBrowser->moveCursor(QTextCursor::End);
    widget->hide();
    widget_2->show();
//    DataList->clear();
}

void TraceRoute::tracefinish()
{
    //////2011-03-08
    //QString strOver;
    //strOver = "Finish";
    //ILanguageManager *lanmanager = MainWindow::sHandler->getLanguageManager();
    //strOver = "\n" + lanmanager->getText(L_NETWORKPROBLEM_TOOLS_BTNROUTE_TRACEROUTEOVER,uuid);
    //DataList->append(strOver);
    ViewResult();


    widget->hide();
    pushButton->setDisabled(false);

    //pi->stopAnimation();
    //pi->setHidden(true);

    //DataList->clear();
}

//void TraceRoute::readOutput()
//{
//    output += cmd->readAll();
//    textBrowser->setText(output);
//    textBrowser->moveCursor(QTextCursor::End);
////    textBrowser->verticalScrollBar()->setValue(textBrowser->verticalScrollBar()->maximumHeight());
//}
//void TraceRoute::ProcessFinish(int exitCode, QProcess::ExitStatus)
//{
//    pushButton->setDisabled(false);
//}
