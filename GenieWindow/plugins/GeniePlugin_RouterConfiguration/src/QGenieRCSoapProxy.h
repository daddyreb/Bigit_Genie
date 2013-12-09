#ifndef QGENIERCSOAPPROXY_H
#define QGENIERCSOAPPROXY_H

#include <QObject>
#include <QUuid>

class ISoapClient;

enum ENUM_SOAP_CMD
{
    ESC_LOGIN_PWD=0,//login
    ESC_INFO_SECURITY_KEY,
    ESC_ROUTE_INFO,

//    ESC_ROUTE_MAC_AFTAPPLY,
    ESC_APPLY_CHANGES_NOSECURE,
    ESC_APPLY_CHANGES_WITHSECURE,
    ESC_GUESTACCESS_ISENABLE,
    ESC_GUESTACCESS_INFO,
    ESC_GUESTACCESS_SETENABLE,
    ESC_GUESTACCESS_SETINFO2,
    ESC_GUESTACCESS_SETINFO,
    ESC_GUESTACCESS_ISENABLE_BEFORE_DISABLE_IT,
    ESC_GUESTACCESS_ISMACOK_BEFORE_DISABLE_IT,



    ESC_NUM

};
#include "qtsoap.h"
#include "QGenieRCPageWireless.h"

#include "netgearsoapschedule.h"
#include "QGenieRCPageLogin.h"
#include "Singleton.h"

class QGenieRCPageWireless;
//class WirelessInfo;
class WirelessSettings;




class QGenieRCSoapProxy : public QObject,public Singleton<QGenieRCSoapProxy>
{
Q_OBJECT
public:
    explicit QGenieRCSoapProxy(QObject *parent = 0);
    ~QGenieRCSoapProxy();
    void command(int cmd,QMap<QString,QString> *paras=NULL);
    void ignoreAllRequest();
    void ignoreRequest(int cmd);
//    void commitUseSelfClass(QString &modulename, QString &methodname, QMap<QString, QString> &params);
#ifdef SUBMIT_FINISH_AFTER_REBOOT
    void submitFinishCommandAfterReboot();
#endif
//    QTimer mSelfCommitTimer;
    QString mSelfCommitMName;
    QString mSelfCommitMethod;
    QMap<QString,QString> mSelfCommitParas;

protected:
    QGenieRCPageWireless *mWireless;
    NetgearSoapSchedule *mSoapApi;
    QGenieRCPageLogin *mLogin;
    QString mCurType[ESC_NUM];
    ISoapClient *getSoapClient(const QString &id,int *cmd=NULL);
//    NetgearSoapAPI *mSelfSoap80;
//    NetgearSoapAPI *mSelfSoap5000;
signals:

public slots:
    void slot_GetSoapResponse(QtSoapMessage&,QUuid&,QString&);
    void slot_SoapError(QUuid&,QString&);
  //  void slot_selfCommitTimeout();

};

#endif // QGENIERCSOAPPROXY_H
