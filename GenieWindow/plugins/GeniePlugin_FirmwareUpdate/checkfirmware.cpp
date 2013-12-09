#include <QtGui>
#include <qhttp.h>

#include "checkfirmware.h"
#include "genieplugin_firmwareupdate.h"
#include "LanguageFirmwareUpdate.h"


CheckFirmware::CheckFirmware()
{

    strRootUrl = "http://updates1.netgear.com";
    strFileName = "fileinfo.txt";
    mHttpFalg = true;

    //connect(&mHttp, SIGNAL(done(bool)), this, SLOT(GetNewVersion(bool)));
    connect(&mHttp, SIGNAL(done(bool)), this, SLOT(dealFile(bool)));


    checkNumber = 1;
    getNewSec = 30000;
    getNewTimer = new QTimer();
    getNewTimer->setSingleShot(false);
    connect(getNewTimer, SIGNAL(timeout()), this, SLOT(dealDelayTime()));

}

CheckFirmware::~CheckFirmware()
{
    //mHttp.close();
}



void CheckFirmware::CheckVersion(QString pUrl, QMap<QString, QString>* *Info, QList<int>* *err)
{
    checkNumber = 0;
    getNewTimer->start(getNewSec);

    if (*Info == NULL)
        *Info = new QMap<QString, QString>;
    returnInfo = *Info;
    returnInfo->clear();
    if (*err == NULL)
        *err = new QList<int>;
    intErrList = *err;
    intErrList->clear();


    mHttpFalg = true;

    if (pUrl!=NULL){
        pUrl.replace(QString("_"), QString("/"));
        middleUrl.clear();
        middleUrl = pUrl;
        middleUrl = middleUrl.toLower();
        strUrl = strRootUrl + "/" + pUrl + "/" + strFileName;
        strUrl = strUrl.toLower();
    }

    //////test
    //strUrl = "http://updates1.netgear.com/dgnd3300/us/fileinfo.txt";

    qDebug() << "Download URL: " << strUrl;
    GeniePlugin_FirmwareUpdate::writeGenieLog("Download:"+strUrl);
    mUrl = QUrl(strUrl);


    checkFile.clear();
    checkFile = QFileInfo(mUrl.path()).fileName();
    if (checkFile.isEmpty())
        checkFile = "fileinfo.txt";
    //////use system's temp path
    DownloadPath.clear();
    DownloadPath = getDownloadPath();
    //DownloadPath.append("\\");
    DownloadPath.append(checkFile);


    if ( mFile.isOpen() )
        mFile.close();
    //mFile.setFileName(checkFile);
    mFile.setFileName(DownloadPath);
    if (!mFile.open(QIODevice::WriteOnly)) {
        qDebug() << "Error: Cannot write file " << mFile.fileName()
                 << ": " << mFile.errorString();
        return;
    }

    qDebug() << "Download FileName:" << checkFile;
    qDebug() << "DownLoad Host:" << mUrl.host();

    //mHttp.setHost(mUrl.host(), mUrl.port(80));
    //mHttp.get(mUrl.path());
    ////mHttp.close();
    //return true;

    //////use file
    mHttp.setHost(mUrl.host(), mUrl.port(80));
    mHttp.get(mUrl.path(), &mFile);

    GeniePlugin_FirmwareUpdate::writeGenieLog("Start send http request for get the lastest firmwre. Path:"+DownloadPath);

}


