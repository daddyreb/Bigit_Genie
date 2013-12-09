#include <QtGui>

#include "IUiManager.h"
#include "IInfoManager.h"

#include "GeniePlugin_Info.h"
#include "getinformation.h"



IManagerHandler *GetInformation::sHandler = NULL;

GetInformation::GetInformation(QObject *parent)
    :QObject(parent)
{

    w_Modelflag = false;

    mNetgearHost = "routerlogin.net";
    //getStart = false;
    getStart = true;

    //mInfoManager = sHandler->getInfoManager();
    //Q_ASSERT(mInfoManager);

    //NetgearSoap::sHandler = sHandler;
//    getFlag = true;
    checkFlag = true;
    timingFlag = false;

    CurrentModel = "";
    CurrentFirmware = "";
    //////Jansion 2011-04-08   ReadyShareSupportedLevel=7
    CurrentSupportLevel = "";

    mDaySec = 86400000;
    //macTime = 60000;
    upgradeSec = 360000;
    //upgradeSec = 30000;

    newFirmwareInfo = new QStringList;
    Q_ASSERT(newFirmwareInfo);
    ProductInfo = new QMap<QString, QString>;
    Q_ASSERT(ProductInfo);
    rErr = new QString;
    Q_ASSERT(rErr);
    mNetgearInfo = new NetgearInfo();
    Q_ASSERT(mNetgearInfo);
    connect(mNetgearInfo, SIGNAL(GetFinish(bool)), this, SLOT(dealProductInfo(bool)));
    connect(mNetgearInfo, SIGNAL(CheckFinish(bool)), this, SLOT(dealVersionData(bool)));


    mTimer = new QTimer();
    Q_ASSERT(mTimer);
    //mTimer->setSingleShot(true);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(setUpgrade()));

    mTimingTime = new QTimer();
    Q_ASSERT(mTimingTime);
    mTimingTime->setSingleShot(false);
    connect(mTimingTime, SIGNAL(timeout()), this, SLOT(isTimingUpgrade()));


    //connect(this, SIGNAL(DnsLookupFinish(bool)), this, SLOT(StartGetProduct(bool)));

    mGwThread = new NetScanThread();
    mGatewayInfo = new QMap<QString, QString>;
    connect(mGwThread, SIGNAL(ThreadFinish()), this, SLOT(GetInfoThreadOver()));

}

GetInformation::~GetInformation()
{
    if (mTimer->isActive())
        mTimer->stop();
    if (mTimingTime->isActive())
        mTimingTime->stop();
    delete mTimer;
    delete mTimingTime;

    delete rErr;
    delete ProductInfo;
    delete newFirmwareInfo;

    delete mGatewayInfo;
    delete mGwThread;

    mNetgearInfo->CloseHttpRequest();

    delete mNetgearInfo;

}


QString GetInformation::isProduct(const QString &para, bool isEmit)
{
    w_Modelflag =false;
    sendEmit = isEmit;

    qDebug() << "Get router of type: " << para;
    if (para=="GATEWAY"){
        GeniePlugin_Info::writeLog("Get router version by MAP driver.");
        setRouterIpMac();
        //getadapterInfo();
    }else if (para=="RT_SET"){
        GeniePlugin_Info::writeLog("Get router version by Router Setttings.");
        getadapterInfo();
        w_Modelflag = true;

        return "?";
    }else {
        GeniePlugin_Info::writeLog("Periodically reads the router version.");
        if (para=="TEST"){
            mGatewayIp = "192.168.1.1";
            qDebug() << "Test -- get router version information.";
        }
    }

    try{
        getNetgearVer();
    }catch(...){
        GeniePlugin_Info::writeLog("Judge the router type.");
        Q_ASSERT(0);
    }

    if ( para == "IS_NETGEAR" ){
        netgearFlag.clear();
        int j = 0;
        while (j++ < 60){
            if (netgearFlag.isEmpty()){
                QTest::qWait(500);
                qDebug() << "Get Firmware time's No." << j;
            }
        }
    }else {
        netgearFlag = "?";
    }

    return netgearFlag;

}

