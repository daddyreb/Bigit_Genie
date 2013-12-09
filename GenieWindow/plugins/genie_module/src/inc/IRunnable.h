#ifndef IRUNNABLE_H
#define IRUNNABLE_H

#include "IService.h"
class QMainWindow;

using namespace svp::framework;

class IRunnable:public IService
{
public:
	virtual int run(QMainWindow *main_window)=0;
	virtual void stop()=0;
};
#endif