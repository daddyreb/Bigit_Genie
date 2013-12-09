#ifndef _QGENIEFRAMEABOUT_H
#define _QGENIEFRAMEABOUT_H
#ifndef IN_STABLE_H
#include <QObject>
#include <QDialog>
#include <QPoint>
#include "ui_QGenieFrameAbout.h"
#endif

namespace Ui {
    class QGenieFrameAbout;
}

class QGenieFrameAbout : public QDialog {
    Q_OBJECT
public:
    QGenieFrameAbout(QWidget *parent = 0);
    ~QGenieFrameAbout();

protected:
    void changeEvent(QEvent *e);
#ifndef SHEET_STYLE_FORMAC
protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    bool mDrag;
    QPoint mDragPosition;
#endif
private:
    Ui::QGenieFrameAbout *ui;

 //   QRect boardRect() const {return QRect(1, 1, width()-2, height()-2);}
 //   QRect frameRect() const {return QRect(0, 0, width()-1, height()-1);}
};

#endif // QGENIEFRAMEABOUT_H
