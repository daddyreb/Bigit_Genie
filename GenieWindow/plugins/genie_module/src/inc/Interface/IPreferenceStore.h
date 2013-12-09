#ifndef IPREFERENCESTORE_H
#define IPREFERENCESTORE_H

#include <QString>
//#include <QObject>
//class IPropertyChangeListener;

class IPreferenceStore
{
public:
	/**
	* <p>
	* Adds a property change listener to this preference store.
	* </p>
	* <p>
	* <b>Note</b> The types of the oldValue and newValue of the
	* generated PropertyChangeEvent are determined by whether
	* or not the typed API in IPreferenceStore was called.
	* If values are changed via setValue(name,type) the 
	* values in the PropertyChangedEvent will be of that type.
	* If they are set using a non typed API (i.e. #setToDefault
	* or using the OSGI Preferences) the values will be unconverted
	* Strings.
	* </p>
	* <p>
	* A listener will be called in the same Thread
	* that it is invoked in. Any Thread dependant listeners (such as 
	* those who update an SWT widget) will need to update in the
	* correct Thread. In the case of an SWT update you can update
	* using Display#syncExec(Runnable) or Display#asyncExec(Runnable).
	* </p>
	* <p>  
	* Likewise any application that updates an IPreferenceStore 
	* from a Thread other than the UI Thread should be aware of
	* any listeners that require an update in the UI Thread. 
	* </p>
	*
	* @param listener a property change listener
	* @see org.eclipse.jface.util.PropertyChangeEvent
	* @see #setToDefault(QString)
	* @see #setValue(QString, bool)
	* @see #setValue(QString, double)
	* @see #setValue(QString, float)
	* @see #setValue(QString, int)
	* @see #setValue(QString, long)
	* @see #setValue(QString, QString)
	*/
	virtual void addPropertyChangeListener(/*IPropertyChangeListener*/void* listener)=0;

	/**
	* Returns whether the named preference is known to this preference
	* store.
	*
	* @param name the name of the preference
	* @return <code>true</code> if either a current value or a default
	*  value is known for the named preference, and <code>false</code> otherwise
	*/
	virtual bool contains(QString name)=0;

	/**
	* Fires a property change event corresponding to a change to the
	* current value of the preference with the given name.
	* <p>
	* This method is provided on this interface to simplify the implementation 
	* of decorators. There is normally no need to call this method since
	* <code>setValue</code> and <code>setToDefault</code> report such
	* events in due course. Implementations should funnel all preference
	* changes through this method.
	* </p>
	*
	* @param name the name of the preference, to be used as the property
	*  in the event object
	* @param oldValue the old value
	* @param newValue the new value
	*/
	virtual void firePropertyChangeEvent(QString name, void* oldValue,
		void* newValue)=0;

	/**
	* Returns the current value of the bool-valued preference with the
	* given name.
	* Returns the default-default value (<code>false</code>) if there
	* is no preference with the given name, or if the current value 
	* cannot be treated as a bool.
	*
	* @param name the name of the preference
	* @return the bool-valued preference
	*/
	virtual bool getBoolean(QString name)=0;

	/**
	* Returns the default value for the bool-valued preference
	* with the given name.
	* Returns the default-default value (<code>false</code>) if there
	* is no default preference with the given name, or if the default 
	* value cannot be treated as a bool.
	*
	* @param name the name of the preference
	* @return the default value of the named preference
	*/
	virtual bool getDefaultBoolean(QString name)=0;

	/**
	* Returns the default value for the double-valued preference
	* with the given name.
	* Returns the default-default value (<code>0.0</code>) if there
	* is no default preference with the given name, or if the default 
	* value cannot be treated as a double.
	*
	* @param name the name of the preference
	* @return the default value of the named preference
	*/
	virtual double getDefaultDouble(QString name)=0;

	/**
	* Returns the default value for the float-valued preference
	* with the given name.
	* Returns the default-default value (<code>0.0f</code>) if there
	* is no default preference with the given name, or if the default 
	* value cannot be treated as a float.
	*
	* @param name the name of the preference
	* @return the default value of the named preference
	*/
	virtual float getDefaultFloat(QString name)=0;

	/**
	* Returns the default value for the integer-valued preference
	* with the given name.
	* Returns the default-default value (<code>0</code>) if there
	* is no default preference with the given name, or if the default 
	* value cannot be treated as an integer.
	*
	* @param name the name of the preference
	* @return the default value of the named preference
	*/
	virtual int getDefaultInt(QString name)=0;

	/**
	* Returns the default value for the long-valued preference
	* with the given name.
	* Returns the default-default value (<code>0L</code>) if there
	* is no default preference with the given name, or if the default 
	* value cannot be treated as a long.
	*
	* @param name the name of the preference
	* @return the default value of the named preference
	*/
	virtual long getDefaultLong(QString name)=0;

