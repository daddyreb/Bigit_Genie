#include "QGenieTopoItems.h"
#include "genie2_interface.h"
#include "QGenieRCTopologyObject.h"
#include <QtGui>
#include <QtCore>

static _PixmapProvider sProvider;


static int parseDeivceSignalLevel(int signal)
{
    int signalLevel = 4;


    if((signal >= 0) && (signal <= 100))
    {
        if(signal < 20)
        {
            signalLevel = 1;
        }
        else if(signal >= 20 && signal < 50)
        {
            signalLevel = 2;
        }
        else if(signal >= 50 && signal < 70)
        {
            signalLevel = 3;
        }
        else
        {
            signalLevel = 4;
        }
    }


    return signalLevel;
}

_PixmapProvider::_PixmapProvider()
{
    memset(&mPixMaps,'\0',sizeof(mPixMaps));

    memset(&mBoundingRects,'\0',sizeof(mBoundingRects));
    memset(&mPainterPath,'\0',sizeof(mPainterPath));
    memset(&mSubIcoPix,'\0',sizeof(mSubIcoPix));
}

_PixmapProvider::~_PixmapProvider()
{
    for(int i=0;i<QGenieTopoDeviceCache::NT_NUM;i++)
    {
        for(int j=0;j<PT_NUM;j++)
        {
            GENIE2_SAFEDELETE(mPixMaps[i][j]);
        }

        GENIE2_SAFEDELETE(mBoundingRects[i]);
        GENIE2_SAFEDELETE(mPainterPath[i]);
    }
    for(int i=0;i<ST_NUM;i++)
    {
        GENIE2_SAFEDELETE(mSubIcoPix[i]);
    }

}

QPixmap &_PixmapProvider::getPixMap(QGenieTopoDeviceCache::NODE_TYPE ntype, PIXMAP_TYPE ptype)
{
    if(mPixMaps[ntype][ptype]==NULL)
    {
        static const char *cParas[QGenieTopoDeviceCache::NT_NUM][PT_NUM]={
            {"InternetNormal","InternetNormal","InternetOffline",""}
            ,{"CentralRouterNormal","CentralRouterSelected","CentralRouterOffline","CentralRouterDetails"}
    #ifdef Q_OS_WIN32
            ,{"LocalHostNormal","LocalHostSelected","LocalHostOffline","LocalHostDetails"}
    #else
            ,{"LocalHostNormal_Mac","LocalHostSelected_Mac","LocalHostOffline_Mac","LocalHostDetails_Mac"}
    #endif
            ,{"NetworkDeviceNormal","NetworkDeviceSelected","NetworkDeviceOffline","NetworkDeviceDetails"}
            ,{"WindowsPCNormal","WindowsPCSelected","WindowsPCOffline","WindowsPCDetails"}
            ,{"MacBookNormal","MacBookSelected","MacBookOffline","MacBookDetails"}
            ,{"iMacNormal","iMacSelected","iMacOffline","iMacDetails"}
            ,{"MacMiniNormal","MacMiniSelected","MacMiniOffline","MacMiniDetails"}
            ,{"MacProNormal","MacProSelected","MacProOffline","MacProDetails"}
            ,{"LinuxPCNormal","LinuxPCSelected","LinuxPCOffline","LinuxPCDetails"}
            ,{"UnixPCNormal","UnixPCSelected","UnixPCOffline","UnixPCDetails"}
            ,{"GamingConsoleNormal","GamingConsoleSelected","GamingConsoleOffline","GamingConsoleDetails"}
            ,{"MediaDeviceNormal","MediaDeviceSelected","MediaDeviceOffline","MediaDeviceDetails"}
            ,{"CameraNormal","CameraSelected","CameraOffline","CameraDetails"}
            ,{"SmartPhoneNormal","SmartPhoneSelected","SmartPhoneOffline","SmartPhoneDetails"}
            ,{"TabletNormal","TabletSelected","TabletOffline","TabletDetails"}
            ,{"PrinterNormal","PrinterSelected","PrinterOffline","PrinterDetails"}
            ,{"ScannerNormal","ScannerSelected","ScannerOffline","ScannerDetails"}
            ,{"StorageNormal","StorageSelected","StorageOffline","StorageDetails"}
            ,{"RouterNormal","RouterSelected","RouterOffline","RouterDetails"}
            ,{"iPadNormal","iPadSelected","iPadOffline","iPadDetails"}
            ,{"iPodTouchNormal","iPodTouchSelected","iPodTouchOffline","iPodTouchDetails"}
            ,{"iPhoneNormal","iPhoneSelected","iPhoneOffline","iPhoneDetails"}
            ,{"BridgeNormal","BridgeSelected","BridgeOffline","BridgeDetails"}
            ,{"RepeaterNormal","RepeaterSelected","RepeaterOffline","RepeaterDetails"}
            ,{"CableSTBNormal","CableSTBSelected","CableSTBOffline","CableSTBDetails"}
            ,{"SatelliteNormal","SatelliteSelected","SatelliteOffline","SatelliteDetails"}
            ,{"SlingBoxNormal","SlingBoxSelected","SlingBoxOffline","SlingBoxDetails"}
            ,{"OtherSTBNormal","OtherSTBSelected","OtherSTBOffline","OtherSTBDetails"}
            ,{"TVNormal","TVSelected","TVOffline","TVDetails"}
            ,{"SwitchNormal","SwitchSelected","SwitchOffline","SwitchDetails"}
            ,{"AndroidTabletNormal","AndroidTabletSelected","AndroidTabletOffline","AndroidTabletDetails"}
            ,{"AndroidPhoneNormal","AndroidPhoneSelected","AndroidPhoneOffline","AndroidPhoneDetails"}
            ,{"AndroidDeviceNormal","AndroidDeviceSelected","AndroidDeviceOffline","AndroidDeviceDetails"}
            ,{"Blu-rayPlayerNormal","Blu-rayPlayerSelected","Blu-rayPlayerOffline","Blu-rayPlayerDetails"}
            ,{"DVRNormal","DVRSelected","DVROffline","DVRDetails"}
            ,{"PowerlineNormal","PowerlineSelected","PowerlineOffline","PowerlineDetails"}
        };
        for(int i=0;i<PT_NUM;i++)
        {
            if(mPixMaps[ntype][i]==NULL)
            {
                mPixMaps[ntype][i]=new QPixmap(GENIE2_RES("map/devices")+"/"+cParas[ntype][i]+".png");
            }
        }
    }
    return *(mPixMaps[ntype][ptype]);
}

