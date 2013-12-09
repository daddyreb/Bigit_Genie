#ifndef IWORKBENCHOPERATIONSUPORT_H
#define IWORKBENCHOPERATIONSUPORT_H
class IUndoContext;

class IWorkbenchOperationSupport
{
public:
	/**
	* Returns the undo context for workbench-wide operations.
	* 
	* @return the workbench operation context
	*/
	virtual IUndoContext* getUndoContext()=0;


};
#endif