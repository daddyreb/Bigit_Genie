#pragma once
#include <QtCore>
#include "IWorkbenchPage.h"

class WorkbenchWindow;
class IWorkbenchPart;


class WorkbenchPage : public IWorkbenchPage
{
public:
	WorkbenchWindow* window;

    bool partBeingOpened;

    QSet<void*>* dirtyPerspectives;

    int deferCount;

    QList<void*> pendingDisposals;

	IWorkingSet* workingSets;
	QString aggregateWorkingSetId;

};