void CheckFirmware::GetNewVersion(bool error)
{
    if (mHttpFalg){
        qDebug() << "Check Http State: " << mHttp.state();
        qDebug() << "Check Http Error: " << mHttp.error();

        if (mHttp.state() == QHttp::Connected){
            if (!error){

                qDebug() << "Get New Version Info Start!";
                QByteArray mHttpInfo;
                mHttpInfo = mHttp.readAll();
                QString strInfo = mHttpInfo.constData();
                //mHttp.close();
                qDebug() << "Download Content: " << strInfo;


                QRegExp reMatch(".*"+strFileName+".*not\\s+found\\s+on\\s+this\\s+server.*");
                if ( !reMatch.exactMatch(strInfo) ){

                    QStringList VersionArray = strInfo.split(QRegExp("\\s+"), QString::SkipEmptyParts);

                    QRegExp FirstMatch(".*Major.*");
                    if ( FirstMatch.exactMatch(VersionArray.value(0)) ){

                        QStringList TempArray;
                        returnInfo->clear();
                        QRegExp DataMatch("^\\[.*language.*\\]$");
                        QRegExp mDataMatch("^\\[.*\\]$");
                        for (int i=0; i<VersionArray.size(); i++){
                            qDebug() << "VersionArray Slipt: " << VersionArray.value(i);
                            if (DataMatch.exactMatch(VersionArray[i]))
                                break;

                            if ( !mDataMatch.exactMatch(VersionArray[i]) ){
                                TempArray.clear();
                                TempArray = VersionArray[i].split("=");
                                if (TempArray.size()>1)
                                    returnInfo->insert(TempArray[0], TempArray[1]);
                                //qDebug() << VersionArray[i];
                            }

                            //goto OVERDATA;
                            //break;
                        }

                        QString strTemp;
                        strTemp = returnInfo->value("file");
                        if ( strTemp != NULL ){
                            //TempArray.clear();
                            //TempArray = strTemp.split("-");
                            //strTemp.clear();

                            QRegExp vMatchChk(".*-(V\\d+.*\\d+)\\.chk$");
                            QRegExp vMatchImg(".*-(V\\d+.*\\d+)\\.img$");
                            int pos;
                            pos = vMatchImg.indexIn(strTemp);
                            if (pos > -1){
                                returnInfo->insert("firmware", vMatchImg.cap(1));
                                //qDebug() << "Match: " << vInfoMatch.cap(1);
                            }else {
                                pos = vMatchChk.indexIn(strTemp);
                                if (pos > -1){
                                    returnInfo->insert("firmware", vMatchChk.cap(1));
                                }else {
                                    returnInfo->insert("firmware", "NULL");
                                }
                            }
                        }

                        strTemp.clear();
                        QRegExp urlMatch(".*"+middleUrl+".*");
                        qDebug() << "Url File: " << middleUrl << "-" << returnInfo->value("file");
                        if ( urlMatch.exactMatch(returnInfo->value("file")) ){
                            strTemp = strRootUrl + returnInfo->value("file");
                        }else {
                            strTemp = strRootUrl + "/" + middleUrl + "/" + returnInfo->value("file");
                        }
                        qDebug() << "Last Url File: " << strTemp;
                        returnInfo->insert("url", strTemp);

                        mHttpFalg = false;
                        returnInfo->insert("error", "NULL");
                        //emit GetVersionFinish();
                        emit CheckFinish(true);

                    }else {
                        qDebug() << "Update Information is worng.";
                        //returnInfo->insert("error", "Update Information is worng.");
                        intErrList->push_back(L_UPGRADE_MAIN_FAILED);
                        mHttpFalg = false;
                        emit CheckFinish(false);

                    }

                }else {
                    qDebug() << "Netgear's Upgrade Info was not found on The Netgear web state.";
                    //returnInfo->insert("error", "Netgear's Upgrade Info was not found on The Netgear web state.");
                    intErrList->push_back(L_UPGRADE_MAIN_FAILED);
                    //returnInfo->insert("error", "1965");
                    mHttpFalg = false;
                    emit CheckFinish(false);

                }

            }else {
                qDebug() << "Get New Version Info Failed!";
                //strError->append("Get New Version Info Failed");
                //mHttp.close();
                //emit CheckFailed();

                //returnInfo->insert("error", "Get New Version Info Failed.");
                intErrList->push_back(L_UPGRADE_MAIN_FAILED);
                //returnInfo->insert("error", "1964");
                mHttpFalg = false;
                emit CheckFinish(false);
            }

        }else {
            mHttpFalg = false;
            qDebug() << "Http Unconnected!";
            qDebug() << "Http Error: " << mHttp.errorString();
            //mHttp.close();
            //returnInfo->insert("error", "There is no connection to the NETGEAR web site.");
            intErrList->push_back(L_UPGRADE_MAIN_FAILED);
            //returnInfo->insert("error", "1966");
            emit CheckFinish(false);
        }

    }

}

