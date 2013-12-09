#include <QtGui>
#include <qhttp.h>

#include "infofirmware.h"
#include "genieplugin_firmwareupdate.h"
#include "LanguageFirmwareUpdate.h"


InfoFirmware::InfoFirmware()
{

    //strUrl = "http://www.routerlogin.net/currentsetting.htm";
    strUrl = "http://routerlogin.net/currentsetting.htm";
    //strUrl = "http://routerlogin.net/index.htm";

    //connect(&mHttp, SIGNAL(done(bool)), this, SLOT(GetFirmware(bool)));


    mUrl = QUrl(strUrl);
    Requestheader = QHttpRequestHeader("GET", mUrl.path());
    Requestheader.setValue("Connection", "Keep-Alive");
    Requestheader.setValue("Host", mUrl.host());

    connect(&mHttp, SIGNAL(requestFinished(int,bool)),
            this, SLOT(dealRequestFinish(int,bool)));
    connect(&mHttp, SIGNAL(readyRead(QHttpResponseHeader)),
            this, SLOT(dealResponseData(QHttpResponseHeader)));


    getNumber = 1;
    delaySec = 10000;
    delayTimer = new QTimer();
    delayTimer->setSingleShot(false);
    connect(delayTimer, SIGNAL(timeout()), this, SLOT(dealDelayTime()));

}

InfoFirmware::~InfoFirmware()
{
    //mHttp.close();
}

void InfoFirmware::GetInfo(QMap<QString, QString>* *VerInfo, QList<int>* *err)
{
    getNumber = 1;
    delayTimer->start(delaySec);

    //qDebug() << "GetSysInfo Http Start " << mHttp.state();
    //qDebug() << "Http Url request: " << strUrl;

    if (*VerInfo == NULL)
        *VerInfo = new QMap<QString, QString>;
    returnInfo = *VerInfo;
    returnInfo->clear();
    if (*err == NULL)
        *err = new QList<int>;
    intList = *err;
    intList->clear();


    //mUrl.clear();
    //mUrl = QUrl(strUrl);
    //mHttp.setHost(mUrl.host(), mUrl.port(80));
    //mHttp.get(mUrl.path());

    mHttp.setHost(mUrl.host(), mUrl.port(80));
    requestId = mHttp.request(Requestheader,0,0);

    strlog.clear();
    strlog = "Start send http request to getting information.";
    GeniePlugin_FirmwareUpdate::writeGenieLog(strlog);

    qDebug() << "Send get firmware Http request of first ID:" << requestId;

}

void InfoFirmware::GetInfo()
{
    //delayTimer->start(delaySec);

    //qDebug() << "GetSysInfo Http Start " << mHttp.state();
    //qDebug() << "Http Url request: " << strUrl;

    returnInfo->clear();
    intList->clear();

    //mUrl.clear();
    //mUrl = QUrl(strUrl);
    ////mHttp.setHost(mUrl.host(), mUrl.port(80));
    //mHttp.get(mUrl.path());

    requestId = mHttp.request(Requestheader,0,0);

    strlog.clear();
    strlog = "Try send http request to getting information again.";
    GeniePlugin_FirmwareUpdate::writeGenieLog(strlog);

    qDebug() << "Send Http request for get firmware again ID" << requestId;

}

