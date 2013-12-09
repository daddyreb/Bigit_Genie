#ifndef GENIEWORKBENCHRUN_H
#define GENIEWORKBENCHRUN_H

#include "workbenchrun.h"
#include "ISvpShell.h"

class QMainWindow;
class GenieWorkbenchWindow;

class GenieWorkbenchRun :
        public WorkbenchRun
{

public:
        GenieWorkbenchRun(void);
        ~GenieWorkbenchRun(void);
protected:
        GenieWorkbenchWindow           *genie_workbench_window;
        ISvpShell			 *svp_shell;
public:
        void setSvpShell(ISvpShell* shell);
public:
        int run(QMainWindow *main_window);
        void stop();
};
#endif // GENIEWORKBENCHRUN_H
