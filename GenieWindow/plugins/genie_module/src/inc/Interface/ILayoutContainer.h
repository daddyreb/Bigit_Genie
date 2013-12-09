#ifndef ILAYOUTCONTAINER_H
#define ILAYOUTCONTAINER_H

class LayoutPart; 
class PartPane;

#include <QList>

class ILayoutContainer 
{
public:
	virtual bool allowsAdd(LayoutPart* toAdd) = 0;

	/**
	* Add a child to the container.
	*/
public:
	virtual void add(LayoutPart* newPart) = 0;

	/**
	* Returns a list of layout children.
	*/
public:
	virtual QList<LayoutPart*> getChildren() = 0;

	/**
	* Remove a child from the container.
	*/
public:
	virtual void remove(LayoutPart* part) = 0;

	/**
	* Replace one child with another
	*/
public:
	virtual void replace(LayoutPart* oldPart, LayoutPart* newPart) = 0;

public:
	//virtual void findSashes(LayoutPart* toFind, PartPane::Sashes result) = 0;

	/**
	* When a layout part closes, focus will return to the previously active part.
	* This method determines whether the parts in this container should participate
	* in this behavior. If this method returns true, its parts may automatically be
	* given focus when another part is closed. 
	* 
	* @return true iff the parts in this container may be given focus when the active
	* part is closed
	*/
public:
	virtual bool allowsAutoFocus() = 0;

	/**
	* Called by child parts to request a zoom in, given an immediate child 
	* 
	* @param toZoom
	* @since 3.1
	*/
public:
	virtual void childRequestZoomIn(LayoutPart* toZoom) = 0;

	/**
	* Called by child parts to request a zoom out
	* 
	* @since 3.1
	*/
public:
	virtual void childRequestZoomOut() = 0;

	/**
	* Returns true iff the given child is obscured due to the fact that the container is zoomed into
	* another part. 
	* 
	* @param toTest
	* @return
	* @since 3.1
	*/
public:
	virtual bool childObscuredByZoom(LayoutPart* toTest) = 0;

	/**
	* Returns true iff we are zoomed into the given part, given an immediate child of this container.
	* 
	* @param toTest
	* @return
	* @since 3.1
	*/
public:
	virtual bool childIsZoomed(LayoutPart* toTest) = 0;

	/**
	* Called when the preferred size of the given child has changed, requiring a
	* layout to be triggered.
	* 
	* @param childThatChanged the child that triggered the new layout
	*/
public:
	virtual void resizeChild(LayoutPart* childThatChanged) = 0;

};

#endif