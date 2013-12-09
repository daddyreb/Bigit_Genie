#include "QGenieConnectToWlanThread.h"
#include <assert.h>

#ifdef Q_OS_WIN

#include "DiagnoseInterface.h"
extern DiagnoseInterface *diagnose_inf ;

#elif defined Q_OS_MACX

#include "WiFiApiGlobal.h"
#include "WiFiApiInterface.h"
extern WiFiApiInterface  *wifiApi_inf;

#endif

QGenieConnectToWlanThread::QGenieConnectToWlanThread(QObject *parent) :
        QThread(parent),m_buseprofile(false)
{
}

QGenieConnectToWlanThread::~QGenieConnectToWlanThread()
{
    this->wait();
}

bool QGenieConnectToWlanThread::begin_connect(const QString &nicguid,
                                              const QString &ssid,const QString &pwd,bool buseprofile)
{
#ifdef Q_OS_WIN
    if( 0 == diagnose_inf )
    {
        return false;
    }
#elif defined Q_OS_MACX
    if( 0 == wifiApi_inf)
    {
        return false;
    }
#endif

    if(this->isRunning()|| nicguid.isEmpty()|| ssid.isEmpty()/* || pwd.isEmpty() */)
    {
        return false;
    }
    else
    {
        m_strnicguid   = nicguid;
        m_strssid      = ssid;
        m_strpwd       = pwd;
        m_buseprofile  = buseprofile;
        this->start();
        return true;
    }

}


void QGenieConnectToWlanThread::run()
{
#ifdef Q_OS_WIN
    bool bassociate = false;

    if( (0 == diagnose_inf) ||  m_strnicguid.isEmpty() || m_strssid.isEmpty() /*|| m_strpwd.isEmpty()*/ )
    {
        emit connect_complete(false);
        return;
    }

    if(m_buseprofile)
    {
        //        if(diagnose_inf->TryConnect2WlanWithProfile(m_strssid))
        //        {
        //            QThread::msleep(5000);
        //
        //            emit connect_complete(true);
        //        }
        //        else
        //        {
        //            emit connect_complete(false);
        //        }
        bassociate = diagnose_inf->TryConnect2WlanWithProfile(m_strssid);
        int times = 30;
        if(bassociate)
        {
            bassociate = false;

            do
            {
                if(1 == diagnose_inf->IsWlanNicConnected(m_strnicguid))
                {
                    bassociate = true;
                    break;
                }

                QThread::msleep(1000);

            }while(--times);
        }
    }
    else
    {
        //        if(diagnose_inf->ConnectToWlan(m_strnicguid,m_strssid,m_strpwd))
        //        {
        //            emit connect_complete(true);
        //        }
        //        else
        //        {
        //            emit connect_complete(false);
        //        }
        bassociate = diagnose_inf->ConnectToWlan(m_strnicguid,m_strssid,m_strpwd,false);
    }

    if(bassociate)
    {
        emit connect_complete(waitdhcp_forgateway(m_strnicguid));
    }
    else
    {
        emit connect_complete(bassociate);
    }

#elif defined Q_OS_MACX

    bool bassociate = false;

    if((0 == wifiApi_inf) ||  m_strnicguid.isEmpty() || m_strssid.isEmpty() )
    {
        emit connect_complete(bassociate);
        return;
    }

    if(m_buseprofile)
    {
        //mac os x can never reach here
        assert (false);
    }
    else
    {
        bassociate = wifiApi_inf->AssociateToNetwork(m_strnicguid,m_strssid,m_strpwd);
    }

    emit connect_complete(bassociate);

#endif

}

#ifdef Q_OS_WIN
bool QGenieConnectToWlanThread::waitdhcp_forgateway(const QString& nicguid)
{
    bool bret = false;
    int times = 90;
    QStringList gateway_list;
    do
    {
        gateway_list.clear();
        if(diagnose_inf->GetInterfaceGateway(nicguid,gateway_list))
        {
            if(gateway_list.count()!=0 && gateway_list[0]!=QString("0.0.0.0"))
            {
                bret = true;
                break;
            }
        }
        QThread::msleep(1000);
    }while(--times);

    return bret;
}
#endif
