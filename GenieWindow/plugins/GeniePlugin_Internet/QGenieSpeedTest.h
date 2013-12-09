#ifndef QGENIESPEEDTEST_H
#define QGENIESPEEDTEST_H

#include <QObject>
#include <QUrl>
#include <QTime>
#include <QTimer>
#include <QNetworkAccessManager>



class QGenieSpeedTest : public QObject
{
Q_OBJECT
public:
    explicit QGenieSpeedTest(QObject *parent = 0);
    virtual ~QGenieSpeedTest();
public:
    static bool is_url_valid(const QString &url);
    bool test_speed(const QString &url);
    bool is_server_nofound();
private:
    QNetworkAccessManager           *network_am;
    QNetworkReply                   *cur_network_reply;
    QTime                           count_timer;
    QTimer                          request_timer;
    QString                         srt_url2test;
    unsigned long                   data_len;
    int                             consum_time_ms;
    bool                            is_server_notfound;
signals:
    void speedtest_complete(int accesstime,double speed_kbps,bool ok);
protected slots:
    void replyFinished(QNetworkReply *reply);
    void sslErrors ( QNetworkReply * reply, const QList<QSslError> & errors );
    void request_timer_timeout();
};

#endif // QGENIESPEEDTEST_H
