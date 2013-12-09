#include "topologynode.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QFontMetrics>
#include <QPixmap>
#include <QtDebug>
#include <QtGui>
#include <QFont>
#include <QBitmap>
#include <QGraphicsSceneContextMenuEvent>

#include "QGenieTopoNodePaintStrategy.h"
#include "topologyview.h"
#include "topologyedge.h"

#include "mapdevicetype.h"
#include "mapuitilities.h"
#include "GeniePlugin_MapV2.h"
#include "blockdevicemanager.h"
#include "topology.h"
//add by caiyujun
#include "newfilenotifywidget.h"
//add end
extern quint32 sMapFlags;

BlockDeviceManager *TopologyNode::sBDM=NULL;
TopologyNode::TopologyNode():
    mNodeFlags(0),
	mDeviceCount(0),
//        m_isCentralRouter(false),
//        m_onLine(false),
//        m_isVm(false),
//#ifdef USE_RDP
//    mRdtEnabled(false),
//#endif
//#ifdef USE_PUTTY
//    mPuttyEnabled(false),
//#endif
//        m_blocked(false),
        m_deviceType(MDT_NETWORKDEV),
#ifdef POWERLINE_REVISION
        m_isPowerLine(false),
#endif
        m_paintStrategy(0)
#ifdef USE_NODE_ANIMATION
        ,mStartAngle(0.0)
        ,mEndAngle(0.0)
        ,mRadius(0.0)
        ,mNodeState(NS_INIT)
        ,mView(NULL)
        ,mCurve(QEasingCurve::OutBack)
#endif
{
    //add by caiyujun
    setAcceptDrops(true);
    //add end
    setAcceptHoverEvents(true);
    setFlag(ItemIsSelectable);
#ifdef USE_NODE_ANIMATION
    mEdge=new TopologyEdge(this);
    mEdge->setFlags(ItemStacksBehindParent);
    connect(&mDeleayTimer,SIGNAL(timeout()),this,SLOT(slot_DelayAddTimeout()));
    mScaleTimeLine.setEasingCurve(mCurve);
    mAngleTimeLine.setCurveShape(QTimeLine::LinearCurve);
#endif
    setZValue(100.0);
}

TopologyNode::~TopologyNode()
{
    delete m_paintStrategy;
}

QRectF TopologyNode::boundingRect() const
{
#ifdef PLUGIN_TARGET
    if(mNodeFlags & NF_ISCENTRALROUTER)
    {
        Q_ASSERT (m_paintStrategy);

        QSize size = m_paintStrategy->getSize();
        return QRectF(-size.width() / 2,-size.height() / 2,size.width(),size.height());
    }
    else
#endif
    {
        QString imagePath;
#ifdef POWERLINE_REVISION
        if(m_isPowerLine)
        {
            imagePath = GENIE2_RES("map/devices/PowerLineNormal.png");
        }
        else
#endif
        {
            imagePath = getDeviceTypeImagePath(m_deviceType,(mNodeFlags & NF_ISONLINE)?DTIR_NORMAL:DTIR_OFFLINE);
        }

        QPixmap pixDetecter(imagePath);
        return QRectF(-pixDetecter.width() / 2,-pixDetecter.height() / 2,pixDetecter.width(),pixDetecter.height());
    }
}

void TopologyNode::updateShapeCache()
{
    QPainterPath    shapePath;
    QString         imagePath;

#ifdef PLUGIN_TARGET
    if(mNodeFlags & NF_ISCENTRALROUTER)
    {
        Q_ASSERT (m_paintStrategy);
        imagePath = m_paintStrategy->getImageName();
    }
    else
#endif
    {
#ifdef POWERLINE_REVISION
        if(m_isPowerLine)
        {
            imagePath = GENIE2_RES("map/devices/PowerLineNormal.png");
        }
        else
#endif
        {
            imagePath = getDeviceTypeImagePath(m_deviceType,DTIR_NORMAL);
        }
    }
    QPixmap pixDetecter(imagePath);
    shapePath.addRegion(QRegion(pixDetecter.mask()));
    shapePath.translate(-pixDetecter.width() / 2,-pixDetecter.height() / 2);

    m_shapePath = shapePath;
}

