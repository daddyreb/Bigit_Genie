#ifndef QGENIETRAYICO_OUT_H
#define QGENIETRAYICO_OUT_H

#ifndef IN_STABLE_H
#include <QObject>
#include <QtGui>
#include <QCheckBox>
#include "QGenieTrayManager.h"
#include <QLocalSocket>
#endif
class QLocalSocket;

class QGenieTrayIco_Out:public QObject,public IGenieTrayIco,
    public Singleton<QGenieTrayIco_Out>
{
    Q_OBJECT
public:
    QGenieTrayIco_Out(QGenieTrayManager *traymanager);
    ~QGenieTrayIco_Out();
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
    void startup();
    void sendCommand(const QString &cmd);
    void parseCommand(const QString &cmd);
    void startUpdate();
    void showNormal();
    QGenieTrayManager *mTrayManager;

protected:
//    QSystemTrayIcon *mTrayIcon;
//    QMenu *mTrayMenu;
//    QAction *mQuitAction;
//    QAction *mPluginMenuHere;
//    QAction *mUpdateAction;
//    QAction *mRestoreAction;

//    QTimer mMessageTimer;
//    QGenieSelfTrayMessage *mSelfTrayMessage;
    QGenieDialogExit *mDialogExit;
    QLocalSocket *mLocalSocket;
    QStringList mCommandList;
    QTimer mStartProcessTimer;
//    QGenieTrayMovieManager *mMoviewManager;
//    QString mIcoCache4Movie;
    QString mTranslateStr;
    bool mOn;

protected slots:
      void slot_StartProcess();
      void slot_Connected();
      void slot_Disconnected();
      void slot_ReadyRead();
      void slot_Timeout();
      void displayError(QLocalSocket::LocalSocketError socketError);

//    void slot_TrayIconClicked(QSystemTrayIcon::ActivationReason reason);
//    void slot_ExitApp();
//    void slot_startUpdate();
//    void slot_AboutToShow();

//    void slot_MessageTimeout();

    friend class QGenieTrayManager;
	friend class QGenieTrayMovieManager;
};

#endif // QGenieTrayIco_Out_H