void CheckFirmware::dealFile(bool err)
{
    if (mHttpFalg){
        qDebug() << "Check Http State: " << mHttp.state();
        qDebug() << "Check Http Error: " << mHttp.error();

        if (getNewTimer->isActive())
            getNewTimer->stop();

        if (mHttp.state() == QHttp::Connected){
//            if ( filePath == NULL ){
//                QString nowDir;
//                QDir mytempDir;
//                nowDir = mytempDir.absolutePath();
//                filePath = nowDir + "/";
//            }
            //QString strFilePath = filePath + checkFile;
            //QString strFilePath = "./" + checkFile;
            QString strFilePath = DownloadPath;
            qDebug() << "Local File Path: " << strFilePath;
            QDir delFile;

            if (!err){
                getNewTimer->stop();

                mFile.close();
                mHttp.abort();

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
                            VersionArray += line;
                        }
                    }
                    file.close();

                    QRegExp FirstMatch(".*Major.*");
                    if ( FirstMatch.exactMatch(VersionArray.value(0)) ){

                        QStringList TempArray;
                        returnInfo->clear();
                        QRegExp DataMatch("^\\[.*language.*\\]$");
                        QRegExp mDataMatch("^\\[.*\\]$");
                        for (int i=0; i<VersionArray.size(); i++){
                            //qDebug() << "VersionArray Slipt: " << VersionArray.value(i);
                            if (DataMatch.exactMatch(VersionArray[i]))
                                break;

                            if ( !mDataMatch.exactMatch(VersionArray[i]) ){
                                TempArray.clear();
                                TempArray = VersionArray[i].split("=");
                                if (TempArray.size()>1)
                                    returnInfo->insert(TempArray[0], TempArray[1]);
                            }
                        }

                        QString strTemp;
                        strTemp = returnInfo->value("file");
                        if ( strTemp != NULL ){
//                            QRegExp vMatchChk(".*-(V\\d+.*\\d+)\\.chk$");
//                            QRegExp vMatchImg(".*-(V\\d+.*\\d+)\\.img$");
//                            int pos;
//                            pos = vMatchImg.indexIn(strTemp);
//                            if (pos > -1){
//                                returnInfo->insert("firmware", vMatchImg.cap(1));
//                                //qDebug() << "Match: " << vInfoMatch.cap(1);
//                            }else {
//                                pos = vMatchChk.indexIn(strTemp);
//                                if (pos > -1){
//                                    returnInfo->insert("firmware", vMatchChk.cap(1));
//                                }else {
//                                    returnInfo->insert("firmware", "NULL");
//                                }
//                            }
                            
                            
                            //QRegExp vMatchVer(".*-(V\\d+.*\\d+).*(chk|img)$");
                            QRegExp vMatchVer(".*-(\\D+\\d+(\\.\\d+)*).*(chk|img)$");
                            int pos;
                            pos = vMatchVer.indexIn(strTemp);
                            if (pos > -1){
                                returnInfo->insert("firmware", vMatchVer.cap(1));
                                //qDebug() << "Match: " << vInfoMatch.cap(1);
                            }else {
                                returnInfo->insert("firmware", "NULL");
                            }
                        }else
                            returnInfo->insert("firmware", "NULL");

                        strTemp.clear();
                        QString mFileName;
                        mFileName = returnInfo->value("file");
                        if ( mFileName == NULL ){
                            strTemp = "NULL";
                        }else {
                            QRegExp urlMatch(".*"+middleUrl+".*");
                            if ( urlMatch.exactMatch(returnInfo->value("file")) ){
                                strTemp = strRootUrl + returnInfo->value("file");
                            }else {
                                strTemp = strRootUrl + "/" + middleUrl + "/" + returnInfo->value("file");
                            }
                        }
                        qDebug() << "Last Url File: " << strTemp;
                        GeniePlugin_FirmwareUpdate::writeGenieLog("Download the lasest firmware from URL "+strTemp);
                        returnInfo->insert("url", strTemp);

                        mHttpFalg = false;
                        returnInfo->insert("error", "NULL");
                        delFile.remove(strFilePath);

                        emit CheckFinish(true);

                    }else {
                        bool errType;
                        errType = false;
                        QRegExp reMatch(".*"+strFileName+".*not\\s+found\\s+on\\s+this\\s+server.*");
                        for (int i=0; i<VersionArray.size(); i++){
                            if ( reMatch.exactMatch(VersionArray.value(i)))
                                errType = true;
                        }

                        GeniePlugin_FirmwareUpdate::writeGenieLog("It's not upgrade file.");
                        if (errType){
                            qDebug() << "Netgear's Upgrade Info was not found on The Netgear web state.";
                            intErrList->push_back(L_UPGRADE_MAIN_FAILED);
                        }else {
                            qDebug() << "Update Information is worng.";
                            intErrList->push_back(L_UPGRADE_MAIN_FAILED);
                        }

                        mHttpFalg = false;
                        delFile.remove(strFilePath);

                        emit CheckFinish(false);
                    }

                }else {
                    //getNewTimer->stop();
                    mHttp.abort();

                    qDebug() << "This is worng when Open the Download's check file.";
                    GeniePlugin_FirmwareUpdate::writeGenieLog("It's worng when Open the file to checked firmware.");
                    intErrList->push_back(L_UPGRADE_MAIN_FAILED);
                    mHttpFalg = false;
                    delFile.remove(strFilePath);

                    emit CheckFinish(false);
                }

            }else {
                //getNewTimer->stop();
                mFile.close();
                delFile.remove(strFilePath);
                mHttp.abort();

                qDebug() << "Get New Version Info Failed!";
                GeniePlugin_FirmwareUpdate::writeGenieLog("Get lastest firmware information failed!");
                intErrList->push_back(L_UPGRADE_MAIN_FAILED);
                mHttpFalg = false;
                emit CheckFinish(false);
            }

        }else {
            //getNewTimer->stop();
            mHttp.abort();
            mHttpFalg = false;

            qDebug() << "Http Error: " << mHttp.errorString();
            GeniePlugin_FirmwareUpdate::writeGenieLog("The network is disconnect.");

            intErrList->push_back(L_UPGRADE_MAIN_FAILED);
            emit CheckFinish(false);
        }

    }

}

