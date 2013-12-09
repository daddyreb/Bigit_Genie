#include "QGenieGetRouterWlanInfoThread.h"
#include "GeniePlugin_WSetupPlugin.h"
#include "IManagerHandler.h"
#include "ISoapManager.h"


#ifdef PLATFORM_WIN32
#include "WSetupApiInterface.h"
extern WSetupApiInterface * setupapi_inf;
#elif defined PLATFORM_MACOSX

#endif



extern IManagerHandler     *g_manager_handler;
QGenieGetRouterWlanInfoThread::QGenieGetRouterWlanInfoThread(QObject *parent) :
        QThread(parent),m_soapapi(0),m_bstart_ok(false)
        ,m_bgetssid_ok(false),m_bgetkey_ok(false),m_bcontinue(true)
{
    this->setTerminationEnabled(true);
    if(g_manager_handler )
    {
        ISoapManager  * soap_manager = g_manager_handler->getSoapManager();
        if(soap_manager)
            m_soapapi=soap_manager-> getSoapAPI();
        if(m_soapapi)
            connect(m_soapapi,SIGNAL(getResponse(QtSoapMessage&,QUuid&,QString&))
                    ,this,SLOT(getResponse(QtSoapMessage&,QUuid&,QString&)));
    }
}

QGenieGetRouterWlanInfoThread::~QGenieGetRouterWlanInfoThread()
{
    this->wait();
}

void QGenieGetRouterWlanInfoThread::run()
{
    if(0==m_soapapi
#ifdef PLATFORM_WIN32
       && 0==setupapi_inf
#endif
       )
    {
        emit getwlaninfo_complete(false,QString(),QString(),QString(),QString());
        return;
    }

    //initialize
    m_bstart_ok         = false;
    m_bgetssid_ok       = false;
    m_bgetkey_ok        = false;
    m_bcontinue         = true;

    m_strnicguid.clear();
    m_strprofilename.clear();
    m_strssid.clear();
    m_strkey.clear();
    //initialize end

    if(false/*setupapi_inf && setupapi_inf->get_export_information(m_strnicguid,m_strssid,m_strprofilename)*/)
    {
         emit getwlaninfo_complete(true,m_strssid,QString(),m_strnicguid,m_strprofilename);
    }
    else
    {
        QString  str_session("f8018c9f-5431-497e-ac5d-511dd0ae7402");
        //get wlan exporting information from router
//        //Start
//        QString start_module("DeviceConfig");
//        QString start_method("ConfigurationStarted");
//        QMap<QString,QString> start_param;
//        QUuid uuid_start ("{14E65BB7-0D3B-4c90-871E-187E58D6712C}");
//        start_param.insert(QString("NewSessionID"), QString("99999999999"));
//        m_soapapi->submitrequest(start_module,start_method ,start_param,uuid_start,
//                                str_session );
//        GeniePlugin_WSetupPlugin::output_log(QString("submit:DeviceConfig ConfigurationStarted UUID:{14E65BB7-0D3B-4c90-871E-187E58D6712C}"));

        QMap<QString,QString> search_param;
        QString  search_module("WLANConfiguration");
        QString  search_method("GetSSID");
        QUuid uuid_ssid( "{CFE911FE-C5E8-4a0e-A073-6FD28B017E41}");
        m_soapapi->submitrequest(search_module,search_method , search_param,uuid_ssid,str_session);
        GeniePlugin_WSetupPlugin::output_log(QString("submit:WLANConfiguration GetSSID UUID:{CFE911FE-C5E8-4a0e-A073-6FD28B017E41}"));

        search_method = "GetWPASecurityKeys";
        QUuid uuid_key("{B8D2DCFD-0A58-4e46-882A-5EB1DC77EBB4}");
        m_soapapi->submitrequest(search_module,search_method , search_param,uuid_key,str_session);
        GeniePlugin_WSetupPlugin::output_log(QString("submit:WLANConfiguration GetWPASecurityKeys UUID:{B8D2DCFD-0A58-4e46-882A-5EB1DC77EBB4}"));
    }
}

bool QGenieGetRouterWlanInfoThread::begind_getwlaninfo()
{
    if(this->isRunning() ||
       (0==m_soapapi
#ifdef PLATFORM_WIN32
       && 0==setupapi_inf
#endif
       )
      )
        return false;
    else
    {
        this->start();
        return true;
    }
}


void QGenieGetRouterWlanInfoThread::getResponse(QtSoapMessage &message,QUuid &uuid,QString &session)
{
    GeniePlugin_WSetupPlugin::output_log(QString("recv:get wlan infomation response,uuid-%1").arg(uuid.toString()));
    if(!m_bcontinue)
        return;

/*    if(uuid == QUuid("{14E65BB7-0D3B-4c90-871E-187E58D6712C}"))//start
    {
        if(message.isFault())
        {
            qDebug("Error: %s", message.faultString().value().toString().toLatin1().constData());
        }
        else
        {
            if( message.returnResponseCode().toString().toInt() == 0)
            {
                m_bstart_ok = true;
            }
        }

        if(!m_bstart_ok)
        {
            m_bcontinue = false;
            emit getwlaninfo_complete(false,QString(),QString(),QString(),QString());
        }
    }
    else */if(uuid == QUuid("{CFE911FE-C5E8-4a0e-A073-6FD28B017E41}"))//ssid
    {
        if(message.isFault())
        {
            qDebug("Error: %s", message.faultString().value().toString().toLatin1().constData());
        }
        else
        {
            if( message.returnResponseCode().toString().toInt() == 0)
            {
                m_bgetssid_ok = true;
                const QtSoapType &value = message.returnValue();
                m_strssid = value["NewSSID"].value().toString();
            }
        }

        if(!m_bgetssid_ok)
        {
            m_bcontinue = false;
            emit getwlaninfo_complete(false,QString(),QString(),QString(),QString());
        }

    }
    else if(uuid == QUuid("{B8D2DCFD-0A58-4e46-882A-5EB1DC77EBB4}"))//key
    {
        if(message.isFault())
        {
            qDebug("Error: %s", message.faultString().value().toString().toLatin1().constData());
        }
        else
        {
            if( message.returnResponseCode().toString().toInt() == 0)
            {
                m_bgetkey_ok = true;
                const QtSoapType &value = message.returnValue();
                m_strkey = value["NewWPAPassphrase"].value().toString();
            }
        }

        if(!m_bgetkey_ok)
        {
            m_bcontinue = false;
            emit getwlaninfo_complete(false,QString(),QString(),QString(),QString());
        }
    }


    if(m_bgetssid_ok && m_bgetkey_ok)
    {
        m_bcontinue = false;
        emit getwlaninfo_complete(true,m_strssid,m_strkey,QString(),QString());
    }
}
