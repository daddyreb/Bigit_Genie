#ifndef GENIEGWORKBENCHFACTORY_H
#define GENIEGWORKBENCHFACTORY_H

#include "workbenchfactory.h"
#include "ISvpShell.h"

class GenieWorkbenchWindow;
class WorkbenchWindow;
class QMainWindow;

class GenieWorkbenchFactory :
        public WorkbenchFactory
{
public:
        GenieWorkbenchFactory();
        GenieWorkbenchWindow* createWorkbenchWindow(QMainWindow *main_wnd,ISvpShell *shell);
private:
        GenieWorkbenchWindow		*m_WorkbenchWnd;

};

#endif // GENIEGWORKBENCHFACTORY_H
