#ifndef IPAGELISTENER_H
#define IPAGELISTENER_H
class IWorkbenchPage;

class IPageListener
{
public:
	/**
	* Notifies this listener that the given page has been activated.
	*
	* @param page the page that was activated
	* @see IWorkbenchWindow#setActivePage
	*/
	virtual void pageActivated(IWorkbenchPage* page)=0;

	/**
	* Notifies this listener that the given page has been closed.
	*
	* @param page the page that was closed
	* @see IWorkbenchPage#close
	*/
	virtual void pageClosed(IWorkbenchPage* page)=0;

	/**
	* Notifies this listener that the given page has been opened.
	*
	* @param page the page that was opened
	* @see IWorkbenchWindow#openPage
	*/
	virtual void pageOpened(IWorkbenchPage* page)=0;
};
#endif