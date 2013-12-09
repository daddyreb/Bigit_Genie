#ifndef GENIEPLUGINTEST_H
#define GENIEPLUGINTEST_H

#include <QObject>
#include "IPluginInf.h"
#include "mainwindow.h"

class GeniePluginTest : public QObject,public IPluginInf
{
    Q_OBJECT
    Q_INTERFACES(IPluginInf)
public:
    GeniePluginTest();
    virtual ~GeniePluginTest();

    MainWindow *w;

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
//    virtual int getStatus(QString notifyPara);
    virtual QString getInfo(int para);
   // virtual void command(QString cmd,QString para);
    virtual void command(int cmd, int para);
    virtual void *getPoint(int para);
    virtual void command(int cmd,QString para0="",QString para1="");

private:
    static bool loginflag;

};

#endif // GeniePluginTest_H
