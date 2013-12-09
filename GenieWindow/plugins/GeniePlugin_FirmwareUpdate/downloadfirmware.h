#ifndef DOWNLOADFIRMWARE_H
#define DOWNLOADFIRMWARE_H

#include <QObject>
#include <QMap>
#include <QUrl>
#include <QHttp>
#include <QStringList>
#include <QFile>
#include <QTime>

#include <QObject>

class QProgressBar;
class QCryptographicHash;


class DownloadFirmware : public QObject
{
    Q_OBJECT
public:
    DownloadFirmware(QObject *parent=0, QProgressBar *bar=0);
    ~DownloadFirmware();

    //bool DownloadFile(const QStringList &Info, QString* *result);
    bool DownloadFile(const QStringList &Info, QString **info, QList<int> **intErr);
    void CloseHttp();
    void DestructorHttp();

signals:
    void DoneError();
    void DoneFinish(bool err);
    void OnceStatistic(int msize, int mtime);
    void LeaveDownloadTime(int ltime);

private slots:
    void HttpDone(bool error);
    void DownloasProgress(int done, int total);

private:
    QString strUrl;
    QUrl mUrl;
    QHttp mHttp;

    QTime currentTime;
    QTime agoTime;
    int milliSecond;
    int tallyPacket;
    int countPacket;
    int onceMiSec;
    int onceSize;
    int agodownloadSize;
    int downloadSize;
    int leaveSize;
    int leaveTime;

    QFile file;
    QString localFileName;
    QString DownloadPath;



    QString mMd5Order;
    QString mFileSize;
    int mbyte;

    QString *strReturn;
    QList<int> *intReturn;

    QProgressBar *_mProgressBar;
    QCryptographicHash *m_Md5Judge;

    void JudgeMd5();
    QString getDownloadPath();
    //bool TestJudgeMd5();

};

#endif // DOWNLOADFIRMWARE_H
