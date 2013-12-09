#ifndef QGENIEREADYSHARE_H
#define QGENIEREADYSHARE_H

#include <QObject>
#include "inc/genie_interface/IPluginInf.h"
#include "pcpform.h"
#include "date.h"
#include <QStackedLayout>


class QGenieReadyShare : public QObject,public IPluginInf
{
    Q_OBJECT
    Q_INTERFACES(IPluginInf)
public:
    QGenieReadyShare();
    virtual ~QGenieReadyShare();

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
//    virtual int getStatus(QString notifyPara);
    static IPluginInf *sSinglePtr;
    virtual void notifyTrayMessage(QSystemTrayIcon::ActivationReason reason,MAIN_FRAME_RESPONSE res);
    virtual QString getInfo(int para);

    virtual void command(int cmd,QString para0="",QString para1="");
    virtual void command(int cmd, int para);
        virtual void *getPoint(int para);
};

#endif // QGENIEREADYSHARE_H
