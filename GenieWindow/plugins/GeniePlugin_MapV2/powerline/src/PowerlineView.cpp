#include "PowerlineNode.h"
#include "PowerlineEdge.h"
#include "PowerlineView.h"
#include "PLDevice.h"
#include "Powerline_mapuitilities.h"
#include "Powerline_DetailDlg.h"
#include "DialogMgr.h"
//#include "blockdevicemanager.h"

#include <QGraphicsScene>
#include <QtGlobal>
#include <math.h>
#include <QtDebug>
#include <QTimerEvent>
#include <QEvent>
#include <QQueue>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QLabel>
#include "language_Map.h"

#include "PowerlineMapView.h"


#define TOPOLOGYVIEW_STYLE		".PowerlineView{border-image:url(:/PowerLineImage/bg_plugin_page.png) 14;}"


PowerlineView::PowerlineView(QWidget *parent)
    :QGraphicsView(parent)
    ,m_scene(0)
    ,m_InternetNode(0)
    ,m_InternetEdge(0)
	,m_RouterNode(0)
	,m_RouterEdge(0)
    //,m_routerRefreshTimerId(0)
    ,m_MouseOverNodeIdx(-1)
    ,m_TopEdge(NULL)
    ,m_TopNode(NULL)
    ,m_LocalDevSonPowel(0)
    ,m_nodePic(NULL)
    ,m_nextBtnProxy(NULL)
    ,m_nextPageBtn(NULL)
    ,m_prevPageBtn(NULL)
    ,m_preBtnProxy(NULL)
    ,m_idxStartPos(0)
    ,m_cacheData(NULL)
    ,m_bFirstStart(false)
	,m_localDevPCState(LOS_INIT)
	,m_IsInternetConnected(true)
	,m_lstPlDev(NULL)
	,m_IsRouterOk(false)
	,m_plLocalDevNode(NULL)
	,m_plLocalDevEdge(NULL)
{
    setViewportUpdateMode(FullViewportUpdate);
    setAttribute(Qt::WA_TranslucentBackground);
    setRenderHint(QPainter::Antialiasing);
    setCacheMode(QGraphicsView::CacheBackground);

    setStyleSheet(QString(TOPOLOGYVIEW_STYLE));

    m_scene = new QGraphicsScene(this);
    // m_scene->setSceneRect(QRectF(-257.443,-37.3571 ,515.985,74.7142) );
    m_scene->setSceneRect(QRectF(0,0 ,515.985,74.7142) );
    m_scene->setItemIndexMethod(QGraphicsScene::NoIndex);

    //    this->setInteractive(true);
    this->setScene(m_scene);

	setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //////两个翻页按钮
    m_nextPageBtn = new QPushButton();
    m_prevPageBtn = new QPushButton();
    m_prevPageBtn->setObjectName("prevPageBtn");
    m_nextPageBtn->setObjectName("nextPageBtn");
    m_prevPageBtn->setAttribute(Qt::WA_TranslucentBackground);
    m_nextPageBtn->setAttribute(Qt::WA_TranslucentBackground);
    m_preBtnProxy = m_scene->addWidget(m_prevPageBtn);
    m_nextBtnProxy = m_scene->addWidget(m_nextPageBtn);
    m_prevPageBtn->setVisible(false);
    m_nextPageBtn->setVisible(false);

	//m_preBtnProxy->setAcceptHoverEvents(true);
	//m_nextBtnProxy->setAcceptHoverEvents(true);

    qreal r2 = 74.7142 / 2;
    m_preBtnProxy->setPos(-50, r2);
    m_nextBtnProxy->setPos(545, r2);
    connect(m_nextPageBtn, SIGNAL(clicked()), this, SLOT(BtnClick()));
    connect(m_prevPageBtn, SIGNAL(clicked()), this, SLOT(BtnClick()));
    ///////////End


    ///////锁的图片节点
    m_nodePic = new PowerlineNode();
    m_scene->addItem(m_nodePic);
    m_nodePic->m_deviceType = GDT_UNLOCKPIC;
    m_nodePic->hide();
    //connect(m_nodePic, SIGNAL(nodeMouseOver(int)), this, SLOT(onMouseOverNode(int)));
    /////////End

    initStyle();
}

PowerlineView::~PowerlineView()
{

    for(int i = 0; i < m_nodesList.count(); ++i)
    {
        m_scene->removeItem(m_nodesList[0]);
        m_scene->removeItem(m_edgesList[0]);

        delete m_nodesList.takeAt(0);
        delete m_edgesList.takeAt(0);
    }

    if(m_nodePic != NULL)
    {
        delete m_nodePic;
        m_nodePic = NULL;
    }
	POWERLINE_SAFEDEL(m_RouterNode);
	POWERLINE_SAFEDEL(m_RouterEdge);
	POWERLINE_SAFEDEL(m_InternetEdge);
	POWERLINE_SAFEDEL(m_InternetNode);
    //if(m_routerRefreshTimerId)
    //{
    //    killTimer(m_routerRefreshTimerId);
    //}
}
void PowerlineView::initStyle()
{

    QFile f(/*GENIE2_RES(*/":styles/powerlineview.qss")/*)*/;
    bool btemp=f.open(QIODevice::ReadOnly);
    Q_ASSERT(btemp);
    QString stemp=f.readAll();
    f.close();


    m_prevPageBtn->setStyleSheet(stemp);
    m_nextPageBtn->setStyleSheet(stemp);

}
QString PowerlineView::verifyNodeName(const PLDevice &dev)
{
    if(!dev[GDP_CUSTOMNAME].toString().isEmpty())
    {
        return dev[GDP_CUSTOMNAME].toString();
    }
    else if(!dev[GDP_NAME].toString().isEmpty())
    {
        return dev[GDP_NAME].toString();
    }

    return dev[GDP_IP].toString();
}

int PowerlineView::parseDeviceSignalLevel(const PLDevice &dev)
{
    int signalLevel = 5;

    bool bok = false;
    int tmp = dev[GDP_WIRELESS_SIGNALSTRENGTH].toInt(&bok);

    if(bok && (tmp >= 0) && (tmp <= 100))
    {
        signalLevel = (tmp == 0 ? 0 : (tmp - 1) / 20 + 1);
    }

    return signalLevel;
}