void InfoFirmware::GetFirmware(bool error)
{
    //delayTime->stop();

    qDebug() << "Get Firmware Info Http's Status --" << mHttp.state();
    qDebug() << "Get Firmware Info Http's Error --" << mHttp.error();
    //if (mHttp.state() != QHttp::Unconnected){
    //    qDebug() << "Http Unconnected!";
    //    strError->append("Internet Connect Failed");
    //}else {
        if (!error){
            qDebug() << "GetSysInfo Success!" << mHttp.state();
            QByteArray mHttpInfo;
            mHttpInfo = mHttp.readAll();
            QString strInfo = mHttpInfo.constData();
            //mHttp.close();

            QRegExp reMatch("^Firmware.*RegionTag.*Region.*Model.*");
            if ( reMatch.exactMatch(strInfo) ){

                QStringList InfoArray = strInfo.split(QRegExp("\\s+"), QString::SkipEmptyParts);
                QStringList TempArray;
                returnInfo->clear();
                for (int i=0; i<InfoArray.size(); i++){
                    TempArray.clear();
                    TempArray = InfoArray[i].split("=");
                    if (TempArray[0]=="Firmware"){
                        //////Modify The Version's Information
                        //QRegExp vInfoMatch("(V\\d+(\\.\\d+){3}).*");
                        QRegExp vInfoMatch("(V\\d+(\\.\\d+)*).*");
                        int pos = vInfoMatch.indexIn(TempArray[1]);
                        if (pos > -1){
                            returnInfo->insert(TempArray[0], vInfoMatch.cap(1));
                            qDebug() << "Current Version Match: " << vInfoMatch.cap(1);
                        }else{
                            returnInfo->insert(TempArray[0], TempArray[1]);
                        }
                    }else {
                        returnInfo->insert(TempArray[0], TempArray[1]);
                    }
                    //qDebug() << InfoArray.value(i) << "-->" << TempArray[1];
                }


                //qDebug() << "Http Info: " << strInfo;
                //emit GetSuccess();
                emit GetFinish(true);

            }else{
                //qDebug() << "Error";
                //strError->clear();
                //strError->append("Not Netgear's Wireless Route.");
                intList->clear();
                intList->push_back(L_UPGRADE_PRODUCT_NONETGEAR);

                //emit GetFailed();
                emit GetFinish(false);
            }

        }else {
            //qDebug() << "GetSysInfo Failed!";
            //mHttp.close();
            //strError->clear();
            //strError->append("Not Netgear's Wireless Route, or Not Network connections.");
            intList->clear();
            intList->push_back(L_UPGRADE_PRODUCT_UNCONNECT);

            //emit GetFailed();
            emit GetFinish(false);
        }

    //}

    delayTimer->stop();
    mHttp.abort();

}

void InfoFirmware::dealDelayTime()
{
    strlog.clear();
    strlog = "It's delay timeout:";
    if ( returnInfo->value("Model").isEmpty() && intList->isEmpty() ){
        qDebug() << "It's delay time--reget firmware version!";
        mHttp.abort();
        //delayTimer->stop();

        if ( getNumber > 0 ){
            strlog += "Try get firmwave information again.";
            GetInfo();
        }else {
            strlog += "It's over for get information.";
            delayTimer->stop();
        }

        emit GetVersionTimeOut(getNumber);
        getNumber--;
    }

    GeniePlugin_FirmwareUpdate::writeGenieLog(strlog);
}

void InfoFirmware::CloseHttp()
{
    mHttp.abort();
}

void InfoFirmware::dealResponseData(QHttpResponseHeader resp)
{
    qDebug() << "Response header's code: " << resp.statusCode();
    qDebug() << "Request Http's Status --" << mHttp.state();
    qDebug() << "Request Http's Error --" << mHttp.error();

    strlog.clear();
    strlog = "Read responseheader data:";
    if (resp.statusCode()==200){
        QByteArray mHttpInfo;
        mHttpInfo = mHttp.readAll();
        QString strInfo = mHttpInfo.constData();

        QRegExp reMatch("^Firmware.*RegionTag.*Region.*Model.*");
        QRegExp ipMatch(".*\((\\d+\\.){3}\\d+\).*");
        if ( reMatch.exactMatch(strInfo) ){
            delayTimer->stop();
            strlog += strInfo;

            QStringList InfoArray = strInfo.split(QRegExp("\\s+"), QString::SkipEmptyParts);
            QStringList TempArray;
            returnInfo->clear();
            for (int i=0; i<InfoArray.size(); i++){
                TempArray.clear();
                TempArray = InfoArray[i].split("=");
                if (TempArray[0]=="Firmware"){
                    //////Modify The Version's Information
                    //QRegExp vInfoMatch("(V\\d+(\\.\\d+){3}).*");
                    QRegExp vInfoMatch("(V\\d+(\\.\\d+)*).*");
                    int pos = vInfoMatch.indexIn(TempArray[1]);
                    if (pos > -1){
                        returnInfo->insert(TempArray[0], vInfoMatch.cap(1));
                        qDebug() << "Current Version Match: " << vInfoMatch.cap(1);
                    }else{
                        returnInfo->insert(TempArray[0], TempArray[1]);
                    }
                }else {
                    returnInfo->insert(TempArray[0], TempArray[1]);
                }
                //qDebug() << InfoArray.value(i) << "-->" << TempArray[1];
            }

            mHttp.abort();

            emit GetFinish(true);

        }
//        else if ( ipMatch.exactMatch(strInfo) ){
//            delayTimer->stop();
//            strlog += "Maybe the router is managed.";
//
//            returnInfo->insert("Model", "N/A");
//            returnInfo->insert("Firmware", "N/A");
//
//            mHttp.abort();
//
//            emit GetFinish(true);
//        }
        else {
            delayTimer->stop();
            strlog += "Not netgear router.";

            //strError->append("Not Netgear's Wireless Route.");
            intList->clear();
            intList->push_back(L_UPGRADE_PRODUCT_NONETGEAR);

            mHttp.abort();

            emit GetFinish(false);
        }

    }else {
        delayTimer->stop();
        strlog += "Not get firmware information.";

        //strError->append("Not Netgear's Wireless Route, or Not Network connections.");
        intList->clear();
        intList->push_back(L_UPGRADE_PRODUCT_UNCONNECT);

        mHttp.abort();

        emit GetFinish(false);
    }

    qDebug() << "Get firmware version finished!";
    GeniePlugin_FirmwareUpdate::writeGenieLog(strlog);

}

