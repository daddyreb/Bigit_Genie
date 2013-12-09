#include "labelbtn.h"
#include <QHBoxLayout>

LabelBtn::LabelBtn(QWidget *parent)
    :QLabel(parent)
{
}


void LabelBtn::mousePressEvent(QMouseEvent *ev)
{
    if(ev->button() == Qt::LeftButton)
    {
        emit labelClicked();
    }
}
