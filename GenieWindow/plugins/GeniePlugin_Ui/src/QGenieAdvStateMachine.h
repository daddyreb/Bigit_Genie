#ifndef QGENIEADVSTATEMACHINE_H
#define QGENIEADVSTATEMACHINE_H

#include <QtCore>
#include "SvtNetworkTool.h"

class QGenieAdvStateMachine;
class _InerAdvUtil:public QObject
{
    Q_OBJECT
    //running in child thread
public:
    enum ADV_STATE
    {
        AS_INIT,
        AS_TOTAL_DISP_OK,
        AS_ROUTE_DISP_OK,
        AS_SHOWING
    };
    enum CONF_IDX
    {
        CI_URL,
        CI_LINK,
        CI_MD5,
        CI_INTERVAL
    };
    _InerAdvUtil(QGenieAdvStateMachine *);
    ~_InerAdvUtil();
    bool mExitFlag;



protected:
    QGenieAdvStateMachine *mGenieAdvStateMachine;
    SVT_IpAddress getDNS(const QString &addr);
    QByteArray &getHttpResult(const QString &address);

    bool getTotalDiscription();
    bool getRouteDiscription();
    bool getAllPictures();
    void showingLoopUpdate();
    void changeState(ADV_STATE state);
	bool CheckSupportArea(const QString& lanType, QString& area);
    ADV_STATE mState;
    QTimer mTimer;
    QTimer mDummyTimer;
    QByteArray mArrayBuffer;
    QMap<QString,QString> mRouteMapUrl;
	QMap<QString, QStringList> mLanTypeCountryMap;
    QString mLocalDirName;
    QDateTime mLastChangeTime;
    QString mCurrentShowingFname;
    QDateTime mShowingStartTime;


    QMap<QString,QMap<int,QString> > mFileMapConf;
    QString mRoute;
    QString mLanType;
signals:
    void changeADV(QString filepath,QString linkage);
protected slots:
    void slot_Timeout();
    void slot_RouteChange(QString route);
    void slot_LanChange(QString lan);
};

class _InerSignalEmiter:public QObject
{
    Q_OBJECT
    _InerSignalEmiter();
    void routeChange(const QString &route);
    void lanChange(const QString &lan);
signals:
    void signal_Router(QString route);
    void signal_Lan(QString lan);
    friend class QGenieAdvStateMachine;
};

class QGenieAdvStateMachine:public QThread
{
    Q_OBJECT
public:
    QGenieAdvStateMachine();
    ~QGenieAdvStateMachine();
    void setRouter(const QString &route);
    void setLan(const QString &lan);
signals:
    void changeADV(QString filepath,QString linkage);

protected:
    void run();
    _InerAdvUtil *mUtil;
    QEventLoop *mEventLoop;
    QMutex mMutex;
    _InerSignalEmiter mSignalEmiter;
};

#endif // QGENIEADVSTATEMACHINE_H
