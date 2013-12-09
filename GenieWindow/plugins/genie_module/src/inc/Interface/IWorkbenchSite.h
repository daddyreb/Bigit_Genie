#ifndef IWORKBENCHSITE_H
#define IWORKBENCHSITE_H
#include "IServiceLocator.h"
class ISelectionProvider;
class IWorkbenchPage;
class IWorkbenchWindow;

class IWorkbenchSite:public IServiceLocator
{
public:
	virtual IWorkbenchPage* getPage()=0;

	/**
	* Returns the selection provider for this workbench site.
	* 
	* @return the selection provider, or <code>null</code> if none
	*/
	virtual ISelectionProvider* getSelectionProvider()=0;

	/**
	* Returns the workbench window containing this workbench site.
	* 
	* @return the workbench window containing this workbench site
	*/
	virtual IWorkbenchWindow* getWorkbenchWindow()=0;

	/**
	* Sets the selection provider for this workbench site.
	* 
	* @param provider
	*            the selection provider, or <code>null</code> to clear it
	*/
	virtual void setSelectionProvider(ISelectionProvider* provider)=0;
};
#endif