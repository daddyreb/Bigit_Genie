#ifndef IVIEWSITE_H
#define IVIEWSITE_H
#include "IWorkbenchPartSite.h"
#include <QString>
class IActionBars;

class IViewSite:public IWorkbenchPartSite
{
public:
	/**
	* Returns the action bars for this part site.
	* Views have exclusive use of their site's action bars.
	*
	* @return the action bars
	*/
	virtual IActionBars* getActionBars()=0;

	/**
	* Returns the secondary id for this part site's part,
	* or <code>null</code> if it has none.
	* 
	* @see IWorkbenchPage#showView(String, String, int)
	* @since 3.0
	*/
	virtual QString getSecondaryId()=0;
};
#endif