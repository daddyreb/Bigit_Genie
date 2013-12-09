#include <QtGui>
#include <qhttp.h>

#include "netgearinfo.h"
//#include "httpget.h"

//#include "QGenieDialogContainerFrame.h"
//#include "RouterPasswordWidget.h"

#include "GeniePlugin_Info.h"


NetgearInfo::NetgearInfo(QObject *parent)
    :QObject(parent)
{
    isGetFlag = true;

    authenFlag = false;
    authenNum = 2;
    //strUrl = "http://www.routerlogin.net/currentsetting.htm";
    strUrl = "http://routerlogin.net/currentsetting.htm";
    mGetUrl = QUrl(strUrl);
    Requestheader = QHttpRequestHeader("GET", mGetUrl.path());
    Requestheader.setValue("Connection", "Keep-Alive");
    Requestheader.setValue("Host", mGetUrl.host());
    //////Authorization Base64
    //QString mPassword = GeniePlugin_Info::getLoginPassword();
    //QString credentials = "admin:" + mPassword;
    //QString credentials = "admin:passwor";
    //credentials = "Basic " + credentials.toAscii().toBase64();
    //Requestheader.addValue("Authorization", credentials.toAscii());


    //////under QHttp::get() function
    connect(&infoHttp, SIGNAL(requestFinished(int,bool)),
            this, SLOT(dealRequestFinish(int,bool)));

    connect(&infoHttp, SIGNAL(readyRead(QHttpResponseHeader)),
            this, SLOT(dealResponseData(QHttpResponseHeader)));

    getCount = 0;
    getTime = 20000;
    mGetTimer = new QTimer();
    Q_ASSERT(mGetTimer);
    //mGetTimer->setSingleShot(true);
    connect(mGetTimer, SIGNAL(timeout()), this, SLOT(dealGetTimeout()));

    waitGetTime = 25000;
    mWaitGetTimer = new QTimer();
    Q_ASSERT(mWaitGetTimer);
    connect(mWaitGetTimer, SIGNAL(timeout()), this, SLOT(dealGetWaitTimeout()));

    intLoopTime = 60000;
    //intLoopTime = 8000;
    mLoopGetTimer = new QTimer();
    Q_ASSERT(mLoopGetTimer);
    connect(mLoopGetTimer, SIGNAL(timeout()), this, SLOT(loopGetRouteInfo()));





    //checkNum = 3;
    mHttpFlag = true;
    isCheckFlag = true;
    strRootUrl = "http://updates1.netgear.com";
    strFileName = "fileinfo.txt";
    connect(&verHttp, SIGNAL(done(bool)), this, SLOT(dealFileData(bool)));

    checkCount = 1;
    checkTime = 20000;
    mCheckTimer = new QTimer();
    Q_ASSERT(mCheckTimer);
    //mCheckTimer->setSingleShot(true);
    connect(mCheckTimer, SIGNAL(timeout()), this, SLOT(CheckTimingDeal()));

    waitCheckTime = 30000;
    mWaitCheckTimer = new QTimer();
    Q_ASSERT(mWaitCheckTimer);
    connect(mWaitCheckTimer, SIGNAL(timeout()), this, SLOT(CheckWaitOut()));

}

NetgearInfo::~NetgearInfo()
{
    //mHttp.close();
}

void NetgearInfo::CloseHttpRequest()
{
    disconnect(&infoHttp, 0, 0, 0);
    disconnect(&verHttp, 0, 0, 0);
    infoHttp.abort();
    verHttp.abort();

    delete mGetTimer;
    delete mWaitGetTimer;
    delete mLoopGetTimer;

    delete mCheckTimer;
    delete mWaitCheckTimer;
}


