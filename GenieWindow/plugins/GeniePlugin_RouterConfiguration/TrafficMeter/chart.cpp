#include "chart.h"
#include <QPainter>
#include <QDebug>
#include "genie2_interface.h"
#include "genie2_netgear.h"
#include "language_RouterConfig.h"

Chart::Chart(int titleid, int unitid, QWidget *parent)
    :QWidget(parent),
    titleid(titleid),
    unitid(unitid)
{
    Chart::retranslateUi();
}

void Chart::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    if(event->type() == QEvent::LanguageChange)
    {
        Chart::retranslateUi();
    }
}

void Chart::retranslateUi()
{
    title = GENIE2_GET_TEXT(titleid);
    unit = GENIE2_GET_TEXT(unitid);
}

BarChart::BarChart(int title, int unit, QList<int>xtitleid, QWidget *parent)
    :Chart(title,unit,parent),
    xtitleid(xtitleid)
{
    data.clear();
    for(int i=0; i<xtitleid.count();i++)
    {
        data << 0 << 0;
    }

    spacewidth = 20;
    barwidth = 60;
    coordinatescalewidth = 41;
    coordinatescaleheight = 20;
    ybarlength = coordinatescaleheight *6;
    xbarlength = xtitleid.count() * (barwidth + spacewidth) + spacewidth;
    unitwidth = 18;
    unitheight = ybarlength;
    xtitleheight = 36;

    colorflagrectwidth=10;
    colorflagtextwidth=90;
    colorflagtextheight=2*coordinatescaleheight-colorflagrectwidth;


    retranslateUi();
}

void BarChart::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPen dashlinepen,oldpen;
    QPen graypen,bluepen;
    QColor blue(159,154,255);
    QColor gray(102,102,102);
    QFont font,oldfont;
#ifdef Q_OS_MAC
    font.setPointSize(12);
#else
    font.setPointSize(8);
#endif
    graypen.setColor(gray);
    bluepen.setColor(blue);
    int zeropixelheight;

    painter.setFont(font);
    oldpen = painter.pen();

    float maxtotal=100;
    int numperpixel;
    for(int i=0; i<data.count();i+=2)
    {
        if(data.at(i)+data.at(i+1) - maxtotal > 0.0)
        {
            maxtotal = data.at(i)+data.at(i+1);
        }
    }

    if((int)maxtotal%(5*coordinatescaleheight) !=0)
    {
        numperpixel = ((int)maxtotal/(5*coordinatescaleheight))+1;
    }else
    {
        numperpixel = (int)maxtotal/(5*coordinatescaleheight);
    }

    dashlinepen.setStyle(Qt::DashLine);
    dashlinepen.setColor(QColor(204,204,204));
    QVector<qreal> dashes;
    dashes << 2 << 3;
    qDebug() << "dashlinepen.width" << dashlinepen.width();
    dashlinepen.setDashPattern(dashes);

    xmargin = (width() - (unitwidth+coordinatescalewidth+xbarlength+colorflagtextwidth))/2;
    ymargin = (height() - (xtitleheight + ybarlength))/2;

    QTransform transform,oldtransform;
    oldtransform = painter.transform();
    transform.rotate(-90);
    transform.translate(-(unitheight/2+ymargin), (unitwidth/2+xmargin));
    painter.setTransform(transform);
    painter.drawText(QRect(-unitheight/2,-unitwidth/2,unitheight,unitwidth),
                     Qt::AlignVCenter|Qt::AlignHCenter|Qt::TextWordWrap,unit);
    painter.setTransform(oldtransform);

    for(int i=0; i<6;i++)
    {
    painter.drawText(QRect(xmargin+unitwidth,ymargin+(i+1)*coordinatescaleheight,
                           coordinatescalewidth-5,coordinatescaleheight),
                     Qt::AlignHCenter|Qt::TextWordWrap,
                     QString("%1").arg((5-i)*coordinatescaleheight * numperpixel));
    if(i != 5)
    {
        painter.drawLine(xmargin+unitwidth+coordinatescalewidth-5,ymargin+(i+1)*coordinatescaleheight,
                     xmargin+unitwidth+coordinatescalewidth,ymargin+(i+1)*coordinatescaleheight);
    }else
    {
        zeropixelheight = ymargin+(i+1)*coordinatescaleheight;
    }
    }

   painter.drawLine(xmargin+unitwidth+coordinatescalewidth,ymargin,
                     xmargin+unitwidth+coordinatescalewidth,ymargin+ybarlength);

   for(int i=0; i<xtitle.count(); i++)
   {
       painter.drawText(QRect(xmargin+unitwidth+coordinatescalewidth+spacewidth/2+i*(spacewidth+barwidth),
                              ymargin+ybarlength,spacewidth+barwidth,xtitleheight),
                        Qt::AlignVCenter|Qt::AlignHCenter|Qt::TextWordWrap,xtitle.value(i));
   }

   painter.setPen(dashlinepen);
   for(int i=0; i<6;i++)
   {
       if(i==5)
       {
           painter.setPen(oldpen);
       }
        painter.drawLine(xmargin+unitwidth+coordinatescalewidth,ymargin+(i+1)*coordinatescaleheight,
                    xmargin+unitwidth+coordinatescalewidth+xbarlength,ymargin+(i+1)*coordinatescaleheight);
   }
