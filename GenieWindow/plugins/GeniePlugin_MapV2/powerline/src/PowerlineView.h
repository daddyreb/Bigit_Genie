#ifndef TOPOLOGYVIEW_H
#define TOPOLOGYVIEW_H

#include <QGraphicsView>
#include <QList>
#include <QtCore>

#include "mainpipedefines.h"
#include "Singleton.h"
#include "PowerlineDefine.h"


class PLDevice;
class PowerlineNode;
class PowerlineEdge;
class QGraphicsScene;
class QResizeEvent;
class QTimerEvent;
class QGraphicsProxyWidget;
class QPushButton;



class PowerlineView : public QGraphicsView, public Singleton<PowerlineView>
{
    Q_OBJECT
public:
    PowerlineView(QWidget *parent = 0);
    ~PowerlineView();

    QGraphicsScene          *m_scene;

	static DEVICE_TYPE_ getDevType(const QString &);
	static QString getDevStrType(const int);
	

private:
	/***********2012-7-23 add*************/
	PowerlineNode			*m_InternetNode;
	PowerlineEdge			*m_InternetEdge;
	PowerlineNode			*m_RouterNode;
	PowerlineEdge			*m_RouterEdge;
	bool					m_IsInternetConnected;
	bool					m_IsRouterOk;

	//void verifyInternetNodeEdge();
	//void getNewNodes();
	void createRouterAndEarth();
	void positionRouterAndEarth(const QPointF &pos);
	void updateInternetState(bool bConnected = true);			//true : connected  false : disconnected
	void updateBtn();
	//void setRouterAndEarthVisible(bool);
	/***************END*******************/
private:
    QList<PowerlineNode *>   m_nodesList;


    PowerlineNode			*m_TopNode;
    QString					m_TopNodeIdx;
    PowerlineEdge			*m_TopEdge;


    PowerlineNode			*m_nodePic;
    //QGraphicsProxyWidget	*m_labPic;

    QGraphicsProxyWidget	*m_preBtnProxy;
    QGraphicsProxyWidget	*m_nextBtnProxy;
    QPushButton				*m_prevPageBtn;
    QPushButton				*m_nextPageBtn;

    int		m_idxStartPos;

    int		m_LocalDevSonPowel;



    int  m_MouseOverNodeIdx;
    bool        m_bFirstStart;

    void itemsFromDevice(PowerlineNode *node, PowerlineEdge *edge, PLDevice *dev);
    void positionItems(int nStart, int nLen/*, int nCurrenIdxPage*/);
    //void verifyInternetNodeEdge();
    void verifyEdgeTooltip(PowerlineEdge *edge);
    //void retranslateUI();
    //bool detectPowerLine(topologynode *node, topologyedge *edge, Device *dev);
    QString verifyNodeName(const PLDevice &dev);
    int parseDeviceSignalLevel(const PLDevice &dev);
    int parseDeviceSignalLevel2(const PLDevice &dev);


    void retranslateUi();
    void stringDealColon(QString &str);


    void preWirelessConfiguration(const QList<PLDevice *> &lstDev);
	const QList<PLDevice *> *m_lstPlDev;
    void initStyle();
protected:
    virtual void resizeEvent(QResizeEvent *event);
    //virtual void timerEvent(QTimerEvent *);
    virtual void changeEvent(QEvent *);


public:
    void onNewData(PLDevice *, const QList<PLDevice *> &, int, int/*, int*/);
    void onInternetStateChanged(bool connected);
    void preWirelessConfiguration();
    enum BTN_IDX
    {
        BI_PRE_BTN,
        BI_NEXT_BTN,
    };
    void btnSetShow(bool bShow, BTN_IDX nBtn, bool bEnable);
	void nodeOnlineStateChange(bool b);

    QList<PowerlineEdge *>   m_edgesList;

private:

	enum LOCALDEV_ONLINE_STATE
	{
		LOS_ON,
		LOS_OFF,
		LOS_INIT,

		LOS_COUNT
	};
	LOCALDEV_ONLINE_STATE m_localDevPCState;

    QString translateText(int idx, const QString &defaultText = QString());

    bool isCurrentPageIdx(int idx, int nStart, int nLen, int nCurrenIdxPage);


    int getWirelessTotal(QList<AttachDeviceEntry> &);
    int getWiredTotal(QList<AttachDeviceEntry> &);
    void dealLinkPLCNode(QList<PowerlineNode *> &node, QList<PowerlineEdge *> &edge, int nLen);

	void createNormalNode(int nParent, int nCurIdx, int nNum, QList<QPair<QString,QString> > &data, DEVICE_TYPE_ nType);
	void createUsbNode(int nParent, int nCurIdx, QList<QPair<QString,QString> > &data);
	void createLocalDevNode(int nParent, int nCurIdx, DEVICE_TYPE_ nType);


    const QList<PLDevice *> *m_cacheData;
signals:
    void translateText(int idx, QString *text);
    void nodeClicked(int idx);
    void nodeContextMenu(int idx, const QPoint &pos);

    void btnClicked(int);		//0:preBtn; 1:nextBtn;
private slots:
    void onNodeClicked(PowerlineNode *);
    void onNodeContextMenu(PowerlineNode *,const QPoint&);
    void onMouseOverNode(int);

    void BtnClick();

private:
	PowerlineNode	*m_plLocalDevNode;
	PowerlineEdge	*m_plLocalDevEdge;
};

#endif // TOPOLOGYVIEW_H
