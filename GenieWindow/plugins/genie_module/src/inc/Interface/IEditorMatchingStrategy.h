#ifndef IEDITORMATCHINGSTRATEGY_H
#define IEDITORMATCHINGSTRATEGY_H

class IEditorReference;
class IEditorInput;

class IEditorMatchingStrategy
{
public:
	/**
	* Returns whether the editor represented by the given editor reference 
	* matches the given editor input.
	* <p>
	* Implementations should inspect the given editor input first, 
	* and try to reject it early before calling <code>IEditorReference.getEditorInput()</code>,
	* since that method may be expensive. 
	* </p>
	* 
	* @param editorRef the editor reference to match against
	* @param input the editor input to match
	* @return <code>true</code> if the editor matches the given editor input,
	*   <code>false</code> if it does not match
	*/
	virtual bool matches(IEditorReference* editorRef, IEditorInput* input)=0;
};
#endif