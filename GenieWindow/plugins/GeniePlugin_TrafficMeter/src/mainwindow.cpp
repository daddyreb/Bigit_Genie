#include "mainwindow.h"
#include "messagebox.h"
#include "inc/plugin_ui_def/plugin_ui_def.h"
#include "inc/genie_interface/IUiManager.h"
#include "inc/genie_interface/IInfoManager.h"
#include "inc/multilanguage_def/LanguageTrafficMeter.h"

#include <QVBoxLayout>

IManagerHandler *MainWindow::sHandler =NULL;

MainWindow::MainWindow(QWidget* parent)
    :QWidget(parent)
{
    uuid = "{35b9be0c-b073-4cf8-bc12-d11923dc3d49}";

    trafficstatistics = new TrafficStatistics();
    trafficsetting = new TrafficSetting(this);
    QVBoxLayout *layout = new QVBoxLayout;

//    statckedlayout = new QStackedLayout;
//    statckedlayout->addWidget(trafficstatistics);
//    statckedlayout->addWidget(trafficsetting);
//    layout->addLayout(statckedlayout);

    layout->setContentsMargins(20,0,0,0);
    stackedWidget = new QStackedWidget;
    stackedWidget->addWidget(trafficstatistics);
    stackedWidget->addWidget(trafficsetting);
    layout->addWidget(stackedWidget);
//    stackedWidget->setContentsMargins(0,0,0,0);

    setLayout(layout);

    qRegisterMetaType<settrafficmeter_t>("settrafficmeter_t");
    qRegisterMetaType<gettrafficmeter_t>("gettrafficmeter_t");

    soapproxy = new SoapProxy();

    uimanager = MainWindow::sHandler->getUiManager();

    connect(trafficstatistics, SIGNAL(modifybtclicked()), this, SLOT(changesecondpage()));
//    connect(trafficstatistics, SIGNAL(modifybtclicked()), trafficsetting, SLOT(startAnimation()));
//    connect(trafficstatistics, SIGNAL(modifybtclicked()), soapproxy, SLOT(gettrafficmeteroptions()));
    connect(trafficstatistics, SIGNAL(logoutbtclicked()), this, SLOT(Cancel()));
    connect(trafficstatistics, SIGNAL(refreshbtclicked()), soapproxy, SLOT(get()));
    connect(soapproxy, SIGNAL(response(bool,int,gettrafficmeter_t&)), trafficstatistics, SLOT(response(bool,int,gettrafficmeter_t&)));
    connect(soapproxy, SIGNAL(response(bool,int,settrafficmeter_t&)), trafficsetting, SLOT(response(bool,int,settrafficmeter_t&)));

    connect(trafficsetting, SIGNAL(settrafficmeter(settrafficmeter_t&)), soapproxy, SLOT(settrafficmeter(settrafficmeter_t&)));
    connect(trafficstatistics, SIGNAL(settrafficmeterenable(settrafficmeter_t&)), soapproxy, SLOT(settrafficmeterenable(settrafficmeter_t&)));
    connect(soapproxy, SIGNAL(response(bool,int)), trafficsetting, SLOT(response(bool,int)));
    connect(trafficsetting, SIGNAL(returnbtclicked()),this, SLOT(changefirstpage()));

    connect(this, SIGNAL(applyendsignal()), trafficsetting, SLOT(stopAnimation()));
//    connect(this, SIGNAL(applyendsignal()), trafficstatistics, SLOT(stopAnimation()));
//    connect(this, SIGNAL(applyendsignal()), soapproxy, SLOT(get()));
//    connect(this, SIGNAL(applyendsignal()), trafficstatistics, SLOT(startAnimation()));

    connect(this,SIGNAL(setwaitstrsignal(QString&)), trafficsetting, SLOT(setwaitstr(QString&)));
    connect(this,SIGNAL(setwaitstrsignal(QString&)), trafficstatistics, SLOT(setwaitstr(QString&)));

    connect(this,SIGNAL(setrefreshsignal(QString&)), trafficsetting, SLOT(setrefreshstr(QString&)));
    connect(this,SIGNAL(setrefreshsignal(QString&)), trafficstatistics, SLOT(setrefreshstr(QString&)));
}
MainWindow::~MainWindow()
{

}
//Change Traffic Page Language
void MainWindow::changeEvent(QEvent *event)
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
//Set this Traffic Page text
void MainWindow::retranslateUi()
{
}

