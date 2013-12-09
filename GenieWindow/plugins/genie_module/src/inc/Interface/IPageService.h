#ifndef IPAGESERVICE_H
#define IPAGESERVICE_H
class IWorkbenchPage;
class IPageListener;
class IPerspectiveListener;

class IPageService
{
public:
	/**
	* Adds the given listener for page lifecycle events.
	* Has no effect if an identical listener is already registered.
	* <p>
	* <b>Note:</b> listeners should be removed when no longer necessary. If
	* not, they will be removed when the IServiceLocator used to acquire this
	* service is disposed.
	* </p>
	*
	* @param listener a page listener
	* @see #removePageListener(IPageListener)
	*/
	virtual void addPageListener(IPageListener* listener)=0;

	/**
	* Adds the given listener for a page's perspective lifecycle events.
	* Has no effect if an identical listener is already registered.
	* <p>
	* <b>Note:</b> listeners should be removed when no longer necessary. If
	* not, they will be removed when the IServiceLocator used to acquire this
	* service is disposed.
	* </p>
	*
	* @param listener a perspective listener
	* @see #removePerspectiveListener(IPerspectiveListener)
	*/
	virtual void addPerspectiveListener(IPerspectiveListener* listener)=0;

	/**
	* Returns the active page.
	*
	* @return the active page, or <code>null</code> if no page is currently active
	*/
	virtual IWorkbenchPage* getActivePage()=0;

	/**
	* Removes the given page listener.
	* Has no affect if an identical listener is not registered.
	*
	* @param listener a page listener
	*/
	virtual void removePageListener(IPageListener* listener)=0;

	/**
	* Removes the given page's perspective listener.
	* Has no affect if an identical listener is not registered.
	*
	* @param listener a perspective listener
	*/
	virtual void removePerspectiveListener(IPerspectiveListener* listener)=0;
};
#endif