#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "inc/genie_interface/IManagerHandler.h"
#include "inc/genie_interface/ISoapManager.h"
#include "netgearsoapschedule.h"
#include "NetgearSoapAPI.h"
#include "QProgressIndicator.h"
#include "messagebox.h"
#include "inc/genie_interface/ILanguageManager.h"
#include "inc/genie_interface/ISettings.h"
#include "inc/genie_interface/IPluginInf.h"
#include "dialogbox.h"

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    MainWindow(QWidget* parent);
    ~MainWindow();

private:
    void retranslateUi();

private:
    QGridLayout *gridLayout_6;
    QFrame *frame;
    QGridLayout *gridLayout;
    QRadioButton *radioButton;
    QSpacerItem *horizontalSpacer;
    QWidget *widget;
    QPushButton *pushButton;
    QRadioButton *radioButton_2;
    QSpacerItem *horizontalSpacer_2;
    QFrame *frame_2;
    QGridLayout *gridLayout_2;
    QLabel *label_3;
    QLineEdit *lineEdit;
    QSpacerItem *horizontalSpacer_3;
    QLabel *label_4;
    QComboBox *comboBox;
    QSpacerItem *horizontalSpacer_4;
    QFrame *frame_3;
    QGridLayout *gridLayout_3;
    QLabel *label_5;
    QRadioButton *radioButton_3;
    QRadioButton *radioButton_4;
    QRadioButton *radioButton_5;
    QFrame *line_3;
    QFrame *frame_4;
    QGridLayout *gridLayout_4;
    QLabel *label_6;
    QLabel *label_7;
    QLineEdit *lineEdit_2;
    QLabel *label_8;
    QFrame *line_4;
    QFrame *frame_5;
    QGridLayout *gridLayout_5;
    QSpacerItem *horizontalSpacer_5;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QSpacerItem *horizontalSpacer_6;

    QStackedLayout* stackedLayout;
    QProgressIndicator* pi;

public:
    static IManagerHandler *sHandler;
    ISoapManager *soapmanager;
    NetgearSoapSchedule *soapapi;
    ILanguageManager* languagemanager;
    ISettings* g_settings;
    IPluginInf* m_Router_plugin;

    IPluginInf *s_router_plugin;

    AgainMessageBox* box;
    MessageBox* box_2;
    MessageBox* box_3;
    MessageBox* box_4;
    MessageBox* box_5;
    MessageBox* box_6;
    QUuid uuid;
    QTimer* timer;
    QTimer* Worktimer;
    QTimer* Closetimer;
    int guestenable;
    QString guestssid;
    QString guestsecurityMode;
    QString guestkey;
    QString geniessid,geniesecurityMode,geniekey;
    int worktime;
    QString GenieMac;
    QString Mac;
    QString CurMac;

    void JudgeRouter();
    void GetSSID();
    void Security();
    void StartPi();
    void StopPi();
    void SetGuestNetworkEnable(QString);
    QString Load_key(const QString& timer);
    void save_key(const QString& time,const QString& data);
    int GetWorktime();
    void GetCurrentMac();
    void GetNetGearMac();
    QString SettingTime(int year,int month,int day,int hour,int minitus,int second);
    void GetWorkTimeFromInI();
    void GetTheCurRouterMac();
    void NETGEATMAC();

private slots:
    void nonechange();
    void WPA2change();
    void WPAAllchange();
    void Apply();
    void Cancel();
    void Refresh();
    void getResponse(QtSoapMessage&,QUuid&,QString&);
    void getSecurityResponse(QtSoapMessage&,QUuid&,QString&);
    void TimerOutDone();
    void DisableGuestAccess();
    void EnableGuestAccess();
    void DataChanged(QString);
    void WorkTimeChanged(int);
    void DoneWorktimer();
    void GetRouterMacResponse(QtSoapMessage&,QUuid&,QString&);
    void JudgeWheterNetGear();
    void GetMacResponse(QtSoapMessage&,QUuid&,QString&);
    void GetCurMacResponse(QtSoapMessage&,QUuid&,QString&);
    void AgainWork();
    void GetNETGEARMACResponse(QtSoapMessage&,QUuid&,QString&);
    void WhenCancel();

protected:
    virtual void changeEvent(QEvent* event);
};

#endif // MAINWINDOW_H
