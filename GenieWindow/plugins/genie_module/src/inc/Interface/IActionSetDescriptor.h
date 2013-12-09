#ifndef IACTIONSETDESCRIPTOR_H
#define IACTIONSETDESCRIPTOR_H

/**
 * A named set of actions which is defined as an extension to the workbench
 * via the standard workbench actions extension point
 * (<code>"org.eclipse.ui.workbenchActions"</code>). 
 * <p>
 * [Issue: This interface is not exposed in API, but time may
 * demonstrate that it should be.  For the short term leave it be.
 * In the long term its use should be re-evaluated. ]
 * </p>
 * <p>
 * This interface is not intended to be implemented by clients.
 * </p>
 * @see ActionSetRegistry
 */
#include <QString>
class IActionSet;

class IActionSetDescriptor {
    
    /**
     * Creates a new action set from this descriptor.
     * <p>
     * [Issue: Consider throwing WorkbenchException rather than CoreException.]
     * </p>
     *
     * @return the new action set
     * @exception CoreException if the action set cannot be created
     */
public:
	virtual IActionSet* createActionSet()=0;

    /**
     * Returns the description of this action set.
     * This is the value of its <code>"description"</code> attribute.
     *
     * @return the description
     */
   virtual QString getDescription()=0;

    /**
     * Returns the id of this action set. 
     * This is the value of its <code>"id"</code> attribute.
     *
     * @return the action set id
     */
     virtual QString getId()=0;

    /**
     * Returns the label of this action set. 
     * This is the value of its <code>"label"</code> attribute.
     *
     * @return the label
     */
     virtual QString getLabel()=0;

    /**
     * Returns whether this action set is initially visible.
     * 
     * @return whether this action set is initially visible
     */
     virtual bool isInitiallyVisible()=0;

    /**
     * Sets whether this action set is initially visible.
     * 
     * @param visible whether the action set should be visible initially.
     * @since 3.0
     */
     virtual void setInitiallyVisible(bool visible)=0;
    
    /**
     * Returns the conconfigurationfig element.
     * 
     * @return the configuration element
     * @since 3.1
     */
    virtual  /*IConfigurationElement*/void* getConfigurationElement()=0;
};
#endif