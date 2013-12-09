#include "QGenieRefreshInternetStateHandler.h"
#include "GeniePlugin_Internet.h"
#include <assert.h>
#include <QTcpSocket>

#define LOOKUP_TIMEOUT_MS       5000 //5 seconds
#define LOOKUP_INTERVAL_MS      3000 //3 seconds
#define MAX_LOOKUPERROR_COUNT   6

#ifdef Q_OS_WIN
#include "DiagnoseInterface.h"
#include "DnsQuery_Win.h"
extern DiagnoseInterface *diagnose_inf;
#elif defined Q_OS_MACX
#include "INetApiInterface.h"
#include "WiFiApiInterface.h"

extern INetApiInterface *inet_api;
extern WiFiApiInterface *wifi_api;

#endif

static const char* domains[] = {"www.netgear.com","www.netgear.com"};

QGenieRefreshInternetStateHandler::QGenieRefreshInternetStateHandler(QObject *parent) :
        QObject(parent)
        ,m_ilookup_id(-1)
        ,m_blookingup(false)
        ,m_Internet_Status(INTERNET_CONNECTED)
        ,m_err_count(0)
        ,m_cur_domain_idx(0)
        //,m_bsingleshot(false)
{
    lookup_timeout_timer = new QGenieThreadTimer(this);
    lookup_timeout_timer->setInterval(LOOKUP_TIMEOUT_MS/1000);
    connect(lookup_timeout_timer,SIGNAL(timeout()),this,SLOT(lookup_timeout()));
}

QGenieRefreshInternetStateHandler::~QGenieRefreshInternetStateHandler()
{
    lookup_timeout_timer->stop();
}

void QGenieRefreshInternetStateHandler::begin_refresh(bool bsingleshot)
{
    assert ( (m_cur_domain_idx >= 0) && ( m_cur_domain_idx < int(sizeof(domains)/sizeof(domains[0])) ) );

    GeniePlugin_Internet::output_log(QString("Internet state handler begin_refresh singleshot = %1,lookingup = %2").arg(QString(bsingleshot?"yes":"no"),QString(m_blookingup?"yes":"no")));

    //m_bsingleshot   = bsingleshot;

    if(!m_blookingup)
    {
        m_ilookup_id    = QHostInfo::lookupHost(QString(domains[m_cur_domain_idx]),this,SLOT(lookup_completed(QHostInfo)));
        m_blookingup    = true;
        lookup_timeout_timer->start();
        m_cur_domain_idx = ((++m_cur_domain_idx) % (sizeof(domains)/sizeof(domains[0])));
    }
}

#ifdef Q_OS_WIN
bool QGenieRefreshInternetStateHandler::verify_netgear_haj_win32(const QHostInfo & info)
{
    bool bok = true;

    if(diagnose_inf)
    {
        QList<NCard> nic_list;
        QStringList  gateways;

        diagnose_inf->EnumInterfaces(nic_list);

        for(int k = 0;k<nic_list.count();++k)
        {
            QStringList tmplist;
            if(diagnose_inf->GetInterfaceGateway(nic_list[k].m_devguid,tmplist))
            {
                gateways.append(tmplist);
            }
        }

        QStringList         hostips;
        QList<QHostAddress> addrlist = info.addresses();

        foreach(QHostAddress addr,addrlist)
        {
            hostips.append(addr.toString());
        }

        foreach(QString hostip,hostips)
        {
            foreach(QString gateway,gateways)
            {
                if(!hostip.compare(gateway,Qt::CaseInsensitive))
                {
                    bok = false;
                    break;
                }
            }

            if(!bok)
            {
                break;
            }
        }
    }

    return bok;
}

#elif defined Q_OS_MACX
bool QGenieRefreshInternetStateHandler::verify_netgear_haj_macx(const QHostInfo & info)
{
    bool bok = true;

    if( inet_api != NULL )
    {
        QStringList         hostips;
        QString             gateway;

        inet_api->GetPrimaryServiceRouter(gateway);

        QList<QHostAddress> addrlist = info.addresses();

        foreach(QHostAddress addr,addrlist)
        {
            hostips.append(addr.toString());
        }

        foreach(QString hostip,hostips)
        {
            qDebug() <<"hajackt test:hostip = " << hostip << ",gateway = " << gateway;
            if(!hostip.compare(gateway,Qt::CaseInsensitive))
            {
                bok = false;
                break;
            }
        }
    }

    return bok;
}
#endif

void QGenieRefreshInternetStateHandler::lookup_completed(const QHostInfo & info)
{
    if(!m_blookingup)
    {
        begin_refresh();
        return;
    }

    m_blookingup = false;

    try{

        bool bok = false;

        if(info.error()==QHostInfo::NoError)
        {
#ifdef Q_OS_WIN
            bok = verify_netgear_haj_win32(info);
#elif defined Q_OS_MACX
            bok = verify_netgear_haj_macx(info);
#endif
        }

        if(bok
#ifdef Q_OS_WIN
           && dnsquery_win(QString(domains[m_cur_domain_idx]))
#endif
           )
        {
            ok_process();
        }
        else
        {
            error_process();
        }

    }catch(...)
    {
        assert (false);
        qDebug()<<"lookup_completed catch a exception "<<m_Internet_Status;
        GeniePlugin_Internet::output_log(QString("lookup_completed catch a exception "));
    }

    //singleshot_process();

    qDebug()<<"current internet state: "<<m_Internet_Status;
    GeniePlugin_Internet::output_log(QString("lookup_completed-current internet state = %1 ").arg(int(m_Internet_Status)));
}

void QGenieRefreshInternetStateHandler::lookup_timeout()
{
    GeniePlugin_Internet::output_log("lookup_timeout invoked");

    if(!m_blookingup)
    {
        begin_refresh();
        return;
    }

    QHostInfo::abortHostLookup(m_ilookup_id);
    m_blookingup = false;

    error_process();

    //singleshot_process();

    qDebug()<<"current internet state: "<<m_Internet_Status;

}

void QGenieRefreshInternetStateHandler::ok_process()
{
    m_err_count = 0;
    if( m_Internet_Status != INTERNET_CONNECTED)
    {
        m_Internet_Status = INTERNET_CONNECTED;
        emit internet_state_changed(int(m_Internet_Status));
    }
}

void QGenieRefreshInternetStateHandler::error_process()
{
    ++m_err_count;
    if( ((m_err_count >= MAX_LOOKUPERROR_COUNT) /*|| m_bsingleshot*/)
        && (m_Internet_Status != INTERNET_DISCONNECTED))
        {
        m_Internet_Status = INTERNET_DISCONNECTED;
        emit internet_state_changed(int(m_Internet_Status));
        m_err_count = 0;
    }
}


int QGenieRefreshInternetStateHandler::internet_status()
{
    return m_Internet_Status;
}