QPixmap &_PixmapProvider::getSubIcoPixmap(SUBICO_TYPE type)
{
    if(mSubIcoPix[type]==NULL)
    {
        const char *cParas[]={"VmFlag","BlockFlag","WirelessSignal_0","WirelessSignal_1"
                              ,"WirelessSignal_2","WirelessSignal_3","WirelessSignal_4"};
        mSubIcoPix[type]=new QPixmap(GENIE2_RES("map/others")+"/"+cParas[type]+".png");
    }
    return *(mSubIcoPix[type]);
}

QRectF &_PixmapProvider::getBoundingRect(QGenieTopoDeviceCache::NODE_TYPE type)
{
    if(NULL==mBoundingRects[type])
    {
        QPixmap &pix=getPixMap(type,PT_NORMAL);
        mBoundingRects[type]=new QRectF(-pix.width()/2,-pix.height()/2
                                        ,pix.width(),pix.height()) ;
    }
    return *(mBoundingRects[type]);
}

QPainterPath &_PixmapProvider::getPainterPath(QGenieTopoDeviceCache::NODE_TYPE type)
{
    if(NULL == mPainterPath[type])
    {
        mPainterPath[type]=new QPainterPath();
        QPixmap &pix=getPixMap(type,PT_NORMAL);

        mPainterPath[type]->addRegion(QRegion(pix.mask()));
        mPainterPath[type]->translate(-pix.width()/2,-pix.height()/2);

    }
    return *(mPainterPath[type]);
}


QGenieTopoNode::QGenieTopoNode(QGraphicsItem *parent)
    :QGraphicsItem(parent)
//    ,mNodeFlags(0)
//    ,mType(NT_NETWORKDEVICE)
{
    setAcceptHoverEvents(true);
    setFlag(ItemIsSelectable);
	mEdge=new QGenieTopoEdge(this);
    mEdge->hide();
}

QGenieTopoNode::~QGenieTopoNode()
{
    mData.clear();
    GENIE2_SAFEDELETE(mEdge);
}

QRectF QGenieTopoNode::boundingRect() const
{
    return sProvider.getBoundingRect(mData.isNull()?QGenieTopoDeviceCache::NT_NETWORKDEVICE:mData->mType);
}
QPainterPath QGenieTopoNode::shape() const
{
    return sProvider.getPainterPath(mData.isNull()?QGenieTopoDeviceCache::NT_NETWORKDEVICE:mData->mType);
}

