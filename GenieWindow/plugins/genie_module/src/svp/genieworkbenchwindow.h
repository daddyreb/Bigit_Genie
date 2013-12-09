#ifndef GENIEWORKBENCHWINDOW_H
#define GENIEWORKBENCHWINDOW_H

#include "workbenchwindow.h"
#include "Workbench.h"
#include "IWorkbenchWindow.h"
#include "ObjectManager.h"
#include "RegisterObject.h"
#include "IExtension.h"
#include "IExtensionPoint.h"
#include "ISvpShell.h"
#include "QGenieFrameMain.h"

class QGenieStyleManager;
class QGenieStackManager;
class QGenieBigButtonManager;
class QGenieInterfaceClickManager;

class QGenieInterfaceInfoManager ;
class QGenieInterfaceToyManager  ;
class QGenieInterfaceTrayManager ;
class QGenieInterfaceManagerHandler;
class QGenieTrayManager ;
class QGenieAllSingletonClasses;


class GenieWorkbenchWindow :
        public WorkbenchWindow
{

        //Q_OBJECT
public:
        GenieWorkbenchWindow();
        ~GenieWorkbenchWindow(void);

        bool create ();

        void show ();

        bool close();
protected:
    QGenieFrameMain      *main_wnd;
protected:
    QGenieAllSingletonClasses *mAllSingletonClasses;
};
#endif // GENIEWORKBENCHWINDOW_H
