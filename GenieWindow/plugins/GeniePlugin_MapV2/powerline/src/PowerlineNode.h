#ifndef TOPOLOGYNODE_H
#define TOPOLOGYNODE_H

#include <QGraphicsItem>
#include <QtCore>
#include <QPoint>
#include <QPainterPath>
#include <QPointF>

#include "PowerlineDefine.h"
#include "mainpipedefines.h"
#include "Powerline_mapuitilities.h"

class QGraphicsSceneMouseEvent;
class QGraphicsSceneContextMenuEvent;
class QGenieTopoNodePaintStrategy;
class PowerlineView;
class PowerlineEdge;
//class BlockDeviceManager;


class PowerlineNode : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    PowerlineNode();
    ~PowerlineNode();

    bool        m_isLocalHost;
    bool        m_onLine;
    bool        m_isPowerLine;
    int         m_deviceType;
	bool        m_isSonNodePaint;
	bool	m_bPicUseDefault;
    ////Powerline data/////
    QString     m_devMac;
    QString     m_devModelName;
    QString     m_devFwVer;
    QString     m_devName;
    QString     m_devType;			//NetworkType
    QString     m_devIp;
    QStringList m_parent;
    QStringList m_son;
    int         m_devLedState;
	int			m_devAttCount;
	bool		m_devSupportUSB;
	bool		m_devSupportWireless;
	bool		m_localDevice;

	bool        m_bDetailDlgParent;
	bool		m_bHubView;

	WLANInfo	m_wlanInfo;
	int         m_Idx;
	bool		m_localHostDevPc;
	bool		m_bLocalHostDevPcOn;			//¶ÏÍøÃ»ÓÐ

	int         m_nID;

	bool		m_isTextShowUp;

	QList<AttachDeviceEntry>		m_attachNetDev;
	QList<QPair<QString,QString> >	m_attachUsbDev;

private:
    void drawDevNum(QPainter *painter, QString data = NULL, int nType = 0, int pixrole = PIR_SMALL);
protected:
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
	//virtual void mouse(QGraphicsSceneMouseEvent *event);
public:
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void updateShapeCache();

	PowerlineNode & operator = (const PowerlineNode &);
private:
    QPainterPath    m_shapePath;
    //QGenieTopoNodePaintStrategy *m_paintStrategy;

	bool		m_bDownLeftBtn;
	
signals:
    void clicked(PowerlineNode *);
    void contextMenu(PowerlineNode *, const QPoint &pos);

	void nodeMouseOver(int);
public:
    enum _NODE_STATE
    {
        NS_INIT,
        NS_INSCREEN,
        NS_FADEOUT,
        NS_BEHIND
    };

	//enum _DBG_FLAG{
	//	DF_CONNECTED=(1<<0),
	//	DF_CONS = (1 << 1)
	//};
	//quint32 mFlags;
    //static BlockDeviceManager *sBDM;
};

#endif // TOPOLOGYNODE_H
