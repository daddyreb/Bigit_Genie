#ifndef IDRAGOVERLISTENER_H
#define IDRAGOVERLISTENER_H

#include <QObject>
#include <QPoint>
#include <QRect>

/**
* Implementers of this interface will receive notifications when objects are dragged over
* a particular SWT control. 
*/
class IDragOverListener {

	/**
	* Notifies the receiver that the given object has been dragged over
	* the given position. Returns a drop target if the object may be
	* dropped in this position. Returns null otherwise.
	* 
	* @param draggedObject object being dragged over this location
	* @param position location of the cursor
	* @param dragRectangle current drag rectangle (may be an empty rectangle if none)
	* @return a valid drop target or null if none
	*/
	/*virtual IDropTarget drag(Control currentControl, QObject draggedObject,
		QPoint position, QRect dragRectangle) = 0;*/
};

#endif