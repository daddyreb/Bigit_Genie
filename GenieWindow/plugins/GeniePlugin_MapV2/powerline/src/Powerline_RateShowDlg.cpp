#include "Powerline_RateShowDlg.h"
#include "ui_Powerline_RateShowDlg.h"
#include "tmpDefine.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QFile>
#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <math.h>


#define D_TOPOLOGYVIEW_STYLE	"border-image:url(:/PowerLineImage/bg_plugin_page.png) 14;"

Powerline_RateShowDlg::Powerline_RateShowDlg(QWidget *parent) :
    QDialog(parent)
    ,ui(new Ui::Powerline_RateShowDlg)
	,m_scene(NULL)
	,m_bMove(false)
	,m_TopEdge(NULL)
	,m_TopNode(NULL)
	,m_flags(0)
	,m_isPowerlineShow(false)
	,m_prevPageBtnState(BTNSTATE_NORMAL)
	,m_nextPageBtnState(BTNSTATE_NORMAL)

	,m_totalPage(0)
	,m_curPageIdx(1)
	/*,m_pPLDev(NULL)*/
{
    ui->setupUi(this);
	
	createGraphics();
	initSlot();
	createControlPannel();
	initStyle();
	initLanguage();

#ifdef TEST_PL_CODE
	ui->prevPageBtn->setFlat(false);
	ui->nextPageBtn->setFlat(false);
#endif
}

Powerline_RateShowDlg::~Powerline_RateShowDlg()
{
	//qDebug() << "~Powerline_RateShowDlg();";

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

	m_pPLDev.clear();

	delete ui;
}


//===============================================
//  initialization funtion
//===============================================
void Powerline_RateShowDlg::initLanguage()
{
	POWERLINE_TRANSLATE_LABEL(ui->label_title, L_POWERLINE_LINKRATE, "Device List");
	POWERLINE_TRANSLATE_BUTTON(ui->but_close, L_POWERLINE_BUT_CLOSE, "Close");
}
void Powerline_RateShowDlg::createGraphics()
{
	m_view	= new QGraphicsView(ui->frm_view);
	m_scene	= new QGraphicsScene(m_view);



	m_view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	m_view->setAttribute(Qt::WA_TranslucentBackground);
	m_view->setRenderHint(QPainter::Antialiasing);
	m_view->setCacheMode(QGraphicsView::CacheBackground);
	m_view->setStyleSheet(QString(D_TOPOLOGYVIEW_STYLE));
	//m_scene->setSceneRect(QRectF(0, 0, 600, 230));
	m_scene->setSceneRect(QRectF(-265, -230, 531, 461));
	//m_scene->setBackgroundBrush(Qt::blue);
	m_scene->setItemIndexMethod(QGraphicsScene::NoIndex);
	
	m_view->setScene(m_scene);
	
	m_view->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
	m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

#ifndef TEST_PL_CODE
	retranslateImages();
#endif

	//////////////////////////
}
void Powerline_RateShowDlg::initStyle()
{

	/////////////////////////////////////////////////////////////////
	QString strStyle = ("QPushButton{"
							"width:0px;height:0px;"
							"min-width:30px;"
							"border-width:0px;"
							"border-radius:0px;"
							"padding-left:0px;"
							"padding-right:0px;"
							"background-color:none;}"
							"QPushButton:hover,QPushButton:disabled, QPushButton:pressed{"
							"background-color:none;"
							"color:none;"
							"border-color:none;}"
							);

	ui->prevPageBtn->setStyleSheet(strStyle);
	ui->nextPageBtn->setStyleSheet(strStyle);

}
void Powerline_RateShowDlg::initSlot()
{
	connect(ui->but_close, SIGNAL(clicked()),this, SLOT(slot_Btn_close()));
}
//====================================================
//  event function
//====================================================
void Powerline_RateShowDlg::mouseMoveEvent(QMouseEvent *evt)
{
	if((evt->buttons() & Qt::LeftButton) && m_bMove && !isFullScreen())
	{
		move(evt->globalPos() - m_position);
		evt->accept();
	}
}
void Powerline_RateShowDlg::mousePressEvent(QMouseEvent *evt)
{
	qDebug()<<"Powerline_RateShowDlg::mousePressEvent(QMouseEvent *evt)";
	if(((evt->pos().y() >= 0) && (evt->pos().y() <= 40)) && (evt->button() == Qt::LeftButton) && !m_bMove)
	{
		m_bMove = true;
		m_position = evt->globalPos() - frameGeometry().topLeft();
	}

}
void Powerline_RateShowDlg::mouseReleaseEvent(QMouseEvent *evt)
{
	if(evt->button() == Qt::LeftButton)
	{
		m_bMove = false;
	}

}
bool Powerline_RateShowDlg::eventFilter(QObject *obj, QEvent *evt)
{
	if((obj == ui->prevPageBtn) || (obj == ui->nextPageBtn))
	{
		QPushButton *btn = qobject_cast<QPushButton *>(obj);

		if(evt->type() == QEvent::EnabledChange)
		{
			int *pbtnState = NULL;

			if(btn == ui->prevPageBtn)
			{
				pbtnState	= &m_prevPageBtnState;
			}
			else if(btn == ui->nextPageBtn)
			{
				pbtnState	= &m_nextPageBtnState;
			}

			if(btn->isEnabled())
			{
				*pbtnState	&= ~BTNSTATE_DISABLED;
			}
			else
			{
				*pbtnState	|= BTNSTATE_DISABLED;
			}
		}
		else if(evt->type() == QEvent::Enter)
		{
			if(btn == ui->prevPageBtn)
			{
				m_prevPageBtnState |= BTNSTATE_HOVER;
			}
			else
			{
				m_nextPageBtnState |= BTNSTATE_HOVER;
			}
		}
		else if(evt->type() == QEvent::Leave)
		{
			if(btn == ui->prevPageBtn)
			{
				m_prevPageBtnState &= ~BTNSTATE_HOVER ;
			}
			else
			{
				m_nextPageBtnState &= ~BTNSTATE_HOVER;
			}

		}
		else
		{
			return false;
		}
#ifndef TEST_PL_CODE
		retranslateImages();
#endif

		return true;
	}

	return QWidget::eventFilter(obj, evt);
}
//===============================================
//  slot funtion
//===============================================
void Powerline_RateShowDlg::slot_Btn_close()
{
	//this->close();
	this->reject();
}