#ifdef Q_OS_MACX
    const int cRightFontOff=3;
#else
   const int cRightFontOff=0;
#endif
   painter.setBrush(gray);
   painter.setPen(graypen);
   painter.drawRect(xmargin+unitwidth+coordinatescalewidth+xbarlength+colorflagrectwidth,ymargin,
                    colorflagrectwidth,colorflagrectwidth);
   painter.setPen(oldpen);
   painter.drawText(QRect(xmargin+unitwidth+coordinatescalewidth+xbarlength+colorflagrectwidth,
                          ymargin+colorflagrectwidth+cRightFontOff,colorflagtextwidth,colorflagtextheight),
                    Qt::AlignLeft|Qt::TextWordWrap,uploadstr);

   painter.setBrush(blue);
   painter.setPen(bluepen);
   painter.drawRect(xmargin+unitwidth+coordinatescalewidth+xbarlength+colorflagrectwidth,
                    ymargin+2*coordinatescaleheight,colorflagrectwidth,colorflagrectwidth);
   painter.setPen(oldpen);
   painter.drawText(QRect(xmargin+unitwidth+coordinatescalewidth+xbarlength+colorflagrectwidth,
                          ymargin+2*coordinatescaleheight+colorflagrectwidth+cRightFontOff,colorflagtextwidth,colorflagtextheight),
                    Qt::AlignLeft|Qt::TextWordWrap,downloadstr);

   oldfont = painter.font();

   for(int i=0; i<data.count(); i+=2)
   {
       painter.setBrush(gray);
       painter.setPen(oldpen);
       painter.drawText(QRect(xmargin+unitwidth+coordinatescalewidth+spacewidth*(i/2+1)+barwidth*i/2,
                              zeropixelheight-(data.at(i)+data.at(i+1))/numperpixel-26,
                              barwidth,26),
                        Qt::AlignCenter|Qt::TextWordWrap,QString("%1").arg(data.at(i)));
        painter.setPen(graypen);
        painter.drawRect(xmargin+unitwidth+coordinatescalewidth+spacewidth*(i/2+1)+barwidth*i/2,
                         zeropixelheight-(data.at(i)+data.at(i+1))/numperpixel,
                         barwidth,data.at(i)/numperpixel);
        painter.setBrush(blue);
        painter.setPen(bluepen);
        painter.drawRect(xmargin+unitwidth+coordinatescalewidth+spacewidth*(i/2+1)+barwidth*i/2,
                         zeropixelheight-data.at(i+1)/numperpixel,
                         barwidth,data.at(i+1)/numperpixel);
        painter.setPen(oldpen);
        if(data.at(i+1)/numperpixel>16)
        {
        painter.drawText(QRect(xmargin+unitwidth+coordinatescalewidth+spacewidth*(i/2+1)+barwidth*i/2,
                               zeropixelheight-data.at(i+1)/numperpixel,
                               barwidth,data.at(i+1)/numperpixel),
                         Qt::AlignCenter|Qt::TextWordWrap,QString("%1").arg(data.at(i+1)));
        }else
        {
            QString numstr = QString("%1").arg(data.at(i+1));
            painter.drawText(xmargin+unitwidth+coordinatescalewidth+spacewidth*(i/2+1)+barwidth*i/2+((barwidth-QFontMetrics(font).width(numstr.toStdString().c_str()))/2),
                                                                                                     zeropixelheight,numstr);
        }
    }
   painter.setPen(oldpen);
}

void BarChart::updateData(QList<float> d)
{
    data = d;
    update();
}

void BarChart::retranslateUi()
{
    uploadstr=GENIE2_GET_TEXT(L_TRAFFICMETER_UPLOAD);
    downloadstr=GENIE2_GET_TEXT(L_TRAFFICMETER_DOWNLOAD);
    xtitle.clear();
    for(int i=0; i<xtitleid.count();i++)
    {
        xtitle.append(GENIE2_GET_TEXT(xtitleid.at(i)));
    }
    update();
}

void BarChart::changeEvent(QEvent *event)
{
    Chart::changeEvent(event);
    if(event->type() == QEvent::LanguageChange)
    {
        retranslateUi();
    }
}
