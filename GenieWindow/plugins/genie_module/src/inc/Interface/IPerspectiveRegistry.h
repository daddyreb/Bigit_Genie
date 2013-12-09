#ifndef IPERSPECTIVEREGISTRY_H
#define IPERSPECTIVEREGISTRY_H
class IPerspectiveDescriptor;
#include <QString>

class IPerspectiveRegistry 
{
public:
	/**
	* Clones an existing perspective.
	* 
	* @param id the id for the cloned perspective, which must not already be used by 
	*   any registered perspective
	* @param label the label assigned to the cloned perspective
	* @param desc the perspective to clone
	* @return the cloned perspective descriptor
	* @throws IllegalArgumentException if there is already a perspective with the given id 
	* 
	* @since 3.0
	*/
	virtual IPerspectiveDescriptor* clonePerspective(QString id, QString label,
		IPerspectiveDescriptor* desc) =0;

	/**
	* Deletes a perspective. Has no effect if the perspective is defined in an
	* extension.
	* 
	* @param persp the perspective to delete
	* @since 3.2
	*/
	virtual void deletePerspective(IPerspectiveDescriptor* persp)=0;

	/**
	* Finds and returns the registered perspective with the given perspective id.
	*
	* @param perspectiveId the perspective id 
	* @return the perspective, or <code>null</code> if none
	* @see IPerspectiveDescriptor#getId
	*/
	virtual IPerspectiveDescriptor* findPerspectiveWithId(QString perspectiveId)=0;

	/**
	* Finds and returns the registered perspective with the given label.
	*
	* @param label the label
	* @return the perspective, or <code>null</code> if none
	* @see IPerspectiveDescriptor#getLabel
	*/
	virtual IPerspectiveDescriptor* findPerspectiveWithLabel(QString label)=0;

	/**
	* Returns the id of the default perspective for the workbench.  This identifies one
	* perspective extension within the workbench's perspective registry.
	* <p>
	* Returns <code>null</code> if there is no default perspective.
	* </p> 
	*
	* @return the default perspective id, or <code>null</code>
	*/
	virtual QString getDefaultPerspective()=0;

	/**
	* Returns a list of the perspectives known to the workbench.
	*
	* @return a list of perspectives
	*/
	virtual QVector<IPerspectiveDescriptor*> getPerspectives()=0;

	/**
	* Sets the default perspective for the workbench to the given perspective id.
	* If non-<code>null</code>, the id must correspond to a perspective extension 
	* within the workbench's perspective registry.
	* <p>
	* A <code>null</code> id indicates no default perspective.
	* </p>
	*
	* @param id a perspective id, or <code>null</code>
	*/
	virtual void setDefaultPerspective(QString id)=0;

	/**
	* Reverts a perspective back to its original definition
	* as specified in the plug-in manifest.
	* 
	* @param perspToRevert the perspective to revert  
	* 
	* @since 3.0
	*/
	virtual void revertPerspective(IPerspectiveDescriptor* perspToRevert)=0;
};
#endif