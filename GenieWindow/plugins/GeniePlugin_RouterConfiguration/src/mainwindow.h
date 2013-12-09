#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "QGenieRCPageLogin.h"
#include "QGenieRCPageWireless.h"
#include "inc/genie_interface/ISoapManager.h"
#include "netgearsoapapi.h"
#include "DiagnoseInterface.h"
#include "inc/genie_interface/IInfoManager.h"
#include "IUiManager.h"
#include "QGenieRCSoapProxy.h"
#include "Singleton.h"

#define DLL_PING_FILE_NAME "./ping.dll"
typedef int (*PingHost)(const char *);

class QThreadConnectWLan : public QThread
{
public:
    QThreadConnectWLan(QObject *parent=0);
    ~QThreadConnectWLan();

protected:
     void run();

public:
    void ConnetWlan(const QString& nic_guid,const QString& ssid,const QString& pwd);
    DiagnoseInterface* nicwireless;
    QString g_guid;
    QString g_ssid;
    QString g_password;
};

class MainWindow : public QWidget,public Singleton<MainWindow>
{
    Q_OBJECT
public:
    MainWindow(QWidget* parent);
    ~MainWindow();
//    QStackedLayout* stackedLayout;
private:
    void retranslateUi();

public:
    QStackedWidget *mStack;
    QVBoxLayout *mLayout;

#if 0
    QGridLayout *gridLayout;
    QFrame *frame;
    QGridLayout *gridLayout_2;
    QLabel *label;
    QLabel *label_2;
    QFrame *line;
    QFrame *frame_2;
#endif
    QUuid uuid;
    ISoapManager *soapmanager;
    NetgearSoapSchedule *mSoapApi;
    QThreadConnectWLan* mThreadConnectWLan;


    QGenieRCPageLogin* login;
    QGenieRCPageWireless* wireless;

    DiagnoseInterface* nicwireless;
    QString nic_guid;
    QString nic_ssid;
    QString nic_password;


    ILanguageManager *lanmanager;

//    QLibrary *m_myLib;
//    PingHost m_pinghost;
//    QString m_filename;

public:
    static IManagerHandler *sHandler;
    IUiManager *uimanager;
    IInfoManager* infomanager;
    QStringList ModifyData;
    QTimeLine mApplyTimeLine;

public:
    void SetWireInfoPage();
    void GetPasswordPage();



    enum APPLY_TYPE
    {
        AT_WIRELESS,
        AT_GACC_ENABLE_AND_SETTING,
        AT_GACC_DISABLE,
        AT_GACC_TIMEOUT_DISABLE,
        AT_TRAFFICMETER,
        AT_NONE,
        AT_NUM
    };

    void whenApply(APPLY_TYPE type,const QString para="");
    APPLY_TYPE mApplayType;

    void wantReconnect();

protected:
    virtual void changeEvent(QEvent* event);
    QGenieRCSoapProxy *mSoapProxy;

protected slots:
    void slot_TimeLineUpdate(int c);
    void slot_TimeLineFinish();
//    void slot_scheduleTimeout();
};

#endif // MAINWINDOW_H
