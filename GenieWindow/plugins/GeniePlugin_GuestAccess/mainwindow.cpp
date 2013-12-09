#include "mainwindow.h"
#include "inc/genie_interface/IManagerHandler.h"
#include "inc/genie_interface/IInfoManager.h"
#include "inc/genie_interface/IUiManager.h"
#include "inc/plugin_ui_def/plugin_ui_def.h"
#include "inc/genie_interface/ILanguageManager.h"
#include "inc/multilanguage_def/LanguageGuestAccess.h"
#include <QDate>
#include "GeniePluginTest.h"

IManagerHandler *MainWindow::sHandler =NULL;
static int setEnableFlag = 0;
#define INI_SECTION  "GuestAccess"

MainWindow::MainWindow(QWidget* parent)
    :QWidget(parent)
{
    gridLayout_6 = new QGridLayout(this);

    frame = new QFrame(this);
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setFrameShadow(QFrame::Raised);

    gridLayout = new QGridLayout(frame);
    gridLayout->setContentsMargins(9,9,9,0);

    radioButton = new QRadioButton(frame);
    QFont font;
    font.setPointSize(10);
    radioButton->setFont(font);

    gridLayout->addWidget(radioButton, 0, 0, 1, 1);

    horizontalSpacer = new QSpacerItem(213, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout->addItem(horizontalSpacer, 0, 1, 1, 2);

    widget = new QWidget(frame);
    widget->setMinimumSize(QSize(80, 60));
    widget->setMaximumSize(QSize(80, 60));

    stackedLayout = new QStackedLayout(widget);
    pi = new QProgressIndicator(widget);
    pi->setMaximumSize(60,50);
    pi->setMinimumSize(60,40);
    pi->setAnimationDelay(50);
    stackedLayout->addWidget(pi);

    gridLayout->addWidget(widget, 0, 3, 2, 1);

    pushButton = new QPushButton(frame);
    pushButton->setFont(font);
    SET_FIX_SIZE(NORMAL_BUTTON ,pushButton);
    pushButton->setMaximumWidth(80);
    pushButton->setMinimumWidth(80);

    gridLayout->addWidget(pushButton, 0, 4, 1, 1);

    radioButton_2 = new QRadioButton(frame);
    radioButton_2->setFont(font);

    gridLayout->addWidget(radioButton_2, 1, 0, 1, 2);

    horizontalSpacer_2 = new QSpacerItem(200, 24, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout->addItem(horizontalSpacer_2, 1, 2, 1, 1);

    gridLayout_6->addWidget(frame, 0, 0, 1, 1);

    frame_2 = new QFrame(this);
    frame_2->setFrameShape(QFrame::StyledPanel);
    frame_2->setFrameShadow(QFrame::Raised);

    gridLayout_2 = new QGridLayout(frame_2);
    gridLayout_2->setContentsMargins(9,0,9,0);

    label_3 = new QLabel(frame_2);
    label_3->setFont(font);

    gridLayout_2->addWidget(label_3, 0, 0, 1, 1);

    lineEdit = new QLineEdit(frame_2);
    lineEdit->setMinimumSize(QSize(230, 0));
    lineEdit->setMaximumSize(QSize(230, 16777215));

    gridLayout_2->addWidget(lineEdit, 0, 1, 1, 2);

    horizontalSpacer_3 = new QSpacerItem(55, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout_2->addItem(horizontalSpacer_3, 0, 3, 1, 1);

    label_4 = new QLabel(frame_2);
    label_4->setFont(font);

    gridLayout_2->addWidget(label_4, 1, 0, 1, 1);

    comboBox = new QComboBox(frame_2);
    comboBox->setFont(font);

    gridLayout_2->addWidget(comboBox, 1, 1, 1, 1);

    horizontalSpacer_4 = new QSpacerItem(173, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout_2->addItem(horizontalSpacer_4, 1, 2, 1, 2);

    gridLayout_6->addWidget(frame_2, 1, 0, 1, 1);

    frame_3 = new QFrame(this);
    frame_3->setFrameShape(QFrame::StyledPanel);
    frame_3->setFrameShadow(QFrame::Raised);

    gridLayout_3 = new QGridLayout(frame_3);
    gridLayout_3->setContentsMargins(-1, 0, -1, 0);

    label_5 = new QLabel(frame_3);
    QFont font1;
    font1.setPointSize(10);
    font1.setBold(true);
    font1.setWeight(75);
    label_5->setFont(font1);

    gridLayout_3->addWidget(label_5, 0, 0, 1, 1);

    radioButton_3 = new QRadioButton(frame_3);
    radioButton_3->setFont(font);

    gridLayout_3->addWidget(radioButton_3, 1, 0, 1, 1);

    radioButton_4 = new QRadioButton(frame_3);
    radioButton_4->setFont(font);

    gridLayout_3->addWidget(radioButton_4, 2, 0, 1, 1);

    radioButton_5 = new QRadioButton(frame_3);
    radioButton_5->setFont(font);

    gridLayout_3->addWidget(radioButton_5, 3, 0, 1, 1);

    line_3 = new QFrame(frame_3);
    line_3->setFrameShape(QFrame::HLine);
    line_3->setFrameShadow(QFrame::Sunken);

    gridLayout_3->addWidget(line_3, 4, 0, 1, 1);

    gridLayout_6->addWidget(frame_3, 2, 0, 1, 1);

    frame_4 = new QFrame(this);
    frame_4->setFrameShape(QFrame::StyledPanel);
    frame_4->setFrameShadow(QFrame::Raised);

    gridLayout_4 = new QGridLayout(frame_4);
    gridLayout_4->setContentsMargins(-1, 0, -1, 0);

    label_6 = new QLabel(frame_4);
    label_6->setFont(font1);

    gridLayout_4->addWidget(label_6, 0, 0, 1, 3);

    label_7 = new QLabel(frame_4);
    label_7->setFont(font);

    gridLayout_4->addWidget(label_7, 1, 0, 1, 1);

    lineEdit_2 = new QLineEdit(frame_4);

    gridLayout_4->addWidget(lineEdit_2, 1, 1, 1, 1);

    label_8 = new QLabel(frame_4);
    label_8->setFont(font);

    gridLayout_4->addWidget(label_8, 1, 2, 1, 1);

    line_4 = new QFrame(frame_4);
    line_4->setFrameShape(QFrame::HLine);
    line_4->setFrameShadow(QFrame::Sunken);

    gridLayout_4->addWidget(line_4, 2, 0, 1, 3);

    gridLayout_6->addWidget(frame_4, 3, 0, 1, 1);

    frame_5 = new QFrame(this);
    frame_5->setFrameShape(QFrame::StyledPanel);
    frame_5->setFrameShadow(QFrame::Raised);

    gridLayout_5 = new QGridLayout(frame_5);
    gridLayout_5->setHorizontalSpacing(9);
    gridLayout_5->setVerticalSpacing(0);
    gridLayout_5->setContentsMargins(-1, 0, -1, 0);

    horizontalSpacer_5 = new QSpacerItem(194, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout_5->addItem(horizontalSpacer_5, 0, 0, 1, 1);

    pushButton_2 = new QPushButton(frame_5);
    pushButton_2->setFont(font);
    SET_FIX_SIZE(NORMAL_BUTTON ,pushButton_2);

    gridLayout_5->addWidget(pushButton_2, 0, 1, 1, 1);

    pushButton_3 = new QPushButton(frame_5);
    pushButton_3->setFont(font);
    SET_FIX_SIZE(NORMAL_BUTTON ,pushButton_3);

    gridLayout_5->addWidget(pushButton_3, 0, 2, 1, 1);

    horizontalSpacer_6 = new QSpacerItem(194, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout_5->addItem(horizontalSpacer_6, 0, 3, 1, 1);

    gridLayout_6->addWidget(frame_5, 4, 0, 1, 1);

    comboBox->clear();
    comboBox->insertItems(0, QStringList()
     << QString(tr("1 hour"))
     << QString(tr("5 hour"))
     << QString(tr("10 hour"))
     << QString(tr("1 day"))
     << QString(tr("1 week"))
    );
    radioButton_3->setText(QString(tr("None")));
    radioButton_4->setText(QString(tr("WPA2-PSK[AES]")));
    radioButton_5->setText(QString(tr("WPA-PSK [TKIP] + WPA2-PSK [AES]")));

    pushButton_2->setDisabled(true);
    radioButton_3->setChecked(true);
    frame_4->hide();

    timer = new QTimer(this);
    Worktimer = new QTimer(this);
    Closetimer = new QTimer(this);

    if(MainWindow::sHandler ==NULL)
        return;
    soapmanager= MainWindow::sHandler->getSoapManager();
    soapapi=soapmanager->getSoapAPI();

    g_settings =MainWindow::sHandler->getSettings();

    //Get Router Configuration point
    QPluginLoader loader_Router(QString("GeniePlugin_RouterConfiguration.dll"));
    s_router_plugin = qobject_cast<IPluginInf*>(loader_Router.instance());

    uuid ="{0a1e6c13-c838-47e0-9217-6ef881af5641}";

    connect(Closetimer,SIGNAL(timeout()),this,SLOT(AgainWork()));
    connect(Worktimer,SIGNAL(timeout()),this,SLOT(DoneWorktimer()));
    connect(timer,SIGNAL(timeout()),this,SLOT(TimerOutDone()));
    connect(radioButton,SIGNAL(clicked()),this,SLOT(DisableGuestAccess()));
    connect(radioButton_2,SIGNAL(clicked()),this,SLOT(EnableGuestAccess()));
    connect(radioButton_3,SIGNAL(clicked()),this,SLOT(nonechange()));
    connect(radioButton_4,SIGNAL(clicked()),this,SLOT(WPA2change()));
    connect(radioButton_5,SIGNAL(clicked()),this,SLOT(WPAAllchange()));
    connect(pushButton_2,SIGNAL(clicked()),this,SLOT(JudgeWheterNetGear()));
    connect(pushButton_3,SIGNAL(clicked()),this,SLOT(Cancel()));
    connect(pushButton,SIGNAL(clicked()),this,SLOT(Refresh()));
    connect(lineEdit,SIGNAL(textEdited(QString)),this,SLOT(DataChanged(QString)));
    connect(lineEdit_2,SIGNAL(textEdited(QString)),this,SLOT(DataChanged(QString)));
    connect(comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(WorkTimeChanged(int)));

    retranslateUi();
}
//Destructors
MainWindow::~MainWindow()
{

}
//Change this Page Language
void MainWindow::changeEvent(QEvent* event)
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
//Set this Page text
void MainWindow::retranslateUi()
{
    languagemanager = MainWindow::sHandler->getLanguageManager();
    radioButton->setText(languagemanager->getText(L_GUESTACCESS_DISABLEGUEST,uuid));
    radioButton_2->setText(languagemanager->getText(L_GUESTACCESS_ENABLEGUEST,uuid));
    label_3->setText(languagemanager->getText(L_GUESTACCESS_GUESTWIRELESS_SSID,uuid));
    label_4->setText(languagemanager->getText(L_GUESTACCESS_WORKTIME,uuid));
    label_5->setText(languagemanager->getText(L_GUESTACCESS_SECURITYOPTIONS,uuid));
    label_6->setText(languagemanager->getText(L_GUESTACCESS_SECURITY,uuid));
    label_7->setText(languagemanager->getText(L_GUESTACCESS_PASSPHRASE,uuid));
    label_8->setText(languagemanager->getText(L_GUESTACCESS_PASS_LABLE,uuid));
    pushButton_2->setText(languagemanager->getText(L_GUESTACCESS_APPLY,uuid));
    pushButton_3->setText(languagemanager->getText(L_GUESTACCESS_CANCEL,uuid));
    pushButton->setText(languagemanager->getText(L_GUESTACCESS_REFRESH,uuid));
}
//When Security options is None,Do it
void MainWindow::nonechange()
{
    if(radioButton_3->isChecked())
    {
        frame_4->hide();
        pushButton_2->setDisabled(false);
    }
}
//When Security options is WPA2,Do it
void MainWindow::WPA2change()
{
    if(radioButton_4->isChecked())
    {
        label_6->setText(languagemanager->getText(L_GUESTACCESS_SECURITY,uuid) + "<B>(WPA-PSK)</B>");
        frame_4->show();
        pushButton_2->setDisabled(false);
    }
}
//When Security options is WPA-PSK + WPA2-PSK,Do it
void MainWindow::WPAAllchange()
{
    if(radioButton_5->isChecked())
    {
        label_6->setText(languagemanager->getText(L_GUESTACCESS_SECURITY,uuid) + "<B>(WPA-PSK + WPA2-PSK)</B>");
        frame_4->show();
        pushButton_2->setDisabled(false);
    }
}
//When click the Guest Access Button,Judge current router whether is NetGear Router
void MainWindow::JudgeRouter()
{
    //Set timer Start and set Pi Start.
    StartPi();
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
        if(curType =="y")//The Current Router is NetGear Router
        {
            GetNetGearMac();
        }
        if(curType =="n")//The current Router isnot NetGear Router
        {
            QString title =languagemanager->getText(L_GUESTACCESS_BOX_3_TITLE,uuid);
            QString message =languagemanager->getText(L_GUESTACCESS_BOX_3_MESSAGE,uuid);
            box_3 = new MessageBox(message,title);
            box_3->show();
            StopPi();
        }
        if(curType =="?"){}
    }
}
//First,Get the Current  Router Mac Address
void MainWindow::GetNetGearMac()
{
    disconnect(soapapi,SIGNAL(getResponse(QtSoapMessage&,QUuid&,QString&)),0,0);
    QString ModeName ="WLANConfiguration";
    QString MethodName ="GetInfo";
    QMap<QString , QString> params;
    QString id ="CurrentMac";
    connect(soapapi,SIGNAL(getResponse(QtSoapMessage&,QUuid&,QString&)),this,SLOT(GetMacResponse(QtSoapMessage&,QUuid&,QString&)));
    soapapi->submitrequest(ModeName,MethodName,params,uuid,id);
}
void MainWindow::GetMacResponse(QtSoapMessage &message, QUuid &uuid, QString &id)
{
    if(uuid =="{0a1e6c13-c838-47e0-9217-6ef881af5641}" && id =="CurrentMac")
    {
        QString qureyfieldsstr;
        QStringList queryfieldlist;
        qureyfieldsstr = "NewEnable,NewSSIDBroadcast,NewStatus,NewSSID,NewRegion,NewChannel,NewWirelessMode,NewBasicEncryptionModes,NewWEPAuthType,NewWPAEncryptionModes,NewWLANMACAddress";
        queryfieldlist = qureyfieldsstr.split(",");
        if (message.isFault())
        {
            qDebug("Error: %s", message.faultString().value().toString().toLatin1().constData());
            QString title =languagemanager->getText(L_GUESTACCESS_BOX_4_TITLE,uuid);
            QString message =languagemanager->getText(L_GUESTACCESS_BOX_4_MESSAGE,uuid);
            box_4 = new MessageBox(message,title);
            box_4->show();
            StopPi();
        }
        else
        {
            const QtSoapType &res = message.returnValue();
            if( message.returnResponseCode().toString().toInt() == 0)
            {
                for (int i = 0; i < queryfieldlist.size(); ++i)
                {
                    if(queryfieldlist.at(i)=="NewWLANMACAddress")
                    {
                        Mac = res[queryfieldlist.at(i)].value().toString();
                        qDebug()<<"Current Mac Address:"<<Mac;
                        GetSSID();
                    }
                }
            }
        }
    }
    else
    {
        StopPi();
    }
}
//Get the current router ssid when it is NetGear Router
void MainWindow::GetSSID()
{
    disconnect(soapapi,SIGNAL(getResponse(QtSoapMessage&,QUuid&,QString&)),0,0);
    QString ModeName ="WLANConfiguration";
    QString MethodName ="GetGuestAccessEnabled";
    QMap<QString , QString> params;
    QString id ="getenable";
    connect(soapapi,SIGNAL(getResponse(QtSoapMessage&,QUuid&,QString&)),this,SLOT(getResponse(QtSoapMessage&,QUuid&,QString&)));
    soapapi->submitrequest(ModeName,MethodName,params,uuid,id);
}
//Get a Signal to get this current router SSID
void MainWindow::getResponse(QtSoapMessage &message, QUuid &uuid,QString& id)
{
    if(uuid =="{0a1e6c13-c838-47e0-9217-6ef881af5641}" && id =="getenable")
    {
        QString qureyfieldsstr;
        QStringList queryfieldlist;
        qureyfieldsstr ="NewGuestAccessEnabled";
        queryfieldlist = qureyfieldsstr.split(",");
        if (message.isFault())
        {
            qDebug("Error: %s", message.faultString().value().toString().toLatin1().constData());
            QString title =languagemanager->getText(L_GUESTACCESS_BOX_4_TITLE,uuid);
            QString message =languagemanager->getText(L_GUESTACCESS_BOX_4_MESSAGE,uuid);
            box_4 = new MessageBox(message,title);
            box_4->show();
            StopPi();
        }
        else
        {
            const QtSoapType &res = message.returnValue();
            if( message.returnResponseCode().toString().toInt() == 0)
            {
                for (int i = 0; i < queryfieldlist.size(); ++i)
                {
                    if(queryfieldlist.at(i) =="NewGuestAccessEnabled")
                    {
                        guestenable = res[queryfieldlist.at(i)].value().toInt();
                        switch(guestenable)
                        {
                            case 0://disable
                                {
                                    radioButton->setChecked(true);
                                    StopPi();
                                    DisableGuestAccess();
                                    pushButton_2->setDisabled(true);
                                    setEnableFlag = 1;//The First,set disable
                                    frame_4->hide();
                                    radioButton_3->setChecked(true);
                                    lineEdit->clear();
                                    break;
                                }
                            case 1://enable
                                {
                                    radioButton_2->setChecked(true);
                                    //And then,get the current router's password when it is NetGear Router
                                    Security();
                                    break;
                                }
                            case 2://not support
                                {
                                    StopPi();
                                    this->setDisabled(true);
                                    break;
                                }
                        }
                    }
                }
            }
        }
    }
}
//Then,Get this NetGear Router Connection's password and Encryption
void MainWindow::Security()
{
    disconnect(soapapi,SIGNAL(getResponse(QtSoapMessage&,QUuid&,QString&)),0,0);
    QString ModeName ="WLANConfiguration:";
    QString MethodName ="GetGuestAccessNetworkInfo";
    QMap<QString , QString> params;
    QString id = "getstatusinfo";
    connect(soapapi, SIGNAL(getResponse(QtSoapMessage&,QUuid&,QString&)),this,SLOT(getSecurityResponse(QtSoapMessage&,QUuid&,QString&)));
    soapapi->submitrequest(ModeName,MethodName,params,uuid,id);
}
//Get Signal to Get this NetGear Router Guest Access connecttion's password and Encryption
void MainWindow::getSecurityResponse(QtSoapMessage &message, QUuid &uuid,QString& id)
{
    if(uuid =="{0a1e6c13-c838-47e0-9217-6ef881af5641}" && id =="getstatusinfo")
    {
        QString qureyfieldsstr;
        QStringList queryfieldlist;
        qureyfieldsstr ="NewSSID,NewSecurityMode,NewKey";
        queryfieldlist = qureyfieldsstr.split(",");
        if (message.isFault())
        {
            qDebug("Error: %s", message.faultString().value().toString().toLatin1().constData());
            QString title =languagemanager->getText(L_GUESTACCESS_BOX_4_TITLE,uuid);
            QString message =languagemanager->getText(L_GUESTACCESS_BOX_4_MESSAGE,uuid);
            box_4 = new MessageBox(message,title);
            box_4->show();
            StopPi();
        }
        else
        {
            const QtSoapType &res = message.returnValue();
            if( message.returnResponseCode().toString().toInt() == 0)
            {
                for (int i = 0; i < queryfieldlist.size(); ++i)
                {
                    if(queryfieldlist.at(i) =="NewSSID")
                    {
                        guestssid = res[queryfieldlist.at(i)].value().toString();
                        lineEdit->setText(guestssid);
                    }
                    if(queryfieldlist.at(i) =="NewSecurityMode")
                    {
                        guestsecurityMode =res[queryfieldlist.at(i)].value().toString();
                        if(guestsecurityMode=="None")
                        {
                            radioButton_3->setChecked(true);
                            nonechange();
                        }
                        if(guestsecurityMode=="WPA2-PSK")
                        {
                            radioButton_4->setChecked(true);
                            WPA2change();
                        }
                        if(guestsecurityMode=="Mixed WPA")
                        {
                            radioButton_5->setChecked(true);
                            WPAAllchange();
                        }
                    }
                    if(queryfieldlist.at(i) =="NewKey")
                    {
                        guestkey =res[queryfieldlist.at(i)].value().toString();
                        lineEdit_2->setText(guestkey);
                        GetWorkTimeFromInI();
                    }
                }
            }
        }
    }
    worktime = comboBox->currentIndex();
}
//When Click the Apply Button, and send these Page Message to the current NetGear Router to Modify Data
void MainWindow::Apply()
{
    //Get the current time
    QDateTime time = QDateTime::currentDateTime();
    QString data = time.toString("yyyy.MM.dd.hh:mm:ss");
    int year =time.toString("yyyy").toInt();
    int month =time.toString("MM").toInt();
    int day = time.toString("dd").toInt();
    int hour = time.toString("hh").toInt();
    int minitus =time.toString("mm").toInt();
    int second = time.toString("ss").toInt();

    qDebug()<<data<<month<<day<<hour;

    QMap<QString,QString> params;
    QString  qureyfieldsstr, modulenamestr, methodnamestr,id;
    QStringList queryfieldlist;

    if(radioButton->isChecked())//Disable Guest Network
    {
        QString status ="0";
        modulenamestr ="WLANConfiguration";
        methodnamestr ="SetGuestAccessEnabled";
        id ="SetGuestAccessStatus";
        qureyfieldsstr ="NewGuestAccessEnabled," + status;

        QString timer =GenieMac;
        QString Data ="00";
        save_key(timer,Data);
    }
    else if(radioButton_2->isChecked())//Enable Guest Network
    {
        //Get this Settings work time,and set the timer to start
        int TimeData = GetWorktime();
        Worktimer->start(TimeData);

        //Write the Over time to the Ini file
        QString currentMac = GenieMac;
        int temp;
        switch(TimeData)
        {
            case 3600000://One Hour
                {
                    hour = hour + 1;
                    temp=1;
                    break;
                }
            case 18000000://Five Hour
                {
                    hour = hour + 5;
                    temp=2;
                    break;
                }
            case 36000000://Ten Hour
                {
                    hour = hour + 10;
                    temp=3;
                    break;
                }
            case 86400000://One Day
                {
                    day = day + 1;
                    temp=4;
                    break;
                }
            case 604800000://One Week
                {
                    day = day + 7;
                    temp=5;
                    break;
                }
        }
        QString Settime = SettingTime(year,month,day,hour,minitus,second) + QString::number(temp);
        save_key(currentMac,Settime);

        //When last set Disable,and then first Set Guest Network Enable
        if(setEnableFlag ==1)
        {
            QString status ="1";
            SetGuestNetworkEnable(status);
        }

        //Get this Page'Data to send to the NetGear Router
        geniessid =lineEdit->text();
        if(radioButton_3->isChecked())
        {
            geniesecurityMode ="None";
            geniekey ="";
        }
        if(radioButton_4->isChecked())
        {
            geniesecurityMode ="WPA2-PSK";
            geniekey =lineEdit_2->text();
        }
        if(radioButton_5->isChecked())
        {
            geniesecurityMode="Mixed WPA";
            geniekey =lineEdit_2->text();
        }
        modulenamestr ="WLANConfiguration";
        methodnamestr ="SetGuestAccessNetwork";
        id ="modifyguestaccess";
        qureyfieldsstr="NewSSID," + geniessid + "," + "NewSecurityMode," + geniesecurityMode + "," + "NewKey1," + geniekey +",NewKey2,0,NewKey3,0,NewKey4,0";
    }
    queryfieldlist = qureyfieldsstr.split(",");
    params.clear();
    for (int i = 0; i+1 < queryfieldlist.count(); i+=2)
    {
        params.insert(queryfieldlist.at(i), queryfieldlist.at(i+1));
    }
    soapapi->submitrequest(modulenamestr, methodnamestr, params,uuid,id);

    disconnect(soapapi,SIGNAL(getResponse(QtSoapMessage&,QUuid&,QString&)),0,0);

    StopPi();

    QPluginLoader loader_Router(QString("GeniePlugin_RouterConfiguration.dll"));
    m_Router_plugin = qobject_cast<IPluginInf*>(loader_Router.instance());
    if(m_Router_plugin)
    {
        m_Router_plugin->command(QString("GuestAccess_LoginPage"),111111);
        m_Router_plugin->command(QString("GuestAccess_Modify"),222222);
    }
}
//When Click the Cancel Button, Now go to the Home Page
void MainWindow::Cancel()
{
    if(s_router_plugin)
    {
        s_router_plugin->command(QString("GuestAccess_LoginPage"),111111);
    }
}
//When Click the Refresh Button ,do it
void MainWindow::Refresh()
{
    JudgeRouter();
}
//When Start this Page,do it
void MainWindow::StartPi()
{
    timer->start(60000);
    pi->startAnimation();
    radioButton->setDisabled(true);
    radioButton_2->setDisabled(true);
    lineEdit->setDisabled(true);
    comboBox->setDisabled(true);
    pushButton_3->setDisabled(true);
    radioButton_3->setDisabled(true);
    radioButton_4->setDisabled(true);
    radioButton_5->setDisabled(true);
    lineEdit_2->setDisabled(true);
    pushButton->setDisabled(true);
    pushButton_2->setDisabled(true);
}
//When get message is ok,do it
void MainWindow::StopPi()
{
    timer->stop();
    pi->stopAnimation();
    radioButton->setDisabled(false);
    radioButton_2->setDisabled(false);
    lineEdit->setDisabled(false);
    comboBox->setDisabled(false);
    pushButton_3->setDisabled(false);
    radioButton_3->setDisabled(false);
    radioButton_4->setDisabled(false);
    radioButton_5->setDisabled(false);
    lineEdit_2->setDisabled(false);
    pushButton->setDisabled(false);
    pushButton_2->setDisabled(false);
}
//When timer is out,do it
void MainWindow::TimerOutDone()
{
    StopPi();
    QString title =languagemanager->getText(L_GUESTACCESS_BOX_2_TITLE,uuid);
    QString message =languagemanager->getText(L_GUESTACCESS_BOX_2_MESSAGE,uuid);
    box_2 = new MessageBox(message,title);
    box_2->show();
}
//When Click the Disable radiobutton
void MainWindow::DisableGuestAccess()
{
    lineEdit->setDisabled(true);
    comboBox->setDisabled(true);
    radioButton_3->setDisabled(true);
    radioButton_4->setDisabled(true);
    radioButton_5->setDisabled(true);
    lineEdit_2->setDisabled(true);
    pushButton_2->setDisabled(false);
}
//When Click the Enabel radiobutton
void MainWindow::EnableGuestAccess()
{
    lineEdit->setDisabled(false);
    comboBox->setDisabled(false);
    radioButton_3->setDisabled(false);
    radioButton_4->setDisabled(false);
    radioButton_5->setDisabled(false);
    lineEdit_2->setDisabled(false);
    pushButton_2->setDisabled(false);
}
//When SSID or Password changed
void MainWindow::DataChanged(QString data)
{
    pushButton_2->setDisabled(false);
}
//When work time changed
void MainWindow::WorkTimeChanged(int time)
{
    if(time != worktime)
    {
        pushButton_2->setDisabled(false);
    }
}
//Set Guest network Enable
void MainWindow::SetGuestNetworkEnable(QString status)
{
    QMap<QString,QString> params;
    QString  qureyfieldsstr, modulenamestr, methodnamestr,id;
    QStringList queryfieldlist;
    modulenamestr ="WLANConfiguration";
    methodnamestr ="SetGuestAccessEnabled";
    id ="SetGuestAccessStatus";
    qureyfieldsstr ="NewGuestAccessEnabled," + status;
    queryfieldlist = qureyfieldsstr.split(",");
    params.clear();
    for (int i = 0; i+1 < queryfieldlist.count(); i+=2)
    {
        params.insert(queryfieldlist.at(i), queryfieldlist.at(i+1));
    }
    soapapi->submitrequest(modulenamestr, methodnamestr, params,uuid,id);
}
//Save this Work time to ini file
void MainWindow::save_key(const QString &time, const QString &data)
{
    if(!g_settings)
    {
        return;
    }
    QString strkey(INI_SECTION);
    strkey+="/";
    strkey += "Mac";
    QString str_data =QString::fromAscii(time.toStdString().c_str());
    g_settings->setValue(strkey,str_data);
    QString initimer = data.left(data.length()-1).trimmed();
    QString inino =data.right(1).trimmed();
    strkey += "Time";
    str_data =QString::fromAscii(initimer.toStdString().c_str());
    g_settings->setValue(strkey,str_data);
    strkey += "No";
    str_data =QString::fromAscii(inino.toStdString().c_str());
    g_settings->setValue(strkey,str_data);
}
//Load this Work time in ini file
QString MainWindow::Load_key(const QString &timer)
{
    if(!g_settings)
    {
        return QString();
    }
    QString strkey(INI_SECTION);
    strkey+="/";
    strkey+= QString::fromAscii(timer.toStdString().c_str());
    QString str_data = g_settings->value(strkey).toString();
    str_data = QString::fromAscii(str_data.toStdString().c_str());
    return str_data;
}
//Get this Setting Work time
int MainWindow::GetWorktime()
{
    int data;
    switch(comboBox->currentIndex())
    {
        case 0:
        {
            data =3600000;
//            data =120000;
            break;
        }
        case 1:
        {
            data =18000000;
            break;
        }
        case 2:
        {
            data =36000000;
            break;
        }
        case 3:
        {
            data =86400000;
            break;
        }
        case 4:
        {
            data =604800000;
            break;
        }
    }
    return data;
}
//Work timer,when Genie is still work ,don't close
void MainWindow::DoneWorktimer()
{
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
        if(curType =="y")//The Current Router is NetGear Router
        {
            GetTheCurRouterMac();
        }
        else if(curType =="n")
        {
            QString title =languagemanager->getText(L_GUESTACCESS_BOX_3_TITLE,uuid);
            QString message =languagemanager->getText(L_GUESTACCESS_BOX_3_MESSAGE,uuid);
            box_3 = new MessageBox(message,title);
            box_3->show();
            StopPi();
        }
        else if(curType =="?"){}
    }
}
//Judge the current router wheter is NetGear
void MainWindow::JudgeWheterNetGear()
{
    StartPi();
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
        if(curType =="y")//The Current Router is NetGear Router
        {
            GetCurrentMac();
        }
        else if(curType =="n")
        {
            QString title =languagemanager->getText(L_GUESTACCESS_BOX_3_TITLE,uuid);
            QString message =languagemanager->getText(L_GUESTACCESS_BOX_3_MESSAGE,uuid);
            box_3 = new MessageBox(message,title);
            box_3->show();
             StopPi();
        }
        else if(curType =="?"){}
    }
}
//When Click the Apply Button,Get the current Router Mac Address
void MainWindow::GetCurrentMac()
{
    disconnect(soapapi,SIGNAL(getResponse(QtSoapMessage&,QUuid&,QString&)),0,0);
    QString ModeName ="WLANConfiguration";
    QString MethodName ="GetInfo";
    QMap<QString , QString> params;
    QString id ="getMac";
    connect(soapapi,SIGNAL(getResponse(QtSoapMessage&,QUuid&,QString&)),this,SLOT(GetRouterMacResponse(QtSoapMessage&,QUuid&,QString&)));
    soapapi->submitrequest(ModeName,MethodName,params,uuid,id);
}
void MainWindow::GetRouterMacResponse(QtSoapMessage &message, QUuid &uuid, QString &id)
{
    if(uuid =="{0a1e6c13-c838-47e0-9217-6ef881af5641}")
    {
        QString qureyfieldsstr;
        QStringList queryfieldlist;
        qureyfieldsstr = "NewEnable,NewSSIDBroadcast,NewStatus,NewSSID,NewRegion,NewChannel,NewWirelessMode,NewBasicEncryptionModes,NewWEPAuthType,NewWPAEncryptionModes,NewWLANMACAddress";
        queryfieldlist = qureyfieldsstr.split(",");
        if (message.isFault())
        {
            qDebug("Error: %s", message.faultString().value().toString().toLatin1().constData());
            QString title =languagemanager->getText(L_GUESTACCESS_BOX_4_TITLE,uuid);
            QString message =languagemanager->getText(L_GUESTACCESS_BOX_4_MESSAGE,uuid);
            box_4 = new MessageBox(message,title);
            box_4->show();
            StopPi();
        }
        else
        {
            const QtSoapType &res = message.returnValue();
            if( message.returnResponseCode().toString().toInt() == 0)
            {
                for (int i = 0; i < queryfieldlist.size(); ++i)
                {
                    if(queryfieldlist.at(i)=="NewWLANMACAddress")
                    {
                        GenieMac = res[queryfieldlist.at(i)].value().toString();
                        if(GenieMac == Mac)
                        {
                            QString inthisation =languagemanager->getText(L_GUESTACCESS_BOX_MESSAGE,uuid);
                            QString title =languagemanager->getText(L_GUESTACCESS_BOX_TITLE,uuid);
                            box = new AgainMessageBox(inthisation,title);
                            connect(box,SIGNAL(ChangePage()),this,SLOT(Apply()));
                            connect(box,SIGNAL(StopthisPage()),this,SLOT(WhenCancel()));
                            box->show();
                            timer->stop();
                        }
                        else
                        {
                            StopPi();
                        }
                    }
                }
            }
        }
    }
    else
    {
        StopPi();
    }
}
//When the Dialog box click the Cancel Button
void MainWindow::WhenCancel()
{
    StopPi();
}
//Set the time Format
QString MainWindow::SettingTime(int year, int month, int day, int hour, int minitus, int second)
{
    //According to to calculate
    if(hour >=24)
    {
        day++;
        hour = hour - 24;
        switch(month)
        {
            case 1:
                {
                    if(day >31)
                    {
                        month =2;
                        day = day -31;
                    }
                    break;
                }
            case 2:
                {
                    if((year%4==0&&year%100!=0)||(year%400==0))//leap-year
                    {
                        if(day>29)
                        {
                            day = day -29;
                            month =3;
                        }
                    }
                    else
                    {
                        if(day > 28)
                        {
                            day = day -28;
                            month =3;
                        }
                    }
                    break;
                }
            case 3:
                {
                    if(day > 31)
                    {
                        day = day -31;
                        month =4;
                    }
                    break;
                }
            case 4:
                {
                    if(day >30)
                    {
                        day = day -30;
                        month =5;
                    }
                    break;
                }
            case 5:
                {
                    if(day >31)
                    {
                        day = day -31;
                        month =6;
                    }
                    break;
                }
            case 6:
                {
                    if(day>30)
                    {
                        day = day -30;
                        month =7;
                    }
                    break;
                }
            case 7:
                {
                    if(day>31)
                    {
                        day = day -31;
                        month =8;
                    }
                    break;
                }
            case 8:
                {
                    if(day>31)
                    {
                        day = day -31;
                        month =9;
                    }
                    break;
                }
            case 9:
                {
                    if(day>30)
                    {
                        day = day -30;
                        month =10;
                    }
                    break;
                }
            case 10:
                {
                    if(day>31)
                    {
                        day = day -31;
                        month =11;
                    }
                    break;
                }
            case 11:
                {
                    if(day>30)
                    {
                        day = day -30;
                        month =12;
                    }
                    break;
                }
            case 12:
                {
                    if(day >31)
                    {
                        month =1;
                        day = day -31;
                        year++;
                    }
                    break;
                }
        }
    }
    else
    {
        switch(month)
        {
            case 1:
                {
                    if(day >31)
                    {
                        month =2;
                        day = day -31;
                    }
                    break;
                }
            case 2:
                {
                    if((year%4==0&&year%100!=0)||(year%400==0))//leap-year
                    {
                        if(day>29)
                        {
                            day = day -29;
                            month =3;
                        }
                    }
                    else
                    {
                        if(day > 28)
                        {
                            day = day -28;
                            month =3;
                        }
                    }
                    break;
                }
            case 3:
                {
                    if(day > 31)
                    {
                        day = day -31;
                        month =4;
                    }
                    break;
                }
            case 4:
                {
                    if(day >30)
                    {
                        day = day -30;
                        month =5;
                    }
                    break;
                }
            case 5:
                {
                    if(day >31)
                    {
                        day = day -31;
                        month =6;
                    }
                    break;
                }
            case 6:
                {
                    if(day>30)
                    {
                        day = day -30;
                        month =7;
                    }
                    break;
                }
            case 7:
                {
                    if(day>31)
                    {
                        day = day -31;
                        month =8;
                    }
                    break;
                }
            case 8:
                {
                    if(day>31)
                    {
                        day = day -31;
                        month =9;
                    }
                    break;
                }
            case 9:
                {
                    if(day>30)
                    {
                        day = day -30;
                        month =10;
                    }
                    break;
                }
            case 10:
                {
                    if(day>31)
                    {
                        day = day -31;
                        month =11;
                    }
                    break;
                }
            case 11:
                {
                    if(day>30)
                    {
                        day = day -30;
                        month =12;
                    }
                    break;
                }
            case 12:
                {
                    if(day >31)
                    {
                        month =1;
                        day = day -31;
                        year++;
                    }
                    break;
                }
        }
    }
    return QString::number(year)+"-"+QString::number(month)+"-"+QString::number(day)+" "+QString::number(hour)+":"+QString::number(minitus)+":"+QString::number(second);
}
//Get the Work time from the Ini file
void MainWindow::GetWorkTimeFromInI()
{
    //First, Get the Current Router' Mac Address
    //Second,Get the Current time
    QDateTime CurrentTime = QDateTime::currentDateTime();
    //Get the Mac,and then Load E-Mac and compare to the Mac.
    //Load the Mac from the Ini file
    QString RouterMac = Load_key("Mac");
    //Load the Time from the Ini file
    QString IniTime =Load_key("MacTime");
    QDateTime IniSetTime =QDateTime::fromString(IniTime,"yyyy-M-d h:m:s");
    //Load the Worktime's No from Ini file
    QString IniNo = Load_key("MacTimeNo");
    //Third,Compare to the time
    int soas= CurrentTime.secsTo(IniSetTime);
    if(soas <0)//Prior to the time of text
    {
        if(RouterMac==Mac)
        {
            QString status ="0";
            SetGuestNetworkEnable(status);
            comboBox->setCurrentIndex(0);
            QString title =languagemanager->getText(L_GUESTACCESS_BOX_5_TITLE,uuid);
            QString message = languagemanager->getText(L_GUESTACCESS_BOX_5_MESSAGE,uuid);
            box_5 = new MessageBox(message,title);
            box_5->show();
            QString timer =RouterMac;
            QString Data ="00";
            save_key(timer,Data);
        }
    }
    else//Late in text time
    {
        Closetimer->start(900000);
        if(RouterMac==Mac)
        {
            switch(IniNo.toInt())
            {
                case 1:
                    comboBox->setCurrentIndex(0);
                    break;
                case 2:
                    comboBox->setCurrentIndex(1);
                    break;
                case 3:
                    comboBox->setCurrentIndex(2);
                    break;
                case 4:
                    comboBox->setCurrentIndex(3);
                    break;
                case 5:
                    comboBox->setCurrentIndex(4);
                    break;
                default:
                    comboBox->setCurrentIndex(0);
                    break;
            }
        }
    }
    StopPi();
    pushButton_2->setDisabled(true);
}
//Get this Information of the current Router
void MainWindow::GetTheCurRouterMac()
{
    disconnect(soapapi,SIGNAL(getResponse(QtSoapMessage&,QUuid&,QString&)),0,0);
    QString ModeName ="WLANConfiguration";
    QString MethodName ="GetInfo";
    QMap<QString , QString> params;
    QString id ="getMac";
    connect(soapapi,SIGNAL(getResponse(QtSoapMessage&,QUuid&,QString&)),this,SLOT(GetCurMacResponse(QtSoapMessage&,QUuid&,QString&)));
    soapapi->submitrequest(ModeName,MethodName,params,uuid,id);
}
void MainWindow::GetCurMacResponse(QtSoapMessage &message, QUuid &uuid, QString &id)
{
    if(uuid =="{0a1e6c13-c838-47e0-9217-6ef881af5641}" & id =="getMac")
    {
        QString qureyfieldsstr;
        QStringList queryfieldlist;
        qureyfieldsstr = "NewEnable,NewSSIDBroadcast,NewStatus,NewSSID,NewRegion,NewChannel,NewWirelessMode,NewBasicEncryptionModes,NewWEPAuthType,NewWPAEncryptionModes,NewWLANMACAddress";
        queryfieldlist = qureyfieldsstr.split(",");
        if (message.isFault())
        {
            qDebug("Error: %s", message.faultString().value().toString().toLatin1().constData());
            QString title =languagemanager->getText(L_GUESTACCESS_BOX_4_TITLE,uuid);
            QString message =languagemanager->getText(L_GUESTACCESS_BOX_4_MESSAGE,uuid);
            box_4 = new MessageBox(message,title);
            box_4->show();
            StopPi();
        }
        else
        {
            const QtSoapType &res = message.returnValue();
            if( message.returnResponseCode().toString().toInt() == 0)
            {
                for (int i = 0; i < queryfieldlist.size(); ++i)
                {
                    if(queryfieldlist.at(i)=="NewWLANMACAddress")
                    {
                        CurMac = res[queryfieldlist.at(i)].value().toString();
                        qDebug()<<"Now the Current Router's Mac Address:"<<CurMac;
                        if(GenieMac == CurMac)//Judge the Current Router's Mac whether is same
                        {
                            QString timer =GenieMac;
                            QString Data ="00";
                            QString status ="0";
                            SetGuestNetworkEnable(status);
                            Worktimer->stop();
                            save_key(timer,Data);
                            QString title =languagemanager->getText(L_GUESTACCESS_BOX_6_TITLE,uuid);
                            QString message =languagemanager->getText(L_GUESTACCESS_BOX_6_MESSAGE,uuid);
                            box_6 = new MessageBox(message,title);
                            box_6->show();
                        }
                    }
                }
            }
        }
    }
}
//Work again ,when Genie has been closed,but start again
void MainWindow::AgainWork()
{
    //First,Judge the Current Router whether is NETGEAR
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
        if(curType =="y")//The Current Router is NetGear Router
        {
            //Second,Get this NETGEAR Router's Mac Address
            NETGEATMAC();
        }
        if(curType =="n")//The current Router isnot NetGear Router
        {
            QString title =languagemanager->getText(L_GUESTACCESS_BOX_3_TITLE,uuid);
            QString message =languagemanager->getText(L_GUESTACCESS_BOX_3_MESSAGE,uuid);
            box_3 = new MessageBox(message,title);
            box_3->show();
            StopPi();
        }
        if(curType =="?"){}
    }
}
void MainWindow::NETGEATMAC()
{
    disconnect(soapapi,SIGNAL(getResponse(QtSoapMessage&,QUuid&,QString&)),0,0);
    QString ModeName ="WLANConfiguration";
    QString MethodName ="GetInfo";
    QMap<QString , QString> params;
    QString id ="CurrentMac";
    connect(soapapi,SIGNAL(getResponse(QtSoapMessage&,QUuid&,QString&)),this,SLOT(GetNETGEARMACResponse(QtSoapMessage&,QUuid&,QString&)));
    soapapi->submitrequest(ModeName,MethodName,params,uuid,id);
}
void MainWindow::GetNETGEARMACResponse(QtSoapMessage &message, QUuid &uuid, QString &id)
{
    if(uuid =="{0a1e6c13-c838-47e0-9217-6ef881af5641}" && id =="CurrentMac")
    {
        QString qureyfieldsstr;
        QStringList queryfieldlist;
        qureyfieldsstr = "NewEnable,NewSSIDBroadcast,NewStatus,NewSSID,NewRegion,NewChannel,NewWirelessMode,NewBasicEncryptionModes,NewWEPAuthType,NewWPAEncryptionModes,NewWLANMACAddress";
        queryfieldlist = qureyfieldsstr.split(",");
        if (message.isFault())
        {
            qDebug("Error: %s", message.faultString().value().toString().toLatin1().constData());
            QString title =languagemanager->getText(L_GUESTACCESS_BOX_4_TITLE,uuid);
            QString message =languagemanager->getText(L_GUESTACCESS_BOX_4_MESSAGE,uuid);
            box_4 = new MessageBox(message,title);
            box_4->show();
            StopPi();
        }
        else
        {
            const QtSoapType &res = message.returnValue();
            if( message.returnResponseCode().toString().toInt() == 0)
            {
                for (int i = 0; i < queryfieldlist.size(); ++i)
                {
                    if(queryfieldlist.at(i)=="NewWLANMACAddress")
                    {
                        //Third,Compare to the Mac Address whether is same
                        QString NETGEARMAC = res[queryfieldlist.at(i)].value().toString();
                        QDateTime CurrentTime = QDateTime::currentDateTime();
                        QString RouterMac = Load_key("Mac");
                        QString IniTime =Load_key("MacTime");
                        QDateTime IniSetTime =QDateTime::fromString(IniTime,"yyyy-M-d h:m:s");
                        int soas= CurrentTime.secsTo(IniSetTime);
                        if(soas <0)
                        {
                            if(RouterMac==NETGEARMAC)
                            {
                                QString status ="0";
                                SetGuestNetworkEnable(status);
                                comboBox->setCurrentIndex(0);
                                Closetimer->stop();
                                QString timer =NETGEARMAC;
                                QString Data ="00";
                                save_key(timer,Data);
                                QString title =languagemanager->getText(L_GUESTACCESS_BOX_5_TITLE,uuid);
                                QString message = languagemanager->getText(L_GUESTACCESS_BOX_5_MESSAGE,uuid);
                                box_5 = new MessageBox(message,title);
                                box_5->show();
                            }
                        }
                    }
                }
            }
        }
    }
}
