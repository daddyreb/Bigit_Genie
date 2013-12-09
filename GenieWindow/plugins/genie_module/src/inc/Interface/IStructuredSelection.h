#ifndef ISTRUCTUREDSELECTION_H
#define ISTRUCTUREDSELECTION_H
#include <QObject>
#include "ISelection.h"

class IStructuredSelection:public ISelection
{
public:
	/**
	* Returns the first element in this selection, or <code>null</code>
	* if the selection is empty.
	*
	* @return an element, or <code>null</code> if none
	*/
	virtual QObject* getFirstElement()=0;


	/**
	* Returns the number of elements selected in this selection.
	*
	* @return the number of elements selected
	*/
	virtual int size()=0;

	/**
	* Returns the elements in this selection as an array.
	*
	* @return the selected elements as an array
	*/
	virtual QObject* toArray()=0;

};
#endif