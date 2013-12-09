#ifndef IPARTSERVICE_H
#define IPARTSERVICE_H
class IWorkbenchPart;
class IPartListener;
class IPartListener2;
class IWorkbenchPartReference;

class IPartService
{
public:
	/**
	* Adds the given listener for part lifecycle events.
	* Has no effect if an identical listener is already registered.
	* <p>
	* <b>Note:</b> listeners should be removed when no longer necessary. If
	* not, they will be removed when the IServiceLocator used to acquire this
	* service is disposed.
	* </p>
	*
	* @param listener a part listener
	* @see #removePartListener(IPartListener)
	*/
	virtual void addPartListener(IPartListener* listener)=0;

	/**
	* Adds the given listener for part lifecycle events.
	* Has no effect if an identical listener is already registered.
	* <p>
	* As of 3.5, the IPartListener2 can also implement IPageChangedListener
	* to be notified about any parts that implement IPageChangeProvider and
	* post PageChangedEvents.
	* </p>
	* <p>
	* <b>Note:</b> listeners should be removed when no longer necessary. If
	* not, they will be removed when the IServiceLocator used to acquire this
	* service is disposed.
	* </p>
	*
	* @param listener a part listener
	* @see #removePartListener(IPartListener2)
	* @see org.eclipse.jface.dialogs.IPageChangeProvider
	* @see org.eclipse.jface.dialogs.IPageChangedListener
	*/
	virtual void addPartListener(IPartListener2* listener)=0;

	/**
	* Returns the active part.
	*
	* @return the active part, or <code>null</code> if no part is currently active
	*/
	virtual IWorkbenchPart* getActivePart()=0;

	/**
	* Returns the active part reference.
	*
	* @return the active part reference, or <code>null</code> if no part
	* is currently active
	*/
	virtual IWorkbenchPartReference* getActivePartReference()=0;

	/**
	* Removes the given part listener.
	* Has no affect if an identical listener is not registered.
	*
	* @param listener a part listener
	*/
	virtual void removePartListener(IPartListener* listener)=0;

	/**
	* Removes the given part listener.
	* Has no affect if an identical listener is not registered.
	*
	* @param listener a part listener
	*/
	virtual void removePartListener(IPartListener2* listener)=0;
};
#endif