//void NetgearInfo::getProductInfo(QMap<QString, QString>* *info, QString* *err)
void NetgearInfo::getProductInfo(QString hostip, QMap<QString, QString>**info, QString**err)
{
    if (*info == NULL){
        //*info = new QMap<QString, QString>;
        return;
    }
    if (*err == NULL){
        //*err = new QString();
        return;
    }

    strLog.clear();
    if ( isGetFlag ){
        isGetFlag = false;

        authenFlag = false;

        dealCount = getCount;
        mGetTimer->start(getTime);


        reInfo = *info;
        reInfo->clear();
        strError = *err;
        strError->clear();

        //infoHttp.setHost(mGetUrl.host(), mGetUrl.port(80));
        infoHttp.setHost(hostip, mGetUrl.port(80));

        //////Use QHttp::get() function
        ////infoHttp.setUser("admin", "password");
        //infoHttp.get(mGetUrl.path());

        //////Use QHttp::request() function
        requestId = infoHttp.request(Requestheader,0,0);

        strLog = "Send http request to " + hostip;
        qDebug() << "Send Http request, and ID=" << requestId;

    }else {
        mWaitGetTimer->start(waitGetTime);

        tempHostip = hostip;
        tempInfo = *info;
        tempInfo->clear();
        tempError = *err;
        tempError->clear();

        strLog = "Waiting, other http request was sended.";

    }

    GeniePlugin_Info::writeLog(strLog);
}

void NetgearInfo::getProductInfo()
{

    //isGetFlag = false;

    reInfo->clear();
    strError->clear();

    //////Use QHttp::get function
    //infoHttp.get(mGetUrl.path());

    //////Use QHttp::request() function
    requestId = infoHttp.request(Requestheader,0,0);

    qDebug() << "Send again Http request, and ID=" << requestId;
    GeniePlugin_Info::writeLog("Send http request again when it's timeout.");

}

void NetgearInfo::dealGetTimeout()
{
    qDebug() << "Get router information - Http Timeout.";
    GeniePlugin_Info::writeLog("Get router information - Http Timeout.");

    infoHttp.abort();
    //isGetFlag = true;

    if ( dealCount==0 ){
        if (mGetTimer->isActive())
            mGetTimer->stop();
        isGetFlag = true;

        /////Get router firmware failed
        strError->append("Timeout");

        emit GetFinish(false);
    }else {
        dealCount--;

        getProductInfo();
    }

}

void NetgearInfo::dealGetWaitTimeout()
{
    if ( isGetFlag ){
        qDebug() << "Waiting Over -- get router firmware!";
        mWaitGetTimer->stop();
        //infoHttp.abort();

        GeniePlugin_Info::writeLog("Waiting over, get permission.");
        getProductInfo(tempHostip, &tempInfo, &tempError);
    }
}

void NetgearInfo::loopGetRouteInfo()
{
    getProductInfo();
}

void NetgearInfo::dealRequestFinish(int id, bool err)
{

    if ( id == requestId ){
        qDebug() << "Finish Request http's Id: " << id;
        qDebug() << "Finish Request Http's Status --" << infoHttp.state();
        qDebug() << "Finish Request Http's Error --" << infoHttp.error();
        qDebug() << "Finish Response header's code: " << infoHttp.lastResponse().statusCode();


        QString w_strLog;
        w_strLog = "Finish response(last) header's code:" + QString::number(infoHttp.lastResponse().statusCode(), 10);
        w_strLog += ", http's status:" + QString::number(infoHttp.state(), 10);
        w_strLog += ", http's error:" + QString::number(infoHttp.error(), 10);
        GeniePlugin_Info::writeLog(w_strLog);

        if ( err ){

            switch (infoHttp.error()){
            case QHttp::HostNotFound:
                {
                    qDebug() << "The host name lookup failed.";
                    if (mGetTimer->isActive())
                        mGetTimer->stop();
                    if (mLoopGetTimer->isActive())
                        mLoopGetTimer->stop();
                    strError->clear();
                    strError->append("The host name lookup failed.");

                    infoHttp.abort();
                    isGetFlag = true;

                    GeniePlugin_Info::writeLog("Request result: The host name lookup failed.");
                    emit GetFinish(false);
                    break;
                }
            case QHttp::Connected:
                {
                    if (mGetTimer->isActive())
                        mGetTimer->stop();

                    mLoopGetTimer->start(intLoopTime);
                    break;
                }
            default:
                {
                    qDebug() << "No about router information.";
                    break;
                }
            }

        }else {
            switch (infoHttp.state()){
            case QHttp::Unconnected:
                {
                    //if (mGetTimer->isActive())
                    //    mGetTimer->stop();
                    //if (mLoopGetTimer->isActive())
                    //    mLoopGetTimer->stop();

                    qDebug() << "There is no connection to the host.";
                    //emit GetFinish(false);

                    break;
                }
            default:
                {
                    qDebug() << "There isn't HTTP request worng.";
                    break;
                }
            }

        }

    }

}

