#ifndef QGENIEFRAMEMAIN_H
#define QGENIEFRAMEMAIN_H
class QGenieFrameMain;


#ifndef IN_STABLE_H
#include <QObject>
#include "Singleton.h"
#include <QFrame>
#include "ui_QGenieFrameMain.h"
#include <QSplashScreen>
#include "QGenieWatchDog.h"
#endif

#include "Singleton.h"
namespace Ui{
    class QGenieFrameMain;
}

class QGenieFrameMain : public QWidget,public Singleton<QGenieFrameMain>
{
    Q_OBJECT
public:
    QGenieFrameMain(QWidget *parent=NULL);
    ~QGenieFrameMain();
    inline const QSize getStackSize()
    {
      return mUi.frame_stack->size(); 
    };
    void startSplash();
    void endSplash();

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
#ifdef CAN_MAXIMIZE_MAIN

    void mouseDoubleClickEvent ( QMouseEvent * event )  ;
#endif
protected:
    void changeEvent(QEvent* event);
private:
    Ui::QGenieFrameMain mUi;
    QPoint mDragPosition;
    bool mDrag;
#ifdef SELF_SPLASH
    QGenieSplash *mSplash;
#else
    QSplashScreen *mSplash;
#endif

    QTimer *mSplashTimer;
//    QTimer *mNotAlwaysOntopTimer;
#ifdef MEMORY_BUG_FIXER
    QTimer *mMemoryBugTimer;
#endif
    QGenieWatchDog mDog;
public slots:
    void slot_SplashTimeout();
#ifdef MEMORY_BUG_FIXER
    void slot_MemoryBugTimer();
#endif
    void slot_ShowNormal();
//    void slot_setNotAlwaysOntop();
    friend class QGenieTrayManager;
	friend class QGenieTrayIco_Out;
    friend class QGenieAllSingletonClasses;

};

#endif // QGENIEFRAMEMAIN_H
