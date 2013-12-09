#ifndef SVTNETGEARSPECIFICHELPER_H
#define SVTNETGEARSPECIFICHELPER_H

#include <QtCore>
#include <QtNetwork>
//#include "qtsoap.h"
class SVT_TopoMethod;
class SVT_SeedFactoryInterface;
class NetgearSoapSchedule;
class SVT_NetgearSpecificHelper;
class SVT_Soap;
class SVT_NetgearSpecificFSM :public QObject
{
    Q_OBJECT
public:


    SVT_NetgearSpecificFSM(SVT_NetgearSpecificHelper *helper);
    ~SVT_NetgearSpecificFSM();
    void start(const QString &ip,const QString &routetype);
    void setCmd(int cmd,const QVariant &para=QVariant());
    void serialize(bool load);
    int getCmd();
protected:
    int mState;
    int mOldState;
//    NetgearSoapSchedule *mSoapSchedule;
    SVT_NetgearSpecificHelper *mHelper;
    QString mHost;
    QString mRouteType;
    bool busy();
    void update();
    void updateGettingAttacheDevice();
	//add by caiyujun
	void updateGettingExtenderAttatcheDevice();
	//add end
    void updateGettingEnableStat();
    void updateRunning();
    void updateClosed();
    void updateRunningADM();
    void updateClosedADM();
    void updateLoging();
    void updateEnabling();
    void updateDisabling();
    void updateEnableDisablingMac();
    bool updateResetFSM();
#ifdef REFRESH_ATTACHDEVICE_WHEN_NEW_DEV_DETECTED
    bool checkIfNeedRefresh();
protected slots:
    void slot_timeoutRefreshAttachDevice();
protected:
//    QTimer mRefreshTimer;
    QDateTime mstartRefreshDT;
#endif
//    void updateInit(QtSoapMessage *soapmessage);
    void changeState(int state);
    QDateTime mDummyBeforThisTime;

    QDateTime mAttachDeviceDT;
    int mRetryTimes;
    int mCurrentCmd;
    QVariant mCurrentCmdPara;
    bool mlogedIn;
    bool mQuickPassthrough;
    QMutex mCmdMutex;
    SVT_Soap *mSoap;
signals:
    void signal_FSMStateChanged(int,int);
protected slots:

   friend class SVT_NetgearSpecificHelper;

};

/*************************************************************************/
/*************************************************************************/
/*************************************************************************/
/*************************************************************************/




class SVT_NetgearSpecificHelper : public QObject
{
    Q_OBJECT
public:
    SVT_NetgearSpecificHelper(SVT_TopoMethod *method,SVT_SeedFactoryInterface *);
    ~SVT_NetgearSpecificHelper();
    void run();
    SVT_TopoMethod *mMethod;
    SVT_SeedFactoryInterface *mFactory;
    SVT_NetgearSpecificFSM mFSM;

//    QSemaphore mSemaphore;
//    QEventLoop mEventLoop;
//    QList<QHttp *> mHttps;
//    QTimer mTimer;
//    QMap<QHttp *,quint32> mHttpMapIp;
//    QMap<QHttp *,int> mHttpMapRequest;

//    NetgearSoapSchedule *mSoapSchedule;
protected:
//    enum SOAP_STATE
//    {
//        SS_INIT,
//        SS_CANSTART,
//        SS_RETRY1,
//        SS_RETRY2,
//        SS_RETRY3,
//        SS_RETRY4,
//        SS_END
//    };
//    int mSoapState;
//    int mSoapCounter;
    QDateTime mLastSoapDT;
//    void trySubmitSoap();

signals:

protected:
//    void slot_RequestFinished( int id, bool error );
//    void slot_Done(bool error);
    void slot_Timeout(bool fsm_busy);
//    void slot_SoapResponse(QtSoapMessage&,QUuid&,QString&);

};

#endif // SVTNETGEARSPECIFICHELPER_H
