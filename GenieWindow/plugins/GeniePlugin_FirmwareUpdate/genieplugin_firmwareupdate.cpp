#include "genieplugin_firmwareupdate.h"
#include <QtGui>

#include <QtPlugin>

#include "IManagerHandler.h"
#include <QAction>
#include "ITrayManager.h"
//#include "IUiManager.h"
#include "bbt_paras.h"
#include "inc/multilanguage_def/LanguageFirmwareUpdate.h"
#include "IClickManager.h"
#include "inc/genie_interface/date.h"
#include "firmwareupdate.h"
//#include "wizardpage.h"
#include "inc/genie_interface/IManagerHandler.h"
#include "inc/genie_interface/ILanguageManager.h"
#include "IInfoManager.h"
#include "IUiManager.h"

#include "info_defs.h"
#include "version.h"

//#define COMMAND_NOTIFYNETSTATE "ntste"
//#define COMMAND_NOTIFYROUTETYPE "ntfrt"

#define PLUGIN_UUID "{c02fb5db-6c62-42e7-bf4a-f704247ed7b8}"

#define PLUGIN_INFO "UUID:{c02fb5db-6c62-42e7-bf4a-f704247ed7b8};\
BIND:1;\
            HAVEWND:1;\
                VERSION:%1"

QUuid uuid ="{c02fb5db-6c62-42e7-bf4a-f704247ed7b8}";
IPluginInf *GeniePlugin_FirmwareUpdate::sSinglePtr = NULL;
ILogManager *GeniePlugin_FirmwareUpdate::mLogHandler = NULL;

GeniePlugin_FirmwareUpdate::GeniePlugin_FirmwareUpdate()
{
    Q_ASSERT(GeniePlugin_FirmwareUpdate::sSinglePtr == 0);
    sSinglePtr = this;

    setFlag = false;
    //oldNetState = "";
    //newNetState = "";
}

GeniePlugin_FirmwareUpdate::~GeniePlugin_FirmwareUpdate()
{

}

QString GeniePlugin_FirmwareUpdate::text()
{
    ILanguageManager* lanmanager = mHandler->getLanguageManager();
    return QString(lanmanager->getText(L_UPGRADE_MAIN_NAME,uuid));
    //return QString("Firmware Update:")+QString::number(L_UPGRADE_MAIN_NAME);
}

QWidget* GeniePlugin_FirmwareUpdate::widget(QWidget *parent)
{
    qDebug() << "Run Firmware Update Plugin!";

    IUiManager *mUimanager = mHandler->getUiManager();
    Q_ASSERT(mUimanager);
    mUimanager->setNetStateFlag("c02fb5db-6c62-42e7-bf4a-f704247ed7b8", true);
    mUimanager->setRouteTypeFlag("c02fb5db-6c62-42e7-bf4a-f704247ed7b8", true);


    wUpdate = new FirmwareUpdate(parent);
    //FirmwareUpdate *wUpdate = new FirmwareUpdate(parent);
    return wUpdate;
}

QIcon* GeniePlugin_FirmwareUpdate::icon()
{
    return NULL;
}

QString  GeniePlugin_FirmwareUpdate::info()
{
    //return QString(PLUGIN_INFO);
    return QString(PLUGIN_INFO).arg(VERSION_STRING);

}

int GeniePlugin_FirmwareUpdate::priority()
{
    return 1950;
}

int GeniePlugin_FirmwareUpdate::changelanguage(int idx)
{
    return 0;
}

void GeniePlugin_FirmwareUpdate::setHandler(IManagerHandler *hdl)
{
    mHandler = hdl;
    mLogHandler = hdl->getLogManager();
    mInfoHandler = hdl->getInfoManager();
    Q_ASSERT(mInfoHandler);

    WizardPage::sHandler = hdl;
    FirmwareUpdate::sHandler = hdl;
    IntroWizard::sHandler = hdl;
    CheckWizard::sHandler = hdl;
    DownloadWizard::sHandler = hdl;
    //UpdateWizard::sHandler = hdl;
    CompleteWizard::sHandler = hdl;
    FinishWizard::sHandler = hdl;
}

