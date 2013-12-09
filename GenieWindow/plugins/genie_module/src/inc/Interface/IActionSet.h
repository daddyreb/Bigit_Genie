#ifndef IACTIONSET_H
#define IACTIONSET_H

/**
 * An action set is responsible for the creation of actions.
 * The end user may add these actions to a workbench page menu and tool bar
 * if they are deemed useful to the particular task at hand.
 * <p>
 * [Issue: This interface is not exposed in API, but time may
 * demonstrate that it should be.  For the short term leave it be.
 * In the long term its use should be re-evaluated. ]
 * </p>
 * <p>
 * In the current workbench implementation the desktop provides the
 * only implementation of this class in PluginActionSet.  So, it may
 * be useful to phase this interface out at some point.  PluginActionSet
 * provides a lazy load strategy, where the actions are declared in
 * XML and represented at runtime by a PluginAction.  
 * </p>
 */
class IWorkbenchWindow;
class IActionBars;

class IActionSet {
    /**
     * Disposes of this action set.
     * <p>
     * Implementation should remove any references to the window and action bars 
     * created in the <code>init</code>.
     * </p>
     * <p>
     * [Issue: Should this say: "...should remove anything they contributed
     *  in <code>init</code>? Or is most of the withdrawal done automatically?
     * ]
     * </p>
     */
    virtual void dispose()=0;

    /**
     * Initializes this action set, which is expected to add it actions as required
     * to the given workbench window and action bars.
     *
     * @param window the workbench window
     * @param bars the action bars
     */
    virtual void init(IWorkbenchWindow* window, IActionBars* bars)=0;
};
#endif