void QGenieTopoNode::paintSubIcons(QPainter *painter,const QRectF &itemRect)
{
    if(mData.isNull())
    {
        return;
    }
    if(mData->mFlags & QGenieTopoDeviceCache::DCF_ISVM)
    {
        const QPixmap &pix=sProvider.getSubIcoPixmap(_PixmapProvider::ST_VM);
        painter->drawPixmap(itemRect.left() + itemRect.width() - pix.width() - 5
                            ,itemRect.top() + (itemRect.height() - pix.height()) / 2
                            , pix);
    }

    if(mData->mFlags & QGenieTopoDeviceCache::DCF_BLOCKED)
    {
        const QPixmap &pix=sProvider.getSubIcoPixmap(_PixmapProvider::ST_BLOCKED1);
        painter->drawPixmap(itemRect.left() + /*itemRect.width() - vmPixmap.width() -*/+ 5
                            ,itemRect.top() + (itemRect.height() - pix.height()) / 2
                            ,pix );
    }
}

void QGenieTopoNode::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
//    emit signal_wantShowDetail(mData.data());
    QGenieRCTopoDetailViewManager *dvm=QGenieRCTopoDetailViewManager::GetSingletonPtr();
    Q_ASSERT(dvm);
    dvm->slot_startShowDetail(mData.data());
    QGraphicsItem::mouseReleaseEvent(event);
}

void QGenieTopoNode::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QGenieRCTopoDetailViewManager *dvm=QGenieRCTopoDetailViewManager::GetSingletonPtr();
    Q_ASSERT(dvm);
    dvm->slot_startShowDetail(mData.data());
    QGraphicsItem::contextMenuEvent(event);
}

void QGenieTopoNode::paint(QPainter *painter,
           const QStyleOptionGraphicsItem *option,
           QWidget *widget )
{
    Q_UNUSED(widget);
//    painter->setPen(Qt::red);
//    painter->drawRect(-5,-5,10,10);

//painter->setPen(Qt::red);
//painter->setBrush(Qt::red);
//    painter->drawPath(shape());
//return;

    QGenieTopoDeviceCache::NODE_TYPE type=mData.isNull()?QGenieTopoDeviceCache::NT_NETWORKDEVICE:mData->mType;
        Q_ASSERT(type==QGenieTopoDeviceCache::NT_CENTERROUTER||type==QGenieTopoDeviceCache::NT_INTERNET|| !mData.isNull());
    QRectF itemRect = boundingRect();
    QPixmap &pix=sProvider.getPixMap(type,(option->state & QStyle::State_MouseOver)
                                     ?_PixmapProvider::PT_HOVERED:_PixmapProvider::PT_NORMAL);
    painter->drawPixmap(itemRect.topLeft(),pix);


    //start draw text
	if(type!=QGenieTopoDeviceCache::NT_CENTERROUTER&&type!=QGenieTopoDeviceCache::NT_INTERNET)
    {
        QFont font(painter->font());
#ifdef Q_OS_MACX
        font.setPointSize(10);
#else
        font.setPointSize(8);
#endif
        painter->setFont(font);

        QFontMetrics fm(font);

        QString labelText = mData->mName.isEmpty()?mData->mIp:mData->mName;

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

        paintSubIcons(painter,itemRect);
    }
    //end draw text

}

//void QGenieTopoNode::setType(NODE_TYPE type)
//{
//    mType=type;
//}

void QGenieTopoNode::setDevData(QSharedPointer<QGenieTopoDeviceCache> data)
{
    mData=data;
    if(!mData.isNull())
    {
        setToolTip(mData->mIp);
    }
    mEdge->init();
}

/**************************************************************************
 ****************************************************************************
 ****************************************************************************
 ****************************************************************************
 ****************************************************************************
 ****************************************************************************
 ****************************************************************************
 ****************************************************************************/
QGenieTopoEdge::QGenieTopoEdge(QGenieTopoNode *node)
    :QGraphicsItem(node)
    ,mNode(node)
{
    Q_ASSERT(node);
    init();
//    this->setZValue(-100.0);
    this->setFlags(ItemStacksBehindParent);

}

