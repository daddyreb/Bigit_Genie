#ifndef IVIEWDESCRIPTOR_H
#define IVIEWDESCRIPTOR_H
#include "IWorkbenchPartDescriptor.h"
#include "IAdaptable.h"
#include <QString>
class IViewPart;

class IViewDescriptor:IWorkbenchPartDescriptor, IAdaptable
{
public:
	/**
	* Creates an instance of the view defined in the descriptor.
	* 
	* @return the view part
	* @throws CoreException thrown if there is a problem creating the part
	*/
	virtual IViewPart* createView()=0;

	/**
	* Returns an array of strings that represent
	* view's category path. This array will be used
	* for hierarchical presentation of the
	* view in places like submenus.
	* @return array of category tokens or null if not specified.
	*/
	virtual QString* getCategoryPath()=0;

	/**
	* Returns the description of this view.
	*
	* @return the description
	*/
	virtual QString getDescription()=0;

	/**
	* Returns the id of the view.
	* 
	* @return the id
	*/
	virtual QString getId()=0;

	/**
	* Returns the label to show for this view.
	* 
	* @return the label
	*/
	virtual QString getLabel()=0;

	/**
	* Returns the default fast view width ratio for this view.
	* 
	* @return the fast view width ratio
	*/
	virtual float getFastViewWidthRatio()=0;

	/**
	* Returns whether this view allows multiple instances.
	* 
	* @return whether this view allows multiple instances
	*/
	virtual bool getAllowMultiple()=0;

	/**
	* Returns whether this view can be restored upon workbench restart.
	* 
	* @return whether whether this view can be restored upon workbench restart
	* @since 3.4
	*/
	virtual bool isRestorable()=0;

};
#endif