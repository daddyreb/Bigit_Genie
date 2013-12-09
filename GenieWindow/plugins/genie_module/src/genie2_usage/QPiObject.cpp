#include "QPiObject.h"
#include <QtCore>

QPiObject::QPiObject(QWidget *parent, const QString &child_objname)
    :m_angle(0)
    ,m_timerId(-1)
    ,m_delay(40)
    ,m_displayedWhenStopped(false)
    ,m_color(Qt::black)
{
    Q_ASSERT(parent);
    QWidget *w=parent;
    if(!child_objname.isEmpty())
    {
        w=qFindChild<QWidget *>(parent,child_objname);
        Q_ASSERT(w);
//        w=(!w)?parent:w;
    }
    mW=w;
    setParent(w);
    w->installEventFilter(this);
}
const qreal cSizePara=0.7;
bool QPiObject::eventFilter(QObject *o, QEvent *env)
{
    QEvent::Type type=env->type();
    if(type==QEvent::Paint)
    {
        QWidget *w=qobject_cast<QWidget *>(o);
        Q_ASSERT(w==mW);


        if (!m_displayedWhenStopped && !isAnimated())
        {
            return false;
        }

        int width = qMin(w->width(), w->height())/2;

        QPainter p(w);
        p.setRenderHint(QPainter::Antialiasing);

        qreal outerRadius = (width-1)*cSizePara;
        qreal innerRadius = (width-1)*cSizePara*0.38;

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
            p.translate(w->rect().center());
            p.rotate(m_angle - i*30.0f);
            p.drawRoundedRect(-capsuleWidth*0.5, -(innerRadius+capsuleHeight), capsuleWidth, capsuleHeight, capsuleRadius, capsuleRadius);
            p.restore();
        }

        return true;
    }
    return false;
}

void QPiObject::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    m_angle = (m_angle+30)%360;

    mW->update();
}

void QPiObject::startAnimation()
{
    if(m_timerId<0)
        m_timerId=startTimer(m_delay);
    mW->update();
}

void QPiObject::stopAnimation()
{
    killTimer(m_timerId);
    m_timerId=-1;
    mW->update();
}


