#include "trafficsetting.h"
#include "ui_trafficsetting.h"
#include "inc/genie_interface/ISettings.h"

IManagerHandler *TrafficSetting::sHandler =NULL;
TrafficSetting::TrafficSetting(QWidget *parent) :
    QWidget(parent),mLogmanager(NULL),uimanager(NULL),
    ui(new Ui::TrafficSetting)
{
    ui->setupUi(this);
    uuid = PLUGIN_UUID;

    flag = false;

    mSettings = NULL;
    mSettings = TrafficSetting::sHandler->getSettings();

    uimanager = TrafficSetting::sHandler->getUiManager();

    mLogmanager = TrafficSetting::sHandler->getLogManager();

    SET_STYLE_PROPERTY(FLEX_BUTTON,ui->pushButton);
    SET_STYLE_PROPERTY(FLEX_BUTTON,ui->pushButton_2);

    progressindicator = ui->widget;

    hourValidator = new QIntValidator(0,11,this);
    minuteValidator = new QIntValidator(0,59, this);
    monthLimitValidator = new QIntValidator(0,1000000, this);
    ui->lineEdit->setValidator(monthLimitValidator);
    ui->lineEdit_2->setValidator(hourValidator);
    ui->lineEdit_3->setValidator(minuteValidator);

    ui->checkBox->setChecked(true);
    ui->checkBox->setHidden(true);

    tmp = "";
    NewControlOption = "No Limit";
    NewMonthlyLimit = "0";
    RestartHour = "0";
    RestartMinute = "0";
    RestartDay = "1";

    initdata();

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(apply()));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(startextraturn()));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(startAnimation()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SIGNAL(returnbtclicked()));
//    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(stopAnimation()));
    connect(ui->checkBox,SIGNAL(clicked()), this, SLOT(checkboxchange()));

    connect(ui->checkBox, SIGNAL(clicked()), this, SLOT(modifyslot()));
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(modifyslot()));
    connect(ui->comboBox_2, SIGNAL(currentIndexChanged(int)), this, SLOT(modifyslot()));
    connect(ui->comboBox_3, SIGNAL(currentIndexChanged(int)), this, SLOT(modifyslot()));
    connect(ui->lineEdit, SIGNAL(textChanged(QString)), this, SLOT(modifyslot()));
    connect(ui->lineEdit_2, SIGNAL(textChanged(QString)), this, SLOT(modifyslot()));
    connect(ui->lineEdit_3, SIGNAL(textChanged(QString)), this, SLOT(modifyslot()));
}

void TrafficSetting::modifyslot()
{
    ui->pushButton->setDisabled(false);
}

void TrafficSetting::initdata()
{
    showError(CLEAR_MESSAGE);
    updateview();
}

TrafficSetting::~TrafficSetting()
{
    delete ui;
}

void TrafficSetting::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
//        ui->retranslateUi(this);
        retranslateUi();
        break;
    default:
        break;
    }
}

QString TrafficSetting::boldword(QString str)
{
    return "<b>"+str+"</b>";
}

