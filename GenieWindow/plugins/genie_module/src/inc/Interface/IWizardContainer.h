#ifndef IWIZARDCONTAINER_H
#define IWIZARDCONTAINER_H
#include "IRunnableContext.h"
class IWizardPage;

class IWizardContainer:public IRunnableContext
{
public:
	/**
	* Returns the current wizard page for this container.
	*
	* @return the current wizard page, or <code>null</code> if the container
	*   is not yet showing the wizard
	* @see #showPage
	*/
	virtual IWizardPage* getCurrentPage()=0;


	/**
	* Makes the given page visible.
	* <p>
	* This method should not be use for normal page
	* sequencing (back, next) which is handled by the 
	* container itself. It may, however, be used to
	* move to another page in response to some custom
	* action such as double clicking in a list.
	* </p>
	*
	* @param page the page to show
	* @see #getCurrentPage
	*/
	virtual void showPage(IWizardPage* page)=0;

	/**
	* Adjusts the enable state of the Back, Next, and Finish 
	* buttons to reflect the state of the currently active 
	* page in this container.
	* <p>
	* This method is called by the container itself
	* when its wizard page changes and may be called
	* by the page at other times to force a button state
	* update.
	* </p>
	*/
	virtual void updateButtons()=0;

	/**
	* Updates the message (or error message) shown in the message line to 
	* reflect the state of the currently active page in this container.
	* <p>
	* This method is called by the container itself
	* when its wizard page changes and may be called
	* by the page at other times to force a message 
	* update.
	* </p>
	*/
	virtual void updateMessage()=0;

	/**
	* Updates the title bar (title, description, and image) to 
	* reflect the state of the currently active page in this container.
	* <p>
	* This method is called by the container itself
	* when its wizard page changes and may be called
	* by the page at other times to force a title bar 
	* update.
	* </p>
	*/
	virtual void updateTitleBar()=0;

	/**
	* Updates the window title to reflect the state of the current wizard.
	* <p>
	* This method is called by the container itself
	* when its wizard changes and may be called
	* by the wizard at other times to force a window 
	* title change.
	* </p>
	*/
	virtual void updateWindowTitle()=0;
};
#endif