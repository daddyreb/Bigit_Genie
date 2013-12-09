#ifndef WORKBENCHEWINDOW1_H
#define WORKBENCHEWINDOW1_H

#include "workbenchwindow.h"
#include "Workbench.h"
#include "IWorkbenchWindow.h"
#include "ObjectManager.h"
#include "RegisterObject.h"
#include "IExtension.h"
#include "IExtensionPoint.h"

class WorkbenchWindow1 :
	public WorkbenchWindow
{
public:
	WorkbenchWindow1(void);
	~WorkbenchWindow1(void);

	bool create ();

	void show ();

	bool close();

private:
	Workbench			m_Workbench;
	ObjectManager objectManager;
	RegisterObject<IExtensionPoint>  ExtensionPointObject;
	RegisterObject<IExtension>  ExtensionObject;
};

#endif