void Powerline_RateShowDlg::pageBtnClick()
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
//void Powerline_RateShowDlg::initializeValue(const QList<AttachDeviceEntry> &lstNormalDev)
//{
//	m_pAttDeviceEntry = &lstNormalDev;
//	m_flags |= DT_NORMAL_DEV;
//
//	int nStart = 0, nEnd = 0;
//	beforeUpdateUi(&nStart, &nEnd, lstNormalDev.count());
//	onSubscribData(DT_NORMAL_DEV, nStart, nEnd);
//}
//void Powerline_RateShowDlg::initializeValue(const QList<QPair<QString,QString> > &lstUsbDev)
//{
//	m_pAttDeviceUsb = &lstUsbDev;
//	m_flags |= DT_USB_DEV;
//
//	int nStart = 0, nEnd = 0;
//	beforeUpdateUi(&nStart, &nEnd, lstUsbDev.count());
//	onSubscribData(DT_USB_DEV, nStart, nEnd);
//
//}
void Powerline_RateShowDlg::initializeValue(const QList<PLDevice *> &lstDev)
{
	//m_pPLDev = &lstDev;

	Q_ASSERT(m_TopNode);

	for(int i = 0; i < lstDev.count(); ++i)
	{
		if((lstDev[i])->m_nID == m_TopNode->m_nID)
		{
			continue;
		}


		m_pPLDev.append(lstDev[i]);
	}
	
	if(lstDev.count() - 1 == 0)
	{
		return;
	}

	int nStart = 0, nEnd = 0;
	beforeUpdateUi(&nStart, &nEnd, lstDev.count() - 1);  // 减一是因为里面包含了一个m_TopNode节点
	onSubscribData(nStart, nEnd);
}
void Powerline_RateShowDlg::initializeTopNode(PowerlineNode *node, PowerlineEdge *edge)
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

	m_TopNode->m_devName = QString("");

	qDebug() << "m_TopNode m_nID: " << m_TopNode->m_nID;

	m_TopEdge->m_bDetailDlgParent = true;
	m_TopNode->m_bDetailDlgParent = true;

	m_TopNode->setVisible(true);

	m_scene->addItem(m_TopNode);
	m_scene->addItem(m_TopEdge);

	QString strName		= POWERLINE_GET_TEXT(L_POWERLINE_TOOLTIP_NAME, "Name");
	QString strModel	= POWERLINE_GET_TEXT(L_POWERLINE_TOOLTIP_MODEL, "Model");
	QString strMac		= POWERLINE_GET_TEXT(L_POWERLINE_TOOLTIP_MAC, "MAC Address");
	strName.replace(":", "");
	strModel.replace(":", "");
	strMac.replace(":", "");

	QString strToolTip = QString("%1: %2\n%3: %4\n%5: %6").arg(\
		strName, DEAL_STR(node->m_devName)).arg(strModel, DEAL_STR(node->m_devModelName)).arg(strMac, node->m_devMac);
	m_TopNode->setToolTip(strToolTip);

	m_TopNode->updateShapeCache();


	ui->but_close->setFocus();
	
}