void QGenieTopoEdge::init()
{
    QSharedPointer<QGenieTopoDeviceCache > data=mNode->mData;
    if(!data.isNull()&&data->mType==QGenieTopoDeviceCache::NT_CENTERROUTER)
    {
        hide();
    }
    else
    {
        show();
        QString toolTip;
        if(!data.isNull()&&!(data->mFlags&QGenieTopoDeviceCache::DCF_WIRED))
        {


            if(data->mFlags&QGenieTopoDeviceCache::DCF_HAVE_SIGNAL_DATA)
            {
                toolTip += "<span>";
                toolTip += QString("Signal Strength:");
                toolTip += " ";
                toolTip += QString::number(data->mSignalStrength)+"%";
                toolTip += "</span>";
            }

            if(data->mFlags&QGenieTopoDeviceCache::DCF_HAVE_LINK_RATE)
            {
                if(!toolTip.isEmpty())
                {
                    toolTip += "<br>";
                }
                toolTip += "<span>";
                toolTip += QString("Link Rate:");
                toolTip += " ";
                toolTip += data->mLinkSpeed;
                toolTip += "</span>";
            }


        }
        setToolTip(toolTip);
    }

}

QGenieTopoEdge::~QGenieTopoEdge()
{

}

QRectF QGenieTopoEdge::boundingRect() const
{
    QPointF pStart=QPointF(0.0,0.0);
    QPointF pEnd=-mNode->pos();
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
}

QPainterPath QGenieTopoEdge::shape() const
{
//	qDebug()<<"QGenieTopoEdge::shape()";
    QPainterPath shapePath;




    QPointF point=-mNode->pos()/2;
//    shapePath.moveTo(QPointF(0.0,0.0));
//    shapePath.lineTo(point);

    QSharedPointer<QGenieTopoDeviceCache > data=mNode->mData;
    if(!data.isNull()
            &&(data->mFlags&QGenieTopoDeviceCache::DCF_HAVE_SIGNAL_DATA)
            &&!(data->mFlags&QGenieTopoDeviceCache::DCF_WIRED)
            )
    {
        QPixmap &pix=sProvider.getSubIcoPixmap(_PixmapProvider::ST_SIGNAL0);
        QRectF rect=pix.rect();
        rect.moveTo(point.x()-pix.width()/2,point.y()-pix.height()/2);
        shapePath.addRect(rect);
    }


    return shapePath;
}

void QGenieTopoEdge::paint(QPainter *painter,
           const QStyleOptionGraphicsItem *option,
           QWidget *widget )
{
    Q_UNUSED(widget)
    Q_UNUSED(option)

//    painter->drawPath(shape());
//return;
    QSharedPointer<QGenieTopoDeviceCache > data=mNode->mData;
    if(/*true ||*/data.isNull())
    {
        painter->setBrush(Qt::red);
        painter->fillRect(boundingRect(),Qt::SolidPattern);
        return;
    }

    Qt::PenStyle penStyle =(!(data->mFlags&QGenieTopoDeviceCache::DCF_WIRED)&&(data->mType!=QGenieTopoDeviceCache::NT_INTERNET))
            ? Qt::DotLine : Qt::SolidLine;

    QGenieTopoDeviceCache::NODE_TYPE type=data.isNull()
            ?QGenieTopoDeviceCache::NT_NETWORKDEVICE
           :data->mType;
    QColor penColor = (type == QGenieTopoDeviceCache::NT_INTERNET) ?
                 QColor(83,166,73):QColor(13, 0, 255);

    painter->setPen(QPen(penColor,2.0,penStyle,Qt::RoundCap,Qt::RoundJoin));


    QPointF pStart=QPointF(0.0,0.0);
    QPointF pEnd=-mNode->pos();

    painter->drawLine(pStart,pEnd);


    //draw indicator image
    if( !(data->mFlags&QGenieTopoDeviceCache::DCF_WIRED)
         &&  (data->mType!=QGenieTopoDeviceCache::NT_INTERNET)
            )
    {
        _PixmapProvider::SUBICO_TYPE type=(_PixmapProvider::SUBICO_TYPE)(_PixmapProvider::ST_SIGNAL0+parseDeivceSignalLevel(data->mSignalStrength));
        QPixmap &signalPixmap=sProvider.getSubIcoPixmap(type);
        QPointF tmpPt = boundingRect().center();

        tmpPt.setX(tmpPt.x() - signalPixmap.width() / 2);
        tmpPt.setY(tmpPt.y() - signalPixmap.height() / 2);

        painter->drawPixmap(tmpPt,signalPixmap);
    }

}


