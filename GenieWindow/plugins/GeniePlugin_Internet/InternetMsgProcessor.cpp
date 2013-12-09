#include "InternetMsgProcessor.h"
#include "IPCCommon.h"
#include "InternetDaemonDefs.h"
#include "GeniePlugin_Internet.h"
#include <assert.h>
#include <QtDebug>
#include <QApplication>
#include <QDir>
#include <assert.h>


#define INTERNET_KEEPALIVE_TIME_MS          2000
#define INTERNETDAEMON_EXIT_WAIT_TIME_MS    3000
#define RPAIRTHREAD_EXIT_WAIT_TIME_MS       5000

InternetMsgProcessor::InternetMsgProcessor(QObject *parent) :
        QObject(parent),m_channel_p(0),m_channel_c(0),
        m_brepairthreadrunning(false),
        brecv_on(false),
        m_isStartingFix(false),
        m_pendingCancel(false)
{
    connect(&m_internet_keepalive_timer,SIGNAL(timeout()),
            this,SLOT(internet_keepalive_timeout()));
    //    connect(&m_internetdaemon,SIGNAL(started()),
    //            this,SLOT(internetdaemon_started()));
    connect(&m_internetdaemon,SIGNAL(finished(int,QProcess::ExitStatus)),
            this,SLOT(internetdaemon_finished(int,QProcess::ExitStatus)));

    connect(this,SIGNAL(mainthread_process_message_signal(int,int,int,QStringList)),
            this,SLOT(mainthread_process_message_slot(int,int,int,QStringList))/*,Qt::BlockingQueuedConnection*/);
}

InternetMsgProcessor::~InternetMsgProcessor()
{
    m_internet_keepalive_timer.stop();
    if(m_channel_c)
    {
        m_channel_c->end_consume();
    }

    //exit InternetDaemon process
    if(send(MESSAGE_SENDER_INVALID,DAEMON_EXIT_PROCESS,MESSAGE_FLAG_INVOKE,QStringList()))
    {
        m_internetdaemon.waitForFinished(INTERNETDAEMON_EXIT_WAIT_TIME_MS);
    }

	delete m_channel_c;
	delete m_channel_p;

}

//this method is called by the thread which receive message from internetdaemon
int InternetMsgProcessor::process_message(int sender, int code, int flag,
                                          const QStringList &str_list)
{
    qDebug()<<"before InternetMsgProcessor::process_message";
    //post to main thread to process
    emit mainthread_process_message_signal(sender,code,flag,str_list);
    qDebug()<<"after InternetMsgProcessor::process_message";

    return 0;
}

//this slot is called by main thread to procss message
void InternetMsgProcessor::mainthread_process_message_slot(int sender, int code,
                                                           int flag, const QStringList &str_list)
{
    //recv_mutex.lock();

    if(!brecv_on && (code != DAEMON_REPAIRTHREAD_EXIT) && (code != INTERNET_REPAIR_COMPLETE))
    {
        //recv_mutex.unlock();
        GeniePlugin_Internet::output_log(QString("skip internet msg"));
        qDebug()<<"skip internet msg";
        return;
    }
    //recv_mutex.unlock();

    switch(flag)
    {
    case 0:
        process_return(sender,code,str_list);
        break;
    case 1:
        process_invoke(sender,code,str_list);
        break;
    default:
        qDebug()<<"flag isn't recongnized.";
    }
}