void NetgearInfo::dealResponseData(QHttpResponseHeader resp)
{

    qDebug() << "Response header's code: " << resp.statusCode();
    qDebug() << "Request Http's Status --" << infoHttp.state();
    qDebug() << "Request Http's Error --" << infoHttp.error();

    QString w_strLog;
    w_strLog = "Response header's code:" + QString::number(resp.statusCode(), 10);
    w_strLog += ", http's status:" + QString::number(infoHttp.state(), 10);
    w_strLog += ", http's error:" + QString::number(infoHttp.error(), 10);
    GeniePlugin_Info::writeLog(w_strLog);

    w_strLog.clear();
    w_strLog = "Request result:";
    if (resp.statusCode()==200){
        QByteArray mHttpInfo;
        mHttpInfo = infoHttp.readAll();
        QString strInfo;
        strInfo = mHttpInfo.constData();
        //////Test
        //strInfo = "Firmware=V1.0.4.38-new-GUI-test-009WW RegionTag=WNDR3700_WW Region=ww Model=WNDR3700 InternetConnectionStatus=Up ParentalControlSupported=1SOAPVersion";
        //qDebug() << "Response Data: " << strInfo;

        QRegExp reMatch("^Firmware.*RegionTag.*Region.*Model.*");
        //QRegExp ipMatch(".*\((\\d+\\.){3}\\d+\).*");
        if ( reMatch.exactMatch(strInfo) ){
            if (mGetTimer->isActive())
                mGetTimer->stop();
            if (mLoopGetTimer->isActive())
                mLoopGetTimer->stop();

            QStringList InfoArray = strInfo.split(QRegExp("\\s+"), QString::SkipEmptyParts);
            QStringList TempArray;
            reInfo->clear();
            reInfo->insert("WholeInfo", strInfo);
            for (int i=0; i<InfoArray.size(); i++){
                TempArray.clear();
                TempArray = InfoArray[i].split("=");
                if (TempArray.size()>1){
                    if (TempArray[0]=="Firmware"){
                        //////Test setting version
                        //TempArray[1] = "V1.0.4.38-new";

                        //////Modify The Version's Information
                        //QRegExp vInfoMatch("(V\\d+(\\.\\d+){3}).*");
                        //QRegExp vInfoMatch("(V\\d+(\\.\\d+)*).*");
                        QRegExp vInfoMatch("(\\D+\\d+(\\.\\d+)*).*");
                        int pos = vInfoMatch.indexIn(TempArray[1]);
                        if (pos > -1){
                            reInfo->insert(TempArray[0], vInfoMatch.cap(1));
                            qDebug() << "Current Version Match: " << vInfoMatch.cap(1);
                        }else {
                            reInfo->insert(TempArray[0], TempArray[1]);
                        }
                        w_strLog += " "+TempArray[0]+"="+TempArray[1];
                    }else {
                        reInfo->insert(TempArray[0], TempArray[1]);
                        w_strLog += " "+TempArray[0]+"="+TempArray[1];
                    }
                }
                //qDebug() << InfoArray.value(i) << "-->" << TempArray[1];
            }

            //infoHttp.abort();
            isGetFlag = true;

            GeniePlugin_Info::writeLog(w_strLog);
            emit GetFinish(true);

        }else {
            if (mGetTimer->isActive())
                mGetTimer->stop();
            if (mLoopGetTimer->isActive())
                mLoopGetTimer->stop();

            qDebug() << "It isn't Netgear firmware information";
            w_strLog += "Not NETGEAR router.";
            strError->append("Not Netgear's Wireless Router");

            //infoHttp.abort();
            //infoHttp.close();
            isGetFlag = true;

            GeniePlugin_Info::writeLog(w_strLog);
            emit GetFinish(false);

        }

    }else {
        if (mGetTimer->isActive())
            mGetTimer->stop();
        if (mLoopGetTimer->isActive())
            mLoopGetTimer->stop();

        w_strLog += "No Netgear router, or Network.";
        qDebug() << "Not Netgear's Wireless Route, or Not Network connections.";
        strError->append("Not Netgear's Wireless Route, or Not Network connections.");

        //infoHttp.abort();
        isGetFlag = true;

        GeniePlugin_Info::writeLog(w_strLog);
        emit GetFinish(false);

    }

    qDebug() << "Get firmware version finished!";

}



























