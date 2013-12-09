#include "InternetDaemonMsgProcessor.h"
#include "InternetDaemonDefs.h"
#include "IPCCommon.h"
#include <QtDebug>
#include <QCoreApplication>
#include <assert.h>

#define INTERNET_KEEPALIVE_INTERVAL         6000

InternetDaemonMsgProcessor::InternetDaemonMsgProcessor
        (QObject *parent) : QObject(parent)//,bsend_on(false)
{
    connec_repairthread_signal();
    m_channel_p = new ChannelProducer(DAEMON2INTERNET_CHANNEL);
    m_channel_c = new ChannelConsumer(INTERNET2DAEMON_CHANNEL);
    m_channel_c->begin_consume(this);
    connect(&m_internet_keepalive_timer,SIGNAL(timeout()),
            this,SLOT(internet_keepalive_timeout()));
    connect(this,SIGNAL(internet_keepalive_signal()),
            this,SLOT(internet_keepalive_slot()));
    connect(&m_repair_thread,SIGNAL(finished()),this
            ,SLOT(repairthread_finished()));

    m_internet_keepalive_timer.start(INTERNET_KEEPALIVE_INTERVAL);

    //for restart the router
    m_soapapi = new NetgearSoapSchedule();
    //m_soapapi->run();
    //end

}

InternetDaemonMsgProcessor::~InternetDaemonMsgProcessor()
{
    m_internet_keepalive_timer.stop();
    m_channel_c->end_consume();
    delete m_channel_p;
    delete m_channel_c;

    //for restart the router
    delete m_soapapi;
    m_soapapi = NULL;
    //end
}

int InternetDaemonMsgProcessor::process_message(int sender, int code, int flag,
                                                const QStringList &str_list)
{
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

    return 0;
}

int InternetDaemonMsgProcessor::process_invoke(int sender,int code,const QStringList &str_list)
{
    switch(code)
    {
    case DAEMON_UPDATE_INTERNET_STATE:
//        if(str_list.size() == 1)
//        {
//            bool bok = false;
//            int retvalue = str_list[0].toInt(&bok);
//            if(bok)
//            {
//                m_repair_thread.update_internetstate(retvalue);
//            }
//            else
//            {
//                qDebug()<<"invalid parameter.";
//            }

//        }
//        else
//        {
//            qDebug()<<"invalid parameter count.";
//        }
        break;
    case DAEMON_BEGIN_FIX:

        if(m_repair_thread.isRunning())
        {
            qDebug () << "command \"begin fix\" fix thread is already running now";
        }

        m_repair_thread.start();
        qDebug()<<"invoke begin fix.";
        break;
    case DAEMON_CANCEL_FIX:
        if(str_list.size() == 1)
        {
            bool bok = false;
            int retvalue = str_list[0].toInt(&bok);
            if(bok)
            {
                qDebug()<<"invoke cancel fix";
                m_repair_thread.cancel_fix_process(retvalue);
            }
            else
            {
                qDebug()<<"invalid parameter.";
            }

        }
        else
        {
            qDebug()<<"invalid parameter count.";
        }
        break;
    case DAEMON_EXIT_PROCESS:
        qDebug()<<"InternetDaemonMsgProcessor DAEMON_EXIT_PROCESS";
        exit_internetdaemon();
        break;
    case DAEMON_INTERNET_KEEPALIVE:
        emit internet_keepalive_signal();
        break;
    default:
        qDebug()<<"invoke code isn't recongnized.";
    }

    return 0;
}

