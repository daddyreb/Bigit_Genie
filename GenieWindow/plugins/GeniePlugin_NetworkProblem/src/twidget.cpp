#include "twidget.h"

TWidget::TWidget(QWidget* parent,QString& filepath)
{
    path =filepath;
    this->setMaximumHeight(2);
    this->setMinimumHeight(2);
    this->setFocusPolicy(Qt::NoFocus);
    this->setAutoFillBackground(true);
    QPalette p;
    QPixmap pixmapbg(filepath);
    QSize size(this->width(),this->height());
    QPixmap pixmap(pixmapbg.scaled(size));
    p.setBrush(QPalette::Background, pixmap);
    this->setPalette(p);
}
TWidget::~TWidget()
{

}
void TWidget::enterEvent(QEvent *e)
{

}
void TWidget::leaveEvent(QEvent *e)
{

}
void TWidget::resizeEvent(QResizeEvent *e)
{
    this->setAutoFillBackground(true);
    QPalette p;
    QPixmap pixmapbg(path);
    QSize size(e->size().width(),e->size().height());
    QPixmap pixmap(pixmapbg.scaled(size));
    p.setBrush(QPalette::Background, pixmap);
    this->setPalette(p);
}
