#include "trafficstatistics.h"
#include "ui_trafficstatistics.h"
#include "QGenieMsgDialog.h"
#include "mainwindow.h"
#include "IInfoManager.h"

QString trafficmeterenable;
IManagerHandler *TrafficStatistics::sHandler =NULL;
TrafficStatistics::TrafficStatistics(QWidget *parent) :
    QWidget(parent),mLogmanager(NULL),uimanager(NULL),
    ui(new Ui::TrafficStatistics)
{
    int i=0;
    int j=0;

    ui->setupUi(this);
    uuid = PLUGIN_UUID;

    flag = false;

    progressindicator = ui->widget;

    uimanager = TrafficStatistics::sHandler->getUiManager();

    mLogmanager = TrafficStatistics::sHandler->getLogManager();

    ui->tableWidget->horizontalHeader()->setVisible(false);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->verticalHeader()->setStretchLastSection(true);
    ui->tableWidget->setRowCount(6);
    ui->tableWidget->setColumnCount(5);
    ui->tableWidget->setRowHeight(0,40);
    ui->tableWidget->setColumnWidth(0,140);
    ui->tableWidget->setWordWrap(true);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setItem(0,0,new QTableWidgetItem());
    ui->tableWidget->item(0,0)->setFlags(Qt::NoItemFlags);
    ui->tableWidget->item(0,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidget->setItem(1,0,new QTableWidgetItem());
    ui->tableWidget->item(1,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidget->item(1,0)->setFlags(Qt::NoItemFlags);
    ui->tableWidget->setItem(2,0,new QTableWidgetItem());
    ui->tableWidget->item(2,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidget->item(2,0)->setFlags(Qt::NoItemFlags);
    ui->tableWidget->setItem(3,0,new QTableWidgetItem());
    ui->tableWidget->item(3,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidget->item(3,0)->setFlags(Qt::NoItemFlags);
    ui->tableWidget->setItem(4,0,new QTableWidgetItem());
    ui->tableWidget->item(4,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidget->item(4,0)->setFlags(Qt::NoItemFlags);
    ui->tableWidget->setItem(5,0,new QTableWidgetItem());
    ui->tableWidget->item(5,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidget->item(5,0)->setFlags(Qt::NoItemFlags);
    ui->tableWidget->setItem(0,1,new QTableWidgetItem());
    ui->tableWidget->item(0,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidget->item(0,1)->setFlags(Qt::NoItemFlags);
    ui->tableWidget->setItem(0,2,new QTableWidgetItem());
    ui->tableWidget->item(0,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidget->item(0,2)->setFlags(Qt::NoItemFlags);
    ui->tableWidget->setItem(0,3,new QTableWidgetItem());
    ui->tableWidget->item(0,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidget->item(0,3)->setFlags(Qt::NoItemFlags);
    ui->tableWidget->setItem(0,4,new QTableWidgetItem());
    ui->tableWidget->item(0,4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidget->item(0,4)->setFlags(Qt::NoItemFlags);
//    ui->tableWidget->setSpan(0,2,1,3);

    for(i=1; i <= 5; i++)
    {
        for(j=1; j<=4; j++)
        {
            ui->tableWidget->setItem(i,j,new QTableWidgetItem());
            ui->tableWidget->item(i,j)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            ui->tableWidget->item(i,j)->setFlags(Qt::NoItemFlags);
        }
    }

    SET_STYLE_PROPERTY(FLEX_BUTTON,ui->pushButton);
    SET_STYLE_PROPERTY(FLEX_BUTTON,ui->pushButton_2);
    SET_STYLE_PROPERTY(FLEX_BUTTON,ui->pushButton_3);

    buttongroup = NULL;
    buttongroup = new QButtonGroup();
    if(buttongroup)
    {
        buttongroup->addButton(ui->radioButton);
        buttongroup->addButton(ui->radioButton_2);
    }

    currentselectedbt = NULL;

    ui->tableWidget->setHidden(true);
    ui->pushButton_2->setDisabled(true);
#ifdef PLATFORM_WIN32
    mesdialog = new QGenieMsgDialog(NULL);
#else
    IInfoManager *infom=MainWindow::sHandler->getInfoManager();
    QWidget *sheetParent=(QWidget *)infom->getSharedMemory(INFO_CMD_SHEET_PARENT);
    mesdialog = new QGenieMsgDialog(sheetParent);
#endif

    connect(ui->pushButton, SIGNAL(clicked()), this, SIGNAL(refreshbtclicked()));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(startAnimation()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SIGNAL(modifybtclicked()));
//    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(stopAnimation()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SIGNAL(logoutbtclicked()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(stopAnimation()));
    connect(buttongroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(radiobuttonchange(QRadioButton*)));
    connect(ui->radioButton, SIGNAL(clicked(bool)), this, SLOT(checkedenableradio(bool)));
    connect(ui->radioButton_2, SIGNAL(clicked(bool)), this, SLOT(checkeddisableradio(bool)));
}

TrafficStatistics::~TrafficStatistics()
{
    delete ui;
}

void TrafficStatistics::checkedenableradio(bool clicked)
{
    if(clicked && trafficmeterenable != "1")
    {
        mesdialog->showMsg(L_TRAFFICMETER_MSGBOX_TITLE_MSG,L_TRAFFICMETER_MSGBOX_ENABLE);
        if(mesdialog->mResult==ER_OK)
        {
            if(MainWindow::sHandler){
                MainWindow::sHandler->getClickManager()->click("R11");
            }
            intervaltime="40";
            startextrapply();
            startAnimation();
            settrafficmeter_t data;
            QString tmp="";
            trafficmeterenable = "1";


            data.set(trafficmeterenable, \
                     tmp, \
                     tmp, \
                     tmp, \
                     tmp, \
                     tmp);
            showError(CLEAR_MESSAGE);
            emit settrafficmeterenable(data);
        }
        else
        {
            updateview();
        }
    }
}

void TrafficStatistics::checkeddisableradio(bool clicked)
{
    if(clicked && trafficmeterenable != "0")
    {
        mesdialog->showMsg(L_TRAFFICMETER_MSGBOX_TITLE_MSG,L_TRAFFICMETER_MSGBOX_DISABLE);

        if(mesdialog->mResult==ER_OK)
        {
            if(MainWindow::sHandler){
                MainWindow::sHandler->getClickManager()->click("R12");
            }
            intervaltime="30";
            startextrapply();
            startAnimation();
            settrafficmeter_t data;
            QString tmp="";
            trafficmeterenable = "0";
            ui->tableWidget->setHidden(true);

            data.set(trafficmeterenable, \
                     tmp, \
                     tmp, \
                     tmp, \
                     tmp, \
                     tmp);
            showError(CLEAR_MESSAGE);
            emit settrafficmeterenable(data);
        }
        else
        {
            updateview();
        }
    }
}

void TrafficStatistics::radiobuttonchange(QRadioButton *bt)
{
    qDebug() << "enter!";
    if(currentselectedbt != bt)
    {
        mesdialog->showMsg(L_TRAFFICMETER_MSGBOX_TITLE_MSG,L_TRAFFICMETER_MSGBOX_ENABLE);
        currentselectedbt = bt;
    }
}

void TrafficStatistics::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi();
        //ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void TrafficStatistics::retranslateUi()
{
    showError(SHOW_LASTEST_MESSAGE);
    if(TrafficStatistics::sHandler != NULL)
            {
                lanmanager = TrafficStatistics::sHandler->getLanguageManager();
                if(lanmanager == NULL)
                    return;
                ui->tableWidget->item(0,0)->setText(lanmanager->getText(L_TRAFFICMETER_TITLE_1,uuid));
                ui->tableWidget->item(1,0)->setText(lanmanager->getText(L_TRAFFICMETER_ROW_1,uuid));
                ui->tableWidget->item(2,0)->setText(lanmanager->getText(L_TRAFFICMETER_ROW_2,uuid));
                ui->tableWidget->item(3,0)->setText(lanmanager->getText(L_TRAFFICMETER_ROW_3,uuid));
                ui->tableWidget->item(4,0)->setText(lanmanager->getText(L_TRAFFICMETER_ROW_4,uuid));
                ui->tableWidget->item(5,0)->setText(lanmanager->getText(L_TRAFFICMETER_ROW_5,uuid));
                ui->tableWidget->item(0,1)->setText(lanmanager->getText(L_TRAFFICMETER_TITLE_2,uuid));
                ui->tableWidget->item(0,2)->setText(lanmanager->getText(L_TRAFFICMETER_TITLE_3,uuid));
                ui->tableWidget->item(0,3)->setText(lanmanager->getText(L_TRAFFICMETER_TITLE_4,uuid));
                ui->tableWidget->item(0,4)->setText(lanmanager->getText(L_TRAFFICMETER_TITLE_5,uuid));

                ui->pushButton->setText(lanmanager->getText(L_TRAFFICMETER_REFRESH,uuid));
                ui->pushButton_2->setText(lanmanager->getText(L_TRAFFICMETER_MODIFY,uuid));
                ui->pushButton_3->setText(lanmanager->getText(L_TRAFFICMETER_CANCEL,uuid));

                ui->radioButton->setText(lanmanager->getText(L_TRAFFICMETER_ENABLE,uuid));
                ui->radioButton_2->setText(lanmanager->getText(L_TRAFFICMETER_DISABLE,uuid));
            }
}

void TrafficStatistics::showError(int index)
{
    static int indexst = -3;
    QString message;

    if(index == CLEAR_MESSAGE||(index == SHOW_LASTEST_MESSAGE && indexst == -3))
    {
        message = "";
        indexst = -3;
    }
    else if(index == SHOW_LASTEST_MESSAGE)
    {
        message = lanmanager->getText(indexst,uuid);
    }
    else
    {
        message = lanmanager->getText(index,uuid);
        indexst = index;
    }
    if(ui->label)
        ui->label->setText(QString("<center><font color=\"red\">%1</font></center>").arg(message));
}


void TrafficStatistics::startAnimation()
{
    flag = true;
    int i,j;
    for(i=1; i <= 5; i++)
    {
        for(j=1; j<=4; j++)
        {
            ui->tableWidget->item(i,j)->setText("");
        }
    }
//    showError(L_TRAFFICMETER_MSG_PLEASEWAIT);
    ui->pushButton->setDisabled(true);
    ui->pushButton_2->setDisabled(true);
    ui->pushButton_3->setDisabled(true);
    ui->radioButton->setDisabled(true);
    ui->radioButton_2->setDisabled(true);
    progressindicator->startAnimation();
    startextrarefresh();
}

void TrafficStatistics::stopAnimation()
{
    if(!flag)
    {
        return;
    }
    else
    {
        flag = false;
    }
    ui->pushButton->setDisabled(false);
//    ui->pushButton_2->setDisabled(false);
    ui->pushButton_3->setDisabled(false);
    ui->radioButton->setDisabled(false);
    ui->radioButton_2->setDisabled(false);
    progressindicator->stopAnimation();
    stopextrarefresh();
}

void TrafficStatistics::refreshdata()
{
//    ui->pushButton->setDisabled(false);
}

void TrafficStatistics::response(bool error, int id, gettrafficmeter_t &data)
{
    QString todyconnection,yesterdayconnection,weekconnection,monthconnection,lastmonthconnection;
    QString todayupload,todaydownload,yesterdayupload,yesterdaydownload;
    QString weekupload,weekdownload,monthupload,monthdownload;
    QString lastupload,lastdownload;

    if(error)
    {
//        if(trafficmeterenable == "0")
//        {
//            trafficmeterenable = "1";
//        }
//        else if (trafficmeterenable == "1")
//        {
//            trafficmeterenable = "0";
//        }
    if(id == L_TRAFFICMETER_BOX_1_MESSAGE || id == L_TRAFFICMETER_BOX_2_MESSAGE || id == L_TRAFFICMETER_BOX_3_MESSAGE)
    {
        trafficmeterenable = "2";
    }
        showError(id);
    }
    else
    {
        showError(CLEAR_MESSAGE);
        data.get(todyconnection, todayupload, todaydownload, \
                 yesterdayconnection, yesterdayupload, yesterdaydownload, \
                 weekconnection, weekupload, weekdownload,\
                 monthconnection, monthupload, monthdownload, \
                 lastmonthconnection, lastupload, lastdownload);
        trafficmeterenable = data.gettrafficmeterenable();

        if(trafficmeterenable.isEmpty())
        {
            id = L_TRAFFICMETER_BOX_3_MESSAGE;
            showError(id);
        }

        if(trafficmeterenable == "1")
        {
        ui->tableWidget->item(1,1)->setText(todyconnection);
        ui->tableWidget->item(1,2)->setText(todayupload);
        ui->tableWidget->item(1,3)->setText(todaydownload);
        ui->tableWidget->item(1,4)->setText(data.gettotaltoday());
        ui->tableWidget->item(2,1)->setText(yesterdayconnection);
        ui->tableWidget->item(2,2)->setText(yesterdayupload);
        ui->tableWidget->item(2,3)->setText(yesterdaydownload);
        ui->tableWidget->item(2,4)->setText(data.gettotalyesterday());
        ui->tableWidget->item(3,1)->setText(weekconnection);
        ui->tableWidget->item(3,2)->setText(weekupload+"/"+data.getuploadweekavg());
        ui->tableWidget->item(3,3)->setText(weekdownload+"/"+data.getdownloadweekavg());
        ui->tableWidget->item(3,4)->setText(data.gettotalweek()+"/"+data.gettotalweekavg());
        ui->tableWidget->item(4,1)->setText(monthconnection);
        ui->tableWidget->item(4,2)->setText(monthupload+"/"+data.getuploadmonthavg());
        ui->tableWidget->item(4,3)->setText(monthdownload+"/"+data.getdownloadmonthavg());
        ui->tableWidget->item(4,4)->setText(data.gettotalmonth()+"/"+data.gettotalmonthavg());
        ui->tableWidget->item(5,1)->setText(lastmonthconnection);
        ui->tableWidget->item(5,2)->setText(lastupload+"/"+data.getuploadlastmonthavg());
        ui->tableWidget->item(5,3)->setText(lastdownload+"/"+data.getdownloadlastmonthavg());
        ui->tableWidget->item(5,4)->setText(data.gettotallastmonth()+"/"+data.gettotallastmonthavg());
        }else if(trafficmeterenable == "2")
        {
            showError(id);
        }
        qDebug() << "receive data:"<< todyconnection << todayupload << todaydownload \
                << yesterdayconnection << yesterdayupload << yesterdaydownload \
                << weekconnection << weekupload << weekdownload \
                << data.getuploadweekavg() << data.getdownloadweekavg() << data.gettotalweekavg() \
                << monthconnection << monthupload << monthdownload \
                << data.getuploadmonthavg() << data.getdownloadmonthavg() << data.gettotalmonthavg() \
                << lastmonthconnection << lastupload << lastdownload \
                << data.gettrafficmeterenable() \
                << data.getuploadlastmonthavg() << data.getdownloadlastmonthavg() << data.gettotallastmonthavg();
    }
//    if(trafficmeterenable=="0")
//    {
//        ui->pushButton_2->setDisabled(true);
//    }
//    else
//    {
//        ui->pushButton_2->setDisabled(false);
//        emit gettrafficmeteroptions();
//    }
    qDebug() << "response";
    stopAnimation();
    updateview();
}

void TrafficStatistics::updateview()
{
    if( trafficmeterenable == "0")
    {
        ui->radioButton_2->setChecked(true);
        ui->tableWidget->setHidden(true);
        ui->pushButton_2->setDisabled(true);
    }
    else if( trafficmeterenable == "1")
    {
        ui->radioButton->setChecked(true);
        ui->tableWidget->setHidden(false);
        ui->pushButton_2->setDisabled(false);
    }
    else //if( trafficmeterenable == "2")
    {
        ui->radioButton_2->setChecked(true);
        ui->radioButton->setDisabled(true);
        ui->radioButton_2->setDisabled(true);
        ui->tableWidget->setHidden(true);
        ui->pushButton_2->setDisabled(true);
    }
}

void TrafficStatistics::initdata()
{
    ui->radioButton->setChecked(false);
    ui->radioButton_2->setChecked(false);
    ui->radioButton->setDisabled(true);
    ui->radioButton_2->setDisabled(true);
    ui->tableWidget->setHidden(true);
}

void TrafficStatistics::setwaitstr(QString & str)
{
    if(ui->label)
        ui->label->setText(str);
}

void TrafficStatistics::setrefreshstr(QString &str)
{
    if(ui->label)
        ui->label->setText(str);
}

void TrafficStatistics::startextrapply()
{
    uimanager->command("apply","20");
}

void TrafficStatistics::startextrarefresh()
{
    mLogmanager->writeLog("startrefresh",uuid);
    uimanager->command("startrefresh","");
}

void TrafficStatistics::stopextrarefresh()
{
    mLogmanager->writeLog("endrefresh",uuid);
    uimanager->command("endrefresh","");
}

void TrafficStatistics::response(bool error, int id)
{
    if(error)
    {
//        showError(id);
    }
    else
    {
        showError(CLEAR_MESSAGE);
    }
    qDebug() << "TrafficSetting::response";
//    stopAnimation();
}
