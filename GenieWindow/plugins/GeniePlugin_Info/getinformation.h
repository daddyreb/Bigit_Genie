#ifndef GETINFORMATION_H
#define GETINFORMATION_H

#include <QObject>
#include <QMap>
#include <QMutex>
#include <QTest>
#include <QTime>
#include <QHostInfo>
#include <QLibrary>

#include "IManagerHandler.h"

//#include "infomessagebox.h"
#include "netgearinfo.h"
//#include "netgearsoap.h"

//#include "RouterPasswordWidget.h"
#include "netscanthread.h"

//class NetgearInfo;
//class NetgearSoap;


class GetInformation : public QObject
{
    Q_OBJECT

public:
    GetInformation(QObject *parent=NULL);
    ~GetInformation();

    //QString isProduct(const QString &para);
    QString isProduct(const QString &para, const bool isEmit);
    QString getNetgearMAC();
    //void startNetgearMAC();
    QString getProductType();
    QString getFirmwareVersion();
    QString getCompareVersion();
    QString getSupportLevel();
    QString getCompareData();
    QString getDownloadUrl();
    QString getNetgearCurrentset();
    void getCurrentVersion();


    void isFirmwareUpgrade();
    void setTimingUpgrade();
    void AbortGetHttp();


private slots:
    void GetLookupData(const QHostInfo &Host);
    void StartGetProduct(bool err);

    void dealProductInfo(bool err);
    void dealVersionData(bool err);
    void setUpgrade();
    void isTimingUpgrade();

    void GetInfoThreadOver();

signals:
    void GetFirmwareFinish(bool err);
    void DnsLookupFinish(bool err);


public:
    static IManagerHandler *sHandler;

private:
    bool isUpgrade(QString oldV, QString newV);
    void NetgearLookup();
    void getNetgearVer();
    void setRouterIpMac();
    //void getadapterInfo(unsigned long & index, QString &mac, QString &adaptername, QString &gateway, QString &localip, QString &mask);
    void getadapterInfo();

    bool w_Modelflag;

    bool getFlag;
    bool checkFlag;
    bool timingFlag;

    bool sendEmit;
    bool getStart;

    int mDaySec;

    int upgradeSec;


    QTimer *mTimer;
    QTimer *mTimingTime;

    QString mGatewayIp;
    QString mGatewayMac;
    QString mNetgearHost;
    QString CurrentModel;
    QString CurrentFirmware;
    QString CurrentSupportLevel;
    QString netgearFlag;
    QString routerMAC;
    QMutex mMutex;

    QString *rErr;
    QMap<QString, QString> *ProductInfo;
    QStringList *newFirmwareInfo;
    QString newVersion;
    QString wDownloadURL;
    QString twoCompareData;
    NetgearInfo *mNetgearInfo;


    NetScanThread *mGwThread;
    QMap<QString, QString> *mGatewayInfo;

    //IInfoManager *mInfoManager;

    //InfoMessageBox *mSetUserPassword;

};


#endif // GETINFORMATION_H