void NetgearInfo::getNewFirmware(QStringList**newinfo, QString mPara)
{
    if (*newinfo == NULL){
        //*newinfo = new QStringList;
        return;
    }
    if (mPara.isEmpty())
        return;

    if ( isCheckFlag ){
        isCheckFlag = false;

        checkCount = 1;
        mCheckTimer->start(checkTime);

        mVersion = *newinfo;
        mVersion->clear();

        mHttpFlag = true;

        strCheckUrl.clear();
        mPara.replace(QString("_"), QString("/"));
        middleUrl.clear();
        middleUrl = mPara;
        middleUrl = middleUrl.toLower();
        strCheckUrl = strRootUrl + "/" + mPara + "/" + strFileName;
        strCheckUrl = strCheckUrl.toLower();
        //strCheckUrl = "http://updates1.netgear.com/wnr1000v2/wo/fileinfo.txt";
        qDebug() << "Download URL: " << strCheckUrl;
        mCheckUrl.clear();
        mCheckUrl = QUrl(strCheckUrl);


        checkFile.clear();
        checkFile = QFileInfo(mCheckUrl.path()).fileName();
        if (checkFile.isEmpty())
            checkFile = "fileinfo.txt";
        //////use system's temp path
        DownloadPath.clear();
        DownloadPath = getDownloadPath();
        DownloadPath.append(checkFile);

        if ( mFile.isOpen() )
            mFile.close();
        //mFile.setFileName(checkFile);
        mFile.setFileName(DownloadPath);

        if (!mFile.open(QIODevice::WriteOnly)){
            qDebug() << "Error: Cannot write file " << mFile.fileName()
                     << ": " << mFile.errorString();
            return;
        }

        //////use file
        GeniePlugin_Info::writeLog("Send http request to get lastest firmware. Version info:" + DownloadPath);
        verHttp.setHost(mCheckUrl.host(), mCheckUrl.port(80));
        verHttp.get(mCheckUrl.path(), &mFile);

    }else {
        mWaitCheckTimer->start(waitCheckTime);

        tempVersion = *newinfo;
        tempPara = mPara;

        GeniePlugin_Info::writeLog("Waiting...get the lastest firmware.");

    }

}

void NetgearInfo::getNewFirmware()
{
    //mCheckTimer->start(checkTime);
    //isCheckFlag = false;

    mVersion->clear();

    mHttpFlag = true;

    //strCheckUrl = "http://updates1.netgear.com/wnr1000v2/wo/fileinfo.txt";
    qDebug() << "Download URL: " << strCheckUrl;
    mCheckUrl.clear();
    mCheckUrl = QUrl(strCheckUrl);


    if ( mFile.isOpen() )
        mFile.close();
    //mFile.setFileName(checkFile);
    mFile.setFileName(DownloadPath);
    if (!mFile.open(QIODevice::WriteOnly)){
        return;
    }

    GeniePlugin_Info::writeLog("Send http request to get lastest firmware again.");
    //verHttp.setHost(mCheckUrl.host(), mCheckUrl.port(80));
    verHttp.get(mCheckUrl.path(), &mFile);

}

