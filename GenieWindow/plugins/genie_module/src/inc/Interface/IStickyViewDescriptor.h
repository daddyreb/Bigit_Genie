#ifndef ISTICKYVIEWDESCRIPTOR_H
#define ISTICKYVIEWDESCRIPTOR_H
#include <QString>

class IStickyViewDescriptor
{
virtual:
	/**
	* Return the id of the view to be made sticky.
	* 
	* @return the id of the view to be made sticky
	*/
	virtual QString getId()=0;

	/**
	* Return the location of this sticky view.  Must be one of 
	* <code>IPageLayout.LEFT</code>, <code>IPageLayout.RIGHT</code>, 
	* <code>IPageLayout.TOP</code>, or <code>IPageLayout.BOTTOM</code>.
	* 
	* @return the location constant
	*/
	virtual int getLocation()=0;

	/**
	* Return whether this view should be closeable.
	* 
	* @return whether this view should be closeeable
	*/
	virtual bool isCloseable()=0;

	/**
	* Return whether this view should be moveable.
	* 
	* @return whether this view should be moveable
	*/
	virtual bool isMoveable()=0;
};
#endif