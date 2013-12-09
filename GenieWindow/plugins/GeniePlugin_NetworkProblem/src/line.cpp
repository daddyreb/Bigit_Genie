#include "line.h"

line::line(QWidget* parent)
        :QLineEdit(parent)
{
    QLineEdit::setText(QString(tr("www.netgear.com")));
}
line::~line()
{

}
//void line::mousePressEvent(QMouseEvent *event)
//{
////    QLineEdit::setStyleSheet("border-color: rgb(255, 0, 0)");
//    return QLineEdit::mousePressEvent(event);
//}
//void line::mouseReleaseEvent(QMouseEvent *event)
//{
////    QLineEdit::setStyleSheet("border-color: rgb(255, 255, 255)");
//    return QLineEdit::mouseReleaseEvent(event);
//}
//void line::enterEvent(QEvent *event)
//{
////    this->setStyleSheet(QString::fromUtf8("border-color: rgb(116, 113, 190);"));
//    emit finished();
//}
//void line::leaveEvent(QEvent *event)
//{
////    this->setStyleSheet(QString::fromUtf8("border-color: rgb(187, 187, 187);"));
//    emit finished();
//}
