#ifndef QGENIELAUNCHINF_H
#define QGENIELAUNCHINF_H

#include "genie2_interface.h"
#include <QObject>
#include "Singleton.h"
#include <QSplashScreen>


//class QGenieFrameMain;
class QGenieAllSingletonClasses;

class QGenieSplashManager:public QObject,public Singleton<QGenieSplashManager>
{
    Q_OBJECT
public:
    QGenieSplashManager();
    ~QGenieSplashManager();
//    void setRouterModel(const QString &model);
    void showSplash();
    void hideSplash();
protected slots:
//    void slot_updateImage();
protected:
//    QTimer mTimer;
    QSplashScreen *mSplash;


};


class QGenieLaunchInf:public QObject,public IPluginInf2,public Singleton<QGenieLaunchInf>
{
    Q_OBJECT
    Q_INTERFACES(IPluginInf2)
    Q_PLUGIN_METADATA(IID "com.netgear.GeniePlugin.IPluginInf2/1.0" FILE "genie_module.json")
public:
    QGenieLaunchInf();
    ~QGenieLaunchInf();
    QVariant command(int cmd,const QVariant &paras);
//    void showSplash();

    QGenieSplashManager *mSplashM;



//    virtual void setLaunchManager(ILaunchManager *manager);
//    virtual void startup();
//    virtual void endup();
//    virtual QString getInfo(QString para);
//    virtual QString command(QString para);
//    virtual QString getMultiLanText(int idx);
//    static ILaunchManager *sLaunchManager;

protected:
    QGenieAllSingletonClasses *mAllSingleClasses;




};

#endif // QGENIELAUNCHINF_H
