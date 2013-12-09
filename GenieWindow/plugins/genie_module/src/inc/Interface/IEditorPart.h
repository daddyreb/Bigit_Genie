#ifndef IEDITORPART_H
#define IEDITORPART_H
#include "IWorkbenchPart.h"
#include "ISaveablePart.h"


class IEditorInput;
class IEditorSite;

class IEditorPart:public IWorkbenchPart,public ISaveablePart	
{
public:
	/**
	* Returns the input for this editor.  If this value changes the part must 
	* fire a property listener event with <code>PROP_INPUT</code>.
	*
	* @return the editor input
	*/
	virtual IEditorInput* getEditorInput()=0;

	/**
	* Returns the site for this editor. 
	* This method is equivalent to <code>(IEditorSite) getSite()</code>.
	* <p>  
	* The site can be <code>null</code> while the editor is being initialized. 
	* After the initialization is complete, this value must be non-<code>null</code>
	* for the remainder of the editor's life cycle.
	* </p>
	* 
	* @return the editor site; this value may be <code>null</code> if the editor
	*         has not yet been initialized
	*/
	virtual IEditorSite* getEditorSite()=0;

	/**
	* Initializes this editor with the given editor site and input.
	* <p>
	* This method is automatically called shortly after the part is instantiated.
	* It marks the start of the part's lifecycle. The 
	* {@link IWorkbenchPart#dispose IWorkbenchPart.dispose} method will be called 
	* automically at the end of the lifecycle. Clients must not call this method.
	* </p><p>
	* Implementors of this method must examine the editor input object type to
	* determine if it is understood.  If not, the implementor must throw
	* a <code>PartInitException</code>
	* </p>
	* @param site the editor site
	* @param input the editor input
	* @exception PartInitException if this editor was not initialized successfully
	*/
	virtual void init(IEditorSite* site, IEditorInput* input)=0;
};
#endif