#ifndef IMEMENTO_H
#define IMEMENTO_H
#include <QString>

class IMemento
{
	/**
	* Creates a new child of this memento with the given type.
	* <p>
	* The <code>getChild</code> and <code>getChildren</code> methods
	* are used to retrieve children of a given type.
	* </p>
	*
	* @param type the type
	* @return a new child memento
	* @see #getChild
	* @see #getChildren
	*/
	virtual IMemento* createChild(QString type)=0;

	/**
	* Creates a new child of this memento with the given type and id.
	* The id is stored in the child memento (using a special reserved
	* key, <code>TAG_ID</code>) and can be retrieved using <code>getId</code>.
	* <p>
	* The <code>getChild</code> and <code>getChildren</code> methods
	* are used to retrieve children of a given type.
	* </p>
	*
	* @param type the type
	* @param id the child id
	* @return a new child memento with the given type and id
	* @see #getID
	*/
	virtual IMemento* createChild(QString type, QString id)=0;

	/**
	* Returns the first child with the given type id.
	*
	* @param type the type id
	* @return the first child with the given type
	*/
	virtual IMemento* getChild(QString type)=0;

	/**
	* Returns all children with the given type id.
	*
	* @param type the type id
	* @return an array of children with the given type
	*/
	virtual IMemento* getChildren(QString type)=0;

	/**
	* Returns the floating point value of the given key.
	*
	* @param key the key
	* @return the value, or <code>null</code> if the key was not found or was found
	*   but was not a floating point number
	*/
	virtual float getFloat(QString key)=0;

	/**
	* Returns the type for this memento.
	* 
	* @return the memento type
	* @see #createChild(java.lang.QString)
	* @see #createChild(java.lang.QString,java.lang.QString)
	* @since 3.4
	*/
	virtual QString getType()=0;

	/**
	* Returns the id for this memento.
	*
	* @return the memento id, or <code>null</code> if none
	* @see #createChild(java.lang.QString,java.lang.QString)
	*/
	virtual QString getID()=0;

	/**
	* Returns the integer value of the given key.
	*
	* @param key the key
	* @return the value, or <code>null</code> if the key was not found or was found
	*   but was not an integer
	*/
	virtual int getInteger(QString key)=0;

	/**
	* Returns the string value of the given key.
	*
	* @param key the key
	* @return the value, or <code>null</code> if the key was not found
	*/
	virtual QString getString(QString key)=0;

	/**
	* Returns the bool value of the given key.
	* 
	* @param key the key
	* @return the value, or <code>null</code> if the key was not found
	* @since 3.4
	*/
	virtual bool getBoolean(QString key)=0;

	/**
	* Returns the data of the Text node of the memento. Each memento is allowed
	* only one Text node.
	* 
	* @return the data of the Text node of the memento, or <code>null</code>
	* if the memento has no Text node.
	* @since 2.0
	*/
	virtual QString getTextData()=0;

	/**
	* Returns an array of all the attribute keys of the memento. This will not
	* be <code>null</code>. If there are no keys, an array of length zero will
	* be returned. 
	* @return an array with all the attribute keys of the memento
	* @since 3.4
	*/
	virtual QString* getAttributeKeys()=0;

	/**
	* Sets the value of the given key to the given floating point number.
	*
	* @param key the key
	* @param value the value
	*/
	virtual void putFloat(QString key, float value)=0;

	/**
	* Sets the value of the given key to the given integer.
	*
	* @param key the key
	* @param value the value
	*/
	virtual void putInteger(QString key, int value)=0;

	/**
	* Copy the attributes and children from  <code>memento</code>
	* to the receiver.
	*
	* @param memento the IMemento to be copied.
	*/
	virtual void putMemento(IMemento* memento)=0;

	/**
	* Sets the value of the given key to the given string.
	*
	* @param key the key
	* @param value the value
	*/
	virtual void putString(QString key, QString value)=0;

	/**
	* Sets the value of the given key to the given bool value.
	* 
	* @param key the key
	* @param value the value
	* @since 3.4
	*/
	virtual void putBoolean(QString key, bool value)=0;

	/**
	* Sets the memento's Text node to contain the given data. Creates the Text node if
	* none exists. If a Text node does exist, it's current contents are replaced. 
	* Each memento is allowed only one text node.
	* 
	* @param data the data to be placed on the Text node
	* @since 2.0
	*/
	virtual void putTextData(QString data)=0;
};
#endif