#ifndef GENIEPLUGIN_TESTUI_H
#define GENIEPLUGIN_TESTUI_H

#include <QObject>
#include "IPluginInf.h"
#include "ILanguageManager.h"
//#include "LanguageInfo.h"
#include "ISettings.h"
#include "ILogManager.h"
#include "IUiManager.h"


class GetInformation;


class GeniePlugin_Info : public QObject,public IPluginInf
{
    Q_OBJECT
    Q_INTERFACES(IPluginInf)
public:
    GeniePlugin_Info();
    virtual ~GeniePlugin_Info();
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
//    virtual int getStatus(QString notifyPara);
    virtual void notifyTrayMessage(QSystemTrayIcon::ActivationReason reason,MAIN_FRAME_RESPONSE res);


//    virtual QString getInfo(QString para);
//    virtual void command(QString cmd, QString para0, QString para1);
//    virtual void command(QString cmd, int para);
//    virtual void *getPoint(QString para);

    virtual QString getInfo(int para);
    virtual void command(int cmd, QString para0, QString para1);
    virtual void command(int cmd, int para);
    virtual void *getPoint(int para);


    static QString translateText(int idx);
    static void writeLog(QString log);

protected:
    static ILanguageManager *mlanguagemanager;
    static ILogManager * mLogHandler;

private slots:
    void StopGetInfo();


private:
    bool startFalg;
    QString oldNetState;
    QString newNetState;

    IUiManager *mUimanager;

    GetInformation *mGenieInfo;

};

#endif // GeniePlugin_TestUi_H
