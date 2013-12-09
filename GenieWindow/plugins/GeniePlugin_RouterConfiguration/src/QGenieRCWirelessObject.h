#ifndef QGENIERCWIRELESSOBJECT_H
#define QGENIERCWIRELESSOBJECT_H

#include <QObject>
#include <QtGui>
#include <QtWidgets>
#include <QHostInfo>
#include "Singleton.h"
class QProgressIndicator;
class PCPForm;
class QGenieTimelineLocker;
class QPiObject;
class _LazyLoader;
class QGenieQRCodeManager;
class LockerWhenAllCPOffline;
class QGenieRCLackDataGetter;
//class QGenieTabCanLockHeader;


//enum RC_FLGAS{
#define  RCF_REMOTE                      (quint64(1)<<0)
#define  RCF_INCHANGE_UI                 (quint64(1)<<1)
#define  RCF_INCHANGE_CPIDX              (quint64(1)<<2)
#define  RCF_MUST_REFRESH_WIRELESS       (quint64(1)<<3)
#define  RCF_MUST_REFRESH_GACC           (quint64(1)<<4)
#define  RCF_MUST_REFRESH_TM             (quint64(1)<<5)
#define  RCF_MUST_REFRESH_TOPO           (quint64(1)<<6)
#define  RCF_MUST_REFRESH_WIRELESS_5G    (quint64(1)<<7)
#define  RCF_MUST_REFRESH_GACC_5G        (quint64(1)<<8)
#define  RCF_LOCAL_LOGEDIN               (quint64(1)<<9)
#define  RCF_REMOTE_LOGEDIN              (quint64(1)<<10)
#define  RCF_IN_LOCAL_LOGIN_PROCESS      (quint64(1)<<11)
#define  RCF_IN_REMOTE_LOGIN_PROCESS     (quint64(1)<<12)
#define  RCF_IN_LOCAL_REFRESHING         (quint64(1)<<13)
#define  RCF_IN_REMOTE_REFRESHING        (quint64(1)<<14)
#define  RCF_READYSHARE_MODE             (quint64(1)<<15)
#define  RCF_TRUNTO_CHANEL               (quint64(1)<<16)
#define  RCF_TURNTO_GACC                 (quint64(1)<<17)
#define  RCF_TURNTO_FUPDATE              (quint64(1)<<18)
#define  RCF_TURNTO_PWD                  (quint64(1)<<19)
#define  RCF_TRUNTO_CHANEL_REMOTE        (quint64(1)<<20)
#define  RCF_TURNTO_GACC_REMOTE          (quint64(1)<<21)
#define  RCF_TURNTO_PWD_REMOTE           (quint64(1)<<22)
#define  RCF_HAVE_FIRMWARE_UPDATE        (quint64(1)<<23)
#define  RCF_TIMELINELOCKER_WORKING      (quint64(1)<<24)
#define  RCF_INCHANGELANGUAGE            (quint64(1)<<25)
#define  RCF_SMUSER_IS_SAVEDUSER         (quint64(1)<<26)
#define  RCF_SMARTNETWORK_UNLOCKED       (quint64(1)<<27)
#define  RCF_FUPDATE_DATA_OK             (quint64(1)<<28)
#define  RCF_FUPDATE_IN_REFRESHING       (quint64(1)<<29)
#define  RCF_ALL_CP_OFFLINE              (quint64(1)<<30)
#define  RCF_READYSHARE_PROCESS_ON       (quint64(1)<<31)
#define  RCF_RELOGIN_DIALOG_ON           (quint64(1)<<32)
#define  RCF_5G_TABCHECKED               (quint64(1)<<33)

#define  RCF_LOCAL_WIRELESS_5GUNLOCKED   (quint64(1)<<34)
#define  RCF_LOCAL_GACC_5GUNLOCKED       (quint64(1)<<35)
#define  RCF_REMOTE_WIRELESS_5GUNLOCKED  (quint64(1)<<36)
#define  RCF_REMOTE_GACC_5GUNLOCKED      (quint64(1)<<37)
#define  RCF_IS_SHOWING_5GFLAG           (quint64(1)<<38)
#define  RCF_IN_LOGOUT_FUNCTION          (quint64(1)<<39)
#define  RCF_DEBUG_GACC_COUNTDOUN        (quint64(1)<<40)
#define  RCF_MODAL_DIALOG_ON             (quint64(1)<<41)


