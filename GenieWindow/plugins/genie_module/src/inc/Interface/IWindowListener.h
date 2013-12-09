#ifndef IWINDOWLISTENER_H
#define IWINDOWLISTENER_H
class IWorkbenchWindow;

class IWindowListener
{
public:
	/**
	* Notifies this listener that the given window has been activated.
	* <p>
	* <b>Note:</b> This event is not fired if no perspective is
	* open (the window is empty).
	* </p>
	*
	* @param window the window that was activated
	*/
	virtual void windowActivated(IWorkbenchWindow* window)=0;

	/**
	* Notifies this listener that the given window has been deactivated.
	* <p>
	* <b>Note:</b> This event is not fired if no perspective is
	* open (the window is empty).
	* </p>
	*
	* @param window the window that was activated
	*/
	virtual void windowDeactivated(IWorkbenchWindow* window)=0;

	/**
	* Notifies this listener that the given window has been closed.
	*
	* @param window the window that was closed
	* @see IWorkbenchWindow#close
	*/
	virtual void windowClosed(IWorkbenchWindow* window)=0;

	/**
	* Notifies this listener that the given window has been opened.
	*
	* @param window the window that was opened
	* @see IWorkbench#openWorkbenchWindow
	*/
	virtual void windowOpened(IWorkbenchWindow* window)=0;
};
#endif