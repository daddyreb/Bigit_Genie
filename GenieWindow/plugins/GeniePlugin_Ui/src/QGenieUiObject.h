#ifndef QGENIEUIOBJECT_H
#define QGENIEUIOBJECT_H

#include <QObject>
#include <QtGui>
#include <QtWidgets>
#include "genie2_interface.h"
#include "Singleton.h"


class GeniePlugin_Ui;
class _tMoveData;
class _tLinkData;
class QGenieButtonBig;
class QGenieTrayManager;
class QGenieTitleUpdate;

class _tLinkData
{
public:
    enum UI_PARAS{
        UP_BUTLINE_MODE
                ,UP_BUTLINE_PARA1
                ,UP_BUTLINE_PARA2
                ,UP_ICO
                ,UP_BBT_LANGUAGE
                ,UP_LEFT_LANGUAGE
                ,UP_NUM
            };


    _tLinkData();
    short priority;
    short uuid_idx;
    QWidget *stackW;
    QPushButton *leftBut;
    QGenieButtonBig *bbt;
    QString cmd;
    QVariant uiPara[UP_NUM];
};
#ifdef USE_DIRTY_FOCUS_CLEARER
class QGenieDirtyFocusFrameClearer:public QObject
{
    Q_OBJECT
    QGenieDirtyFocusFrameClearer();
    ~QGenieDirtyFocusFrameClearer();
    void startRefresh();
    void stop();
protected:
    enum STATE
    {
        S_GETWINDOW
        ,S_SETFOCUS_BACK
        ,S_END
    };

    QTimer mTimer;
    QWidget *mFocusW;
    int mState;
protected slots:
    void slot_TimeOut();
    void slot_WidgetDistroyed(QObject *o);
    friend class QGenieUiObject;
};
#endif

class QGenieAdvStateMachine;

class QGenieUiObject : public QObject,public Singleton<QGenieUiObject>
{
    Q_OBJECT
public:
    explicit QGenieUiObject(QObject *parent=0);
    ~QGenieUiObject();
    void enableMove(QWidget *w);
    void registerUi(const QVariant &paras);

    void deployUi();
    void createMainFrame();
    void showMainFrame();
    void changeLanguage();
    void initlanguageSetting();
    QVariant parseUiCmd(const QVariant &para_list);
    void checkOtherPlugin(const QVariant &para_list);

    //if already checked ,return false
    bool checkBBt(_tLinkData *t);
    //    void _createTrayIco();

    /////update
    void dealUpdateCmd(const QVariant &paras);

    void unlockSM4Title(QPushButton *but_appstore=NULL,QWidget *head=NULL );
    void unlockSM4Splash();
    ////wifi
    void setWifiName(const QString &ssid);
    ////trayMessage
    void showTrayMessage(int cmd, const QVariant &para_list);
    void setNetState(int state);
    QString mWifiType;
    QPushButton *mGetPwd;
//    QGenieAdvStateMachine *mAdvStateMachine;
        QString mCurLan;
#ifdef USE_DIRTY_FOCUS_CLEARER
        QGenieDirtyFocusFrameClearer mDirtyFocusClearer;
#endif

protected:

    QMap<QWidget *,_tMoveData *> mMoveWidgetData;
    QMap<short,_tLinkData *> mLinkData;
    QMap<short,QList<_tLinkData *> > mLinkData2;
    virtual bool eventFilter(QObject *, QEvent *);
    QWidget *mMainFrame;
    QStackedWidget *mStackWidget;
    QWidget *mLeftPan;
    QWidget *mHomePage;

	//LQ
	QWidget *mBottomPan;
	QWidget *mRightMargin;
	QWidget *mLeftMargin;
	//

    QComboBox *mLanComb;
    QPushButton *mHomeBut;
//    QLineEdit *mSearchEdt;
//    QPushButton *mGo;
//    QWidget *mAdvW;
    QGenieButtonBig *mBbts[6];

    QString mCurLink;
    QButtonGroup mLeftButGroup;
    _tLinkData *mCurrentLinkData;


    QPushButton *mUpdateBut;
    QGenieTitleUpdate *mTitleUpdate;


	
    ///wif
    QLabel *mWifi;
    ///language
    QLabel *mLanSel;


//    QWidget *mTitleBut;
    QTimer mUrlTimer;



#ifdef GENIE2_TRAY
    QGenieTrayManager *mTrayManager;
    void _translateIco();

protected slots:
    void slot_trayActionTrigged(const QString &objname);
#endif
protected:
    void _setFramelessStyle(QWidget *w);
    void _doDBG();
    bool _showModalDialog(const QVariant &para, bool warring=false);
    void _showAboutDialog();

#ifdef USE_DBG_STACK
    quint16 mSavedData;
    void tryInitDbgPan();
    QStackedWidget *mDbgStack;
public:
    void registerDBGUi(const QVariant &paras);
#endif



public slots:
    void slot_Exit();
    void slot_MovableWidgetDestroyed(QObject *);
    void slot_LinkButClicked();
    void slot_LanChanged(int i);
    void slot_ChangeAdvPic(QString filepath,QString link);
    void slot_StartSearch();
    void slot_ButClicked();
    void slot_SaveSplash();

    void slot_but_static();
    void slot_ReloadQss();
#ifdef GENIE2_TRAY
    void slot_InitTray();
#endif
    void slot_updateIcon();
    friend class AppMenuHandler;

};

#endif // QGENIEUIOBJECT_H