//};
#define IS_REMOTE_UI() ((sRCFlags&RCF_REMOTE)&& !(sRCFlags&RCF_READYSHARE_MODE))

#define IS_WIRELESS_5G() ((sRCFlags&RCF_5G_TABCHECKED)&&\
            (\
            (IS_REMOTE_UI()&&(sRCFlags&RCF_REMOTE_WIRELESS_5GUNLOCKED))\
            ||(!IS_REMOTE_UI()&&(sRCFlags&RCF_LOCAL_WIRELESS_5GUNLOCKED))\
            ))
#define IS_GACC_5G() ((sRCFlags&RCF_5G_TABCHECKED)&&\
    (\
    (IS_REMOTE_UI()&&(sRCFlags&RCF_REMOTE_GACC_5GUNLOCKED))\
    ||(!IS_REMOTE_UI()&&(sRCFlags&RCF_LOCAL_GACC_5GUNLOCKED))\
    ))

class QTellLPCHelper:public Singleton<QTellLPCHelper>
{
public:
    QTellLPCHelper(const QMap<QString,QPair<QString,QString> > &map);
    QString mLoginUser;
    QString mLoginPwd;
    QString mCpName;
    bool mRemote;
    const QMap<QString,QPair<QString,QString> > &mCpSecureRef;
    void whenLoginOk(bool remote,const QString &usr,const QString &pwd);
    void tryTellLPC();

};

class QGenieRCWirelessObject : public QObject,public Singleton<QGenieRCWirelessObject>
{
    Q_OBJECT
public:
    class tRCData
    {
    public:
        tRCData();
        ~tRCData();
        QString mSavedMac;
        QString mSavedSsid;
        QString mSavedPwd;
        QRadioButton* mSavedRdt;
        QString mSavedMod;
        QString mSavedRegion;
        QString mSavedCannel;

        QString mEncryption;
        QString mAuthentation;
    };
    explicit QGenieRCWirelessObject(QObject *parent = 0);
    ~QGenieRCWirelessObject();
    void logOut(bool remote);
    QWidget *getWirelessUi();
    bool lazyLoad();//return if need re_enter;

    QWidget *mWirelessW;
//    QWidget *mFakeStack;
//    QPushButton *mFakeLogoutBut;
//    QWidget *mFakeCanvas;
    QWidget *mPage1;
    QWidget *mGaccW;
    QWidget *mTpW;
    QTabWidget *mTab;
//    QString mSavedMac;
    QStackedWidget *mStackTotal;
    QLabel *lab_model;
    QLabel *lab_local_model;
    QFrame *login_frame;
    QPushButton *but_apply;
    QFrame *fr_pwd;
    QLineEdit *mEdtUsr;
    QLineEdit *mEdtSMUsr;
    QLineEdit *mEdtPwd;
    QLineEdit *mEdtSMPwd;
    QCheckBox *chk_remmember;
    QCheckBox *chk_smremember;
	QCheckBox *chk_inputIP;
	QFrame *frame_ip;
	QLineEdit *mEdtIP;
//    QString mSavedSsid;
//    QString mSavedPwd;

    QGenieRCLackDataGetter *mLDGetter;
    QMap<QString,QPair<QString,QString> > mCpSecure;
    void whenLanChaned();
    void startLogin(bool remote);
    void changeRemoteState(bool bremote);
    void updateUi();
#ifdef USE_5GFUNC
    QTabBar *m5GTab;
    QLabel *m5GFlagLabel;
    void update5GUi();
#endif
    void _updateLockState();
    QString getCpName() const;
    bool getCpOnline() const;
    QString getCpFirmware() const;
    QString getModelName() const;
    void setLoginError(int lan_idx, bool remote);
    bool tryReloginCP();
    void tryUpdateData();
    void tryUpdateFocus();
    void tryUpdateFirmware();


    QPushButton *mModify;
    QPushButton *mLogout;
    QPushButton *mExport;
    QPushButton *mButSMLogin;
    QString mModel;
    QString mFirmware;
    QWidget *mSavedTabW;
    QCompleter *mCompleter;
    QStandardItemModel *mCompleteModel;
    QSharedPointer<tRCData> mSavedData;
    QSharedPointer<tRCData> mSavedData_5G;
protected:
    //for init ui
    void translateWirelessWidget(QWidget *w);
    void translateSMLoginDialog();

