#include "QGenieColumnLabel.h"

QGenieColumnLabel::QGenieColumnLabel(QWidget *parent) :
    QLabel(parent)
{
}

void   QGenieColumnLabel::mouseReleaseEvent ( QMouseEvent * event )
{
    if(event->button()==Qt::LeftButton)
        emit clicked();

    QLabel::mouseReleaseEvent(event);
}
