#ifndef IPERSISTABLE_H
#define IPERSISTABLE_H
class IMemento;

class IPersistable
{
public:
	/**
	* Saves the state of the object in the given memento.
	*
	* @param memento the storage area for object's state
	*/
	virtual void saveState(IMemento* memento)=0;
};
#endif