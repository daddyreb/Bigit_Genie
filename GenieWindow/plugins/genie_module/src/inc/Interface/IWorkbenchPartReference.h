#ifndef IWORKBENCHPARTREFERENCE_H
#define IWORKBENCHPARTREFERENCE_H
#include <QString>
#include <QImage>
class IWorkbenchPart;
class IWorkbenchPage;
class IPropertyListener;
class IPropertyChangeListener;
class IWorkbenchPage;

class IWorkbenchPartReference
{
public:
	int length;

	/**
	* Returns the IWorkbenchPart referenced by this object.
	* 
	* @param restore
	*            tries to restore the part if <code>true</code>.
	* @return the part, or <code>null</code> if the part was not instantiated
	*         or it failed to be restored.
	*/
	virtual IWorkbenchPart* getPart(bool restore)=0;

	/**
	* @see IWorkbenchPart#getTitle
	*/
	virtual QString getTitle()=0;

	/**
	* @see IWorkbenchPart#getTitleImage
	*/
	virtual QImage* getTitleImage()=0;

	/**
	* @see IWorkbenchPart#getTitleToolTip
	*/
	virtual QString getTitleToolTip()=0;

	/**
	* @see IWorkbenchPartSite#getId
	*/
	virtual QString getId()=0;

	/**
	* @see IWorkbenchPart#addPropertyListener
	*/
	virtual void addPropertyListener(IPropertyListener* listener)=0;

	/**
	* @see IWorkbenchPart#removePropertyListener
	*/
	virtual void removePropertyListener(IPropertyListener* listener)=0;

	/**
	* Returns the workbench page that contains this part
	*/
	virtual IWorkbenchPage* getPage()=0;

	/**
	* Returns the name of the part, as it should be shown in tabs.
	* 
	* @return the part name
	* 
	* @since 3.0
	*/
	virtual QString getPartName()=0;

	/**
	* Returns the content description for the part (or the empty string if none)
	* 
	* @return the content description for the part
	* 
	* @since 3.0  
	*/
	virtual QString getContentDescription()=0;


	/**
	* Returns whether the part is dirty (i.e. has unsaved changes).
	* 
	* @return <code>true</code> if the part is dirty, <code>false</code> otherwise
	* 
	* @since 3.2 (previously existed on IEditorReference)
	*/
	virtual bool isDirty()=0;

	/**
	* Return an arbitrary property from the reference. If the part has been
	* instantiated, it just delegates to the part. If not, then it looks in its
	* own cache of properties. If the property is not available or the part has
	* never been instantiated, it can return <code>null</code>.
	* 
	* @param key
	*            The property to return. Must not be <code>null</code>.
	* @return The String property, or <code>null</code>.
	* @since 3.3
	*/
	virtual QString getPartProperty(QString key)=0;

	/**
	* Add a listener for changes in the arbitrary properties set.
	* 
	* @param listener
	*            Must not be <code>null</code>.
	* @since 3.3
	*/
	virtual void addPartPropertyListener(IPropertyChangeListener* listener)=0;

	/**
	* Remove a listener for changes in the arbitrary properties set.
	* 
	* @param listener
	*            Must not be <code>null</code>.
	* @since 3.3
	*/
	virtual void removePartPropertyListener(IPropertyChangeListener* listener)=0;
};
#endif