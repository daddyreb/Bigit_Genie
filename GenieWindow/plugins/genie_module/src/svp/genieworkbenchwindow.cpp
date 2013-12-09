#include "genieworkbenchwindow.h"
#include "IMenuExtension.h"
#include "IToolBarExtension.h"
#include "IMenuExtensionPoint.h"
#include "IToolbarExtensionPoint.h"
#include "WorkbenchPart.h"
#include "tabpaget1base.h"
#include "tabpaget2base.h"


#include <QApplication>
#include <QVariant>
#include <QDir>
#include <QtGui/QLabel>
#include <QtGui/QStatusBar>
#include <QtGui/QPushButton>
#include <QtGui/QLineEdit>
#include <QtGui/QTreeWidget>
#include <QtGui/QSpacerItem>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QComboBox>
#include <QtGui/QHeaderView>
#include <QtGui/QTabWidget>
#include <QtGui/QMessageBox>
#include <QtCore/QObject>
#include <QSizePolicy>
#include <QTranslator>
#include <QList>
#include <QtGui/QSplashScreen>
#include <QtGui/QPixmap>
#include "defines.h"

extern QTranslator appTranslator;
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
#include "QGenieAllSingletonClasses.h"




GenieWorkbenchWindow::GenieWorkbenchWindow()
    :mAllSingletonClasses(NULL)
{
    main_wnd    =   0;

}


GenieWorkbenchWindow::~GenieWorkbenchWindow(void)
{
    SAFE_DELETE(main_wnd);
    SAFE_DELETE(mAllSingletonClasses);
}


bool GenieWorkbenchWindow::create ()
{
    mAllSingletonClasses=new QGenieAllSingletonClasses();
	
	
    main_wnd=new QGenieFrameMain();
    QGenieTrayManager *traymanager=QGenieTrayManager::GetSingletonPtr();
    ASSERT(traymanager);

    main_wnd->setObjectName("MainWindow");
    traymanager->startService(main_wnd,":/ico_tray.ico");

    QGenieBigButtonManager *bigbuttonmanager=QGenieBigButtonManager::GetSingletonPtr();
    ASSERT(bigbuttonmanager);
    bigbuttonmanager->realRegistBigButton();

    QGenieStackManager *stackmanager=QGenieStackManager::GetSingletonPtr();
    ASSERT(stackmanager);
    stackmanager->detectWidgets();

    return true;
}

void GenieWorkbenchWindow::show ()
{
    if(main_wnd)
    {
        main_wnd->show();
        QGenieStyleManager *stylemanager=QGenieStyleManager::GetSingletonPtr();
        ASSERT(stylemanager);
        stylemanager->setStyle(EST_DEFAULT);
    }
}

bool GenieWorkbenchWindow::close()
{
        return true;
}
