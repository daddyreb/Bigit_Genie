#include "PowerlineNode.h"
#include "PowerlineEdge.h"
#include "PowerlineMapView.h"
#include "Powerline_mapuitilities.h"
#include "PowerlineDefine.h"
#include "PowerlineView.h"
#include "genie2_routeranim.h"

//#include "blockdevicemanager.h"
//#include "QGenieTopoNodePaintStrategy.h"
//#include "GeniePlugin_Map.h"


#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QFontMetrics>
#include <QPixmap>
#include <QtDebug>
#include <QtGui>
#include <QBitmap>
#include <QGraphicsSceneContextMenuEvent>


//#define GENIE2_RES(p) (p);

//BlockDeviceManager *TopologyNode::sBDM=NULL;
PowerlineNode::PowerlineNode()
    :m_isLocalHost(false)
    ,m_onLine(false)
    ,m_deviceType(-1)
    //,m_paintStrategy(0)
    ,m_isPowerLine(false)
    ,m_devAttCount(0)
	,m_nID(-1)
    ,m_isSonNodePaint(false)
    ,m_localDevice(false)
    ,m_bDownLeftBtn(false)
    ,m_bPicUseDefault(false)
    ,m_bDetailDlgParent(false)
	,m_localHostDevPc(false)
	,m_bLocalHostDevPcOn(false)
	,m_bHubView(false)
	,m_isTextShowUp(false)

{
    setAcceptHoverEvents(true);
    setFlag(ItemIsSelectable);
    setZValue(100.0);

}

PowerlineNode::~PowerlineNode()
{
    //    delete m_paintStrategy;
    //    m_paintStrategy = NULL;
}

QRectF PowerlineNode::boundingRect() const
{
    QString imagePath;
    QPixmap pixDetecter;

    switch(m_deviceType)
    {
	case GDT_POWERLINE:
		{
			QString tmpModelName = m_devModelName;
			QRegExp regx("v(\\d+)",Qt::CaseSensitive);
			int nPos = tmpModelName.lastIndexOf(regx);
			if(nPos > 0)
			{
				tmpModelName = tmpModelName.left(nPos);
			}
			/*imagePath = getPLDevTypeImagePath(m_bPicUseDefault ? "DEFAULT" : m_devModelName, m_devLedState ? PIS_ON : PIS_OFF, PIR_SMALL);*/
			//if(m_devModelName == "--")
			//{
			//	imagePath = getPLDevTypeImagePath(POWERLINE_UNKNOWN_DEV, PIS_NONE, PIR_SMALL);
			//}
			//else
			//{
				if(m_bDetailDlgParent)
				{
					imagePath = getDlgPLDevTypeImagePath(m_bPicUseDefault ? POWERLINE_DEFAULT_DEV : tmpModelName, m_devLedState);
				}
				else
				{
					imagePath = getPLDevTypeImagePath(m_bPicUseDefault ? POWERLINE_DEFAULT_DEV : tmpModelName, m_devLedState ? PIS_ON : PIS_OFF, PIR_SMALL);
				}
			//}
		}
		break;
	case GDT_HUB:
		{
			imagePath = getPLOtherDevImagePath(m_deviceType, PIS_NONE, PIR_SMALL);
		}
		break;
	case GDT_LOCALDEV:
	case GDT_STORAGE:
	case GDT_PRINTER:
	case GDT_SOUND:
	case GDT_UNKNOWN:
		{
			imagePath = getPLOtherDevImagePath(m_deviceType);
		}
		break;
	case GDT_INTERNET:
		{
			imagePath = getPLOtherDevImagePath(m_deviceType, PIS_ON);
		}
		break;
	case GDT_ROUTER:
		{
#ifdef TEST_PL_CODE
			imagePath = getPLOtherDevImagePath(m_deviceType);
#else
			QString model=GENIE2_HANDLER_CMD(HCMD_ASK_SUBSCRIBED_MESSAGE,SCCMD_ROUTER_MODEL).toString();

			QImageNameGengerator gener(model);
			imagePath = gener.getImageName(IE_NORMAL1);
#endif
		}
		break;
	case GDT_UNLOCKPIC:
	case GDT_LOCKPIC:
		{
			imagePath = getPLOtherDevImagePath(m_deviceType, PIS_NONE, PIR_SMALL);
		}
		break;
	default:
		break;
    };
    pixDetecter.load(imagePath);

    return QRectF(-pixDetecter.width() / 2, -pixDetecter.height() / 2, pixDetecter.width(), pixDetecter.height());
    
}