int InternetDaemonMsgProcessor::process_return(int sender,int code,const QStringList &str_list)
{
    switch(code)
    {
    case INTERNET_SHOW_VNDLG:
        if(str_list.size()==2)
        {
            m_repair_thread.show_visiblenetworklist_dlg_return(str_list[0],str_list[1]);
        }
        else
        {
            qDebug()<<"invalid parameter count.";
        }
        break;
    case INTERNET_SHOW_MESSAGEBOX:
        if(str_list.size()==1)
        {
            bool bok = false;
            int retvalue = str_list[0].toInt(&bok);
            if(bok)
            {
                m_repair_thread.show_messagebox_return(retvalue);
            }
            else
            {
                qDebug()<<"invalid parameter.";
            }
        }
        else
        {
            qDebug()<<"invalid parameter count.";
        }
        break;
    case INTERNET_SHOW_PLUGINCABLE_DLG:
        if(str_list.size()==1)
        {
            bool bok = false;
            int retvalue = str_list[0].toInt(&bok);
            if(bok)
            {
                m_repair_thread.show_plugincable_dlg_return(retvalue);
            }
            else
            {
                qDebug()<<"invalid parameter.";
            }
        }
        else
        {
            qDebug()<<"invalid parameter count.";
        }
        break;
    case INTERNET_SHOW_WLANRADIOOFF_DLG:
        if(str_list.size()==1)
        {
            bool bok = false;
            int retvalue = str_list[0].toInt(&bok);
            if(bok)
            {
                m_repair_thread.show_wlanoff_dlg_return(retvalue);
            }
            else
            {
                qDebug()<<"invalid parameter.";
            }
        }
        else
        {
            qDebug()<<"invalid parameter count.";
        }
        break;

    case INTERNET_SHOW_RESTARTROUTER_DLG:
        if(str_list.size() == 1)
        {
            bool bok = false;
            int retvalue = str_list[0].toInt(&bok);
            if(bok)
            {
                m_repair_thread.show_restartrouter_dlg_return(retvalue);
            }
            else
            {
                qDebug()<<"invalid parameter.";
            }
        }
        else
        {
            qDebug()<<"invalid parameter count.";
        }
        break;

#ifdef Q_OS_MACX
    case INTERNET_SHOW_CHOOSELOCACTION_DLG:
        if(str_list.size() == 1)
        {

            m_repair_thread.show_chooselocation_dlg_return(QString(),str_list[0]);
        }
        else if(str_list.size() == 0)
        {
            m_repair_thread.show_chooselocation_dlg_return(QString(),QString());
        }
        else
        {
            qDebug()<<"invalid parameter count.";
        }
        break;
#endif
    default:
        qDebug()<<"return code isn't recongnized.";
    }
    return 0;
}

