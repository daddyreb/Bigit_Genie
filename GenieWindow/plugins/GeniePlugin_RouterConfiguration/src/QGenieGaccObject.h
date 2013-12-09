#ifndef QGENIEGACCOBJECT_H
#define QGENIEGACCOBJECT_H

#include <QObject>
#include <QtGui>
#include <QtWidgets>
#include "Singleton.h"
#ifdef GENIE_RELEASE
#else
#define DBG_GACC_DISABLING_PROCESS
#endif

class QPiObject;
class QGenieRCWirelessObject;
class QGenieGaccDisableingLogic:public QObject,public Singleton<QGenieGaccDisableingLogic>
{
    Q_OBJECT
public:
    enum _STATE{
        STATE_INIT //getting 2.4g mac address
//        ,STATE_GETING_24G_MAC
        ,STATE_CHECKING_SSID_ALIVE
        ,STATE_COUNTDOWN
        ,STATE_WAITTO_POPUP
        ,STATE_END
    };
    _STATE mState;

    QGenieGaccDisableingLogic(QObject *parent);
    ~QGenieGaccDisableingLogic();
    void whenLogout();
    void whenLogIn();

    void LoadMap(QMap<QString,QMap<int,QVariant> *> &map);
    void SaveMap(const QMap<QString,QMap<int,QVariant> *> &map);
    void ClearMap(QMap<QString,QMap<int,QVariant> *> &map);
    //two function to start or disable timmer
    void notifyMac(const QString &mac);
    void setDisablingTimer(const QString &mac,const QString ssid,const QDateTime &disable_time,int idx,bool _5G);
    void RemoveSsid(bool _5G);
//    void clearMac(const QString &mac);
    int getSavedIdx(const QString &mac,bool _5g);
    int mSessionCheck24Mac;
    int mSessionCheckSsidAlive;
    int mRetryTimes;
    int mCountDown;
    bool mIs5GFirstTimer;
//    QString *mCountDownSsid;
    QString mCurMac;
//    int mSessionCheckMac;
//    int mSessionCheckGacc;
//    int mCheckMacRetryTimes;
//    int mCheckGaccTimes;

//    QTimer mDisablingCheckMacTimer;
//    QTimer mDisablingCheckGaccTimer;
    QTimer mTimer;
//    QString mCurrentMac;
    enum _DATA_ENUM{
        DE_COB_IDX
        ,DE_DEADLINE
        ,DE_SSID
        ,DE_COB_IDX_5G
        ,DE_DEADLINE_5G
        ,DE_SSID_5G
    };

    QMap<QString,QMap<int,QVariant> *> mData;
protected:
    void start();
    void tryEnd(bool force);

#ifdef DBG_GACC_DISABLING_PROCESS
public:
    QLabel *mDbglabel;
    QTimer mDbgTimer;
protected slots:
    void slot_DbgTimeout();
#endif
protected slots:
    void slot_SoapDataReturned(int session,const QStringList &data);
//    void slot_DisablingCheckMacTimeout();
//    void slot_DisablingCheckGaccTimeout();
    void slot_Timeout();
    void slot_SettingProcessEnd();



    friend class QGenieGaccObject;
};

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

class QGenieGaccObject : public QObject,public Singleton<QGenieGaccObject>
{
    Q_OBJECT
public:
    class tGaccData
    {
    public:
        tGaccData();
        ~tGaccData();
        int mSavedEnable;
        QString mSavedSsid;
        QString mSavedPwd;
        QRadioButton *mSavedRdt;
    };
    explicit QGenieGaccObject(QObject *parent = 0);
    ~QGenieGaccObject();
    QWidget *getGaccUi();
    void translateGaccUi();

    void setGaccInfo();
    void whenLogin();
//    void whenRaise();
    void whenLogout();
//    void deployIndicators(QWidget *w);
    QStackedWidget *stack_gacc;
    void updateShowingPage();
    void updateSettingPage();
    void refreshGaccInfo(bool remote,bool _5G);
protected:
    void setError(int lan);

    QWidget *mW;
    QGenieGaccDisableingLogic *mDisablingLogic;
    int mRefreshSession;
    int mRefreshSession5G;
    //QProgressIndicator *mPi;
    QLabel *mErrorLabel1;
	QLabel *mErrorLabel2;
    int mLanError;
//    int mLanEnableState;
    bool mCobTimeChanged;

    QLabel *lab_enable_r;
    QLabel *lab_ssid_r;
    QLabel *lab_pwd_r;

    QLabel *lab_enable_l;
    QLabel *lab_ssid_l;
    QLabel *lab_pwd_l;
    QLabel *lab_set_period;

    QRadioButton *rdt_enable;
    QRadioButton *rdt_disable;

    QLineEdit *edt_ssid;
    QLineEdit *edt_pwd;
    QComboBox *cob_time;
    QRadioButton *rdt_none;
    QRadioButton *rdt_aes;
    QRadioButton *rdt_mix;
    QPushButton *but_modify;
    QPushButton *but_apply;
    QFrame *fr_set_down;
    QFrame *fr_pwd;


//    QRadioButton *mRdtCurrent;
//    bool mEnableState;

//    bool mDataOk; //refresh at first login
//    bool mRefreshAfterLogin;

    ///////////2012-3-7 chensi///////////////
    void deployIndicators(QWidget *w);

    QPiObject *mPi_1;
    QPiObject *mPi_2;

    QSharedPointer<tGaccData> mSavedGaccData;
    QSharedPointer<tGaccData> mSavedGaccData_5G;
signals:

public slots:
    void slot_RefreshGaccInfo();
    void slot_DataRreturned(int session, const QStringList &lst);
    void slot_SMDataRreturned(int session, const QStringList &lst);
    void slot_buttonClicked();
    void slot_UpdateButEnableState();

    friend class QGenieDelayProcessLogic;
    friend class QGenieGaccDisableingLogic;
    friend class QGenieRCWirelessObject;
    friend class QGenieRCLackDataGetter;



};

#endif // QGENIEGACCOBJECT_H
