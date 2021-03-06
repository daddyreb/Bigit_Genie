#ifndef __QGENIEABOUTTHREAD_H
#define __QGENIEABOUTTHREAD_H

#include <QThread>
#include <QRgb>
#include <QImage>
#include <QMutex>

#include "Globals.h"
#include "QGenie_MFNetTraffic.h"

class QGenie_qtNetTraffic;



//QT_BEGIN_NAMESPACE
//class QImage;
//QT_END_NAMESPACE

class QGenieAboutThread : public QThread
{
    Q_OBJECT
public:
    QGenieAboutThread();
    ~QGenieAboutThread();
    QGenie_qtNetTraffic* mw;
    void stopThread();
    void startThread();
    void resetBG();
    void getTrafficData();
    int iThree;

protected:
    void run();


signals:
    void sendTraffic();

public:

    void selfstop();

    double averageincoming(int icount,double dbltraffic);
    double averageoutgoing(int icount,double dbltraffic);
    double averagetotal(int icount,double dbltraffic);

    bool mAbort;

    bool mLocked;
    QMutex mLock; //use this to let this function not run when the about window not show
    QMutex mDrawMutex;
    QMutex mMutex;

    QTime *mTime;
    int mCurTime;


};

#endif // THREAD_H
