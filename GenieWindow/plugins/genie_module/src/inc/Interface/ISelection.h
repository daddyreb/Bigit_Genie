#ifndef ISELECTION_H
#define ISELECTION_H
class ISelection
{
public:
	/**
	* Returns whether this selection is empty.
	* 
	* @return <code>true</code> if this selection is empty,
	*   and <code>false</code> otherwise
	*/
	virtual bool isEmpty()=0;
};
#endif