#ifndef QGENIEINTERFACETRAYMANAGER_H
#define QGENIEINTERFACETRAYMANAGER_H

#include <QObject>
#ifndef IN_STABLE_H
#include <QObject>
#include <QDialog>
#include <QPoint>
#include "Singleton.h"
#include "ITrayManager.h"
#include <QTimer>
#endif
class ITrayManager;



class QGenieInterfaceTrayManager:public QObject,public Singleton<QGenieInterfaceTrayManager>
            ,public ITrayManager
{
    Q_OBJECT
public:
    QGenieInterfaceTrayManager(QObject *parent=NULL);
    ~QGenieInterfaceTrayManager();
    virtual void setMessage(QString textTitle,QString textStr,QSystemTrayIcon::MessageIcon ico);
    virtual void setAction(QAction *act,QUuid uuid);
    virtual void hideWindow();
    virtual void showWindow();
    virtual void exitApp();
    virtual void addMenu(QMenu *menu,QUuid uuid);
    virtual void setMessageFlag(IPluginInf *pi,bool b=true);//when plugin want the main frame notify it when tray message,call this
    virtual void setTrayStateIcon(E_TRAY_STATUS state,QString selficon="");
    virtual void startMovie(MOVIE_IDX idx=MI_REPAIR,int msec=0);
    virtual void stopMovie();
    QSet<IPluginInf *> mPluginInfs;    //this plugin inf can receive tray message
    void unloadPi(IPluginInf *pi);
    void notifyTrayMessage(QSystemTrayIcon::ActivationReason reason,MAIN_FRAME_RESPONSE res);
    int mCurState;
    QTimer mTimer;
protected slots:
    void slot_updateIcon();
};

#endif // QGENIEINTERFACETRAYMANAGER_H