void GetInformation::setRouterIpMac()
{
    IInfoManager *mInfoManager = sHandler->getInfoManager();
    Q_ASSERT(mInfoManager);
    QString mMacIp;
    QStringList mMacIpList;
    mMacIp = mInfoManager->getInfo(INFO_CMD_GATEWAY);
    mMacIpList = mMacIp.split(",");
    mGatewayMac.clear();
    mGatewayIp.clear();
    if (mMacIpList.size()>1){
        mGatewayMac = mMacIpList.value(0);
        if (mMacIpList.value(1)!="0.0.0.0")
            mGatewayIp = mMacIpList.value(1);
    }

    //////
    if ( mGatewayMac.isEmpty() ){
        //routerMAC = "FF:FF:FF:FF:FF:FF";
        routerMAC = "NULL";
        mInfoManager->notifyPluginMessage(INFO_NOTIFY_MAC_CHANGE, "", "");
    }else {

        routerMAC = mGatewayMac;
        routerMAC.replace(QString(":"), QString("-"));

        mInfoManager->notifyPluginMessage(INFO_NOTIFY_MAC_CHANGE, routerMAC, "");
    }

    QString wlog = "The NETGEAR router IP:" + mGatewayIp + ", and MAC:" + routerMAC + "--" + mMacIp;
    GeniePlugin_Info::writeLog(wlog);

}

void GetInformation::getadapterInfo()
{
    mGwThread->StartGetGatewayInfo(&mGatewayInfo);

}

void GetInformation::GetInfoThreadOver()
{
    mGatewayMac = mGatewayInfo->value("MAC");
    mGatewayIp = mGatewayInfo->value("IP");

    QString wlog = "NetScan DLL: The NETGEAR router IP:" + mGatewayIp + ", and MAC:" + routerMAC;
    GeniePlugin_Info::writeLog(wlog);


    try{
        getNetgearVer();
    }catch(...){
        GeniePlugin_Info::writeLog("Judge the router type for router setting.");
        Q_ASSERT(0);
    }

}

void GetInformation::getNetgearVer()
{
    if ( !mGatewayIp.isEmpty() ){
        mNetgearInfo->getProductInfo(mGatewayIp, &ProductInfo, &rErr);
    }else {
        GeniePlugin_Info::writeLog("Not router IP address.");
        //NetgearLookup();
    }

}

