#ifndef ISELECTIONSERVICE_H
#define ISELECTIONSERVICE_H
#include <QString>
class ISelection;
class ISelectionListener;

class ISelectionService
{
public:
	/**
	* Adds the given selection listener.
	* Has no effect if an identical listener is already registered.
	* <p>
	* <b>Note:</b> listeners should be removed when no longer necessary. If
	* not, they will be removed when the IServiceLocator used to acquire this
	* service is disposed.
	* </p>
	*
	* @param listener a selection listener
	* @see #removeSelectionListener(ISelectionListener)
	*/
	virtual void addSelectionListener(ISelectionListener* listener)=0;

	/**
	* Adds a part-specific selection listener which is notified when selection changes
	* in the part with the given id. This is independent of part activation - the part
	* need not be active for notification to be sent.
	* <p>
	* When the part is created, the listener is passed the part's initial selection.
	* When the part is disposed, the listener is passed a <code>null</code> selection,
	* but only if the listener implements <code>INullSelectionListener</code>.
	* </p>
	* <p>
	* Note: This will not correctly track editor parts as each editor does 
	* not have a unique partId.
	* </p>
	* <p>
	* <b>Note:</b> listeners should be removed when no longer necessary. If
	* not, they will be removed when the IServiceLocator used to acquire this
	* service is disposed.
	* </p>
	*
	* @param partId the id of the part to track
	* @param listener a selection listener
	* @since 2.0
	* @see #removeSelectionListener(String, ISelectionListener)
	*/
	virtual void addSelectionListener(QString partId, ISelectionListener* listener)=0;

	/**
	* Adds the given post selection listener.It is equivalent to selection 
	* changed if the selection was triggered by the mouse but it has a 
	* delay if the selection is triggered by the keyboard arrows.
	* Has no effect if an identical listener is already registered.
	* 
	* Note: Works only for StructuredViewer(s).
	* <p>
	* <b>Note:</b> listeners should be removed when no longer necessary. If
	* not, they will be removed when the IServiceLocator used to acquire this
	* service is disposed.
	* </p>
	*
	* @param listener a selection listener
	* @see #removePostSelectionListener(ISelectionListener)
	*/
	virtual void addPostSelectionListener(ISelectionListener* listener)=0;

	/**
	* Adds a part-specific selection listener which is notified when selection changes
	* in the part with the given id. This is independent of part activation - the part
	* need not be active for notification to be sent.
	* <p>
	* When the part is created, the listener is passed the part's initial selection.
	* When the part is disposed, the listener is passed a <code>null</code> selection,
	* but only if the listener implements <code>INullSelectionListener</code>.
	* </p>
	* <p>
	* Note: This will not correctly track editor parts as each editor does 
	* not have a unique partId.
	* </p>
	* <p>
	* <b>Note:</b> listeners should be removed when no longer necessary. If
	* not, they will be removed when the IServiceLocator used to acquire this
	* service is disposed.
	* </p>
	*
	* @param partId the id of the part to track
	* @param listener a selection listener
	* @since 2.0
	* @see #removePostSelectionListener(String, ISelectionListener)
	*/
	virtual void addPostSelectionListener(QString partId,
		ISelectionListener* listener)=0;

	/**
	* Returns the current selection in the active part.  If the selection in the
	* active part is <em>undefined</em> (the active part has no selection provider)
	* the result will be <code>null</code>.
	*
	* @return the current selection, or <code>null</code> if undefined
	*/
	virtual ISelection* getSelection()=0;

	/**
	* Returns the current selection in the part with the given id.  If the part is not open,
	* or if the selection in the active part is <em>undefined</em> (the active part has no selection provider)
	* the result will be <code>null</code>.
	*
	* @param partId the id of the part
	* @return the current selection, or <code>null</code> if undefined
	* @since 2.0
	*/
	virtual ISelection* getSelection(QString partId)=0;

	/**
	* Removes the given selection listener.
	* Has no effect if an identical listener is not registered.
	*
	* @param listener a selection listener
	*/
	virtual void removeSelectionListener(ISelectionListener* listener)=0;

	/**
	* Removes the given part-specific selection listener.
	* Has no effect if an identical listener is not registered for the given part id.
	*
	* @param partId the id of the part to track
	* @param listener a selection listener
	* @since 2.0
	*/
	virtual void removeSelectionListener(QString partId,
		ISelectionListener* listener)=0;

	/**
	* Removes the given post selection listener.
	* Has no effect if an identical listener is not registered.
	*
	* @param listener a selection listener
	*/
	virtual void removePostSelectionListener(ISelectionListener* listener)=0;

	/**
	* Removes the given part-specific post selection listener.
	* Has no effect if an identical listener is not registered for the given part id.
	*
	* @param partId the id of the part to track
	* @param listener a selection listener
	* @since 2.0
	*/
	virtual void removePostSelectionListener(QString partId,
		ISelectionListener* listener)=0;
};
#endif