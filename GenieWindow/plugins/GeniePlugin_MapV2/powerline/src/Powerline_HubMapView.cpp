#include "Powerline_HubMapView.h"
#include "PowerlineView.h"

#define D_TOPOLOGYVIEW_STYLE	"border-image:url(:/PowerLineImage/bg_plugin_page.png) 14;"

Powerline_HubMapView::Powerline_HubMapView(QWidget *parent) :
    QGraphicsView(parent)
	,m_scene(NULL)
	,m_TopEdge(NULL)
	,m_TopNode(NULL)
	,m_flags(0)
	,m_totalPage(0)
	,m_curPageIdx(1)
	,m_devTotalCount(0)
{
    //ui->setupUi(this);
	
	createGraphics();
	//initSlot();
	//initStyle();
	//initLanguage();
}

Powerline_HubMapView::~Powerline_HubMapView()
{

	for(int i = 0; i < m_plNode.count(); ++i)
	{
		m_scene->removeItem(m_plNode[0]);
		m_scene->removeItem(m_plEdge[0]);

		delete m_plNode.takeAt(0);
		delete m_plEdge.takeAt(0);
	}

	if(m_TopNode!= NULL && m_TopEdge != NULL)
	{
		m_scene->removeItem(m_TopNode);
		m_scene->removeItem(m_TopEdge);
	}
	POWERLINE_SAFEDEL(m_TopNode);
	POWERLINE_SAFEDEL(m_TopEdge);
}

void Powerline_HubMapView::delAllData()
{
	for(int i = 0; i < m_plNode.count(); ++i)
	{
		m_scene->removeItem(m_plNode[0]);
		m_scene->removeItem(m_plEdge[0]);

		delete m_plNode.takeAt(0);
		delete m_plEdge.takeAt(0);
	}

	if(m_TopNode!= NULL && m_TopEdge != NULL)
	{
		m_scene->removeItem(m_TopNode);
		m_scene->removeItem(m_TopEdge);
	}

	POWERLINE_SAFEDEL(m_TopNode);
	POWERLINE_SAFEDEL(m_TopEdge);

	m_pAttDeviceUsb.clear();
}
//===============================================
//  initialization funtion
//===============================================
void Powerline_HubMapView::initLanguage()
{
	//POWERLINE_TRANSLATE_LABEL(ui->label_title, L_POWERLINE_DETAIL_TITLE, "Device List");
	//POWERLINE_TRANSLATE_BUTTON(ui->but_close, L_POWERLINE_BUT_CLOSE, "Close");
}
void Powerline_HubMapView::createGraphics()
{
	//m_view	= new QGraphicsView(ui->frm_view);
	m_scene	= new QGraphicsScene(this);



	setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	setAttribute(Qt::WA_TranslucentBackground);
	setRenderHint(QPainter::Antialiasing);
	setCacheMode(QGraphicsView::CacheBackground);
	setStyleSheet(QString(D_TOPOLOGYVIEW_STYLE));
	//m_scene->setSceneRect(QRectF(0, 0, 600, 230));
	m_scene->setSceneRect(QRectF(-270, -115, 540, 230));
	//m_scene->setBackgroundBrush(Qt::yellow);
	m_scene->setItemIndexMethod(QGraphicsScene::NoIndex);
	
	setScene(m_scene);
	
	setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

}
void Powerline_HubMapView::initStyle()
{


	/////////////////////////////////////////////////////////////////
	//QFile f(/*GENIE2_RES(*/":styles/powerlineview.qss")/*)*/;
	//bool btemp=f.open(QIODevice::ReadOnly);
	//Q_ASSERT(btemp);
	//QString stemp;
	//stemp=f.readAll();
	//f.close();

	//ui->prevPageBtn->setStyleSheet(stemp);
	//ui->nextPageBtn->setStyleSheet(stemp);
}
void Powerline_HubMapView::initSlot()
{
	
}