//====================================================
//  other function
//====================================================
QPair<qint16, qint16> Powerline_RateShowDlg::getRate(const QString &strCurMac, QMap<QString, QPair<qint16,qint16> >&mapLst)
{
	return mapLst.value(strCurMac);
}
void Powerline_RateShowDlg::itemsFromDevice(PowerlineNode *node, PowerlineEdge *edge, PLDevice *dev)
{
	node->m_deviceType		 = GDT_POWERLINE;
	node->m_bDetailDlgParent = true;
	node->m_isPowerLine		 = true;
	node->m_devLedState      = (*dev)[GDP_LED_STATUS].toBool();
	node->m_devModelName     = (*dev)[GDP_MODEL_NAME].toString();
	node->m_bPicUseDefault	 = false;
	node->m_nID				 = dev->m_nID;
	node->m_devMac			 = (*dev)[GDP_PHYSICALADDR].toString();
	node->m_devName			 = (*dev)[GDP_NAME].toString();
	node->updateShapeCache();

	QString strName		= POWERLINE_GET_TEXT(L_POWERLINE_TOOLTIP_NAME, "Name");
	QString strModel	= POWERLINE_GET_TEXT(L_POWERLINE_TOOLTIP_MODEL, "Model");
	QString strMac		= POWERLINE_GET_TEXT(L_POWERLINE_TOOLTIP_MAC, "MAC Address");
	strName.replace(":", "");
	strModel.replace(":", "");
	strMac.replace(":", "");

	QString strToolTip = QString("%1: %2\n%3: %4\n%5: %6").arg(\
		strName, DEAL_STR(node->m_devName)).arg(strModel, DEAL_STR(node->m_devModelName)).arg(strMac, node->m_devMac);
	node->setToolTip(strToolTip);
	node->updateShapeCache();

	edge->m_deviceType		 = GDT_POWERLINE;
	edge->m_bDetailDlgParent = true;
	//edge->m_linkRate_send    = dev->m_linkRate_send;
	//edge->m_linkRate_recv	 = dev->m_linkRate_recv;

	qDebug() << "dev:" << dev->m_linkRate;
	if(!node->m_devMac.isEmpty() && m_TopNode)
	{
		QPair<qint16, qint16> tmpRate = getRate(m_TopNode->m_devMac, dev->m_linkRate);
		QString strSend;
		QString strRecv;

		strSend = QString("%1").arg(tmpRate.second);
		strRecv = QString("%1").arg(tmpRate.first);
		edge->m_linkRate_send	= (strSend == "0") ? "10" : strSend;
		edge->m_linkRate_recv	= (strRecv == "0") ? "10" : strRecv;

		qDebug() << "Powerline_RateShowDlg:" << "Send:" << tmpRate.first;
	}
	edge->m_deviceType       = node->m_deviceType;
	edge->m_devLedState		 = node->m_devLedState;
}
void Powerline_RateShowDlg::delRedundanceNodes(int nCurIdx, int nCnt)
{
	for(int k = nCurIdx ; k < nCnt; ++k)
	{
		m_scene->removeItem(m_plNode[nCurIdx]);
		m_scene->removeItem(m_plEdge[nCurIdx]);

		delete m_plNode.takeAt(nCurIdx);
		delete m_plEdge.takeAt(nCurIdx);
	}
}
void Powerline_RateShowDlg::createNewNodes(int nCurIdx)
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

