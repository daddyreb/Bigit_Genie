#include "Powerline_DetailDlg.h"
#include "ui_Powerline_DetailDlg.h"
#include "tmpDefine.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QFile>
#include <QDebug>


#define D_TOPOLOGYVIEW_STYLE	"border-image:url(:/PowerLineImage/bg_plugin_page.png) 14;"

Powerline_DetailDlg::Powerline_DetailDlg(QWidget *parent) :
    QDialog(parent)
    ,ui(new Ui::Powerline_DetailDlg)
	,m_scene(NULL)
	,m_bMove(false)
	,m_TopEdge(NULL)
	,m_TopNode(NULL)
	,m_flags(0)
{
    ui->setupUi(this);
	
	createGraphics();
	initSlot();
	createControlPannel();
	initStyle();
	initLanguage();
}

Powerline_DetailDlg::~Powerline_DetailDlg()
{
    delete ui;

	for(int i = 0; i < m_plNode.count(); ++i)
	{
		m_scene->removeItem(m_plNode[0]);
		m_scene->removeItem(m_plEdge[0]);

		delete m_plNode.takeAt(0);
		delete m_plEdge.takeAt(0);
	}

	m_scene->removeItem(m_TopNode);
	m_scene->removeItem(m_TopEdge);
	POWERLINE_SAFEDEL(m_TopNode);
	POWERLINE_SAFEDEL(m_TopEdge);
}


//===============================================
//  initialization funtion
//===============================================
void Powerline_DetailDlg::initLanguage()
{
	POWERLINE_TRANSLATE_LABEL(ui->label_title, L_POWERLINE_DETAIL_TITLE, "Device List");
	POWERLINE_TRANSLATE_BUTTON(ui->but_close, L_POWERLINE_BUT_CLOSE, "Close");
}
void Powerline_DetailDlg::createGraphics()
{
	m_view	= new QGraphicsView(ui->frm_view);
	m_scene	= new QGraphicsScene(m_view);



	m_view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	m_view->setAttribute(Qt::WA_TranslucentBackground);
	m_view->setRenderHint(QPainter::Antialiasing);
	m_view->setCacheMode(QGraphicsView::CacheBackground);
	m_view->setStyleSheet(QString(D_TOPOLOGYVIEW_STYLE));
	//m_scene->setSceneRect(QRectF(0, 0, 600, 230));
	m_scene->setSceneRect(QRectF(0, 0, 540, 230));
	//m_scene->setBackgroundBrush(Qt::yellow);
	m_scene->setItemIndexMethod(QGraphicsScene::NoIndex);
	
	m_view->setScene(m_scene);
	
	m_view->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
	m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}
void Powerline_DetailDlg::initStyle()
{
	//Qt::WindowFlags flag= (this)?(Qt::Dialog| Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint):(Qt::FramelessWindowHint);

	//this->setWindowFlags(flag);
	//this->setAttribute(Qt::WA_TranslucentBackground);

	////enableMove(w);

	//QFile f(/*GENIE2_RES(*/":styles/framelessframe.qss")/*)*/;
	//bool btemp=f.open(QIODevice::ReadOnly);
	//Q_ASSERT(btemp);
	//QString stemp=f.readAll();

	//f.close();

	//this->setStyleSheet(stemp);

	/////////////////////////////////////////////////////////////////
	QFile f(/*GENIE2_RES(*/":styles/powerlineview.qss")/*)*/;
	bool btemp=f.open(QIODevice::ReadOnly);
	Q_ASSERT(btemp);
	QString stemp;
	stemp=f.readAll();
	f.close();


	//m_prevPageBtn->setStyleSheet(stemp);
	//m_nextPageBtn->setStyleSheet(stemp);

	ui->prevPageBtn->setStyleSheet(stemp);
	ui->nextPageBtn->setStyleSheet(stemp);
}
void Powerline_DetailDlg::initSlot()
{
	connect(ui->but_close, SIGNAL(clicked()),this, SLOT(slot_Btn_close()));
}
//====================================================
//  event function
//====================================================
void Powerline_DetailDlg::mouseMoveEvent(QMouseEvent *evt)
{
	if((evt->buttons() & Qt::LeftButton) && m_bMove && !isFullScreen())
	{
		move(evt->globalPos() - m_position);
		evt->accept();
	}
}
void Powerline_DetailDlg::mousePressEvent(QMouseEvent *evt)
{
	qDebug()<<"Powerline_DetailDlg::mousePressEvent(QMouseEvent *evt)";
	if(((evt->pos().y() >= 0) && (evt->pos().y() <= 40)) && (evt->button() == Qt::LeftButton) && !m_bMove)
	{
		m_bMove = true;
		m_position = evt->globalPos() - frameGeometry().topLeft();
	}

}
void Powerline_DetailDlg::mouseReleaseEvent(QMouseEvent *evt)
{
	if(evt->button() == Qt::LeftButton)
	{
		m_bMove = false;
	}

}
void Powerline_DetailDlg::changeEvent(QEvent *evt)
{
	if(evt->type() == QEvent::LanguageChange)
	{
		retranslateUi();
	}
	QDialog::changeEvent(evt);
}
//===============================================
//  slot funtion
//===============================================
void Powerline_DetailDlg::slot_Btn_close()
{
	//this->close();
	this->reject();
}

