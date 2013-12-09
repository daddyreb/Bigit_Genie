#ifndef TABPAGE_H
#define TABPAGE_H

#include <QMainWindow>
#include <QString>
#include <QIcon>
#include <QToolBar>
#include <QList>
#include <QAction>

class TabPage : public QMainWindow
{
	Q_OBJECT
public:
	enum TabPageType{Type1,Type2};
public:
	explicit TabPage(QWidget* parent=0);
	virtual ~TabPage();

public:
	virtual TabPageType type()=0;
protected:
	QString strPageTxt;
	QIcon*	icon;
protected:
	QToolBar*	tb;
	QList<QAction*> tbActs;
	bool		btoolBarShow;
protected:
	void createToolBar();
public:
	void setTabPageTxt(const QString& strTxt);
	QString tabPageTxt();

	void setTabPageIcon(const QIcon& icon);
	QIcon* tabPageIcon();

	QList<QAction*> getActionList();
	void setToolBarVisible(bool bVisible);

	void setContentWidget(QWidget* w);
	void setPageActions(const QList<QAction*>& acts);
};

#endif // TABPAGE_H
