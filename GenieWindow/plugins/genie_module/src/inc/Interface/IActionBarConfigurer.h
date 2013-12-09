#ifndef IACTIONBARCONFIGURER
#define IACTIONBARCONFIGURER
/**
 * Interface providing special access for configuring the action bars
 * of a workbench window.
 * <p>
 * Note that these objects are only available to the main application
 * (the plug-in that creates and owns the workbench).
 * </p>
 * <p>
 * This interface is not intended to be implemented by clients.
 * </p>
 * 
 * @see org.eclipse.ui.application.WorkbenchAdvisor#fillActionBars
 * @since 3.0
 * @noimplement This interface is not intended to be implemented by clients.
 */

class IWorkbenchWindowConfigurer;
class IMenuManager;
class IStatusLineManager;
class ICoolBarManager;
class IAction;

class IActionBarConfigurer {
public:
    /**
     * Returns the workbench window configurer for the window
     * containing this configurer's action bars. 
     * 
     * @return the workbench window configurer
     * @since 3.1
     */
    IWorkbenchWindowConfigurer* getWindowConfigurer();
    
    
    /**
     * Returns the menu manager for the main menu bar of a workbench window.
     * 
     * @return the menu manager
     */
    IMenuManager* getMenuManager();

    /**
     * Returns the status line manager of a workbench window.
     * 
     * @return the status line manager
     */
    IStatusLineManager* getStatusLineManager();

    /**
     * Returns the cool bar manager of the workbench window.
     * 
     * @return the cool bar manager
     */
    ICoolBarManager* getCoolBarManager();
    
    /**
     * Register the action as a global action with a workbench
     * window.
     * <p>
     * For a workbench retarget action 
     * ({@link org.eclipse.ui.actions.RetargetAction RetargetAction})
     * to work, it must be registered.
     * You should also register actions that will participate
     * in custom key bindings.
     * </p>
     *  
     * @param action the global action
     * @see org.eclipse.ui.actions.RetargetAction
     */
    void registerGlobalAction(IAction* action);

};


#endif 
