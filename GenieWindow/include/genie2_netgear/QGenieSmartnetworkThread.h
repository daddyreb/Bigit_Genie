#ifndef QGENIESMARTNETWORKTHREAD_H
#define QGENIESMARTNETWORKTHREAD_H
#include "INetgearInterface.h"
#include "SvtNetworkTool.h"
#include <QThread>
extern QMap<QString,QString> sNullMap;
class QGenieSmartnetworkThread : public IThreadInterface
{
    Q_OBJECT
public:
    explicit QGenieSmartnetworkThread(QGenieNetgearInterface *inf);
    ~QGenieSmartnetworkThread();
    bool canProcess(int cmd);
protected:
    void doCmdLogin(const QStringList &reqlist,QStringList &retlist);
    void doCmdWireless(const QStringList &reqlist,QStringList &retlist);
    void doCmdSetWireless(const QStringList &reqlist,QStringList &retlist);
    void doCmdGacc(const QStringList &reqlist,QStringList &retlist);
    void doCmdSetGacc(const QStringList &reqlist,QStringList &retlist);
    void doCmdTMOption(const QStringList &reqlist,QStringList &retlist);
    void doCmdTMStatistics(const QStringList &reqlist,QStringList &retlist);
    void doCmdTMSetOption(const QStringList &reqlist,QStringList &retlist);
    void doCmdTMSetEnable(const QStringList &reqlist,QStringList &retlist);
    void doCmdFirmware(const QStringList &reqlist,QStringList &retlist);
    void doCmdLogout(const QStringList &reqlist, QStringList &retlist);

    void doCmdIs5GOk(const QStringList &reqlist, QStringList &retlist);
    void doCmdGet5GInfo(const QStringList &reqlist, QStringList &retlist);
    void doCmdSet5GInfo(const QStringList &reqlist, QStringList &retlist);
    void doCmdGet5GGaccInfo(const QStringList &reqlist, QStringList &retlist);
    void doCmdSet5GGaccInfo(const QStringList &reqlist, QStringList &retlist);
    void doCmdAttachDevice(const QStringList &reqlist, QStringList &retlist);

//    void blockForAuthData();
    const QDomNode &doFunc(const QString &cp, const QString &model, const QString &func
                           ,int *error=NULL,QString *rets=NULL,
                             const QMap<QString,QString> &paras=sNullMap);


//signals:
//    void signal_waitAuthenticate();
//public slots:
//    void slot_waitAuthenticate();
protected:
    void getNetgearData(int cmd,const QStringList &reqlist,QStringList &retlist);
    SVT_Fcml mFcml;
    QString mUserName;
    QString mPassword;
//    QMutex mCondisionMutex;
//    QWaitCondition mWaitCondition;
    friend class QGenieNetgearInterface;



};

#endif // QGENIESMARTNETWORKTHREAD_H
