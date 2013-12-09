#include "topologyview.h"


#include <QGraphicsScene>
#include <QtGlobal>
#include <math.h>
#include <QtDebug>
#include <QTimerEvent>
#include <QEvent>


#include "device.h"
#include "mapdevicetype.h"
#include "topologynode.h"
#include "topologyedge.h"
#include "mapuitilities.h"
#include "topology.h"
#include "language_Map.h"
#include "blockdevicemanager.h"



#define ROUTER_ANIMATION_INTERVAL 500
#define ANGLE_DELAY_THRESHOLD 30.0
#define TOPOLOGYVIEW_STYLE "QGraphicsView{background-color:transparent;border: 0px;}"
extern quint32 sMapFlags;

inline void checkFlags(bool b,quint32 &flags,quint32 checkFlag)
{
	if(b)
	{
		flags |=checkFlag;
	}
	else
	{
		flags &= ~checkFlag;
	}
}

TopologyView::TopologyView(QWidget *parent) :
        QGraphicsView(parent),
        m_scene(0),
        m_InternetNode(0),
        m_InternetEdge(0),
//        m_internetConnected(true),
        m_routerRefreshTimerId(0)
{
    setViewportUpdateMode(FullViewportUpdate);

    setAttribute(Qt::WA_TranslucentBackground);
    setRenderHint(QPainter::Antialiasing);
    setCacheMode(QGraphicsView::CacheBackground);
    //add by caiyujun
    setAcceptDrops(true);
    //add end

    setStyleSheet(QString(TOPOLOGYVIEW_STYLE));

    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(QRectF(-257.443,-37.3571 ,515.985,74.7142) );
    m_scene->setItemIndexMethod(QGraphicsScene::NoIndex);

	/////////////2012-3-13 add///////////////////
	//this->setFrameShape(QFrame::NoFrame);
	//this->setFrameShadow(QFrame::Plain);
	//this->setAttribute(Qt::WA_TranslucentBackground);


#ifdef USE_NODE_ANIMATION
    connect(&mAnimTimer,SIGNAL(timeout()),this,SLOT(slot_animTimout()));
#endif

    this->setScene(m_scene);
}

TopologyView::~TopologyView()
{
#ifdef USE_NODE_ANIMATION
    mAnimTimer.stop();
#endif
    foreach(TopologyNode *node,mAllNodes)
    {
        delete node;
    }

    if(m_routerRefreshTimerId)
    {
        killTimer(m_routerRefreshTimerId);
    }
}

QString TopologyView::verifyNodeName(const Device &dev)
{
    if(!dev[DP_CUSTOMNAME].toString().isEmpty())
    {
        return dev[DP_CUSTOMNAME].toString();
    }
    else if(!dev[DP_NAME].toString().isEmpty())
    {
        return dev[DP_NAME].toString();
    }

    return dev[DP_IP].toString();
}


int TopologyView::parseDeivceSignalLevel(const Device &dev)
{
    int signalLevel = 5;

    bool bok = false;
    int tmp = dev[DP_WIRELESS_SIGNALSTRENGTH].toInt(&bok);

    if(bok && (tmp >= 0) && (tmp <= 100))
    {
        signalLevel = (tmp == 0 ? 0 : (tmp - 1) / 20 + 1);
    }

    return signalLevel;
}