//add by caiyujun
void TopologyNode::setDevCount(int count)
{
	mDeviceCount = count;
}

void TopologyNode::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    event->setAccepted(event->mimeData()->hasUrls());
    event->acceptProposedAction();
}

void TopologyNode::dropEvent(QGraphicsSceneDragDropEvent *event)
{

}

void TopologyNode::hoverEnterEvent( QGraphicsSceneHoverEvent * event)
{
   emit mouseInNode(true, this); 
}

void TopologyNode::hoverLeaveEvent( QGraphicsSceneHoverEvent * event)
{
    emit mouseInNode(false, this);
}
//add end

QPainterPath TopologyNode::shape() const
{
    return m_shapePath;
}

void TopologyNode::paint(QPainter *painter,
                         const QStyleOptionGraphicsItem *option,
                         QWidget *widget)
{
    Q_UNUSED(widget)

    //add by caiyujun
    if(option->state & QStyle::State_MouseOver)
    {
        emit mouseInNode(true, this);
    }
    //add end

    QRectF itemRect = boundingRect();

#ifdef POWERLINE_REVISION
    if(m_name == QString("POWERLINE"))
    {
        QString imagePath;
        if(option->state & QStyle::State_MouseOver)
        {
            imagePath = GENIE2_RES("map/devices/PowerLineSelected.png");
        }
        else
        {
            imagePath = GENIE2_RES("map/devices/PowerLineNormal.png");
        }

        QPixmap nodePixmap(imagePath);
        painter->drawPixmap(itemRect.topLeft(),nodePixmap);
        return;
    }
#endif

#ifdef PLUGIN_TARGET
    if((mNodeFlags & NF_ISCENTRALROUTER)||(m_deviceType==MDT_ROUTER))
    {
        Q_ASSERT (m_paintStrategy);
        m_paintStrategy->paint(painter,(option->state & QStyle::State_MouseOver),(mNodeFlags &NF_ISONLINE));
    }
    else
#endif
    {
        int pixmapRole = DTIR_NORMAL;

        if((option->state & QStyle::State_MouseOver) && (mNodeFlags & NF_ISONLINE) && (m_deviceType != MDT_INTERNET))
        {
            pixmapRole = DTIR_SELECTED;
        }
        else if(!(mNodeFlags&NF_ISONLINE))
        {
            pixmapRole = DTIR_OFFLINE;
        }

        QString pixmapPath;

        if(mNodeFlags & NF_ISLOCALHOST)
        {
            pixmapPath = getLocalHostImagePath(pixmapRole);
        }
        else if(mNodeFlags & NF_ISCENTRALROUTER)
        {
            pixmapPath = getCentralRouterImagePath(pixmapRole);
        }
        else
        {
            pixmapPath = getDeviceTypeImagePath(m_deviceType,pixmapRole);
        }
        QPixmap nodePixmap(pixmapPath);
        painter->drawPixmap(itemRect.topLeft(),nodePixmap);

    }

    //draw indicator pixmap

    paintSubIcons(painter,itemRect);

    //draw name
    if(!(mNodeFlags & NF_ISCENTRALROUTER) && (m_deviceType != MDT_INTERNET )
             &&(m_deviceType != MDT_ROUTER))
        {
        QColor penColor = ((mNodeFlags & NF_ISONLINE) ? ((option->state & QStyle::State_MouseOver) ? Qt::white : Qt::black) : QColor(204,204,204));

        painter->setPen(penColor);

        QFont font(painter->font());
#ifdef Q_OS_MACX
        font.setPointSize(10);
#else
        font.setPointSize(8);
#endif
        painter->setFont(font);

        QFontMetrics fm(font);

        QString labelText = m_name;

        if(fm.width(labelText) > (itemRect.width() - 2*fm.width('X')))
        {
            for(int k = 0 ; k < labelText.size() ; ++k)
            {
                if(fm.width(labelText.left(k)) + fm.width("...") >= (itemRect.width() - 2*fm.width('X')))
                {
                    labelText = labelText.left(k) + "...";
                    break;
                }
            }
        }

        int width = fm.width(labelText);
        int x = itemRect.left() + (itemRect.width() - width) / 2;

        painter->drawText(x, itemRect.top() + 62, labelText);
    }
}

