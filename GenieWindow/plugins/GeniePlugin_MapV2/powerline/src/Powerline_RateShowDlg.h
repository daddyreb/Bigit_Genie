#ifndef POWERLINE_RATESHOWDLG_H
#define POWERLINE_RATESHOWDLG_H

#include <QDialog>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QGraphicsProxyWidget>


#include "PowerlineNode.h"
#include "PowerlineEdge.h"
#include "PLDevice.h"



namespace Ui {
    class Powerline_RateShowDlg;
}

class Powerline_RateShowDlg : public QDialog
{
    Q_OBJECT

public:
    explicit Powerline_RateShowDlg(QWidget *parent = 0);
    ~Powerline_RateShowDlg();

private:
    Ui::Powerline_RateShowDlg *ui;

public:
	void initializeValue(const QList<PLDevice *> &);					//Powerline dev
	void initializeTopNode(PowerlineNode *node, PowerlineEdge *edge);		//Top Node Infomation
protected:
	void mousePressEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent *);
	bool eventFilter(QObject *, QEvent *);

	

	void retranslateImages();


	bool    m_bMove;
	QPoint  m_position;
private:

	//===========2012-08-08==============
	QPair<qint16,qint16> getRate(const QString &strCurMac, QMap<QString, QPair<qint16,qint16> >&mapLst);  //查找当前节点(strCurMac)到目标节点(strDestMac)的速率
	//==============END==================

	quint32 m_flags;

	/********************/
	QGraphicsView		*m_view;
	QGraphicsScene		*m_scene;

	void createGraphics();
	void initStyle();
	void initSlot();
	void initLanguage();
	void createControlPannel();
	/*********************/
	QList<PowerlineNode *>	m_plNode;
	QList<PowerlineEdge *>	m_plEdge;
	PowerlineNode			*m_TopNode;
	PowerlineEdge			*m_TopEdge;

	void itemsFromDevice(PowerlineNode *node, PowerlineEdge *edge, PLDevice *dev);

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
	void onSubscribData(int nStart, int nEnd);		//顯示一頁數據
	

	//const QList<AttachDeviceEntry>			*m_pAttDeviceEntry;
	//const QList<QPair<QString, QString> >	*m_pAttDeviceUsb;
	QList<PLDevice *>  m_pPLDev;
	void SubScribeData();

	/************************/
	//QPushButton				*m_nextPageBtn;
	//QPushButton				*m_prevPageBtn;
	//QGraphicsProxyWidget	*m_preBtnProxy;
	//QGraphicsProxyWidget	*m_nextBtnProxy;


	/***********************/
	bool	m_isPowerlineShow;
	enum DIRECT_ARROW
	{
		DA_LEFT = 0,
		DA_RIGHT,
		DA_UP,
		DA_DOWN
	};

	enum NavBtnState
	{
		BTNSTATE_NORMAL = 1 << 0,
		BTNSTATE_HOVER	= 1 << 1,
		BTNSTATE_DISABLED = 1 << 2,
	};



	int				m_prevPageBtnState;
	int				m_nextPageBtnState;






private slots:
	void slot_Btn_close();
	
	void pageBtnClick();
};

#endif // POWERLINE_RATESHOWDLG_H
