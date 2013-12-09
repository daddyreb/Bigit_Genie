#ifndef QSTACKMANAGER_H
#define QSTACKMANAGER_H

#ifndef IN_STABLE_H
#include <QObject>
#include "Singleton.h"
#include "QGenieFrameStackView.h"
#endif

class QGeniePageMovie;
class IPluginInf;
class QGeniePageHome;


class QGenieButtonStackViewController;
class QGeniePagePluginProxy;

struct PlgMsg;

class QGenieStackManager:public QObject,public Singleton<QGenieStackManager>
{
    Q_OBJECT
public:
    QGenieStackManager();
    ~QGenieStackManager();
    void setStackView(QGenieFrameStackView *stackview);
    void changeStackViewIdx(QWidget *w=NULL);

    inline int getPageNum(){
        return mPageNum;
    };
#ifdef CAN_LOCK_STACK
    inline bool canAutoChangePage()
    {
        return mCanAutoChangePage;
    }
#endif

    void createBasicPage();

#ifdef UNLOAD_PI
    void unloadPi(IPluginInf *pi);
#endif
    QGenieButtonStackViewController*mHomeBut;
protected:
    QGenieFrameStackView *mStackView;
    QWidget *createStackView(int type);
  //  QWidget *createTestView(int type);
  //  QList<QWidget *> mPages;
    int mPageNum;
    QGeniePageMovie *mMovieView;
    QGeniePageHome *mHomeView;
    QGenieButtonStackViewController *createButton(QWidget *parent,QString text);
//    QGenieButtonStackViewController *createHomeButton(QWidget *parent);

    void registPluginWidget(QGeniePagePluginProxy *w,IPluginInf *pi);

    //void realRegistPluginProxy();
  //
#ifdef CAN_LOCK_STACK
    bool mCanAutoChangePage;
#endif


    QWidget * mPageCur;

    QWidget * mPageNext;
    QWidget * mPageLast;
    bool mMovieOn;


public:
    QWidget *getCurView();
    QWidget *getNextView();
    void realChangeStackViewIdx();
    bool mInProcess;
protected slots:
    void slot_NotifyRaiseOrLower();
};

#endif // QSTACKMANAGER_H
