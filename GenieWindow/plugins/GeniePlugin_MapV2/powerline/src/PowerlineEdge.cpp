#include "PowerlineEdge.h"
#include "PowerlineNode.h"
#include "Powerline_mapuitilities.h"
#include "PowerlineDefine.h"
#include <QPainterPath>
#include <QPainter>
#include <QPen>
#include <QPixmap>


PowerlineEdge::PowerlineEdge()
    :m_isLocalHost(false)
    ,m_wireless(false)
    ,m_onLine(false)
    ,m_deviceType(GDT_UNKNOWN)
    ,m_signalLevel(0)
	,m_Angle(0.0)
	,m_bArrow(false)
	,m_bLocalDevice(false)
	,m_S_nodeHeight(0)
	,m_S_nodeWidth(0)
	,m_P_nodeWidth(0)
	,m_P_nodeHeight(0)
	,m_bDetailDlgParent(false)
	,m_isUsb(false)
	,m_rotate(0)
	,m_bHubView(false)

{
    this->setZValue(-100.0);
}


QRectF PowerlineEdge::calSignalPixmapRect() const
{
    //if(m_wireless)
    //{
    //    QPixmap signalPixmap(getWirelessSignalImagePathStr(m_onLine ? m_signalLevel : 0));
    //    QPointF tmpPt = boundingRect().center();

    //    tmpPt.setX(tmpPt.x() - signalPixmap.width() / 2);
    //    tmpPt.setY(tmpPt.y() - signalPixmap.height() / 2);

    //    return QRectF(tmpPt, QSizeF(qreal(signalPixmap.width()), qreal(signalPixmap.height())));
    //}
	if(m_isUsb)
	{
		QPixmap pixmap(getPLOtherDevImagePath(m_deviceType));
		QPointF tmpPt = boundingRect().center();

		tmpPt.setX(tmpPt.x() - pixmap.width() / 2);
		tmpPt.setY(tmpPt.y() - pixmap.height() / 2);

		return QRectF(tmpPt, QSizeF(qreal(pixmap.width()), qreal(pixmap.height())));
	}
    else
    {
        return QRectF();
    }
}
QRectF PowerlineEdge::boundingRect() const
{
    return QRectF(m_headPt, m_endPt).normalized();
}

QPainterPath PowerlineEdge::shape() const
{
    QPainterPath shapePath;

    shapePath.moveTo(m_headPt);
    shapePath.lineTo(m_endPt);
    shapePath.addRect(calSignalPixmapRect());

    return shapePath;
}

