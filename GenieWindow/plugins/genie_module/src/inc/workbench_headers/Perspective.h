#ifndef PERSPECTIVE_H
#define PERSPECTIVE_H

#include "binarysplitter.h"
#include "WorkbenchPart.h"
#include "tabpage.h"
#include "tabpaget1base.h"
#include "tabpaget2base.h"
#include <QtCore/QMap>
#include <QtGui/QAction>
#include <QtGui/QToolBar>

class Perspective :
	public BinarySplitter
{
	Q_OBJECT
public:
	Perspective (Qt::Orientation orientation, QWidget * parent = 0);
	~Perspective(void);
public:
	QMap<int, WorkbenchPart*> mapWorkbenchPart;
	void addActionsToTabPage(TabPage* tab,QList<QAction*> acts);
	void setWorkbenchRightToolBar(WorkbenchPart * workbenchpart,QToolBar* righttoolbar);
	void addTabPageToWorkbenchPart(WorkbenchPart* workbenchpart,TabPage* tabpage,QIcon& qicon,QString tabname);
	void setObjectNameToTabPage(TabPage* tabpage,QString tabname);
	void setWorkbenchRightToolBars(QToolBar* righttoolbar);
};

#endif