//void GeniePlugin_FirmwareUpdate::startup()
//{
//
//}

void GeniePlugin_FirmwareUpdate::startup(bool mini)
{

}

void GeniePlugin_FirmwareUpdate::notifyRaise()
{
    //IInfoManager *setImages = mHandler->getInfoManager();
    //Q_ASSERT(setImages);
    //QString currentModel;
    //currentModel = setImages->getInfo(INFO_CMD_ROUTE_TYPE);
    //qDebug() << "Wizard Page get Model: " << currentModel;

    QString routerType;
    //routerType = mInfoHandler->getInfo(INFO_CMD_ROUTE_TYPE);
    QString strModel;
    strModel = mInfoHandler->getInfo(INFO_CMD_ROUTE_TYPE_IMMEDIATE);
    QStringList strTempArray;
    if (strModel.isEmpty())
        routerType.clear();
    else{
        //strTempArray = strModel.split(" ");
        //routerType = strTempArray[2];
        routerType = strModel;
    }


    //qDebug() << "Router Type: " << routerType;
    //if (routerType.isEmpty())
    wUpdate->setFirstPageTitle(routerType);


//    IInfoManager *setImages = mHandler->getInfoManager();
//    Q_ASSERT(setImages);
//    QString mainPath;
//    mainPath = setImages->getInfo(INFO_CMD_ROUTE_NAME_B3);
//    qDebug() << "Main Path: " << mainPath;
//
//    //WizardPage::changePic(currentModel);
//    wUpdate->changePagePic(mainPath);

}

void GeniePlugin_FirmwareUpdate::notifyLower()
{

}

QStringList GeniePlugin_FirmwareUpdate::bigButtonParas()
{
    QStringList reButton;
    QString inputPara = "Firmware Update:1950";
    inputPara += QString(":"BBT_PARA_LAN_IDX"=%1").arg(QString::number(L_UPGRADE_MAIN_NAME));
    //inputPara += QString(":"BBT_PARA_STATE_UPDATE_POLICY"=0");

    reButton.append(inputPara);
    return reButton;
}

void GeniePlugin_FirmwareUpdate::notifyBigButton(QString s)
{
//    QString mOper = "g";
//    IClickManager *clickManager = mHandler->getClickManager();
//    clickManager->click(mOper);

}

//int GeniePlugin_FirmwareUpdate::getStatus()
//{
//    static int tempstate=0;
//    tempstate=(tempstate+1)%4;
//    return tempstate;
//}

//int GeniePlugin_FirmwareUpdate::getStatus(QString notifyPara)
//{
//    //return 0;
//    return 5;
//}

void GeniePlugin_FirmwareUpdate::notifyTrayMessage(QSystemTrayIcon::ActivationReason reason,MAIN_FRAME_RESPONSE res)
{

}

QString GeniePlugin_FirmwareUpdate::getInfo(int para)
{

    if(PLUGIN_INF_DEPENDENCE == para)
    {
        return "";
    }
    else if(PLUGIN_INF_LANGUAGE_FILE == para)
    {
        return "LanguageFirmwareUpdate.txt";
    }
    if(PLUGIN_INF_CREATE_DATE==para)
    {
        return VERSTION_STR_DATE;
    }
    else
    {
        return "";
    }
    
    return "";

}

