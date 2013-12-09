#ifndef CHANNELPRODUCER_H
#define CHANNELPRODUCER_H

#include <QSharedMemory>
#include <QSystemSemaphore>
#include <QStringList>
#include <QString>
#include <QThread>
#include <QSemaphore>
#include <QTimer>

class MessageSender;

class ChannelProducer :public QThread
{
    Q_OBJECT
public:
    ChannelProducer(const QString &key,QObject *parent=0);
    ~ChannelProducer();
protected:
    QSystemSemaphore    *package_consumer_syssem;
    QSystemSemaphore    *package_producer_syssem;
    QSharedMemory       *package_shared_memory;
    QSemaphore          sender_ready_sem;

    QTimer              send_timeout_timer;

    int                 pending_send_count;
public:
    void send(int sender,int code,int flag,const QStringList &str_list);
protected:
    virtual void run();
signals:
    void send_msg(int sender,int code,int flag,const QStringList &str_list);
    void send_timeout();
protected slots:
    void send_msg_complete(bool bok);
    void sendtimer_timeout();
};

class MessageSender :public QObject
{
    Q_OBJECT
public:
    MessageSender(QSystemSemaphore *con,QSystemSemaphore *pro,QSharedMemory *sh_mem
                  ,QObject *parent = 0);
private:
    QSystemSemaphore    *package_consumer_syssem;
    QSystemSemaphore    *package_producer_syssem;
    QSharedMemory       *package_shared_memory;
signals:
    void send_msg_complete(bool bok);
private slots:
    bool send_msg(int sender,int code,int flag,const QStringList &str_list);
};

#endif // CHANNELPRODUCER_H
