#ifndef IVIEWCATEGORY_H
#define IVIEWCATEGORY_H
class IViewDescriptor;
#include <QString>

class IViewCategory
{
public:
	/**
	* Return the views contained within this category. Never <code>null</code>,
	* but may be empty.
	* 
	* @return the views contained within this category
	*/
	IViewDescriptor* getViews()=0;

	/**
	* Return the id of this category.  Never <code>null</code>.
	* 
	* @return the id
	*/
	QString getId()=0;

	/**
	* Return the human readable name of this view category.  Never <code>null</code>.
	* 
	* @return the label
	*/
	QString getLabel()=0;

};
#endif