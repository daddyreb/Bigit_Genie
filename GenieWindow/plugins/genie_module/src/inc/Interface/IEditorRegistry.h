#ifndef IEDITORREGISTRY_H
#define IEDITORREGISTRY_H
#include <QString>
class IPropertyListener;
class IEditorDescriptor;
class IFileEditorMapping;

class IEditorRegistry 
{
public:
	/**
	* Adds a listener for changes to properties of this registry.
	* Has no effect if an identical listener is already registered.
	* <p>
	* The properties ids are as follows:
	* <ul>
	*   <li><code>PROP_CONTENTS</code>: Triggered when the file editor mappings in
	*       the editor registry change.</li>
	* </ul>
	* </p>
	*
	* @param listener a property listener
	*/
	virtual void addPropertyListener(IPropertyListener* listener)=0;

	/**
	* Finds and returns the descriptor of the editor with the given editor id.
	*
	* @param editorId the editor id
	* @return the editor descriptor with the given id, or <code>null</code> if not
	*   found
	*/
	virtual IEditorDescriptor* findEditor(QString editorId)=0;

	/**
	* Returns the default editor. The default editor always exist.
	*
	* @return the descriptor of the default editor
	* @deprecated The system external editor is the default editor.
	* Use <code>findEditor(IEditorRegistry.SYSTEM_EXTERNAL_EDITOR_ID)</code>
	* instead.
	*/
	virtual IEditorDescriptor* getDefaultEditor()=0;

	/**
	* Returns the default editor for a given file name. This method assumes an
	* unknown content type for the given file.
	* <p>
	* The default editor is determined by taking the file extension for the
	* file and obtaining the default editor for that extension.
	* </p>
	* 
	* @param fileName
	*            the file name in the system
	* @return the descriptor of the default editor, or <code>null</code> if
	*         not found
	*/
	virtual IEditorDescriptor* getDefaultEditor(QString fileName)=0;

	/**
	* Returns the list of file editors registered to work against the file with
	* the given file name. This method assumes an unknown content type for the
	* given file.
	* <p>
	* Note: Use <code>getDefaultEditor(QString)</code> if you only the need
	* the default editor rather than all candidate editors.
	* </p>
	* 
	* @param fileName
	*            the file name in the system
	* @return a list of editor descriptors
	*/
	virtual IEditorDescriptor* getEditors(QString fileName)=0;

	/**
	* Returns a list of mappings from file type to editor.  The resulting list
	* is sorted in ascending order by file extension.
	* <p>
	* Each mapping defines an extension and the set of editors that are 
	* available for that type. The set of editors includes those registered 
	* via plug-ins and those explicitly associated with a type by the user 
	* in the workbench preference pages.
	* </p>
	*
	* @return a list of mappings sorted alphabetically by extension
	*/
	virtual IFileEditorMapping* getFileEditorMappings()=0;

	/**
	* Removes the given property listener from this registry.
	* Has no affect if an identical listener is not registered.
	*
	* @param listener a property listener
	*/
	virtual void removePropertyListener(IPropertyListener* listener)=0;

	/**
	* Sets the default editor id for the files that match that
	* specified file name or extension. The specified editor must be
	* defined as an editor for that file name or extension.
	*
	* @param fileNameOrExtension the file name or extension pattern (e.g. "*.xml")=0;
	* @param editorId the editor id or <code>null</code> for no default
	*/
	virtual void setDefaultEditor(QString fileNameOrExtension, QString editorId)=0;

	/**
	* Returns whether there is an in-place editor that could handle a file
	* with the given name.
	* 
	* @param filename the file name
	* @return <code>true</code> if an in-place editor is available, and
	* <code>false</code> otherwise
	* @since 3.0
	*/
	virtual bool isSystemInPlaceEditorAvailable(QString filename)=0;

	/**
	* Returns whether the system has an editor that could handle a file
	* with the given name.
	* 
	* @param filename the file name
	* @return <code>true</code> if an external editor available, and
	* <code>false</code> otherwise
	* @since 3.0
	*/
	virtual bool isSystemExternalEditorAvailable(QString filename)=0;

};
#endif