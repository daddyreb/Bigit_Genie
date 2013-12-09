#ifndef GeniePlugin_About_H
#define GeniePlugin_About_H

#include <QObject>
#include "IPluginInf.h"
#include <QSystemTrayIcon>


class GeniePlugin_About : public QObject,public IPluginInf
{
    Q_OBJECT
    Q_INTERFACES(IPluginInf)
public:
    GeniePlugin_About();
    virtual ~GeniePlugin_About(){}
public:
    virtual QString text();
    virtual QWidget* widget(QWidget *parent);

    virtual QIcon  *icon();
    virtual QString  info();
    virtual int priority();
    virtual int changelanguage(int idx);
    virtual void setHandler(IManagerHandler *hdl);
    virtual void startup();

    virtual void notifyRaise();		//when plugin raise to top of stack,plugin may catch it to start/stop some processes
    virtual void notifyLower();		//opposite to notifyRaise
    virtual int getStatus();			//return the status of plugin,refer to ENUM_PLUGIN_STATUS
    virtual void notifyTrayMessage(QSystemTrayIcon::ActivationReason reason,MAIN_FRAME_RESPONSE res);



};

#endif // GeniePlugin_About_H
