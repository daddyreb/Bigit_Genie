#ifndef PCPFORM_H
#define PCPFORM_H

#include <QWidget>
#include <QWidget>
#include <QLabel>
#include <QFrame>
#include <QTimer>

#include <QResizeEvent>
#include <QFocusEvent>
#include <QShowEvent>
#include <QPushButton>

#include <QStackedLayout>
#include <QVBoxLayout>

#include <QHostInfo>

#include "QGenieRSBasic.h"
#include "QGenieRSRemote.h"
#include "QGenieRSPrint.h"
//#include "QGeniePlugin_Remote_Unregister_ReadyShare.h"

#include "Singleton.h"

#include "QGenieRSManager.h"
class _HostInfoHelper;

class _HostInfoHelper:public QThread
{
    Q_OBJECT
public:
    _HostInfoHelper();
    ~_HostInfoHelper();
    void getDNS(const QString &path);

    QHostInfo getHostInfo();

    void run();
protected:
    QString mPath;
    QMutex mMutex;
    QHostInfo mHostInfo;
};


class PCPForm : public QWidget,public Singleton<PCPForm>
{
    Q_OBJECT
public:
    PCPForm(QWidget *parent=NULL);
    ~PCPForm();
//
//    static int iPreIdx;


//    void changeEvent(QEvent *e);

    /////////////////////////


    QStackedLayout* mStack;
//    QVBoxLayout* vlyMain;
    QGenieRSManager *mManagerObj;
    QGenieRSBasic* mBasicObj;
    QGenieRSRemote* mRemoteObj;
    QGenieRSPrint* mPrintObj;
//    QGeniePlugin_Remote_Unregister_ReadyShare* widgetUnregister;

    const QString &Genie_IP();
    int iIpChangedCount;
    int indexOfStackLayout;
	/////////////////////////////////////////

    bool getPath(QString &exePath);
    void whenLogin(const QString &username,const QString &password);

    void runExe();
    QProcess mProcess;
    quint32 mFlags;
    void setupUi0();
    void setupUi1();
//signals:
//    void FirstGetIp(QString strIpNew);

private:

//    bool bLog;

    QString mRouterlogin_netIp;
    _HostInfoHelper *mHostInfoHelper;
public:
    QString mUsername;
    QString mPassword;
//    QString mRSUser;
//    QString mRSPwd;




public slots:

//    void slotPreIndex(int iPreIndex);
    void showPreFrom();

    void init();
//    void RegisterOk(QString strUser,QString strPassword);
//    void UnRegisterOk();
//    void RegisterNotYet();
    void routerlogin_netDNS();
};

#endif // PCPFORM_H
