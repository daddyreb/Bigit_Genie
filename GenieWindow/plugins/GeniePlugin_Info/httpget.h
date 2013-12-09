#ifndef HTTPGET_H
#define HTTPGET_H

#include <QObject>

#include <QNetworkAccessManager>
#include <QAuthenticator>
#include <QDebug>
#include <QFile>

#include <QNetworkReply>

//class QNetworkAccessManager;
//class QAuthenticator;


class HttpGet:public QObject
{
    Q_OBJECT
public:
    HttpGet();
    ~HttpGet();
    QIODevice* requestUrl();

signals:
    void networkError(const QString);
    void requestFinished();

protected slots:
    void netManagerFinish(QNetworkReply*);
    void autheticate(QNetworkReply*, QAuthenticator*);
    void finishReply();
    void errorReply(QNetworkReply::NetworkError);
    void replyFinished(QNetworkReply*);

private:
    QNetworkAccessManager* networkManager;
    QString m_user, m_passwd;

};

#endif