    void deployIndicators(QWidget *w);

    void loginOk(bool remote);


    void setError(int lan_idx);
    void connectStateCheckSignalSlots(QWidget *stack_wc);
    void updateShowingPage();
    void updateSettingPage();


    QDialog *mReloginDlg;
    LockerWhenAllCPOffline *mLockerWhenAllCPOffline;

    //for logic

    int mSessionLogin;
    int mSessionSMLogin;
    int mSessionWireless;
    int mSessionWireless5G;
    int mSessionFirmware;

//    int mSession5G;
    QPiObject *mIndicatorLogin;
    QPiObject *mIndicatorWireless;
    QPiObject *mIndicatorWirelessSetting;

//    QRadioButton *mSavedRdt;
//    int mSavedCobIdx;

//    QString mSavedMod;
//    QString mSavedRegion;


protected:
    QLabel *mErrorLabel;
    int mLanError;
    QTellLPCHelper mTellLpcHelper;


    QStackedWidget *mStackWC;
    QGenieTimelineLocker *mTimelineLocker;
    int mLanLoginErrorLocal;
    int mLanLoginErrorRemote;

    QLineEdit *mEdtSsid;
    QLineEdit *mEdtKey;
    QLabel *lab_login_error;

    QLabel *lab_firmware;
    QLabel *lab_remote_firmware;
    int mSavedCpIdx;
#ifdef USE_SUBPLUGINS
    QWidget *mSubpluginUpdateW;
    //    int mSubpluginUpdateUuid;
    QWidget *mWExportW0;
    QWidget *mWExportW1;
    int mSubpluginExportUuid;
#endif
    QRadioButton *mRdtNone;//=qFindChild<QRadioButton *>(mWirelessW,"rdt_none");
    QRadioButton *mRdtAES;//rdt_aes=qFindChild<QRadioButton *>(mWirelessW,"rdt_aes");
    QRadioButton *mRdtMix;
    //        QRadioButton *rdt_mix=qFindChild<QRadioButton *>(mWirelessW,"rdt_mix");
    QComboBox *mCobChannel;//=qFindChild<QComboBox *>(mWirelessW,"cob_channel");
    QComboBox *cob_remote_model;
    QComboBox *cob_loginmode;
    QStackedWidget *stack_login;
    QGenieQRCodeManager *mQRCM;
    void refreshWireless(bool remote, bool _5G);


//    QStringList mCpList;
#ifdef USE_READYSHARE
    PCPForm *mPcpForm;
#endif
#ifdef USE_TRAFFICMETER
    QWidget *mTMW;
#endif

#ifdef USE_QOSFUNC
	QWidget* mQosW;
#endif
    QWidget* mRebootW; ///kai.yan add
    _LazyLoader *mLazyLoader;

private:
	uint m_QosState; ///路由器是否支持Qos
	QString m_ip_Manual; ///manual input ip address
private slots:
	void slot_Lookup(const QHostInfo hostInfo);
signals:

public slots:
    void slot_buttonClicked();
    void slot_DataReturned(int session,const QStringList &lst);
    void slot_SMDataReturned(int session,const QStringList &lst);
    void slot_FirmwareDataRetruned(int session,const QStringList &lst);
    void slot_RefreshWireless();
    void slot_RefreshWirelessSM();
    void slot_LoginModeChaged(int idx);

    void slot_TabIdxChanged(int idx);
    void slot_RemmemberClicked(bool b);
    void slot_CheckUiEnableState();
    void slot_CpNameChaged(int idx);
    void slot_RememberCpCheckBoxChanged(bool b);
    void slot_SMUserNameChanged(const QString &text);
	
	void slot_InputIPCheckBoxChanged(bool b); ///kai.yan add
	
#ifdef USE_5GFUNC
    void slot_5GTabIndexChanged(int idx);
    void slot_Refresh5GUiWhenModifyModeChanged();
#endif

//    void slot_ChkRememberClicked(bool b);
    friend class QGenieDelayProcessLogic;
    friend class _LazyLoader;
    friend class QGenieQRCodeManager;
    friend class QGenieGaccDisableingLogic;
    friend class QGenieRCLackDataGetter;

};

#endif // QGENIERCWIRELESSOBJECT_H
