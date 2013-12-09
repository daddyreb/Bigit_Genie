#ifndef IPARTLISTENER_H
#define IPARTLISTENER_H
class IWorkbenchPart;

class IPartListener
{
public:
	/**
	* Notifies this listener that the given part has been activated.
	*
	* @param part the part that was activated
	* @see IWorkbenchPage#activate
	*/
	virtual void partActivated(IWorkbenchPart* part)=0;

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
	* @param part the part that was surfaced
	* @see IWorkbenchPage#bringToTop
	*/
	virtual void partBroughtToTop(IWorkbenchPart* part)=0;

	/**
	* Notifies this listener that the given part has been closed.
	*
	* @param part the part that was closed
	* @see IWorkbenchPage#hideView(IViewPart)
	*/
	virtual void partClosed(IWorkbenchPart* part)=0;

	/**
	* Notifies this listener that the given part has been deactivated.
	*
	* @param part the part that was deactivated
	* @see IWorkbenchPage#activate(IWorkbenchPart)
	*/
	virtual void partDeactivated(IWorkbenchPart* part)=0;

	/**
	* Notifies this listener that the given part has been opened.
	*
	* @param part the part that was opened
	* @see IWorkbenchPage#showView(String)
	*/
	virtual void partOpened(IWorkbenchPart* part)=0;
};
#endif