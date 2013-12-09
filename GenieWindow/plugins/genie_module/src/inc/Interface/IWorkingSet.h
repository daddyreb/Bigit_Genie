#ifndef IWORKINGSET_H
#define IWORKINGSET_H
#include "IPersistableElement.h"
#include "IAdaptable.h"

class IWorkingSet:public IPersistableElement,public IAdaptable
{
public:
	/**
	* Returns the elements that are contained in this working set.
	* <p>
	* This method might throw an {@link IllegalStateException} if 
	* the working set is invalid.
	* </p>
	* @return	the working set's elements
	*/
	virtual IAdaptable* getElements()=0;

	/**
	* Returns the working set id. Returns <code>null</code> if no
	* working set id has been set.
	* This is one of the ids defined by extensions of the 
	* org.eclipse.ui.workingSets extension point.
	* It is used by the workbench to determine the page to use in 
	* the working set edit wizard. The default resource edit page
	* is used if this value is <code>null</code>.
	* 
	* @return the working set id. May be <code>null</code>
	* @since 2.1 
	*/
	virtual QString getId()=0;


	/**
	* Returns the name of the working set.
	* 
	* @return	the name of the working set
	*/
	virtual QString getName()=0;

	/**
	* Sets the elements that are contained in this working set.
	* 
	* @param elements
	*            the elements to set in this working set
	* @since 3.3 it is now recommended that all calls to this method pass
	*        through the results from calling
	*        {@link #adaptElements(IAdaptable[])} with the desired elements.
	*/
	virtual void setElements(IAdaptable* elements)=0;

	/**
	* Sets the working set id. This is one of the ids defined by extensions of
	* the org.eclipse.ui.workingSets extension point. It is used by the
	* workbench to determine the page to use in the working set edit wizard.
	* The default resource edit page is used if this value is <code>null</code>.
	* 
	* @param id
	*            the working set id. May be <code>null</code>
	* @since 2.1
	*/
	virtual void setId(QString id)=0;

	/**
	* Sets the name of the working set. 
	* The working set name should be unique.
	* The working set name must not have leading or trailing 
	* whitespace.
	* 
	* @param name the name of the working set
	*/
	virtual void setName(QString name)=0;

	/**
	* Returns whether this working set can be edited or not. To make
	* a working set editable the attribute <code>pageClass</code> of
	* the extension defining a working set must be provided.
	* 
	* @return <code>true</code> if the working set can be edited; otherwise
	*  <code>false</code>
	*  
	* @since 3.1
	*/
	virtual bool isEditable()=0;

	/**
	* Returns whether this working set should be shown in user interface
	* components that list working sets by name.
	* 
	* @return <code>true</code> if the working set should be shown in the
	*         user interface; otherwise <code>false</code>
	* 
	* @since 3.2
	*/
	virtual bool isVisible()=0;

	/**
	* Return the name of this working set, formated for the end user. Often this value is
	* the same as the one returned from {@link #getName()}.
	* 
	* @return the name of this working set, formated for the end user
	* @since 3.2
	*/
	virtual QString getLabel()=0;

	/**
	* Set the name of this working set, formated for the end user.
	* 
	* @param label
	*            the label for this working set. If <code>null</code> is
	*            supplied then the value of {@link #getName()} will be used.
	* @since 3.2
	*/
	virtual void setLabel(QString label)=0;

	/**
	* Returns <code>true</code> if this working set is capable of updating
	* itself and reacting to changes in the state of its members. For
	* non-aggregate working sets this means that the working set has an
	* {@link IWorkingSetUpdater} installed while for aggregates it means that
	* all component sets have {@link IWorkingSetUpdater}s installed. Otherwise
	* returns <code>false</code>.
	* 
	* @return whether the set is self-updating or not
	* @since 3.2
	*/
	virtual bool isSelfUpdating()=0;

	/**
	* Returns whether this working set is an aggregate working set or not.
	* 
	* <p>
	* It is recommended that clients of aggregate working sets treat them in a
	* specific way. Please see the documentation for
	* {@link IWorkbenchPage#getAggregateWorkingSet()} for details.
	* <p>
	* If this is true, you can cast this working set to an {@link IAggregateWorkingSet} 
	* 
	* @return whether this working set is an aggregate working set or not
	* @since 3.2
	*/
	virtual bool isAggregateWorkingSet()=0;	

	/**
	* Returns whether this working set is currently empty (has no elements).
	* 
	* @return whether this working set is currently empty
	* @since 3.2
	*/
	virtual bool isEmpty()=0;

	/**
	* Transforms the supplied elements into elements that are suitable for
	* containment in this working set. This is useful for UI elements which
	* wish to filter contributions to working sets based on applicability. This
	* is a hint, however, and is not considered when the
	* {@link #setElements(IAdaptable[])} method is invoked.
	* 
	* @param objects
	*            the objects to transform
	* @return an array of transformed elements that be empty if no elements
	*         from the original array are suitable
	* @since 3.3
	* @see org.eclipse.ui.IWorkingSetElementAdapter
	* @see org.eclipse.ui.BasicWorkingSetElementAdapter
	*/
	virtual IAdaptable* adaptElements(IAdaptable* objects)=0;
};
#endif