void Powerline_DetailDlg::pageBtnClick()
{
	QPushButton *btn = qobject_cast<QPushButton*>(sender());
	if(btn == /*m_prevPageBtn*/ui->prevPageBtn)
	{
		if(m_curPageIdx > 1)
		{
			--m_curPageIdx;
			SubScribeData();
		}
	}
	else if(btn == /*m_nextPageBtn*/ui->nextPageBtn)
	{
		if(m_curPageIdx < m_totalPage)
		{
			++m_curPageIdx;
			SubScribeData();
		}
	}

	updateUiElements();
}
//====================================================
//  public function
//====================================================
void Powerline_DetailDlg::initializeValue(const QList<AttachDeviceEntry> &lstNormalDev)
{
	m_pAttDeviceEntry = &lstNormalDev;
	m_flags |= DT_NORMAL_DEV;

	int nStart = 0, nEnd = 0;
	beforeUpdateUi(&nStart, &nEnd, lstNormalDev.count());
	onSubscribData(DT_NORMAL_DEV, nStart, nEnd);
}
void Powerline_DetailDlg::initializeValue(const QList<QPair<QString,QString> > &lstUsbDev)
{
	m_pAttDeviceUsb = &lstUsbDev;
	m_flags |= DT_USB_DEV;

	int nStart = 0, nEnd = 0;
	beforeUpdateUi(&nStart, &nEnd, lstUsbDev.count());
	onSubscribData(DT_USB_DEV, nStart, nEnd);

}

void Powerline_DetailDlg::initializeTopNode(PowerlineNode *node, PowerlineEdge *edge)
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

	m_TopEdge->m_bDetailDlgParent = true;

	m_TopNode->setVisible(true);

	m_scene->addItem(m_TopNode);
	m_scene->addItem(m_TopEdge);

	m_TopNode->updateShapeCache();

	ui->but_close->setFocus();
	
}

//====================================================
//  other function
//====================================================
void Powerline_DetailDlg::itemFromData(PowerlineNode *node, PowerlineEdge *edge, const AttachDeviceEntry &attDev)
{
	Q_UNUSED(attDev)

	/*node->m_deviceType	= GDT_WINDOWSPC;*/
	node->m_deviceType	= GDT_UNKNOWN;
	node->m_devName		= attDev.m_devName;
	node->m_devIp		= attDev.m_devIP;
	node->m_devMac		= attDev.m_devMac;
	node->m_bDetailDlgParent = true;

	edge->m_deviceType	= node->m_deviceType;
	edge->m_devName		= node->m_devName;
	edge->m_devIP		= node->m_devIp;
	edge->m_devMac		= node->m_devMac;
	edge->m_bDetailDlgParent = true;
}
void Powerline_DetailDlg::itemFromData(PowerlineNode *node, PowerlineEdge *edge, const QPair<QString,QString> &addUsb)
{
	Q_UNUSED(addUsb)

	//node->m_deviceType = GDT_WINDOWSPC;
	node->m_deviceType = GDT_UNKNOWN;
	node->m_bDetailDlgParent = true;
	node->m_devName = addUsb.second;

	edge->m_deviceType = GDT_UNKNOWN;
	//edge->m_deviceType = GDT_WINDOWSPC;
	edge->m_bDetailDlgParent = true;
	edge->m_devName = node->m_devName;
}
void Powerline_DetailDlg::delRedundanceNodes(int nCurIdx, int nCnt)
{
	for(int k = nCurIdx ; k < nCnt; ++k)
	{
		m_scene->removeItem(m_plNode[nCurIdx]);
		m_scene->removeItem(m_plEdge[nCurIdx]);

		delete m_plNode.takeAt(nCurIdx);
		delete m_plEdge.takeAt(nCurIdx);
	}
}
void Powerline_DetailDlg::createNewNodes(int nCurIdx)
{
	if(nCurIdx >= m_plNode.count())
	{
		m_plNode.append(new PowerlineNode());
		m_plEdge.append(new PowerlineEdge());

		m_scene->addItem(m_plNode[nCurIdx]);
		m_scene->addItem(m_plEdge[nCurIdx]);
	}

	m_plNode[nCurIdx]->setVisible(true);

	m_plNode[nCurIdx]->updateShapeCache();

}

