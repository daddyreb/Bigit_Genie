/*-------------------------------------
  @author jianghua
  *this class is create system tray ico
  *class Function
   main window hide , this application show tray icon
 -----------------------------------------*/

#ifndef QGENIETRAYMANAGER_H
#define QGENIETRAYMANAGER_H
#ifndef IN_STABLE_H
#include <QObject>
#include "Singleton.h"
#include <QSystemTrayIcon>
#include <QMenu>
#include <QTimer>
#include "QGenieDialogExit.h"
#endif

class IPluginInf;

class IGenieTrayIco{
public:
    virtual ~IGenieTrayIco(){};
    virtual void startMovie()=0;
    virtual void stopMovie()=0;
    virtual void setToolTip(const QString &tooltip)=0;
    virtual void setMessage(const QString &msg)=0;
    virtual void setIcon(const QString &ico)=0;
    virtual void retranslateUi(const QStringList &texts)=0;
    virtual void addPluginAction(QAction *act)=0;
    virtual void setLayoutDirection(Qt::LayoutDirection drt)=0;
    virtual QAction* addPluginMenu(QMenu *menu)=0;
    enum _TRAY_PARA
    {
        AUTO_START_CHECKED,
        SHOW_NETSTATE_CHECKED,
        _TRAY_PARA_NUM
    };

    virtual void setActionState(const QMap<int ,bool> &para)=0;
    virtual void exit()=0;
};

class QGenieTrayManager :public QObject, public Singleton<QGenieTrayManager>
{
    Q_OBJECT
public:
    QGenieTrayManager();
    ~QGenieTrayManager();
 //   QSystemTrayIcon *getiTrayIcon() const {return iTrayIcon;}  //send QSystemTrayIcon object

//    void setTrayIcon(QString iconFile);                 //change Icon
 //   void setToolTipText(QString textStr);               // set Tooltip Text
    void setMessage(const QString &strText);  // set tray icon message

    void setToolTipText(int idx);               // set Tooltip Text
//    void setMessage(QString strTitle,int idx);  // set tray icon message
//    void hide();
//    void show();
    void exit();
    void showMsg(QString textTitle,QString textStr,QSystemTrayIcon::MessageIcon ico);
    void setIcon(QString sIconPath);
    void startService(QWidget *parent,QString iconFile);
    void setLayoutDirection(Qt::LayoutDirection drt);

    void changeText();
    void unloadPi(IPluginInf *pi);
    void startMovie();
    void stopMovie();


private :
    IGenieTrayIco *mTrayIco;

 //   QString iconFilePath;
  //  QString toolTipText;
//    QString msgTitle,msgText;

    QWidget *mMainWnd;
 //   QTimer mTimer;


//used for QGenieInterfaceTrayManager
    friend class QGenieInterfaceTrayManager;
    void addPluginAction(QAction *act);
    QAction* addPluginMenu(QMenu *menu);
protected:

    int mCurToolTip;
    int mCurMessage;
#ifdef TRAY_MESSAGEBUG
protected:
    QTimer mMBTimer;
    QTimer mMBNoMessageNow;
    QDateTime mMBStamp;
    void MBCheckOSRestarted();
#endif



//    QString mIconStrCache;
    void trayIconClicked(QSystemTrayIcon::ActivationReason reason);

public slots:
    void slot_SetAutoStart(bool b);
    void slot_SetShowNetworkChange(bool b);
//#ifdef TRAY_MESSAGEBUG
//    void slot_MBInterval();
//#endif
//    void slot_ExitApp();
//    void slot_HiMessage();
     // void slot_UpdateNetState();
    void startUpdate();
//    void slot_AboutToShow();
#ifdef SELF_TRAY_BOX
//    void slot_RealShowMessageCache();
//    void realShowMessage(const QString &msg);
#endif
    friend class QGenieSelfTrayMessage;
    friend class QGenieTrayMovieManager;
    friend class QGenieTrayIco_In;
    friend class QGenieTrayIco_Out;

};

#endif // QGENIETRAYMANAGER_H