void TopologyNode::paintSubIcons(QPainter *painter,const QRectF &itemRect)
{
    if(mNodeFlags & NF_ISVM)
    {
        QPixmap vmPixmap(getImagePath(MII_VMFLAG));
        painter->drawPixmap(itemRect.left() + itemRect.width() - vmPixmap.width() - 5
                            ,itemRect.top() + (itemRect.height() - vmPixmap.height()) / 2 , vmPixmap);
    }
    if(!sBDM)
    {
        sBDM=BlockDeviceManager::GetSingletonPtr();
    }
    if((mNodeFlags & NF_BLOCKED) && sBDM &&
       (sBDM->state()!=BLOCKDEVICE_FSM::FS_CLOSED
        &&sBDM->state()!=BLOCKDEVICE_FSM::FS_CLOSED_ADMIN)
       )
    {
        QPixmap vmPixmap(getImagePath(MII_BLOCKFLAG));
        painter->drawPixmap(itemRect.left() + /*itemRect.width() - vmPixmap.width() -*/+ 5
                            ,itemRect.top() + (itemRect.height() - vmPixmap.height()) / 2 , vmPixmap);

    }
	else
	{
		//add by caiyujun
		if (mNodeFlags & NF_ISTURBOONLINE)
		{
			QPixmap vmPixmap(getImagePath(MII_TURBOFLAG));
			painter->drawPixmap(itemRect.left() + itemRect.width() - vmPixmap.width() - 5
				,itemRect.top() -5   , vmPixmap);
		}
		//add end
	}

	//if (mDeviceCount > 0 && (mNodeFlags & (NF_ISCENTRALROUTER | NF_ISEXTENDER)))
	//{
	//	QPixmap vmPixmap(getImagePath(MII_DEVICECOUNT));
 //       painter->drawPixmap(itemRect.left() + itemRect.width() - vmPixmap.width() - 10
 //                           ,itemRect.top() -5   , vmPixmap);

	//	if (mDeviceCount >= 10)
	//	{
	//		QString strCount = QString::number(mDeviceCount);
	//		painter->drawText(itemRect.left() + itemRect.width() - vmPixmap.width() - 6
	//			,itemRect.top() + 8, strCount);
	//	}
	//	else
	//	{
	//		QString strCount = QString::number(mDeviceCount);
	//		painter->drawText(itemRect.left() + itemRect.width() - vmPixmap.width()
	//			,itemRect.top() + 8, strCount);
	//	}
	//}

    if(!(mNodeFlags & (NF_ISLOCALHOST|NF_ISCENTRALROUTER)))
    {
#ifdef USE_RDP
        if((mNodeFlags & NF_RDP_ENABLED) &&(sMapFlags & MF_USE_RDP_SCAN))
        {

            QPixmap vmPixmap(getImagePath(MII_RDPFLAG));
            painter->drawPixmap(itemRect.left() + itemRect.width()/2-vmPixmap.width()/2
                                ,itemRect.top() + (itemRect.height() - vmPixmap.height()) / 2 , vmPixmap);

        }
        else
        {
#endif

#ifdef USE_PUTTY
        if((mNodeFlags & NF_PUTTY_ENABLED) && (sMapFlags & MF_USE_PUTTY_SCAN))
        {
            QPixmap vmPixmap(getImagePath(MII_PUTTYFLAG));
            painter->drawPixmap(itemRect.left() + itemRect.width()/2-vmPixmap.width()/2
                                ,itemRect.top() + (itemRect.height() - vmPixmap.height()) / 2 , vmPixmap);

        }
#endif

#ifdef USE_RDP
        }
#endif
    }
//    if((false
//#ifdef USE_RDP
//        ||  ((mNodeFlags & NF_RDP_ENABLED) &&(sMapFlags & MF_USE_RDP_SCAN))
//#endif
//#ifdef USE_PUTTY
//        || ((mNodeFlags & NF_PUTTY_ENABLED) && (sMapFlags & MF_USE_PUTTY_SCAN))
//#endif
//        )&&  !(mNodeFlags & (NF_ISLOCALHOST|NF_ISCENTRALROUTER)))
//    {
//        int img_idx=-1;
//#ifdef USE_RDP
//         img_idx=MII_RDPFLAG;
//#endif
//#ifdef USE_PUTTY
//        img_idx=MII_PUTTYFLAG;
//#endif
//        Q_ASSERT(img_idx != -1);
//        QPixmap vmPixmap(getImagePath(img_idx));
//        painter->drawPixmap(itemRect.left() + itemRect.width()/2-vmPixmap.width()/2
//                            ,itemRect.top() + (itemRect.height() - vmPixmap.height()) / 2 , vmPixmap);

//    }

}