void CheckFirmware::dealDelayTime()
{
    GeniePlugin_FirmwareUpdate::writeGenieLog("It's delay timeout of check firmware.");
    if ( returnInfo->value("firmware").isEmpty() && intErrList->isEmpty() ){
        qDebug() << "It's delay time--get lastest firmware!";
        mHttp.abort();
        //getNewTimer->stop();

        if ( checkNumber > 0 )
            CheckVersion();
        else
            if (getNewTimer->isActive())
                getNewTimer->stop();

        emit GetNewTimeOut(checkNumber);

        checkNumber--;
    }

}

void CheckFirmware::CheckVersion()
{
    getNewTimer->start(getNewSec);

    returnInfo->clear();
    intErrList->clear();


    mHttpFalg = true;

    //strUrl = "http://updates1.netgear.com/wnr1000v2/wo/fileinfo.txt";
    qDebug() << "Download URL: " << strUrl;
    mUrl = QUrl(strUrl);


    if ( mFile.isOpen() )
        mFile.close();
    //mFile.setFileName(checkFile);
    mFile.setFileName(DownloadPath);
    if (!mFile.open(QIODevice::WriteOnly)) {
        qDebug() << "Error: Cannot write file " << mFile.fileName()
                 << ": " << mFile.errorString();
        return;
    }


    //////use file
    mHttp.setHost(mUrl.host(), mUrl.port(80));
    mHttp.get(mUrl.path(), &mFile);

}

QString CheckFirmware::getDownloadPath()
{
    QString returnDir;
    QString tempHomeDir;
    QString childDir;
    QDir mytempDir;

    tempHomeDir = QDir::home().absolutePath()+QString("/NETGEARGenie");
    childDir = tempHomeDir + QString("/update_temp/");

    if ( !QDir::home().exists(tempHomeDir) )
        QDir::home().mkdir(tempHomeDir);

    if (!QDir::home().exists(childDir))
        QDir(tempHomeDir).mkdir(childDir);

    returnDir = mytempDir.toNativeSeparators(childDir);

    return returnDir;

}

void CheckFirmware::CloseHttp()
{
    mHttp.abort();
}

void CheckFirmware::DestructorHttp()
{
    disconnect(&mHttp, 0, 0, 0);
    mHttp.abort();
}

