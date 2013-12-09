#ifndef QPIOBJECT_H
#define QPIOBJECT_H

#include <QtGui>

class QPiObject : public QObject
{
    Q_OBJECT
public:
    explicit QPiObject(QWidget *parent,const QString &child_objname=QString());
    void startAnimation();
    void stopAnimation();
    virtual void timerEvent(QTimerEvent * event);
    QWidget *mW;
protected:
    virtual bool eventFilter(QObject *, QEvent *);

    inline bool isAnimated() const
    {
        return (m_timerId>=0);
    }
protected:
    int m_angle;
    int m_timerId;
    int m_delay;
    bool m_displayedWhenStopped;
    QColor m_color;




};

#endif // QPIOBJECT_H
