#ifndef POWERLINE_DETAILDLG_H
#define POWERLINE_DETAILDLG_H

#include <QDialog>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QGraphicsProxyWidget>


#include "PowerlineNode.h"
#include "PowerlineEdge.h"



namespace Ui {
    class Powerline_DetailDlg;
}

class Powerline_DetailDlg : public QDialog
{
    Q_OBJECT

public:
    explicit Powerline_DetailDlg(QWidget *parent = 0);
    ~Powerline_DetailDlg();

private:
    Ui::Powerline_DetailDlg *ui;

public:
	void initializeValue(const QList<AttachDeviceEntry> &);				//wireless and wired device
	void initializeValue(const QList<QPair<QString,QString> > &);		//usb device
	void initializeTopNode(PowerlineNode *node, PowerlineEdge *edge);		//Top Node Infomation
protected:
	void mousePressEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent *);
	void changeEvent(QEvent *);
	

	void retranslateUi();


	bool    m_bMove;
	QPoint  m_position;
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

	void itemFromData(PowerlineNode *node, PowerlineEdge *edge, const AttachDeviceEntry &);
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

	enum DATA_TYPE
	{
		DT_NORMAL_DEV = (1 << 0),
		DT_USB_DEV = (1 << 1),

		DT_COUNT
	};

	void positionItem();
	void onSubscribData(DATA_TYPE nType, int nStart, int nEnd);		//顯示一頁數據
	

	const QList<AttachDeviceEntry>			*m_pAttDeviceEntry;
	const QList<QPair<QString, QString> >	*m_pAttDeviceUsb;
	void SubScribeData();

	/************************/
	//QPushButton				*m_nextPageBtn;
	//QPushButton				*m_prevPageBtn;
	//QGraphicsProxyWidget	*m_preBtnProxy;
	//QGraphicsProxyWidget	*m_nextBtnProxy;

	void createControlPannel();

private slots:
	void slot_Btn_close();
	
	void pageBtnClick();
};

#endif // POWERLINE_DETAILDLG_H
