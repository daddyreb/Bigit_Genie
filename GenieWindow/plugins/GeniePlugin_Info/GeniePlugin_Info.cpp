#include "GeniePlugin_Info.h"
#include <QtPlugin>
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMutex>
#include "IManagerHandler.h"
#include <QAction>
#include <QDebug>
#include "ITrayManager.h"
#include "bbt_paras.h"
//#include "LanguageInfo.h"
//#include "LanguageTestUi.h"
#include "inc/genie_interface/date.h"
#include "setting_def.h"

//#include "IUiManager.h"
#include "info_defs.h"
#include "version.h"

#include "getinformation.h"
//#include "netgearsoap.h"



//#define COMMAND_NOTIFYNETSTATE "ntste"
#define PLUGIN_UUID "{f9b01961-e3b2-474c-8521-42371d2cab61}"

#define PLUGIN_INFO "UUID:{f9b01961-e3b2-474c-8521-42371d2cab61};\
BIND:1;\
            HAVEWND:0;\
                VERSION:%1"

ILanguageManager *GeniePlugin_Info::mlanguagemanager = NULL;
ILogManager *GeniePlugin_Info::mLogHandler = NULL;


GeniePlugin_Info::GeniePlugin_Info()
{
    ////
    mGenieInfo = new GetInformation();
    Q_ASSERT(mGenieInfo);
    //connect(mGenieInfo, SIGNAL(GetFirmwareFinish(bool)), this, SLOT(GetFirmwareOver(bool)));

    startFalg = false;

}

GeniePlugin_Info::~GeniePlugin_Info()
{
    qDebug("GeniePlugin_Info::~GeniePlugin_Info()");

    delete mGenieInfo;
}

QString GeniePlugin_Info::text()
{
    return QString("TestUi:");
    //return QString("TestUi:")+QString::number(L_TESTUI_LEFTBT);
}

QWidget* GeniePlugin_Info::widget(QWidget *parent)
{
    return NULL;
}


QStringList GeniePlugin_Info::bigButtonParas()
{
    QStringList ret;
    QString testuiPara="TestUi:300";
    //testuiPara+=":"BBT_PARA_WAITING_TEXT_LEFT"=testdd:"BBT_PARA_FREQUENCY"=10000";
    testuiPara+=QString(":"BBT_PARA_LAN_IDX"=%1").arg("");
    //testuiPara+=QString(":"BBT_PARA_LAN_IDX"=%1").arg(QString::number(L_TESTUI_BIGBT));
    //testuiPara+=QString(":"BBT_PARA_STATE_UPDATE_POLICY"=2");
    ret.append(testuiPara);

    testuiPara="TestUiother:200";
    //testuiPara+=":"BBT_PARA_WAITING_TEXT_LEFT"=testdd:"BBT_PARA_FREQUENCY"=10000";
    testuiPara+=QString(":"BBT_PARA_LAN_IDX"=%1").arg("");
    //testuiPara+=QString(":"BBT_PARA_LAN_IDX"=%1").arg(QString::number(L_TESTUI_BIGBT));
    //testuiPara+=QString(":"BBT_PARA_STATE_UPDATE_POLICY"=2");
    ret.append(testuiPara);


   // ret.append("justtest:400");
    return ret;
}

void GeniePlugin_Info::notifyBigButton(QString s)
{
    qDebug()<< "GeniePlugin_Info::notifyBigButton" << s;
}

QIcon * GeniePlugin_Info::icon()
{
    return NULL;
}

int GeniePlugin_Info::changelanguage(int idx)
{
    return 0;
}

QString  GeniePlugin_Info::info()
{
    //return QString(PLUGIN_INFO);
    return QString(PLUGIN_INFO).arg(VERSION_STRING);
}

int GeniePlugin_Info::priority()
{
    return 2200;
}


void GeniePlugin_Info::setHandler(IManagerHandler *hdl)
{

    //todo set handler
    GetInformation::sHandler = hdl;


    mlanguagemanager = hdl->getLanguageManager();
    Q_ASSERT(mlanguagemanager);

    mLogHandler = hdl->getLogManager();
    Q_ASSERT(mLogHandler);

    mUimanager = hdl->getUiManager();
    Q_ASSERT(mUimanager);

}

void GeniePlugin_Info::startup(bool mini)
{
    mUimanager->setRouteTypeFlag("f9b01961-e3b2-474c-8521-42371d2cab61", true);
    mUimanager->setGatewayFlag("f9b01961-e3b2-474c-8521-42371d2cab61", true);

    //////Test - get version
    //QString tempType;
    //tempType = mGenieInfo->isProduct("TEST", false);

    //mGenieInfo->isFirmwareUpgrade();

    //////Timing Upgrade Firmware
    //mGenieInfo->setTimingUpgrade();

}

void GeniePlugin_Info::notifyRaise()
{
    qDebug("GeniePlugin_Info::notifyRaise()");

}

void GeniePlugin_Info::notifyLower()
{
    qDebug("GeniePlugin_Info::notifyLower()");

}

//int GeniePlugin_Info::getStatus(QString notifyPara)
//{
//    return 0;
//}


