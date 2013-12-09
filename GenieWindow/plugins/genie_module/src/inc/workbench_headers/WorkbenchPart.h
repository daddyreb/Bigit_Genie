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
	//���ظ�Tab�ؼ��е�Tabҳ������������ʾ�ĺ�δ��ʾ��
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
	//Tab�ؼ������ϽǵĹ�����
	CornerBar* cb;
	//��Tab�ؼ��������ص�Tab��ǩʱ��ʾ�ö����Թ��л��������صı�ǩҳ
	QAction* moreAct;
	//��ָ��ָ��Tab�ؼ������һ��������Tabҳ�Ķ�����ʾ��CornerBarʱ����ʾ�ڸö�����ǰ��
	QAction* actBefore;
	//��moreAct������Ĳ˵��˵��������Tab�ؼ������еı�ǩҳ
	QMenu* moreMenu;

	QList<QAction*> pageActs;//��ÿ��TABҳ��صĶ���
	QList<QAction*> thisActs;//��TAB�ؼ�������صĶ���

	//����Tabҳ��صĶ�����ʾ��TAB�ؼ������Ͻ�ʱ bpageActsShow=true������bpageActsShow=false
	bool			bpageActsShow;
protected:
	//����TAB�ؼ��е�Tabҳ��CornerBar�еĶ�������ʾ������
	void updateTabs(UpdateTabsEvent* e);
	//���㲢����TAB�ؼ����Ͻ�CornerBar����ռ�ÿռ�Ŀ��
	int cornerWidth();
	//����һ�鶯�����㽫����ӵ��������н�ռ�õĿռ�Ŀ��
	int actionsWidth(const QList<QAction*>& acts);

	void hideTabForSpace();//Tab�ؼ��е�TAB��ǩ��ʾ����ʱ���ز��ֱ�ǩ
	void showTabToSpace();//��ʾTab�ؼ���δ��ʾ�ı�ǩ��ռ�ö���Ŀռ�


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