void Powerline_DetailDlg::positionItem()
{
	Q_ASSERT(m_TopNode && m_TopEdge);

	//qreal fHeight = ui->frm_view->height();
	//qreal fWidth  = ui->frm_view->width();
	qreal fHeight = 230;
	//qreal fWidth  = 600;
	qreal fWidth = 540;
	const qreal PI = 3.14159265358979323846;

	////Top Node position
	qreal fCenterX = 0, fCenterY = 0;

	fCenterX = fWidth / 2;
	fCenterY = m_TopNode->boundingRect().height() / 2;

	m_TopNode->setPos(fCenterX, fCenterY);
	m_TopEdge->m_endPt		= QPointF(fCenterX, fCenterY);
	m_TopEdge->m_headPt		= QPointF(fCenterX, fCenterY + fHeight / 4);
	m_TopEdge->m_topPt		= QPointF(50, fCenterY + fHeight / 4);
	m_TopEdge->m_bottomPt	= QPointF(fWidth - 50, fCenterY + fHeight / 4);
	m_TopEdge->m_topTopPt   = m_TopEdge->m_topPt - QPointF(50, 0);
	m_TopEdge->m_bottomBottomPt = m_TopEdge->m_bottomPt + QPointF(50, 0);
	

	/////other node position
	int nCount = m_plNode.count();
	if(nCount > PER_PAGE_NUM)
	{
		qDebug() << "Powerline_DetailDlg::positionItem(): nCount > PER_PAGE_NUM, So failed!";

		return;
	}
	//qreal fGap = (fWidth - m_TopNode->boundingRect().width()) / 3;
	qreal tPosY = fHeight - m_TopNode->boundingRect().height();
	int nAngel = 0;

	for(int i = 0; i < nCount; ++i)
	{
		nAngel = cos(i * 90 * PI / 180);
		m_plNode[i]->setPos(i * fCenterX + 50 * nAngel, tPosY);

		m_plEdge[i]->m_headPt	= QPointF(i * fCenterX + 50 * nAngel, fCenterY + fHeight / 4);
		m_plEdge[i]->m_endPt	= QPointF(i * fCenterX + 50 * nAngel, tPosY);
		m_plEdge[i]->m_topPt	= m_plEdge[i]->m_endPt;
		m_plEdge[i]->m_bottomPt	= m_plEdge[i]->m_endPt;
	}
}
void Powerline_DetailDlg::updateUiElements()
{    
	bool bShow = false;

	if(m_totalPage >= 2 && m_curPageIdx >= 1)
	{
		bShow = true;
	}
	else
	{
		bShow = false;
	}
	//m_nextPageBtn->setVisible(bShow);
	//m_prevPageBtn->setVisible(bShow);
	ui->nextPageBtn->setVisible(bShow);
	ui->prevPageBtn->setVisible(bShow);

	ui->nextPageBtn->setEnabled(m_curPageIdx < m_totalPage);
	ui->prevPageBtn->setEnabled(m_curPageIdx > 1);

	//m_nextPageBtn->setEnabled(m_curPageIdx < m_totalPage);
	//m_prevPageBtn->setEnabled(m_curPageIdx > 1);

}
void Powerline_DetailDlg::onSubscribData(DATA_TYPE nType, int nStart, int nEnd)
{
	Q_ASSERT(m_plNode.count() == m_plEdge.count());
	

	m_scene->invalidate();
	/////建立新節點
	if(nStart > nEnd)
	{
		qDebug() << "Powerline_DetailDlg::onSubscribData:   nStart > nEnd ,so failed!";
		return;
	}
	int nCount = nEnd + 1 - nStart;
	int i = 0;
	int idx = 0;
	int tmpStart = nStart;

	if(nType == DT_USB_DEV)
	{
		Q_ASSERT(m_pAttDeviceUsb);


		for(i = 0; i < nCount; ++i)
		{

			createNewNodes(i);
			itemFromData(m_plNode[idx], m_plEdge[idx], (*m_pAttDeviceUsb)[i++]);
			++idx;
		}
	}
	else if(nType == DT_NORMAL_DEV)
	{
		Q_ASSERT(m_pAttDeviceEntry);


		for(i = 0; i < nCount; ++i)
		{
			createNewNodes(i);
			itemFromData(m_plNode[idx], m_plEdge[idx], (*m_pAttDeviceEntry)[tmpStart++]);
			++idx;
		}
	}
	



	////刪除多餘節點
	nCount = m_plNode.count();
	delRedundanceNodes(idx, nCount);


	////display nodes and edges
	positionItem();
}