void InfoFirmware::dealRequestFinish(int id, bool err)
{
    if ( id == requestId ){
        qDebug() << "Finish Request http's Id: " << id;
        qDebug() << "Finish Request Http's Status --" << mHttp.state();
        qDebug() << "Finish Request Http's Error --" << mHttp.error();
        qDebug() << "Finish Response header's code: " << mHttp.lastResponse().statusCode();

        strlog.clear();
        strlog = "Request finish result:";
        if ( err ){

            switch (mHttp.error()){
            case QHttp::HostNotFound:
                {
                    //////The host name lookup failed
                    strlog += "The host name lookup failed.";
                    delayTimer->stop();

                    intList->clear();
                    intList->push_back(L_UPGRADE_PRODUCT_UNCONNECT);

                    mHttp.abort();

                    emit GetFinish(false);

                    break;
                }
//            case QHttp::AuthenticationRequiredError:
//                {
//                    strlog += "Need basic authentiza.";
//                    delayTimer->stop();
//
//                    QList< QPair<QString, QString> > mHeaderValues;
//                    mHeaderValues = mHttp.lastResponse().values();
//                    QRegExp mKeyMatch("^WWW-Authenticate$");
//                    QRegExp mValueMatch("^Basic.*NETGEAR.*");
//                    QRegExp mModelMatch("^Basic.*NETGEAR\\s+(\\S+)\"$");
//                    QString strModel;
//                    bool isFlag;
//                    isFlag = false;
//                    for(int j=0; j<mHeaderValues.size(); j++){
//
//                        if ( mValueMatch.exactMatch(mHeaderValues[j].second) ){
//                            qDebug() << "Authentica http's header Values: " << mHeaderValues[j].first
//                                    << "-->" << mHeaderValues[j].second;
//
//                            int pos = mModelMatch.indexIn(mHeaderValues[j].second);
//                            if (pos > -1)
//                                strModel = mModelMatch.cap(1);
//                            else
//                                strModel = "N/A";
//
//                            isFlag = true;
//                        }
//                    }
//
//
//                    if ( isFlag ){
//                        //////It's netgear router, but get firmware failed.
//                        if ( returnInfo->empty() ){
//                            returnInfo->insert("Model", strModel);
//                            returnInfo->insert("Firmware", "N/A");
//                            mHttp.abort();
//                            //mHttp.close();
//
//                            strlog += strModel;
//                            strlog += "--N/A";
//
//                            emit GetFinish(true);
//                        }
//
//                    }else {
//                        //////Maybe not netgear router
//                        if ( intList->empty() ){
//                            strlog += "Not netgear router.";
//                            qDebug() << "Not Netgear's Wireless Route.";
//                            //strError->append("Not Netgear's Wireless Route.");
//                            intList->push_back(L_UPGRADE_PRODUCT_UNCONNECT);
//
//                            mHttp.abort();
//
//                            emit GetFinish(false);
//                        }
//                    }
//                    break;
//                }
            default:
                break;
            }

        }else {
            strlog += "Not http request Error.";
            qDebug() << "Netgear's Wireless Route.";
        }

        GeniePlugin_FirmwareUpdate::writeGenieLog(strlog);
    }

}

