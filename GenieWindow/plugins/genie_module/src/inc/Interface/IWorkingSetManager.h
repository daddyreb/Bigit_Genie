#ifndef IWORKINGSETMANAGER_H
#define IWORKINGSETMANAGER_H
class IAdaptable;
class IWorkingSet;
class IPropertyChangeListener;
class IMemento;

#include <QString>

class IWorkingSetManager
{
virtual:
	/**
	* Adds a property change listener.
	* 
	* @param listener the property change listener to add
	*/
	virtual void addPropertyChangeListener(IPropertyChangeListener* listener)=0;

	/**
	* Adds a working set to the top of the list of most recently used 
	* working sets, making it the most recently used working set.
	* The last (oldest) item will be deleted if the list exceeds the 
	* size limit.
	* 
	* @param workingSet the working set to add to the list of most 
	* 	recently used working sets.
	*/
	virtual void addRecentWorkingSet(IWorkingSet* workingSet)=0;

	/**
	* Adds a working set to the receiver. The working set must 
	* not exist yet.
	* 
	* @param workingSet the working set to add
	*/
	virtual void addWorkingSet(IWorkingSet* workingSet)=0;

	/**
	* Creates a new working set.
	* The working set is not added to the working set manager.
	* 
	* @param name the name of the new working set. Should not have 
	* 	leading or trailing whitespace.
	* @param elements the working set contents
	* @return a new working set with the specified name and content
	*/
	virtual IWorkingSet* createWorkingSet(QString name, IAdaptable* elements)=0;

	/**
	* Create a working set that is the union of a collection of other working
	* sets. One connected (via
	* {@link IWorkingSetManager#addWorkingSet(IWorkingSet)} this working set
	* will be automatically updated to reflect the contents of the component
	* sets, should they themselves change.
	* 
	* @param name
	*            the name of the new working set. Should not have leading or
	*            trailing whitespace.
	* @param label
	*            the user-friendly label the working set
	* @param components
	*            the component working sets
	* @return a new working set with the specified name and content
	* 
	* @since 3.2
	*/
	virtual IWorkingSet* createAggregateWorkingSet(QString name, QString label,
		IWorkingSet* components)=0;

	/**
	* Re-creates and returns a working set from the state captured within the 
	* given memento. 
	*
	* @param memento a memento containing the state for the working set
	* @return the restored working set, or <code>null</code> if it could not be created
	* 
	* @since 3.0
	*/
	virtual IWorkingSet* createWorkingSet(IMemento* memento)=0;

	/**
	* Returns the list of most recently used working sets.
	* The most recently used working set appears first in the list.
	* 
	* @return the list of most recently used working sets
	*/
	virtual IWorkingSet* getRecentWorkingSets()=0;

	/**
	* Returns the working set with the specified name.
	* Returns null if there is no working set with that name.
	* 
	* @param name the name of the working set to return
	* @return the working set with the specified name.
	*/
	virtual IWorkingSet* getWorkingSet(QString name)=0;

	/**
	* Returns an array of all working sets stored in the receiver. Any working
	* set whose {@link IWorkingSet#isVisible()} method returns false will not be
	* included in this array. For a complete list of working sets please use
	* {@link #getAllWorkingSets()}.
	* 
	* @return the working sets stored in the receiver
	*/
	virtual IWorkingSet* getWorkingSets()=0;

	/**
	* Returns an array of all working sets stored in the receiver including
	* those that are marked as being not visible.
	* 
	* @see IWorkingSet#isVisible()
	* @return the working sets stored in the receiver
	* @since 3.2
	*/
	virtual IWorkingSet* getAllWorkingSets()=0;

	/**
	* Removes the property change listener.
	* 
	* @param listener the property change listener to remove
	*/
	virtual void removePropertyChangeListener(IPropertyChangeListener* listener)=0;

	/**
	* Removes the working set
	* 
	* @param workingSet the working set to remove
	*/
	virtual void removeWorkingSet(IWorkingSet* workingSet)=0;

	/**
	* Disposes the working set manager.
	* 
	* @since 3.1
	*/
	virtual void dispose()=0;

	/**
	* Utility method that will add the <code>element</code> to each given
	* working set in <code>workingSets</code> if possible. This method will
	* invoke {@link IWorkingSet#adaptElements(IAdaptable[])} for the element on
	* each working set and the result of this method will be used rather than
	* the original element in the addition operation.
	* 
	* @param element
	*            the element to adapt and then add to the working sets
	* @param workingSets
	*            the working sets to add the element to
	* @since 3.4
	*/
	virtual void addToWorkingSets(IAdaptable* element, IWorkingSet* workingSets)=0;
};
#endif