void GeniePlugin_Info::notifyTrayMessage(QSystemTrayIcon::ActivationReason reason,MAIN_FRAME_RESPONSE res)
{
    qDebug(":notifyTrayMessage(QSystemTrayIcon::ActivationReason %d)",reason);
}

QString GeniePlugin_Info::getInfo(int para)
{
    QString strReturn;
    QString wlog;
    wlog = "Call getInfo() function, and return values = ";
    if(para == PLUGIN_INF_DEPENDENCE ){
        strReturn = "";
    }else if (para == INFO_CMD_NETGEAR){
        strReturn = "?";
        //strReturn = mGenieInfo->isProduct(para, false);
        strReturn = mGenieInfo->isProduct("IS_NETGEAR", false);
    }else if (para == INFO_CMD_NETGEAR_REFRESH ){
        QString strTemp;
        strTemp = mGenieInfo->isProduct("RT_SET", false);
        strReturn = "?";
    }else if (para == INFO_CMD_ROUTEMAC){
        strReturn = mGenieInfo->getNetgearMAC();
        if (strReturn=="NULL")
            strReturn = "";
    }else if (para == INFO_CMD_FIRMWARE){
        strReturn = mGenieInfo->getFirmwareVersion();
    }else if (para == INFO_CMD_ROUTE_TYPE){
        strReturn = mGenieInfo->getProductType();
    }else if (para == INFO_CMD_RATROSHARELEVEL){
        strReturn = mGenieInfo->getSupportLevel();

    }else if (PLUGIN_INF_LANGUAGE_FILE == para){
        //strReturn = "LanguageInfo.txt";
        strReturn = "";
    }else if (PLUGIN_INF_CREATE_DATE==para){
        strReturn = VERSTION_STR_DATE;
    }else if (para==INFO_CMD_FUPDATE_PARA1){
        //////return two firmware version
        strReturn = mGenieInfo->getCompareVersion();
    }else if (para==INFO_CMD_FUPDATE_PARA2){
        //////return firmware download URL
        strReturn = mGenieInfo->getDownloadUrl();
    }else if (para==INFO_CMD_FUPDATE_PARA3){
        //////return compare data
        strReturn = mGenieInfo->getCompareData();
    }else if (para==INFO_CMD_NETGEAR_CURRENTSET){
        //////return Netgear's currentsetting information
        strReturn = mGenieInfo->getNetgearCurrentset();
    }else {
        strReturn = "";
    }

    wlog += strReturn;
   // writeLog(wlog);

    return strReturn;

}


void GeniePlugin_Info::command(int cmd, QString para0, QString para1)
{
    qDebug() << "Get current router Model and Version when these was change." << cmd;
//    if( cmd == COMMAND_NOTIFYROUTETYPE ){
//        //writeLog("Get current router Model and Version when these was change.");
//        writeLog("The NETGEAR router model is changed.");
//        mGenieInfo->getCurrentVersion();
//
//        //////check Firmware Update when model is change
//        //mUimanager->command(UI_CMD_HIDE_UPDATE_BUTTON, PLUGIN_UUID);
//        //mGenieInfo->isFirmwareUpgrade();
//    }

    if ( cmd == COMMAND_NOTIFYGATEWAY ){
        writeLog("The gateway is changed, get router information again.");

        QString tempType;
        tempType = mGenieInfo->isProduct("GATEWAY", false);

    }

}

void GeniePlugin_Info::command(int cmd, int para)
{

//    if( cmd == COMMAND_NOTIFYNETSTATE ){
//        char * cText[]={"net ok","net error","over flow"};
//        if( para >= 0 && para < sizeof(cText)/sizeof(cText[0]) ){
//            QString mlog = "Network change: " + oldNetState + " --> " + cText[para];
//            qDebug() << mlog;
//            writeLog(mlog);
//
//            if ( startFalg ){
//                if ( cText[para] == "net ok" || cText[para] == "net error" ){
//                    newNetState = cText[para];\
//                    if ( newNetState != oldNetState ){
//                        if ( newNetState == "net ok" && oldNetState == "net error"){
//                            oldNetState = newNetState;
//
//                            //////2010-11-03 Jansion
//                            //StopGetInfo();
//                        }
//                        if ( newNetState == "net error" && oldNetState == "net ok"){
//                            //qDebug() << "Start network state: " << newNetState;
//                            oldNetState = newNetState;
//                        }
//                        if ( oldNetState.isEmpty() )
//                            oldNetState = newNetState;
//                    }
//                }
//            }else {
//                startFalg = true;
//            }
//
//        }
//    }

}

void *GeniePlugin_Info::getPoint(int para)
{
    return NULL;
}



QString GeniePlugin_Info::translateText(int idx)
{
    if(mlanguagemanager)
    {
        return mlanguagemanager->getText(idx, QUuid("f9b01961-e3b2-474c-8521-42371d2cab61"));
    }
    else
    {
        return "";
    }
}

void GeniePlugin_Info::writeLog(QString log)
{
    if (log.isEmpty())
        mLogHandler->writeLog("Nothing", PLUGIN_UUID);
    else
        mLogHandler->writeLog(log, PLUGIN_UUID);
}

void GeniePlugin_Info::StopGetInfo()
{
    mGenieInfo->AbortGetHttp();
}

Q_EXPORT_PLUGIN2(GeniePlugin_Info, GeniePlugin_Info)
