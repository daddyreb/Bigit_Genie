#ifndef QGENIEBUTTONSUB_H
#define QGENIEBUTTONSUB_H

#ifndef IN_STABLE_H
#include <QtGui>
#include "QGenieButtonSub.h"
#include "ui_QGenieButtonSub.h"
#endif

namespace Ui {
    class QGenieButtonSub;
}

class QGenieButtonSub : public QPushButton
{
    Q_OBJECT

public:
    explicit QGenieButtonSub(QWidget *parent = 0);
    ~QGenieButtonSub();

public:
    Ui::QGenieButtonSub *ui;
};

#endif // QGENIEBUTTONSUB_H
