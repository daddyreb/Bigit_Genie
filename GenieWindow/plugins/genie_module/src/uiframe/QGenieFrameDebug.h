#ifndef QGenieFrameDebug_H
#define QGenieFrameDebug_H
#ifndef IN_STABLE_H
#include <QObject>
#include <QFrame>
#include "Singleton.h"
#include "ui_QGenieFrameDebug.h"
#endif



class QGenieFrameDebug : public QFrame,public Singleton<QGenieFrameDebug>
{
    Q_OBJECT
public:
    QGenieFrameDebug(QWidget *parent=NULL);

    Ui::QGenieFrameDebug mUi;

protected:
    void changeEvent(QEvent* event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    QPoint mDragPosition;
    bool mDrag;

//signals:
//    void signal_SetButState(int idx,int state);

private slots:
    void slot_but0();
    void slot_but1();
    void slot_but2();
    void slot_but3();
    void slot_but4();
    void slot_but5();
    void slot_but6();
    void slot_but7();
    void slot_but8();
    void slot_but9();
    void slot_but10();
    void slot_but11();
    void on_but1_clicked();
    void on_but_crash_clicked();
    void on_but1_2_clicked();
    void on_but1_3_clicked();
    void on_cob_routetype_currentIndexChanged(QString );
    void on_but_gacc_timer_clicked();
    void on_but_dbg_plugin_clicked();
};

#endif // QGenieFrameDebug_H
