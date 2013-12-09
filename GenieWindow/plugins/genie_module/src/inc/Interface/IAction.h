#ifndef IACTION_H
#define IACTION_H
#include <QString>

class IMenuCreator;
class IPropertyChangeListener;

class IAction
{
public:
	/**
	* Adds a property change listener to this action.
	* Has no effect if an identical listener is already registered.
	*
	* @param listener a property change listener
	*/
	virtual void addPropertyChangeListener(IPropertyChangeListener* listener)=0;

	/**
	* Returns the accelerator keycode for this action.
	* The result is the bit-wise OR of zero or more modifier masks
	* and a key, as explained in <code>MenuItem.getAccelerator</code>.
	*
	* @return the accelerator keycode
	* @see org.eclipse.swt.widgets.MenuItem#getAccelerator()
	*/
	virtual int getAccelerator()=0;

	/**
	* Returns the action definition id of this action.
	* 
	* @return the action definition id of this action, or
	* <code>null</code> if none
	* @since 2.0
	*/
	virtual QString getActionDefinitionId()=0;

	/**
	* Returns the action's description if it has one.
	* Otherwise it returns <code>getToolTipText()</code>.
	* 
	* @return a description for the action; may be <code>null</code> 
	*/
	virtual QString getDescription()=0;

	/**
	* Returns a unique identifier for this action, or <code>null</code> if it has
	* none.
	*
	* @return the action id, or <code>null</code> if none
	*/
	virtual QString getId()=0;


	/**
	* Returns the menu creator for this action.
	*
	* @return the menu creator, or <code>null</code> if none
	*/
	virtual IMenuCreator* getMenuCreator()=0;

	/**
	* Return this action's style.
	*
	* @return one of <code>AS_PUSH_BUTTON</code>, <code>AS_CHECK_BOX</code>,
	* <code>AS_RADIO_BUTTON</code> and <code>AS_DROP_DOWN_MENU</code>.
	*/
	virtual int getStyle()=0;

	/**
	* Returns the text for this action.
	* <p>
	* This method is associated with the <code>TEXT</code> property;
	* property change events are reported when its value changes.
	* </p>
	*
	* @return the text, or <code>null</code> if none
	* @see #TEXT
	*/
	virtual QString getText()=0;

	/**
	* Returns the tool tip text for this action.
	* <p>
	* This method is associated with the <code>TOOL_TIP_TEXT</code> property;
	* property change events are reported when its value changes.
	* </p>
	*
	* @return the tool tip text, or <code>null</code> if none
	* @see #TOOL_TIP_TEXT
	*/
	virtual QString getToolTipText()=0;

	/**
	* Returns the checked status of this action. Applicable only if the style is
	* <code>AS_CHECK_BOX</code> or <code>AS_RADIO_BUTTON</code>.
	* <p>
	* This method is associated with the <code>CHECKED</code> property;
	* property change events are reported when its value changes.
	* </p>
	*
	* @return the checked status
	* @see #CHECKED
	*/
	virtual bool isChecked()=0;

	/**
	* Returns whether this action is enabled.
	* <p>
	* This method is associated with the <code>ENABLED</code> property;
	* property change events are reported when its value changes.
	* </p>
	*
	* @return <code>true</code> if enabled, and
	*   <code>false</code> if disabled
	* @see #ENABLED
	*/
	virtual bool isEnabled()=0;

	/**
	* Returns whether this action is handled. In the default case, this is
	* always <code>true</code>. However, if the action delegates some of its
	* behaviour to some other object, then this method should answer whether
	* such an object is currently available.
	* 
	* @return <code>true</code> if all of the action's behaviour is
	*         available; <code>false</code> otherwise.
	* @since 3.1
	*/
	virtual bool isHandled()=0;

	/**
	* Removes the given listener from this action.
	* Has no effect if an identical listener is not registered.
	*
	* @param listener a property change listener
	*/
	virtual void removePropertyChangeListener(IPropertyChangeListener* listener)=0;

	/**
	* Runs this action.
	* Each action implementation must define the steps needed to carry out this action.
	* The default implementation of this method in <code>Action</code>
	* does nothing.
	*/
	virtual void run()=0;