void PowerlineNode::updateShapeCache()
{
    QPainterPath    shapePath;
    QString         imagePath;
    QPixmap			pixDetecter;


    switch(m_deviceType)
    {
        case GDT_POWERLINE:
        {
			QString tmpModelName = m_devModelName;
			QRegExp regx("v(\\d+)",Qt::CaseSensitive);
			int nPos = tmpModelName.lastIndexOf(regx);
			if(nPos > 0)
			{
				tmpModelName = tmpModelName.left(nPos);
			}
            //imagePath = ":/PowerLineImage/PLineDev/XAV1004-OFF-L.png";
			//if(m_devModelName == "--")
			//{
			//	imagePath = getPLDevTypeImagePath(POWERLINE_UNKNOWN_DEV, PIS_NONE, PIR_SMALL);
			//}
			//else
			//{
				if(m_bDetailDlgParent)
				{
					imagePath = getDlgPLDevTypeImagePath(m_bPicUseDefault ? POWERLINE_DEFAULT_DEV : tmpModelName, m_devLedState);
				}
				else
				{
					imagePath = getPLDevTypeImagePath(m_bPicUseDefault ? POWERLINE_DEFAULT_DEV : tmpModelName, m_devLedState ? PIS_ON : PIS_OFF, PIR_SMALL);
				}
			//}

            //imagePath = getPLDevTypeImagePath(m_bPicUseDefault ? "DEFAULT" : m_devModelName, m_devLedState ? PIS_ON : PIS_OFF, PIR_SMALL);
            pixDetecter.load(imagePath);
            if(pixDetecter.isNull())
            {
                //imagePath = getPLDevTypeImagePath("DEFAULT", m_devLedState ? PIS_ON : PIS_OFF, PIR_SMALL);
				if(m_bDetailDlgParent)
				{
					imagePath = getDlgPLDevTypeImagePath(POWERLINE_DEFAULT_DEV, m_devLedState);
				}
				else
				{
					imagePath = getPLDevTypeImagePath(POWERLINE_DEFAULT_DEV, m_devLedState ? PIS_ON : PIS_OFF, PIR_SMALL);
				}
                m_bPicUseDefault = true;
            }

        }
            break;
		case GDT_HUB:
			{
				imagePath = getPLOtherDevImagePath(m_deviceType, PIS_NONE, PIR_SMALL);
			}
			break;
		case GDT_LOCALDEV:
		case GDT_STORAGE:
		case GDT_PRINTER:
		case GDT_SOUND:
		case GDT_UNKNOWN:
			{
				imagePath = getPLOtherDevImagePath(m_deviceType);
			}
			break;
		case GDT_INTERNET:
			{
				imagePath = getPLOtherDevImagePath(m_deviceType, PIS_ON);
			}
			break;
		case GDT_ROUTER:
			{
#ifdef TEST_PL_CODE
				imagePath = getPLOtherDevImagePath(m_deviceType);
#else
				QString model=GENIE2_HANDLER_CMD(HCMD_ASK_SUBSCRIBED_MESSAGE,SCCMD_ROUTER_MODEL).toString();

				QImageNameGengerator gener(model);
				imagePath = gener.getImageName(IE_NORMAL1);
#endif
			}
			break;
        case GDT_UNLOCKPIC:
        case GDT_LOCKPIC:
        {
            imagePath = getPLOtherDevImagePath(m_deviceType, PIS_NONE, PIR_SMALL);
        }
            break;
        default:
            break;
    };

    pixDetecter.load(imagePath);
    shapePath.addRegion(QRegion(pixDetecter.mask()));
    shapePath.translate(-pixDetecter.width() / 2,-pixDetecter.height() / 2);

    m_shapePath = shapePath;
}

