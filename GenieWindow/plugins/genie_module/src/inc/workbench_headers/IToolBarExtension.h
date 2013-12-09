#ifndef ITOOLBAREXTENSION_H
#define ITOOLBAREXTENSION_H

#include "IExtension.h"
#include <QToolBar>

class IToolBarExtension : public IExtension
{
public:
	virtual QString getLabel()
	{
		return "ToolBarExtension";
	}

	virtual QToolBar* create(QMainWindow* pMainWin) = 0;
};

#endif