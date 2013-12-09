#ifndef CHANNELCONSUMER_H
#define CHANNELCONSUMER_H
#include <QSystemSemaphore>
#include <QSharedMemory>
#include <QThread>

#include "IPCCommon.h"
#include "IChannelMessageProcessor.h"

class ChannelConsumer:public QThread
{
    Q_OBJECT
public:
    ChannelConsumer(const QString &key,QObject *parent = 0);
    virtual ~ChannelConsumer();
public:
    bool begin_consume(IChannelMessageProcessor *msg_processor);
    void end_consume();
protected:
    bool                        m_bstop;
    IChannelMessageProcessor    *m_msgprocessor;
protected:
    QSystemSemaphore            *package_consumer_syssem;
    QSystemSemaphore            *package_producer_syssem;
    QSharedMemory               *package_shared_memory;
protected:
    virtual void run();
protected:
    int receive();
    virtual int process_message(IPC_MESSAGE *msg);
};

#endif // CHANNELCONSUMER_H
