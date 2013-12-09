#ifndef WIRELESS_H
#define WIRELESS_H

#include <QtGui>
//#include "wirelessinfo.h"
//#include "wirelesssettings.h"
#include "netgearsoapschedule.h"
#include "ILanguageManager.h"
#include "netgearsoapapi.h"
#include "inc/genie_interface/IPluginInf.h"
#include "Singleton.h"
#include "QGenieRCSoapProxy.h"
#include "QGenieRCPageGuessAccess.h"
class QGenieRCPageGuessAccess;
#include "ui_QGenieRCPageWireless.h"
#include "soapclient.h"

class QGenieRCPageGuessAccess;

enum ENUM_PAGE
{
    EP_INFO=0,
    EP_SETTING,
//    EP_GACC_INFO,
//    EP_GACC_SETTING,
    EP_NUM

};

class QGenieRCPageWireless : public QWidget,public ISoapClient,public Singleton<QGenieRCPageWireless>
{
    Q_OBJECT
public:
    QGenieRCPageWireless(QWidget* parent=NULL);
    ~QGenieRCPageWireless();
    Ui::QGenieRCPageWireless mUi;

private:
    void retranslateUi();

private:

public:
//    QTimer mTimers[EP_NUM];
    QTimer mTimer;
    //WirelessExport
//    IPluginInf *m_wirelessexport_plugin;
    QWidget *m_wirelessexport_widget;
//    IPluginInf *m_TrafficMeter_plugin;
    QWidget *m_TrafficMeter_widget;
#ifdef ENABLE_READYSHARE
    QWidget *m_ReadyShare_Widget;
#endif

    int TrafficMeter_No;
    QWidget *m_RouterUpdate_widget;

public:

    ILanguageManager *lanmanager;
    QUuid mUuid;
    void loadSubPlugins();
    void Load_WirelessExport();
    void Load_TrafficMeter();
    void Load_RouterUpdate();
#ifdef ENABLE_READYSHARE
    void Load_ReadyShare();
#endif
    void SetWirelessPage();
    void endSettingMode();

    void SetExportPage();
    void startSettingMode();
    inline bool isRefreshing()
    {
        return  mTimer.isActive();
    }

    QString mSsid;
    QString mPwd;
    QString mAuthentation;
    QString mEncryption;

    QString mRegion;
    int mChannelIndex;
    int mModeIndex;
#ifdef NOT_SET_MODE
    QString mModeStr;
#endif
    int mErrStrIdx;
    QRadioButton *mCurrentWpaRdt;
    QString mTrafficMetterImg;

    void notifyMacChange(QString mac);
    void setBackgroundImg();

    void clearText();
    void startUiRefresh();
    void stopUiRefresh(bool bOk=true);
    void showError(int lanidx);
    void hideError();
    virtual void parseSoapResponse(int cmd,QtSoapMessage&);
    virtual void whenSoapError(int cmd,const QString& reson );

    void translateRouteType();

    QString mNotifyWpa;
    QString mNotifyRZ;
    QString mNotifySsid;
    QString mNotifyPwd;

    void tryNotifyRouteChanges();
    inline void setNotifyParas(const QString &wpa,const QString &rz,const QString &ssid,const QString &pwd)
    {
        mNotifyWpa=wpa;
        mNotifyRZ=rz;
        mNotifySsid=ssid;
        mNotifyPwd=pwd;
    }

    void setNotifyParas();

    void refresh(int delay=0);


public slots:
    void slot_EnableApply();
    void slot_TabIdxChanged(int);
    void slot_Timeout();
    void slot_refresh();
    void slot_refreshDelayTimeout();
    void on_but_apply_clicked();
    void on_but_export_clicked();
    void on_but_modify_clicked();
    void on_but_info_cancel_clicked();
    void on_but_set_cancel_clicked();

protected:
    virtual void changeEvent(QEvent* event);
    QTimer mRefreshDelay;
};

#endif // WIRELESS_H
