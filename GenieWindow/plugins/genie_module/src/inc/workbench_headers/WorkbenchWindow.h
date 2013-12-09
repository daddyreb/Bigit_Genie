#ifndef WORKBENCHWINDOW_H
#define WORKBENCHWINDOW_H

#include "Workbench.h"
//#include "IWorkbenchWindow.h"
#include "ObjectManager.h"
#include "RegisterObject.h"
#include "IExtension.h"
#include "IExtensionPoint.h"

class WorkbenchWindow 
{
public:
	WorkbenchWindow ();

	virtual bool create ()=0;

	virtual void show ()=0;

	virtual bool close()=0;

};
#endif