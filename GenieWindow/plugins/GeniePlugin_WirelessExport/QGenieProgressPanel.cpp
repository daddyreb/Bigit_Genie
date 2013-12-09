#include "QGenieProgressPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

QGenieProgressPanel::QGenieProgressPanel(QWidget *parent) :
    QWidget(parent)
{
    m_progress_indicator = new QProgressIndicator(this);
    m_progress_indicator->setFixedSize(50,50);
    m_progress_indicator->setAnimationDelay(100);
    QHBoxLayout  *hor_layout = new QHBoxLayout();
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

void QGenieProgressPanel::start()
{
    m_progress_indicator->startAnimation();
}

void QGenieProgressPanel::stop()
{
    m_progress_indicator->stopAnimation();
}
void QGenieProgressPanel::setAnimationDelay(int delay)
{
    m_progress_indicator->setAnimationDelay(delay);
}

QSize QGenieProgressPanel::sizeHint() const
 {
    return QSize(60,60);
 }
