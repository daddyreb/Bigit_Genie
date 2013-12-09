#include "label.h"

Flexlabel::Flexlabel(QWidget* parent)
    :QLabel(parent)
{

}
Flexlabel::~Flexlabel()
{

}
void Flexlabel::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        emit labelclick();
    }
}
void Flexlabel::enterEvent(QEvent *event)
{
    QLabel::setCursor(Qt::PointingHandCursor);
}
void Flexlabel::leaveEvent(QEvent *event)
{
    QLabel::setCursor(Qt::ArrowCursor);
}
