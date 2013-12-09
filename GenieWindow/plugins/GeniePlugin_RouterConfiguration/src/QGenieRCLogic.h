#ifndef QGENIERCLOGIC_H
#define QGENIERCLOGIC_H
class QGenieRCPageGuessAccess;
class QGenieRCLogic;
class QGenieRCPageLogin;
class QGenieRCPageWireless;
class MainWindow;
class GeniePlugin_RouterConfiguration;
class QGenieRCSoapProxy;
#include <QtCore>
enum PENDING_CMD
{
        SLP_NONE=0,
        SLP_GETPWD=(1<<0),
        SLP_CHANGE_CHANNEL=(1<<1),
        SLP_FIRMWARE_UPDATE=(1<<2),
        SLP_TURNTOGACC=(1<<3),
        SLP_LOGOUT=(1<<4),
        SLP_DISABLE_GACC=(1<<5),
        SLP_LOGIN=(1<<6),
        SLP_SUBMIT_SOAP_FINISH=(1<<7),
#ifdef ENABLE_READYSHARE
        SLP_TURNTOREADYSHARE = (1 << 8),
#endif
        SLP_ALL=(0xffffffff)

};

class QGenieRCLogic
{
public:
    QGenieRCLogic();
    void parseCmd(int cmd,const QString &para0,const QString &para1);
    void parseCmd(int cmd,const int &para);
//    void whenLogIn();
    void loginProcessEnd(bool ok,int reson,bool forcestop=false);

    bool isRefreshing();
    bool isLogin();
    bool isInApplyProcess();
    bool processPendingCmd(const quint32 &flag=SLP_ALL);
    void logout(bool toframehome=false);
    void writeLog(const QString &str);
    void limitTextLength();

    void applyStart();
    void tryApplyEnd();
    QString getDisableTimerStatus();
    static quint32 mPendingCmd;
#ifdef ENABLE_READYSHARE
    void turnto_wirelesssettings();
#endif
protected:
    bool mInitOk;
    static bool sInApplyProcess;
    static QString sRouteType;
    MainWindow *mMain;
    QGenieRCPageGuessAccess *mGacc;
    QGenieRCPageWireless *mWireless;
    QGenieRCPageLogin *mLogin;
    GeniePlugin_RouterConfiguration *mInf;
    QGenieRCSoapProxy *mSoapProxy;

};

#endif // QGENIERCLOGIC_H
