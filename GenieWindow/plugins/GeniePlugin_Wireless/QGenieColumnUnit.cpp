#include "QGenieColumnUnit.h"
#include <QVBoxLayout>

#define COLUMNUNIT_MAXWIDTH 30

QGenieColumnUnit::QGenieColumnUnit(QWidget *parent) :
    QWidget(parent)
{
    //disable not to response to user's click event
    this->setEnabled(false);
    this->setFixedWidth(COLUMNUNIT_MAXWIDTH);

    m_column_lb     =   new QGenieColumnLabel(this);
    m_column_lb->setAlignment(Qt::AlignCenter);
    m_column_widget = new QGenieColumnWidget(this);

    QVBoxLayout     *unit_layout = new QVBoxLayout(this);
    unit_layout->setMargin(0);
    unit_layout->setSpacing(0);

    unit_layout->addStretch(1);
    unit_layout->addWidget(m_column_lb);
    unit_layout->addWidget(m_column_widget);

    unit_layout->setStretch(0,0);
    unit_layout->setStretch(0,1);
}

int  QGenieColumnUnit::getColumnBlockHeight()
{
    return size().height() - m_column_lb->sizeHint().height();
}

void QGenieColumnUnit::setLabelText(const QString &str_lable)
{
    m_column_lb->setText(str_lable);
}

void QGenieColumnUnit::setColumnTooltip(const QString &tooltip)
{
    m_column_widget->setToolTip(tooltip);
}

void QGenieColumnUnit::setColumnBlockHeight(int height)
{
    m_column_widget->m_column_height = height;
    m_column_widget->updateGeometry();
}

void QGenieColumnUnit::updateColumnBlock(bool btwoParts,int totalParts)
{
    m_column_widget->m_bdrawTwoParts = btwoParts;
    m_column_widget->m_itotalParts   = totalParts;
    m_column_widget->update();
}
