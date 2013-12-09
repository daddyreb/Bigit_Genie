#ifndef POWERLINE_HUBMAPVIEW_H
#define POWERLINE_HUBMAPVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include "PowerlineDefine.h"
#include "PowerlineNode.h"
#include "PowerlineEdge.h"

class Powerline_HubMapView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit Powerline_HubMapView(QWidget *parent = 0);
	~Powerline_HubMapView();

public:

	void initializeValue(const QList<QPair<QString,QString> > &);		//usb device
	void initializeTopNode(PowerlineNode *node, PowerlineEdge *edge);		//Top Node Infomation
	void butReturn();			//Return button
	void pageBtnClick(int nCurPageIdx);
	void delAllData();
protected:
	void retranslateUi();
	void resizeEvent(QResizeEvent *event);
signals:
	void pageTotal(int);			//Page total num
private:

	quint32 m_flags;

	/********************/
	QGraphicsView		*m_view;
	QGraphicsScene		*m_scene;

	void createGraphics();
	void initStyle();
	void initSlot();
	void initLanguage();
	/*********************/
	QList<PowerlineNode *>	m_plNode;
	QList<PowerlineEdge *>	m_plEdge;
	PowerlineNode			*m_TopNode;
	PowerlineEdge			*m_TopEdge;

	void itemFromData(PowerlineNode *node, PowerlineEdge *edge, const QPair<QString,QString> &);

	void delRedundanceNodes(int nCurIdx, int nCnt);		//Remove redundance nodes
	void createNewNodes(int nCurIdx);					//Create New nodes

	void updateUiElements();
	void beforeUpdateUi(int *pStart, int *pEnd, int nCnd);		//翻頁前的數據初始化

	void dealPostionStartAndEnd(int *pStart, int *pEnd, int nModeNum);
	/************************/
	int		m_curPageIdx;		//Current page idx
	int		m_totalPage;		//Total page numbers
	int		m_devTotalCount;	//Total Dev num
	int		m_modNum;


	void positionItem();
	void onSubscribData(int nStart, int nEnd);		//顯示一頁數據


	QList<QPair<QString, QString> >	m_pAttDeviceUsb;
	void SubScribeData();
private slots:

};

#endif // POWERLINE_HUBMAPVIEW_H
