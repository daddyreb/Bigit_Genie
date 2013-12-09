#ifndef IVIEWPART_H
#define IVIEWPART_H
#include "IWorkbenchPart.h"
#include "IPersistable.h"
class IViewSite;
class IMemento;

class IViewPart:public IWorkbenchPart,public IPersistable
{
public:
	/**
	* Returns the site for this view. 
	* This method is equivalent to <code>(IViewSite) getSite()</code>.
	* <p>  
	* The site can be <code>null</code> while the view is being initialized. 
	* After the initialization is complete, this value must be non-<code>null</code>
	* for the remainder of the view's life cycle.
	* </p>
	* 
	* @return the view site; this value may be <code>null</code> if the view
	*         has not yet been initialized
	*/
	virtual IViewSite* getViewSite()=0;

	/**
	* Initializes this view with the given view site.  
	* <p>
	* This method is automatically called by the workbench shortly after the
	* part is instantiated.  It marks the start of the views's lifecycle. Clients must 
	* not call this method.
	* </p>
	*
	* @param site the view site
	* @exception PartInitException if this view was not initialized successfully
	*/
	virtual void init(IViewSite* site)=0;

	/**
	* Initializes this view with the given view site.  A memento is passed to
	* the view which contains a snapshot of the views state from a previous
	* session.  Where possible, the view should try to recreate that state
	* within the part controls.
	* <p>
	* This method is automatically called by the workbench shortly after the part 
	* is instantiated.  It marks the start of the views's lifecycle. Clients must 
	* not call this method.
	* </p>
	*
	* @param site the view site
	* @param memento the IViewPart state or null if there is no previous saved state
	* @exception PartInitException if this view was not initialized successfully
	*/
	virtual void init(IViewSite* site, IMemento* memento)=0;

	/**
	* Saves the object state within a memento.
	*
	* @param memento a memento to receive the object state
	*/
	virtual void saveState(IMemento* memento)=0;
};
#endif