#ifndef QMYWIDGET_H
#define QMYWIDGET_H

#include <QWidget>
#include "QGenieBaseWidget.h"

namespace Ui {
    class QMyWidget;
}


class QMyWidget : public QGenieBaseWidget
{
    Q_OBJECT

public:
    explicit QMyWidget(QWidget *parent = 0);
    ~QMyWidget();
protected:
    void changeEvent(QEvent *e);
private:
    Ui::QMyWidget *ui;
};

#endif // QMYWIDGET_H
