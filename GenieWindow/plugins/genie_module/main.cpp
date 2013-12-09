#include <QtGui/QApplication>
#include "QGenieFrameMain.h"
#include "QGenieStyleManager.h"
#include "QGeniePageHome.h"
#include "QGenieFrameLeftPanel.h"
#include "QGenieStackManager.h"
#include "QGenieBigButtonManager.h"
#include "QGenieTrayManager.h"
#include "QGenieInterfaceClickManager.h"
#include "QGenieInterfaceInfoManager.h"
#include "QGenieInterfaceManagerHandler.h"
#include "QGenieInterfaceToyManager.h"
#include "QGenieInterfaceTrayManager.h"
#include "QGenieSingleProcessHandler.h"
#include "QGeniePluginProxyManager.h"
#include "QGenieInterfaceUiManager.h"
#include "QGenieInterfaceSoapManager.h"
#include "QGenieLanguageManager.h"
#include "QGenieInterfaceLanguageManager.h"
#include "QGenieAllSingletonClasses.h"

#include "QGenieFrameTitle.h"
#include <QTranslator>
#include <QDesktopWidget>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setQuitOnLastWindowClosed(false);
    QGenieAllSingletonClasses allSinglerClass;



    QGenieSingleProcessHandler *processhandler=QGenieSingleProcessHandler::GetSingletonPtr();
    ASSERT(processhandler);
    if(!processhandler->tryCreate())
    {
        return -1;
    }

    QGenieTrayManager *traymanager=QGenieTrayManager::GetSingletonPtr();
    ASSERT(traymanager);

    QGenieFrameMain w;
    w.setObjectName("MainWindow");
    traymanager->startService(&w,":/ico_tray_good.png");

    QGenieStackManager *stackmanager=QGenieStackManager::GetSingletonPtr();
    ASSERT(stackmanager);
  //  stackmanager->createBasicPage();

    QGenieBigButtonManager *bigbuttonmanager=QGenieBigButtonManager::GetSingletonPtr();
    ASSERT(bigbuttonmanager);
    bigbuttonmanager->realRegistBigButton();


    //stackmanager->detectWidgets();
    QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
    ASSERT(proxymanager);
    proxymanager->detectWidgets();

//    QGenieFrameTitle *title=QGenieFrameTitle::GetSingletonPtr();
//    ASSERT(title);
//    title->setFirmwareAndRoute("");
    w.show();

    QDesktopWidget* desktop = QApplication::desktop();
    w.move((desktop->width() - w.width())/2,
         (desktop->height() - w.height())/2);
    //qDebug() << desktop->width() << desktop->height() << this->width() << this->height();

    QGenieStyleManager *stylemanager=QGenieStyleManager::GetSingletonPtr();
    ASSERT(stylemanager);
    stylemanager->setStyle(EST_DEFAULT);

   proxymanager->startupAllNoWndPi();

    return a.exec();
}