int TopologyView::parseDeivceSignalLevel2(const Device &dev)
{
    int signalLevel = 4;

    bool bok = false;
    int tmp = dev[DP_WIRELESS_SIGNALSTRENGTH].toInt(&bok);


    if(bok && (tmp >= 0) && (tmp <= 100))
    {
        if(tmp < 20)
        {
            signalLevel = 1;
        }
        else if(tmp >= 20 && tmp < 50)
        {
            signalLevel = 2;
        }
        else if(tmp >= 50 && tmp < 70)
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

void TopologyView::verifyRouterRefreshTimer()
{
    if((0 == m_routerRefreshTimerId) && (m_nodesList.size() >= 2))
    {
        m_routerRefreshTimerId = startTimer(ROUTER_ANIMATION_INTERVAL);
    }
    else if((m_routerRefreshTimerId != 0) && (m_nodesList.size() < 2))
    {
        killTimer(m_routerRefreshTimerId);
        m_routerRefreshTimerId = 0;
    }
}

void TopologyView::retranslateUi()
{
#ifdef USE_NODE_ANIMATION
    foreach(TopologyNode *node,m_nodesList)
    {
        verifyEdgeTooltip(node->mEdge);
    }
#else
    foreach(TopologyEdge *edge,m_edgesList)
    {
        verifyEdgeTooltip(edge);
    }
#endif
    BlockDeviceManager *bdm=BlockDeviceManager::GetSingletonPtr();
    if(bdm)
    {
        bdm->retranslateUi();
    }
}

void TopologyView::verifyEdgeTooltip(TopologyEdge *edge)
{
    if(0 == edge)
        return;

    QString toolTip;
    edge->setToolTip(toolTip);//clear first,assume no tooltip to display
	if(edge->mEdgeFlags & TopologyEdge::EF_ISWIRELESS)
    {
        if(!edge->m_signalStrength.isEmpty())
        {
            toolTip += "<span>";
            toolTip += translateText(L_MAP_DEVICEPROPERTY_SIGNALSTRENGTH,QString("Signal Strength:"));
            toolTip += " ";
            toolTip += edge->m_signalStrength;
            toolTip += "</span>";
        }

        if(!edge->m_linkRate.isEmpty())
        {
            if(!toolTip.isEmpty())
            {
                toolTip += "<br>";
            }
            toolTip += "<span>";
            toolTip += translateText(L_MAP_DEVICEPROPERTY_ATTACHDEVICE_LINKRATE,QString("Link Rate:"));
            toolTip += " ";
            toolTip += edge->m_linkRate;
            toolTip += "</span>";
        }

        edge->setToolTip(toolTip);
    }
}

//add by caiyujun
quint32 TopologyView::getNodeFlag(QString mac, bool &bResult)
{
	QList<TopologyNode *> tmpNodeLst=m_nodesList;

	int existIdx=getNodeFromMac(tmpNodeLst,mac);
	if (existIdx >= 0)
	{
		TopologyNode *node= tmpNodeLst.takeAt(existIdx);
		bResult = true;
		return node->mNodeFlags;
	}
	else
	{
		return 0;
	}
}
//add end

void TopologyView::itemsFromDevice(TopologyNode *node ,
                                   TopologyEdge *edge ,
                                   Device *dev)
{
    Q_ASSERT (node && edge && dev);

#ifdef POWERLINE_REVISION
    if(detectPowerLine(node,edge,dev))
    {
        return;
    }
#endif

    const Device &devRef = *dev;

//    qDebug()<<devRef[DP_UPNP_MODEL]<<devRef[DP_UPNP_DEVICETYPE];
    //node
	bool b=devRef[DP_ISLOCALHOST].toBool();
	checkFlags(b,node->mNodeFlags,TopologyNode::NF_ISLOCALHOST);
	checkFlags(b,edge->mEdgeFlags,TopologyEdge::EF_ISLOCALHOST);

	checkFlags(devRef[DP_ISDEFAULTGATEWAY].toBool(),node->mNodeFlags,TopologyNode::NF_ISCENTRALROUTER);

    node->m_deviceType       = dev->parseType();
	b=devRef[DP_STATUS].toBool();
	checkFlags(b,node->mNodeFlags,TopologyNode::NF_ISONLINE);
	checkFlags(b,edge->mEdgeFlags ,TopologyEdge::EF_ONLINE);
	checkFlags(devRef[DP_VMFLAG].toBool(), node->mNodeFlags ,TopologyNode::NF_ISVM);
        //add by caiyujun
        checkFlags(devRef[DP_TURBOFLAG].toBool(), node->mNodeFlags ,TopologyNode::NF_ISTURBOONLINE);
		checkFlags(MDT_REPEATER == devRef[DP_TYPE].toInt(), node->mNodeFlags, TopologyNode::NF_ISEXTENDER);
        //add end
	checkFlags((1==devRef[DP_ATTACHDEVICE_BLOCKED].toInt()),node->mNodeFlags,TopologyNode::NF_BLOCKED);


    node->m_name             = verifyNodeName(devRef);
    node->m_routerModel      = devRef[DP_CURRENTSETTING_MODEL].toString();
#ifdef USE_NODE_ANIMATION 
    node->mMac               = devRef[DP_PHYSICALADDR].toString();
    SVT_IpAddress  ip ;
    ip.Set(devRef[DP_IP].toString());
    node->mIp                = ip.asLong();
#endif
#ifdef USE_RDP
	checkFlags(devRef[DP_RDP_ENABLE].toBool(),node->mNodeFlags,TopologyNode::NF_RDP_ENABLED);
#endif
#ifdef USE_PUTTY
	checkFlags(devRef[DP_PUTTY_ENABLE].toBool(),node->mNodeFlags,TopologyNode::NF_PUTTY_ENABLED);
#endif
    if(node->m_deviceType == MDT_ROUTER || node->m_deviceType == MDT_REPEATER)
    {
        if(node->m_routerModel.isEmpty())
        {
            node->m_routerModel=devRef[DP_UPNP_MODEL].toString();
        }
        node->preparePaintStrategy();
    }

    node->setToolTip(devRef[DP_IP].toString());
    node->updateShapeCache();


    //edge

//	edge->m_isLocalHost     =  (node->mNodeFlags &TopologyNode::NF_ISLOCALHOST);
    edge->m_deviceType      = node->m_deviceType;

//    edge->m_onLine          = devRef[DP_STATUS].toInt();
    int temp_type=dev->parseWiredInCB();
    bool iswireless        = //(dev->parseWired()==MCT_WIRELESS);
            (temp_type==MCT_WIRED || temp_type==MCT_WIRELESS)?
                              (temp_type==MCT_WIRELESS):
                              !devRef[DP_ISWIRED].toBool();
	checkFlags(iswireless,edge->mEdgeFlags,TopologyEdge::EF_ISWIRELESS);


    edge->m_signalLevel     = parseDeivceSignalLevel2(devRef);

    edge->m_signalStrength  = (devRef[DP_WIRELESS_SIGNALSTRENGTH].toInt() != -1 ?
                               devRef.getDetailPropertyContent(DP_WIRELESS_SIGNALSTRENGTH)
                                   : "");
    edge->m_linkRate        = devRef.getDetailPropertyContent(DP_ATTACHDEVICE_LINKRATE);

    verifyEdgeTooltip(edge);

    //end
}

#ifdef POWERLINE_REVISION
bool TopologyView::detectPowerLine(TopologyNode *node , TopologyEdge *edge , Device *dev)
{
    const Device &devRef = *dev;

    if((devRef[DP_PHYSICALADDR].toString() == QString("FF:FF:FF:FF:FF:FF")) &&
       (devRef[DP_IP].toString() == QString("0.0.0.0")))
    {
        //node
        node->m_isLocalHost      = false;
        node->m_isCentralRouter  = false;
        node->m_deviceType       = MDT_DEVICETYPE_COUNT;
        node->m_onLine           = true;
        node->m_isVm             = false;
        node->m_blocked          = false;
        node->m_name             = "POWERLINE";
        node->m_isPowerLine      = true;
        node->m_routerModel.clear();
    #ifdef USE_NODE_ANIMATION
        node->mMac               = devRef[DP_PHYSICALADDR].toString();
        SVT_IpAddress  ip ;
        ip.Set(devRef[DP_IP].toString());
        node->mIp                = ip.asLong();
    #endif

        node->updateShapeCache();


        //edge
        edge->m_deviceType      = node->m_deviceType;
        edge->m_isLocalHost     = node->m_isLocalHost;
        edge->m_onLine          = node->m_onLine;
        edge->m_wireless        = false;
        edge->m_signalLevel     = 0;

        edge->m_signalStrength.clear();
        edge->m_linkRate.clear();
        edge->setToolTip(QString());

        //end
    }
    else
    {
        node->m_isPowerLine = false;
    }

    return node->m_isPowerLine;
}
#endif

void TopologyView::positionItems()
{
#ifdef USE_NODE_ANIMATION
    Q_ASSERT(0);
#else
    int nodeTotal = m_nodesList.size();

    if(nodeTotal < 2)
    {
        qDebug () << "Node list too small ,size = " << nodeTotal;
        return;
    }

    Q_ASSERT (m_InternetNode && m_InternetEdge);

    qreal Angle = qreal(360) / nodeTotal;

    qreal r1 = qreal(size().width()) / 3;
    qreal r2 = qreal(size().height()) / 3;


    //router position center
    //compute the central point of the router node
    qreal centerX = (2 * r1 - m_nodesList[1]->boundingRect().width()) / 2 -r1;
    qreal centerY = (2 * r2 - m_nodesList[1]->boundingRect().height()) / 2 - r2;

    m_nodesList[1]->setPos(centerX,centerY);


    QList<TopologyNode *> tmpNodeList = m_nodesList;
    tmpNodeList.removeAt(1);//remove router node
    tmpNodeList.append(tmpNodeList.takeAt(0));//swap host node to the end of the temp list
    tmpNodeList.prepend(m_InternetNode);//add the internet node to the head of the temp list

    QList<TopologyEdge *> tmpEdgeList = m_edgesList;
    tmpEdgeList.removeAt(1);
    tmpEdgeList.append(tmpEdgeList.takeAt(0));
    tmpEdgeList.prepend(m_InternetEdge);

    const qreal PI = 3.14159265358979323846;

    for(int i = 0 ; i < tmpNodeList.size() ; ++i)
    {

        //compute the central point of a node
        qreal x = r1 * cos(i * Angle * PI / 180)
                  - tmpNodeList[i]->boundingRect().width() / 2;
        qreal y = r2 * sin(i * Angle * PI / 180)
                  - tmpNodeList[i]->boundingRect().height() / 2;

        tmpNodeList[i]->setPos(x,y);

        tmpEdgeList[i]->m_headPt = QPointF(centerX,centerY);
        tmpEdgeList[i]->m_endPt = QPointF(x,y);

    }
#endif
}

void TopologyView::verifyInternetNodeEdge()
{
#ifdef USE_NODE_ANIMATION
    if(!m_InternetNode)
    {
        m_InternetNode=getNewNode();
        m_InternetEdge=m_InternetNode->mEdge;
        m_InternetNode->addToView(this,0);
    }
    m_InternetNode->m_deviceType = m_InternetEdge->m_deviceType = MDT_INTERNET;
//    m_InternetNode->m_onLine =
    checkFlags(!(sMapFlags & MF_INTERNET_OFFLINE),m_InternetNode->mNodeFlags,TopologyNode::NF_ISONLINE);
    checkFlags(!(sMapFlags & MF_INTERNET_OFFLINE), m_InternetEdge->mEdgeFlags ,TopologyEdge::EF_ONLINE);

//    m_InternetEdge->m_onLine = m_internetConnected;
    m_InternetNode->m_name = "Internet";
    m_InternetEdge->mEdgeFlags &= ~TopologyEdge::EF_ISWIRELESS;

#else
    if(!m_InternetNode)
    {
        m_InternetNode = new TopologyNode();
        m_scene->addItem(m_InternetNode);

    }

    if(!m_InternetEdge)
    {
        m_InternetEdge = new TopologyEdge();
        m_scene->addItem(m_InternetEdge);
    }

    m_InternetNode->m_deviceType = m_InternetEdge->m_deviceType = MDT_INTERNET;
    m_InternetNode->m_onLine = m_InternetEdge->m_onLine = m_internetConnected;
    m_InternetNode->m_name = "Internet";
    m_InternetEdge->m_wireless = false;

    m_InternetNode->setVisible(m_nodesList.size() > 0);
    m_InternetEdge->setVisible(m_nodesList.size() > 0);

#endif

}

void TopologyView::hideAllNodes()
{
    QList<TopologyNode *> tmpNodeLst=m_nodesList;
    m_nodesList.clear();
    while(!tmpNodeLst.isEmpty())
    {

        TopologyNode *node=tmpNodeLst.takeFirst();
        node->delFromView(this);
    }
    if(m_InternetNode)
    {
        m_InternetNode->delFromView(this);
        m_InternetNode=NULL;
    }

}

//add by caiyujun
void TopologyView::onTurboStateChanged(Device *data, QString ip, bool bOnLine, int iType)
{
    QList<TopologyNode *> tmpNodeLst = m_nodesList;
    QString mac = (*data)[DP_PHYSICALADDR].toString();
    int existIdx = getNodeFromMac(tmpNodeLst,mac);

    if (bOnLine)
	{
		if (!m_TurBoOnLineIp.contains(ip))
		{
			m_TurBoOnLineIp.insert(ip, iType);
			if (!m_ipToType.contains(ip))
			{
				m_ipToType.insert(ip, iType);
			}
			else
			{
				m_ipToType.remove(ip);
				m_ipToType.insert(ip,iType);
			}
		}
	}
	else 
    {
        if (m_TurBoOnLineIp.contains(ip))
        {
            m_TurBoOnLineIp.remove(ip);
            //m_ipToType.remove(ip);
			QMap<QString,int>::iterator it = m_ipToType.find(ip);
			if (it != m_ipToType.end() && 0 != it.value())
			{
				(*data)[DP_TYPE] = it.value();
			}
        }
    }

    if (existIdx>=0)
    {
		if (bOnLine && (*data)[DP_STATUS].toBool() && (!(*data)[DP_ISLOCALHOST].toBool()))
		{
			TopologyNode *node= (existIdx >=0)?tmpNodeLst.takeAt(existIdx):(getNewNode());

		    itemsFromDevice(node,node->mEdge,data);
		
			node->update(); 
        }

        if (!bOnLine)
        {
            TopologyNode *node= (existIdx >=0)?tmpNodeLst.takeAt(existIdx):(getNewNode());

            itemsFromDevice(node,node->mEdge,data);

            node->update();
        }
    }
}

void TopologyView::otherDevCountChanged(Device *data, int count, bool showDeviceCount)
{
	QList<TopologyNode *> tmpNodeLst = m_nodesList;
    QString mac = (*data)[DP_PHYSICALADDR].toString();
    int existIdx = getNodeFromMac(tmpNodeLst,mac);

	if (existIdx>=0)
	{
		TopologyNode *node= (existIdx >=0)?tmpNodeLst.takeAt(existIdx):(getNewNode());
		itemsFromDevice(node,node->mEdge,data);
		if (showDeviceCount)
		{
			node->setDevCount(count);
		}
		else
		{
			node->setDevCount(0);
		}
		node->update();
	}
}

void TopologyView::clearTurboOnlineIp()
{
    m_TurBoOnLineIp.clear();
    m_ipToType.clear();
}

//add end

void TopologyView::onNewData(const QList<Device *> &data)
{
    qDebug () << "Topology view get new data , size = " << data.size();

    int idx = 0;
#ifdef USE_NODE_ANIMATION
    QList<TopologyNode *> tmpNodeLst=m_nodesList;
	QList<TopologyNode *> tmpNodeLstNotDel=m_nodesList;
    m_nodesList.clear();

    //    bool bWantRestartTimer=false;

    QSet<TopologyNode *> newNodeSet;
    QSet<TopologyNode *> oldNodeSet;
    for(idx = 0 ; idx < data.size() ; ++idx)
    {
        QString mac=(*(data[idx]))[DP_PHYSICALADDR].toString();
        int existIdx=getNodeFromMac(tmpNodeLst,mac);
        TopologyNode *node=
                (existIdx >=0)?tmpNodeLst.takeAt(existIdx):(getNewNode());
 
        //add by caiyujun
		QMap<QString,int>::iterator itIpToType = m_ipToType.find((*data[idx])[DP_IP].toString());
        if (m_TurBoOnLineIp.contains((*data[idx])[DP_IP].toString()))
        {
            QMap<QString,int>::iterator it = m_TurBoOnLineIp.find((*data[idx])[DP_IP].toString());
			
            if (!(*data[idx])[DP_ISLOCALHOST].toBool() && 1 != (*data[idx])[DP_ATTACHDEVICE_BLOCKED].toInt() && (*data[idx])[DP_STATUS].toBool())
            {
                (*data[idx])[DP_TURBOFLAG] = true;
                //if (it != m_TurBoOnLineIp.end() && it.value() != 0 && (*data[idx])[DP_TYPE].toInt() != it.value())
				if (it != m_TurBoOnLineIp.end() && it.value() != 0 && (*data[idx])[DP_TYPE].toInt() != it.value())
                {
                    qDebug() << "change type--------------" << it.value() << ":ip:" << it.key();
                    (*data[idx])[DP_TYPE] = it.value();
                    //(*data[idx])[DP_CUSTOMTYPE] = it.value();
                    //it.value() = 0;
                    //emit bonjourTypeChanged(*data[idx]);
                }		
            }
            if (!(*data[idx])[DP_STATUS].toBool() || (1 == (*data[idx])[DP_ATTACHDEVICE_BLOCKED].toInt()))
            {
				QString strIp = (*data[idx])[DP_IP].toString();
                (*data[idx])[DP_TURBOFLAG] = false;
            }
        }
		else if (itIpToType != m_ipToType.end())
		{
			if (0 != itIpToType.value())
			{
				(*data[idx])[DP_TYPE] = itIpToType.value();
			}
		}
        //add end
        itemsFromDevice(node,node->mEdge,data[idx]);
        //        node->setVisible(true);
        m_nodesList.append(node);

        if(existIdx<0) //new node
        {
            newNodeSet.insert(node);
        }
        else
        {
            oldNodeSet.insert(node);
        }

    } //for

	//modify by caiyujun
    const int listSize=m_nodesList.size();
    //const qreal step = qreal(360) / listSize;
	qreal step = qreal(360) / listSize;
    qreal angle=0;
    QMap<TopologyNode *,qreal> tmpAngleCache;

	QString strCenterNodeMac;
	if (tmpNodeLstNotDel.size() > 0)
	{
		if (tmpNodeLstNotDel[0]->mNodeFlags & TopologyNode::NF_ISLOCALHOST)
		{
			if (tmpNodeLstNotDel.size() > 1)
			{
				strCenterNodeMac = tmpNodeLstNotDel[1]->mMac;
			}
		}
		else
		{	
			if (tmpNodeLstNotDel.size() > 0)
			{
				strCenterNodeMac = tmpNodeLstNotDel[0]->mMac;
		}
		}
	}

	bool bLocalHost = false;
	if (listSize>1)
	{
		bLocalHost = m_nodesList[0]->mNodeFlags & TopologyNode::NF_ISLOCALHOST;
	}
	if (bLocalHost)
	{
		if(listSize>1 && m_nodesList[1]->mNodeFlags & TopologyNode::NF_ISEXTENDER)
		{
			step = qreal(360) / (listSize - 1);
		}

		
		//if((listSize>1 && newNodeSet.contains(m_nodesList[1])) || (listSize>1 && (m_nodesList[1]->mNodeFlags & TopologyNode::NF_ISEXTENDER)))

		if(listSize>1)
		{
			if (m_nodesList[1]->mNodeFlags & TopologyNode::NF_ISCENTRALROUTER)
			{
				if (m_InternetNode)
				{
					m_InternetNode->setVisible(true);
				}
				if (strCenterNodeMac != m_nodesList[1]->mMac)
				{			
					m_nodesList[1]->addToView(this,0,0,0);
					m_nodesList[1]->setZValue(101);
				}

				for(int i = 2 ; i < listSize ; ++i)
				{
					angle +=step;
					tmpAngleCache.insert(m_nodesList[i],angle);
				} //for
				if(listSize>0)
				{
					tmpAngleCache.insert(m_nodesList[0],angle +step);
				}
			}
			else if (m_nodesList[1]->mNodeFlags & TopologyNode::NF_ISEXTENDER)
			{
				if (m_InternetNode)
				{
					m_InternetNode->setVisible(false);
				}

				if (strCenterNodeMac != m_nodesList[1]->mMac)
				{
					m_nodesList[1]->addToView(this,0,0,0);
					m_nodesList[1]->setZValue(101);

					if (listSize>2 && (m_nodesList[2]->mNodeFlags & TopologyNode::NF_ISCENTRALROUTER))
					{
						m_nodesList[2]->addToView(this,0);
					}
				}

				for(int i = 3 ; i < listSize ; ++i)
				{
					angle +=step;
					tmpAngleCache.insert(m_nodesList[i],angle);
				} //for
				if(listSize>0)
				{
					tmpAngleCache.insert(m_nodesList[0],angle +step);
				}
			}		
		}
	}
	else
	{	
		if((listSize>1 && newNodeSet.contains(m_nodesList[0])) || (listSize>1 && (m_nodesList[0]->mNodeFlags & TopologyNode::NF_ISEXTENDER)))
		{
			step = qreal(360) / (listSize - 1);
			for(int i = 2 ; i < listSize ; ++i)
			{
				angle +=step;
				tmpAngleCache.insert(m_nodesList[i],angle);
			}

			if (strCenterNodeMac != m_nodesList[0]->mMac)
			{
				m_nodesList[0]->addToView(this,0,0,0);
				m_nodesList[0]->setZValue(101);

				if (listSize>1 && (m_nodesList[1]->mNodeFlags & TopologyNode::NF_ISCENTRALROUTER))
				{
					m_nodesList[1]->addToView(this,0);
					if (m_InternetNode)
					{
						m_InternetNode->setVisible(false);
					}
				}
			}	
		}

		if((listSize>1 && newNodeSet.contains(m_nodesList[0])) || (listSize>1 && (m_nodesList[0]->mNodeFlags & TopologyNode::NF_ISCENTRALROUTER)))
		{
			step = qreal(360) / listSize;
			for(int i = 1 ; i < listSize ; ++i)
			{
				angle +=step;
				tmpAngleCache.insert(m_nodesList[i],angle);
			}

			if (strCenterNodeMac != m_nodesList[0]->mMac)
			{
				m_nodesList[0]->addToView(this,0,0,0);
				m_nodesList[0]->setZValue(101);
				m_InternetNode->setVisible(true);

			}	
		}	
	}
	//modify end

    qreal maxRange=0;
    foreach(TopologyNode *node,oldNodeSet)
    {
        if(tmpAngleCache.contains(node))
        {
            qreal rtemp=node->setAngleAnimTo(tmpAngleCache.value(node));
            maxRange=qMax(rtemp,maxRange);
        }
    }
    foreach(TopologyNode *node,newNodeSet)
    {
        if(tmpAngleCache.contains(node))
        {
            node->addToView(this,tmpAngleCache.value(node),ELLIPSE_RADIUS
                            ,(maxRange>ANGLE_DELAY_THRESHOLD)?1000:0);
        }
    }


    while(!tmpNodeLst.isEmpty())
    {

        TopologyNode *node=tmpNodeLst.takeFirst();
        node->delFromView(this);
    }
    if(data.isEmpty() && m_InternetNode)
    {
        m_InternetNode->delFromView(this);
        m_InternetNode=NULL;
    }
    else
    {
        verifyInternetNodeEdge();
    }

    mAnimTimer.start(100);

#else
    Q_ASSERT (m_nodesList.size() == m_edgesList.size());
    for(idx = 0 ; idx < data.size() ; ++idx)
    {
        if(idx >= m_nodesList.size())
        {
            m_nodesList.append(new TopologyNode());
            m_edgesList.append(new TopologyEdge());

            m_scene->addItem(m_nodesList[idx]);
            m_scene->addItem(m_edgesList[idx]);

            connect(m_nodesList[idx],SIGNAL(clicked(TopologyNode*)),
                    this,SLOT(onNodeClicked(TopologyNode*)));
            connect(m_nodesList[idx],SIGNAL(contextMenu(TopologyNode*,QPoint)),
                    this,SLOT(onNodeContextMenu(TopologyNode*,QPoint)));
        }

        m_nodesList[idx]->setVisible(true);
        m_edgesList[idx]->setVisible(idx != 1);//default router edge not visible

        itemsFromDevice(m_nodesList[idx],m_edgesList[idx],data[idx]);
    }

    int tmpCount = m_nodesList.size();

    for(int k = idx ; k < tmpCount; ++k)
    {
        m_scene->removeItem(m_nodesList[idx]);
        m_scene->removeItem(m_edgesList[idx]);

        delete m_nodesList.takeAt(idx);
        delete m_edgesList.takeAt(idx);
    }
    verifyInternetNodeEdge();
    positionItems();
#endif


    m_scene->invalidate();
    update();

    verifyRouterRefreshTimer();


}

void TopologyView::onInternetStateChanged(bool connected)
{
//    m_internetConnected = connected;
    checkFlags(!connected,sMapFlags,MF_INTERNET_OFFLINE);

    if(m_InternetNode)
    {
        checkFlags(!(sMapFlags & MF_INTERNET_OFFLINE),m_InternetNode->mNodeFlags, TopologyNode::NF_ISONLINE);
        m_InternetNode->update();
    }

    if(m_InternetEdge)
    {
        checkFlags(!(sMapFlags & MF_INTERNET_OFFLINE), m_InternetEdge->mEdgeFlags ,TopologyEdge::EF_ONLINE);

        m_InternetEdge->update();
    }
}

void TopologyView::resizeEvent(QResizeEvent *event)
{
#ifdef USE_NODE_ANIMATION
#else
    positionItems();
#endif
    QGraphicsView::resizeEvent(event);
}

void TopologyView::timerEvent(QTimerEvent *event)
{
    if(m_routerRefreshTimerId == event->timerId())
    {
        if((m_nodesList.size() >= 2) && (m_nodesList[1] != NULL))
        {
            m_nodesList[1]->update();
        }
    }
}

void TopologyView::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange)
    {
        retranslateUi();
    }

    QGraphicsView::changeEvent(event);
}