void PowerlineEdge::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)


    bool bPowerline = false;
    if(m_deviceType == GDT_POWERLINE)
    {
        bPowerline = true;
    }
    Qt::PenStyle penStyle = (m_wireless || (bPowerline && !m_bDetailDlgParent)) ? Qt::DotLine : Qt::SolidLine;
    //QColor penColor = (m_isLocalHost /*|| (m_deviceType == GDT_INTERNET)*/) ?
    //            QColor(83,166,73) : QColor(13, 0, 255);

	bool b = (bPowerline || m_deviceType == GDT_INTERNET || m_deviceType == GDT_ROUTER);
	QColor penColor = (b) ? QColor(13, 0, 255) : QColor(0, 0, 0);
	
	if(m_deviceType == GDT_INTERNET && !m_onLine)
	{
		penColor	= QColor(255, 0, 0);
		penStyle	= Qt::DashDotDotLine;
		qDebug() << "m_Internet Edge X, y" << m_endPt.x() << "," << m_endPt.y();
	}
	
	if(!b)
	{

		painter->setPen(QPen(penColor,1.0,penStyle,Qt::RoundCap,Qt::RoundJoin));
	}
	else
	{
		painter->setPen(QPen(penColor,2.0,penStyle,Qt::RoundCap,Qt::RoundJoin));
	}
    
    painter->drawLine(m_headPt, m_endPt);


	if(m_bDetailDlgParent && bPowerline && m_headPt != m_endPt)
	{
		//painter->drawLine(m_topPt, m_bottomPt);
		//painter->drawLine(m_topTopPt, m_topPt);
		//painter->drawLine(m_bottomPt, m_bottomBottomPt);

		qint16 nRate = m_linkRate_send.toInt();
		if(nRate < 50)
		{
			//red
			penColor = Qt::red;
		}
		else if(nRate < 80)
		{
			//yellow
			penColor = Qt::yellow;
		}
		else
		{
			penColor = Qt::green;
		}
		      
		painter->setPen(QPen(penColor, 2.0, penStyle, Qt::RoundCap, Qt::RoundJoin));
		drawArrowLine2(painter, penColor);


		nRate = m_linkRate_recv.toInt();
		if(nRate < 50)
		{
			//red
			penColor = Qt::red;
		}
		else if(nRate < 80)
		{
			//yellow
			penColor = Qt::yellow;
		}
		else
		{
			penColor = Qt::green;
		}
		painter->setPen(QPen(penColor, 2.0, penStyle, Qt::RoundCap, Qt::RoundJoin));

		drawArrowLine2(painter, penColor, DA_RIGHT);
	}
	else if(!m_bDetailDlgParent && bPowerline && m_parent.count() == 1 && !m_bHubView)
	{
		////LocalDevice Node
		painter->drawLine(m_topPt, m_bottomPt);
		qDebug() << "here : " << m_topPt << " " << m_bottomPt;
		qDebug() << "GDT_TYPE:" << m_deviceType;
	}

  //  if(bPowerline && !m_bLocalDevice && m_bArrow)
  //  {
		//qint16 nRate = m_linkRate_send.toInt();
		//if(nRate < 50)
		//{
		//	//red
		//	penColor = Qt::red;
		//}
		//else if(nRate < 80)
		//{
		//	//yellow
		//	penColor = Qt::yellow;
		//}
		//else
		//{
		//	penColor = Qt::green;
		//}
  //      
  //      painter->setPen(QPen(penColor, 2.0, penStyle, Qt::RoundCap, Qt::RoundJoin));


  //      drawArrowLine(painter, penColor);


		//nRate = m_linkRate_recv.toInt();
		//if(nRate < 50)
		//{
		//	//red
		//	penColor = Qt::red;
		//}
		//else if(nRate < 80)
		//{
		//	//yellow
		//	penColor = Qt::yellow;
		//}
		//else
		//{
		//	penColor = Qt::green;
		//}
		//painter->setPen(QPen(penColor, 2.0, penStyle, Qt::RoundCap, Qt::RoundJoin));

  //      drawArrowLine(painter, penColor, DA_RIGHT);

  //  }

	if(m_isUsb)
	{
        QPixmap pixmap(getPLOtherDevImagePath(GDT_USB));
		QPointF tmpPt = boundingRect().center();

		tmpPt.setX(tmpPt.x() - pixmap.width() / 2);
		tmpPt.setY(tmpPt.y() - pixmap.height() / 2);

		painter->drawPixmap(tmpPt, pixmap);
	}
  
    //if(m_wireless)
    //{
    //    QPixmap signalPixmap(getWirelessSignalImagePathStr(m_signalLevel));
    //    QPointF tmpPt = boundingRect().center();

    //    tmpPt.setX(tmpPt.x() - signalPixmap.width() / 2);
    //    tmpPt.setY(tmpPt.y() - signalPixmap.height() / 2);

    //    painter->drawPixmap(tmpPt,signalPixmap);
    //}
}
void PowerlineEdge::drawArrowLine2(QPainter *painter, QColor &pColor, DIRECT_ARROW nDirection)
{
    Q_ASSERT(painter);
    QPointF ptUpStart,
            ptDownStart,
			ptOffsetEnd,
			ptOffsetEnd2;
	QColor  txtColor;
	QPointF tmpHPt, tmpEPt, tmpHPt1, tmpEPt1;
	

	txtColor = Qt::black;


    painter->setPen(QPen(pColor, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	const qreal PI		= 3.14159265358979323846;
	const int	radius  = 10;

	/*      m_headPt       m_endPt
				<-----------------	*/
	int nTmpPosX = 0, nTmpPosY = 0;
//	int ntmp_s_nodeWidth = 0;
	qreal tmpRota = m_rotate;
	int nR = 1;		//四象限

	

	int nX = 0, nY = 0;
	int nX1 = 0, nY1 = 0;
	nX	= m_P_nodeWidth / 2;
	nX1 = m_S_nodeWidth / 2;
	nX	= (nX > nX1) ? nX : nX1;
	nX1 = nX;
	nY	= m_P_nodeHeight / 2;
	nY1	= m_S_nodeHeight / 2;
	nY	= (nY > nY1) ? nY : nY1;
	nY1	= nY;

	QString strRate;

	if(nDirection == DA_RIGHT)
	{
		QPointF tmpPt;
		strRate = QString("Rx:%1Mbps").arg(m_linkRate_recv);
		int nWidth	= getStringWidth(strRate);



		if(m_rotate == 90 || m_rotate == 270)
		{
			tmpHPt = QPointF(m_headPt.x() + radius, m_headPt.y());
			tmpEPt = QPointF(m_endPt.x() + radius, m_endPt.y());

			///////start
			if(m_rotate == 90)
			{
				tmpHPt += QPointF(0, +(nY + 13));
				tmpEPt += QPointF(0, -(nY1 + 13));
			}
			else
			{
				tmpHPt += QPointF(0, -(nY + 13));
				tmpEPt += QPointF(0, +(nY1 + 13));
			}
			///////end
		}
		else
		{
			tmpHPt = QPointF(m_headPt.x()/* + sin(m_Angle) * radius*/, m_headPt.y() + /*cos(m_Angle) **/ radius);
			tmpEPt = QPointF(m_endPt.x()/* + sin(m_Angle) * radius*/, m_endPt.y() + /*cos(m_Angle) **/ radius);

			///////start
			if(m_rotate == 0)
			{
				tmpHPt += QPointF(+(nX + 10), 0);
				tmpEPt += QPointF(-(nX1 + 10), 0);
			}
			else
			{
				tmpHPt += QPointF(-(nX + 10), 0);
				tmpEPt += QPointF(+(nX1 + 10), 0);
			}
			///////end
		}

qDebug() << " " << m_rotate;
		if(m_rotate == 0 || m_rotate == 360)
		{	
			ptOffsetEnd2 = tmpHPt/* + QPointF(nX , 0)*/;
			ptOffsetEnd = tmpEPt /*- QPointF(nX1, 0)*/;

			if(ptOffsetEnd.x() > ptOffsetEnd2.x())
			{
				nTmpPosX	= (ptOffsetEnd.x() + ptOffsetEnd2.x()) / 2 - nWidth / 2;
			}
			else
			{
				nTmpPosX	= (ptOffsetEnd2.x() - ptOffsetEnd.x()) / 2 - nWidth / 2;
			}
			
			nTmpPosY	= ptOffsetEnd.y() + 12 + 10;
		}
		else if(m_rotate == 90)
		{
			ptOffsetEnd = tmpEPt/* - QPointF(0, nY1)*/;
			ptOffsetEnd2 = tmpHPt /*+ QPointF(0, nY)*/;


			nTmpPosX	= ptOffsetEnd.x() + 12;
			nTmpPosY	= ptOffsetEnd2.y() + ((ptOffsetEnd.y() - ptOffsetEnd2.y()) / 2 - nWidth / 2);
		}
		else if(m_rotate == 180)
		{
			ptOffsetEnd = tmpEPt/* + QPointF(nX1, 0)*/;
			ptOffsetEnd2 = tmpHPt /*- QPointF(nX, 0)*/;

			tmpRota = 0;
			nR = 2;

			nTmpPosX	= ptOffsetEnd.x() + ((ptOffsetEnd2.x() - ptOffsetEnd.x()) / 2 - nWidth / 2);
			nTmpPosY	= ptOffsetEnd.y() + 12 + 10;
		}
		else if(m_rotate == 270)
		{
			ptOffsetEnd = tmpEPt /*+ QPointF(0, nY1)*/;
			ptOffsetEnd2 = tmpHPt/* - QPointF(0, nY)*/;

			tmpRota = 90;
			nR = 3;

			nTmpPosX	= ptOffsetEnd.x() + 12;
			nTmpPosY	= ptOffsetEnd.y() + ((ptOffsetEnd2.y() - ptOffsetEnd.y()) / 2 - nWidth / 2);
		}

		/////////线段上的文字
		//painter->save();

		//painter->translate(nTmpPosX, nTmpPosY);
		//painter->rotate(tmpRota);
		//painter->translate(-nTmpPosX, -nTmpPosY);

		//painter->setPen(QPen(txtColor, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
		//painter->drawText(nTmpPosX, nTmpPosY, strRate);
		//painter->restore();		
		//////End
	}
	else if(nDirection == DA_LEFT)
	{

		strRate = QString("Tx:%1Mbps").arg(m_linkRate_send);
		int nWidth	= getStringWidth(strRate);

		//int nX = 0, nY = 0;
		//int nX1 = 0, nY1 = 0;
		//nX	= m_P_nodeWidth / 2;
		//nX1 = m_S_nodeWidth / 2;
		//nY	= m_P_nodeHeight / 2;
		//nY1	= m_S_nodeHeight / 2;

		qDebug() << " " << m_rotate;

		if(m_rotate == 90 || m_rotate == 270)
		{
			tmpHPt = QPointF(m_endPt.x() - radius, m_endPt.y());
			tmpEPt = QPointF(m_headPt.x() - radius, m_headPt.y());

			///////start
			if(m_rotate == 90)
			{
				tmpHPt += QPointF(0, -(nY + 13));
				tmpEPt += QPointF(0, +(nY1 + 13));
			}
			else
			{
				tmpHPt += QPointF(0, +(nY + 13));
				tmpEPt += QPointF(0, -(nY1 + 13));
			}
			///////end
		}
		else
		{
			tmpHPt = QPointF(m_endPt.x() /*+ sin(m_Angle) * radius*/, m_endPt.y() - /*cos(m_Angle) **/ radius);
			tmpEPt = QPointF(m_headPt.x(), m_headPt.y() - /*cos(m_Angle) * */radius);

			///////start
			if(m_rotate == 0)
			{
				tmpHPt += QPointF(-(nX + 10), 0);
				tmpEPt += QPointF(+(nX1 + 10), 0);
			}
			else
			{
				tmpHPt += QPointF(+(nX + 10), 0);
				tmpEPt += QPointF(-(nX1 + 10), 0);
			}
			///////end
		}



		if(m_rotate == 0 || m_rotate == 360)
		{	
			ptOffsetEnd2 = tmpHPt/* - QPointF(nX1 , 0)*/;
			ptOffsetEnd = tmpEPt/* + QPointF(nX, 0)*/;

			if(ptOffsetEnd.x() > ptOffsetEnd2.x())
			{

				nTmpPosX	= (ptOffsetEnd.x() - ptOffsetEnd2.x()) / 2 - nWidth / 2;
			}
			else
			{
				nTmpPosX	= (ptOffsetEnd2.x() + ptOffsetEnd.x()) / 2 - nWidth / 2;
			}
			
			nTmpPosY	= ptOffsetEnd.y() - 12;
		}
		else if(m_rotate == 90)
		{
			ptOffsetEnd = tmpEPt/* + QPointF(0, nY)*/;
			ptOffsetEnd2 = tmpHPt /*- QPointF(0, nY1)*/;


			nTmpPosX	= ptOffsetEnd.x() - 12 - 10;
			nTmpPosY	= ptOffsetEnd2.y() + ((ptOffsetEnd.y() - ptOffsetEnd2.y()) / 2 - nWidth / 2);
		}
		else if(m_rotate == 180)
		{
			ptOffsetEnd = tmpEPt/* - QPointF(nX, 0)*/;
			ptOffsetEnd2 = tmpHPt/* + QPointF(nX1, 0)*/;

			tmpRota = 0;
			nR = 2;

			nTmpPosX	= ptOffsetEnd.x() + ((ptOffsetEnd2.x() - ptOffsetEnd.x()) / 2 - nWidth / 2);
			nTmpPosY	= ptOffsetEnd.y() - 12;
		}
		else if(m_rotate == 270)
		{
			ptOffsetEnd = tmpEPt/* - QPointF(0, nY)*/;
			ptOffsetEnd2 = tmpHPt/* + QPointF(0, nY1)*/;

			tmpRota = 90;
			nR = 3;

			nTmpPosX	= ptOffsetEnd.x() - 12 - 10;
			nTmpPosY	= ptOffsetEnd.y() + ((ptOffsetEnd2.y() - ptOffsetEnd.y()) / 2 - nWidth / 2);
		}

		///////线段上的文字



		//painter->save();


		//painter->translate(nTmpPosX, nTmpPosY);
		//painter->rotate(tmpRota);
		//painter->translate(-nTmpPosX, -nTmpPosY);

		////if(nR == 3)
		////{
		////	painter->translate(nTmpPosX, nTmpPosY);
		////	painter->rotate(tmpRota);
		////	painter->translate(-nTmpPosX, -nTmpPosY);
		////}
		////else
		////{
		////	painter->rotate(tmpRota);
		////}
		//painter->setPen(QPen(txtColor, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
		//painter->drawText(nTmpPosX, nTmpPosY, strRate);
		//painter->restore();


	}



	qreal x0, y0, x1, y1, x2, y2;
	x0 = ptOffsetEnd2.x() - ptOffsetEnd.x();
	y0 = ptOffsetEnd2.y() - ptOffsetEnd.y();
	qreal theta = 30 * PI / 180;
	qreal len = 10;
	x1 = x0 * cos(theta) - y0 * sin(theta);
	y1 = x0 * sin(theta) + y0 * cos(theta);
	x2 = x0 * cos(theta) + y0 * sin(theta);
	y2 = x0 * sin(-theta) + y0 * cos(theta);
	qreal l1, l2;
	l1 = sqrt(x1 * x1 + y1 * y1);
	l2 = sqrt(x2 * x2 + y2 * y2);
	x1 = x1 * len / l1;
	x2 = x2 * len / l2;
	y1 = y1 * len / l1;
	y2 = y2 * len / l2;
	x1 += ptOffsetEnd.x();
	y1 += ptOffsetEnd.y();
	x2 += ptOffsetEnd.x();
	y2 += ptOffsetEnd.y();

	painter->drawLine(tmpHPt, tmpEPt);
	painter->drawLine(ptOffsetEnd, QPointF(x1, y1));
	painter->drawLine(ptOffsetEnd, QPointF(x2, y2));

	///////线段上的文字
	painter->save();

	painter->translate(nTmpPosX, nTmpPosY);
	painter->rotate(tmpRota);
	painter->translate(-nTmpPosX, -nTmpPosY);

	painter->setPen(QPen(txtColor, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	painter->drawText(nTmpPosX, nTmpPosY, strRate);
	painter->restore();		
	////End

	if(nDirection == DA_RIGHT)
	{
		//tmpHPt = QPointF(m_headPt.x()/* + sin(m_Angle) * radius*/, m_headPt.y() + cos(m_Angle) * radius);
		//tmpEPt = QPointF(m_endPt.x() + sin(m_Angle) * radius, m_endPt.y() + cos(m_Angle) * radius);

		//
		//int ntmp_s_nodeWidth = 0;

		//ntmp_s_nodeWidth = m_S_nodeWidth + 8;

		//ptOffsetEnd = tmpEPt - QPointF(ntmp_s_nodeWidth / 2, 0);
		////ptOffsetEnd2= tmpEPt1 + QPointF(m_P_nodeWidth / 2, 0);

		////painter->drawLine(tmpHPt1, ptOffsetEnd2);
		////painter->drawLine(tmpHPt, tmpHPt1);
		////painter->drawLine(tmpHPt, ptOffsetEnd);
		//painter->drawLine(tmpHPt, tmpEPt);
		//////Draw Arrow
		//int nX = 0, nY = 0;
		//nX = 10 * cos(m_Angle);
		//nY = 10 * sin(m_Angle);
		//ptUpStart	= ptOffsetEnd + QPointF(-5, -5);
		//ptDownStart	= ptOffsetEnd + QPointF(-5, +5);

		//

		/////////线段上的文字
		//nTmpPosX = m_headPt.x() + 14;
		//nTmpPosY = m_headPt.y() + 12 + 10;	//多加10是因为drawText的是从文本左下角开始的坐标，所以要加个文本的高度
		//painter->save();
		//painter->setPen(QPen(txtColor, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
		//painter->drawText(nTmpPosX, nTmpPosY, QString("Rx:%1Mbps").arg(m_linkRate_recv));
		//painter->restore();
		//////End
	}
	else if(nDirection == DA_LEFT)
	{

	  /*      m_headPt       m_endPt
				<-----------------*/

		//tmpHPt = QPointF(m_headPt.x() - nPowerlinePos * 10, m_headPt.y() - 10);
		//tmpEPt = QPointF(m_endPt.x(), m_endPt.y() - 10);
		//

		//tmpHPt1 = QPointF(m_bottomPt.x() - nPowerlinePos * 10, m_bottomPt.y() - 10);
		//tmpEPt1 = QPointF(m_topPt.x(), m_topPt.y() - 10);
		////painter->drawLine(tmpHPt1, tmpEPt1);


		//ptOffsetEnd = tmpEPt1 + QPointF(m_P_nodeWidth / 2, 0);
		//ptOffsetEnd2 = tmpEPt - QPointF(m_S_nodeWidth / 2, 0);

		//painter->drawLine(tmpHPt, ptOffsetEnd2);	//->
		//painter->drawLine(tmpHPt, tmpHPt1);			//|
		//painter->drawLine(tmpHPt1, ptOffsetEnd);	//<-
		///////Draw Arrow
		//
		//ptUpStart	= ptOffsetEnd + QPointF(+5, -5);
		//ptDownStart	= ptOffsetEnd + QPointF(+5, +5);

		//
		/////////线段上的文字
		//nTmpPosX = m_headPt.x() + 14;
		//nTmpPosY = m_headPt.y() - 12;
		//painter->save();
		//painter->setPen(QPen(txtColor, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
		//painter->drawText(nTmpPosX, nTmpPosY, QString("Tx:%1Mbps").arg(m_linkRate_send));
		//painter->restore();
		////////End
	}




	

	//painter->drawLine(ptUpStart, ptOffsetEnd);
	//painter->drawLine(ptDownStart, ptOffsetEnd);
}

void PowerlineEdge::drawArrowLine(QPainter *painter, QColor &pColor, DIRECT_ARROW nDirection)
{
    Q_ASSERT(painter);
    QPointF ptUpStart,
            ptDownStart,
			ptOffsetEnd,
			ptOffsetEnd2;
	QColor  txtColor;
	QPointF tmpHPt, tmpEPt, tmpHPt1, tmpEPt1;
	

	txtColor = Qt::black;



    painter->setPen(QPen(pColor, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));


	/*      m_headPt       m_endPt
				<-----------------
				|
				|
	------------>
	m_topPt		m_bottomPt	*/
	int nOffset = sin(m_Angle);
	int nPowerlinePos = (nOffset == 0) ? 1 : nOffset;
	int nTmpPosX = 0, nTmpPosY = 0;

	if(nDirection == DA_RIGHT)
	{
		tmpHPt = QPointF(m_headPt.x() + nPowerlinePos * 10, m_headPt.y() + 10);
		tmpEPt = QPointF(m_endPt.x(), m_endPt.y() + 10);

		tmpHPt1 = QPointF(m_bottomPt.x() + nPowerlinePos * 10, m_bottomPt.y() + 10);
		tmpEPt1 = QPointF(m_topPt.x(), m_topPt.y() + 10);
		
		int ntmp_s_nodeWidth = 0;
		//if(m_devLedState)
		//{
			ntmp_s_nodeWidth = m_S_nodeWidth + 8;
		//}
		//else
		//{
		//	ntmp_s_nodeWidth = m_S_nodeWidth;
		//}
		ptOffsetEnd = tmpEPt - QPointF(ntmp_s_nodeWidth / 2, 0);
		ptOffsetEnd2= tmpEPt1 + QPointF(m_P_nodeWidth / 2, 0);

		painter->drawLine(tmpHPt1, ptOffsetEnd2);
		painter->drawLine(tmpHPt, tmpHPt1);
		painter->drawLine(tmpHPt, ptOffsetEnd);
		////Draw Arrow
		
		ptUpStart	= ptOffsetEnd + QPointF(-5, -5);
		ptDownStart	= ptOffsetEnd + QPointF(-5, +5);

		

		///////线段上的文字
		nTmpPosX = m_headPt.x() + 14;
		nTmpPosY = m_headPt.y() + 12 + 10;	//多加10是因为drawText的是从文本左下角开始的坐标，所以要加个文本的高度
		painter->save();
		painter->setPen(QPen(txtColor, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
		painter->drawText(nTmpPosX, nTmpPosY, QString("Rx:%1Mbps").arg(m_linkRate_recv));
		painter->restore();
		////////End
	}
	else if(nDirection == DA_LEFT)
	{

	  /*      m_headPt       m_endPt
				<-----------------
				|
				|
	------------>
	m_topPt		m_bottomPt	*/

		tmpHPt = QPointF(m_headPt.x() - nPowerlinePos * 10, m_headPt.y() - 10);
		tmpEPt = QPointF(m_endPt.x(), m_endPt.y() - 10);
		

		tmpHPt1 = QPointF(m_bottomPt.x() - nPowerlinePos * 10, m_bottomPt.y() - 10);
		tmpEPt1 = QPointF(m_topPt.x(), m_topPt.y() - 10);
		//painter->drawLine(tmpHPt1, tmpEPt1);


		ptOffsetEnd = tmpEPt1 + QPointF(m_P_nodeWidth / 2, 0);
		ptOffsetEnd2 = tmpEPt - QPointF(m_S_nodeWidth / 2, 0);

		painter->drawLine(tmpHPt, ptOffsetEnd2);	//->
		painter->drawLine(tmpHPt, tmpHPt1);			//|
		painter->drawLine(tmpHPt1, ptOffsetEnd);	//<-
		/////Draw Arrow
		
		ptUpStart	= ptOffsetEnd + QPointF(+5, -5);
		ptDownStart	= ptOffsetEnd + QPointF(+5, +5);

		
		///////线段上的文字
		nTmpPosX = m_headPt.x() + 14;
		nTmpPosY = m_headPt.y() - 12;
		painter->save();
		painter->setPen(QPen(txtColor, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
		painter->drawText(nTmpPosX, nTmpPosY, QString("Tx:%1Mbps").arg(m_linkRate_send));
		painter->restore();
		////////End
	}




	

	painter->drawLine(ptUpStart, ptOffsetEnd);
	painter->drawLine(ptDownStart, ptOffsetEnd);
}

PowerlineEdge & PowerlineEdge::operator = (const PowerlineEdge &edge)
{
	if(this != &edge)
	{
		m_onLine		= edge.m_onLine;
		m_isLocalHost	= edge.m_isLocalHost;
		m_wireless		= edge.m_wireless;
		m_bLocalDevice	= edge.m_bLocalDevice;
		m_devLedState	= edge.m_devLedState;
		m_deviceType	= edge.m_deviceType;
		m_signalLevel	= edge.m_signalLevel;
		m_signalStrength= edge.m_signalStrength;
		m_linkRate_send	= edge.m_linkRate_send;
		m_linkRate_recv	= edge.m_linkRate_recv;
		m_headPt		= edge.m_headPt;
		m_endPt			= edge.m_endPt;
		m_topPt			= edge.m_topPt;
		m_bottomPt		= edge.m_bottomPt;
		m_S_nodeHeight	= edge.m_S_nodeHeight;
		m_S_nodeWidth	= edge.m_S_nodeWidth;
		m_P_nodeWidth	= edge.m_P_nodeWidth;
		m_parent		= edge.m_parent;
		m_son			= edge.m_son;
		m_Angle			= edge.m_Angle;
		m_bArrow		= edge.m_bArrow;
	}


	return *this;
}




















































