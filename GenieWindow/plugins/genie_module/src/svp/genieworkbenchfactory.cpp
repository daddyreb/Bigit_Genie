#include "genieworkbenchfactory.h"
#include "GenieWorkbenchWindow.h"


GenieWorkbenchFactory::GenieWorkbenchFactory(void)
{
        m_WorkbenchWnd=0;
}

GenieWorkbenchWindow* GenieWorkbenchFactory::createWorkbenchWindow(QMainWindow *main_wnd,ISvpShell *shell)
{
        if (m_WorkbenchWnd)
                delete m_WorkbenchWnd;

        m_WorkbenchWnd = new GenieWorkbenchWindow ();
        m_WorkbenchWnd->create ();
        m_WorkbenchWnd->show ();

        return m_WorkbenchWnd;
}