//must be called after the m_routerModel have a valid value
void TopologyNode::preparePaintStrategy()
{
    if(m_paintStrategy)
    {
        delete m_paintStrategy;
        m_paintStrategy = 0;
    }

//    m_routerModel = "WNDR4300";//yankai modify ,only to test
    m_paintStrategy = new QGenieTopoNodePaintStrategy(AT_MAP,NULL,m_routerModel);
}

void TopologyNode::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    emit clicked(this);

    QGraphicsItem::mouseReleaseEvent(event);
}

void TopologyNode::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    emit contextMenu(this,event->screenPos());
    event->accept();
}

#ifdef USE_NODE_ANIMATION
//void TopologyNode::setScaleAnimTo(qreal scale)
//{
//    mScaleTimeLine.start();

//}

qreal TopologyNode::setAngleAnimTo(qreal angle)
{
    qreal ret=qAbs(mEndAngle-angle);
    mStartAngle=mStartAngle+(mEndAngle-mStartAngle)*mAngleTimeLine.currentValue();
    mEndAngle=angle;
    mAngleTimeLine.stop();
    mAngleTimeLine.start();
    mRadius=ELLIPSE_RADIUS;
    return ret;
}

#ifdef REBOUNCE_FOR_RAISE
void TopologyNode::reBounce()
{
    setVisible(true);
    setScale(0.01);
    mScaleTimeLine.stop();
    mScaleTimeLine.start();
}
#endif

bool TopologyNode::animFrame()
{
    setScale(mScaleTimeLine.currentValue());
    qreal angle=mStartAngle+(mEndAngle-mStartAngle)*mAngleTimeLine.currentValue();

    const qreal PI = 3.14159265358979323846;
    angle=angle*PI/180;
    qreal x = mRadius * cos(angle);
    qreal y = mRadius*ELLIPSE_FORUM * sin(angle);

    this->setPos(x,y);

    return QTimeLine::Running==mScaleTimeLine.state()
            ||QTimeLine::Running==mAngleTimeLine.state();


}

void TopologyNode::addToView(TopologyView *view, qreal angle, qreal radius,int delay)
{

    view->m_scene->addItem(this);
    mView=view;
    //    setScale(0.001);
    mStartAngle=mEndAngle=angle;
    mRadius=radius;
    mNodeState=NS_INSCREEN;
    //mScaleTimeLine.start();
    //if(!view->mAnimTimer.isActive())
    //{
    //    view->mAnimTimer.start(100);
    //}

    mDeleayTimer.setSingleShot(true);
    mDeleayTimer.start(delay);
    setVisible(false);
    //    QTimer::singleShot(delay,this,SLOT(slot_DelayAddTimeout()));
}

void TopologyNode::delFromView(TopologyView *view)
{
    mDeleayTimer.stop();
    //        disconnect(node,SIGNAL(clicked(TopologyNode*)),0,0);
    //        m_scene->removeItem(node);
    //  m_scene->removeItem(edge);
    view->mBackGroundNodes.insert(this);
    view->m_scene->removeItem(this);
    mNodeState=TopologyNode::NS_BEHIND;
    mScaleTimeLine.stop();
    mAngleTimeLine.stop();
    if(m_paintStrategy)
    {
        delete m_paintStrategy;
        m_paintStrategy=NULL;
    }
    setVisible(false);
}

void TopologyNode::slot_DelayAddTimeout()
{ 
    //    Q_ASSERT(!isVisible());
    setScale(0.01);
    setVisible(true
#ifdef REBOUNCE_FOR_RAISE
               && (sMapFlags & MF_ONSTACK_TOP)
#endif
               );

    mScaleTimeLine.start();
    if(mView &&!mView->mAnimTimer.isActive())
    {
        mView->mAnimTimer.start(100);
    }
}

#endif
