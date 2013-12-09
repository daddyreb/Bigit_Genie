#include "detailviewcontentwidget.h"
#include <QPaintEvent>
#include <QRectF>
#include <QPainter>
#include <QPoint>
#include "genie2_interface.h"

#ifdef Q_OS_WIN
#define IMAGE_PATH "map/others/DetailViewFrameBkg.png"
#elif defined Q_OS_MACX
#define IMAGE_PATH "map/others/Mac_DetailViewFrameBkg.png"
#endif

DetailViewContentWidget::DetailViewContentWidget(QWidget *parent) :
    QWidget(parent)
{
}

void DetailViewContentWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPixmap pixmap(GENIE2_RES(IMAGE_PATH));
    QRect src(QPoint(0,0),pixmap.size()),des(rect());

    src.adjust(10,40,-10,-10);

    QPainter painter(this);
    painter.drawPixmap(des,pixmap,src);
}
