#ifndef QGENIETOPOITEMS_H
#define QGENIETOPOITEMS_H

#include <QGraphicsItem>
#include <QObject>
#include <QtGui>
#include "QGenieRCTopologyObject.h"
class QGenieTopoEdge;
class QGenieTopoNode : public QGraphicsItem
{
public:
    explicit QGenieTopoNode(QGraphicsItem *parent = 0);
    ~QGenieTopoNode();




//    void setType(NODE_TYPE type);
    void setDevData(QSharedPointer<QGenieTopoDeviceCache> data);

protected:
//    enum NODE_FLAGS
//    {
//        NF_IS_INTERNETNODE=(1<<0)
//        ,NF_IS_ROUTERNODE=(1<<1)
//        ,NF_IS_GENIENODE=(1<<2)


//    };

    QRectF boundingRect() const;
    QPainterPath shape() const;

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0);
    void paintSubIcons(QPainter *painter,const QRectF &itemRect);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
protected:
//    QPainterPath mPainterPath;
//    QRectF mBoundingRect;
//    QString mTypePara;
//    quint32 mNodeFlags;
//    NODE_TYPE mType;
    QGenieTopoEdge*mEdge;
//public:
    QSharedPointer<QGenieTopoDeviceCache> mData;
    
signals:
//    void signal_wantShowDetail(QGenieTopoDeviceCache *cache);
public slots:
    friend class QGenieRCTopologyObject;
    friend class QGenieTopoEdge;
    
};

class _PixmapProvider{
public:
    enum PIXMAP_TYPE
    {
        PT_NORMAL
        ,PT_HOVERED
        ,PT_OFFLINE
        ,PT_DETAIL
        ,PT_NUM
    };
    enum SUBICO_TYPE
    {
        ST_VM
        ,ST_BLOCKED1
        ,ST_SIGNAL0
        ,ST_SIGNAL1
        ,ST_SIGNAL2
        ,ST_SIGNAL3
        ,ST_SIGNAL4
        ,ST_NUM
    };

    _PixmapProvider();
    ~_PixmapProvider();
    inline QPixmap &getPixMap(QGenieTopoDeviceCache::NODE_TYPE ntype,PIXMAP_TYPE ptype);
    inline QPixmap &getSubIcoPixmap(SUBICO_TYPE type);
    QPixmap * mPixMaps[QGenieTopoDeviceCache::NT_NUM][PT_NUM];


    QRectF &getBoundingRect(QGenieTopoDeviceCache::NODE_TYPE type);
    QRectF *mBoundingRects[QGenieTopoDeviceCache::NT_NUM];

    QPainterPath &getPainterPath(QGenieTopoDeviceCache::NODE_TYPE type);
    QPainterPath *mPainterPath[QGenieTopoDeviceCache::NT_NUM];

    /******************************************/
    QPixmap *mSubIcoPix[ST_NUM];

};

/***************************************************************
 *
 *
 */
class QGenieTopoEdge : public QGraphicsItem
{
public:
    QGenieTopoEdge(QGenieTopoNode *node);

    ~QGenieTopoEdge();
    void init();
protected:
    QRectF boundingRect() const;
    QPainterPath shape() const;

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0);
    QGenieTopoNode *mNode;
    friend class QGenieTopoNode;
};

#endif // QGENIETOPOITEMS_H
