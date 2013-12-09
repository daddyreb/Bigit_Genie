#ifndef IWIZARD_H
#define IWIZARD_H
#include <QString>
#include <QImage>
class IWizardContainer;
class IWizardPage;

class IWizard
{
virtual:
	/**
	* Adds any last-minute pages to this wizard.
	* <p>
	* This method is called just before the wizard becomes visible, to give the 
	* wizard the opportunity to add any lazily created pages.
	* </p>
	*/
	virtual void addPages()=0;

	/**
	* Returns whether this wizard could be finished without further user
	* interaction.
	* <p>
	* The result of this method is typically used by the wizard container to enable
	* or disable the Finish button.
	* </p>
	*
	* @return <code>true</code> if the wizard could be finished,
	*   and <code>false</code> otherwise
	*/
	virtual bool canFinish()=0;

	/**
	* Disposes of this wizard and frees all SWT resources.
	*/
	virtual void dispose()=0;

	/**
	* Returns the container of this wizard.
	*
	* @return the wizard container, or <code>null</code> if this
	*   wizard has yet to be added to a container
	*/
	virtual IWizardContainer* getContainer()=0;

	/**
	* Returns the default page image for this wizard.
	* <p>
	* This image can be used for pages which do not
	* supply their own image.
	* </p>
	*
	* @return the default page image
	*/
	virtual QImage getDefaultPageImage()=0;


	/**
	* Returns the successor of the given page.
	* <p>
	* This method is typically called by a wizard page
	* </p>
	*
	* @param page the page
	* @return the next page, or <code>null</code> if none
	*/
	virtual IWizardPage* getNextPage(IWizardPage* page)=0;

	/**
	* Returns the wizard page with the given name belonging to this wizard.
	*
	* @param pageName the name of the wizard page
	* @return the wizard page with the given name, or <code>null</code> if none
	*/
	virtual IWizardPage* getPage(QString pageName)=0;

	/**
	* Returns the number of pages in this wizard.
	*
	* @return the number of wizard pages
	*/
	virtual int getPageCount()=0;

	/**
	* Returns all the pages in this wizard.
	*
	* @return a list of pages
	*/
	virtual IWizardPage* getPages()=0;

	/**
	* Returns the predecessor of the given page.
	* <p>
	* This method is typically called by a wizard page
	* </p>
	*
	* @param page the page
	* @return the previous page, or <code>null</code> if none
	*/
	virtual IWizardPage* getPreviousPage(IWizardPage* page)=0;

	/**
	* Returns the first page to be shown in this wizard.
	*
	* @return the first wizard page
	*/
	virtual IWizardPage* getStartingPage()=0;


	/**
	* Returns the window title string for this wizard.
	*
	* @return the window title string, or <code>null</code> for no title
	*/
	virtual QString getWindowTitle()=0;

	/**
	* Returns whether help is available for this wizard.
	* <p>
	* The result of this method is typically used by the container to
	* show or hide the Help button.
	* </p>
	*
	* @return <code>true</code> if help is available,
	*   and <code>false</code> if this wizard is helpless
	*/
	virtual bool isHelpAvailable()=0;

	/**
	* Returns whether this wizard needs Previous and Next buttons.
	* <p>
	* The result of this method is typically used by the container.
	* </p>
	*
	* @return <code>true</code> if Previous and Next buttons are required,
	*   and <code>false</code> if none are needed
	*/
	virtual bool needsPreviousAndNextButtons()=0;

	/**
	* Returns whether this wizard needs a progress monitor.
	* <p>
	* The result of this method is typically used by the container.
	* </p>
	*
	* @return <code>true</code> if a progress monitor is required,
	*   and <code>false</code> if none is needed
	*/
	virtual bool needsProgressMonitor()=0;

	/**
	* Performs any actions appropriate in response to the user 
	* having pressed the Cancel button, or refuse if canceling
	* now is not permitted.
	*
	* @return <code>true</code> to indicate the cancel request
	*   was accepted, and <code>false</code> to indicate
	*   that the cancel request was refused
	*/
	virtual bool performCancel()=0;

	/**
	* Performs any actions appropriate in response to the user 
	* having pressed the Finish button, or refuse if finishing
	* now is not permitted.
	*
	* Normally this method is only called on the container's
	* current wizard. However if the current wizard is a nested wizard
	* this method will also be called on all wizards in its parent chain.
	* Such parents may use this notification to save state etc. However,
	* the value the parents return from this method is ignored.
	*
	* @return <code>true</code> to indicate the finish request
	*   was accepted, and <code>false</code> to indicate
	*   that the finish request was refused
	*/
	virtual bool performFinish()=0;

	/**
	* Sets or clears the container of this wizard.
	*
	* @param wizardContainer the wizard container, or <code>null</code> 
	*/
	virtual void setContainer(IWizardContainer* wizardContainer)=0;
};
#endif