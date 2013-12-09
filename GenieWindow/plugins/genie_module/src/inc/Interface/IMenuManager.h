#ifndef IMENUMANAGER_H
#define IMENUMANAGER_H
#include <QString>

class IMenuManager
{
public:

	/**
	* Finds the manager for the menu at the given path. A path
	* consists of contribution item ids separated by the separator 
	* character.  The path separator character is <code>'/'</code>.
	* <p>
	* Convenience for <code>findUsingPath(path)</code> which
	* extracts an <code>IMenuManager</code> if possible.
	* </p>
	*
	* @param path the path string
	* @return the menu contribution item, or <code>null</code>
	*   if there is no such contribution item or if the item does
	*   not have an associated menu manager
	*/
	virtual IMenuManager* findMenuUsingPath(QString path)=0;


	/**
	* Returns whether all items should be removed when the menu is about to
	* show, but before notifying menu listeners. The default is
	* <code>false</code>.
	* 
	* @return <code>true</code> if all items should be removed when shown,
	*         <code>false</code> if not
	*/
	virtual bool getRemoveAllWhenShown()=0;

	/**
	* Returns whether this menu should be enabled or not.
	*
	* @return <code>true</code> if enabled, and
	*   <code>false</code> if disabled
	*/
	virtual bool isEnabled()=0;



	/**
	* Sets whether all items should be removed when the menu is about to show,
	* but before notifying menu listeners.
	* 
	* @param removeAll
	*            <code>true</code> if all items should be removed when shown,
	*            <code>false</code> if not
	*/
	virtual void setRemoveAllWhenShown(bool removeAll)=0;

	/**
	* Incrementally builds the menu from the contribution items, and
	* does so recursively for all submenus.
	*
	* @param force <code>true</code> means update even if not dirty,
	*   and <code>false</code> for normal incremental updating
	*/
	virtual void updateAll(bool force)=0;
};
#endif