int PowerlineView::parseDeviceSignalLevel2(const PLDevice &dev)
{
    int signalLevel = 4;

    bool bok = false;
    int tmp = dev[GDP_WIRELESS_SIGNALSTRENGTH].toInt(&bok);


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



//void topologyview::retranslateUi()
//{
//    foreach(topologyedge *edge, m_edgesList)
//    {
//        verifyEdgeTooltip(edge);
//    }

//    BlockDeviceManager *bdm = BlockDeviceManager::GetSingletonPtr();
//    if(bdm)
//    {
//        bdm->retranslateUi();
//    }
//}

void PowerlineView::verifyEdgeTooltip(PowerlineEdge *edge)
{
    if(0 == edge)
        return;

    QString toolTip;
    edge->setToolTip(toolTip);//clear first,assume no tooltip to display
    if(edge->m_wireless)
    {
        if(!edge->m_signalStrength.isEmpty())
        {
            toolTip += "<span>";
            toolTip += /*translateText(L_MAP_DEVICEPROPERTY_SIGNALSTRENGTH,QString(*/"Signal Strength:"/*))*/;
            toolTip += edge->m_signalStrength;
            toolTip += "</span>";
        }

        if(!edge->m_linkRate_send.isEmpty())
        {
            if(!toolTip.isEmpty())
            {
                toolTip += "<br>";
            }
            toolTip += "<span>";
            toolTip += /*translateText(L_MAP_DEVICEPROPERTY_ATTACHDEVICE_LINKRATE,QString(*/"Link Rate:"/*))*/;
            toolTip += edge->m_linkRate_send;
            toolTip += "</span>";
        }

        edge->setToolTip(toolTip);
    }
}

void PowerlineView::stringDealColon(QString &str)
{
    str.replace(":", "");
	//str.replace("\"", "");
}

void PowerlineView::retranslateUi()
{
    QString strToolTip;
    QString strName		= POWERLINE_GET_TEXT(L_POWERLINE_TOOLTIP_NAME, "Name");
    QString strModel	= POWERLINE_GET_TEXT(L_POWERLINE_TOOLTIP_MODEL, "Model");
    QString strFWV		= POWERLINE_GET_TEXT(L_POWERLINE_TOOLTIP_FW, "Firmware Version");
    QString strIp		= POWERLINE_GET_TEXT(L_POWERLINE_TOOLTIP_IP, "IP Address");
    QString strMac		= POWERLINE_GET_TEXT(L_POWERLINE_TOOLTIP_MAC, "MAC Address");
    QString strNetType	= POWERLINE_GET_TEXT(L_POWERLINE_TOOLTIP_TYPE, "Network Type");
    QString strLed		= POWERLINE_GET_TEXT(L_POWERLINE_TOOLTIP_LED, "LED Status:");
    QString strSSID		= POWERLINE_GET_TEXT(L_POWERLINE_TOOLTIP_SSID, "SSID");
    QString strPriv		= POWERLINE_GET_TEXT(L_POWERLINE_TOOLTIP_PRIVATE, "Private");
    QString strPublic	= POWERLINE_GET_TEXT(L_POWERLINE_TOOLTIP_PUBLIC, "Public");
    QString strOn		= POWERLINE_GET_TEXT(L_POWERLINE_TOOLTIP_ON, "ON");
    QString strOff		= POWERLINE_GET_TEXT(L_POWERLINE_TOOLTIP_OFF, "OFF");
    stringDealColon(strName);
    stringDealColon(strModel);
    stringDealColon(strFWV);
    stringDealColon(strIp);
    stringDealColon(strMac);
    stringDealColon(strNetType);
    stringDealColon(strLed);
    stringDealColon(strSSID);


    foreach(PowerlineNode *node,m_nodesList)
    {
        if(node->m_devSupportWireless)
        {
            /*strToolTip = QString("%1: \t%2\n%3: \t%4\n%5: \t%6\n%7: \t%8\n%9: \t%10\n%11: \t%12\n%13: \t%14\n%15: \t%16").arg(\
   strName, DEAL_STR(node->m_devName)).arg(strModel, DEAL_STR(node->m_devModelName)).arg(strMac, node->m_devMac).arg(strLed, DEAL_BOOL(node->m_devLedState, strOn, strOff)).arg(\
   strFWV, node->m_devFwVer).arg(strIp, DEAL_STR(node->m_devIp)).arg(strSSID, DEAL_STR(node->m_wlanInfo.m_ssid)).arg(strNetType, DEAL_EQU_EX(node->m_devType, "Public", strPublic, strPriv)); */
            strToolTip = QString("%1: %2\n%3: %4\n%5: %6\n%7: %8\n%9: %10\n%11: %12\n%13: %14\n%15: %16").arg(\
                        strName, DEAL_STR(node->m_devName)).arg(strModel, DEAL_STR(node->m_devModelName)).arg(strMac, node->m_devMac).arg(strLed, DEAL_BOOL(node->m_devLedState, strOn, strOff)).arg(\
                        strFWV, node->m_devFwVer).arg(strIp, DEAL_STR(node->m_devIp)).arg(strSSID, DEAL_STR(node->m_wlanInfo.m_ssid)).arg(strNetType, DEAL_EQU_EX(node->m_devType, "Public", strPublic, strPriv));
        }
        else
        {
            strToolTip = QString("%1: %2\n%3: %4\n%5: %6\n%7: %8\n%9: %10\n%11: %12\n%13: %14").arg(\
                        strName, DEAL_STR(node->m_devName)).arg(strModel, DEAL_STR(node->m_devModelName)).arg(strMac, node->m_devMac).arg(strLed, DEAL_BOOL(node->m_devLedState, strOn, strOff)).arg(\
                        strFWV, node->m_devFwVer).arg(strIp, DEAL_STR(node->m_devIp)).arg(strNetType, DEAL_EQU_EX(node->m_devType, "Public", strPublic, strPriv));
        }
        node->setToolTip(strToolTip);
    }
}
void PowerlineView::itemsFromDevice(PowerlineNode *node, PowerlineEdge *edge, PLDevice *dev)
{
    Q_ASSERT(node && edge && dev);

    //    if(detectPowerLine(node,edge,dev))
    //    {
    //        return;
    //    }


    const PLDevice &devRef = *dev;

    //node


    node->m_isSonNodePaint	 = false;

    node->m_isPowerLine		 = true;
    node->m_isLocalHost      = devRef[GDP_ISLOCALHOST].toBool();
    node->m_onLine           = devRef[GDP_STATUS].toBool();


    node->m_devSupportUSB	 = devRef[GDP_SUPPORT_USB].toBool();
    node->m_devSupportWireless = devRef[GDP_SUPPORT_WIRELESS].toBool();
    node->m_devFwVer         = devRef[GDP_VERSION].toString();
    node->m_devIp            = devRef[GDP_IP].toString();
    node->m_devLedState      = devRef[GDP_LED_STATUS].toBool();
    node->m_devModelName     = devRef[GDP_MODEL_NAME].toString();
    node->m_devName          = devRef[GDP_NAME].toString();
    node->m_devMac           = devRef[GDP_PHYSICALADDR].toString();
    node->m_devType          = devRef[GDP_NETWORK_TYPE].toString();
    node->m_wlanInfo		 = devRef.m_wlanInfo;
    node->m_deviceType       = GDT_POWERLINE;
    node->m_localDevice		 = false;
    node->m_bPicUseDefault	 = false;
	node->m_nID				 = devRef.m_nID;
	


    QString strToolTip;
    //======================
    // ToolTip
    //======================
    // Name:
    // Model:
    // FWVersion:
    // IP:
    // Mac:
    // NetworkType: public / private
    // LEDOn:
    // SupportUSB:
    // SupportWireless:
    //======================
    /*strToolTip = QString("Name:\t%1\nModel:\t%2\nFWVersion:\t%3\nIP:\t%4\nMac:\t%5\nNetWorkType:\t%6\nLEDOn:\t%7\nSupportUSB:\t%8\nSupportWireless:%9\nSSID:\t%10").arg(\
       node->m_devName).arg(node->m_devModelName).arg(node->m_devFwVer).arg(node->m_devIp).arg(\
       node->m_devMac).arg(node->m_devType).arg((node->m_devLedState ? "Yes" : "No")).arg(\
       (node->m_devSupportUSB ? "Yes" : "No")).arg((node->m_devSupportWireless ? "Yes" : "No")).arg(node->m_wlanInfo.m_ssid);*/
    QString strName		= POWERLINE_GET_TEXT(L_POWERLINE_TOOLTIP_NAME, "Name");
    QString strModel	= POWERLINE_GET_TEXT(L_POWERLINE_TOOLTIP_MODEL, "Model");
    QString strFWV		= POWERLINE_GET_TEXT(L_POWERLINE_TOOLTIP_FW, "Firmware Version");
    QString strIp		= POWERLINE_GET_TEXT(L_POWERLINE_TOOLTIP_IP, "IP Address");
    QString strMac		= POWERLINE_GET_TEXT(L_POWERLINE_TOOLTIP_MAC, "MAC Address");
    QString strNetType	= POWERLINE_GET_TEXT(L_POWERLINE_TOOLTIP_TYPE, "Network Type");
    QString strLed		= POWERLINE_GET_TEXT(L_POWERLINE_TOOLTIP_LED, "LED Status:");
    QString strSSID		= POWERLINE_GET_TEXT(L_POWERLINE_TOOLTIP_SSID, "SSID");
    QString strPriv		= POWERLINE_GET_TEXT(L_POWERLINE_TOOLTIP_PRIVATE, "Private");
    QString strPublic	= POWERLINE_GET_TEXT(L_POWERLINE_TOOLTIP_PUBLIC, "Public");
    QString strOn		= POWERLINE_GET_TEXT(L_POWERLINE_TOOLTIP_ON, "ON");
    QString strOff		= POWERLINE_GET_TEXT(L_POWERLINE_TOOLTIP_OFF, "OFF");
    stringDealColon(strName);
    stringDealColon(strModel);
    stringDealColon(strFWV);
    stringDealColon(strIp);
    stringDealColon(strMac);
    stringDealColon(strNetType);
    stringDealColon(strLed);
    stringDealColon(strSSID);

    if(node->m_devSupportWireless)
    {
        /*strToolTip = QString("%1: \t%2\n%3: \t%4\n%5: \t%6\n%7: \t%8\n%9: \t%10\n%11: \t%12\n%13: \t%14\n%15: \t%16").arg(\
   strName, DEAL_STR(node->m_devName)).arg(strModel, DEAL_STR(node->m_devModelName)).arg(strMac, node->m_devMac).arg(strLed, DEAL_BOOL(node->m_devLedState, strOn, strOff)).arg(\
   strFWV, node->m_devFwVer).arg(strIp, DEAL_STR(node->m_devIp)).arg(strSSID, DEAL_STR(node->m_wlanInfo.m_ssid)).arg(strNetType, DEAL_EQU_EX(node->m_devType, "Public", strPublic, strPriv)); */
        strToolTip = QString("%1: %2\n%3: %4\n%5: %6\n%7: %8\n%9: %10\n%11: %12\n%13: %14\n%15: %16").arg(\
                    strName, DEAL_STR(node->m_devName)).arg(strModel, DEAL_STR(node->m_devModelName)).arg(strMac, node->m_devMac).arg(strLed, DEAL_BOOL(node->m_devLedState, strOn, strOff)).arg(\
                    strFWV, node->m_devFwVer).arg(strIp, DEAL_STR(node->m_devIp)).arg(strSSID, DEAL_STR(node->m_wlanInfo.m_ssid)).arg(strNetType, DEAL_EQU_EX(node->m_devType, "Public", strPublic, strPriv));
    }
    else
    {
        strToolTip = QString("%1: %2\n%3: %4\n%5: %6\n%7: %8\n%9: %10\n%11: %12\n%13: %14").arg(\
                    strName, DEAL_STR(node->m_devName)).arg(strModel, DEAL_STR(node->m_devModelName)).arg(strMac, node->m_devMac).arg(strLed, DEAL_BOOL(node->m_devLedState, strOn, strOff)).arg(\
                    strFWV, node->m_devFwVer).arg(strIp, DEAL_STR(node->m_devIp)).arg(strNetType, DEAL_EQU_EX(node->m_devType, "Public", strPublic, strPriv));
    }
    node->setToolTip(strToolTip);
    node->updateShapeCache();


    //edge

    //edge->m_linkRate_send   = devRef.m_linkRate_send;
    //edge->m_linkRate_recv	= devRef.m_linkRate_recv;
    edge->m_deviceType      = node->m_deviceType;
    edge->m_isLocalHost     = node->m_isLocalHost;
    edge->m_onLine          = devRef[GDP_STATUS].toInt();
    edge->m_wireless        = false;
    edge->m_signalLevel     = parseDeviceSignalLevel2(devRef);
    edge->m_bLocalDevice	= false;
    edge->m_devLedState		= node->m_devLedState;
	edge->m_isUsb			= false;

    //edge->m_parent          = node->m_parent;
    //edge->m_son             = node->m_son;
    edge->m_deviceType		= GDT_POWERLINE;
	qDebug() << "Powerline Model: " << node->m_devModelName << "Send Rate: " << edge->m_linkRate_send << "Rec Rate: " << edge->m_linkRate_recv;


    edge->m_signalStrength  = (devRef[GDP_WIRELESS_SIGNALSTRENGTH].toInt() != -1 ?
                devRef.getDetailPropertyContent(GDP_WIRELESS_SIGNALSTRENGTH)
              : "");

    verifyEdgeTooltip(edge);
}
QString PowerlineView::getDevStrType(const int dev)
{
	switch(dev)
	{
	case GDT_HUB:
		return "Hub";
	case GDT_SOUND:
		return "Speaker";
	case GDT_PRINTER:
		return "Printer";
	case GDT_STORAGE:
		return "Storage";
	default:
		return "Unknown";
	}
}

DEVICE_TYPE_ PowerlineView::getDevType(const QString &str)
{
	if(str == "Hub")
	{
		return GDT_HUB;
	}
	else if(str == "Speaker")
	{
		return GDT_SOUND;
	}
	else if(str == "Printer")
	{
		return GDT_PRINTER;
	}
	else if(str == "Storage")
	{
		return GDT_STORAGE;
	}
	else if(str == "Unknown")
	{
		return GDT_UNKNOWN;
	}


	return GDT_UNKNOWN;
}
void PowerlineView::createRouterAndEarth()
{
	if(m_InternetEdge && m_InternetNode)
	{
		m_InternetNode->setVisible(false);
		m_InternetEdge->setVisible(false);
	}

	if(!m_IsRouterOk)
	{
		return;
	}

	//==========Internet Node==============
	if(!m_InternetNode)
	{
		m_InternetNode = new PowerlineNode;
		m_scene->addItem(m_InternetNode);
	}
	if(!m_InternetEdge)
	{
		m_InternetEdge = new PowerlineEdge;
		m_scene->addItem(m_InternetEdge);
	}

	m_InternetNode->m_deviceType = m_InternetEdge->m_deviceType = GDT_INTERNET;
	m_InternetNode->m_onLine = m_InternetEdge->m_onLine = m_IsInternetConnected;
	m_InternetNode->setVisible(m_nodesList.size() > 0);
	m_InternetEdge->setVisible(m_nodesList.size() > 0);
	//=============END===================

	//==============Router Node===============
	if(!m_RouterNode)
	{
		m_RouterNode = new PowerlineNode;
		m_scene->addItem(m_RouterNode);
	}
	if(!m_RouterEdge)
	{
		m_RouterEdge = new PowerlineEdge;
		m_scene->addItem(m_RouterEdge);
	}

	m_RouterNode->m_deviceType = m_RouterEdge->m_deviceType = GDT_ROUTER;
	m_RouterNode->m_onLine = m_RouterEdge->m_onLine = m_IsInternetConnected;
	m_RouterNode->setVisible(m_nodesList.size() > 0);
	m_RouterEdge->setVisible(m_nodesList.size() > 0);
	//=================END====================
}
void PowerlineView::positionRouterAndEarth(const QPointF &pos)
{
	Q_ASSERT(m_RouterNode && m_RouterEdge);
	Q_ASSERT(m_InternetNode && m_InternetEdge);

	qreal nX = 0, nY = 0;

	//===========Position=============//
	nY = pos.y() - m_RouterNode->boundingRect().width() / 2 - 60;
	m_RouterNode->setPos(pos.x(), nY);
	m_RouterEdge->m_endPt = QPointF(pos.x(), nY);
	m_RouterEdge->m_headPt = pos;
	
	nX = pos.x() - m_InternetNode->boundingRect().height() / 3 - 80;
	m_InternetNode->setPos(nX, nY);
	qDebug() << "m_InternetNode X,Y: " << nX << "," << nY;
	m_InternetEdge->m_endPt = QPointF(nX, nY);
	m_InternetEdge->m_headPt = m_RouterEdge->m_endPt;
	//===============END==============//

}
void PowerlineView::positionItems(int nStart, int nLen/*, int nCurrenIdxPage*/)
{
    Q_UNUSED(nStart)
    Q_UNUSED(nLen)
    //int nodeTotal = m_nodesList.size();

    //if(nodeTotal < 2)
    //{
    //    qDebug () << "Node list too small ,size = " << nodeTotal;
    //    return;
    //}

    //    Q_ASSERT (m_InternetNode && m_InternetEdge);


    qreal r1 = qreal(size().width()) / 3;
    qreal r2 = qreal(size().height()) / 3;

    /////Test code
    r2 = 74.7142 / 3;
    r1 = 515.985 / 3;
    /////end

    QList<PowerlineNode *> tmpNodeList = m_nodesList;
    QList<PowerlineEdge *> tmpEdgeList = m_edgesList;



    ////find top m_parent;
    qDebug() << "m_parent: " << tmpNodeList.count();
    QList<PowerlineNode *> tmpTopNode;


    qDebug() << "topologyview::positionItems()" << tmpNodeList.count();
    /////position items
    int nCount = 0;
    int nIdx = 0;
    qreal x = 0, y = 0, centryX = 0, centryY = 0;
    int nCurCount = 0;

    nCurCount = 0;

    /////queue visiter node
    QQueue<int> tmpQue;
    int nParentIdx = 0;
    int nSonPlc = 0, nTmpSonPlc = 0;

    const qreal PI = 3.14159265358979323846;

    int nLocalDevSonOfPc = 0;


	const int nWidth = 100;
	const int nHeight = 100;
    //for(int i = 0; i < tmpTopNode.count(); ++i)
    //{
    nCount = m_TopNode->m_son.count() - 1;


    nParentIdx = m_TopNodeIdx.toInt();

    nLocalDevSonOfPc = nCount - m_LocalDevSonPowel;	//localdevice中不算powerline的子节点个数
    tmpQue.enqueue(nParentIdx);
    while(!tmpQue.isEmpty())
    {
        nIdx = tmpQue.dequeue();

        //bRet = isCurrentPageIdx(nIdx, nStart, nLen, nCurrenIdxPage);
        if(/*(!bRet) &&*/ tmpNodeList.count() <= nIdx)
        {
            continue;
        }

        for(int j = 1; j < tmpNodeList[nIdx]->m_son.count(); ++j)
        {
            tmpQue.enqueue(tmpNodeList[nIdx]->m_son[j].toInt());
        }





        if(tmpNodeList[nIdx]->m_deviceType != GDT_POWERLINE)
        {
            //////处理非powerline设备
            if(tmpNodeList[nIdx]->m_parent.count() <= 1)
            {
                continue;
            }

            int tmp = tmpNodeList[nIdx]->m_parent[1].toInt();


            tmpEdgeList[nIdx]->m_headPt = tmpEdgeList[tmp]->m_endPt;


            if(tmpEdgeList[tmp]->m_headPt.x() < tmpEdgeList[tmp]->m_endPt.x())
            {/////父节点在左边
                /*x = tmpEdgeList[tmp]->m_endPt.x() + r1 / 2.1 + tmpNodeList[nIdx]->boundingRect().width() / 2;*/
				x = tmpEdgeList[tmp]->m_endPt.x() + r1 / 2.1 + nWidth / 2;
            }
            else
            {
                //x = tmpEdgeList[tmp]->m_endPt.x() - r1 / 2.1 - tmpNodeList[nIdx]->boundingRect().width() / 2;
				x = tmpEdgeList[tmp]->m_endPt.x() - r1 / 2.1 - nWidth / 2;
            }


            if(tmpNodeList[tmp]->m_son.count() - 1 == 1 || (tmpNodeList[tmp]->m_localDevice && nLocalDevSonOfPc == 1))
            {
                //只有一个子节点
                y = tmpEdgeList[tmp]->m_endPt.y();
            }
            else
            {
                if(tmpNodeList[tmp]->m_isSonNodePaint)
                {

                   // y = tmpEdgeList[tmp]->m_endPt.y() + tmpNodeList[nIdx]->boundingRect().height() / 3;
					y = tmpEdgeList[tmp]->m_endPt.y() + nHeight / 3;

                }
                else
                {
                    //y = tmpEdgeList[tmp]->m_endPt.y() -tmpNodeList[nIdx]->boundingRect().height() / 3;
					y = tmpEdgeList[tmp]->m_endPt.y() - nHeight / 3;
                }

                tmpNodeList[tmp]->m_isSonNodePaint = true;
            }


            tmpEdgeList[nIdx]->m_endPt = QPointF(x, y);

            tmpNodeList[nIdx]->setPos(x, y);
        }
        else
        {
            ////////////处理powerline设备

            //bRet = isCurrentPageIdx(nIdx, nStart, nLen, nCurrenIdxPage);
            //if(!bRet)
            //{////不在当前页，处理下个节点

            //	/////ADD:如若需要画连接到其他页面的线,此处添加代码
            //	continue;
            //}

            int tmp = -1;
            if(tmpNodeList[nIdx]->m_parent.count() > 1)
            {
                tmp = tmpNodeList[nIdx]->m_parent[1].toInt();

                tmpEdgeList[nIdx]->m_S_nodeWidth	= tmpNodeList[nIdx]->shape().boundingRect().width();
                tmpEdgeList[nIdx]->m_S_nodeHeight	= tmpNodeList[nIdx]->shape().boundingRect().height();
                tmpEdgeList[nIdx]->m_P_nodeWidth	= tmpNodeList[tmp]->shape().boundingRect().width();
				tmpEdgeList[nIdx]->m_P_nodeHeight	= tmpNodeList[tmp]->shape().boundingRect().height();
            }



            int Angel = 0;
            if(nIdx == nParentIdx)
            {
                /////Top Node
                x = (1.5 * r1 + tmpNodeList[nIdx]->boundingRect().width()) / 2 - 17;
                y = (2 * r2 + tmpNodeList[nIdx]->boundingRect().height()) / 2 - r2;

                centryX = x;
                centryY = y;


				//Router and Earth position
				if(m_IsRouterOk)
				{
					positionRouterAndEarth(QPointF(x, y));
				}
				

            }
            else
            {
                x = 1.9 * r1 + tmpNodeList[nIdx]->boundingRect().width() / 2;

                nSonPlc = (nSonPlc == 2) ? 3 : nSonPlc;

                if(tmpNodeList[nIdx]->m_parent.count() > 1 && tmpNodeList[nIdx]->m_parent[1].toInt() == nParentIdx)
                {

                    switch(m_LocalDevSonPowel)
                    {
                        case 1:
                        case 3:
                            nTmpSonPlc = nSonPlc;
                            break;
                        case 2:
                            nTmpSonPlc = nTmpSonPlc + nSonPlc + 1;
                            break;
                        default:
                            break;
                    }
                }
                Angel = sin((nTmpSonPlc * 90) * PI / 180);
                Angel = -Angel;
                y = centryY + 3.9 * r2 * Angel;

                tmpEdgeList[nIdx]->m_Angle = nTmpSonPlc * 90 * PI / 180;

                nSonPlc++;
            }

            tmpNodeList[nIdx]->setPos(x, y);


            if(tmp != -1)
            {
                qreal tmpX = 0, tmpY = 0;
                tmpX = tmpEdgeList[tmp]->m_headPt.x();
                if(Angel != 0)
                {
                    tmpY = tmpEdgeList[tmp]->m_headPt.y() - tmpEdgeList[tmp]->m_topPt.y();
                    tmpY = tmpY * Angel + tmpEdgeList[tmp]->m_headPt.y();
                }
                else
                {
                    tmpY = tmpEdgeList[tmp]->m_headPt.y();
                }

                tmpEdgeList[nIdx]->m_headPt = QPointF(tmpX, tmpY);

                tmpEdgeList[nIdx]->m_topPt = tmpEdgeList[tmp]->m_endPt;
                tmpEdgeList[nIdx]->m_bottomPt = tmpEdgeList[tmp]->m_headPt;		//子powerline节点记录父节点的坐标
            }
            else
            {
                ////////LocalDevice Node
                qreal tmpX = 0;


                tmpX = x + r1 / 2;
                tmpEdgeList[nIdx]->m_headPt = QPointF(tmpX, y);

                ////////////暂时没用了
                //tmpEdgeList[nIdx]->m_topTopPt = QPointF(tmpX, y - 7.5 * r2);
                //tmpEdgeList[nIdx]->m_bottomBottomPt = QPointF(tmpX, y + 6.5 * r2);
                //////////End

                switch(m_LocalDevSonPowel)
                {
                    case 1:
                        tmpEdgeList[nIdx]->m_topPt = QPointF(tmpX, y);
                        tmpEdgeList[nIdx]->m_bottomPt = QPointF(tmpX, y);
                        break;
                    case 2:
                    case 3:
                        tmpEdgeList[nIdx]->m_topPt = QPointF(tmpX, centryY + 3.9 * r2 * (-1));
                        tmpEdgeList[nIdx]->m_bottomPt = QPointF(tmpX, centryY + 3.9 * r2);
                        break;
                    case 0:
                    default:
                        tmpEdgeList[nIdx]->m_headPt = QPointF(x, y);
                        tmpEdgeList[nIdx]->m_endPt = QPointF(x, y);
                        tmpEdgeList[nIdx]->m_topPt = QPointF(x, y);
                        tmpEdgeList[nIdx]->m_bottomPt = QPointF(x, y);
                        //tmpEdgeList[nIdx]->m_topTopPt = QPointF(x, y);
                        //tmpEdgeList[nIdx]->m_bottomBottomPt = QPointF(x, y);
                        break;
                }
            }
            tmpEdgeList[nIdx]->m_endPt = QPointF(x, y);

            ++nCurCount;
        }
    }


    ////////锁的图片节点
    m_nodePic->updateShapeCache();
    qreal picXpox = 0, picYpox = 0, tmpXpic = 0;
    picXpox = m_edgesList[nParentIdx]->m_endPt.x();
    picYpox = m_edgesList[nParentIdx]->m_endPt.y();
    tmpXpic = picXpox + m_nodesList[nParentIdx]->boundingRect().width() / 2;
    qreal pixWidth = (m_edgesList[nParentIdx]->m_headPt.x() - tmpXpic) / 2;
    tmpXpic = tmpXpic + pixWidth - m_nodePic->boundingRect().width() / 2;

    m_nodePic->setPos(tmpXpic, picYpox);

    //////////////End
}

//void PowerlineView::verifyInternetNodeEdge()
//{
//    if(!m_InternetNode)
//    {
//        m_InternetNode = new PowerlineNode();
//        m_scene->addItem(m_InternetNode);
//
//    }
//
//    if(!m_InternetEdge)
//    {
//        m_InternetEdge = new PowerlineEdge();
//        m_scene->addItem(m_InternetEdge);
//    }
//
//    //m_InternetNode->m_deviPowerlineEdgenternetEdge->m_deviceType = GDT_INTERNET;
//    m_InternetNode->m_onLine = m_InternetEdge->m_onLine = m_internetConnected;
//    m_InternetNode->m_name = "Internet";
//    m_InternetEdge->m_wireless = false;
//
//    //    m_InternetNode->m_Father = FT_ROUTER;
//
//    m_InternetNode->setVisible(m_nodesList.size() > 0);
//    m_InternetEdge->setVisible(m_nodesList.size() > 0);
//}
void PowerlineView::dealLinkPLCNode(QList<PowerlineNode *> &node, QList<PowerlineEdge *> &edge, int nLen)
{
    //QQueue<PLCNodeData *> tmpQueue;
    //PLCNodeData *tmpDev = NULL;
    QString strSonIdx, strParentIdx;




    if(node.count() == 0)
    {
        return;
    }

    ////initialized dev m_parent and m_son
    for(int i = 0; i < nLen; ++i)
    {
        node[i]->m_parent.clear();
        node[i]->m_son.clear();
        edge[i]->m_parent.clear();
        edge[i]->m_son.clear();

        node[i]->m_parent << "None";
        node[i]->m_son << "None";
        edge[i]->m_parent << "None";
        edge[i]->m_son << "None";
    }
    strParentIdx = QString("%1").arg(0);
    ///////end

    for(int i = 1; i < nLen; ++i)
    {
        strSonIdx = QString("%1").arg(i);
        node[i]->m_parent << strParentIdx;
        edge[i]->m_parent << strParentIdx;

        node[0]->m_son << strSonIdx;
        edge[0]->m_son << strSonIdx;
    }
}
void PowerlineView::onNewData(PLDevice * TopNode, const QList<PLDevice *> &data, int nStart, int nLen/*, int nCurrenIdxPage*/)
{
    int idx = 0;
    int nLength = 0;

    m_cacheData = &data;

    int tmpCount = m_nodesList.size();

	PowerlineMapView *pMapView = PowerlineMapView::GetSingletonPtr();
	Q_ASSERT(pMapView);
	m_IsRouterOk	= pMapView->isRouterOk();

    m_scene->invalidate();

    Q_ASSERT (m_nodesList.size() == m_edgesList.size());

    if(TopNode == NULL)
    {
        for(int k = data.count() ; k < tmpCount; ++k)
        {
            m_scene->removeItem(m_nodesList[data.count()]);
            m_scene->removeItem(m_edgesList[data.count()]);

            delete m_nodesList.takeAt(data.count());
            delete m_edgesList.takeAt(data.count());
        }

        m_nodePic->hide();
        m_nodePic->update();


		createRouterAndEarth();
        return;
    }
    else if(data.count() == 1)
    {
        nLength = 1;
    }
    else
    {
        nLength = (nStart < data.count()) ? (nLen + 1) : (nStart - nLen);
    }

    m_idxStartPos = nStart;

	/////Local Dev idx
	int nLocalIdx = -1;
	////save
	m_lstPlDev = &data;
    ////add powerline
	bool bOk = false;
    for(idx = 0 ; idx < nLength/*data.size()*/ ; ++idx)
    {
        if(idx >= m_nodesList.size())
        {
            PowerlineNode *node=new PowerlineNode();
            /*		node->mFlags |= topologynode::DF_CONNECTED;*/
            m_nodesList.append(node);
            m_edgesList.append(new PowerlineEdge());

            m_scene->addItem(m_nodesList[idx]);
            m_scene->addItem(m_edgesList[idx]);

            connect(m_nodesList[idx],SIGNAL(clicked(PowerlineNode*)),
                    this,SLOT(onNodeClicked(PowerlineNode*)));
            connect(m_nodesList[idx],SIGNAL(contextMenu(PowerlineNode*,QPoint)),
                    this,SLOT(onNodeContextMenu(PowerlineNode*,QPoint)));
            connect(m_nodesList[idx], SIGNAL(nodeMouseOver(int)), this, SLOT(onMouseOverNode(int)));
        }

        m_nodesList[idx]->setVisible(true);
        //m_edgesList[idx]->setVisible(idx != 1);//default router edge not visible

        m_nodesList[idx]->m_Idx = idx;

        if(idx == 0)
        {
            itemsFromDevice(m_nodesList[idx], m_edgesList[idx], TopNode);
            m_TopNode = m_nodesList[idx];
            m_TopNodeIdx = QString("%1").arg(idx);
        }
        else
        {
            itemsFromDevice(m_nodesList[idx], m_edgesList[idx], data[nStart + idx - 1]);
        }


		//Lock's node
		if(!bOk && pMapView->isLocalDev(m_nodesList[idx]->m_devMac))
		{
			nLocalIdx = idx;	//Local Dev Idx
			m_plLocalDevNode = m_nodesList[idx];
			m_plLocalDevEdge = m_edgesList[idx];

			if(m_nodesList[idx]->m_devType == "Public")
			{
				m_nodePic->m_deviceType = GDT_UNLOCKPIC;
				m_nodePic->show();
			}
			else if(m_nodesList[idx]->m_devType == "Private")
			{
				m_nodePic->m_deviceType = GDT_LOCKPIC;
				m_nodePic->show();
			}

			bOk = true;
		}
		//end
    }


    dealLinkPLCNode(m_nodesList, m_edgesList, nLength);

    m_LocalDevSonPowel = m_TopNode->m_son.count() - 1;
    m_TopNode->m_localDevice = true;
    m_edgesList[0]->m_bLocalDevice = true;

    //if(m_TopNode->m_devType == "Public")
    //{
    //    m_nodePic->m_deviceType = GDT_UNLOCKPIC;
    //    m_nodePic->show();
    //}
    //else if(m_TopNode->m_devType == "Private")
    //{
    //    m_nodePic->m_deviceType = GDT_LOCKPIC;
    //    m_nodePic->show();
    //}
    if(m_LocalDevSonPowel == 0)
    {
        m_nodePic->hide();
        m_nodePic->update();
    }



    /////add other device
    QList<AttachDeviceEntry>		tmpAttaList;
    QList<QPair<QString,QString> >	tmpUsbList;
    QString strIdx;
    int nDx = 0;
    //int nWireless = 0, nWired = 0;
    int nDataIdx = 0;
    int nParentIdx = 0;


    for(int i = 0; i < nLength/*data.size()*/; ++i)
    {
        //tmpStrList = (*(data[nStart + i - 1]))[GDP_ATTACH_DEV].toStringList();
        if(i == 0)
        {
            nDataIdx = i;
            nParentIdx = i;
        }
        else
        {
            nDataIdx = i + nStart - 1;
            nParentIdx = i;
        }

    //    tmpAttaList = (data[nDataIdx])->m_attachDeviceList;
    //    if(tmpAttaList.count() < 1)
    //    {
    //        //continue;
    //    }
    //    else
    //    {

    //        ////////
    //        nWired		= getWiredTotal(tmpAttaList);
    //        nWireless	= getWirelessTotal(tmpAttaList);

    //        if(nWired != 0)
    //        {
				//createNormalNode(i, idx + nDx, nWired, tmpAttaList, false);
				//++nDx;
    //        }

    //        if(nWireless != 0)
    //        {
    //            createNormalNode(i, idx + nDx, nWireless, tmpAttaList, true);
    //            ++nDx;
    //        }
    //    }

        //tmpStrList = (*(data[nStart + i - 1]))[GDP_ATTACH_DEV].toStringList();
        tmpUsbList = (data[nDataIdx])->m_attachUsbDevice;
        if(tmpUsbList.count() < 1)
        {
            continue;
        }

        //if(nWireless != 0 && nWired != 0)
        //{
        //    continue;
        //}
       
		//createUsbNode(i, idx + nDx, tmpUsbList);
		if(tmpUsbList.count() > 1)
		{
			//Hub
			bool bHub = false;
			qDebug() << tmpUsbList;
			for(int j = 0; j < tmpUsbList.count(); ++j)
			{

				if(tmpUsbList[j].first == "Hub")
				{
					if(!bHub)
					{
						bHub = true;
						continue;
					}
					
					tmpUsbList.removeAt(j);
					j--;
				}
			}
			createNormalNode(i, idx + nDx, 0, tmpUsbList, GDT_HUB);
		}
		else
		{
			//Only one Element
			QString strDevType = tmpUsbList.at(0).first;
			DEVICE_TYPE_ nDevType = getDevType(strDevType);
			createNormalNode(i, idx + nDx, 0, tmpUsbList, nDevType);
		}
		
        ++nDx;
    }

	//////Create Local Dev
	if(nLocalIdx != -1)
	{
		createLocalDevNode(nLocalIdx, idx + nDx, GDT_LOCALDEV);
		++nDx;
	}
	//////End

	idx += nDx;
    tmpCount = m_nodesList.size();
    for(int k = idx ; k < tmpCount; ++k)
    {
        m_scene->removeItem(m_nodesList[idx]);
        m_scene->removeItem(m_edgesList[idx]);

        delete m_nodesList.takeAt(idx);
        delete m_edgesList.takeAt(idx);
    }



    if(TopNode != NULL && nLength != 0)
    {
		createRouterAndEarth();
        positionItems(nStart, nLen/*, nCurrenIdxPage*/);
    }


    m_scene->invalidate();
    update();

    //PowerlineMapView *pMapView = PowerlineMapView::GetSingletonPtr();
    //Q_ASSERT(pMapView);
    //if(pMapView->isShow())
    //{
    //	preWirelessConfiguration(data);
    //}

    preWirelessConfiguration();

}
void PowerlineView::createLocalDevNode(int nParent, int nCurIdx, DEVICE_TYPE_ nType)
{
	/////create local device

	Q_ASSERT(m_scene);

	QString strIdx;
	if((nCurIdx) >= m_nodesList.size())
	{
		m_nodesList.append(new PowerlineNode());
		m_edgesList.append(new PowerlineEdge());

		m_scene->addItem(m_nodesList[nCurIdx]);
		m_scene->addItem(m_edgesList[nCurIdx]);

		connect(m_nodesList[nCurIdx],SIGNAL(clicked(PowerlineNode*)),
			this,SLOT(onNodeClicked(PowerlineNode*)));
		connect(m_nodesList[nCurIdx],SIGNAL(contextMenu(PowerlineNode*,QPoint)),
			this,SLOT(onNodeContextMenu(PowerlineNode*,QPoint)));
		connect(m_nodesList[nCurIdx], SIGNAL(nodeMouseOver(int)), this, SLOT(onMouseOverNode(int)));
	}

	m_nodesList[nCurIdx]->setVisible(true);
	m_edgesList[nCurIdx]->setVisible(true);


	m_nodesList[nCurIdx]->m_isSonNodePaint = false;
	m_nodesList[nCurIdx]->m_Idx = nCurIdx;

	strIdx = QString("%1").arg(nCurIdx);
	m_nodesList[nParent]->m_son << strIdx;
	m_edgesList[nParent]->m_son << strIdx;
	strIdx = QString("%1").arg(nParent);
	m_nodesList[nCurIdx]->m_parent.clear();
	m_nodesList[nCurIdx]->m_parent << "None" << strIdx;

	m_nodesList[nCurIdx]->m_son.clear();
	m_nodesList[nCurIdx]->m_son << "None";
	m_nodesList[nCurIdx]->m_deviceType = nType;
	m_nodesList[nCurIdx]->m_isPowerLine = false;
//	m_nodesList[nCurIdx]->m_devAttCount = nNum;
	m_nodesList[nCurIdx]->updateShapeCache();
	m_nodesList[nCurIdx]->m_localDevice = false;
	m_nodesList[nCurIdx]->m_bPicUseDefault = false;
	//m_nodesList[nCurIdx]->m_attachUsbDev = data;
	m_nodesList[nCurIdx]->m_devName.clear();


	m_edgesList[nCurIdx]->m_parent.clear();
	m_edgesList[nCurIdx]->m_son.clear();
	m_edgesList[nCurIdx]->m_parent = m_nodesList[nCurIdx]->m_parent;
	m_edgesList[nCurIdx]->m_son = m_nodesList[nCurIdx]->m_son;
	m_edgesList[nCurIdx]->m_deviceType = nType;
	m_edgesList[nCurIdx]->m_isUsb = false;
	m_edgesList[nCurIdx]->m_bLocalDevice = false;
}
void PowerlineView::createNormalNode(int nParent, int nCurIdx, int nNum, QList<QPair<QString,QString> > &data, DEVICE_TYPE_ nType)
{
	QString strIdx;

	if(( nCurIdx) >= m_nodesList.size())
	{
		PowerlineNode *node=new PowerlineNode();
		/*	node->mFlags |= topologynode::DF_CONS;*/
		m_nodesList.append(node);
		m_edgesList.append(new PowerlineEdge());

		m_scene->addItem(m_nodesList[nCurIdx]);
		m_scene->addItem(m_edgesList[nCurIdx]);

		/*if(nParent != 0)
		{*/
			connect(m_nodesList[nCurIdx],SIGNAL(clicked(PowerlineNode*)),
				this,SLOT(onNodeClicked(PowerlineNode*)));
			connect(m_nodesList[nCurIdx],SIGNAL(contextMenu(PowerlineNode*,QPoint)),
				this,SLOT(onNodeContextMenu(PowerlineNode*,QPoint)));
			connect(m_nodesList[nCurIdx], SIGNAL(nodeMouseOver(int)), this, SLOT(onMouseOverNode(int)));
		/*}*/

	}


	m_nodesList[nCurIdx]->setVisible(true);
	m_edgesList[nCurIdx]->setVisible(true);

	m_nodesList[nCurIdx]->m_isSonNodePaint = false;

	m_nodesList[nCurIdx]->m_Idx = nCurIdx;

	strIdx = QString("%1").arg(nCurIdx);

	m_nodesList[nParent]->m_son << strIdx;
	m_edgesList[nParent]->m_son << strIdx;
	strIdx = QString("%1").arg(nParent);

	m_nodesList[nCurIdx]->m_parent.clear();
	m_nodesList[nCurIdx]->m_parent << "None" << strIdx;

	m_nodesList[nCurIdx]->m_son.clear();
	m_nodesList[nCurIdx]->m_son << "None";
	m_nodesList[nCurIdx]->m_deviceType = nType;
	m_nodesList[nCurIdx]->m_isPowerLine = false;
	m_nodesList[nCurIdx]->m_devAttCount = nNum;
	m_nodesList[nCurIdx]->updateShapeCache();
	m_nodesList[nCurIdx]->m_localDevice = false;
	m_nodesList[nCurIdx]->m_bPicUseDefault = false;
	//m_nodesList[nCurIdx]->m_localHostDevPc = (nParent == 0) ? true : false;
	//m_nodesList[nCurIdx]->m_bLocalHostDevPcOn = (m_localDevPCState != LOS_INIT) ? ((m_localDevPCState == LOS_ON) ? true : false) : true;

	m_nodesList[nCurIdx]->m_attachUsbDev = data;
	m_nodesList[nCurIdx]->m_devName.clear();


	m_edgesList[nCurIdx]->m_parent.clear();
	m_edgesList[nCurIdx]->m_son.clear();
	m_edgesList[nCurIdx]->m_parent = m_nodesList[nCurIdx]->m_parent;
	m_edgesList[nCurIdx]->m_son = m_nodesList[nCurIdx]->m_son;
	m_edgesList[nCurIdx]->m_deviceType = nType;
	m_edgesList[nCurIdx]->m_isUsb = true;
	//m_edgesList[nCurIdx]->m_wireless  = bWireless ? true : false;
	//m_edgesList[nCurIdx]->m_signalLevel = bWireless ? 4 : 0;
	//m_edgesList[nCurIdx]->m_signalStrength = bWireless ? "60%" : "";
	m_edgesList[nCurIdx]->m_bLocalDevice = false;
}

void PowerlineView::nodeOnlineStateChange(bool b)
{
	if(m_nodesList.count() == 0)
	{
		////save state
		if(b)
		{
			m_localDevPCState = LOS_ON;
		}
		else
		{
			m_localDevPCState = LOS_OFF;
		}
		//////end
		return;
	}

	m_localDevPCState = LOS_INIT;
	foreach(PowerlineNode *node, m_nodesList)
	{
		if(node->m_localHostDevPc)
		{
			node->m_bLocalHostDevPcOn = b;
			node->update();

			break;
		}
	}
	
};
void PowerlineView::createUsbNode(int nParent, int nCurIdx, QList<QPair<QString,QString> > &data)
{

	Q_UNUSED(data)
	Q_UNUSED(nParent)
	Q_UNUSED(nCurIdx)
	/*QString strIdx;

	if((nCurIdx) >= m_nodesList.size())
	{
		m_nodesList.append(new PowerlineNode());
		m_edgesList.append(new PowerlineEdge());

		m_scene->addItem(m_nodesList[nCurIdx]);
		m_scene->addItem(m_edgesList[nCurIdx]);

		connect(m_nodesList[nCurIdx],SIGNAL(clicked(PowerlineNode*)),
			this,SLOT(onNodeClicked(PowerlineNode*)));
		connect(m_nodesList[nCurIdx],SIGNAL(contextMenu(PowerlineNode*,QPoint)),
			this,SLOT(onNodeContextMenu(PowerlineNode*,QPoint)));
		connect(m_nodesList[nCurIdx], SIGNAL(nodeMouseOver(int)), this, SLOT(onMouseOverNode(int)));

	}


	m_nodesList[nCurIdx]->setVisible(true);
	m_edgesList[nCurIdx]->setVisible(true);
	m_nodesList[nCurIdx]->m_isSonNodePaint = false;

	m_nodesList[nCurIdx]->m_Idx = nCurIdx;

	strIdx = QString("%1").arg(nCurIdx);

	m_nodesList[nParent]->m_son << strIdx;
	m_edgesList[nParent]->m_son << strIdx;
	strIdx = QString("%1").arg(nParent);

	m_nodesList[nCurIdx]->m_parent.clear();
	m_nodesList[nCurIdx]->m_parent << "None" << strIdx;

	m_nodesList[nCurIdx]->m_son.clear();
	m_nodesList[nCurIdx]->m_son << "None";
	m_nodesList[nCurIdx]->m_deviceType = GDT_NETWORKUSB;
	m_nodesList[nCurIdx]->m_isPowerLine = false;
	m_nodesList[nCurIdx]->m_devAttCount = data.count();
	m_nodesList[nCurIdx]->updateShapeCache();
	m_nodesList[nCurIdx]->m_localDevice = false;
	m_nodesList[nCurIdx]->m_bPicUseDefault = false;

	m_nodesList[nCurIdx]->m_attachUsbDev = data;


	m_edgesList[nCurIdx]->m_parent.clear();
	m_edgesList[nCurIdx]->m_son.clear();
	m_edgesList[nCurIdx]->m_parent = m_nodesList[nCurIdx]->m_parent;
	m_edgesList[nCurIdx]->m_son = m_nodesList[nCurIdx]->m_son;
	m_edgesList[nCurIdx]->m_deviceType = GDT_NETWORKUSB;
	m_edgesList[nCurIdx]->m_wireless  = false;
	m_edgesList[nCurIdx]->m_bLocalDevice = false;*/
}
void PowerlineView::preWirelessConfiguration()
{
    Q_ASSERT(m_cacheData && "PowerlineView::preWirelessConfiguration1");
    PowerlineMapView *pMapView = PowerlineMapView::GetSingletonPtr();
    Q_ASSERT(pMapView && "PowerlineView::preWirelessConfiguration2");
    qDebug () << "cdt1 = " << pMapView->isShow() << "cdt2 = " << pMapView->isWirelessShowed();
    if((!pMapView->isShow()) || (pMapView->isWirelessShowed()))
    {
        qDebug () << "!pMapView->isShow()) || (pMapView->isWirelessShowed()" <<
                     " cdt1 = " << pMapView->isShow() << "cdt2 = " << pMapView->isWirelessShowed();
        return;
    }
    else
    {
        qDebug () << "preWirelessConfiguration";
        preWirelessConfiguration((*m_cacheData));
        
    }

}
void PowerlineView::preWirelessConfiguration(const QList<PLDevice *> &lstDev)
{
    DialogMgr *pDlgMgr = DialogMgr::GetSingletonPtr();
    Q_ASSERT(pDlgMgr);
	PowerlineMapView *pMapView = PowerlineMapView::GetSingletonPtr();
	Q_ASSERT(pMapView && "PowerlineView::preWirelessConfiguration2");



//    bool b = pDlgMgr->isFirstStartup();
    if(m_bFirstStart)
    {
        return;
    }


    int nRet = -1;

    QVariant tmp = POWERLINE_GET_SETTING(CONFIG_DEFAULT_KEY);
    int nKey = tmp.toInt();
    if(nKey == 1)
    {
        //Checked
        return;
    }

    QString strTitle = POWERLINE_GET_TEXT(L_POWERLINE_WIRELESS_TITLE, "Wireless Configuration");
    QString strTmp = POWERLINE_GET_TEXT(L_POWERLINE_WIRELESS_MSGBOX_EXPLAIN, "The wizard found an unconfigured %model (%s). Do you want to configure the wireless settings for this device?");
    QString strChkBox = POWERLINE_GET_TEXT(L_POWERLINE_WIRELESS_MSGBOX_CHKBOX, "Don't show this message again ");

#ifndef TEST_PL_CODE
	quint32 &flags = QGenieIface2ShareObject::sSingleInstance->mFlags;
#endif

	foreach(PLDevice *pDev, lstDev)
    {
#ifndef TEST_PL_CODE
		 if(flags & SOF_EXIT_DIALOG_ON)
		 {
			 break;
		 }
#endif

        if(pDev->m_wlanInfo.m_wpsConfigured != WLANInfo::WPS_NOT_CONFIGURED)
        {
            continue;
        }

		 pMapView->initShowState();

        QString strMac = (*pDev)[GDP_PHYSICALADDR].toString();
        strMac.replace("-", ":");
        QString strModel = (*pDev)[GDP_MODEL_NAME].toString();
        QString strContent = strTmp;

        strContent.replace("%model", strModel);
        strContent.replace("%s", strMac);

        nRet = pDlgMgr->showMsgBox(strTitle, strContent, strChkBox, PL_MB_OK | PL_MB_CANCEL);
        int nRetNum = pDlgMgr->getChkboxState();
        if((nRetNum & PL_MB_CHKED) && (nRetNum & PL_MB_CANCEL))
        {
            POWERLINE_SET_SETTING(CONFIG_DEFAULT_KEY, 1);
            break;
        }
        if(nRet & PL_MB_CANCEL)
        {
            continue;
        }
        else
        {
            pDlgMgr->setOperIp((*pDev)[GDP_IP].toString());
            pDlgMgr->showWirelessSet((*pDev)[GDP_MODEL_NAME].toString(), pDev->m_wlanInfo);
        }
    }

    m_bFirstStart = true;
}
void PowerlineView::onInternetStateChanged(bool connected)
{
    m_IsInternetConnected = connected;

    if(m_InternetNode)
    {
        m_InternetNode->m_onLine = m_IsInternetConnected;
        m_InternetNode->update();
    }

    if(m_InternetEdge)
    {
        m_InternetEdge->m_onLine = m_IsInternetConnected;
        m_InternetEdge->update();
    }
}

void PowerlineView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
}

