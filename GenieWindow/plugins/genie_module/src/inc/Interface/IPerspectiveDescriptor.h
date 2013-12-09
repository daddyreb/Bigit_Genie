#ifndef IPERSPECTIVEDESCRIPTOR_H
#define IPERSPECTIVEDESCRIPTOR_H
#include <QString>

class IPerspectiveDescriptor
{
public:
	/**
	* Returns the description of this perspective.
	* This is the value of its <code>"description"</code> attribute.
	*
	* @return the description
	* @since 3.0
	*/
	virtual QString getDescription()=0;

	/**
	* Returns this perspective's id. For perspectives declared via an extension,
	* this is the value of its <code>"id"</code> attribute.
	*
	* @return the perspective id
	*/
	virtual QString getId()=0;

	/**
	* Returns this perspective's label. For perspectives declared via an extension,
	* this is the value of its <code>"label"</code> attribute.
	*
	* @return the label
	*/
	virtual QString getLabel()=0;
};
#endif