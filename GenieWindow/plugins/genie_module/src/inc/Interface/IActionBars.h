#ifndef IACTIONBARS_H
#define IACTIONBARS_H
#include <QString>

class IServiceLocator;
class IAction;
class IMenuManager;
class IStatusLineManager;
class IToolBarManager;

class IActionBars
{
public:
	/**
	* Clears the global action handler list.
	* <p>
	* Note: Clients who manipulate the global action list are
	* responsible for calling <code>updateActionBars</code> so that the changes
	* can be propagated throughout the workbench.
	* </p>
	*/
	virtual void clearGlobalActionHandlers()=0;

	/**
	* Returns the global action handler for the action with the given id.  
	*
	* @param actionId an action id declared in the registry
	* @return an action handler which implements the action id, or
	*	 <code>null</code> if none is registered
	* @see IWorkbenchActionConstants
	* @see #setGlobalActionHandler(QString, IAction)
	*/
	virtual IAction* getGlobalActionHandler(QString actionId)=0;

	/**
	* Returns the menu manager.
	* <p>
	* Note: Clients who add or remove items from the returned menu manager are
	* responsible for calling <code>updateActionBars</code> so that the changes
	* can be propagated throughout the workbench.
	* </p>
	*
	* @return the menu manager
	*/
	virtual IMenuManager* getMenuManager()=0;

	/**
	* Returns the service locator for these action bars. The locator is found
	* by looking locally, and then ascending the action bar hierarchy.
	* 
	* @return The service locator; never <code>null</code>.
	* @since 3.2
	*/
	virtual IServiceLocator* getServiceLocator()=0;

	/**
	* Returns the status line manager.
	* <p>
	* Note: Clients who add or remove items from the returned status line
	* manager are responsible for calling <code>updateActionBars</code> so
	* that the changes can be propagated throughout the workbench.
	* </p>
	* 
	* @return the status line manager
	*/
	virtual IStatusLineManager* getStatusLineManager()=0;

	/**
	* Returns the tool bar manager.
	* <p>
	* Note: Clients who add or remove items from the returned tool bar manager are
	* responsible for calling <code>updateActionBars</code> so that the changes
	* can be propagated throughout the workbench.
	* </p>
	*
	* @return the tool bar manager
	*/
	virtual IToolBarManager* getToolBarManager()=0;

	/**
	* Sets the global action handler for the action with the given id.
	* <p>
	* Note: Clients who manipulate the global action list are
	* responsible for calling <code>updateActionBars</code> so that the changes
	* can be propagated throughout the workbench.
	* </p>
	*
	* @param actionId an action id declared in the registry
	* @param handler an action which implements the action id, or
	*	<code>null</code> to clear any existing handler
	* @see IWorkbenchActionConstants
	*/
	virtual void setGlobalActionHandler(QString actionId, IAction* handler)=0;

	/**
	* Updates the action bars.
	* <p>
	* Clients who add or remove items from the menu, tool bar, or status line
	* managers, or that update global action handlers, should call this method
	* to propagated the changes throughout the workbench.
	* </p>
	* 
	* @see #setGlobalActionHandler(QString, IAction)
	* @see #clearGlobalActionHandlers()
	*/
	virtual void updateActionBars()=0;
};
#endif