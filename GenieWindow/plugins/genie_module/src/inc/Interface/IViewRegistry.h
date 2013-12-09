#ifndef IVIEWREGISTRY_H
#define IVIEWREGISTRY_H
#include <QString>
class IViewDescriptor;
class IViewCategory;
class IStickyViewDescriptor;
class IViewRegistry
{
public:
	/**
	* Return a view descriptor with the given extension id.  If no view exists
	* with the id return <code>null</code>.
	* Will also return <code>null</code> if the view descriptor exists, but
	* is filtered by an expression-based activity.
	* 
	* @param id the id to search for
	* @return the descriptor or <code>null</code>
	*/
	virtual IViewDescriptor* find(QString id)=0;

	/**
	* Returns an array of view categories.
	* 
	* @return the categories.  Never <code>null</code>.
	*/
	virtual IViewCategory* getCategories()=0;

	/**
	* Return a list of views defined in the registry.
	* 
	* @return the views.  Never <code>null</code>.
	*/
	virtual IViewDescriptor* getViews()=0;

	/**
	* Return a list of sticky views defined in the registry.
	* 
	* @return the sticky views.  Never <code>null</code>.
	*/
	virtual IStickyViewDescriptor* getStickyViews()=0;
};
#endif