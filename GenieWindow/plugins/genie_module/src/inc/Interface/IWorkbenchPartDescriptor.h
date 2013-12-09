#ifndef IWORKBENCHPARTDESCRIPTOR_H
#define IWORKBENCHPARTDESCRIPTOR_H
#include <QString>

class IWorkbenchPartDescriptor
{
public:
	/**
	* Returns the part id.
	*
	* @return the id of the part
	*/
	virtual QString getId()=0;


	/**
	* Returns the label to show for this part.
	*
	* @return the part label
	*/
	virtual QString getLabel()=0;
};
#endif