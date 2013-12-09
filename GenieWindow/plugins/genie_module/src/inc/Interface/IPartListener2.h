#ifndef IPARTLISTENER2_H
#define IPARTLISTENER2_H
class IWorkbenchPartReference;

class IPartListener2
{
public:
	/**
	* Notifies this listener that the given part has been activated.
	*
	* @param partRef the part that was activated
	* @see IWorkbenchPage#activate
	*/
	virtual void partActivated(IWorkbenchPartReference* partRef)=0;

	/**
	* Notifies this listener that the given part has been brought to the top.
	* <p>
	* These events occur when an editor is brought to the top in the editor area,
	* or when a view is brought to the top in a page book with multiple views.
	* They are normally only sent when a part is brought to the top 
	* programmatically (via <code>IPerspective.bringToTop</code>). When a part is
	* activated by the user clicking on it, only <code>partActivated</code> is sent.
	* </p>
	*
	* @param partRef the part that was surfaced
	* @see IWorkbenchPage#bringToTop
	*/
	virtual void partBroughtToTop(IWorkbenchPartReference* partRef)=0;

	/**
	* Notifies this listener that the given part has been closed.
	* <p>
	* Note that if other perspectives in the same page share the view,
	* this notification is not sent.  It is only sent when the view
	* is being removed from the page entirely (it is being disposed).
	* </p>
	*
	* @param partRef the part that was closed
	* @see IWorkbenchPage#hideView
	*/
	virtual void partClosed(IWorkbenchPartReference* partRef)=0;

	/**
	* Notifies this listener that the given part has been deactivated.
	*
	* @param partRef the part that was deactivated
	* @see IWorkbenchPage#activate
	*/
	virtual void partDeactivated(IWorkbenchPartReference* partRef)=0;

	/**
	* Notifies this listener that the given part has been opened.
	* <p>
	* Note that if other perspectives in the same page share the view,
	* this notification is not sent.  It is only sent when the view
	* is being newly opened in the page (it is being created).
	* </p>
	*
	* @param partRef the part that was opened
	* @see IWorkbenchPage#showView
	*/
	virtual void partOpened(IWorkbenchPartReference* partRef)=0;

	/**
	* Notifies this listener that the given part is hidden or obscured by another part.
	*
	* @param partRef the part that is hidden or obscured by another part
	*/
	virtual void partHidden(IWorkbenchPartReference* partRef)=0;

	/**
	* Notifies this listener that the given part is visible.
	*
	* @param partRef the part that is visible
	*/
	virtual void partVisible(IWorkbenchPartReference* partRef)=0;

	/**
	* Notifies this listener that the given part's input was changed.
	*
	* @param partRef the part whose input was changed
	*/
	virtual void partInputChanged(IWorkbenchPartReference* partRef)=0;
};
#endif