void GetInformation::dealProductInfo(bool err)
{
    qDebug() << "Obtained router information";
    QString wlog;
    wlog = "Obtained router information: ";
    if (err){

        QString mTempInfo;
        QString tempModel;
        tempModel = ProductInfo->value("Model");
        QString tempFirmware;
        tempFirmware = ProductInfo->value("Firmware");
        //////Jansion 2011-04-08
        CurrentSupportLevel = ProductInfo->value("ReadyShareSupportedLevel");


        if ( tempModel != "" && tempFirmware != "" ){
            netgearFlag = "y";

            if (tempModel!=CurrentModel || tempFirmware!=CurrentFirmware){
                IUiManager *mUimanager = GetInformation::sHandler->getUiManager();
                Q_ASSERT(mUimanager);
                //mTempInfo = "Firmware Version " + tempFirmware;
                mTempInfo = tempFirmware;
                mUimanager->command(UI_CMD_SET_FRIMWARE, mTempInfo);
                mTempInfo.clear();
                //mTempInfo = "Router Model " + tempModel;
                mTempInfo = tempModel;
                mUimanager->command(UI_CMD_SET_ROUTE, mTempInfo);

                CurrentModel = tempModel;
                CurrentFirmware = tempFirmware;

                //////check upgrade
                mUimanager->command(UI_CMD_HIDE_UPDATE_BUTTON, "");
                if (mTimer->isActive())
                    mTimer->stop();
                if (mTimingTime->isActive())
                    mTimingTime->stop();
                checkFlag = true;
                isFirmwareUpgrade();
                setTimingUpgrade();


                getStart = false;

            }else {
                if ( getStart || w_Modelflag ){
                    QString mTempInfo;
                    mTempInfo.clear();

                    IUiManager *mUimanager = GetInformation::sHandler->getUiManager();
                    Q_ASSERT(mUimanager);
                    //mTempInfo = "Firmware Version " + tempFirmware;
                    mTempInfo = tempFirmware;
                    mUimanager->command(UI_CMD_SET_FRIMWARE, mTempInfo);
                    mTempInfo.clear();
                    //mTempInfo = "Router Model " + tempModel;
                    mTempInfo = tempModel;
                    mUimanager->command(UI_CMD_SET_ROUTE, mTempInfo);

                    CurrentModel = tempModel;
                    CurrentFirmware = tempFirmware;

                    //////check upgrade
                    //mUimanager->command(UI_CMD_HIDE_UPDATE_BUTTON, "");
                    if (mTimer->isActive())
                        mTimer->stop();
                    if (mTimingTime->isActive())
                        mTimingTime->stop();
                    checkFlag = true;
                    isFirmwareUpgrade();
                    setTimingUpgrade();


                    getStart = false;
                }
            }

        }else {
            netgearFlag = "n";

            if (tempModel!=CurrentModel || tempFirmware!=CurrentFirmware){
                IUiManager *mUimanager = GetInformation::sHandler->getUiManager();
                Q_ASSERT(mUimanager);
                mTempInfo.clear();
                mTempInfo = "";
                mUimanager->command(UI_CMD_SET_FRIMWARE, mTempInfo);
                mUimanager->command(UI_CMD_SET_ROUTE, mTempInfo);

                CurrentModel = mTempInfo;
                CurrentFirmware = mTempInfo;

                //////stop timer for upgrade
                mUimanager->command(UI_CMD_HIDE_UPDATE_BUTTON, "");
                if (mTimer->isActive())
                    mTimer->stop();
                if (mTimingTime->isActive())
                    mTimingTime->stop();


                getStart = false;
            }else {
                if ( getStart || w_Modelflag ){
                    QString mTempInfo;
                    mTempInfo = "";

                    IUiManager *mUimanager = GetInformation::sHandler->getUiManager();
                    Q_ASSERT(mUimanager);
                    mUimanager->command(UI_CMD_SET_FRIMWARE, mTempInfo);
                    mUimanager->command(UI_CMD_SET_ROUTE, mTempInfo);

                    mUimanager->command(UI_CMD_HIDE_UPDATE_BUTTON, "");

                    CurrentModel = mTempInfo;
                    CurrentFirmware = mTempInfo;

                    getStart = false;
                }
            }
        }

        wlog = wlog + "NETGEAR Model=" + tempModel + ", Firmware=" + tempFirmware + ", Level=" + CurrentSupportLevel + ". Get over";
    }else {
        netgearFlag = "n";
        CurrentSupportLevel = "";

        if (ProductInfo->value("Model")!=CurrentModel || ProductInfo->value("Firmware")!=CurrentFirmware){
            ////View The Version on MainFram
            QString mTempInfo;
            mTempInfo = "";

            IUiManager *mUimanager = GetInformation::sHandler->getUiManager();
            Q_ASSERT(mUimanager);
            mUimanager->command(UI_CMD_SET_FRIMWARE, mTempInfo);
            mUimanager->command(UI_CMD_SET_ROUTE, mTempInfo);

            CurrentModel = mTempInfo;
            CurrentFirmware = mTempInfo;

            //////stop timer for upgrade
            mUimanager->command(UI_CMD_HIDE_UPDATE_BUTTON, "");
            if (mTimer->isActive())
                mTimer->stop();
            if (mTimingTime->isActive())
                mTimingTime->stop();

            getStart = false;
        }else {
            if ( getStart || w_Modelflag ){
                QString mTempInfo;
                mTempInfo = "";

                IUiManager *mUimanager = GetInformation::sHandler->getUiManager();
                Q_ASSERT(mUimanager);
                mUimanager->command(UI_CMD_SET_FRIMWARE, mTempInfo);
                mUimanager->command(UI_CMD_SET_ROUTE, mTempInfo);

                mUimanager->command(UI_CMD_HIDE_UPDATE_BUTTON, "");

                CurrentModel = mTempInfo;
                CurrentFirmware = mTempInfo;

                getStart = false;
            }
        }

        wlog += "Not NETGEAR router. Get over";
    }

    GeniePlugin_Info::writeLog(wlog);

    if (sendEmit)
        emit GetFirmwareFinish(err);

//    getFlag = true;
}




QString GetInformation::getNetgearMAC()
{
    return routerMAC;

}

QString GetInformation::getProductType()
{
    return CurrentModel;
}

QString GetInformation::getFirmwareVersion()
{
    return CurrentFirmware;
}

QString GetInformation::getSupportLevel()
{
    return CurrentSupportLevel;
}

void GetInformation::getCurrentVersion()
{
    //////2010-12-07 Jansion.zhou
//    IInfoManager *mInfoManager = sHandler->getInfoManager();
//    Q_ASSERT(mInfoManager);
//    QString strVersion;
//    QString strModel;
//    strModel = mInfoManager->getInfo(INFO_CMD_ROUTE_TYPE_IMMEDIATE);
//    strVersion = mInfoManager->getInfo(INFO_CMD_FIRMWARE_IMMEDIATE);
//
//    QStringList strTempArray;
//    if (strModel.isEmpty())
//        CurrentModel.clear();
//    else{
//        strTempArray = strModel.split(" ");
//        if (strTempArray.size()>2)
//            CurrentModel = strTempArray[2];
//    }
//    strTempArray.clear();
//    if (strVersion.isEmpty())
//        CurrentFirmware.clear();
//    else{
//        strTempArray = strVersion.split(" ");
//        if (strTempArray.size()>2)
//            CurrentFirmware = strTempArray[2];
//    }
//
//    QString tmpLog;
//    tmpLog = "Current router information, Model=" + CurrentModel + ", Version=" + CurrentFirmware;
//    GeniePlugin_Info::writeLog(tmpLog);
//    qDebug() << "Current router information:" << CurrentModel << "-->" << CurrentFirmware;

}

