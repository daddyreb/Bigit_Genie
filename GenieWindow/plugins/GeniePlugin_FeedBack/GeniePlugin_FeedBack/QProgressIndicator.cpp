#include "QProgressIndicator.h"

#include <QPainter>

QProgressIndicator::QProgressIndicator(QWidget* parent)
        : QWidget(parent),
        m_angle(0),
        m_timerId(-1),
        m_delay(40),
        m_displayedWhenStopped(false),
        m_color(Qt::black),
        mSizePara(0.7)

{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFocusPolicy(Qt::NoFocus);
}

bool QProgressIndicator::isAnimated () const
{
    return (m_timerId != -1);
}

void QProgressIndicator::setDisplayedWhenStopped(bool state)
{
    m_displayedWhenStopped = state;

    update();
}

bool QProgressIndicator::isDisplayedWhenStopped() const
{
    return m_displayedWhenStopped;
}

void QProgressIndicator::startAnimation()
{
    m_angle = 0;

    if (m_timerId == -1)
        m_timerId = startTimer(m_delay);
}

void QProgressIndicator::stopAnimation()
{
    if (m_timerId != -1)
        killTimer(m_timerId);

    m_timerId = -1;

    update();
}

void QProgressIndicator::setAnimationDelay(int delay)
{
    if (m_timerId != -1)
        killTimer(m_timerId);

    m_delay = delay;

    if (m_timerId != -1)
        m_timerId = startTimer(m_delay);
}

void QProgressIndicator::setColor(const QColor & color)
{
    m_color = color;

    update();
}

QSize QProgressIndicator::sizeHint() const
{
    return QSize(50,50);
}

int QProgressIndicator::heightForWidth(int w) const
{
    return w;
}

void QProgressIndicator::timerEvent(QTimerEvent * /*event*/)
{
    m_angle = (m_angle+30)%360;

    update();
}

void QProgressIndicator::setSizePara(qreal c)
{
    mSizePara=c;
}

void QProgressIndicator::paintEvent(QPaintEvent * /*event*/)
{
    if (!m_displayedWhenStopped && !isAnimated())
         return;

     qreal width = qMin(this->width()/2, this->height()/2);

     QPainter p(this);
     p.setRenderHint(QPainter::Antialiasing);

     qreal outerRadius = (width-1)*mSizePara;
     qreal innerRadius = (width-1)*mSizePara*0.38;

     qreal capsuleHeight = outerRadius - innerRadius;
     qreal capsuleWidth  = (width > 32 ) ? capsuleHeight *.23 : capsuleHeight *.35;
     qreal capsuleRadius = capsuleWidth/2;

     for (int i=0; i<12; i++)
     {
         QColor color = m_color;
         color.setAlphaF(1.0f - (i/12.0f));
         p.setPen(Qt::NoPen);

         p.setBrush(color);
         p.save();
         p.translate(rect().center());
         p.rotate(m_angle - i*30.0f);
         p.drawRoundedRect(-capsuleWidth*0.5, -(innerRadius+capsuleHeight), capsuleWidth, capsuleHeight, capsuleRadius, capsuleRadius);
         p.restore();
     }
}