void Powerline_RateShowDlg::positionItem()
{
	Q_ASSERT(m_TopNode && m_TopEdge);

	int nCount = m_plNode.count();
	if((nCount + 1) < 2)
	{
		qDebug() << "Node list too small, size = " << nCount;
		return;
	}

	//qreal Angle = qreal(360) / nCount;
	qreal Angle = 0;
	int nNum = m_pPLDev.count();
	if(nCount == 1 || nCount == 2)
	{
		Angle = 180;
	}
	else
	{
		Angle = qreal(360) / 4;
	}
	

	qreal r1	= qreal(ui->frm_view->width()) / 3;
	qreal r2	= qreal(ui->frm_view->height()) / 3;
	//r2 = r1;

	qDebug() << "Width: " << ui->frm_view->width();

	const qreal PI = 3.14159265358979323846;

	////Top Node position
	qreal fCenterX = 0, fCenterY = 0;



	//m_TopNode->setPos(fCenterX, fCenterY);



	if(nNum == 1)
	{
		//Two Node
		fCenterX = r1 * cos(Angle * PI / 180)/* - (m_TopNode->boundingRect().width() / 2)*/;
		fCenterY = r2 * sin(0.0)/* - (m_TopNode->boundingRect().height() / 2)*/;

	}
	else
	{
		//fCenterX = (2 * r1 - m_TopNode->boundingRect().width()) / 2 - r1;
		//fCenterY = (2 * r2 - m_TopNode->boundingRect().height()) / 2 - r2;
		fCenterX = 0;
		fCenterY = 0;
	}

	m_TopNode->setPos(fCenterX, fCenterY);
	m_TopEdge->m_headPt	= m_TopEdge->m_endPt = QPointF(fCenterX, fCenterY);

	/////other node position

	if(nCount > PER_PAGE_NUM)
	{
		qDebug() << "Powerline_RateShowDlg::positionItem(): nCount > PER_PAGE_NUM, So failed!";

		return;
	}
	//qreal fGap = (fWidth - m_TopNode->boundingRect().width()) / 3;
	qreal tmpAngle = 0;
	qreal tmp = 0;


	qDebug() << "Angle" << Angle;
	for(int i = 0; i < nCount; ++i)
	{
		if(i == 1 && nCount == 3)
		{
			
			m_plEdge[i]->m_rotate	=  3 * Angle;
			tmp = 3 * Angle;
			m_plNode[i]->m_isTextShowUp = true;
		}
		else
		{
			m_plEdge[i]->m_rotate	= i * Angle;
			tmp = Angle;
		}
		tmpAngle = i * tmp * PI / 180;
	

		m_plEdge[i]->m_Angle	= tmpAngle;	//角度记录
		

		qreal x = r1 * cos(tmpAngle);
		qreal y = r2 * sin(tmpAngle);

		m_plNode[i]->setPos(x, y);

		m_plEdge[i]->m_headPt	= QPointF(fCenterX, fCenterY);
		m_plEdge[i]->m_endPt	= QPointF(x, y);

		m_plEdge[i]->m_S_nodeWidth	= m_plNode[i]->shape().boundingRect().width();
		m_plEdge[i]->m_S_nodeHeight	= m_plNode[i]->shape().boundingRect().height();
		m_plEdge[i]->m_P_nodeWidth	= m_TopNode->shape().boundingRect().width();
		m_plEdge[i]->m_P_nodeHeight = m_TopNode->shape().boundingRect().height();
	}

	
}
void Powerline_RateShowDlg::updateUiElements()
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
	ui->pagesLabel->setVisible(bShow);

	ui->nextPageBtn->setEnabled(m_curPageIdx < m_totalPage);
	ui->prevPageBtn->setEnabled(m_curPageIdx > 1);

	if(m_totalPage >= 2 && m_curPageIdx >= 1)
	{
		QString strTmp("%1/%2");
		ui->pagesLabel->setText(strTmp.arg(QString::number(m_curPageIdx),QString::number(m_totalPage)));
	}
	//m_nextPageBtn->setEnabled(m_curPageIdx < m_totalPage);
	//m_prevPageBtn->setEnabled(m_curPageIdx > 1);

}
void Powerline_RateShowDlg::onSubscribData(int nStart, int nEnd)
{
	Q_ASSERT(m_plNode.count() == m_plEdge.count());
	

	m_scene->invalidate();
	/////建立新節點
	if(nStart > nEnd)
	{
		qDebug() << "Powerline_RateShowDlg::onSubscribData:   nStart > nEnd ,so failed!";
		return;
	}
	int nCount = nEnd + 1 - nStart;
	int i = 0;
	int idx = 0;
	int tmpStart = nStart;
	int j = 0;


		Q_ASSERT(/*m_pPLDev && */m_TopNode);


		for(i = 0; i < nCount/* + 1*/; ++i)
		{
			qDebug() << "idx=" << i << "m_nID: " << ((m_pPLDev)[i])->m_nID;
			//if(((*m_pPLDev)[i])->m_nID == m_TopNode->m_nID)
			//{
			//	tmpStart++;
			//	continue;
			//}

			createNewNodes(j++);
			itemsFromDevice(m_plNode[idx], m_plEdge[idx], (m_pPLDev)[tmpStart++]);
			++idx;
		}




	////刪除多餘節點
	nCount = m_plNode.count();
	delRedundanceNodes(idx, nCount);


	////display nodes and edges
	positionItem();
}