int InternetMsgProcessor::process_invoke(int sender,int code,const QStringList &str_list)
{
    switch(code)
    {
    case INTERNET_SHOW_VNDLG:
        emit_show_visiblenetworklist_dlg(str_list);
        break;
    case INTERNET_SHOW_CHOOSELOCACTION_DLG:
        emit_show_chooselocation_dlg(str_list);
        break;
    case INTERNET_SHOW_MESSAGEBOX:
        messagebox(str_list);
        break;
    case INTERNET_SHOW_PLUGINCABLE_DLG:
        emit show_plugincable_dlg(0);
        break;
    case INTERNET_SHOW_WLANRADIOOFF_DLG:
        emit show_wlanradiohwoff_dlg(0);
        break;
    case INTERNET_SHOW_RESTARTROUTER_DLG:
        emit show_restart_router_dlg();
        break;
    case INTERNET_RESTARROUTER_FINISHED:
        emit end_show_restart_router_dlg();
        break;
    case INTERNET_REALTIME_MESSAGE:
        emit_realtimemsg(str_list);
        break;
    case INTERNET_REALTIME_MESSAGE2:
        emit_realtime_msg2(str_list);
        break;
    case INTERNET_REPAIR_COMPLETE:
        m_brepairthreadrunning = false;
        assert (str_list.count() == 1);
        {
            bool bok = false;
            int fixstate = str_list[0].toInt(&bok);
            assert(bok);
            emit repair_completed(fixstate);
        }
        break;
    case INTERNET_BEGIN_PCFLASH:
        emit begin_pcflash();
        break;
    case INTERNET_END_PCFLASH:
        emit end_pcflash();
        break;
    case INTERNET_BEGIN_ROUTERFLASH:
        emit begin_routerflash();
        break;
    case INTERNET_END_ROUTERFLASH:
        emit end_routerflash();
        break;
    case INTERNET_BEGIN_INTERNETFLASH:
        emit begin_internetflash();
        break;
    case INTERNET_END_INTERNETFLASH:
        emit end_internetflash();
        break;
    case INTERNET_BEGIN_PCROUTERLINKFLASH:
        emit begin_pcrouterlinkflash();
        break;
    case INTERNET_END_PCROUTERLINKFLASH:
        emit end_pcrouterlinkflash();
        break;
    case INTERNET_BEGIN_ROUTERINTERNETLINKFLASH:
        emit begin_routerinternetlinkflash();
        break;
    case INTERNET_END_ROUTERINTERNETLINKFLASH:
        emit end_routerinternetlinkflash();
        break;
    case INTERNET_BEGIN_CONNECT:
        emit begin_connect();
        break;
    case INTERNET_END_CONNECT:
        emit end_connect();
        break;
    case INTERNET_PCROUTERLINK_OK:
        emit pcrouter_link_ok();
        break;
    case INTERNET_PCROUTERLINK_FAILED:
        emit pcrouter_link_failed();
        break;
    case INTERNET_PCROUTERLINK_UNKNOWN:
        emit pcrouter_link_unknown();
        break;
    case INTERNET_ROUTERINTERNETLINK_OK:
        emit routerinternet_link_ok();
        break;
    case INTERNET_ROUTERINTERNETLINK_FAILED:
        emit routerinternet_link_failed();
        break;
    case INTERNET_ROUTERINTERNETLINK_UNKNOWN:
        emit routerinternet_link_unknown();
        break;
    case DAEMON_REPAIRTHREAD_EXIT:
        m_brepairthreadrunning = false;
        //        if(m_bterminating && m_sem_repairthread_exit.available() < 1)
        //        {
        //            m_sem_repairthread_exit.release(1);
        //        }
        //        m_bterminating = false;
        brecv_on        = false;
        emit daemon_repairthread_exit();
        break;
    default:
        qDebug()<<"invoke code isn't recongnized.";
    }

    return 0;
}

int InternetMsgProcessor::process_return(int sender,int code,const QStringList &str_list)
{
    switch(code)
    {
    default:
        qDebug()<<"return code isn't recongnized.";
    }
    return 0;
}

void InternetMsgProcessor::reset_channel()
{
    qDebug()<<"InternetMsgProcessor reset_channel begin";
    if(m_channel_p)
    {
        delete m_channel_p;
        m_channel_p = 0;
    }

    if(m_channel_c)
    {
        m_channel_c->end_consume();
        delete m_channel_c;
        m_channel_c = 0;
    }

    m_channel_p = new ChannelProducer(INTERNET2DAEMON_CHANNEL);
    m_channel_c = new ChannelConsumer(DAEMON2INTERNET_CHANNEL,this);

    connect(m_channel_p,SIGNAL(send_timeout()),
            this,SLOT(sendmsg_timeout()));

    m_channel_c->begin_consume(this);

    qDebug()<<"InternetMsgProcessor reset_channel end";
}

bool InternetMsgProcessor::start()
{
    m_isStartingFix = true;
    m_pendingCancel = false;

    bool bstart = false;

    if(m_internetdaemon.state() == QProcess::NotRunning)
    {
        QStringList str_args;
        str_args.append(INTERNET_DAEMON_START_MAGIC);
        QString daemonpath = QApplication::applicationDirPath();
#if defined Q_OS_MACX
        QDir    plugins_dir(daemonpath);
        plugins_dir.cdUp();
        plugins_dir.cd("PlugIns");
        daemonpath = plugins_dir.absolutePath();
#endif
        daemonpath += "/";
        daemonpath += INTERNET_DAEMON;

        qDebug () << "internet daemon path = " << daemonpath;
        m_internetdaemon.start(daemonpath,str_args);
        bstart = true;
        GeniePlugin_Internet::output_log(QString("Internet start daemon:")+daemonpath);
    }

    if(m_internetdaemon.waitForStarted(-1))
    {
        if(bstart)
        {
            internetdaemon_started();
        }

        //在win7下面提升权限启动修复进程时弹出的对话框的消息循环中有可能收到连网
        //消息从而cancel_fix_process被调用来取消修复操作，然而当前修复进程尚未完全启动，无法对该取消
        //操作进行处理，所以该取消操作将被修复进程错过从而导致后启动的修复进程未正常取消的问题
        //解决方法是将修复进程启动过程中的取消操作缓存起来，待修复进程启动完全后判断是否存在未决的
        //取消操作，如果有则不启动修复操作
        //m_pendingCancel = true 缓存一个取消操作消息

        if(m_pendingCancel)
        {
            GeniePlugin_Internet::output_log(QString("Internet start daemon:begin fix message not sent because of pending cancel operation detected"));
        }
        else
        {
            brecv_on = true;
            m_brepairthreadrunning = send(MESSAGE_SENDER_INVALID,DAEMON_BEGIN_FIX,MESSAGE_FLAG_INVOKE,QStringList());
            if(m_brepairthreadrunning)
            {
                GeniePlugin_Internet::output_log(QString("Internet start daemon:send begin fix message ok"));
            }
            else
            {
                GeniePlugin_Internet::output_log(QString("Internet start daemon:send begin fix message failed"));
            }
        }
    }
    else
    {
        qDebug() << "start internet daemon failed.";
        GeniePlugin_Internet::output_log(QString("Internet start daemon:wait for daemon starting failed"));
    }

    m_isStartingFix = false;
    m_pendingCancel = false;

    return m_brepairthreadrunning;
}

