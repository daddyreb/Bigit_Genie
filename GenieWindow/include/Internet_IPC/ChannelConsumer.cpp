#include "ChannelConsumer.h"
#include <iostream>
#include <QtDebug>
#include <QStringList>


ChannelConsumer::ChannelConsumer(const QString &key,QObject *parent)
    :QThread(parent),m_bstop(false),m_msgprocessor(0)
{
    package_consumer_syssem = new QSystemSemaphore(QString(PACKAGE_CONSUMER_SEMAPHORE)+key
                                                    ,0,
#ifdef INTERNET_DAEMON_UNIX
                                                    QSystemSemaphore::Open
#else
                                                    QSystemSemaphore::Create
#endif
                                                    );

    package_producer_syssem = new QSystemSemaphore(QString(PACKAGE_PRODUCER_SEMAPHORE)+key
                                                   ,0,
#ifdef INTERNET_DAEMON_UNIX
                                                    QSystemSemaphore::Open
#else
                                                    QSystemSemaphore::Create
#endif
                                                   );

    package_shared_memory   = new QSharedMemory(PACKAGE_SHARED_MEMORY_KEY+key);
}

ChannelConsumer::~ChannelConsumer()
{
    this->wait();
    delete package_consumer_syssem;
    delete package_producer_syssem;
    delete package_shared_memory;
}

int ChannelConsumer::receive()
{
    //ready to consume and wake up the producer
    package_producer_syssem->release(1);

   // qDebug()<<"before package_consumer_syssem acquire()";
    if(package_consumer_syssem->acquire())
    {
        qDebug()<<"after package_consumer_syssem acquire()";
        if(m_bstop)//thread is to exit
        {
            return 0;
        }
        else
        {
            //get a message
           // qDebug()<<"get a message from shared memory.";
            if(!package_shared_memory->isAttached())
            {
                if(package_shared_memory->attach(QSharedMemory::ReadOnly))
                {
                    qDebug()<<"package_shared_memory attach ok.";
                }
                else
                {
                    qDebug()<<"package_shared_memory attach failed.";
                    return 0;
                }
            }
            else
            {
                qDebug()<<"package_shared_memory is already attached.";
            }

            //process the message
            IPC_MESSAGE * data = (IPC_MESSAGE*)package_shared_memory->data();
            return process_message(data);
        }
    }
    else
    {
        qDebug()<<"acquire package_consumer_syssem failed.";
        qDebug()<<"error string:"
                <<package_consumer_syssem->errorString().toStdString().c_str();
        return 0;
    }
}

void ChannelConsumer::run()
{
    while(!m_bstop)
    {
        //qDebug()<<"before channel consumer run receive";
        receive();
       // qDebug()<<"after channel consumer run receive";
    }
    //qDebug()<<"The channel consumer thread is exit ok.";
}

bool ChannelConsumer::begin_consume(IChannelMessageProcessor *msg_processor)
{
    if(isRunning())
    {
        return false;
    }
    else
    {
        m_msgprocessor  = msg_processor;
        m_bstop         = false;
        start();
        return true;
    }
}

void ChannelConsumer::end_consume()
{
    if(isRunning())
    {
        m_bstop = true;
        package_consumer_syssem->release(1);
    }
}

int ChannelConsumer::process_message(IPC_MESSAGE *msg)
{
    if(0==msg)
    {
        return -1;
    }

    //qDebug()<<"ChannelConsumer::process_message(IPC_MESSAGE *msg)_1";

    QStringList str_list;
    wchar_t     *ch_context = (wchar_t*)msg->msg_context;

    while(((char*)ch_context)<(((char*)msg->msg_context)+msg->msg_context_size))
    {
        str_list.append(QString::fromWCharArray(ch_context));
        ch_context +=(wcslen(ch_context)+1);
    }

    if(m_msgprocessor)
    {
        //qDebug()<<"ChannelConsumer::process_message(IPC_MESSAGE *msg)_2";
        return m_msgprocessor->process_message(msg->msg_sender,msg->msg_code,msg->msg_flag,str_list);
    }

   // qDebug()<<"ChannelConsumer::process_message(IPC_MESSAGE *msg)_3";
    return 0;
}
