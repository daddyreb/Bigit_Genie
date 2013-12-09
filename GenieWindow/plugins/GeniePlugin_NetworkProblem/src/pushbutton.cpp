#include "pushbutton.h"
#include <QDebug>
#include <QApplication>
PushButton::PushButton(QWidget* parent)
    :QPushButton(parent)
{

}
PushButton::~PushButton()
{

}
static int testI;
void PushButton::enterEvent(QEvent *event)
{
    qDebug()<<"PushButton::enterEvent"<<testI++;
//    QPushButton::setCursor(Qt::PointingHandCursor);
    qApp->setOverrideCursor(Qt::PointingHandCursor);
    QPushButton::enterEvent(event);
    update();
}
void PushButton::leaveEvent(QEvent *event)
{
    qDebug()<<"PushButton::leaveEvent"<<testI++;
//    QPushButton::setCursor(Qt::ArrowCursor);
    qApp->setOverrideCursor(Qt::ArrowCursor);
    QPushButton::leaveEvent(event);
    update();
}
