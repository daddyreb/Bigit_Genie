#ifndef TOPOLOGYEDGE_H
#define TOPOLOGYEDGE_H

#include <QGraphicsItem>
#include <QPointF>
#include <QString>

class TopologyNode;

class TopologyEdge : public QGraphicsItem
{
    friend class TopologyView;
public:
#ifdef USE_NODE_ANIMATION
    TopologyEdge(TopologyNode *nodeStart,TopologyNode *nodeEnd=0);
#else
    TopologyEdge();
#endif
    enum TOPYEDGE_FLAGS
    {
        EF_ONLINE=(1<<0)
        ,EF_ISLOCALHOST=(1<<1)
        ,EF_ISWIRELESS=(1<<2)
    };

protected:
    quint32 mEdgeFlags;
//    bool            m_onLine;
//    bool            m_isLocalHost;
//    bool            m_wireless;
    int             m_deviceType;
    int             m_signalLevel;

    QString         m_signalStrength;
    QString         m_linkRate;
protected:
    //ui relative
#ifdef USE_NODE_ANIMATION
    TopologyNode *mNodeStart;
    TopologyNode *mNodeEnd;
#else
    QPointF         m_headPt;
    QPointF         m_endPt;
#endif

private:
    QRectF calSignalPixmapRect() const;
public:
    QRectF boundingRect() const;
    QPainterPath shape() const;

public:
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0);
};

#endif // TOPOLOGYEDGE_H
