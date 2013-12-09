#include "progressindicator.h"

#include <QPainter>

ProgressIndicator::ProgressIndicator(QWidget* parent)
        : QWidget(parent),
        m_angle(0),
        m_timerId(-1),
        m_delay(40),
        m_displayedWhenStopped(false),
        mSizePara(0.7),
        m_color(Qt::black)

{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFocusPolicy(Qt::NoFocus);
}

bool ProgressIndicator::isAnimated () const
{
    return (m_timerId != -1);
}

void ProgressIndicator::setDisplayedWhenStopped(bool state)
{
    m_displayedWhenStopped = state;

    update();
}

bool ProgressIndicator::isDisplayedWhenStopped() const
{
    return m_displayedWhenStopped;
}

void ProgressIndicator::startAnimation()
{
    m_angle = 0;

    if (m_timerId == -1)
        m_timerId = startTimer(m_delay);
}

void ProgressIndicator::stopAnimation()
{
    if (m_timerId != -1)
        killTimer(m_timerId);

    m_timerId = -1;

    update();
}

void ProgressIndicator::setAnimationDelay(int delay)
{
    if (m_timerId != -1)
        killTimer(m_timerId);

    m_delay = delay;

    if (m_timerId != -1)
        m_timerId = startTimer(m_delay);
}

void ProgressIndicator::setColor(const QColor & color)
{
    m_color = color;

    update();
}

QSize ProgressIndicator::sizeHint() const
{
    return QSize(50,50);
}

int ProgressIndicator::heightForWidth(int w) const
{
    return w;
}

void ProgressIndicator::timerEvent(QTimerEvent * /*event*/)
{
    m_angle = (m_angle+30)%360;

    update();
}

void ProgressIndicator::setSizePara(qreal c)
{
    mSizePara=c;
}

void ProgressIndicator::paintEvent(QPaintEvent * /*event*/)
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
