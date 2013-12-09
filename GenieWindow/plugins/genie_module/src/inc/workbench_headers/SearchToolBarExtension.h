#ifndef SEARCHTOOLBAREXTENSION_H
#define SEARCHTOOLBAREXTENSION_H
#include "IToolBarExtension.h"
#include <QObject>
#include <QMainWindow>
#include <QMessageBox>

#include "RegisterObject.h"

class SearchToolBarExtension : public QObject, public IToolBarExtension
{
	Q_OBJECT
public:
	virtual QString getLabel()
	{
		return "HelloToolBar";
	}

	virtual QToolBar* create(QMainWindow* pMainWin)
	{
		QToolBar* pToolBarTemp = pMainWin->addToolBar("Search");
		QAction* pAction = pToolBarTemp->addAction(QIcon(":/image/2.png"), "Search");
		QObject::connect((QObject*)pAction, SIGNAL(triggered(bool)), this, SLOT(Search(bool)));
		return pToolBarTemp;
	}


public slots:
	void Search(bool b)
	{
		QMessageBox msgBox;
		msgBox.setWindowTitle("SearchToolBarExtension#Search");
		msgBox.setText("Search");
		msgBox.exec();
	}
};

REGISTER_EXTENSION_CLASS("toolbars.Search", SearchToolBarExtension);


#endif