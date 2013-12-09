#include "topologyedge.h"
#include "mapdevicetype.h"
#include "mapuitilities.h"
#include "topologynode.h"

#include <QPainterPath>
#include <QPainter>
#include <QPen>
#include <QPixmap>
#ifdef USE_NODE_ANIMATION
TopologyEdge::TopologyEdge(TopologyNode *nodeStart,TopologyNode *nodeEnd)
    :QGraphicsItem(nodeStart)
    ,mNodeStart(nodeStart),
      mNodeEnd(nodeEnd),
#else
TopologyEdge::TopologyEdge()
    :
#endif
//        m_onLine(false),
      mEdgeFlags(0),
//        m_isLocalHost(false),
//        m_wireless(false),
        m_deviceType(MDT_NETWORKDEV),
        m_signalLevel(0)
{
    this->setZValue(-100.0);

}

QRectF TopologyEdge::calSignalPixmapRect() const
{
    if(mEdgeFlags & EF_ISWIRELESS)
    {
        QPixmap signalPixmap(getWirelessSignalImagePath((mEdgeFlags & EF_ONLINE) ? m_signalLevel : 0));
        QPointF tmpPt = boundingRect().center();

        tmpPt.setX(tmpPt.x() - signalPixmap.width() / 2);
        tmpPt.setY(tmpPt.y() - signalPixmap.height() / 2);

        return QRectF(tmpPt,QSizeF(qreal(signalPixmap.width()),qreal(signalPixmap.height())));
    }
    else
    {
        return QRectF();
    }
}

QRectF TopologyEdge::boundingRect() const
{
#ifdef USE_NODE_ANIMATION
    QPointF pStart=QPointF(0.0,0.0);
    QPointF pEnd=mNodeEnd?mNodeEnd->pos():QPointF(0.0,0.0)-mNodeStart->pos();

    //如果是水平线或者垂直线时...
    const int cMinWidth=16;
    const int cMinHeight=13;
    if(qAbs(pStart.x()-pEnd.x())<cMinWidth)
    {
        int center=(pStart.x()+pEnd.x())/2;
        pStart.setX(center-(cMinWidth>>1));
        pEnd.setX(center+(cMinWidth>>1));
    }
    if(qAbs(pStart.y()-pEnd.y())<cMinHeight)
    {
        int center=(pStart.y()+pEnd.y())/2;
        pStart.setY(center-(cMinHeight>>1));
        pEnd.setY(center+(cMinHeight>>1));
    }
    return QRectF(pStart,pEnd).normalized();
#else
    return QRectF(m_headPt,m_endPt).normalized();
#endif
}

QPainterPath TopologyEdge::shape() const
{
    QPainterPath shapePath;

#ifdef USE_NODE_ANIMATION
    QPointF pStart=QPointF(0.0,0.0);
    QPointF pEnd=mNodeEnd?mNodeEnd->pos():QPointF(0.0,0.0)-mNodeStart->pos();

    shapePath.moveTo(pStart);
    shapePath.lineTo(pEnd);
#else
    shapePath.moveTo(m_headPt);
    shapePath.lineTo(m_endPt);
#endif

    shapePath.addRect(calSignalPixmapRect());

    return shapePath;
}

void TopologyEdge::paint(QPainter *painter,
                         const QStyleOptionGraphicsItem *option,
                         QWidget *widget)
{
    Q_UNUSED(widget)
	Q_UNUSED(option)

    Qt::PenStyle penStyle = ((mEdgeFlags & EF_ONLINE) ? ((mEdgeFlags & EF_ISWIRELESS) ? Qt::DotLine : Qt::SolidLine) : Qt::DashDotDotLine);
    QColor penColor = ((mEdgeFlags & EF_ISLOCALHOST) || (m_deviceType == MDT_INTERNET)) ?
                ((mEdgeFlags & EF_ONLINE) ? QColor(83,166,73) : QColor(206,1,0)) :
                ((mEdgeFlags & EF_ONLINE) ? QColor(13, 0, 255) : QColor(13, 0, 255,70));

    painter->setPen(QPen(penColor,2.0,penStyle,Qt::RoundCap,Qt::RoundJoin));

#ifdef USE_NODE_ANIMATION
    if(!mNodeStart)
    {
        return;
    }

    QPointF pStart=QPointF(0.0,0.0);
    QPointF pEnd=mNodeEnd?mNodeEnd->pos():QPointF(0.0,0.0)-mNodeStart->pos();
	qreal tempscale=parentItem()?parentItem()->scale():scale();
	if(tempscale >1.0)
	{
		pEnd*=1.0/tempscale;
	}
    painter->drawLine(pStart,pEnd);
#else
    painter->drawLine(m_headPt,m_endPt);
#endif

    //draw indicator image
    if(mEdgeFlags & EF_ISWIRELESS)
    {
        QPixmap signalPixmap(getWirelessSignalImagePath((mEdgeFlags & EF_ONLINE) ? m_signalLevel : 0));
        QPointF tmpPt = boundingRect().center();

        tmpPt.setX(tmpPt.x() - signalPixmap.width() / 2);
        tmpPt.setY(tmpPt.y() - signalPixmap.height() / 2);

        painter->drawPixmap(tmpPt,signalPixmap);
    }
}