	/**
	* Sets the action definition id of this action.
	* 
	* @param id the action definition id
	* @since 2.0
	*/
	virtual void setActionDefinitionId(QString id)=0;

	/**
	* Sets the checked status of this action. Applicable for the styles
	* <code>AS_CHECK_BOX</code> or <code>AS_RADIO_BUTTON</code>.
	* <p>
	* Fires a property change event for the <code>CHECKED</code> property
	* if the checked status actually changes as a consequence.
	* </p>
	*
	* @param checked the new checked status
	* @see #CHECKED
	*/
	virtual void setChecked(bool checked)=0;

	/**
	* Sets this action's description.
	* Typically the description is shown as a (longer) help text in the status line.
	* <p>
	* Fires a property change event for the <code>DESCRIPTION</code> property
	* if the description actually changes as a consequence.
	* </p>
	*
	* @param text the description, or <code>null</code> to clear the description
	* @see #DESCRIPTION
	*/
	virtual void setDescription(QString text)=0;

	/**
	* Sets the enabled state of this action.
	* <p>
	* When an action is in the enabled state, the control associated with 
	* it is active; triggering it will end up inkoking this action's 
	* <code>run</code> method.
	* </p>
	* <p>
	* Fires a property change event for the <code>ENABLED</code> property
	* if the enabled state actually changes as a consequence.
	* </p>
	*
	* @param enabled <code>true</code> to enable, and
	*   <code>false</code> to disable
	* @see #ENABLED
	*/
	virtual void setEnabled(bool enabled)=0;

	/**
	* Sets the unique identifier for this action. This is used to identify actions
	* when added to a contribution manager.
	* It should be set when the action is created.  It should not be modified once
	* the action is part of an action contribution item.
	*
	* @param id the action id
	*
	* @see ActionContributionItem
	* @see IContributionItem#getId
	*/
	virtual void setId(QString id)=0;


	/**
	* Sets the menu creator for this action. Applicable for style
	* <code>AS_DROP_DOWN_MENU</code>.
	*
	* @param creator the menu creator, or <code>null</code> if none
	*/
	virtual void setMenuCreator(IMenuCreator* creator)=0;

	/**
	* Sets the text for this action.
	* <p>
	* An accelerator specification may follow the actual text, separated from it by
	* an '@' or a '\t' character.  An accelerator specification consists of zero or more 
	* modifier tokens followed by a key code token.  The tokens are separated by a '+' character.
	* </p>
	* <p>
	* Fires a property change event for the <code>TEXT</code> property
	* if the text actually changes as a consequence.
	* </p>
	*
	* @param text the text, or <code>null</code> if none
	* @see #TEXT
	* @see Action#findModifier
	* @see Action#findKeyCode
	*/
	virtual void setText(QString text)=0;

	/**
	* Sets the tool tip text for this action.
	* <p>
	* Fires a property change event for the <code>TOOL_TIP_TEXT</code> property
	* if the tool tip text actually changes as a consequence.
	* </p>
	*
	* @param text the tool tip text, or <code>null</code> if none
	* @see #TOOL_TIP_TEXT
	*/
	virtual void setToolTipText(QString text)=0;

	/**
	* <p>
	* Sets the accelerator keycode that this action maps to. This is a bitwise OR
	* of zero or more SWT key modifier masks (i.e. SWT.CTRL or SWT.ALT) and a
	* character code. For example, for Ctrl+Z, use <code>SWT.CTRL | 'Z'</code>.
	* Use 0 for no accelerator.
	* </p>
	* <p>
	* This method should no longer be used for actions in the Eclipse workbench.
	* <code>IWorkbenchCommandSupport</code> and
	* <code>IWorkbenchContextSupport</code> provide all the functionality
	* required for key bindings. If you set an accelerator using this method, then
	* it will not work in the workbench if it conflicts any existing key binding,
	* or if there is a different key binding defined for this action's definition
	* id. The definition id should be used instead -- referring to the command in
	* the workbench from which the key binding should be retrieved.
	* </p>
	* 
	* @param keycode
	*            the keycode to be accepted.
	*/
	virtual void setAccelerator(int keycode)=0;
};
#endif