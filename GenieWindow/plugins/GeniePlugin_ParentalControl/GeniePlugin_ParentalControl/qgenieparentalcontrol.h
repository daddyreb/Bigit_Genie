#ifndef QGENIEPARENTALCONTROL_H
#define QGENIEPARENTALCONTROL_H

#include <QObject>
#include "inc/genie_interface/IPluginInf.h"
#include "pcpform.h"
#include "date.h"

class QGenieParentalControl : public QObject,public IPluginInf
{
    Q_OBJECT
    Q_INTERFACES(IPluginInf)
public:
    QGenieParentalControl();
    virtual ~QGenieParentalControl();

public:
   /*
     virtual QString text();
     virtual QWidget* widget(QWidget *parent);
     virtual QStringList bigButtonParas();
     virtual void notifyBigButton(QString s);
     virtual QIcon * icon();
     virtual QString  info();
     virtual int priority();
     virtual int changelanguage(int l);
     virtual void setHandler(IManagerHandler *hdl);
     virtual void startup(); //windowless plugin use this
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
    static IPluginInf *sSinglePtr;
    virtual void notifyTrayMessage(QSystemTrayIcon::ActivationReason reason,MAIN_FRAME_RESPONSE res);
    virtual QString getInfo(QString para);

    virtual void command(QString cmd,QString para0="",QString para1="");
    virtual void command(QString cmd, int para);
        virtual void *getPoint(QString para);
};

#endif // QGENIEPARENTALCONTROL_H
