#ifndef IEDITORREFERENCE_H
#define IEDITORREFERENCE_H
#include <QString>

class IEditorInput;
class IEditorPart;

class IEditorReference
{
public:
	/**
	* Returns the factory id of the factory used to 
	* restore this editor. Returns null if the editor
	* is not persistable.
	*/
	virtual QString getFactoryId()=0;

	/**
	* Returns the editor input name. May return null is the
	* name is not available or if the editor failed to be 
	* restored.
	*/
	virtual QString getName()=0;

	/**
	* Returns the editor referenced by this object.
	* Returns <code>null</code> if the editor was not instantiated or
	* it failed to be restored. Tries to restore the editor
	* if <code>restore</code> is true.
	*/
	virtual IEditorPart* getEditor(bool restore)=0;

	/**
	* Returns true if the editor is pinned otherwise returns false.
	*/
	virtual bool isPinned()=0;

	/**
	* Returns the editor input for the editor referenced by this object.
	* <p>
	* Unlike most of the other methods on this type, this method
	* can trigger plug-in activation.
	* </p>
	*  
	* @return the editor input for the editor referenced by this object
	* @throws PartInitException if there was an error restoring the editor input
	* @since 3.1
	*/
	virtual IEditorInput* getEditorInput()=0;
};
#endif