void GetInformation::isFirmwareUpgrade()
{
    if ( checkFlag ){
        checkFlag = false;
        mTimer->start(upgradeSec);
        GeniePlugin_Info::writeLog("The NETGEAR router update after 6 mins.");
    }
}

void GetInformation::setUpgrade()
{
    QString wlog;
    wlog = "Start the update check, isNetgear=" + netgearFlag + ", ";

    IUiManager *mUiPage = sHandler->getUiManager();
    Q_ASSERT(mUiPage);
    QString mUiPara;
    mUiPara = "";
    if ( netgearFlag == "y" ){
        qDebug() << "Time out Y";
        QString mRegionTag;
        if (ProductInfo->value("Region").size()>2)
            mRegionTag = ProductInfo->value("Model") + "_ww";
        else
            mRegionTag = ProductInfo->value("Model") + "_" + ProductInfo->value("Region");
        qDebug() << "Url: " << mRegionTag;
        //mRegionTag = "wnr3500l_us";


        QString oldVersion;
        oldVersion = ProductInfo->value("Firmware");
        newVersion.clear();
        int j = 0;
        try{
            mNetgearInfo->getNewFirmware(&newFirmwareInfo, mRegionTag);
        }catch(...){
            GeniePlugin_Info::writeLog("Start the update check.");
            Q_ASSERT(0);
        }


        while (j++ < 60){
            if (newVersion.isEmpty()){
                QTest::qWait(1000);
                qDebug() << "Set upgrade time's No." << j;
            }
        }

        wlog = wlog + "lastest version:" + newVersion + ", current version:" + ProductInfo->value("Firmware");
        qDebug() << "Check Version Number: " << newVersion << ProductInfo->value("Firmware");

        if ( isUpgrade(oldVersion, newVersion) ){
            //IUiManager *mUiPage = sHandler->getUiManager();
            //Q_ASSERT(mUiPage);
            //QString mUiPara;
            //mUiPara = "";
            mUiPage->command(UI_CMD_SHOW_UPDATE_BUTTON, mUiPara);

            if (mTimer->isActive())
                mTimer->stop();

        }else {
            mUiPage->command(UI_CMD_HIDE_UPDATE_BUTTON, mUiPara);

            if (mTimer->isActive())
                mTimer->stop();
        }

    }else {
        wlog += "Not Netgear router.";
        mUiPage->command(UI_CMD_HIDE_UPDATE_BUTTON, mUiPara);
    }

    checkFlag = true;

    GeniePlugin_Info::writeLog(wlog);

}

void GetInformation::dealVersionData(bool err)
{
    //qDebug() << "deal Version!";
    if ( err ){
        newVersion.clear();
        newVersion = newFirmwareInfo->value(0);
        //QString tempLog = "The latest version of the official:" + newVersion;
        //GeniePlugin_Info::writeLog(tempLog);

        wDownloadURL.clear();
        wDownloadURL = newFirmwareInfo->value(1);
        qDebug() << "While download url is: " << wDownloadURL;

        twoCompareData.clear();
        twoCompareData = newFirmwareInfo->value(2)+":"+newFirmwareInfo->value(3);
        qDebug() << "Compare Data: " << twoCompareData;
    }
}

