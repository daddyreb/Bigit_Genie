#ifndef INFOFIRMWARE_H
#define INFOFIRMWARE_H

#include <QObject>
#include <QMap>
#include <QUrl>
#include <QHttp>
#include <QStringList>
#include <QTimer>

#include <QObject>


class InfoFirmware : public QObject
{
    Q_OBJECT
public:
    InfoFirmware();
    ~InfoFirmware();

    void GetInfo(QMap<QString, QString>* *VerInfo, QList<int>* *err);
    void CloseHttp();

private slots:
    void GetFirmware(bool error);
    void dealDelayTime();

    void dealRequestFinish(int id, bool err);
    void dealResponseData(QHttpResponseHeader resp);

signals:
    //void GetFailed();
    //void GetSuccess();
    void GetFinish(bool err);
    void GetVersionTimeOut(int stat);


private:
    void GetInfo();

    int getNumber;
    QString strUrl;
    QUrl mUrl;
    QHttp mHttp;
    QTimer *delayTimer;
    int delaySec;

    QHttpRequestHeader Requestheader;
    int requestId;


    QString strErr_con;
    QString strErr_not;
    //QString *strError;
    QList<int> *intList;
    QMap<QString, QString> *returnInfo;


    QString strlog;

};

#endif // INFOFIRMWARE_H