void InternetDaemonMsgProcessor::connec_repairthread_signal()
{
    connect(&m_repair_thread,SIGNAL(repair_completed(int)),this,SLOT(repair_completed(int)));
    connect(&m_repair_thread,SIGNAL(realtime_msg2(int,const QString&)),this,SLOT(realtime_msg2(int,const QString&)));
    connect(&m_repair_thread,SIGNAL(show_visiblenetworklist_dlg(const QString&,QString*,QString*)),
            this,SLOT(show_visiblenetworklist_dlg(const QString&,QString*,QString*)),Qt::BlockingQueuedConnection);
#ifdef Q_OS_MACX
    connect(&m_repair_thread,SIGNAL(show_chooselocation_dlg(QStringList)),this,SLOT(show_chooselocation_dlg(QStringList)),Qt::BlockingQueuedConnection);
#endif
    connect(&m_repair_thread,SIGNAL(show_plugincable_dlg(int*)),this,SLOT(show_plugincable_dlg(int*)),Qt::BlockingQueuedConnection);
    connect(&m_repair_thread,SIGNAL(show_wlanradiohwoff_dlg(int*)),this,SLOT(show_wlanradiohwoff_dlg(int*)),Qt::BlockingQueuedConnection);
    connect(&m_repair_thread,SIGNAL(show_restartrouter_dlg()),this,SLOT(show_restartrouter_dlg()),Qt::BlockingQueuedConnection);
    connect(&m_repair_thread,SIGNAL(end_show_restartrouter_dlg()),
            this,SLOT(end_show_restartrouter_dlg()),Qt::BlockingQueuedConnection);
    connect(&m_repair_thread,SIGNAL(show_messagebox(int,int,bool,bool*))
            ,this,SLOT(show_messagebox(int,int,bool,bool*)),Qt::BlockingQueuedConnection);


    //connect repair thread to state panel relative signal
    connect(&m_repair_thread,SIGNAL(begin_pcflash()),this,SLOT(begin_pcflash()));
    connect(&m_repair_thread,SIGNAL(end_pcflash()),this,SLOT(end_pcflash()));
    connect(&m_repair_thread,SIGNAL(begin_routerflash()),this,SLOT(begin_routerflash()));
    connect(&m_repair_thread,SIGNAL(end_routerflash()),this,SLOT(end_routerflash()));
    connect(&m_repair_thread,SIGNAL(begin_internetflash()),this,SLOT(begin_internetflash()));
    connect(&m_repair_thread,SIGNAL(end_internetflash()),this,SLOT(end_internetflash()));
    connect(&m_repair_thread,SIGNAL(begin_pcrouterlinkflash()),this,SLOT(begin_pcrouterlinkflash()));
    connect(&m_repair_thread,SIGNAL(end_pcrouterlinkflash()),this,SLOT(end_pcrouterlinkflash()));
    connect(&m_repair_thread,SIGNAL(begin_routerinternetlinkflash()),this,SLOT(begin_routerinternetlinkflash()));
    connect(&m_repair_thread,SIGNAL(end_routerinternetlinkflash()),this,SLOT(end_routerinternetlinkflash()));
    connect(&m_repair_thread,SIGNAL(begin_connect()),this,SLOT(begin_connect()));
    connect(&m_repair_thread,SIGNAL(end_connect()),this,SLOT(end_connect()));
    connect(&m_repair_thread,SIGNAL(pcrouter_link_ok()),this,SLOT(pcrouter_link_ok()));
    connect(&m_repair_thread,SIGNAL(pcrouter_link_failed()),this,SLOT(pcrouter_link_failed()));
    connect(&m_repair_thread,SIGNAL(pcrouter_link_unknown()),this,SLOT(pcrouter_link_unknown()));
    connect(&m_repair_thread,SIGNAL(routerinternet_link_ok()),this,SLOT(routerinternet_link_ok()));
    connect(&m_repair_thread,SIGNAL(routerinternet_link_failed()),this,SLOT(routerinternet_link_failed()));
    connect(&m_repair_thread,SIGNAL(routerinternet_link_unknown()),this,SLOT(routerinternet_link_unknown()));

    //for restart the router
    connect(&m_repair_thread,SIGNAL(reboot_router()),this,SLOT(reboot_router()));
    //end

}

//slots response to repair thread
void InternetDaemonMsgProcessor::realtime_message(const QString &msg)
{
    QStringList str_list;
    str_list.append(msg);
    qDebug()<<"InternetDaemonMsgProcessor begin send";
    send(MESSAGE_SENDER_INVALID,
         INTERNET_REALTIME_MESSAGE,MESSAGE_FLAG_INVOKE,str_list);
    qDebug()<<"InternetDaemonMsgProcessor end send";
}

void InternetDaemonMsgProcessor::realtime_msg2(int idx,const QString &param)
{
    QStringList str_list;
    QString     str_idx;

    str_idx.setNum(idx);
    str_list.append(str_idx);
    str_list.append(param);

    send(MESSAGE_SENDER_INVALID,
         INTERNET_REALTIME_MESSAGE2,MESSAGE_FLAG_INVOKE,str_list);
}

void InternetDaemonMsgProcessor::show_visiblenetworklist_dlg
        (const QString &strnicguid,QString *strssid,QString *strpwd)
{
    QStringList str_list;
    str_list.append(strnicguid);
    send(MESSAGE_SENDER_INVALID,
         INTERNET_SHOW_VNDLG,MESSAGE_FLAG_INVOKE,str_list);
}

#ifdef Q_OS_MACX
void InternetDaemonMsgProcessor::show_chooselocation_dlg(const QStringList &sets)
{
    qDebug() << "show_chooselocation_dlg";
    assert ( sets.size()%2 == 0 );

    send(MESSAGE_SENDER_INVALID,
         INTERNET_SHOW_CHOOSELOCACTION_DLG,MESSAGE_FLAG_INVOKE,sets);
}
#endif

