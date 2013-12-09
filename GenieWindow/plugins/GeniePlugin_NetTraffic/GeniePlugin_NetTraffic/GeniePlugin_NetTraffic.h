#ifndef GENIEPLUGIN_NETTRAFFIC_H
#define GENIEPLUGIN_NETTRAFFIC_H


#include <QObject>
#include "IPluginInf.h"
#include "date.h"
class QGenie_qtNetTraffic;

//网络流量显示	Network Traffic	0	2000	{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}	0	邓治国


class GeniePlugin_NetTraffic : public QObject,public IPluginInf {
    Q_OBJECT
    Q_INTERFACES(IPluginInf)
public:
            GeniePlugin_NetTraffic();
            virtual ~GeniePlugin_NetTraffic();
/*
            virtual QString text();
            virtual QWidget* widget(QWidget *parent);
            virtual QStringList bigButtonParas();
            virtual void notifyBigButton(QString s);

            virtual QIcon  *icon();
            virtual QString  info();
            virtual int priority();
            virtual int changelanguage(int idx);
            virtual void setHandler(IManagerHandler *hdl);
            virtual void startup();
            virtual void notifyRaise();
            virtual void notifyLower();
            virtual int getStatus(QString notifyPara);
            virtual QString getInfo(QString para);
            virtual void notifyTrayMessage(QSystemTrayIcon::ActivationReason reason,MAIN_FRAME_RESPONSE res);
*/
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
            virtual int getStatus(QString notifyPara);

            virtual void notifyTrayMessage(QSystemTrayIcon::ActivationReason reason,MAIN_FRAME_RESPONSE res);
            virtual QString getInfo(QString para);

            virtual void command(QString cmd,QString para0="",QString para1="");
            virtual void command(QString cmd, int para);
            virtual void *getPoint(QString para);



public:
       QGenie_qtNetTraffic *w;
       static IPluginInf *sSinglePtr;


};

#endif // GENIEPLUGIN_NETTRAFFIC_H
