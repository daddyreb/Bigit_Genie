#ifndef IPROPERTYLISTENER
#define IPROPERTYLISTENER

#include <QObject>

class IPropertyListener
{
public:
	/**
	* Indicates that a property has changed.
	*
	* @param source the object whose property has changed
	* @param propId the id of the property which has changed; property ids
	*   are generally defined as constants on the source class
	*/
virtual void propertyChanged(QObject* source, int propId)=0;
};
#endif