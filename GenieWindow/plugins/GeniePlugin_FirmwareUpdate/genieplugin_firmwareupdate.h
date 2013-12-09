#ifndef GENIEPLUGIN_FIRMWAREUPDATE_H
#define GENIEPLUGIN_FIRMWAREUPDATE_H

#include <QObject>
#include "IPluginInf.h"
#include "ILogManager.h"

class FirmwareUpdate;
class IInfoManager;


class GeniePlugin_FirmwareUpdate : public QObject,public IPluginInf
{
    Q_OBJECT
    Q_INTERFACES(IPluginInf)

public:
    GeniePlugin_FirmwareUpdate();
    ~GeniePlugin_FirmwareUpdate();

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
    //virtual void startup();
    virtual void startup(bool mini);
    virtual void notifyRaise();
    virtual void notifyLower();
    //virtual int getStatus();
//    virtual int getStatus(QString notifyPara);
    static IPluginInf *sSinglePtr;
    virtual void notifyTrayMessage(QSystemTrayIcon::ActivationReason reason,MAIN_FRAME_RESPONSE res );


//    virtual QString getInfo(QString para);
//    virtual void command(QString cmd, QString para0, QString para1);
//    //virtual void command(QString cmd, QString para);
//    virtual void command(QString cmd, int para);
//    virtual void *getPoint(QString para);

    virtual QString getInfo(int para);
    virtual void command(int cmd, QString para0, QString para1);
    virtual void command(int cmd, int para);
    virtual void *getPoint(int para);



    static void writeGenieLog(QString log);

protected:
    static ILogManager *mLogHandler;

private:
    QString mProduct;
    bool setFlag;
    QString oldNetState;
    QString newNetState;
    FirmwareUpdate *wUpdate;
    IManagerHandler *mHandler;

    IInfoManager *mInfoHandler;

};

#endif // GENIEPLUGIN_FIRMWAREUPDATE_H
