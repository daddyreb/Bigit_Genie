#ifndef IPERSISTABLEELEMENT_H
#define IPERSISTABLEELEMENT_H
#include "IPersistable;
#include <QString>

class IPersistableElement:IPersistable
{
public:
	/**
	* Returns the id of the element factory which should be used to re-create this
	* object.
	* <p>
	* Factory ids are declared in extensions to the standard extension point
	* <code>"org.eclipse.ui.elementFactories"</code>.
	* </p>
	* 
	* @return the element factory id
	* @see IElementFactory
	*/
	virtual QString getFactoryId()=0;
};
#endif