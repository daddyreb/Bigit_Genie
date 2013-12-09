#ifndef NETGEARINFO_H
#define NETGEARINFO_H


#include <QObject>
#include <QMap>
#include <QUrl>
#include <QHttp>
#include <QFile>
#include <QDir>
#include <QStringList>
#include <QTimer>
#include <QAuthenticator>
#include <QHttpResponseHeader>

#include <QObject>

//#include "RouterPasswordWidget.h"

class NetgearInfo : public QObject
//class NetgearInfo : public QWidget
{
    Q_OBJECT
public:
    NetgearInfo(QObject *parent=NULL);
    ~NetgearInfo();

    //void getProductInfo(QMap<QString, QString>* *info, QString* *err);
    void getProductInfo(QString hostip, QMap<QString, QString>* *info, QString* *err);
    //QString getCurrentset();

    void getNewFirmware(QStringList **newinfo, QString mPara);
    void CloseHttpRequest();
    void AbortRouterHttp();


private slots:
    void dealFileData(bool err);
    void dealGetTimeout();
    void dealGetWaitTimeout();
    void loopGetRouteInfo();

    void CheckTimingDeal();
    void CheckWaitOut();


    void dealResponseData(QHttpResponseHeader resp);
    void dealRequestFinish(int id, bool err);

signals:
    void GetFinish(bool err);
    void CheckFinish(bool error);

    void UserPasswordError();

private:
    void getProductInfo();
    void getNewFirmware();
    QString getDownloadPath();

    QString DownloadPath;

    int getCount;
    int dealCount;
    int getTime;
    QTimer *mGetTimer;

    int waitGetTime;
    QTimer *mWaitGetTimer;

    int intLoopTime;
    QTimer *mLoopGetTimer;

    bool mHttpFlag;
    int authenNum;
    bool authenFlag;
    bool isGetFlag;
    QString strUrl;
    QUrl mGetUrl;
    QHttp infoHttp;
    QString *strError;
    QMap<QString, QString> *reInfo;
    QString tempHostip;
    QString *tempError;
    QMap<QString, QString> *tempInfo;
    QHttpRequestHeader Requestheader;
    int requestId;
    //QString mCurrentsetting;


    int checkCount;
    int checkTime;
    QTimer *mCheckTimer;
    int waitCheckTime;
    QTimer *mWaitCheckTimer;
    //bool isWait;

    bool isCheckFlag;
    QString strRootUrl;
    QString strFileName;
    QUrl mCheckUrl;
    QString strCheckUrl;
    QHttp verHttp;
    QFile mFile;
    QString checkFile;
    QStringList *mVersion;
    QStringList *tempVersion;
    QString tempPara;
    QString middleUrl;
    QString wDownloadUrl;


    QString strLog;


    ////check version
    //int checkNum;

    //QString middleUrl;

};

#endif // NETGEARINFO_H