QPainterPath PowerlineNode::shape() const
{
    return m_shapePath;
}
void PowerlineNode::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    emit nodeMouseOver(m_Idx);
    update();

    event->accept();
}
void PowerlineNode::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{

    emit nodeMouseOver(-1);

    update();
    event->accept();
}
void PowerlineNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{


    Q_UNUSED(widget)

    QRectF itemRect = boundingRect();

    QString imagePath;



    switch(m_deviceType)
    {
        case GDT_POWERLINE:
        {
            int pixmapRole	= PIR_SMALL;
            int pixmapState = PIS_ON;

			QString tmpModelName = m_devModelName;
			QRegExp regx("v(\\d+)",Qt::CaseSensitive);
			int nPos = tmpModelName.lastIndexOf(regx);
			if(nPos > 0)
			{
				tmpModelName = tmpModelName.left(nPos);
				qDebug() << "Chensi" <<tmpModelName;
			}
			//tmpModelName.replace(regx,"");


            if((option->state & QStyle::State_MouseOver) && !m_bDetailDlgParent && !m_bHubView)
            {
                pixmapRole = PIR_LARGE;
            }
            if(!m_devLedState)
            {
                pixmapState = PIS_OFF;
            }

			//if(m_devModelName != "--")
			//{
				if(m_bDetailDlgParent)
				{
					imagePath = getDlgPLDevTypeImagePath(m_bPicUseDefault ? POWERLINE_DEFAULT_DEV : tmpModelName, pixmapState);
				}
				else
				{
					imagePath = getPLDevTypeImagePath(m_bPicUseDefault ? POWERLINE_DEFAULT_DEV : tmpModelName, pixmapState, pixmapRole);
				}

	/*		}
			else
			{
				imagePath = getPLDevTypeImagePath(POWERLINE_UNKNOWN_DEV, PIS_NONE, pixmapRole);
			}*/

			/*imagePath = ":/PowerLineImage/PLineDev/XAV1004-OFF-L.png";*/


            QPixmap nodePixmap(imagePath);
            if(nodePixmap.isNull())
            {
                ////use default Device type image;
				if(m_bDetailDlgParent)
				{
					imagePath = getDlgPLDevTypeImagePath(POWERLINE_DEFAULT_DEV, pixmapState);
				}
				else
				{
					imagePath = getPLDevTypeImagePath(POWERLINE_DEFAULT_DEV, pixmapState, pixmapRole);
				}
                m_bPicUseDefault = true;
            }



            nodePixmap.load(imagePath);
            painter->drawPixmap(itemRect.topLeft(),nodePixmap);

            drawDevNum(painter, m_devModelName, 1);
        }
            break;
		case GDT_HUB:
			{
				int pixmapRole	= PIR_SMALL;

				if((option->state & QStyle::State_MouseOver) && m_attachUsbDev.count() > 1)
				{
					pixmapRole = PIR_LARGE;
				}

				imagePath = getPLOtherDevImagePath(m_deviceType, PIS_NONE, pixmapRole);
				QPixmap nodePixmap(imagePath);
				painter->drawPixmap(itemRect.topLeft(), nodePixmap);

				QString strType = PowerlineView::getDevStrType(m_deviceType);
				drawDevNum(painter, strType, 1);
			}
			break;
		case GDT_STORAGE:
		case GDT_PRINTER:
		case GDT_SOUND:
		case GDT_UNKNOWN:
			{
				imagePath = getPLOtherDevImagePath(m_deviceType);
				QPixmap nodePixmap(imagePath);
				painter->drawPixmap(itemRect.topLeft(), nodePixmap);

				QString strType = PowerlineView::getDevStrType(m_deviceType);
				drawDevNum(painter, strType, 1);
			}
			break;
		case GDT_LOCALDEV:
			{
				imagePath = getPLOtherDevImagePath(m_deviceType);
				QPixmap nodePixmap(imagePath);
				painter->drawPixmap(itemRect.topLeft(), nodePixmap);
			}
			break;
		case GDT_ROUTER:
			{
#ifdef TEST_PL_CODE
				imagePath = getPLOtherDevImagePath(m_deviceType);
				QPixmap nodePixmap(imagePath);
				painter->drawPixmap(itemRect.topLeft(), nodePixmap);
#else
				QString model=GENIE2_HANDLER_CMD(HCMD_ASK_SUBSCRIBED_MESSAGE,SCCMD_ROUTER_MODEL).toString();

				QImageNameGengerator gener(model);
				QPixmap nodePixmap(gener.getImageName(IE_NORMAL1));
#endif
				painter->drawPixmap(itemRect.topLeft(), nodePixmap);

			}
			break;
		case GDT_INTERNET:
			{

				int pixmapState = PIS_ON;
				if(!m_onLine)
				{
					pixmapState = PIS_OFF;
				}
				imagePath = getPLOtherDevImagePath(m_deviceType, pixmapState);
				QPixmap nodePixmap(imagePath);
				painter->drawPixmap(itemRect.topLeft(), nodePixmap);
			}
			break;
        case GDT_UNLOCKPIC:
        case GDT_LOCKPIC:
        {
            int pixmapRole	= PIR_SMALL;


            if(option->state & QStyle::State_MouseOver)
            {
                pixmapRole = PIR_LARGE;
            }

            imagePath = getPLOtherDevImagePath(m_deviceType, PIS_NONE, pixmapRole);

            QPixmap nodePixmap(imagePath);
            painter->drawPixmap(itemRect.topLeft(),nodePixmap);
        }
            break;
        default:
            break;
    };

    //if(m_devAttCount != 0)
    //{
    //    drawDevNum(painter, QString("%1").arg(m_devAttCount));
    //}

    //draw_end:
    //    if(mFlags & DF_CONNECTED)
    //    {
    //        painter->drawText(0,0,"CD");
    //    }
    //    else if(mFlags & DF_CONS)
    //    {
    //        painter->drawText(0, 0, "HE");
    //    }
}