void GeniePlugin_FirmwareUpdate::command(int cmd, QString para0, QString para1)
{
    qDebug() << "router firmware change:" << cmd;
    if( cmd == COMMAND_NOTIFYROUTETYPE ){
        //IInfoManager *setImages = mHandler->getInfoManager();
        //Q_ASSERT(setImages);
        //QString mainPath;
        //QString mainPath2;
        //mainPath = mInfoHandler->getInfo(INFO_CMD_ROUTE_NAME_B3, 1);
        //mainPath2 = mInfoHandler->getInfo(INFO_CMD_ROUTE_NAME_B3, 2);
        //Debug() << "Main Path: " << mainPath;
        //wUpdate->changePagePic(mainPath);
        //wUpdate->changePagePic(mainPath, mainPath2);
        wUpdate->changePagePic();


        QString routerModel;
        //QString routerFirmware;
        //routerModel = mInfoHandler->getInfo(INFO_CMD_ROUTE_TYPE);
        QString strModel;
        strModel = mInfoHandler->getInfo(INFO_CMD_ROUTE_TYPE_IMMEDIATE);
        QStringList strTempArray;
        if (strModel.isEmpty())
            routerModel.clear();
        else{
            //strTempArray = strModel.split(" ");
            //routerModel = strTempArray[2];
            routerModel = strModel;
        }
        qDebug() << "Router Type: " << routerModel;
        wUpdate->setFirstPageTitle(routerModel);
        //wUpdate->setFirstPageTitle(routerModel, routerFirmware);

    }else if ( cmd == COMMAND_NOTIFY_FUPDATE ){
        qDebug() << "Turn upgrade interface.";

        //////Get information from Genieplugin_Info
        //////firmware info and download url
        QString routerModel;
        //routerModel = mInfoHandler->getInfo(INFO_CMD_ROUTE_TYPE);
        QString strModel;
        strModel = mInfoHandler->getInfo(INFO_CMD_ROUTE_TYPE_IMMEDIATE);
        QStringList strTempArray;
        if (strModel.isEmpty())
            routerModel.clear();
        else{
            //strTempArray = strModel.split(" ");
            //routerModel = strTempArray[2];
            routerModel = strModel;
        }

        QString strTwoVersion;
        strTwoVersion = mInfoHandler->getInfo(INFO_CMD_FUPDATE_PARA1);

        QString strUrl;
        strUrl = mInfoHandler->getInfo(INFO_CMD_FUPDATE_PARA2);

        QString strTwoCompare;
        strTwoCompare = mInfoHandler->getInfo(INFO_CMD_FUPDATE_PARA3);

        QString updatelog;
        updatelog = "Update Info: " + routerModel + strTwoVersion + strUrl + strTwoCompare;
        writeGenieLog(updatelog);

        wUpdate->turnUpgradePage(routerModel, strTwoVersion, strUrl, strTwoCompare);

    }

}

//void GeniePlugin_FirmwareUpdate::command(QString cmd, QString para)
//{
//
//}

void GeniePlugin_FirmwareUpdate::command(int cmd, int para)
{
    if( cmd == COMMAND_NOTIFYNETSTATE ){
        char * cText[]={"net ok","net error","over flow"};
        if( para >= 0 && para < sizeof(cText)/sizeof(cText[0]) ){
            //qDebug() << "Network Changed: " << cText[para];

//            if ( cText[para] == "net ok" ){
//                qDebug() << "reuturn First page.";
//                if ( setFlag ){
//                    //wUpdate->turnFirstPage();
//                }
//                else
//                    setFlag = true;
//            }

            if ( setFlag ){
                if ( cText[para] == "net ok" || cText[para] == "net error" ){
                    newNetState = cText[para];
                    if ( newNetState != oldNetState ){
                        if ( newNetState == "net ok" && oldNetState == "net error"){
                            oldNetState = newNetState;
                            wUpdate->turnFirstPage();
                        }
                        if ( newNetState == "net error" && oldNetState == "net ok"){
                            oldNetState = newNetState;
                            wUpdate->turnFirstPage();
                        }
                        if ( oldNetState.isEmpty() )
                            oldNetState = newNetState;
                    }
                }
            }else {
                setFlag = true;
            }


        }
    //}else if ( cmd == "logout" ){
    }else {
        writeGenieLog("Router setting is logout.");
        wUpdate->PageToHome();
    }

}

void *GeniePlugin_FirmwareUpdate::getPoint(int para)
{
    return NULL;
}

void GeniePlugin_FirmwareUpdate::writeGenieLog(QString log)
{
    if (log.isEmpty())
        mLogHandler->writeLog("Nothing", PLUGIN_UUID);
    else
        mLogHandler->writeLog(log, PLUGIN_UUID);
}


Q_EXPORT_PLUGIN2(GeniePlugin_FirmwareUpdate, GeniePlugin_FirmwareUpdate)
