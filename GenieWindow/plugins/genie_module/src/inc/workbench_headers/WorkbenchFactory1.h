#ifndef WORKBENCHFACTORY1_H
#define WORKBENCHFACTORY1_H

#include "workbenchfactory.h"
class WorkbenchWindow1;
class WorkbenchWindow;

class WorkbenchFactory1 :
	public WorkbenchFactory
{
public:
	WorkbenchFactory1 ();
	WorkbenchWindow1* createWorkbenchWindow();
private:
	WorkbenchWindow1		*m_WorkbenchWnd;

};

#endif