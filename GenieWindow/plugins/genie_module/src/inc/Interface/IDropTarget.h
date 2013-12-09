#ifndef IDROPTARGET_H
#define IDROPTARGET_H

class Cursor;

#include <QRect>

/**
* This interface is used to drop objects. It knows how to drop a particular object
* in a particular location. IDropTargets are typically created by IDragOverListeners, and
* it is the job of the IDragOverListener to supply the drop target with information about 
* the object currently being dragged.
* 
* @see org.eclipse.ui.internal.dnd.IDragOverListener
*/
class IDropTarget
{

	/**
	* Drops the object in this position
	*/
public:
	virtual void drop() = 0;

	/**
	* Returns a cursor describing this drop operation
	* 
	* @return a cursor describing this drop operation
	*/
	virtual Cursor getCursor() = 0;

	/**
	* Returns a rectangle (screen coordinates) describing the target location
	* for this drop operation.
	* 
	* @return a snap rectangle or null if this drop target does not have a specific snap
	* location.
	*/
	virtual QRect getSnapRectangle() = 0;
};

#endif
