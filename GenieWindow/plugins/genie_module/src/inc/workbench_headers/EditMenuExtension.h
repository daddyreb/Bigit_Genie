#ifndef EDITMENUEXTENSION_H
#define EDITMENUEXTENSION_H

#include "IMenuExtension.h"
#include <QObject>
#include <QMessageBox>
#include "RegisterObject.h"

class EditMenuExtension : public QObject, public IMenuExtension
{
	Q_OBJECT
public:
	virtual QString getLabel()
	{
		return "menus.Edit";
	}

	virtual void create(QMenuBar* pMenuBar, QMainWindow* pMainWin)
	{
		QMenu* pMenu = pMenuBar->addMenu("Edit");
		QAction* pAction = new QAction(QIcon(":/image/2.png"), QObject::tr("Find"), pMainWin);
		pMenu->addAction(pAction);
		QObject::connect((QObject*)pAction, SIGNAL(triggered(bool)), this, SLOT(Find(bool)));

		pAction = new QAction(QIcon(":/image/3.png"), QObject::tr("Replace"), pMainWin);
		pMenu->addAction(pAction);
		QObject::connect((QObject*)pAction, SIGNAL(triggered(bool)), this, SLOT(Replace(bool)));
	}

public slots:
	void Find(bool b)
	{
		QMessageBox msgBox;
		msgBox.setWindowTitle("EditMenuExtension#Find");
		msgBox.setText("Find");
		msgBox.exec();
	}

	void Replace(bool b)
	{
		QMessageBox msgBox;
		msgBox.setWindowTitle("FindMenuExtension#Replace");
		msgBox.setText("Replace");
		msgBox.exec();
	}
};

REGISTER_EXTENSION_CLASS("menus.Edit", EditMenuExtension);

#endif