//void topologyview::timerEvent(QTimerEvent *event)
//{
//    if(m_routerRefreshTimerId == event->timerId())
//    {
//        if((m_nodesList.size() >= 2) && (m_nodesList[1] != NULL))
//        {
//            m_nodesList[1]->update();
//        }
//    }
//}


void PowerlineView::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange)
    {
        retranslateUi();
    }

    QGraphicsView::changeEvent(event);
}
QString PowerlineView::translateText(int idx, const QString &defaultText)
{
    QString text;
    emit translateText(idx,&text);
    return text.isEmpty() ? defaultText : text;
}

void PowerlineView::onNodeClicked(PowerlineNode *node)
{
    int idx = m_nodesList.indexOf(node);
	

    qDebug() << "Node " << idx << "clicked";
    if(idx == -1)
    {
        return;
    }

	qDebug() << "node Name: " << node->m_devModelName;
	DialogMgr *dlgMgr = DialogMgr::GetSingletonPtr();
	Q_ASSERT(dlgMgr);
    if(node->m_deviceType == GDT_HUB && node->m_parent.count() > 1)
    {
		PowerlineMapView *pMapView = PowerlineMapView::GetSingletonPtr();
		Q_ASSERT(pMapView);
		qDebug() << node->m_attachUsbDev.count();
		if(node->m_attachUsbDev.count() > 1)
		{
			int nParent = node->m_parent[1].toInt();

			pMapView->showHubView(m_nodesList[nParent], m_edgesList[nParent], node->m_attachUsbDev);
		}
    }
	else if(node->m_deviceType == GDT_POWERLINE)
	{
		qDebug() << "node Name: " << node->m_devModelName;
		if(m_lstPlDev->count() == 1 || node->m_devModelName == POWERLINE_UNKNOWN_DEV)
		{
			return;
		}

		
		//dlgMgr->showDlgRate(node, m_edgesList[idx], *m_lstPlDev);
		Q_ASSERT(m_plLocalDevNode && m_plLocalDevEdge);
		dlgMgr->showDlgRate(m_plLocalDevNode, m_plLocalDevEdge, *m_lstPlDev);
	}
    else
    {
        emit nodeClicked(idx);
    }
}

