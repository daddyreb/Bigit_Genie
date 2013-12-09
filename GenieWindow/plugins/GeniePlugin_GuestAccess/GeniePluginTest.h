#ifndef GENIEPLUGINTEST_H
#define GENIEPLUGINTEST_H

#include <QObject>
#include "IPluginInf.h"
#include "mainwindow.h"

class GeniePlugin_GuestAccess : public QObject,public IPluginInf
{
    Q_OBJECT
    Q_INTERFACES(IPluginInf)
public:
    GeniePlugin_GuestAccess();

    MainWindow* w;

    virtual ~GeniePlugin_GuestAccess();
public:
    virtual QString text();
    virtual QWidget* widget(QWidget *parent);

    virtual QIcon  *icon();
    virtual QString  info();
    virtual int priority();
    virtual int changelanguage(int idx);
    virtual void setHandler(IManagerHandler *hdl);
    virtual void startup();
    virtual void notifyRaise();
    virtual void notifyLower();
    virtual int getStatus();
    static IPluginInf *sSinglePtr;
    virtual void notifyTrayMessage(QSystemTrayIcon::ActivationReason reason,MAIN_FRAME_RESPONSE res);

    virtual QStringList bigButtonParas();
    virtual void notifyBigButton(QString s);
    virtual void startup(bool mini);
    virtual int getStatus(QString notifyPara);
    virtual QString getInfo(QString para);
    virtual void command(QString cmd,QString para);
    virtual void command(QString cmd, int para);
    virtual void *getPoint(QString para);
    virtual void command(QString cmd,QString para0="",QString para1="");
};

#endif // GeniePluginTest_H
