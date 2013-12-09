#ifndef IWIZARDPAGE_H
#define IWIZARDPAGE_H
#include <QString>
#include "IDialogPage.h"
class IWizard;
class IWizardPage:public IDialogPage
{
public:
	/**
	* Returns whether the next page could be displayed.
	*
	* @return <code>true</code> if the next page could be displayed,
	*   and <code>false</code> otherwise
	*/
	virtual bool canFlipToNextPage()=0;

	/**
	* Returns this page's name.
	*
	* @return the name of this page
	*/
	virtual QString getName()=0;

	/**
	* Returns the wizard page that would to be shown if the user was to
	* press the Next button.
	*
	* @return the next wizard page, or <code>null</code> if none
	*/
	virtual IWizardPage* getNextPage()=0;

	/**
	* Returns the wizard page that would to be shown if the user was to
	* press the Back button.
	*
	* @return the previous wizard page, or <code>null</code> if none
	*/
	virtual IWizardPage* getPreviousPage()=0;

	/**
	* Returns the wizard that hosts this wizard page.
	*
	* @return the wizard, or <code>null</code> if this page has not been
	*   added to any wizard
	* @see #setWizard
	*/
	virtual IWizard* getWizard()=0;

	/**
	* Returns whether this page is complete or not.
	* <p>
	* This information is typically used by the wizard to decide
	* when it is okay to finish.
	* </p>
	*
	* @return <code>true</code> if this page is complete, and
	*  <code>false</code> otherwise
	*/
	virtual bool isPageComplete()=0;

	/**
	* Sets the wizard page that would typically be shown 
	* if the user was to press the Back button.
	* <p>
	* This method is called by the container.
	* </p>
	*
	* @param page the previous wizard page
	*/
	virtual void setPreviousPage(IWizardPage* page)=0;

	/**
	* Sets the wizard that hosts this wizard page.
	* Once established, a page's wizard cannot be changed
	* to a different wizard.
	*
	* @param newWizard the wizard
	* @see #getWizard
	*/
	virtual void setWizard(IWizard* newWizard)=0;
};
#endif