void PowerlineNode::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_bDownLeftBtn = true;
    }

    QGraphicsItem::mousePressEvent(event);
}
void PowerlineNode::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    //qDebug () << "Clicked device " << m_name << "online: " << m_onLine;

    //emit clicked(this);
    if(m_bDownLeftBtn && m_deviceType == GDT_POWERLINE)
    {
        emit clicked(this);
        m_bDownLeftBtn  = false;
    }
    else if(m_deviceType == GDT_LOCKPIC || m_deviceType == GDT_UNLOCKPIC)
    {
        PowerlineMapView *pPowerline = PowerlineMapView::GetSingletonPtr();
        Q_ASSERT_X(pPowerline, "PowerlineNode.cpp", "pPowerline is NULL");
        pPowerline->showSecurity();
    }
    else
    {
        emit clicked(this);
    }
    QGraphicsItem::mouseReleaseEvent(event);
}
//void PowerlineNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
//{
//    if(m_deviceType == GDT_LOCKPIC || m_deviceType == GDT_UNLOCKPIC)
//    {
//        PowerlineMapView *pPowerline = PowerlineMapView::GetSingletonPtr();
//        Q_ASSERT_X(pPowerline, "PowerlineNode.cpp", "pPowerline is NULL");
//        pPowerline->showSecurity();
//    }
//    else
//    {
//        emit clicked(this);
//    }
//
//
//    QGraphicsItem::mouseReleaseEvent(event);
//}
void PowerlineNode::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if(m_deviceType == GDT_POWERLINE)
    {
        emit contextMenu(this,event->screenPos());
    }
    event->accept();
}




