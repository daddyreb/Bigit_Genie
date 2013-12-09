#ifndef QGENIETRAYMOVIEMANAGER_H
#define QGENIETRAYMOVIEMANAGER_H

#ifndef IN_STABLE_H
#include <QObject>
#include <QtGui>
#endif


#define MOVIE_PROPOSAL 1

#ifndef IN_STABLE_H
#include <QObject>
#include <QtGui>
#include "Singleton.h"
#endif


class QGenieTrayIco_In;

class QGenieTrayMovieManager:public QObject,public Singleton<QGenieTrayMovieManager>
{
    Q_OBJECT
public:
    QGenieTrayMovieManager(QGenieTrayIco_In *trayico);
    void startMovie(int msec=0);
    void stopMovie();
    bool isOn();
private:
    QGenieTrayIco_In *mTrayIco;
    QTimeLine mTimeLine;
#if MOVIE_PROPOSAL==0
    QMovie mMovie;
#endif
    bool mOn;

protected slots:
    void slot_FrameUpdate(int val);

};

#endif // QGENIETRAYMOVIEMANAGER_H
