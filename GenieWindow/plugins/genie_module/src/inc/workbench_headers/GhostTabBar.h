#ifndef IQTABBAR_H
#define IQTABBAR_H

#include <QtGui/QTabBar>
#include <QtGui/QTabWidget>
#include <QtGui/QMouseEvent>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QDropEvent>

#define TABITEM_WIDTH 100

class GhostTabBar :
	public QTabBar
{
public:
	GhostTabBar(QWidget* parent=0);
	~GhostTabBar(void);
protected :
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void dragEnterEvent(QDragEnterEvent *event);
	void dragMoveEvent(QDragMoveEvent *event);
	void dropEvent(QDropEvent *event);
private:
	void performDrag();
	QPoint dragStartPosition;
protected:
	virtual QSize tabSizeHint (int index) const;
};
#endif

