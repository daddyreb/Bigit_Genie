#include <QVBoxLayout>
#include <QHBoxLayout>
#include "progresswidget.h"

ProgressWidget::ProgressWidget(QWidget *parent) :
    QWidget(parent)
{
    m_progress_indicator = new MyProgressIndicator(this);
    m_progress_indicator->setAnimationDelay(100);
    m_progress_indicator->setSizePara(1.0);

    QHBoxLayout *hor_layout = new QHBoxLayout();
    hor_layout->setSpacing(0);
    hor_layout->setMargin(0);
    hor_layout->addStretch(1);
    hor_layout->addWidget(m_progress_indicator);
    hor_layout->addStretch(1);

    QVBoxLayout  *ver_layout = new QVBoxLayout(this);
    ver_layout->setMargin(0);
    ver_layout->setSpacing(0);
    ver_layout->addStretch(1);
    ver_layout->addLayout(hor_layout);
    ver_layout->addStretch(1);
}

void ProgressWidget::start()
{
    m_progress_indicator->startAnimation();
}

void ProgressWidget::stop()
{
    m_progress_indicator->stopAnimation();
}

void ProgressWidget::setAnimationDelay(int delay)
{
    m_progress_indicator->setAnimationDelay(delay);
}

QSize ProgressWidget::sizeHint() const
{
    return QSize(60,60);
}