void TrafficSetting::retranslateUi()
{
    showError(SHOW_LASTEST_MESSAGE);
    if(TrafficSetting::sHandler != NULL)
      {
        lanmanager = TrafficSetting::sHandler->getLanguageManager();
        if(lanmanager == NULL)
            return;

        ui->checkBox->setText(lanmanager->getText(L_TRAFFICMETER_ENABLETRAFFIC,uuid));
        ui->label_2->setText(boldword(lanmanager->getText(L_TRAFFICMETER_CONTROLBY,uuid)));
        ui->label_6->setText(lanmanager->getText(L_TRAFFICMETER_MBYTES,uuid));
        ui->label_9->setText(boldword(lanmanager->getText(L_TRAFFICMETER_COUNTER,uuid)));
        ui->label->setText(lanmanager->getText(L_TRAFFICMETER_RESTARTCOUNTER,uuid));
        ui->label_3->setText(lanmanager->getText(L_TRAFFICMETER_COUNTER_AT,uuid));
        ui->label_5->setText(lanmanager->getText(L_TRAFFICMETER_COUNTER_DAY,uuid));
        ui->label_8->setText(boldword(lanmanager->getText(L_TRAFFICMETER_MONTHBY,uuid)));

        ui->pushButton->setText(lanmanager->getText(L_TRAFFICMETER_APPLY,uuid));
        ui->pushButton_2->setText(lanmanager->getText(L_TRAFFICMETER_RETURN,uuid));

        ui->comboBox->clear();
        ui->comboBox->insertItems(0, QStringList()
         << QString(lanmanager->getText(L_TRAFFICMETER_NO_LIMIT,uuid))
         << QString(lanmanager->getText(L_TRAFFICMETER_DOWNLOAD_ONLY,uuid))
         << QString(lanmanager->getText(L_TRAFFICMETER_BOTH_DIRECTIONS,uuid))
        );

        ui->comboBox_2->clear();
        ui->comboBox_2->insertItems(0, QStringList()
         << QString(lanmanager->getText(L_TRAFFICMETER_AM,uuid))
         << QString(lanmanager->getText(L_TRAFFICMETER_PM,uuid))
        );

        ui->comboBox_3->clear();
        ui->comboBox_3->insertItems(0, QStringList()
         << QString(lanmanager->getText(L_TRAFFICMETER_01ST,uuid))
         << QString(lanmanager->getText(L_TRAFFICMETER_02ND,uuid))
         << QString(lanmanager->getText(L_TRAFFICMETER_03RD,uuid))
         << QString(lanmanager->getText(L_TRAFFICMETER_04TH,uuid))
         << QString(lanmanager->getText(L_TRAFFICMETER_05TH,uuid))
         << QString(lanmanager->getText(L_TRAFFICMETER_06TH,uuid))
         << QString(lanmanager->getText(L_TRAFFICMETER_07TH,uuid))
         << QString(lanmanager->getText(L_TRAFFICMETER_08TH,uuid))
         << QString(lanmanager->getText(L_TRAFFICMETER_09TH,uuid))
         << QString(lanmanager->getText(L_TRAFFICMETER_10TH,uuid))
         << QString(lanmanager->getText(L_TRAFFICMETER_11TH,uuid))
         << QString(lanmanager->getText(L_TRAFFICMETER_12TH,uuid))
         << QString(lanmanager->getText(L_TRAFFICMETER_13TH,uuid))
         << QString(lanmanager->getText(L_TRAFFICMETER_14TH,uuid))
         << QString(lanmanager->getText(L_TRAFFICMETER_15TH,uuid))
         << QString(lanmanager->getText(L_TRAFFICMETER_16TH,uuid))
         << QString(lanmanager->getText(L_TRAFFICMETER_17TH,uuid))
         << QString(lanmanager->getText(L_TRAFFICMETER_18TH,uuid))
         << QString(lanmanager->getText(L_TRAFFICMETER_19TH,uuid))
         << QString(lanmanager->getText(L_TRAFFICMETER_20TH,uuid))
         << QString(lanmanager->getText(L_TRAFFICMETER_21TH,uuid))
         << QString(lanmanager->getText(L_TRAFFICMETER_22TH,uuid))
         << QString(lanmanager->getText(L_TRAFFICMETER_23TH,uuid))
         << QString(lanmanager->getText(L_TRAFFICMETER_24TH,uuid))
         << QString(lanmanager->getText(L_TRAFFICMETER_25TH,uuid))
         << QString(lanmanager->getText(L_TRAFFICMETER_26TH,uuid))
         << QString(lanmanager->getText(L_TRAFFICMETER_27TH,uuid))
         << QString(lanmanager->getText(L_TRAFFICMETER_28TH,uuid))
         << QString(lanmanager->getText(L_TRAFFICMETER_29TH,uuid))
         << QString(lanmanager->getText(L_TRAFFICMETER_30TH,uuid))
         << QString(lanmanager->getText(L_TRAFFICMETER_31TH,uuid))
        );
      }
}

void TrafficSetting::showError(int index)
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
        ui->label_7->setText(QString("<center><font color=\"red\">%1</font></center>").arg(message));
}

void TrafficSetting::startextraturn()
{
    uimanager->command("apply","20");
}

