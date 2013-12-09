#ifndef IWORKBENCH_H
#define IWORKBENCH_H

#include <QString>
#include <QImage>
//#include "Composite.h"
#include "IPropertyListener.h"
#include "IWorkbenchPartSite.h"


class IWorkbenchPart
{
public:
	/**
	* Adds a listener for changes to properties of this workbench part.
	* Has no effect if an identical listener is already registered.
	* <p>
	* The property ids are defined in {@link IWorkbenchPartConstants}.
	* </p>
	*
	* @param listener a property listener
	*/
	virtual void addPropertyListener(IPropertyListener* listener)=0;


	/**
	* Disposes of this workbench part.
	* <p>
	* This is the last method called on the <code>IWorkbenchPart</code>.  At this
	* point the part controls (if they were ever created) have been disposed as part 
	* of an SWT composite.  There is no guarantee that createPartControl() has been 
	* called, so the part controls may never have been created.
	* </p>
	* <p>
	* Within this method a part may release any resources, fonts, images, etc.&nbsp; 
	* held by this part.  It is also very important to deregister all listeners
	* from the workbench.
	* </p>
	* <p>
	* Clients should not call this method (the workbench calls this method at
	* appropriate times).
	* </p>
	*/
	virtual void dispose()=0;

	/**
	* Returns the site for this workbench part. The site can be
	* <code>null</code> while the workbench part is being initialized. After
	* the initialization is complete, this value must be non-<code>null</code>
	* for the remainder of the part's life cycle.
	* 
	* @return The part site; this value may be <code>null</code> if the part
	*         has not yet been initialized
	*/
	//virtual IWorkbenchPartSite* getSite()=0;

	/**
	* Returns the title of this workbench part. If this value changes 
	* the part must fire a property listener event with 
	* <code>PROP_TITLE</code>.
	* <p>
	* The title is used to populate the title bar of this part's visual
	* container.  
	* </p>
	*
	* @return the workbench part title (not <code>null</code>)
	*/
	virtual QString getTitle()=0;

	/**
	* Returns the title image of this workbench part.  If this value changes 
	* the part must fire a property listener event with 
	* <code>PROP_TITLE</code>.
	* <p>
	* The title image is usually used to populate the title bar of this part's
	* visual container. Since this image is managed by the part itself, callers
	* must <b>not</b> dispose the returned image.
	* </p>
	*
	* @return the title image
	*/
	virtual QImage* getTitleImage()=0;

	/**
	* Returns the title tool tip text of this workbench part. 
	* An empty string result indicates no tool tip.
	* If this value changes the part must fire a property listener event with 
	* <code>PROP_TITLE</code>.
	* <p>
	* The tool tip text is used to populate the title bar of this part's 
	* visual container.  
	* </p>
	*
	* @return the workbench part title tool tip (not <code>null</code>)
	*/
	virtual QString getTitleToolTip()=0;

	/**
	* Removes the given property listener from this workbench part.
	* Has no affect if an identical listener is not registered.
	*
	* @param listener a property listener
	*/
	virtual void removePropertyListener(IPropertyListener* listener)=0;

	/**
	* Asks this part to take focus within the workbench. Parts must
	* assign focus to one of the controls contained in the part's
	* parent composite.
	* <p>
	* Clients should not call this method (the workbench calls this method at
	* appropriate times).  To have the workbench activate a part, use
	* <code>IWorkbenchPage.activate(IWorkbenchPart) instead</code>.
	* </p>
	*/
	virtual void setFocus()=0;
};
#endif