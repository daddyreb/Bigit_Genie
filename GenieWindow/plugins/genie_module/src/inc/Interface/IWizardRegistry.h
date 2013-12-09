#ifndef IWIZARDREGISTRY_H
#define IWIZARDREGISTRY_H
#include <QString>
class IWizardCategory;
class IWizardDescriptor;

class IWizardRegistry
{
public:
	/**
	* Find a wizard with the given id.
	* 
	* @param id the id to search for
	* @return the wizard descriptor matching the given id or <code>null</code>
	*/
	virtual IWizardDescriptor* findWizard(QString id)=0;

	/**
	* Return the wizards that have been designated as "primary".
	* 
	* @return the primary wizard descriptors.  Never <code>null</code>.
	*/
	virtual IWizardDescriptor* getPrimaryWizards()=0;

	/**
	* Find the category with the given id.
	* 
	* @param id the id of the category to search for
	* @return the category matching the given id or <code>null</code>
	*/
	virtual IWizardCategory* findCategory(QString id)=0;

	/**
	* Return the root category.
	* 
	* @return the root category.  Never <code>null</code>.
	*/
	virtual IWizardCategory* getRootCategory()=0;
};
#endif