void PowerlineNode::drawDevNum(QPainter *painter, QString data , int nType, int pixrole)
{

    /////nType: 1 powerline
    Q_ASSERT(painter && "topologynode::drawDevNum");

    QRectF itemRect = boundingRect();




    QColor penColor =  Qt::black;
    painter->setPen(penColor);
    QFont font(painter->font());
    font.setPointSize(8);
    //#ifdef Q_OS_MACX
    //	font.setPointSize(10);
    //#else
    //	font.setPointSize(8);
    //#endif
    painter->setFont(font);
    QFontMetrics fm(font);
    QString labelText = data;
    //if(fm.width(labelText) > (itemRect.width() - 2*fm.width('X')))
    //{
    //    for(int k = 0 ; k < labelText.size() ; ++k)
    //    {
    //        if(fm.width(labelText.left(k)) + fm.width("...") >= (itemRect.width() - 2*fm.width('X')))
    //        {
    //            labelText = labelText.left(k) + "...";
    //            break;
    //        }
    //    }
    //}

    int width = fm.width(labelText);
    int x = itemRect.left() + (itemRect.width() - width) / 2;
    int nHeight = fm.height();

    if(nType == 0)
    {
		//其他设备
        
        //int tmp = labelText.toInt();
        QPixmap nodePixmap(":/PowerLineImage/NetworkDeviceDetailss_N2.png");


        QRectF rectTmp = QRectF(-nodePixmap.width() / 2, -nodePixmap.height() / 2, nodePixmap.width(), nodePixmap.height());
        x = (itemRect.right() - 22) + (nodePixmap.width() - width) / 2;

        //int tmpY = rectTmp.bottom() + (rectTmp.height() - fm.height()) / 2;
        if(pixrole == PIR_LARGE)
        {
            painter->drawPixmap(itemRect.right() - 20, itemRect.top() + 24, nodePixmap);
			QRect r(itemRect.right() - 20, itemRect.top() + 24,nodePixmap.size().width(),nodePixmap.size().height());
			painter->drawText(r, Qt::AlignCenter | Qt::AlignVCenter, labelText);
            //painter->drawText(x + 2, itemRect.top() + 23 +tmpY, labelText);
        }
        else
        {
            painter->drawPixmap(itemRect.right() - 22, itemRect.top() + 26, nodePixmap);
			QRect r(itemRect.right() - 22, itemRect.top() + 26,nodePixmap.size().width(),nodePixmap.size().height());
			painter->drawText(r, Qt::AlignCenter | Qt::AlignVCenter, labelText);
            //painter->drawText(x, itemRect.top() + 25 +tmpY, labelText);
        }

        
    }
    else if(nType == 1)
    {
		//Powerline
        QRectF tmpRect;
        //if(m_deviceType == GDT_NETWORKUSB)
        //{
        //    tmpRect = QRectF(itemRect.left() - 10, itemRect.bottom() - 15, itemRect.width() + 27, 3 * nHeight);
        //    painter->drawText(tmpRect, Qt::TextWordWrap, labelText);
        //}
        //else
        //{

		//if(m_isTextShowUp)
		//{
		//	painter->drawText(x, itemRect.top() - nHeight / 2, labelText);
		//}
		//else
		//{
		if(m_bDetailDlgParent)
		{
            painter->drawText(x, itemRect.bottom() - nHeight + 4, labelText);
		}
		else
		{
			painter->drawText(x, itemRect.bottom() + 8, labelText);
			if(!m_devName.isEmpty() && !m_bHubView)
			{
				int nameWidth	= fm.width(m_devName);
				int nameX		= itemRect.left() + (itemRect.width() - nameWidth) / 2;
				painter->drawText(nameX, itemRect.bottom() + 8 + nHeight, m_devName);
			}
		}
		//}
        //}

    }
	else if(nType == 2)
	{
		//Local Device PC
		QPixmap nodePixmap(":/PowerLineImage/OtherDev/Connected.png");
		if(m_bLocalHostDevPcOn)
		{
			painter->drawPixmap(itemRect.right() - 33, itemRect.bottom() - 46, nodePixmap);
		}
		
		//painter->drawText(x, itemRect.bottom() + 10, labelText);
	}
}



PowerlineNode &PowerlineNode::operator = (const PowerlineNode &node)
{
    if(this != &node)
    {
        m_isPowerLine		= node.m_isPowerLine;
        m_deviceType		= node.m_deviceType;
        m_isSonNodePaint	= node.m_isSonNodePaint;
        m_devMac			= node.m_devMac;
        m_devModelName		= node.m_devModelName;
        m_devFwVer			= node.m_devFwVer;
        m_devName			= node.m_devName;
        m_devType			= node.m_devType;
        m_devIp				= node.m_devIp;
        m_attachNetDev		= node.m_attachNetDev;
		m_attachUsbDev		= node.m_attachUsbDev;
        m_parent			= node.m_parent;
        m_son				= node.m_son;
        m_devLedState		= node.m_devLedState;
        m_devAttCount		= node.m_devAttCount;
        m_devSupportUSB		= node.m_devSupportUSB;
        m_devSupportWireless= node.m_devSupportWireless;
        m_localDevice		= node.m_localDevice;
        m_wlanInfo			= node.m_wlanInfo;
        m_Idx				= node.m_Idx;
		m_nID				= node.m_nID;
    }
    return *this;
}

