void InternetDaemonMsgProcessor::repair_completed(int fixstate)
{
    QStringList params;
    QString     param;
    param.setNum(fixstate);
    params.append(param);
    send(MESSAGE_SENDER_INVALID,
         INTERNET_REPAIR_COMPLETE,MESSAGE_FLAG_INVOKE,params);
}

void InternetDaemonMsgProcessor::show_plugincable_dlg(int *result)
{
    send(MESSAGE_SENDER_INVALID,
         INTERNET_SHOW_PLUGINCABLE_DLG,MESSAGE_FLAG_INVOKE,QStringList());
}

void InternetDaemonMsgProcessor::show_wlanradiohwoff_dlg(int *result)
{
    send(MESSAGE_SENDER_INVALID,
         INTERNET_SHOW_WLANRADIOOFF_DLG,MESSAGE_FLAG_INVOKE,QStringList());
}

void InternetDaemonMsgProcessor::show_restartrouter_dlg()
{
    send(MESSAGE_SENDER_INVALID,
         INTERNET_SHOW_RESTARTROUTER_DLG,MESSAGE_FLAG_INVOKE,QStringList());
}

void InternetDaemonMsgProcessor::end_show_restartrouter_dlg()
{
    send(MESSAGE_SENDER_INVALID,
         INTERNET_RESTARROUTER_FINISHED,MESSAGE_FLAG_INVOKE,QStringList());
}

void InternetDaemonMsgProcessor::show_messagebox
        (int title_id,int text_id,bool byesorno,bool *result)
{
    QStringList str_list;
    QString     str_temp;
    str_list.append(str_temp.setNum(title_id));
    str_list.append(str_temp.setNum(text_id));
    str_list.append(str_temp.setNum(byesorno?1:0));

    send(MESSAGE_SENDER_INVALID,
         INTERNET_SHOW_MESSAGEBOX,MESSAGE_FLAG_INVOKE,str_list);
}

//state panel
void InternetDaemonMsgProcessor::begin_pcflash()
{
    send(MESSAGE_SENDER_INVALID,
         INTERNET_BEGIN_PCFLASH,MESSAGE_FLAG_INVOKE,QStringList());
}

void InternetDaemonMsgProcessor::end_pcflash()
{
    send(MESSAGE_SENDER_INVALID,
         INTERNET_END_PCFLASH,MESSAGE_FLAG_INVOKE,QStringList());
}

void InternetDaemonMsgProcessor::begin_routerflash()
{
    send(MESSAGE_SENDER_INVALID,
         INTERNET_BEGIN_ROUTERFLASH,MESSAGE_FLAG_INVOKE,QStringList());
}

void InternetDaemonMsgProcessor::end_routerflash()
{
    send(MESSAGE_SENDER_INVALID,
         INTERNET_END_ROUTERFLASH,MESSAGE_FLAG_INVOKE,QStringList());
}

void InternetDaemonMsgProcessor::begin_internetflash()
{
    send(MESSAGE_SENDER_INVALID,
         INTERNET_BEGIN_INTERNETFLASH,MESSAGE_FLAG_INVOKE,QStringList());
}

void InternetDaemonMsgProcessor::end_internetflash()
{
    send(MESSAGE_SENDER_INVALID,
         INTERNET_END_INTERNETFLASH,MESSAGE_FLAG_INVOKE,QStringList());
}

void InternetDaemonMsgProcessor::begin_pcrouterlinkflash()
{
    send(MESSAGE_SENDER_INVALID,
         INTERNET_BEGIN_PCROUTERLINKFLASH,MESSAGE_FLAG_INVOKE,QStringList());
}

void InternetDaemonMsgProcessor::end_pcrouterlinkflash()
{
    send(MESSAGE_SENDER_INVALID,
         INTERNET_END_PCROUTERLINKFLASH,MESSAGE_FLAG_INVOKE,QStringList());
}

void InternetDaemonMsgProcessor::begin_routerinternetlinkflash()
{
    send(MESSAGE_SENDER_INVALID,
         INTERNET_BEGIN_ROUTERINTERNETLINKFLASH,MESSAGE_FLAG_INVOKE,QStringList());
}

