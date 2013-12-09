#include "QGenieColumnWidget.h"
#include "QGenieWirelessCommon.h"
#include "GeniePlugin_Wireless.h"

#include <QPainter>
#include <QLinearGradient>
#include <QRectF>
#include <assert.h>


QGenieColumnWidget::QGenieColumnWidget(QWidget *parent) :
        QWidget(parent),
        m_bdrawTwoParts(false),
        m_itotalParts(0)
{
    m_column_height = COLUMN_BLOCK_MINIMUMHEIGHT;
}

void QGenieColumnWidget::paintEvent (QPaintEvent *event)
{
    QPainter    painter(this);

    painter.setPen(Qt::NoPen);
    QLinearGradient lg(this->rect().left(),this->rect().top(),this->rect().right(),this->rect().bottom());

    if(m_bdrawTwoParts)
    {
//        assert(m_itotalParts != 0);
        lg.setColorAt(0,QColor(0,113,215));
        lg.setColorAt(1,QColor(0,113,215));
        painter.setBrush(lg);
        painter.drawRect(this->rect());

        if(m_itotalParts >= 2)
        {
            float factor = float(m_itotalParts-1) / float(m_itotalParts);

            QRectF tmp_rect(this->rect().left(),this->rect().top()
                   ,this->rect().width(),this->rect().height()*factor);

            QLinearGradient lg_tmp(tmp_rect.topLeft(),tmp_rect.bottomRight());
            lg_tmp.setColorAt(0,QColor(238,210,51));
            lg_tmp.setColorAt(1,QColor(238,210,51));

            painter.setBrush(lg_tmp);
            painter.drawRect(tmp_rect);
        }

    }
    else
    {

        lg.setColorAt(0,QColor(238,210,51));
        lg.setColorAt(1,QColor(238,210,51));
        painter.setBrush(lg);
        painter.drawRect(this->rect());
    }
}

QSize QGenieColumnWidget::sizeHint() const
{
    return QSize(2,m_column_height);
}


