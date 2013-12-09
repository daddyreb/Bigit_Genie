#include "ChannelProducer.h"
#include "IPCCommon.h"

#include <QtDebug>
#include <assert.h>

#define  SEND_TIMEOUT_INTERVAL      10000

ChannelProducer::ChannelProducer(const QString &key,QObject *parent)
    :QThread(parent),pending_send_count(0)
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

    connect(&send_timeout_timer,SIGNAL(timeout()),
            this,SLOT(sendtimer_timeout()));

    send_timeout_timer.setInterval(SEND_TIMEOUT_INTERVAL);

}

ChannelProducer::~ChannelProducer()
{
    if(send_timeout_timer.isActive())
    {
        send_timeout_timer.stop();
    }

    package_producer_syssem->release(2);
    this->quit();
    this->wait();

    delete package_consumer_syssem;
    delete package_producer_syssem;
    delete package_shared_memory;
}

void ChannelProducer::send(int sender,int code,
       int flag,const QStringList &str_list)
{
    if(!this->isRunning())
    {
        this->start();
        //wait for sender thread ready
        qDebug() << "wait for sender thread ready";
        sender_ready_sem.acquire(1);
    }

    emit send_msg(sender,code,flag,str_list);

    qDebug() << "emit send msg signal";

    //star or restart timeout timer
    ++pending_send_count;
    if(send_timeout_timer.isActive())
    {
        send_timeout_timer.stop();
    }

    send_timeout_timer.start();
}

void ChannelProducer::run()
{
    MessageSender msg_sender(package_consumer_syssem,
    package_producer_syssem,package_shared_memory);

    connect(this,SIGNAL(send_msg(int,int,int,QStringList)),&msg_sender,
            SLOT(send_msg(int,int,int,QStringList)));
    connect(&msg_sender,SIGNAL(send_msg_complete(bool)),this,SLOT(send_msg_complete(bool)));

    //notify sender is ready now
    sender_ready_sem.release(1);

    exec();
    qDebug()<<"Send Message thread exit ok";
}

void ChannelProducer::send_msg_complete(bool bok)
{
    --pending_send_count;
}

void ChannelProducer::sendtimer_timeout()
{
    if(pending_send_count > 0)
    {
        emit send_timeout();
    }
}

MessageSender::MessageSender(QSystemSemaphore *con,QSystemSemaphore *pro,
                             QSharedMemory *sh_mem,QObject *parent)
    : QObject(parent),package_consumer_syssem(con),
    package_producer_syssem(pro),package_shared_memory(sh_mem)
{

}


bool MessageSender::send_msg(int sender,int code,
              int flag,const QStringList &str_list)
{
    //prepare a shared memory

    if(package_shared_memory->isAttached())
    {
        qDebug()<<"package_shared_memory already attached(already create).";
    }
    else
    {
        bool bresult = package_shared_memory->create(PACKAGE_SHARED_MEMORY_SIZE,QSharedMemory::ReadWrite);
        if( bresult )
        {
            qDebug()<<"create package_shared_memory ok.";
        }
        else if( (!bresult) && (package_shared_memory->error() == QSharedMemory::AlreadyExists) )
        {
            qDebug () << "create package_shared_memory failed ,error == AlreadyExists";
            if(package_shared_memory->attach())
            {
                qDebug() << "attach package_shared_memory ok";
            }
            else
            {
                qDebug()<<"attach package_shared_memory failed.";
                emit send_msg_complete(false);
                return false;
            }
        }
        else
        {
            qDebug()<<"create package_shared_memory failed.";
            emit send_msg_complete(false);
            return false;
        }
    }

    //compute the message size
    int     msg_size         = sizeof(IPC_MESSAGE);
    foreach(QString str_item,str_list)
    {
        msg_size+=(str_item.length()+1)*sizeof(wchar_t);
    }

    if(msg_size>package_shared_memory->size())
    {
        qDebug()<<"message too long to send";
        emit send_msg_complete(false);
        return false;
    }

    if(package_producer_syssem->acquire())
    {
        //put the message into the shared memory
        IPC_MESSAGE *msg        = (IPC_MESSAGE*)package_shared_memory->data();
        msg->msg_sender         = sender;
        msg->msg_size           = msg_size;
        msg->msg_context_size   = msg_size - sizeof(IPC_MESSAGE);
        msg->msg_code           = code;
        msg->msg_flag           = flag;

        wchar_t* dest_buf = (wchar_t*)msg->msg_context;
        foreach(QString str_item,str_list)
        {
            dest_buf+=str_item.toWCharArray(dest_buf);
            *dest_buf = L'\0';
            ++dest_buf;
        }

        //notify the consumer the data is ready now
        package_consumer_syssem->release(1);
    }
    else
    {
        qDebug()<<"acquire producer_syssem failed.";
        emit send_msg_complete(false);
        return false;
    }

    qDebug()<<"send message ok.";
    emit send_msg_complete(true);
    return true;
}
