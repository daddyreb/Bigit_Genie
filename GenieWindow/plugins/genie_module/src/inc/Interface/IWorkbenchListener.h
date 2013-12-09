#ifndef IWORKBENCHLISTENER_H
#define IWORKBENCHLISTENER_H
class IWorkbench;

class IWorkbenchListener
{
public:
	/**
	* Notifies that the workbench is about to shut down.
	* <p>
	* This method is called immediately prior to workbench shutdown before any
	* windows have been closed.
	* </p>
	* <p>
	* The listener may veto a regular shutdown by returning <code>false</code>, 
	* although this will be ignored if the workbench is being forced to shut down.
	* </p>
	* <p>
	* Since other workbench listeners may veto the shutdown, the listener should
	* not dispose resources or do any other work during this notification that would 
	* leave the workbench in an inconsistent state.
	* </p>
	* 
	* @param workbench the workbench
	* @param forced <code>true</code> if the workbench is being forced to shutdown,
	*   <code>false</code> for a regular close
	* @return <code>true</code> to allow the workbench to proceed with shutdown,
	*   <code>false</code> to veto a non-forced shutdown
	*/
	virtual bool preShutdown(IWorkbench* workbench, bool forced)=0;

	/**
	* Performs arbitrary finalization after the workbench stops running.
	* <p>
	* This method is called during workbench shutdown after all windows
	* have been closed.
	* </p>
	* 
	* @param workbench the workbench
	*/
	virtual void postShutdown(IWorkbench* workbench)=0;
};
#endif