#ifndef ISELECTIONPROVIDER_H
#define ISELECTIONPROVIDER_H
class ISelectionChangedListener;
class ISelection;

class ISelectionProvider
{
public:
	/**
	* Adds a listener for selection changes in this selection provider.
	* Has no effect if an identical listener is already registered.
	*
	* @param listener a selection changed listener
	*/
	virtual void addSelectionChangedListener(ISelectionChangedListener* listener)=0;

	/**
	* Returns the current selection for this provider.
	* 
	* @return the current selection
	*/
	virtual ISelection* getSelection()=0;

	/**
	* Removes the given selection change listener from this selection provider.
	* Has no affect if an identical listener is not registered.
	*
	* @param listener a selection changed listener
	*/
	virtual void removeSelectionChangedListener(
		ISelectionChangedListener* listener)=0;

	/**
	* Sets the current selection for this selection provider.
	*
	* @param selection the new selection
	*/
	virtual void setSelection(ISelection* selection)=0;
};
#endif