void TrafficSetting::startAnimation()
{
//    uimanager->command("apply","");
//    showError(L_TRAFFICMETER_MSG_COMMITWAIT);
    flag = true;
    ui->pushButton->setDisabled(true);
    ui->pushButton_2->setDisabled(true);
        ui->lineEdit->setDisabled(true);
        ui->lineEdit_2->setDisabled(true);
        ui->lineEdit_3->setDisabled(true);
        ui->comboBox->setDisabled(true);
        ui->comboBox_2->setDisabled(true);
        ui->comboBox_3->setDisabled(true);
    progressindicator->startAnimation();
    startextrarefresh();
}

void TrafficSetting::stopAnimation()
{
    if(!flag)
    {
        return;
    }
    else
    {
        flag = false;
    }
//    ui->pushButton->setDisabled(false);
    progressindicator->stopAnimation();
    ui->lineEdit->setDisabled(false);
    ui->lineEdit_2->setDisabled(false);
    ui->lineEdit_3->setDisabled(false);
    ui->comboBox->setDisabled(false);
    ui->comboBox_2->setDisabled(false);
    ui->comboBox_3->setDisabled(false);
    ui->pushButton_2->setDisabled(false);
    stopextrarefresh();
}

void TrafficSetting::apply()
{
//    QString NewTrafficMeterEnable;
//    QString NewControlOption;
//    QString NewMonthlyLimit;
//    QString RestartHour;
//    QString RestartMinute;
//    QString RestartDay;
    settrafficmeter_t data;
    int hour;
    int minute;

//    if(ui->checkBox->isChecked())
//    {
//        NewTrafficMeterEnable = "1";
//    }
//    else
//    {
//        NewTrafficMeterEnable = "0";
//    }

    switch(ui->comboBox->currentIndex())
        {
            case 0:
                NewControlOption ="No Limit";
                break;
            case 1:
                NewControlOption ="Download only";
                break;
            case 2:
                NewControlOption ="Both directions";
                break;
        }

    NewMonthlyLimit = ui->lineEdit->text().trimmed();

    RestartHour = ui->lineEdit_2->text().trimmed();
    hour=ui->comboBox_2->currentIndex()==1?RestartHour.toInt()%12+12:RestartHour.toInt()%12;
    RestartHour = QString("%1").arg(hour);

    minute = ui->lineEdit_3->text().trimmed().toInt()%60;
    RestartMinute = QString("%1").arg(minute);

    switch(ui->comboBox_3->currentIndex())
            {
                case 0:
                    RestartDay="1";
                    break;
                case 1:
                    RestartDay="2";
                    break;
                case 2:
                    RestartDay="3";
                    break;
                case 3:
                    RestartDay="4";
                    break;
                case 4:
                    RestartDay="5";
                    break;
                case 5:
                    RestartDay="6";
                    break;
                case 6:
                    RestartDay="7";
                    break;
                case 7:
                    RestartDay="8";
                    break;
                case 8:
                    RestartDay="9";
                    break;
                case 9:
                    RestartDay="10";
                    break;
                case 10:
                    RestartDay="11";
                    break;
                case 11:
                    RestartDay="12";
                    break;
                case 12:
                    RestartDay="13";
                    break;
                case 13:
                    RestartDay="14";
                    break;
                case 14:
                    RestartDay="15";
                    break;
                case 15:
                    RestartDay="16";
                    break;
                case 16:
                    RestartDay="17";
                    break;
                case 17:
                    RestartDay="18";
                    break;
                case 18:
                    RestartDay="19";
                    break;
                case 19:
                    RestartDay="20";
                    break;
                case 20:
                    RestartDay="21";
                    break;
                case 21:
                    RestartDay="22";
                    break;
                case 22:
                    RestartDay="23";
                    break;
                case 23:
                    RestartDay="24";
                    break;
                case 24:
                    RestartDay="25";
                    break;
                case 25:
                    RestartDay="26";
                    break;
                case 26:
                    RestartDay="27";
                    break;
                case 27:
                    RestartDay="28";
                    break;
                case 28:
                    RestartDay="29";
                    break;
                case 29:
                    RestartDay="30";
                    break;
                case 30:
                    RestartDay="31";
                    break;
            }

//    if(mSettings)
//            {
//                mSettings->setValue("plugin/TRAFFICMETER_ENABLETRAFFICMETER", NewTrafficMeterEnable);
//                mSettings->setValue("plugin/TRAFFICMETER_NEWCONTROLOPTION", ui->comboBox->currentIndex());
//                mSettings->setValue("plugin/TRAFFICMETER_NEWMONTHLYLIMIT", NewMonthlyLimit);
//
//                mSettings->setValue("plugin/TRAFFICMETER_RESTARTHOUR", RestartHour);
//                mSettings->setValue("plugin/TRAFFICMETER_RESTARTMINUTE", RestartMinute);
//                mSettings->setValue("plugin/TRAFFICMETER_RESTARTDAY", RestartDay);
//            }

    data.set(tmp, \
             NewControlOption, \
             NewMonthlyLimit, \
             RestartHour, \
             RestartMinute, \
             RestartDay);
    emit settrafficmeter(data);
    qDebug() \
            << "settrafficmeter_t send" \
            << tmp \
            << NewControlOption \
            << NewMonthlyLimit \
            << RestartHour \
            << RestartMinute \
            << RestartDay;
}