void PowerlineView::onNodeContextMenu(PowerlineNode *node,const QPoint &pos)
{
    int idx = m_nodesList.indexOf(node);

    qDebug() << "Node " << idx << "context menu";

    if(idx == -1)
    {
        return;
    }

	//if(node->m_deviceType == GDT_LOCALDEV)
	//{
	//	return;
	//}

    if(idx == 0)
    {
        ////跟路由器连接的设备
        emit nodeContextMenu(idx, pos);
    }
    else
    {
        emit nodeContextMenu(m_idxStartPos + idx - 1,pos);
    }
}


bool PowerlineView::isCurrentPageIdx(int idx, int nStart, int nLen, int nBase)
{
    if(idx == 0)
    {
        /////默认规定0为LocalDevice,应该在每页出现
        return true;
    }

    if((nBase + idx < nStart) || (nBase + idx > (nStart + nLen - 1)))
    {
        return false;
    }

    return true;
}


void PowerlineView::onMouseOverNode(int idx)
{
    //this->invalidateScene();	//现在是全部重画，有时间应该在每个node和edge中写个重画函数调用
    if(idx == -1 && m_MouseOverNodeIdx != -1)
    {
        m_edgesList[m_MouseOverNodeIdx]->update(/*m_edgesList[m_MouseOverNodeIdx]->boundingRect()*/);
        m_edgesList[m_MouseOverNodeIdx]->m_bArrow = false;
        m_MouseOverNodeIdx = idx;
    }
    //else if(idx == -2)
    //{
    //	m_nodePic->update();
    //}
    else if(idx >= 0 && idx <= (m_nodesList.count() - 1))
    {
        m_edgesList[idx]->update(/*m_edgesList[idx]->boundingRect()*/);
        //m_nodesList[idx]->update();
        m_MouseOverNodeIdx = idx;
        m_edgesList[idx]->m_bArrow = true;
    }



}