bool InternetMsgProcessor::cancel_fix_process(int code)
{
    m_pendingCancel = m_isStartingFix;

    if( 0 == code )//code==0 internet ok cancel current fix thread;code==1 indicate user canceled
    {
        brecv_on  = false;
    }

    if(m_brepairthreadrunning)
    {
        m_brepairthreadrunning = false;
        //emit daemon_repairthread_exit();
        QStringList arg_list;
        QString     str_code;
        str_code.setNum(code);
        arg_list.append(str_code);
        return send(MESSAGE_SENDER_INVALID,DAEMON_CANCEL_FIX,MESSAGE_FLAG_INVOKE,arg_list);
    }
    else
    {
        return true;
    }
}

bool InternetMsgProcessor::update_internetstate(bool bok)
{
    qDebug()<<"InternetMsgProcessor update_internetstate";
    QStringList str_list;
    QString     str_temp;
    str_temp.setNum(bok?1:0);
    str_list.append(str_temp);

    return send(MESSAGE_SENDER_INVALID,DAEMON_UPDATE_INTERNET_STATE,
                MESSAGE_FLAG_INVOKE,str_list);
}

bool InternetMsgProcessor::isRunning()
{
    return m_brepairthreadrunning;
}

bool InternetMsgProcessor::send(int sender,int code,int flag,
                                const QStringList &str_list)
{
    //bool ret = false;
    if(m_internetdaemon.state() == QProcess::Running
       && m_channel_p)
    {
        qDebug()<<"InternetMsgProcessor send begin";
        m_channel_p->send(sender,code,flag,str_list);
        qDebug()<<"InternetMsgProcessor send end";

        return true;
    }
    else
    {
        return false;
    }

}

void InternetMsgProcessor::sendto(int sender,int code
                                  ,int flag,const QStringList &str_list,bool &ret)
{
    ret = send(sender,code,flag,str_list);
}

void InternetMsgProcessor::emit_realtimemsg(const QStringList &str_list)
{
    if(str_list.size() == 1)
    {
        emit realtime_message(str_list[0]);
    }
}

void InternetMsgProcessor::emit_realtime_msg2(const QStringList &str_list)
{
    if(str_list.size() == 2)
    {
        bool bok = false;
        int  idx = str_list[0].toInt(&bok);
        if(bok)
        {
            emit realtime_msg2(idx,str_list[1]);
        }
    }
}

void InternetMsgProcessor::emit_show_visiblenetworklist_dlg(const QStringList &str_list)
{
    if(str_list.size() == 1)
    {
        emit show_visiblenetworklist_dlg(str_list[0],0,0);
    }
}

void InternetMsgProcessor::emit_show_chooselocation_dlg(const QStringList &sets)
{
    assert ( sets.size()%2 == 0 );

    emit show_chooselocation_dlg(sets);
}

void InternetMsgProcessor::messagebox(const QStringList &str_list)
{
    if(str_list.size() == 3)
    {
        bool bok = false,byesorno = false;
        int title_id = 0,text_id = 0;
        title_id = str_list[0].toInt(&bok);
        if(bok)
        {
            text_id = str_list[1].toInt(&bok);
            if(bok)
            {
                byesorno = (str_list[2].toInt(&bok)!=0);
                if(bok)
                {
                    emit show_messagebox(title_id,text_id,byesorno,0);
                }
            }
        }

    }
}

void InternetMsgProcessor::internetdaemon_finished(int exitcode,
                                                   QProcess::ExitStatus exitstatus)
{
    qDebug()<<"InternetMsgProcessor internetdaemon_finished";
    m_brepairthreadrunning = false;
    m_internet_keepalive_timer.stop();
    emit internetdaemon_finished();
}

void InternetMsgProcessor::internet_keepalive_timeout()
{
    this->send(MESSAGE_SENDER_INVALID,
               DAEMON_INTERNET_KEEPALIVE,MESSAGE_FLAG_INVOKE,
               QStringList());
    qDebug() << "Internet msg send keep alive packet";
}

void InternetMsgProcessor::internetdaemon_started()
{
    reset_channel();
    m_internet_keepalive_timer.start(INTERNET_KEEPALIVE_TIME_MS);
}

void InternetMsgProcessor::sendmsg_timeout()
{
    m_internetdaemon.kill();
}
