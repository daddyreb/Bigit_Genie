#ifndef IEDITORDESCRIPTOR_H
#define IEDITORDESCRIPTOR_H
#include <QString>
class IEditorMatchingStrategy;

class IEditorDescriptor
{
virtual:
	/**
	* Returns the editor id.
	* <p>
	* For internal editors, this is the extension id as defined in the workbench
	* registry; for external editors, it is path and file name of the external 
	* program.
	* </p>
	*
	* @return the id of the editor
	*/
	virtual QString getId()=0;

	/**
	* Returns the label to show for this editor.
	*
	* @return the editor label
	*/
	virtual QString getLabel()=0;

	/**
	* Returns whether this editor descriptor will open a regular editor
	* part inside the editor area.
	* 
	* @return <code>true</code> if editor is inside editor area, and
	* <code>false</code> otherwise
	* @since 3.0
	*/
	virtual bool isInternal()=0;

	/**
	* Returns whether this editor descriptor will open an external
	* editor in-place inside the editor area.
	* 
	* @return <code>true</code> if editor is in-place, and <code>false</code>
	* otherwise
	* @since 3.0
	*/
	virtual bool isOpenInPlace()=0;

	/**
	* Returns whether this editor descriptor will open an external editor
	* in a new window outside the workbench.
	* 
	* @return <code>true</code> if editor is external, and <code>false</code>
	* otherwise
	* @since 3.0
	*/
	virtual bool isOpenExternal()=0;

	/**
	* Returns the editor matching strategy object for editors
	* represented by this editor descriptor, or <code>null</code>
	* if there is no explicit matching strategy specified.
	* 
	* @return the editor matching strategy, or <code>null</code> if none
	* @since 3.1
	*/
	virtual IEditorMatchingStrategy* getEditorMatchingStrategy()=0;
};
#endif