//add by caiyujun
void TopologyView::dragEnterEvent(QDragEnterEvent *event)
{

}

void TopologyView::dropEvent(QDropEvent *event)
{

}

int TopologyView::getDeviceType(QString strIp)
{
    QMap<QString,int>::iterator it = m_ipToType.find(strIp);
    if (it != m_ipToType.end())
    {
        return it.value();
    }
    else
    {
        return -1;
    }

}

//add end

#ifdef USE_NODE_ANIMATION
int TopologyView::getNodeFromMac(const QList<TopologyNode *> &lst,const QString &mac)
{
    for(int i=0;i<lst.count();i++)
    {
        if(lst[i]->mMac==mac)
        {
            return i;
        }
    }
    return -1;
}

TopologyNode *TopologyView::getNewNode()
{
    TopologyNode *ret=NULL;
    foreach(TopologyNode *node,mBackGroundNodes)
    {
        if(node->mNodeState==TopologyNode::NS_BEHIND)
        {
            mBackGroundNodes.remove(node);

//            node->m_isLocalHost=false;
            node->mNodeFlags =0;
            node->mEdge->mEdgeFlags=0;
//            node->m_isVm=false;
//#ifdef USE_RDP
//            node->mRdtEnabled=false;
//#endif
//#ifdef USE_PUTTY
//            node->mPuttyEnabled=false;
//#endif
            node->m_deviceType=MDT_NETWORKDEV;
            if(node->m_paintStrategy)
            {
                delete node->m_paintStrategy;
                node->m_paintStrategy=0;
            }

            node->mStartAngle=0.0;
            node->mEndAngle=0.0;
            node->mRadius=0.0;
            node->mNodeState=TopologyNode::NS_INIT;
            node->mView=NULL;
            node->setToolTip("");
            node->mEdge->setToolTip("");

            ret = node;
            break;
        }
    }
    if(!ret){
        ret =new TopologyNode();
        mAllNodes.append(ret);
        connect(ret,SIGNAL(clicked(TopologyNode*)),
                this,SLOT(onNodeClicked(TopologyNode*)));
        connect(ret,SIGNAL(contextMenu(TopologyNode*,QPoint)),
                this,SLOT(onNodeContextMenu(TopologyNode*,QPoint)));
        //add by caiyujun
        connect(ret,SIGNAL(mouseInNode(bool,TopologyNode*)),
                this,SLOT(onMouseInNode(bool,TopologyNode*)));
        //add end

    }
    ret->setZValue(100);

    return ret;
}


