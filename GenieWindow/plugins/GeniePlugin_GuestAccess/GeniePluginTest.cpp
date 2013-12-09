#include "GeniePluginTest.h"
#include <QtPlugin>
#include <QLabel>
#include <QHBoxLayout>
#include "IManagerHandler.h"
#include "TestForm.h"
#include <QAction>
#include "ITrayManager.h"
#include "mainwindow.h"
#include "inc/multilanguage_def/LanguageGuestAccess.h"
#include "inc/genie_interface/bbt_paras.h"
#include "inc/genie_interface/date.h"
#include "inc/genie_interface/ILanguageManager.h"
#include "inc/genie_interface/IManagerHandler.h"
#include "version.h"

#define PLUGIN_INFO "UUID:{0a1e6c13-c838-47e0-9217-6ef881af5641};\
BIND:1;\
            HAVEWND:1;\
                VERSION:%1"

QUuid uuid ="{0a1e6c13-c838-47e0-9217-6ef881af5641}";
IPluginInf *GeniePlugin_GuestAccess::sSinglePtr=NULL;

GeniePlugin_GuestAccess::GeniePlugin_GuestAccess()
{
    Q_ASSERT(GeniePlugin_GuestAccess::sSinglePtr ==0);
    sSinglePtr=this;
}

QString GeniePlugin_GuestAccess::text()
{
    ILanguageManager* lanmanager = MainWindow::sHandler->getLanguageManager();
    return QString(lanmanager->getText(L_GUESTACCESS_BIGBT,uuid));
}

QWidget* GeniePlugin_GuestAccess::widget(QWidget *parent)
{
    w = new MainWindow(parent);
    return w;
}

QIcon * GeniePlugin_GuestAccess::icon()
{
    return NULL;
}

int GeniePlugin_GuestAccess::changelanguage(int idx)
{
    return 0;
}

QString  GeniePlugin_GuestAccess::info()
{
    return QString(PLUGIN_INFO).arg(VERSION_STRING);
}

int GeniePlugin_GuestAccess::priority()
{
    return 1150;
}

void GeniePlugin_GuestAccess::setHandler(IManagerHandler *hdl)
{
    MainWindow::sHandler=hdl;
}

void GeniePlugin_GuestAccess::startup()
{

}

GeniePlugin_GuestAccess::~GeniePlugin_GuestAccess()
{
    qDebug("GeniePlugin_GuestAccess::~GeniePlugin_GuestAccess()");
}

void GeniePlugin_GuestAccess::notifyRaise()
{
    w->JudgeRouter();
}

void GeniePlugin_GuestAccess::notifyLower()
{
    qDebug("GeniePlugin_GuestAccess::notifyLower()");
}

int GeniePlugin_GuestAccess::getStatus()
{
    //just for test
    static int tempstate=0;
    tempstate=(tempstate+1)%4;
    return tempstate;
    return 0;
}

QStringList GeniePlugin_GuestAccess::bigButtonParas()
{
    QStringList ret;
    QString testuiPara="Guest Access:1150";
    testuiPara+=QString(":"BBT_PARA_LAN_IDX"=%1").arg(QString::number(L_GUESTACCESS_BIGBT));
    testuiPara+=QString(":"BBT_PARA_STATE_UPDATE_POLICY"=0");
    ret.append(testuiPara);
    return ret;
}

void GeniePlugin_GuestAccess::notifyBigButton(QString s)
{

}

void GeniePlugin_GuestAccess::startup(bool mini)
{

}

int GeniePlugin_GuestAccess::getStatus(QString notifyPara)
{
    return 0;
}

QString GeniePlugin_GuestAccess::getInfo(QString para)
{
    if(PLUGIN_INF_LANGUAGE_FILE == para)
    {
        return "LanguageGuestAccess.txt";
    }
    if(PLUGIN_INF_CREATE_DATE==para)
    {
        return VERSTION_STR_DATE;
    }

    return "";
}

void GeniePlugin_GuestAccess::command(QString cmd, QString para)
{

}

void GeniePlugin_GuestAccess::command(QString cmd, int para)
{
    if((cmd ==QString("LoginOK")) && para ==12345678)
    {
//        w->GetTheCurRouterMac();
    }
}

void GeniePlugin_GuestAccess::notifyTrayMessage(QSystemTrayIcon::ActivationReason reason,MAIN_FRAME_RESPONSE res)
{
    qDebug(":notifyTrayMessage(QSystemTrayIcon::ActivationReason %d)",reason);
}

void *GeniePlugin_GuestAccess::getPoint(QString para)
{

}

void GeniePlugin_GuestAccess::command(QString cmd, QString para0, QString para1)
{

}

Q_EXPORT_PLUGIN2(GeniePlugin_GuestAccess, GeniePlugin_GuestAccess)
