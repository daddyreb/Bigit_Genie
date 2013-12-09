#ifndef SVTTRACER_H
#define SVTTRACER_H
#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include "Singleton.h"

class SVT_Tracer:public QObject,public Singleton<SVT_Tracer>
{
    Q_OBJECT
public:
    SVT_Tracer(QObject *parent=NULL);
    ~SVT_Tracer();
    void whenArp();
    QWidget *getUi();
protected:
    QDateTime mDt;
    QTimer mTimer;
    QMutex mMutex;
    QLabel *mDbgW;
    int mArpCounter;
    bool eventFilter(QObject *, QEvent *);
    QVector<int> mData;
    int mDataIdx;
protected slots:
    void slot_Timeout();

};

#endif // SVTTRACER_H
