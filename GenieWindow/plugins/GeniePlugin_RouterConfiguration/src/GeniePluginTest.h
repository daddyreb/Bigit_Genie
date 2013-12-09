#ifndef GENIEPLUGINTEST_H
#define GENIEPLUGINTEST_H

#include <QObject>
#include "IPluginInf.h"
#include "mainwindow.h"
#include "QGenieMsgDialog.h"
#include "QRCManagerHandler.h"
#include "Singleton.h"

#ifdef ENABLE_READYSHARE
#include "QGeniePlugin_Readyshare.h"
#endif

class GeniePlugin_RouterConfiguration : public QObject,public IPluginInf,public Singleton<GeniePlugin_RouterConfiguration>
{
    Q_OBJECT
    Q_INTERFACES(IPluginInf)
public:
    GeniePlugin_RouterConfiguration();
    virtual ~GeniePlugin_RouterConfiguration();

    enum SUBPLUGINS
    {
      SP_EXPORT,
      SP_TRAFFICMETER,
      SP_ROUTERUPDATE,
#ifdef ENABLE_READYSHARE
      SP_READYSHARE,
#endif
      SP_NUM
    };

    QPluginLoader mLoaders[SP_NUM];
    IPluginInf * mPlugins[SP_NUM];
    QWidget* loadSubPluginWidget(SUBPLUGINS type);
    void unloadAllSubPlugins();

    MainWindow* mW;
    QGenieMsgDialog *mDialog;
public:
    virtual QString text();
    virtual QWidget* widget(QWidget *parent);
    virtual QStringList bigButtonParas();
    virtual void notifyBigButton(QString s);
    virtual QIcon  *icon();
    virtual QString  info();
    virtual int priority();
    virtual int changelanguage(int idx);
    virtual void setHandler(IManagerHandler *hdl);
    virtual void startup(bool mini);
    virtual void notifyRaise();
    virtual void notifyLower();
    //virtual int getStatus();
    static IPluginInf *sSinglePtr;
    virtual QString getInfo(int para);
    virtual void notifyTrayMessage(QSystemTrayIcon::ActivationReason reason,MAIN_FRAME_RESPONSE res);
   // virtual int getStatus(QString notifyPara);
    virtual void command(int cmd, int para);
    virtual void *getPoint(int para);
    virtual void command(int cmd,QString para0="",QString para1="");
    QRCManagerHandler mManagerHandler;

#ifdef ENABLE_READYSHARE
protected:
    bool                m_bgo_readyshare;
    QGenieReadyShare    *m_inner_readyshare_plugin;
public:
    QString getReadyShareUuid();
#endif
};

#endif // GeniePluginTest_H
