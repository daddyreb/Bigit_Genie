#include <QtGui>
#include <qhttp.h>

#include "downloadfirmware.h"
#include "genieplugin_firmwareupdate.h"
#include "LanguageFirmwareUpdate.h"



DownloadFirmware::DownloadFirmware(QObject *parent, QProgressBar *bar)
    : QObject(parent), _mProgressBar(bar)
{
    countPacket = 1;

    connect(&mHttp, SIGNAL(done(bool)), this, SLOT(HttpDone(bool)));
    connect(&mHttp, SIGNAL(dataReadProgress(int, int)), this, SLOT(DownloasProgress(int, int)));

}

DownloadFirmware::~DownloadFirmware()
{
    //mHttp.close();
}

bool DownloadFirmware::DownloadFile(const QStringList &Info, QString**info, QList<int>**intErr)
{
    if (*info == NULL)
        *info = new QString;
    strReturn = *info;
    strReturn->clear();
    if (*intErr == NULL)
        *intErr = new QList<int>;
    intReturn = *intErr;
    intReturn->clear();

    tallyPacket = 0;
    onceMiSec = 0;
    downloadSize = 0;
    agodownloadSize = 0;

    strUrl.clear();
    strUrl = Info[0];
    //strUrl = "http://updates1.netgear.com/wnr2000/us/fileinfo.txt";
    //strUrl = "http://updates1.netgear.com/wndr3400/us/fileinfo.txt";
    mFileSize.clear();
    mFileSize = Info[1];
    //mFileSize = "3129";
    mMd5Order.clear();
    mMd5Order = Info[2];
    qDebug() << "Para: " << strUrl << mFileSize << mMd5Order;

    mUrl.clear();
    mUrl = QUrl(strUrl);
    if (!mUrl.isValid()) {
        qDebug() << "Error: Invalid URL";
        strReturn->append("Error: Invalid URL");
        intReturn->push_back(L_UPGRADE_MAIN_FAILED);
        return false;
    }
    if (mUrl.scheme() != "http") {
        qDebug() << "Error: URL must start with 'http:'";
        strReturn->append("Error: URL must start with 'http:'");
        intReturn->push_back(L_UPGRADE_MAIN_FAILED);
        return false;
    }
    if (mUrl.path().isEmpty()) {
        qDebug() << "Error: URL has no path";
        strReturn->append("Error: URL has no path");
        intReturn->push_back(L_UPGRADE_MAIN_FAILED);
        return false;
    }


    localFileName.clear();
    localFileName = QFileInfo(mUrl.path()).fileName();
    if (localFileName.isEmpty())
        localFileName = "download.img";
    DownloadPath.clear();
    DownloadPath = getDownloadPath();
    //DownloadPath.append("\\");
    DownloadPath.append(localFileName);

    if ( file.isOpen() )
        file.close();
    //file.setFileName(localFileName);
    file.setFileName(DownloadPath);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Error: Cannot write file " << file.fileName()
                 << ": " << file.errorString();

        QString tmpError = "Error: Cannot write file " + file.fileName() + ": " + file.errorString();
        strReturn->append(tmpError);
        intReturn->push_back(L_UPGRADE_MAIN_FAILED);
        return false;
    }

    qDebug() << "Download FileName:" << localFileName;
    qDebug() << "DownLoad Host:" << mUrl.host();

    currentTime = QTime::currentTime();
    mHttp.setHost(mUrl.host(), mUrl.port(80));
    mHttp.get(mUrl.path(), &file);
    //mHttp.close();

    return true;
}

