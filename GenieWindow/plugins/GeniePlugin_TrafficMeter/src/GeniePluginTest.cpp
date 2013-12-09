#include "GeniePluginTest.h"
#include <QtPlugin>
#include <QLabel>
#include <QHBoxLayout>
#include "IManagerHandler.h"
#include "TestForm.h"
#include <QAction>
#include "ITrayManager.h"
#include "mainwindow.h"
#include "trafficsetting.h"
#include "trafficstatistics.h"
#include "inc/multilanguage_def/LanguageTrafficMeter.h"
#include "inc/genie_interface/date.h"
#include "inc/genie_interface/bbt_paras.h"
#include "inc/genie_interface/ILanguageManager.h"
#include "inc/genie_interface/IManagerHandler.h"
#include "version.h"

#define PLUGIN_INFO "UUID:{35b9be0c-b073-4cf8-bc12-d11923dc3d49};\
BIND:1;\
            HAVEWND:1;\
                VERSION:%1"

QUuid uuid ="{35b9be0c-b073-4cf8-bc12-d11923dc3d49}";
IPluginInf *GeniePluginTest::sSinglePtr=NULL;

bool GeniePluginTest::loginflag = false;
GeniePluginTest::GeniePluginTest()
{
        Q_ASSERT(GeniePluginTest::sSinglePtr ==0);
        sSinglePtr=this;
}

QString GeniePluginTest::text()
{
    ILanguageManager* lanmanager = MainWindow::sHandler->getLanguageManager();
    return QString(lanmanager->getText(L_TRAFFICMETER_BIGBT,uuid));
}

QWidget* GeniePluginTest::widget(QWidget *parent)
{
    w=new MainWindow(parent);
    return w;
}

QIcon * GeniePluginTest::icon()
{
    return NULL;
}

int GeniePluginTest::changelanguage(int idx)
{
	return 0;
}

QString  GeniePluginTest::info()
{
    return QString(PLUGIN_INFO).arg(VERSION_STRING);
}

int GeniePluginTest::priority()
{
    return 1975;
}

void GeniePluginTest::setHandler(IManagerHandler *hdl)
{
    MainWindow::sHandler = hdl;
    TrafficStatistics::sHandler = hdl;
    TrafficSetting::sHandler = hdl;
    SoapProxy::sHandler = hdl;
}

void GeniePluginTest::startup()
{

}

GeniePluginTest::~GeniePluginTest()
{
    qDebug("GeniePluginTest::~GeniePluginTest()");
}

void GeniePluginTest::notifyRaise()
{
    if(loginflag)
    {
        w->JudgeNetgear();
        loginflag = false;
        qDebug() << "notifyRaise";
    }
}

void GeniePluginTest::notifyLower()
{
    qDebug("GeniePluginTest::notifyLower()");
}

int GeniePluginTest::getStatus()
{
    //just for test
    static int tempstate=0;
    tempstate=(tempstate+1)%4;
    return tempstate;
   // return 0;
}

QStringList GeniePluginTest::bigButtonParas()
{
    QStringList ret;
    QString testuiPara="Traffic Meter:1975";
    testuiPara+=QString(":"BBT_PARA_LAN_IDX"=%1").arg(QString::number(L_TRAFFICMETER_BIGBT));
//    testuiPara+=QString(":"BBT_PARA_STATE_UPDATE_POLICY"=0");
    ret.append(testuiPara);
    return ret;
}

void GeniePluginTest::notifyBigButton(QString s)
{

}

void GeniePluginTest::startup(bool mini)
{

}

//int GeniePluginTest::getStatus(QString notifyPara)
//{
//    return 0;
//}

QString GeniePluginTest::getInfo(int para)
{
    if(PLUGIN_INF_LANGUAGE_FILE == para)
    {
        return "LanguageTrafficMeter.txt";
    }
    if(PLUGIN_INF_CREATE_DATE==para)
    {
        return VERSTION_STR_DATE;
    }
    return "";
}

//void GeniePluginTest::command(QString cmd, QString para)
//{
//
//}

void GeniePluginTest::command(int cmd, int para)
{
    loginflag = true;
    if(cmd == INFO_CMD_ROUTECONFIG_LOGOUT)
    {
        w->initdata();
        w->changefirstpage2();
    }
}


void GeniePluginTest::notifyTrayMessage(QSystemTrayIcon::ActivationReason reason,MAIN_FRAME_RESPONSE res)
{
    qDebug(":notifyTrayMessage(QSystemTrayIcon::ActivationReason %d)",reason);
}

void *GeniePluginTest::getPoint(int para)
{
	return NULL;
}

void GeniePluginTest::command(int cmd, QString para0, QString para1)
{
    if(cmd == INFO_CMD_ROUTECONFIG_WAITSTR)
    {
        w->setwaitstr(para0);
        qDebug() << "GeniePluginTest::command" <<para0;
    }
    else if(cmd == INFO_CMD_ROUTECONFIG_APPLYEND)
    {
        w->applyend();
    }
//    else if(cmd == "startrefresh")
//    {
//        w->setrefreshstr(para0);
//    }
//    else if(cmd == "endrefresh")
//    {
//        w->refreshend();
//    }
}

Q_EXPORT_PLUGIN2(GeniePluginTest, GeniePluginTest)
