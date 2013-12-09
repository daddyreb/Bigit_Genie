#ifndef IADAPTABLE_H
#define IADAPTABLE_H
#include <QObject>
#include <QString>

class IAdaptable
{
public:
	/**
	* Returns an object which is an instance of the given class
	* associated with this object. Returns <code>null</code> if
	* no such object can be found.
	*
	* @param adapter the adapter class to look up
	* @return a object castable to the given class, 
	*    or <code>null</code> if this object does not
	*    have an adapter for the given class
	*/
	virtual QObject* getAdapter(QString adapter)=0;
};
#endif