//bool DownloadFirmware::DownloadFile(const QStringList &Info, QString* *result)
//{
//    if (*result == NULL)
//        *result = new QString;
//    strReturn = *result;
//    strReturn->clear();
//
//    strUrl.clear();
//    strUrl = Info[0];
//    //strUrl = "http://updates1.netgear.com/wnr2000/us/fileinfo.txt";
//    mFileSize.clear();
//    mFileSize = Info[1];
//    mMd5Order.clear();
//    mMd5Order = Info[2];
//    qDebug() << "Para: " << strUrl << mFileSize << mMd5Order;
//
//    mUrl.clear();
//    mUrl = QUrl(strUrl);
//    if (!mUrl.isValid()) {
//        qDebug() << "Error: Invalid URL";
//        strReturn->append("Error: Invalid URL");
//        return false;
//    }
//    if (mUrl.scheme() != "http") {
//        qDebug() << "Error: URL must start with 'http:'";
//        strReturn->append("Error: URL must start with 'http:'");
//        return false;
//    }
//    if (mUrl.path().isEmpty()) {
//        qDebug() << "Error: URL has no path";
//        strReturn->append("Error: URL has no path");
//        return false;
//    }
//
//
//    localFileName.clear();
//    localFileName = QFileInfo(mUrl.path()).fileName();
//    if (localFileName.isEmpty())
//        localFileName = "download.img";
//
//    if ( file.isOpen() )
//        file.close();
//    file.setFileName(localFileName);
//    if (!file.open(QIODevice::WriteOnly)) {
//        qDebug() << "Error: Cannot write file " << file.fileName()
//                 << ": " << file.errorString();
//
//        QString tmpError = "Error: Cannot write file " + file.fileName() + ": " + file.errorString();
//        strReturn->append(tmpError);
//        return false;
//    }
//
//    qDebug() << "Download FileName:" << localFileName;
//    qDebug() << "DownLoad Host:" << mUrl.host();
//
//    mHttp.setHost(mUrl.host(), mUrl.port(80));
//    mHttp.get(mUrl.path(), &file);
//    //mHttp.close();
//
//    return true;
//}

void DownloadFirmware::DownloasProgress(int done, int total)
{
    mbyte = done;

    agoTime = currentTime;
    currentTime = QTime::currentTime();
    milliSecond = agoTime.msecsTo(currentTime);

    onceSize = done - agodownloadSize;
    agodownloadSize = done;
    if ( tallyPacket==countPacket ){
        onceMiSec += milliSecond;
        downloadSize += onceSize;

        leaveSize = total - done;
        leaveTime = leaveSize/downloadSize * onceMiSec;

        qDebug() << "Done:" << done << " Total:" << total
                << " Leavesize:" << leaveSize
                << " Leavetime:" << leaveTime
                << " Downloadsize:" << downloadSize;

        tallyPacket = 0;
        onceMiSec = 0;
        downloadSize = 0;
        if (countPacket==1)
            countPacket =10;

        //emit OnceStatistic(downloadSize, onceMiSec);
        emit LeaveDownloadTime(leaveTime);

    }else {
        onceMiSec += milliSecond;
        downloadSize += onceSize;
        tallyPacket++;

        //emit OnceStatistic(downloadSize, onceMiSec);
    }

    _mProgressBar->setMaximum(total);
    _mProgressBar->setValue(done);

}


void DownloadFirmware::HttpDone(bool error)
{
    qDebug() << "Download Http State: " << mHttp.state();
    qDebug() << "Download Http Error: " << mHttp.error();
    qDebug() << "Download Http Error: " << mHttp.errorString();

    if (error) {
        qDebug() << "Download Error: " << mHttp.errorString();
        //mHttp.clearPendingRequests();
        file.close();

        strReturn->append("The download Url's host name lookup failed.");
        intReturn->push_back(L_UPGRADE_MAIN_FAILED);
        emit DoneFinish(false);
    } else {
        qDebug() << "File downloaded as " << file.fileName();
        //mHttp.clearPendingRequests();
        file.close();

        if (mbyte == mFileSize.toInt()){
            JudgeMd5();

            //QString bbbb = "D:/Download/ddddddddddddddddddddddddddddddddddd/ddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd/Path";
            //strReturn->append(bbbb);
            //emit DoneFinish(true);
        }else {
            qDebug() << "File Size: " << mFileSize << " Download: " << mbyte;

            strReturn->append("Download File's Size Error");
            intReturn->push_back(L_UPGRADE_FILESIZE_ERROR);
            emit DoneFinish(false);
        }

        //emit DoneFinish();
    }
}