	/**
	* Returns the default value for the string-valued preference
	* with the given name.
	* Returns the default-default value (the empty string <code>""</code>) 
	* is no default preference with the given name, or if the default 
	* value cannot be treated as a string.
	*
	* @param name the name of the preference
	* @return the default value of the named preference
	*/
	virtual QString getDefaultString(QString name)=0;

	/**
	* Returns the current value of the double-valued preference with the
	* given name.
	* Returns the default-default value (<code>0.0</code>) if there
	* is no preference with the given name, or if the current value 
	* cannot be treated as a double.
	*
	* @param name the name of the preference
	* @return the double-valued preference
	*/
	virtual double getDouble(QString name)=0;

	/**
	* Returns the current value of the float-valued preference with the
	* given name.
	* Returns the default-default value (<code>0.0f</code>) if there
	* is no preference with the given name, or if the current value 
	* cannot be treated as a float.
	*
	* @param name the name of the preference
	* @return the float-valued preference
	*/
	virtual float getFloat(QString name)=0;

	/**
	* Returns the current value of the integer-valued preference with the
	* given name.
	* Returns the default-default value (<code>0</code>) if there
	* is no preference with the given name, or if the current value 
	* cannot be treated as an integter.
	*
	* @param name the name of the preference
	* @return the int-valued preference
	*/
	virtual int getInt(QString name)=0;

	/**
	* Returns the current value of the long-valued preference with the
	* given name.
	* Returns the default-default value (<code>0L</code>) if there
	* is no preference with the given name, or if the current value 
	* cannot be treated as a long.
	*
	* @param name the name of the preference
	* @return the long-valued preference
	*/
	virtual long getLong(QString name)=0;

	/**
	* Returns the current value of the string-valued preference with the
	* given name.
	* Returns the default-default value (the empty string <code>""</code>)
	* if there is no preference with the given name, or if the current value 
	* cannot be treated as a string.
	*
	* @param name the name of the preference
	* @return the string-valued preference
	*/
	virtual QString getString(QString name)=0;

	/**
	* Returns whether the current value of the preference with the given name
	* has the default value.
	*
	* @param name the name of the preference
	* @return <code>true</code> if the preference has a known default value
	* and its current value is the same, and <code>false</code> otherwise
	* (including the case where the preference is unknown to this store)
	*/
	virtual bool isDefault(QString name)=0;

	/**
	* Returns whether the current values in this property store
	* require saving.
	*
	* @return <code>true</code> if at least one of values of 
	*  the preferences known to this store has changed and 
	*  requires saving, and <code>false</code> otherwise.
	*/
	virtual bool needsSaving()=0;

	/**
	* Sets the current value of the preference with the given name to
	* the given string value without sending a property change.
	* <p>
	* This method does not fire a property change event and 
	* should only be used for setting internal preferences 
	* that are not meant to be processed by listeners.
	* Normal clients should instead call #setValue.
	* </p>
	*
	* @param name the name of the preference
	* @param value the new current value of the preference
	*/
	virtual void putValue(QString name, QString value)=0;

	/**
	* Removes the given listener from this preference store.
	* Has no affect if the listener is not registered.
	*
	* @param listener a property change listener
	*/
	virtual void removePropertyChangeListener(/*IPropertyChangeListener*/void* listener)=0;

	/**
	* Sets the default value for the double-valued preference with the
	* given name. 
	* <p>
	* Note that the current value of the preference is affected if
	* the preference's current value was its old default value, in which
	* case it changes to the new default value. If the preference's current
	* is different from its old default value, its current value is
	* unaffected. No property change events are reported by changing default
	* values.
	* </p>
	*
	* @param name the name of the preference
	* @param value the new default value for the preference
	*/
	virtual void setDefault(QString name, double value)=0;

	/**
	* Sets the default value for the float-valued preference with the
	* given name. 
	* <p>
	* Note that the current value of the preference is affected if
	* the preference's current value was its old default value, in which
	* case it changes to the new default value. If the preference's current
	* is different from its old default value, its current value is
	* unaffected. No property change events are reported by changing default
	* values.
	* </p>
	*
	* @param name the name of the preference
	* @param value the new default value for the preference
	*/
	virtual void setDefault(QString name, float value)=0;

	/**
	* Sets the default value for the integer-valued preference with the
	* given name. 
	* <p>
	* Note that the current value of the preference is affected if
	* the preference's current value was its old default value, in which
	* case it changes to the new default value. If the preference's current
	* is different from its old default value, its current value is
	* unaffected. No property change events are reported by changing default
	* values.
	* </p>
	*
	* @param name the name of the preference
	* @param value the new default value for the preference
	*/
	virtual void setDefault(QString name, int value)=0;

	/**
	* Sets the default value for the long-valued preference with the
	* given name. 
	* <p>
	* Note that the current value of the preference is affected if
	* the preference's current value was its old default value, in which
	* case it changes to the new default value. If the preference's current
	* is different from its old default value, its current value is
	* unaffected. No property change events are reported by changing default
	* values.
	* </p>
	*
	* @param name the name of the preference
	* @param value the new default value for the preference
	*/
	virtual void setDefault(QString name, long value)=0;

