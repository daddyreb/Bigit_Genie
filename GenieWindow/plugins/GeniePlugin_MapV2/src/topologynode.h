#ifndef TOPOLOGYNODE_H
#define TOPOLOGYNODE_H

#include <QGraphicsItem>
//#include <QString>
//#include <QPixmap>
//#include <QObject>
//#include <QTimer>
#include <QtCore>
#include <QPoint>
#include <QPainterPath>
//add by caiyujun
#include <QDragEnterEvent>
//add end

#ifdef USE_NODE_ANIMATION
#define ELLIPSE_RADIUS 208.0
#define ELLIPSE_FORUM (111.0/208.0)
#endif

class QGraphicsSceneMouseEvent;
class QGraphicsSceneContextMenuEvent;
class QGenieTopoNodePaintStrategy;
class TopologyView;
class TopologyEdge;
class BlockDeviceManager;

class TopologyNode : public QObject, public QGraphicsItem
{
    friend class TopologyView;

    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    TopologyNode();
    ~TopologyNode();
    enum TOPONODE_FLAGS
    {
        NF_ISLOCALHOST=(1<<0)
        ,NF_ISCENTRALROUTER=(1<<1)
        ,NF_ISONLINE=(1<<2)
        ,NF_ISVM=(1<<3)
        ,NF_BLOCKED=(1<<4)
        ,NF_RDP_ENABLED=(1<<5)
        ,NF_PUTTY_ENABLED=(1<<6)
        ,NF_ISPOWERLINE=(1<<7)
        //add by caiyujun
        ,NF_ISTURBOONLINE=(1<<8)
		,NF_ISEXTENDER=(1<<9)
        //add end
    };

protected:
    quint32 mNodeFlags;
//    bool    m_isLocalHost;
//    bool    m_isCentralRouter;
//    bool    m_onLine;
//    bool    m_isVm;
//    bool    m_blocked;
    int     m_deviceType;
//#ifdef USE_RDP
//    bool    mRdtEnabled;
//#endif
//#ifdef USE_PUTTY
//    bool    mPuttyEnabled;
//#endif
#ifdef POWERLINE_REVISION
    bool    m_isPowerLine;
#endif

    QString m_name;
    QString m_routerModel;

protected:
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    //add by caiyujun
    virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    virtual void dropEvent(QGraphicsSceneDragDropEvent *event);
    virtual void hoverEnterEvent( QGraphicsSceneHoverEvent * event);
    virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent * event);
    //add end

public:
    QRectF boundingRect() const;
    QPainterPath shape() const;

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0);


    void paintSubIcons(QPainter *painter, const QRectF &itemRect);

private:
    QPainterPath    m_shapePath;
public:
    void updateShapeCache();

	//add by caiyujun
	void setDevCount(int count);
	//add end

private:
    QGenieTopoNodePaintStrategy *m_paintStrategy;
public:
    void preparePaintStrategy();

signals:
    void clicked(TopologyNode *);
    void contextMenu(TopologyNode * ,const QPoint &pos);
    //add by caiyujun
    void mouseInNode(bool isIn,TopologyNode *);
    //add end

#ifdef USE_NODE_ANIMATION
public:
    enum _NODE_STATE
    {
        NS_INIT,
        NS_INSCREEN,
        NS_FADEOUT,
        NS_BEHIND
    };

//    void setScaleAnimTo(qreal scale);
    qreal setAngleAnimTo(qreal angle);
    bool animFrame();
    void addToView(TopologyView *view,qreal angle,qreal radius=ELLIPSE_RADIUS,int delay=0);
    void delFromView(TopologyView *view);
    TopologyView *mView;
    QString mMac;
    quint32 mIp;
	//add by caiyujun
	int mDeviceCount;
	//add end
    int mNodeState;
    TopologyEdge *mEdge;
    QTimer mDeleayTimer;
#ifdef REBOUNCE_FOR_RAISE	
	void reBounce();
#endif	

protected:
    QEasingCurve mCurve;
    QTimeLine mScaleTimeLine;
    QTimeLine mAngleTimeLine;
    qreal mStartAngle;
    qreal mEndAngle;
    qreal mRadius;
    static BlockDeviceManager *sBDM;
protected slots:
    void slot_DelayAddTimeout();

#endif
};

#endif // TOPOLOGYNODE_H
