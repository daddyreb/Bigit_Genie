#ifndef IWIZARDCATEGORY_H
#define IWIZARDCATEGORY_H
class IWizardDescriptor;
#include <QString>

class IWizardCategory
{
public:

	/**
	* Find a wizard that has the provided id. This will search recursivly over
	* this categories children.
	* 
	* @param id
	*            the id to search for
	* @return the wizard or <code>null</code>
	*/
	virtual IWizardDescriptor* findWizard(QString id)=0;

	/**
	* Return the immediate child categories.
	* 
	* @return the child categories. Never <code>null</code>.
	*/
	virtual IWizardCategory* getCategories()=0;

	/**
	* Return the identifier of this category.
	* 
	* @return the identifier of this category
	*/
	virtual QString getId()=0;

	/**
	* Return the label for this category.
	* 
	* @return the label for this category
	*/
	virtual QString getLabel()=0;

	/**
	* Return the parent category.
	* 
	* @return the parent category. May be <code>null</code>.
	*/
	IWizardCategory* getParent()=0;


	/**
	* Return the wizards in this category, minus the wizards which failed
	* the Expressions check.
	* 
	* @return the wizards in this category. Never <code>null</code>
	*/
	IWizardDescriptor* getWizards()=0;
};
#endif