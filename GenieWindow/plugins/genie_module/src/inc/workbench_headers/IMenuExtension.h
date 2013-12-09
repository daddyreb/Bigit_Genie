#ifndef IMENUEXTENSION_H
#define IMENUEXTENSION_H

#include "IExtension.h"
#include <QMenuBar>
#include <QMainWindow>

class IMenuExtension : public IExtension
{
public:
	virtual QString getLabel()
	{
		return "MenuExtension";
	}

	virtual void create(QMenuBar* pMenuBar, QMainWindow* pMainWin) = 0;
};

#endif