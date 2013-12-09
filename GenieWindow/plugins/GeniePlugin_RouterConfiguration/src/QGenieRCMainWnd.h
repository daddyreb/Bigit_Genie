#ifndef QGENIERCMAINWND_H
#define QGENIERCMAINWND_H

#include <QWidget>

namespace Ui {
    class QGenieRCMainWnd;
}

class QGenieRCMainWnd : public QWidget
{
    Q_OBJECT

public:
    explicit QGenieRCMainWnd(QWidget *parent = 0);
    ~QGenieRCMainWnd();

private:
    Ui::QGenieRCMainWnd *ui;
};

#endif // QGENIERCMAINWND_H