void Powerline_RateShowDlg::beforeUpdateUi(int *pStart, int *pEnd, int nCnt)
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
void Powerline_RateShowDlg::dealPostionStartAndEnd(int *pStart, int *pEnd, int nModeNum)
{
	*pStart	= PER_PAGE_NUM * (m_curPageIdx - 1);
	*pEnd	= *pStart + PER_PAGE_NUM - 1;
	*pEnd = (m_curPageIdx == m_totalPage && nModeNum != 0) ? (*pStart + nModeNum - 1): (*pEnd);
}

void Powerline_RateShowDlg::createControlPannel()
{
#ifndef TEST_PL_CODE
	ui->nextPageBtn->setAttribute(Qt::WA_TranslucentBackground);
	ui->prevPageBtn->setAttribute(Qt::WA_TranslucentBackground);
#endif
	ui->nextPageBtn->setVisible(false);
	ui->prevPageBtn->setVisible(false);

	connect(ui->nextPageBtn, SIGNAL(clicked()), this, SLOT(pageBtnClick()));
	connect(ui->prevPageBtn, SIGNAL(clicked()), this, SLOT(pageBtnClick()));

	ui->prevPageBtn->installEventFilter(this);
	ui->nextPageBtn->installEventFilter(this);
}

void Powerline_RateShowDlg::retranslateImages()
{
	QString prevBtnImage = POWERLINE_RES((qApp->layoutDirection() == Qt::LeftToRight) ? "map/others/Prev.png" : "map/others/Next.png");
	QString nextBtnImage = POWERLINE_RES((qApp->layoutDirection() == Qt::LeftToRight) ? "map/others/Next.png" : "map/others/Prev.png");

	QString prevBtnDisableImage = POWERLINE_RES((qApp->layoutDirection() == Qt::LeftToRight) ? "map/others/PrevDisabled.png" : "map/others/NextDisabled.png");
	QString nextBtnDisableImage = POWERLINE_RES((qApp->layoutDirection() == Qt::LeftToRight) ? "map/others/NextDisabled.png" : "map/others/PrevDisabled.png");

	QString prevBtnSelectedImage = POWERLINE_RES((qApp->layoutDirection() == Qt::LeftToRight) ? "map/others/PrevSelected.png" : "map/others/NextSelected.png");
	QString nextBtnSelectedImage = POWERLINE_RES((qApp->layoutDirection() == Qt::LeftToRight) ? "map/others/NextSelected.png" : "map/others/PrevSelected.png");


	QPixmap prevPixmap(prevBtnImage);
	ui->prevPageBtn->setFixedSize(prevPixmap.size());
	QPixmap nextPixmap(nextBtnImage);
	ui->nextPageBtn->setFixedSize(nextPixmap.size());


	if (m_prevPageBtnState & BTNSTATE_DISABLED)
	{
		ui->prevPageBtn->setIcon(QIcon(prevBtnDisableImage));
	}
	else if(m_prevPageBtnState & BTNSTATE_HOVER)
	{
		ui->prevPageBtn->setIcon(QIcon(prevBtnSelectedImage));
	}
	else
	{
		ui->prevPageBtn->setIcon(QIcon(prevBtnImage));
	}

	if (m_nextPageBtnState & BTNSTATE_DISABLED)
	{
		ui->nextPageBtn->setIcon(QIcon(nextBtnDisableImage));
	}
	else if(m_nextPageBtnState & BTNSTATE_HOVER)
	{
		ui->nextPageBtn->setIcon(QIcon(nextBtnSelectedImage));
	}
	else
	{
		ui->nextPageBtn->setIcon(QIcon(nextBtnImage));
	}
}

void Powerline_RateShowDlg::SubScribeData()
{

	int nStart = 0, nEnd = 0;
	dealPostionStartAndEnd(&nStart, &nEnd, m_modNum);


	onSubscribData(nStart, nEnd);


}
