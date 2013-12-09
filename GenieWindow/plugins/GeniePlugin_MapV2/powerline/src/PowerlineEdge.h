#ifndef TOPOLOGYEDGE_H
#define TOPOLOGYEDGE_H

#include <QGraphicsItem>
#include <QPointF>
#include <QString>
#include "PowerlineDefine.h"

class PowerlineNode;

class PowerlineEdge : public QGraphicsItem
{
public:
    PowerlineEdge();

    bool        m_onLine;
    bool        m_isLocalHost;
    bool        m_wireless;
	bool		m_bLocalDevice;

	int			m_devLedState;

    int         m_deviceType;
    int         m_signalLevel;

	bool		m_isUsb;

	QString		m_devName;
	QString		m_devIP;
	QString		m_devMac;

    QString     m_signalStrength;
    QString     m_linkRate_send;
	QString     m_linkRate_recv;

    QPointF     m_headPt;
    QPointF     m_endPt;
	QPointF		m_topPt;	//如果是Powerline且是localdevice那么就为那条竖线，m_topPt = m_endPt;
	QPointF		m_bottomPt;	//如果是powerline不是localdevice那么就为父节点的起始跟结束坐标。
	QPointF		m_topTopPt;
	QPointF		m_bottomBottomPt;
	bool		m_bDetailDlgParent;
	bool		m_bHubView;

	int			m_S_nodeWidth;	//自己的宽
	int			m_P_nodeHeight;
	int			m_P_nodeWidth;	//父节点的宽
	int			m_S_nodeHeight;	//节点的宽与高，主要是记录powerline的就ok了

    QStringList m_parent;
    QStringList m_son;
	
	qreal       m_Angle;
	qreal		m_rotate;

	bool        m_bArrow;

private:
    QRectF  calSignalPixmapRect() const;		//No use
	QRectF calUsbPixmapRect() const;
private:
	enum DIRECT_ARROW
	{
		DA_LEFT = 0,
		DA_RIGHT,
		DA_UP,
		DA_DOWN
	};
	void drawArrowLine(QPainter *painter, QColor &pColor, DIRECT_ARROW nDirection = DA_LEFT);
	void drawArrowLine2(QPainter *painter, QColor &pColor, DIRECT_ARROW nDirection = DA_LEFT);
public:
    QRectF  boundingRect() const;
    QPainterPath    shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	PowerlineEdge & operator = (const PowerlineEdge &);
};

#endif // TOPOLOGYEDGE_H
