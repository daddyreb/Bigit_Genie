#ifndef CHECKFIRMWARE_H
#define CHECKFIRMWARE_H

#include <QObject>
#include <QMap>
#include <QUrl>
#include <QHttp>
#include <QFile>
#include <QStringList>
#include <QDir>
#include <QTimer>

#include <QObject>

class CheckFirmware : public QObject
{
    Q_OBJECT
public:
    CheckFirmware();
    ~CheckFirmware();

    //void CheckVersion(QString pUrl, QMap<QString, QString>* *Info);
    void CheckVersion(QString pUrl, QMap<QString, QString>* *Info, QList<int>* *err);
    void CloseHttp();
    void DestructorHttp();

private slots:
    void GetNewVersion(bool error);
    void dealFile(bool err);
    void dealDelayTime();

signals:
    void CheckFinish(bool error);
    void GetNewTimeOut(int stat);

private:
    void CheckVersion();

    QString getDownloadPath();
    QString DownloadPath;

    QString strRootUrl;
    QString strFileName;
    QString strUrl;
    QString middleUrl;
    QUrl mUrl;
    QHttp mHttp;
    bool mHttpFalg;

    int checkNumber;
    QTimer *getNewTimer;
    int getNewSec;

    QString *strError;
    QList<int> *intErrList;
    QMap<QString, QString> *returnInfo;

    QMap<QString, QString> newVersionInfo;


    QFile mFile;
    QString checkFile;
    QString filePath;
    //QDir delFile;


};

#endif // CHECKFIRMWARE_H
