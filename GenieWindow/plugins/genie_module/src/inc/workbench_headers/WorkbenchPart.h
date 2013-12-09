#ifndef IQTABWIDGET_H
#define IQTABWIDGET_H

#include "GhostTabBar.h"
#include "tabpage.h"
#include "cornerbar.h"
#include "binarysplitter.h"
#include <QImage>
#include <QString>
#include <QtGui/QTabWidget>
#include <QtGui/QMouseEvent>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QDropEvent>
#include <QVector>
#include <QMap>
#include <QAction>
#include <QList>

class UpdateTabsEvent;

class WorkbenchPart :
	public QTabWidget
{
	Q_OBJECT
protected:
	enum DropAction{NO_DROP,ADD_TAB,SPLIT_HORIZONTAL,SPLIT_VERTICAL};
public:
	explicit WorkbenchPart(QWidget *parent=0);

	~WorkbenchPart(void);
protected :
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void dragEnterEvent(QDragEnterEvent *event);
	void dragMoveEvent(QDragMoveEvent *event);
	void dropEvent(QDropEvent *event);
private:
	QPoint dragStartPosition;
protected:
	DropAction dropAction;
	QAction* menutoolbar;
	QAction* minAction;
	QAction* maxAction;
	QAction* menuAction;
	QToolBar* MainWindowRightToolBar;
	QAction* restoreTabAction;
	QString strActionText;
	bool	bMax;

protected:
	void recycleWidget();
	virtual void resizeEvent(QResizeEvent * event);
protected:
	QVector<TabPage*> tabPages;
	QMap<QAction*,TabPage*> act2Page;
public:
	virtual QSize minimumSizeHint () const ;
	virtual QSize sizeHint() const;

public:
	int addTab (QWidget * page, const QString & label);
	int addTab (QWidget * page, const QIcon & icon,const QString & label);

	int insertTab(int index,QWidget * page,const QString & label); 
	int insertTab(int index,QWidget * page,const QIcon & icon, const QString & label);

	void removeTab(int index);

	void setTitle ( int index, const QString & label );
	void setTitleImage ( int index, const QIcon & icon );
	int indexOf ( QWidget * w ) const;
protected:
	int addTabPage(TabPage* page);
	TabPage* removeTabPage(int index);
public:
	//********lao5 start*********/
	void setRightToolBar(QToolBar* rightToolBar);
	void showAllTabWidget();
	void showAllInSplitter(BinarySplitter* insplitter);
	//********lao5 end*********/
public:
	virtual bool event (QEvent * e);
	//返回该Tab控件中的Tab页的数量包括显示的和未显示的
	int pageCount(void);
protected slots:
	void cornerMoreBtnMenuTriggered(QAction * action);
	void updateMenu();
	void ontabCloseRequested (int index);
	void oncurrentChanged(int index);

	//lao5 start
	void minTabWidget();
	void maxTabWidget();
	void showTabWidget();
	void ShowToolBarClickedPage(QAction*  action);
	//lao5 end 
public:
	void setCurrentIndex(int index);
protected:
	//Tab控件中右上角的工具栏
	CornerBar* cb;
	//在Tab控件中有隐藏的Tab标签时显示该动作以供切换到被隐藏的标签页
	QAction* moreAct;
	//该指针指向Tab控件本身的一个动作，Tab页的动作显示到CornerBar时都显示在该动作的前面
	QAction* actBefore;
	//与moreAct相关联的菜单菜单项包含该Tab控件中所有的标签页
	QMenu* moreMenu;

	QList<QAction*> pageActs;//与每个TAB页相关的动作
	QList<QAction*> thisActs;//与TAB控件本身相关的动作

	//当与Tab页相关的动作显示在TAB控件的右上角时 bpageActsShow=true，否则bpageActsShow=false
	bool			bpageActsShow;
protected:
	//更新TAB控件中的Tab页和CornerBar中的动作的显示和隐藏
	void updateTabs(UpdateTabsEvent* e);
	//计算并返回TAB控件右上角CornerBar部分占用空间的宽度
	int cornerWidth();
	//给定一组动作计算将其添加到工具栏中将占用的空间的宽度
	int actionsWidth(const QList<QAction*>& acts);

	void hideTabForSpace();//Tab控件中的TAB标签显示不下时隐藏部分标签
	void showTabToSpace();//显示Tab控件中未显示的标签以占用多余的空间


	private:
	 QString title ; //$NON-NLS-1$

     //ImageDescriptor* imageDescriptor;

     QImage* titleImage;

     QString toolTip ; //$NON-NLS-1$

     //IConfigurationElement* configElement;



     QString partName; //$NON-NLS-1$

     QString contentDescription ; //$NON-NLS-1$
    

};
#endif

