#ifndef __QGENIEABOUTTHREAD_H
#define __QGENIEABOUTTHREAD_H

#ifndef IN_STABLE_H
#include <QObject>
#include <QThread>
#include <QRgb>
#include <QVector>
#include <QImage>
#include <QMutex>
#endif

//QT_BEGIN_NAMESPACE
//class QImage;
//QT_END_NAMESPACE

class QGenieAboutThread : public QThread
{
    Q_OBJECT
public:
    QGenieAboutThread();
    ~QGenieAboutThread();
    void stopThread();
    void startThread();
    void resetBG();

protected:
    void run();
    bool mLocked;

signals:
    void sendupdate();
    void signal_ResizeWnd(const QSize &siz);

public:
    void updateWater(int npage, int density);

    void addWave(int x, int y, int radius, int height);

    void drawWater(QRgb* srcImage, QRgb* dstImage);
    void selfstop();

    static QRgb shiftColor(QRgb color,int shift) {
        return qRgb(qBound(0, qRed(color) - shift,  255),
            qBound(0, qGreen(color) - shift,255),
            qBound(0, qBlue(color) - shift, 255));
    }

    void prepareDraw();

    int			    mPage;
    int			    mDensity;
    QVector<int>    mHeightField1;
    QVector<int>    mHeightField2;
    QVector<int>    mFullHeight;
    int             mWantUpdatePointNum;
    QVector<int>    mWantUpdatePoint;

    QImage          mImage1;
    QImage          mImage2;
 //   RsIface         *rsiface;
  //  RsDisc          *rsDisc;

    QMutex mMutex;
    QMutex mLock; //use this to let this function not run when the about window not show
    QMutex mDrawMutex;
    bool mAbort;
    QTime *mTime;
    int mCurTime;

    int mImageW;
    int mImageH;
    int mHeightW;
    int mHeightH;

    int mRandomCounter;
};

#endif // THREAD_H