void TrafficSetting::response(bool error, int id)
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

void TrafficSetting::response(bool error, int id, settrafficmeter_t &data)
{
    if(error)
    {
        showError(id);
    }
    else
    {
        showError(CLEAR_MESSAGE);
        data.get(tmp ,NewControlOption,NewMonthlyLimit,RestartHour,RestartMinute,RestartDay);
        updateview();
       qDebug() << "TrafficSetting::response d" << tmp << NewControlOption << NewMonthlyLimit << RestartHour << RestartMinute << RestartDay;
    }
    qDebug() << "TrafficSetting::response settrafficmeter_t";
    stopAnimation();
}

void TrafficSetting::checkboxchange()
{
    if(ui->checkBox->isChecked())
    {
        ui->lineEdit->setDisabled(false);
        ui->lineEdit_2->setDisabled(false);
        ui->lineEdit_3->setDisabled(false);
        ui->comboBox->setDisabled(false);
        ui->comboBox_2->setDisabled(false);
        ui->comboBox_3->setDisabled(false);
    }
    else
    {
        ui->lineEdit->setDisabled(true);
        ui->lineEdit_2->setDisabled(true);
        ui->lineEdit_3->setDisabled(true);
        ui->comboBox->setDisabled(true);
        ui->comboBox_2->setDisabled(true);
        ui->comboBox_3->setDisabled(true);
    }
}

void TrafficSetting::setwaitstr(QString & str)
{
    if(ui->label_7)
        ui->label_7->setText(str);
}

void TrafficSetting::setrefreshstr(QString &str)
{
    if(ui->label_7)
        ui->label_7->setText(str);
}

void TrafficSetting::startextrarefresh()
{
    mLogmanager->writeLog("startrefresh",uuid);
    uimanager->command("startrefresh","");
}

void TrafficSetting::stopextrarefresh()
{
    mLogmanager->writeLog("endrefresh",uuid);
    uimanager->command("endrefresh","");
}

void TrafficSetting::updateview()
{
        if(NewControlOption == "No Limit")
        {
            ui->comboBox->setCurrentIndex(0);
        }
        else if(NewControlOption == "Download only")
        {
            ui->comboBox->setCurrentIndex(1);
        }
        else if(NewControlOption == "Both directions")
        {
            ui->comboBox->setCurrentIndex(2);
        }

        ui->lineEdit->setText(NewMonthlyLimit);
        int hour = RestartHour.toInt();
        ui->lineEdit_2->setText(QString("%1").arg(hour%12));
        ui->lineEdit_3->setText(RestartMinute);
        ui->comboBox_2->setCurrentIndex(hour/12>0?1:0);
        ui->comboBox_3->setCurrentIndex(RestartDay.toInt()-1);

    checkboxchange();
    ui->pushButton->setDisabled(true);
}
