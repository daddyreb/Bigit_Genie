#ifndef SEARCHMENUEXTENSION_H
#define SEARCHMENUEXTENSION_H

#include "IMenuExtension.h"
#include <QObject>
#include <QMessageBox>
#include "RegisterObject.h"


class SearchMenuExtension : public QObject, public IMenuExtension
{
	Q_OBJECT
public:
	virtual QString getLabel()
	{
		return "TestMenu";
	}

	virtual void create(QMenuBar* pMenuBar, QMainWindow* pMainWin)
	{
		QMenu* pMenu = pMenuBar->addMenu("Search");
		QAction* pAction = new QAction(QIcon(":/image/4.png"), QObject::tr("Search"), pMainWin);
		pMenu->addAction(pAction);
		QObject::connect((QObject*)pAction, SIGNAL(triggered(bool)), this, SLOT(Search(bool)));
	}

public slots:
	void Search(bool b)
	{
		QMessageBox msgBox;
		msgBox.setWindowTitle("SearchMenuExtension#Search");
		msgBox.setText("Search");
		msgBox.exec();
	}
};

REGISTER_EXTENSION_CLASS("menus.Search", SearchMenuExtension);

#endif