void NetgearInfo::CheckTimingDeal()
{
    qDebug() << "reget lastest firmware version!";
    //verHttp.abort();

    if ( checkCount==0 ){
        if (mCheckTimer->isActive())
            mCheckTimer->stop();
        isCheckFlag = true;

        mVersion->push_back("NULL");
        emit CheckFinish(false);

    }else {
        checkCount--;

        getNewFirmware();
    }

}

void NetgearInfo::CheckWaitOut()
{
    //isCheckFlag = true;
    if ( isCheckFlag ){
        qDebug() << "Waiting Get the lastest firmware version!";
        if (mWaitCheckTimer->isActive())
            mWaitCheckTimer->stop();
        //verHttp.abort();

        getNewFirmware(&tempVersion, tempPara);
    }

}

void NetgearInfo::dealFileData(bool err)
{
    if (mHttpFlag){
        qDebug() << "Check Http State: " << verHttp.state();
        qDebug() << "Check Http Error: " << verHttp.error();

        strLog.clear();
        strLog = "Check lastest firmware:";
        if (verHttp.state() == QHttp::Connected){
            //////use system's path
            //QString strFilePath = "./" + checkFile;
            QString strFilePath = DownloadPath;

            qDebug() << "Local File Path: " << strFilePath;
            QDir delFile;

            if (!err){
                mFile.close();

                QStringList VersionArray;
                QFile file(strFilePath);
                if ( file.open(QIODevice::ReadOnly) ){
                    QTextStream stream(&file);
                    QString line;
                    QRegExp spaceMatch("\\s*");
                    while ( !stream.atEnd() ){
                        line = stream.readLine();
                        if ( !spaceMatch.exactMatch(line) ){
                            //qDebug() << "File Info: " << line;
                            //VersionArray += line;
                            VersionArray.push_back(line);
                        }
                    }
                    file.close();

                    QRegExp FirstMatch(".*Major.*");
                    if ( FirstMatch.exactMatch(VersionArray.value(0)) ){

                        QStringList TempArray;
                        QMap<QString, QString> returnInfo;
                        QRegExp DataMatch("^\\[.*language.*\\]$");
                        QRegExp mDataMatch("^\\[.*\\]$");
                        for (int i=0; i<VersionArray.size(); i++){
                            if (DataMatch.exactMatch(VersionArray[i]))
                                break;

                            if ( !mDataMatch.exactMatch(VersionArray[i]) ){
                                TempArray.clear();
                                TempArray = VersionArray[i].split("=");
                                //qDebug() << TempArray[0] << " -- " << TempArray[1];
                                if (TempArray.size()>1)
                                    returnInfo.insert(TempArray[0], TempArray[1]);
                            }
                        }

                        QString strTemp;
                        strTemp = returnInfo.value("file");
                        if ( strTemp != NULL ){

                            //QRegExp vMatchVer(".*-(V\\d+.*\\d+).*(chk|img)$");
                            QRegExp vMatchVer(".*-(\\D+\\d+(\\.\\d+)*).*(chk|img)$");
                            int pos;
                            pos = vMatchVer.indexIn(strTemp);
                            if (pos > -1){
                                qDebug() << "Match: " << vMatchVer.cap(1);
                                mVersion->push_back(vMatchVer.cap(1));
                                //returnInfo->insert("firmware", vMatchVer.cap(1));
                            }else {
                                mVersion->push_back("NULL");
                            }
                        }else
                            mVersion->push_back("NULL");


                        wDownloadUrl.clear();
                        QRegExp urlMatch(".*"+middleUrl+".*");
                        //qDebug() << "Url File: " << middleUrl << "-" << strTemp;
                        if ( urlMatch.exactMatch(strTemp) ){
                            wDownloadUrl = strRootUrl + strTemp;
                        }else {
                            wDownloadUrl = strRootUrl + "/" + middleUrl + "/" + strTemp;
                        }
                        //wDownloadUrl = wDownloadUrl.toLower();
                        mVersion->push_back(wDownloadUrl);
                        //qDebug() << "Last Url File: " << wDownloadUrl;

                        mVersion->push_back(returnInfo.value("size"));
                        mVersion->push_back(returnInfo.value("md5"));

                        mHttpFlag = false;
                        delFile.remove(strFilePath);

                        strLog += "Get the lastest firmware.";
                        if (mCheckTimer->isActive())
                            mCheckTimer->stop();
                        isCheckFlag = true;

                        GeniePlugin_Info::writeLog(strLog);

                        emit CheckFinish(true);

                    }else {

                        mHttpFlag = false;
                        delFile.remove(strFilePath);
                        strLog += "Data isn't match.";

                        mVersion->push_back("NULL");
                        if (mCheckTimer->isActive())
                            mCheckTimer->stop();
                        isCheckFlag = true;

                        GeniePlugin_Info::writeLog(strLog);

                        emit CheckFinish(false);
                    }

                }else {
                    qDebug() << "This is worng while Open the Download's check file.";
                    //intErrList->push_back(L_UPGRADE_CHECK_ERR_OPENFILE);
                    strLog += "It's worng to open file.";
                    mHttpFlag = false;
                    delFile.remove(strFilePath);

                    mVersion->push_back("NULL");
                    if (mCheckTimer->isActive())
                        mCheckTimer->stop();
                    isCheckFlag = true;

                    GeniePlugin_Info::writeLog(strLog);

                    emit CheckFinish(false);
                }

            }else {
                mFile.close();
                delFile.remove(strFilePath);

                strLog += "The header response is error.";
                qDebug() << "Get New Version Info Failed!";

                mHttpFlag = false;
                mVersion->push_back("NULL");
                if (mCheckTimer->isActive())
                    mCheckTimer->stop();
                isCheckFlag = true;

                GeniePlugin_Info::writeLog(strLog);

                emit CheckFinish(false);
            }

        }else {
            mHttpFlag = false;
            qDebug() << "Http Error: " << verHttp.errorString();
            //mHttp.close();
            //mVersion->push_back(verHttp.errorString());
            strLog += "The request is disconnect.";

            mVersion->push_back("NULL");

            if (mCheckTimer->isActive())
                mCheckTimer->stop();
            isCheckFlag = true;

            GeniePlugin_Info::writeLog(strLog);

            emit CheckFinish(false);
        }

    }

    //verHttp.abort();
    qDebug() << "Get the lastest firmware finished!";
}