void Powerline_HubMapView::pageBtnClick(int nCurPageIdx)
{
	m_curPageIdx = nCurPageIdx;

		if(m_curPageIdx > 1)
		{
			SubScribeData();
		}


		if(m_curPageIdx < m_totalPage)
		{
			SubScribeData();
		}


	updateUiElements();
}
//====================================================
//  public function
//====================================================
void Powerline_HubMapView::initializeValue(const QList<QPair<QString,QString> > &lstUsbDev)
{
	//m_pAttDeviceUsb = &lstUsbDev;

	for(int i = 0; i < lstUsbDev.count(); ++i)
	{
		if((lstUsbDev)[i].first == "Hub")
		{
			continue;
		}


		m_pAttDeviceUsb.append(lstUsbDev[i]);
	}

	int nStart = 0, nEnd = 0;
	beforeUpdateUi(&nStart, &nEnd, m_pAttDeviceUsb.count());
	onSubscribData(nStart, nEnd);

}

void Powerline_HubMapView::initializeTopNode(PowerlineNode *node, PowerlineEdge *edge)
{

	if(m_TopEdge == NULL)
	{
		m_TopEdge = new PowerlineEdge();
	}
	if(m_TopNode == NULL)
	{
		m_TopNode = new PowerlineNode();
	}

	//(*m_TopEdge) = (*edge);
	(*m_TopNode) = (*node);
	(*m_TopEdge) = (*edge);

//	m_TopEdge->m_bDetailDlgParent = true;
	m_TopEdge->m_bHubView = true;
	m_TopNode->m_bHubView = true;

	m_TopNode->setVisible(true);

	m_scene->addItem(m_TopNode);
	m_scene->addItem(m_TopEdge);

	m_TopNode->updateShapeCache();

	//initializeValue(m_TopNode->m_attachUsbDev);
	//ui->but_close->setFocus();
	
}

//====================================================
//  other function
//====================================================
void Powerline_HubMapView::itemFromData(PowerlineNode *node, PowerlineEdge *edge, const QPair<QString,QString> &addUsb)
{

	QString strType = addUsb.first;
	PowerlineView::getDevType(strType);

	node->m_deviceType = PowerlineView::getDevType(strType);;
	//node->m_devName = addUsb.second;			////不要求显示名字
	node->m_devName.clear();
	node->m_bHubView = true;
	
	edge->m_isUsb = true;
	edge->m_deviceType = node->m_deviceType;
	edge->m_devName = node->m_devName;
	edge->m_bHubView = true;
}
void Powerline_HubMapView::delRedundanceNodes(int nCurIdx, int nCnt)
{
	for(int k = nCurIdx ; k < nCnt; ++k)
	{
		m_scene->removeItem(m_plNode[nCurIdx]);
		m_scene->removeItem(m_plEdge[nCurIdx]);

		delete m_plNode.takeAt(nCurIdx);
		delete m_plEdge.takeAt(nCurIdx);
	}
}
void Powerline_HubMapView::createNewNodes(int nCurIdx)
{
	if(nCurIdx >= m_plNode.count())
	{
		m_plNode.append(new PowerlineNode());
		m_plEdge.append(new PowerlineEdge());

		m_scene->addItem(m_plNode[nCurIdx]);
		m_scene->addItem(m_plEdge[nCurIdx]);

		connect(m_plNode[nCurIdx],SIGNAL(clicked(PowerlineNode*)),
			this,SLOT(onNodeClicked(PowerlineNode*)));
		connect(m_plNode[nCurIdx],SIGNAL(contextMenu(PowerlineNode*,QPoint)),
			this,SLOT(onNodeContextMenu(PowerlineNode*,QPoint)));
		connect(m_plNode[nCurIdx], SIGNAL(nodeMouseOver(int)), this, SLOT(onMouseOverNode(int)));
	}

	m_plNode[nCurIdx]->setVisible(true);

	m_plNode[nCurIdx]->updateShapeCache();

}