void InternetDaemonMsgProcessor::end_routerinternetlinkflash()
{
    send(MESSAGE_SENDER_INVALID,
         INTERNET_END_ROUTERINTERNETLINKFLASH,MESSAGE_FLAG_INVOKE,QStringList());
}

void InternetDaemonMsgProcessor::begin_connect()
{
    send(MESSAGE_SENDER_INVALID,
         INTERNET_BEGIN_CONNECT,MESSAGE_FLAG_INVOKE,QStringList());
}

void InternetDaemonMsgProcessor::end_connect()
{
    send(MESSAGE_SENDER_INVALID,
         INTERNET_END_CONNECT,MESSAGE_FLAG_INVOKE,QStringList());
}

void InternetDaemonMsgProcessor::pcrouter_link_ok()
{
    send(MESSAGE_SENDER_INVALID,
         INTERNET_PCROUTERLINK_OK,MESSAGE_FLAG_INVOKE,QStringList());
}

void InternetDaemonMsgProcessor::pcrouter_link_failed()
{
    send(MESSAGE_SENDER_INVALID,
         INTERNET_PCROUTERLINK_FAILED,MESSAGE_FLAG_INVOKE,QStringList());
}

void InternetDaemonMsgProcessor::pcrouter_link_unknown()
{
    send(MESSAGE_SENDER_INVALID,
         INTERNET_PCROUTERLINK_UNKNOWN,MESSAGE_FLAG_INVOKE,QStringList());
}

void InternetDaemonMsgProcessor::routerinternet_link_ok()
{
    send(MESSAGE_SENDER_INVALID,
         INTERNET_ROUTERINTERNETLINK_OK,MESSAGE_FLAG_INVOKE,QStringList());
}

void InternetDaemonMsgProcessor::routerinternet_link_failed()
{
    send(MESSAGE_SENDER_INVALID,
         INTERNET_ROUTERINTERNETLINK_FAILED,MESSAGE_FLAG_INVOKE,QStringList());
}

void InternetDaemonMsgProcessor::routerinternet_link_unknown()
{
    send(MESSAGE_SENDER_INVALID,
         INTERNET_ROUTERINTERNETLINK_UNKNOWN,MESSAGE_FLAG_INVOKE,QStringList());
}

//slots response to repair thread end

void InternetDaemonMsgProcessor::internet_keepalive_timeout()
{
    qDebug()<<"InternetDaemonMsgProcessor::internet_keepalive_timeout()";
    exit_internetdaemon();
}


void InternetDaemonMsgProcessor::exit_internetdaemon()
{
    qDebug() << "try exit internet daemon";

    if(m_repair_thread.isRunning())
    {
        m_repair_thread.cancel_fix_process(1);
    }

    qApp->quit();
}

void InternetDaemonMsgProcessor::internet_keepalive_slot()
{
    m_internet_keepalive_timer.stop();
    m_internet_keepalive_timer.start(INTERNET_KEEPALIVE_INTERVAL);
    qDebug() << "InternetDaemon internet keepalive";
}

void InternetDaemonMsgProcessor::repairthread_finished()
{
    send(MESSAGE_SENDER_INVALID,
         DAEMON_REPAIRTHREAD_EXIT,MESSAGE_FLAG_INVOKE,QStringList());
}

void InternetDaemonMsgProcessor::send(int sender,int code,int flag,const QStringList &str_list)
{
    m_channel_p->send(sender,code,flag,str_list);
}

//----------for restart the router---------------

void InternetDaemonMsgProcessor::reboot_router()
{

    QMap<QString,QString> reboot_param;

    QString  str_session("reboot the router");
    QString  reboot_module("DeviceConfig");
    QString  reboot_method("Reboot");
    QUuid    uuid_reboot("{017479AA-0D4C-454c-BE10-2B2835F34F38}");

    m_soapapi->submitrequest(reboot_module,reboot_method , reboot_param,uuid_reboot,str_session);
}

//--------------end---------------------
