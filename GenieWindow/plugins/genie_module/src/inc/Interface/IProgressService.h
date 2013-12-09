#ifndef IPROGRESSSERVICE_H
#define IPROGRESSSERVICE_H
#include "IRunnableContext.h"
#include <QObject>

class IRunnableWithProgress;
class ISchedulingRule;


class IProgressService:public IRunnableContext
{
public:
	/**
	* The time at which an operation becomes considered a long
	* operation. Used to determine when the busy cursor will 
	* be replaced with a progress monitor.
	* @return int 
	* @see IProgressService#busyCursorWhile(IRunnableWithProgress)
	*/
	virtual int getLongOperationTime()=0;

	/**
	* Runs the given operation in the UI thread using the given runnable context.  
	* The given scheduling rule, if any, will be acquired for the duration of the operation. 
	* If the rule is not available when this method is called, a progress dialog will be 
	* displayed that gives users control over the background processes that may 
	* be blocking the runnable from proceeding.
	* <p>
	* This method can act as a wrapper for uses of <tt>IRunnableContext</tt>
	* where the <tt>fork</tt> parameter was <tt>false</tt>. 
	* <p>
	* Note: Running long operations in the UI thread is generally not 
	* recommended. This can result in the UI becoming unresponsive for
	* the duration of the operation. Where possible, <tt>busyCursorWhile</tt>
	* should be used instead.  
	* </p>
	* <p>
	* Modal dialogs should also be avoided in the runnable as there will already
	* be a modal progress dialog open when this operation runs.
	* </p>
	* @see org.eclipse.jface.dialogs.Dialog
	* @see org.eclipse.swt.SWT#APPLICATION_MODAL
	* 
	* @param context The runnable context to run the operation in
	* @param runnable The operation to run
	* @param rule A scheduling rule, or <code>null</code>
	* @throws InvocationTargetException wraps any exception or error which occurs 
	*  while running the runnable
	* @throws InterruptedException propagated by the context if the runnable 
	*  acknowledges cancelation by throwing this exception.
	*  
	*/
	virtual void runInUI(IRunnableContext* context,
		IRunnableWithProgress* runnable, ISchedulingRule* rule)=0;


	/**
	* Set the cursor to busy and run the runnable in a non-UI Thread.
	* The calling thread will be blocked for the duration of the execution
	* of the supplied runnable.
	* 
	* After the cursor has been running for 
	* <code>getLongOperationTime()</code> replace it with
	* a ProgressMonitorDialog so that the user may cancel.
	* Do not open the ProgressMonitorDialog if there is already a modal
	* dialog open.
	* 
	* @param runnable The runnable to execute and show the progress for.
	* @see IProgressService#getLongOperationTime
	* @throws InvocationTargetException
	* @throws InterruptedException
	*/
	virtual void busyCursorWhile(IRunnableWithProgress* runnable)=0;

	/**
	* This specialization of IRunnableContext#run(bool, bool,
	* IRunnableWithProgress) might run the runnable asynchronously
	* if <code>fork</code> is <code>true</code>.
	* 
	* @since 3.2
	*/
	virtual void run(bool fork, bool cancelable, IRunnableWithProgress* runnable)=0;
};
#endif