void Powerline_HubMapView::positionItem()
{
	Q_ASSERT(m_TopNode && m_TopEdge);

	int nCount	= m_plNode.count();
	
	if(nCount < 1)
	{
		qDebug() << "HubMapView Node list too small, size = " << nCount;
		return;
	}

	qreal Angle		= qreal(360) / nCount;
	const qreal PI	= 3.14159265358979323846;
#ifdef TEST_PL_CODE
	qreal r1		= qreal(540 / 3);
	qreal r2		= qreal(230 / 3);
#else
	qreal r1		= qreal(size().width() / 3);
	qreal r2		= qreal(size().height() / 3);
#endif
	qreal fCenterX	= 0;
	qreal fCenterY	= 0;

	m_TopNode->setPos(fCenterX, fCenterY);
	m_TopEdge->m_headPt	= m_TopEdge->m_endPt = QPointF(fCenterX, fCenterY);

	if(nCount > PER_PAGE_NUM)
	{
		qDebug() << "Powerline_HubView::positionItem(): nCount > PER_PAGE_NUM, so failed!";

		return;
	}

	for(int i = 0; i < nCount; ++i)
	{
		qreal x	= r1 * cos(i * Angle * PI / 180);
		qreal y = r2 * sin(i * Angle * PI / 180);

		m_plNode[i]->setPos(x, y);
		m_plEdge[i]->m_endPt	= QPointF(x, y);
		m_plEdge[i]->m_headPt	= QPointF(fCenterX, fCenterY);	
	}	
}
void Powerline_HubMapView::updateUiElements()
{    
	//bool bShow = false;

	//if(m_totalPage >= 2 && m_curPageIdx >= 1)
	//{
	//	bShow = true;
	//}
	//else
	//{
	//	bShow = false;
	//}

	//ui->nextPageBtn->setVisible(bShow);
	//ui->prevPageBtn->setVisible(bShow);

	//ui->nextPageBtn->setEnabled(m_curPageIdx < m_totalPage);
	//ui->prevPageBtn->setEnabled(m_curPageIdx > 1);

}
void Powerline_HubMapView::onSubscribData(int nStart, int nEnd)
{
	Q_ASSERT(m_plNode.count() == m_plEdge.count());
	

	m_scene->invalidate();
	/////建立新節點
	if(nStart > nEnd)
	{
		qDebug() << "Powerline_HubMapView::onSubscribData:   nStart > nEnd ,so failed!";
		return;
	}
	int nCount = nEnd + 1 - nStart;
	int i = 0;
	int idx = 0;
	int tmpStart = nStart;


		//Q_ASSERT(m_pAttDeviceUsb);


		for(i = 0; i < nCount; ++i)
		{
			//if((*m_pAttDeviceUsb)[tmpStart].first == "Hub")
			//{
			//	tmpStart++;
			//	continue;
			//}

			createNewNodes(idx);
			itemFromData(m_plNode[idx], m_plEdge[idx], (m_pAttDeviceUsb)[tmpStart++]);
			++idx;
		}


	



	////刪除多餘節點
	nCount = m_plNode.count();
	delRedundanceNodes(idx, nCount);


	////display nodes and edges
	positionItem();
}

void Powerline_HubMapView::beforeUpdateUi(int *pStart, int *pEnd, int nCnt)
{
	int nTmp = 0;

	m_curPageIdx = 1;

	m_devTotalCount = nCnt;
	nTmp = m_devTotalCount / PER_PAGE_NUM;
	m_modNum = m_devTotalCount % (PER_PAGE_NUM);
	m_totalPage	= (m_modNum == 0) ? nTmp : (nTmp + 1);

	emit pageTotal(m_totalPage);

	updateUiElements();

	dealPostionStartAndEnd(pStart, pEnd, m_modNum);
}
void Powerline_HubMapView::dealPostionStartAndEnd(int *pStart, int *pEnd, int nModeNum)
{
	*pStart	= PER_PAGE_NUM * (m_curPageIdx - 1);
	*pEnd	= *pStart + PER_PAGE_NUM - 1;
	*pEnd = (m_curPageIdx == m_totalPage && nModeNum != 0) ? (*pStart + nModeNum - 1): (*pEnd);
}


void Powerline_HubMapView::SubScribeData()
{

	int nStart = 0, nEnd = 0;
	dealPostionStartAndEnd(&nStart, &nEnd, m_modNum);


		onSubscribData(nStart, nEnd);



}

void Powerline_HubMapView::retranslateUi()
{

}


void Powerline_HubMapView::resizeEvent(QResizeEvent *event)
{
	Q_UNUSED(event)
	positionItem();
}