#include "genieworkbenchrun.h"
#include "GenieWorkbenchWindow.h"
#include "GenieWorkbenchFactory.h"


GenieWorkbenchRun::GenieWorkbenchRun(void)
{
        genie_workbench_window	=0;
        svp_shell		=0;
}

GenieWorkbenchRun::~GenieWorkbenchRun(void)
{
        this->stop();
}

void GenieWorkbenchRun::setSvpShell(ISvpShell* shell)
{
        svp_shell=shell;
}

int GenieWorkbenchRun::run(QMainWindow *main_window)
{
        GenieWorkbenchFactory factory;
        genie_workbench_window = factory.createWorkbenchWindow (main_window,svp_shell);
        return 0;
}

void GenieWorkbenchRun::stop()
{
        if (genie_workbench_window)
        {
                delete genie_workbench_window;
                genie_workbench_window=0;
        }
}
