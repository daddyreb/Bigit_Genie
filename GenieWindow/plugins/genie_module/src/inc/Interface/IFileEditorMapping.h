#ifndef IFILEEDITORMAPPING_H
#define IFILEEDITORMAPPING_H

class IEditorDescriptor;


class IFileEditorMapping
{
virtual:
	/**
	* Returns the default editor registered for this type mapping.
	*
	* @return the descriptor of the default editor, or <code>null</code> if there
	*   is no default editor registered. Will also return <code>null</code> if
	*   the default editor exists but fails the Expressions check.
	*/
	virtual IEditorDescriptor* getDefaultEditor()=0;

	/**
	* Returns the list of editors registered for this type mapping.
	*
	* @return a possibly empty list of editors.
	*/
	virtual IEditorDescriptor* getEditors()=0;

	/**
	* Returns the list of editors formerly registered for this type mapping
	* which have since been deleted.
	*
	* @return a possibly empty list of editors
	*/
	virtual IEditorDescriptor* getDeletedEditors()=0;

	/**
	* Returns the file's extension for this type mapping.
	*
	* @return the extension for this mapping
	*/
	virtual QString getExtension()=0;


	/**
	* Returns the label to use for this mapping.
	* Labels have the form "<it>name</it>.<it>extension</it>".
	*
	* @return the label to use for this mapping
	*/
	virtual QString getLabel()=0;

	/**
	* Returns the file's name for this type mapping.
	*
	* @return the name for this mapping
	*/
	virtual QString getName()=0;
};
#endif