bool GetInformation::isUpgrade(QString oldV, QString newV)
{
    //////remove "V" form version
    //QRegExp vNumMatch("^V(\\d+.*\\d+)$");
    QRegExp vNumMatch("^\\D+(\\d+(\\.\\d+)*).*");
    int verpos = vNumMatch.indexIn(oldV);
    if (verpos > -1){
        oldV = vNumMatch.cap(1);
    }
    verpos = vNumMatch.indexIn(newV);
    if (verpos > -1){
        newV = vNumMatch.cap(1);
    }

    QStringList nowVersionNum = oldV.split(QRegExp("\\."), QString::SkipEmptyParts);
    QStringList newVersionNum = newV.split(QRegExp("\\."), QString::SkipEmptyParts);

    int newNum;
    int nowNum;
    QString strNum;
    bool oknew;
    bool oknow;
    int complenght;
    int nowVerL = nowVersionNum.size();
    int newVerL = newVersionNum.size();
    if ( newVerL<nowVerL ){
        complenght = newVerL;

        for (int k=nowVerL-newVerL; k>0; k--){
            if (!nowVersionNum.isEmpty())
                nowVersionNum.pop_front();
        }
    }else {
        complenght = nowVerL;

        for (int k=newVerL-nowVerL; k>0; k--){
            if (!newVersionNum.isEmpty())
                newVersionNum.pop_front();
        }
    }
    for (int i=0; i<complenght; i++){

        //////2010-10-25 jansion
//        if (i==0){
//            QRegExp vNumMatch("^V(\\d+)$");
//            int newpos = vNumMatch.indexIn(newVersionNum[i]);
//            if (newpos > -1){
//                strNum = vNumMatch.cap(1);
//                newNum = strNum.toInt(&oknew, 10);
//            }
//            int nowpos = vNumMatch.indexIn(nowVersionNum[i]);
//            if (nowpos > -1){
//                strNum = vNumMatch.cap(1);
//                nowNum = strNum.toInt(&oknow, 10);
//            }
//
//            if (oknew && oknow){
//                if (newNum>nowNum)
//                    return true;
//            }
//        }else {
//            strNum = newVersionNum[i];
//            newNum = strNum.toInt(&oknew, 10);
//            strNum = nowVersionNum[i];
//            nowNum = strNum.toInt(&oknow, 10);
//
//            if (oknew && oknow){
//                if (newNum>nowNum)
//                    return true;
//            }
//        }
        //////



        strNum = newVersionNum[i];
        newNum = strNum.toInt(&oknew, 10);
        strNum = nowVersionNum[i];
        nowNum = strNum.toInt(&oknow, 10);

        if (oknew && oknow){
            if (newNum>nowNum){
                GeniePlugin_Info::writeLog("Need upgared firmware to:" + newV);
                return true;
            }
        }

    }

    return false;

}

void GetInformation::setTimingUpgrade()
{
    if ( !timingFlag )
        timingFlag = true;

    QTime currentTime = QTime::currentTime();

    int mMinute;
    qsrand(currentTime.second());
    mMinute = qrand()%60;
    qDebug() << "The second time: " << mMinute;
    QTime updateTime(11, mMinute, mMinute);
    //QTime updateTime(15, 52, 10);

    int mSec = currentTime.secsTo(updateTime);
    if ( mSec <= 0 )
        mSec += 86400;

    mSec *= 1000;
    qDebug() << "The second time: " << mSec;
    mTimingTime->start(mSec);

    GeniePlugin_Info::writeLog("Set daily check for updates regularly.");

}

void GetInformation::isTimingUpgrade()
{
    if ( timingFlag ){
        mTimingTime->stop();
        mTimingTime->start(mDaySec);

        timingFlag = false;
    }

    //qDebug() << "It's timing to Firmware upgarde!";
    if ( checkFlag ){
        checkFlag = false;
        GeniePlugin_Info::writeLog("Regularly check for updates daily.");
        setUpgrade();
    }

}

QString GetInformation::getCompareVersion()
{
    QString twoVersion;
    twoVersion = ProductInfo->value("Firmware") + ":" + newVersion;
    return twoVersion;
}

QString GetInformation::getDownloadUrl()
{
    return wDownloadURL;
}

QString GetInformation::getNetgearCurrentset()
{
    QString mWholeInfo = ProductInfo->value("WholeInfo");
    qDebug() << "Return Currentseting:" << mWholeInfo;
    return mWholeInfo;
}

QString GetInformation::getCompareData()
{
    return twoCompareData;
}

void GetInformation::AbortGetHttp()
{
    mNetgearInfo->AbortRouterHttp();
}





void GetInformation::NetgearLookup()
{
    int intReturn = QHostInfo::lookupHost(mNetgearHost, this, SLOT(GetLookupData(QHostInfo)));
    qDebug() << "Return int from DNS lookup:" << intReturn;

}

void GetInformation::GetLookupData(const QHostInfo &Host)
{

    if (Host.error() != QHostInfo::NoError) {
        //mGatewayIp = "0.0.0.0";

        qDebug() << "Lookup failed:" << Host.errorString();
        emit DnsLookupFinish(false);
    }else {
        foreach (QHostAddress address, Host.addresses())
            mGatewayIp = address.toString();

        //qDebug() << "Found address:" << address.toString();
        emit DnsLookupFinish(true);
    }

    //emit DnsLookupFinish(true);
}

void GetInformation::StartGetProduct(bool err)
{
    if (err){
        mNetgearInfo->getProductInfo(mGatewayIp, &ProductInfo, &rErr);
    }else{
        GeniePlugin_Info::writeLog("Not find \"routerlogin.net\".");
    }
}