void TopologyView::slot_animTimout()
{
    //    qDebug()<<"TopologyNode::slot_animTimout"<<mScaleTimeLine.currentValue();
    bool stopTimer=true;
    if(m_InternetNode)
    {
        m_InternetNode->animFrame();
    }
    foreach(TopologyNode *node,m_nodesList)
    {
        if(node->animFrame())
        {
            stopTimer=false;
        }
    }
    if(stopTimer && mAnimTimer.isActive())
    {
        mAnimTimer.stop();
    }
}



#endif


QString TopologyView::translateText(int idx, const QString &defaultText)
{
    QString text;
    emit translateText(idx,&text);
    return text.isEmpty() ? defaultText : text;
}

void TopologyView::onNodeClicked(TopologyNode *node)
{
    int idx = m_nodesList.indexOf(node);

    qDebug() << "Node " << idx << "clicked";

    if(idx != -1)
        emit nodeClicked(idx);
}

//add by caiyujun
void TopologyView::onMouseInNode(bool isIn,TopologyNode *node)
{
    int idx = m_nodesList.indexOf(node);

    if(idx != -1)
        emit mouseInNode(isIn, idx);
}
//add end

void TopologyView::onNodeContextMenu(TopologyNode *node,const QPoint &pos)
{
    int idx = m_nodesList.indexOf(node);

    qDebug() << "Node " << idx << "context menu";

    if(idx != -1)
        emit nodeContextMenu(idx,pos);
}