void Powerline_DetailDlg::beforeUpdateUi(int *pStart, int *pEnd, int nCnt)
{
	int nTmp = 0;

	m_curPageIdx = 1;

	m_devTotalCount = nCnt;
	nTmp = m_devTotalCount / PER_PAGE_NUM;
	m_modNum = m_devTotalCount % (PER_PAGE_NUM);
	m_totalPage	= (m_modNum == 0) ? nTmp : (nTmp + 1);

	updateUiElements();

	dealPostionStartAndEnd(pStart, pEnd, m_modNum);
}
void Powerline_DetailDlg::dealPostionStartAndEnd(int *pStart, int *pEnd, int nModeNum)
{
	*pStart	= PER_PAGE_NUM * (m_curPageIdx - 1);
	*pEnd	= *pStart + PER_PAGE_NUM - 1;
	*pEnd = (m_curPageIdx == m_totalPage && nModeNum != 0) ? (*pStart + nModeNum - 1): (*pEnd);
}

void Powerline_DetailDlg::createControlPannel()
{
	//m_nextPageBtn = new QPushButton();
	//m_prevPageBtn = new QPushButton();
	//m_prevPageBtn->setObjectName("prevPageBtn");
	//m_nextPageBtn->setObjectName("nextPageBtn");
	//m_prevPageBtn->setAttribute(Qt::WA_TranslucentBackground);
	//m_nextPageBtn->setAttribute(Qt::WA_TranslucentBackground);
	//m_preBtnProxy = m_scene->addWidget(m_prevPageBtn);
	//m_nextBtnProxy = m_scene->addWidget(m_nextPageBtn);
	//m_prevPageBtn->setVisible(false);
	//m_nextPageBtn->setVisible(false);

	//qreal r2 = ui->frm_view->height() / 2 - 35;
	//m_preBtnProxy->setPos(10, r2);
	//m_nextBtnProxy->setPos(560, r2);



	//connect(m_nextPageBtn, SIGNAL(clicked()), this, SLOT(pageBtnClick()));
	//connect(m_prevPageBtn, SIGNAL(clicked()), this, SLOT(pageBtnClick()));

	ui->nextPageBtn->setAttribute(Qt::WA_TranslucentBackground);
	ui->prevPageBtn->setAttribute(Qt::WA_TranslucentBackground);
	ui->nextPageBtn->setVisible(false);
	ui->prevPageBtn->setVisible(false);

	connect(ui->nextPageBtn, SIGNAL(clicked()), this, SLOT(pageBtnClick()));
	connect(ui->prevPageBtn, SIGNAL(clicked()), this, SLOT(pageBtnClick()));
}

void Powerline_DetailDlg::SubScribeData()
{

	int nStart = 0, nEnd = 0;
	dealPostionStartAndEnd(&nStart, &nEnd, m_modNum);

	if(m_flags & DT_NORMAL_DEV)
	{
		onSubscribData(DT_NORMAL_DEV, nStart, nEnd);
	}
	else if(m_flags & DT_USB_DEV)
	{
		onSubscribData(DT_USB_DEV, nStart, nEnd);
	}
	else
	{

	}

}

void Powerline_DetailDlg::retranslateUi()
{

}