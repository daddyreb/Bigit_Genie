#include "QGenieSpeedTest.h"
#include <QtDebug>
#include <QNetworkRequest>
#include <QNetworkReply>

#define MINIMUM_DATA_TRANSFER_BYTES   204800 //200k
#define REQUEST_TIMER_TIMEOUT_MS      60000//60 sec

QGenieSpeedTest::QGenieSpeedTest(QObject *parent)
    : QObject(parent),cur_network_reply(0),data_len(0),consum_time_ms(0)
    ,is_server_notfound(false)
{
    count_timer.start();
    request_timer.setInterval(REQUEST_TIMER_TIMEOUT_MS);

    network_am = new QNetworkAccessManager(this);
    connect(network_am,SIGNAL(finished(QNetworkReply*)),
            this,SLOT(replyFinished(QNetworkReply*)));
    connect(network_am,SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)),
            this,SLOT(sslErrors(QNetworkReply*,QList<QSslError>)));
    connect(&request_timer,SIGNAL(timeout()),this,SLOT(request_timer_timeout()));
}

QGenieSpeedTest::~QGenieSpeedTest()
{
}

bool QGenieSpeedTest::is_url_valid(const QString &url)
{
//    QString str_url(url);
//    if(!str_url.contains("://"))
//    {
//        str_url.prepend("http://");
//    }
//
//    QUrl url_temp(str_url);
//    if(!url_temp.isValid() || url_temp.scheme()!= "http")
//        return false;

    return !url.isEmpty();
}

bool QGenieSpeedTest::test_speed(const QString &url)
{

    if(!is_url_valid(url))
    {
        return false;
    }

    srt_url2test=url;
    if(!srt_url2test.contains("http://"))
    {
        srt_url2test.prepend("http://");
    }

    cur_network_reply =  network_am->get(QNetworkRequest(QUrl(srt_url2test)));
    count_timer.restart();
    request_timer.start();
    data_len = 0;
    consum_time_ms = 0;
    return true;
}

bool QGenieSpeedTest::is_server_nofound()
{
    return is_server_notfound;
}

void QGenieSpeedTest::replyFinished(QNetworkReply *reply)
{
    is_server_notfound = (reply->error()==QNetworkReply::HostNotFoundError);

    if(reply->error()==QNetworkReply::NoError)
    {
        data_len += reply->bytesAvailable();
        consum_time_ms += count_timer.elapsed();
        if((data_len  >= MINIMUM_DATA_TRANSFER_BYTES  || consum_time_ms > 30000) && consum_time_ms > 3000 ) //  test speed  optimization  zhouklansman 2013-06-13
        {
            if(!consum_time_ms)
            {
                consum_time_ms = 1;
            }
            request_timer.stop();
            double speed_Kbps = (double(data_len)/double(consum_time_ms))*(double(125)/double(16));
            emit speedtest_complete(consum_time_ms,speed_Kbps,true);
        }
        else
        {
           cur_network_reply =  network_am->get(QNetworkRequest(QUrl(srt_url2test)));
           count_timer.restart();
        }

    }
    else
    {
        request_timer.stop();
        qDebug()<<"reply finished error.";
        emit speedtest_complete(0,0.0,false);
    }

    reply->deleteLater();

}

void QGenieSpeedTest::sslErrors ( QNetworkReply * reply, const QList<QSslError> & errors )
{
    qDebug()<<" reply ssl error";
    reply->ignoreSslErrors();
}

void QGenieSpeedTest:: request_timer_timeout()
{
     request_timer.stop();
     cur_network_reply->deleteLater();
     qDebug()<<"request timeout!";
     emit speedtest_complete(0,0.0,false);
}
