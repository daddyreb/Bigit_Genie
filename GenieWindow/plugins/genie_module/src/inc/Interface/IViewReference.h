#ifndef IVIEWREFERENCE
#define IVIEWREFERENCE
#include <QString>
#include "IWorkbenchPartReference.h"
class IViewPart;

class IViewReference:public IWorkbenchPartReference
{
public:
	/**
	* Returns the secondary ID for the view.
	* 
	* @return the secondary ID, or <code>null</code> if there is no secondary id
	* @see IWorkbenchPage#showView(QString, QString, int)
	* @since 3.0
	*/
	virtual QString getSecondaryId()=0;

	/**
	* Returns the <code>IViewPart</code> referenced by this object.
	* Returns <code>null</code> if the view was not instantiated or
	* it failed to be restored.  Tries to restore the view
	* if <code>restore</code> is true.
	*/
	virtual IViewPart* getView(bool restore)=0;

	/**
	* Returns true if the view is a fast view otherwise returns false.
	*/
	virtual bool isFastView()=0;
};
#endif