#ifndef IPERSPECTIVELISTENER_H
#define IPERSPECTIVELISTENER_H
class IPerspectiveDescriptor;
class IWorkbenchPage;
#include <QString>

class IPerspectiveListener
{
public:
	/**
	* Notifies this listener that a perspective in the given page
	* has been activated.
	*
	* @param page the page containing the activated perspective
	* @param perspective the perspective descriptor that was activated
	* @see IWorkbenchPage#setPerspective
	*/
	virtual void perspectiveActivated(IWorkbenchPage* page,
		IPerspectiveDescriptor* perspective)=0;

	/**
	* Notifies this listener that a perspective has changed in some way 
	* (for example, editor area hidden, perspective reset,
	* view show/hide, editor open/close, etc).
	*
	* @param page the page containing the affected perspective
	* @param perspective the perspective descriptor
	* @param changeId one of the <code>CHANGE_*</code> constants on IWorkbenchPage
	*/
	virtual void perspectiveChanged(IWorkbenchPage* page,
		IPerspectiveDescriptor* perspective, QString changeId)=0;
};
#endif