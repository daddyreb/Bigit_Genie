#ifndef GENIEPLUGIN_MAPV2_H
#define GENIEPLUGIN_MAPV2_H

#include <QObject>
#include <QtCore>
#include "genie2_interface.h"


class NetworkMapView;
class NewDeviceNotifyManager;
class NetScanningController;
class QTimer;
class BlockDeviceManager;
class PowerlineHandler;


class GeniePlugin_MapV2 : public QObject ,public IPluginInf2
{
    Q_OBJECT
    Q_INTERFACES(IPluginInf2)
    Q_PLUGIN_METADATA(IID "com.netgear.GeniePlugin.IPluginInf2/1.0" FILE "GeniePlugin_Map.json")
public:
    /*explicit*/ GeniePlugin_MapV2(QObject *parent = 0);
    ~GeniePlugin_MapV2();
    virtual QVariant command(int cmd,const QVariant &paras);

//    virtual QString text();

     QWidget* widget();

//    virtual QStringList bigButtonParas();

//    virtual void notifyBigButton(QString s);

//    virtual QIcon *icon();
//    virtual QString info();


//    virtual int priority();
//    virtual int changelanguage(int l);
//    virtual void setHandler(IManagerHandler *hdl);
//    virtual void startup(bool mini);
//    virtual void notifyRaise();
//    virtual void notifyLower();
//    virtual void notifyTrayMessage(QSystemTrayIcon::ActivationReason reason,MAIN_FRAME_RESPONSE res);

//    virtual QString getInfo(int para);
//#ifdef REBOUNCE_FOR_RAISE
//    static bool sInFadeoutState;
//#endif
#ifdef INFO_WRITE_BACK
    virtual void notifyInfo(QString request,QString result);
#endif
//    virtual void command(int cmd,QString para0 = "",QString para1 = "");
//    virtual void command(int cmd, int para);

//    virtual void *getPoint(int para);
//----------------------------------------------------------------------------------
private:
//    IManagerHandler         *m_managerHandler;
//    IUiManager              *m_uiManager;
//    ILanguageManager        *m_langManager;
//    ISettings               *m_settings;
//    IClickManager           *m_clickManager;
//    IInfoManager            *m_infoManager;
//    ILogManager             *m_logManager;
//#ifdef DELAY_CURRENTSETTING_CMD
//    QTimer                  *m_routerTypeTimer;
//#endif
private:
    NetworkMapView          *m_nmv;
    NewDeviceNotifyManager  *m_ndnm;
    NetScanningController   *m_nsc;
    BlockDeviceManager      *m_bdm;
    int                     m_deviceCount;
#ifdef POWERLINE_REVISION
    PowerlineHandler *mPowerlineHandler;
#endif

private:
    void loadConfigurationItem(const QString &key,QVariant &value);
    void storeConfigurationItem(const QString &key,const QVariant &value);
    QString mFUPUrl;
    QString mFUPMd5;
    QString mFUPSize;
    QString mFUPVersion;
    QString mFirmware;
    QString mCurrentSetting;
    QString mRouterLoginNetIp;
    QString mRouter;
    QString mRsLevel;
//    QTimer mNetgearInfoTimer;
    QDateTime mLastUpdateTime;
//    void setUiState(const QMap<int,QString> &data);

private slots:
    void onDeviceCountChanged(int count);
    void retranslateUi();
    void translateText(int idx ,QString *text);
//    void dumpLog(const QString &log);
    void onRequestRouterImagePath(const QString &model,int imageRole,QString *imagePath);


    void loadDefaultInterfaceFromConfiguration(QString *infIdx);
    void storeDefaultInterfaceToConfiguration(const QString &infIdx);

    void loadNewDeviceNotifyOption(int *option);
    void storeNewDeviceNotifyOption(int option);

	void loadRecvFolderFromConfiguration(QString *folder);
	void storeDefaultRecvFolderToConfiguration(const QString &folder);

	void loadSendFolderFromConfiguration(QString *folder);
	void storeDefaultSendFolderToConfiguration(const QString &folder);


#ifdef ENABLE_POWERLINE
    void loadPLCDevicePassword(const QString &devMac, QString *pwd);
    void storePLCDevicePassword(const QString &devMac, const QString &pwd);
#endif

    void Statistics(const QString &funCode);
    void onRefreshRequest();
    void onRouterTypeChanged();

//    void tryRefreshRouterType();
    void onDeviceSetupMenuItemTriggered();
    //add by caiyujun
    void onSendFileTriggered();
    void onSendFolderTriggered();
    //add end

//    void onNetgearInfo(const QMap<int,QString> &data);
//    void onNeedRefreshNetgearInfo();
    void onGatewayChanged(QString data);
};

#endif // GENIEPLUGIN_MAPV2_H