//When this checkBox is Checked,Do this
void MainWindow::JudgeEnable()
{
}
//First,do this,Judge whether connect to NetGear
void MainWindow::JudgeNetgear()
{
    qDebug() << "MainWindow::JudgeNetgear()";
    IInfoManager* mInfoManager = MainWindow::sHandler->getInfoManager();
    if(mInfoManager==NULL)
    {
        return;
    }
    else
    {
        QString curType;
        curType.clear();
        curType = mInfoManager->getInfo(INFO_CMD_NETGEAR);

        if(curType =="y")//Now,Connect to the NetGear Router
        {
            trafficstatistics->startAnimation();
//            soapproxy->gettrafficmeter();
            soapproxy->get();
        }
        else if(curType =="n")//Now,don't connect to the NetGear Router
        {
            trafficsetting->response(true,L_TRAFFICMETER_BOX_1_MESSAGE);
        }
        else if(curType =="?")//Don't know Judge the router if it is NetGear Router
        {}
    }
}

//Do this ,Get NetGear Router Information
void MainWindow::GetMessage()
{
}

void MainWindow::GetResponseEnable(QtSoapMessage &message, QUuid &uuid,QString &id)
{
}

void MainWindow::GetTrafficStatistics()
{
}

void MainWindow::GetResponseStatistics(QtSoapMessage &message, QUuid &uuid,QString& id)
{
}

//Refresh
void MainWindow::Refresh()
{
    qDebug() << "MainWindow::Refresh()";
    GetTrafficStatistics();
}

//Apply
void MainWindow::Apply()
{
}

//Cancel
void MainWindow::Cancel()
{
    IUiManager* uimanager = MainWindow::sHandler->getUiManager();
    if(uimanager)
        uimanager->command("logout","");
}

void MainWindow::startAnimation()
{
    static int i = 0;
//    int k = 0;
//    static int j = 0;
////    while(k != 10)
//    {
//        k++;
//        j = j++%2;
//        qDebug() << j;
//    }
//    qDebug() << "startAnimation" << i << statckedlayout->count();
//    i = (++i) % statckedlayout->count();
//    statckedlayout->setCurrentIndex(i);
//    qDebug() << "startAnimation" << i << statckedlayout->count();

    qDebug() << "startAnimation" << i << stackedWidget->count();
    ++i;
    i = i % stackedWidget->count();
    stackedWidget->setCurrentIndex(i);
    qDebug() << "startAnimation" << i << stackedWidget->count();
}

void MainWindow::stopAnimation()
{
}

void MainWindow::initdata()
{
    trafficstatistics->initdata();
}

void MainWindow::changefirstpage()
{
    if(stackedWidget->currentIndex() != 0)
    {
        uimanager->setBackGroundImg(uuid, "");
    //    statckedlayout->setCurrentIndex(0);
        stackedWidget->setCurrentIndex(0);
    }
    else
    {
        if(trafficmeterenable=="1")
        {
            soapproxy->get();
//            trafficstatistics->startextrarefresh();
            trafficstatistics->startAnimation();
        }
        else
        {
            trafficstatistics->stopAnimation();
        }
    }
}

void MainWindow::changefirstpage2()
{
    stackedWidget->setCurrentIndex(0);
}

void MainWindow::changesecondpage()
{
    uimanager->setBackGroundImg(uuid, ":/Images/bjtu.png");
    trafficsetting->initdata();
//    statckedlayout->setCurrentIndex(1);
    stackedWidget->setCurrentIndex(1);
}

void MainWindow::setwaitstr(QString &str)
{
    emit setwaitstrsignal(str);
}

void MainWindow::setrefreshstr(QString &str)
{
    emit setrefreshsignal(str);
}

void MainWindow::applyend()
{
    emit applyendsignal();
    changefirstpage();
    //trafficstatistics->stopAnimation();
}

void MainWindow::refreshend()
{
    trafficstatistics->stopAnimation();
    trafficsetting->stopAnimation();
}
