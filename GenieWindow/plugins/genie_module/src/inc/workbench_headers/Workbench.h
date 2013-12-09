#ifndef MAINWND_H
#define MAINWND_H

#include <QMainWindow>
#include <QMap>
#include <QAction>
#include "binarysplitter.h"
#include <QWidget>
#include <QToolBar>

class Workbench : public QMainWindow
{
	Q_OBJECT

public:
	Workbench(QWidget *parent = 0);
	~Workbench();


protected slots:
	void switchPerspective(QAction*);
protected:
	QMap<QAction*,BinarySplitter*> m_act2Perspectives;
	QMap<BinarySplitter*,QToolBar*> m_perspectives2Tb;
	QWidget perParent;
public:
	void addPerspective(QAction* act,BinarySplitter* bs,QToolBar* tb);
};

#endif // MAINWND_H
