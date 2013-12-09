#ifndef QGENIETRAYICO_IN_H
#define QGENIETRAYICO_IN_H
class QGenieSelfTrayMessage;
class QAction;

#ifndef IN_STABLE_H
#include <QObject>
#include <QtGui>
#include <QCheckBox>
#include "QGenieTrayManager.h"
#endif
class QGenieTrayMovieManager;


class QGenieTrayIco_In:public QObject,public IGenieTrayIco
{
    Q_OBJECT
public:
    QGenieTrayIco_In(QGenieTrayManager *traymanager);
    ~QGenieTrayIco_In();
    virtual void startMovie();
    virtual void stopMovie();
    virtual void setToolTip(const QString &tooltip);
    virtual void setMessage(const QString &msg);
    virtual void setIcon(const QString &ico);
    virtual void retranslateUi(const QStringList &texts);
    virtual void exit();
    virtual void addPluginAction(QAction *act);
    void setLayoutDirection(Qt::LayoutDirection drt);
    virtual QAction* addPluginMenu(QMenu *menu);
    virtual void setActionState(const QMap<int ,bool> &para);
protected:
    void startup(QGenieTrayManager *traymanager);
    QGenieTrayManager *mTrayManager;
protected:
    QSystemTrayIcon *mTrayIcon;
    QMenu *mTrayMenu;
    QAction *mQuitAction;
    QAction *mPluginMenuHere;
    QAction *mUpdateAction;
    QAction *mRestoreAction;

    QMenu *mSettingSubMenu;
    QAction *mShowNetworkChangeAction;
    QAction *mAutoStartAction;

    QTimer mMessageTimer;
#ifdef SELF_TRAY_BOX
    QGenieSelfTrayMessage *mSelfTrayMessage;
#endif
    QGenieDialogExit *mDialogExit;

    QGenieTrayMovieManager *mMoviewManager;
    QString mIcoCache4Movie;

protected slots:
    void slot_TrayIconClicked(QSystemTrayIcon::ActivationReason reason);
    void slot_ExitApp();
    void slot_startUpdate();
    void slot_AboutToShow();
    void slot_ShowNormal();
    void slot_MessageTimeout();

    friend class QGenieTrayManager;
	friend class QGenieTrayMovieManager;
};

#endif // QGENIETRAYICO_IN_H
