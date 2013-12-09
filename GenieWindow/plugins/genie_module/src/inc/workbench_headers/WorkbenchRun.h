#ifndef WORKBENCHRUN
#define WORKBENCHRUN

#include "IRunnable.h"
class QMainWindow;
class WorkbenchRun:public IRunnable
{
public:
	WorkbenchRun(void);
	~WorkbenchRun(void);
public:
	virtual int run(QMainWindow *main_window)=0;
	virtual void stop()=0;
};

#endif
