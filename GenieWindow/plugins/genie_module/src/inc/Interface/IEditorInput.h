#ifndef IEDITORINPUT_H
#define IEDITORINPUT_H
#include <QString>

class IEditorInput
{
public:
	/**
	* Returns whether the editor input exists.
	* <p>
	* This method is primarily used to determine if an editor input should
	* appear in the "File Most Recently Used" menu. An editor input will appear
	* in the list until the return value of <code>exists</code> becomes
	* <code>false</code> or it drops off the bottom of the list.
	* 
	* @return <code>true</code> if the editor input exists;
	*         <code>false</code> otherwise
	*/
	virtual bool exists()=0;


	/**
	* Returns the name of this editor input for display purposes.
	* <p>
	* For instance, when the input is from a file, the return value would
	* ordinarily be just the file name.
	* 
	* @return the name string; never <code>null</code>;
	*/
	virtual QString getName()=0;
	/**
	* Returns the tool tip text for this editor input. This text is used to
	* differentiate between two input with the same name. For instance,
	* MyClass.java in folder X and MyClass.java in folder Y. The format of the
	* text varies between input types.
	* </p>
	* 
	* @return the tool tip text; never <code>null</code>.
	*/
	virtual QString getToolTipText()=0;
};
#endif