QString NetgearInfo::getDownloadPath()
{
    QString returnDir;
    QString tempHomeDir;
    QString childDir;
    QDir mytempDir;

    tempHomeDir = QDir::home().absolutePath()+QString("/NETGEARGenie");
    childDir = tempHomeDir + QString("/update_temp/");

//    if (QDir::home().exists(tempHomeDir)){
//        if (QDir::home().exists(childDir)){
//            returnDir = mytempDir.toNativeSeparators(childDir);
//        }else {
//            QDir(tempHomeDir).mkdir(childDir);
//            returnDir = mytempDir.toNativeSeparators(childDir);
//        }
//    }else {
//        QDir::home().mkdir(tempHomeDir);

//        if (QDir::home().exists(childDir)){
//            returnDir = mytempDir.toNativeSeparators(childDir);
//        }else {
//            QDir(tempHomeDir).mkdir(childDir);
//            returnDir = mytempDir.toNativeSeparators(childDir);
//        }
//    }

    if ( !QDir::home().exists(tempHomeDir) )
        QDir::home().mkdir(tempHomeDir);

    if (!QDir::home().exists(childDir))
        QDir(tempHomeDir).mkdir(childDir);

    returnDir = mytempDir.toNativeSeparators(childDir);

    return returnDir;

}

void NetgearInfo::AbortRouterHttp()
{
    infoHttp.abort();
}