int PowerlineView::getWirelessTotal(QList<AttachDeviceEntry> &data)
{
    int nLen = 0;
    int nCount = 0;

    nLen = data.count();
    for(int i = 0; i < nLen; ++i)
    {
        if(!data[i].m_isWired)
        {
            ++nCount;
        }
    }

    return nCount;
}
int PowerlineView::getWiredTotal(QList<AttachDeviceEntry> &data)
{
    int nLen = 0;
    int nCount = 0;

    nLen = data.count();
    for(int i = 0; i < nLen; ++i)
    {
        if(data[i].m_isWired)
        {
            ++nCount;
        }
    }

    return nCount;
}

void PowerlineView::updateBtn()
{
	Q_ASSERT(m_preBtnProxy && m_nextBtnProxy);

	//m_preBtnProxy->update();
	//m_nextBtnProxy->update();
	m_prevPageBtn->update();
	m_nextPageBtn->update();

}
void PowerlineView::BtnClick()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());

	

    if(btn == m_prevPageBtn)
    {
        emit btnClicked(0);
		
    }
    else if(btn == m_nextPageBtn)
    {
		
        emit btnClicked(1);
		
    }

	
}
void PowerlineView::btnSetShow(bool bShow, BTN_IDX nBtn, bool bEnable)
{

    m_nextPageBtn->setVisible(bShow);
    m_prevPageBtn->setVisible(bShow);
    switch(nBtn)
    {
        case BI_PRE_BTN:
            m_prevPageBtn->setEnabled(bEnable);
            break;
        case BI_NEXT_BTN:
            m_nextPageBtn->setEnabled(bEnable);
            break;
        default:
            break;
    }

	updateBtn();
}