void DownloadFirmware::JudgeMd5()
{

    //localFileName = "WNR2000-V1.2.3.7.img";
    if ( DownloadPath.isEmpty() ){
//        QString nowDir;
//        QDir mytempDir;
//        nowDir = mytempDir.absolutePath();
//        DownloadPath = nowDir + "/";
        DownloadPath.clear();
        DownloadPath = getDownloadPath();
        //DownloadPath.append("/");
        DownloadPath.append(localFileName);
    }
    //QString strFilePath = DownloadPath + localFileName;
    //QString strFilePath = DownloadPath;
    //qDebug() << "Local File Path: " << strFilePath;

    QString m_strMd5;
    //QFile m_file(strFilePath);
    QFile m_file(DownloadPath);
    m_file.open(QFile::ReadOnly);
    //QString strFileMd5 = QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md5).toHex();
    m_Md5Judge = new QCryptographicHash(QCryptographicHash::Md5);
    m_Md5Judge->addData(m_file.readAll());
    //mMd5Result = QCryptographicHash::hash(allFile, QCryptographicHash::Md5);
    m_strMd5 = m_Md5Judge->result().toHex();
    m_file.close();
    qDebug() << "MD5: " << mMd5Order << " Md5 Result:" << m_strMd5;

    if (m_strMd5 == mMd5Order){
        qDebug() << "Now Path: " << DownloadPath;

        //QString tmpPath = "Download Path: " + strFilePath;
        //strReturn->append(tmpPath);
        //strReturn->append(strFilePath);
        strReturn->append(DownloadPath);

        emit DoneFinish(true);
    }else {
        mMd5Order = mMd5Order.toLower();
        if (m_strMd5 == mMd5Order){
            qDebug() << "Now Path: " << DownloadPath;
            //strReturn->append(strFilePath);
            strReturn->append(DownloadPath);

            emit DoneFinish(true);
        }else {
            strReturn->append("MD5 Verify Error");
            intReturn->push_back(L_UPGRADE_FILEMD5_ERROR);
            emit DoneFinish(false);
        }
    }

}

void DownloadFirmware::CloseHttp()
{
    mHttp.abort();

    if ( file.isOpen() )
        file.close();

    _mProgressBar->reset();


//    mHttp.close();
//    mHttp.clearPendingRequests();
}

void DownloadFirmware::DestructorHttp()
{
    disconnect(&mHttp, 0, 0, 0);
    mHttp.abort();

    if ( file.isOpen() )
        file.close();

    _mProgressBar->reset();
}

QString DownloadFirmware::getDownloadPath()
{
    QString returnDir;

    //////Jansion.zhou 2011-04-19
    //QString tempDir;
    //tempDir = "../";
    //QDir mytempDir(tempDir);
    //tempDir = mytempDir.absolutePath();
    //
    //returnDir = tempDir + "/downloads";
    //qDebug() << "Download's Path: " << returnDir;
    //if ( !mytempDir.cd(returnDir) ){
    //    mytempDir.mkdir(returnDir);
    //}


    ////// user's Dir
//    QString tempDir;
//    tempDir = QDir::homePath();
//    QDir mytempDir;
//    returnDir = tempDir + "\\.DownloadGenie";
//    returnDir = mytempDir.toNativeSeparators(returnDir);
//    if ( !mytempDir.cd(returnDir) )
//        mytempDir.mkdir(returnDir);

    //QString tempDir;
    //tempDir = QDir::tempPath();
    //QDir mytempDir;
    //returnDir = mytempDir.toNativeSeparators(tempDir);

    //////end


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



