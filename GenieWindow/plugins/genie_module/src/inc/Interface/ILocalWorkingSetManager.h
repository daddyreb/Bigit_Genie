#ifndef ILOCALWORKINGSETMANAGER_H
#define ILOCALWORKINGSETMANAGER_H

#include "IWorkingSetManager.h"

class IMemento;

class ILocalWorkingSetManager:public IWorkingSetManager
{
public:
	/**
	* Saves the state of the working set manager to the given
	* memento. 
	* 
	* @param memento the memento to save the state to
	*/
	virtual void saveState(IMemento* memento)=0;

	/**
	* Restores the state of the working set manager from the given
	* memento. The method can only be called as long as the working
	* set manager is still empty.
	* 
	* @param memento the memento to restore the state from
	*/
	virtual void restoreState(IMemento* memento)=0;
};
#endif