	/**
	* Sets the default value for the string-valued preference with the
	* given name. 
	* <p>
	* Note that the current value of the preference is affected if
	* the preference's current value was its old default value, in which
	* case it changes to the new default value. If the preference's current
	* is different from its old default value, its current value is
	* unaffected. No property change events are reported by changing default
	* values.
	* </p>
	*
	* @param name the name of the preference
	* @param defaultObject the new default value for the preference
	*/
	virtual void setDefault(QString name, QString defaultObject)=0;

	/**
	* Sets the default value for the bool-valued preference with the
	* given name. 
	* <p>
	* Note that the current value of the preference is affected if
	* the preference's current value was its old default value, in which
	* case it changes to the new default value. If the preference's current
	* is different from its old default value, its current value is
	* unaffected. No property change events are reported by changing default
	* values.
	* </p>
	*
	* @param name the name of the preference
	* @param value the new default value for the preference
	*/
	virtual void setDefault(QString name, bool Value)=0;

	/**
	* Sets the current value of the preference with the given name back
	* to its default value.
	* <p>
	* Note that the preferred way of re-initializing a preference to the
	* appropriate default value is to call <code>setToDefault</code>.
	* This is implemented by removing the named value from the store, 
	* thereby exposing the default value.
	* </p>
	*
	* @param name the name of the preference
	*/
	virtual void setToDefault(QString name)=0;

	/**
	* Sets the current value of the double-valued preference with the
	* given name.
	* <p>
	* A property change event is reported if the current value of the 
	* preference actually changes from its previous value. In the event
	* object, the property name is the name of the preference, and the
	* old and new values are wrapped as objects.
	* </p>
	* <p>
	* Note that the preferred way of re-initializing a preference to its
	* default value is to call <code>setToDefault</code>.
	* </p>
	*
	* @param name the name of the preference
	* @param value the new current value of the preference
	*/
	virtual void setValue(QString name, double value)=0;

	/**
	* Sets the current value of the float-valued preference with the
	* given name.
	* <p>
	* A property change event is reported if the current value of the 
	* preference actually changes from its previous value. In the event
	* object, the property name is the name of the preference, and the
	* old and new values are wrapped as objects.
	* </p>
	* <p>
	* Note that the preferred way of re-initializing a preference to its
	* default value is to call <code>setToDefault</code>.
	* </p>
	*
	* @param name the name of the preference
	* @param value the new current value of the preference
	*/
	virtual void setValue(QString name, float value)=0;

	/**
	* Sets the current value of the integer-valued preference with the
	* given name.
	* <p>
	* A property change event is reported if the current value of the 
	* preference actually changes from its previous value. In the event
	* object, the property name is the name of the preference, and the
	* old and new values are wrapped as objects.
	* </p>
	* <p>
	* Note that the preferred way of re-initializing a preference to its
	* default value is to call <code>setToDefault</code>.
	* </p>
	*
	* @param name the name of the preference
	* @param value the new current value of the preference
	*/
	virtual void setValue(QString name, int value)=0;

	/**
	* Sets the current value of the long-valued preference with the
	* given name.
	* <p>
	* A property change event is reported if the current value of the 
	* preference actually changes from its previous value. In the event
	* object, the property name is the name of the preference, and the
	* old and new values are wrapped as objects.
	* </p>
	* <p>
	* Note that the preferred way of re-initializing a preference to its
	* default value is to call <code>setToDefault</code>.
	* </p>
	*
	* @param name the name of the preference
	* @param value the new current value of the preference
	*/
	virtual void setValue(QString name, long value)=0;

	/**
	* Sets the current value of the string-valued preference with the
	* given name.
	* <p>
	* A property change event is reported if the current value of the 
	* preference actually changes from its previous value. In the event
	* object, the property name is the name of the preference, and the
	* old and new values are wrapped as objects.
	* </p>
	* <p>
	* Note that the preferred way of re-initializing a preference to its
	* default value is to call <code>setToDefault</code>.
	* </p>
	*
	* @param name the name of the preference
	* @param value the new current value of the preference
	*/
	virtual void setValue(QString name, QString value)=0;

	/**
	* Sets the current value of the bool-valued preference with the
	* given name.
	* <p>
	* A property change event is reported if the current value of the 
	* preference actually changes from its previous value. In the event
	* object, the property name is the name of the preference, and the
	* old and new values are wrapped as objects.
	* </p>
	* <p>
	* Note that the preferred way of re-initializing a preference to its
	* default value is to call <code>setToDefault</code>.
	* </p>
	*
	* @param name the name of the preference
	* @param value the new current value of the preference
	*/
	virtual void setValue(QString name, bool value)=0;
};
#endif