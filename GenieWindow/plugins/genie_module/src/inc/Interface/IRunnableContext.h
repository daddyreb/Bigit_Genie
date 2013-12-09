#ifndef IRUNNABLECONTEXT_H
#define IRUNNABLECONTEXT_H
class IRunnableWithProgress;

class IRunnableContext
{
public:
	/**
	* <p>
	* Runs the given <code>IRunnableWithProgress</code> in this context.
	* For example, if this is a <code>ProgressMonitorDialog</code> then the runnable
	* is run using this dialog's progress monitor.
	* </p>
	* <p>
	* If <code>fork</code> is <code>false</code>, the current thread is used
	* to run the runnable. Note that if <code>fork</code> is <code>true</code>,
	* it is unspecified whether or not this method blocks until the runnable
	* has been run. Implementers should document whether the runnable is run
	* synchronously (blocking) or asynchronously (non-blocking), or if no
	* assumption can be made about the blocking behaviour.
	* </p>
	*
	* @param fork <code>true</code> if the runnable should be run in a separate thread,
	*  and <code>false</code> to run in the same thread
	* @param cancelable <code>true</code> to enable the cancelation, and
	*  <code>false</code> to make the operation uncancellable
	* @param runnable the runnable to run
	*
	* @exception InvocationTargetException wraps any exception or error which occurs 
	*  while running the runnable
	* @exception InterruptedException propagated by the context if the runnable 
	*  acknowledges cancelation by throwing this exception.  This should not be thrown
	*  if cancelable is <code>false</code>.
	*/
	virtual void run(bool fork, bool cancelable,
		IRunnableWithProgress* runnable)=0;
};
#endif