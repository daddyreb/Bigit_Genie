#include "GeniePlugin_Wireless.h"
#include "QGenieWirelessTabWidget.h"
#include "QGenieRefreshWirelessStateThread.h"
#include "QGenieWlanKeyPersistent.h"
#include "version.h"
#include "genie2_interface.h"

#include <QtPlugin>
#include <assert.h>


WIRELESS_STATE g_Wireless_Status = WIRELESS_CONNECTED;

QGenieRefreshWirelessStateThread   g_Refresh_WirelessStatus_Thread;

IPluginInf2* GeniePlugin_Wireless::m_single_instance = 0;

QString GeniePlugin_Wireless::m_s_ssid;

#ifdef Q_OS_WIN
#include "DiagnoseInterface.h"
extern  DiagnoseInterface  *diagnose_inf;
#elif defined Q_OS_MACX
#include "WiFiApiInterface.h"
#include "WiFiApiGlobal.h"
extern WiFiApiInterface    *wifiApi_inf;
class GenieReconnectThread:public QThread
{
public:
    GenieReconnectThread(){};
    void reconnect(const QString &ssid,const QString &pwd)
    {
        if(!isRunning())
        {
            mMutex.lock();
            mSsid=ssid;
            mPwd=pwd;
            start();
            mMutex.unlock();
        }
    }

    void run()
    {
        if(wifiApi_inf == 0)
            return;
        mMutex.lock();
        QString ssid=mSsid;
        QString pwd=mPwd;
        mMutex.unlock();

        QStringList nic_list;

        if(!wifiApi_inf->AllWLanInterfaces(nic_list)
                || (nic_list.count()<=0))
        {
            return;
        }

        wifiApi_inf->AssociateToNetwork(nic_list[0],ssid,pwd);
    };
protected:
    QString mSsid;
    QString mPwd;
    QMutex mMutex;

};
#endif



GeniePlugin_Wireless::GeniePlugin_Wireless():
    m_widget(NULL)
  ,m_rcfgBusy(false)
{
    if(0 == m_single_instance)
    {
        m_single_instance = this;
    }
#ifdef Q_OS_MACX	
    mReconnectThread=new GenieReconnectThread();
#endif
}

GeniePlugin_Wireless::~GeniePlugin_Wireless()
{
#ifdef Q_OS_MACX	
    mReconnectThread->wait();
    delete mReconnectThread;
#endif	
}

QVariant GeniePlugin_Wireless::command(int cmd,const QVariant &paras)
{
    if(cmd == I2CMD_UUID)
    {
        return QString(PLUGIN_UUID);
    }
    else if(cmd == I2CMD_VERSIONNUMBER)
    {
        return QString(VERSION_STRING);
    }
    else if(cmd == I2CMD_TIMETO_CREATEUI)
    {
        m_widget = new QGenieWirelessTabWidget();
        connect(&g_Refresh_WirelessStatus_Thread,SIGNAL(wirelessstate_changed(QString,int,int)),
                m_widget,SLOT(wirelessstate_changed(QString,int,int)));
        connect(&g_Refresh_WirelessStatus_Thread,SIGNAL(update_ssid_connected(QString)),
                m_widget,SLOT(update_ssid_connected(QString)));

        GENIE2_REGISTER_UI(m_widget,QString::number(L_WIRELESS_BIGBT) + ":1100:Wireless5:cmd1");
		GENIE2_SUBSCRIB_MESSAGE(SCCMD_ROUTERCONFIG_BUSY);
        GENIE2_SUBSCRIB_MESSAGE(SCCMD_ROUTER_MODEL);

    }
    else if(cmd == I2CMD_TIMETO_START)
    {
        GENIE2_UI_COMMAND(UICMD_LEFTBUTTON_LANGUAGE,L_WIRELESS_LEFTBT);
        if(!g_Refresh_WirelessStatus_Thread.isRunning())
        {
            g_Refresh_WirelessStatus_Thread.start();
        }

        if(m_widget){
            m_widget->update_bigbutton_ui();
        }
    }
    else if(cmd ==I2CMD_UI_ACTION)
    {
        if(m_widget)
        {
            m_widget->notifyRaise();
        }
    }
    else if(cmd ==I2CMD_NOTIFY_MESSAGE)
    {
        QVariantList varParas = paras.toList();
        if(varParas.count() > 1)
        {
            int msg = varParas[0].toInt();
            if(msg == SCCMD_ROUTER_MODEL)
            {
                m_routerModel = varParas[1].toString();
            }
            else if(msg == SCCMD_ROUTERCONFIG_BUSY)
            {
                m_rcfgBusy = varParas[1].toBool();
            }

            if(m_widget){
                bool benable = (!m_routerModel.isEmpty() && !m_rcfgBusy);
                m_widget->enableChangeChannel(benable);
                m_widget->enablegotoguestaccess(benable);
            }
        }

    }
    else if(cmd ==EPCC_CONNECT_TO_SSID)
    {
        QStringList paraList = paras.toStringList();
        Q_ASSERT(paraList.count() >= 2);

#ifdef Q_OS_WIN
        if(diagnose_inf)
        {
            diagnose_inf->TryConnect2WlanWithProfile(paraList[0]);
        }
#elif defined Q_OS_MACX
        if( wifiApi_inf )
        {
            reconnect_airport(paraList[0],paraList[1]);
        }
#endif
    }
    else if(cmd ==EPCC_SSID_KEY)
    {
        QVariantList lst=paras.toList();
        Q_ASSERT(lst.count()>=4);
        qDebug()<<"ssid,pwd,wpa,rz"<<lst[0]<<lst[1]<<lst[2]<<lst[3];

        const QString &ssid = lst[0].toString();
        const QString &pwd =  lst[1].toString();
        const QString &cipher = lst[2].toString();
        const QString &auth = lst[3].toString();

        QGenieWlanKeyPersistent::save_key(ssid,pwd);

#ifdef Q_OS_WIN
        if(diagnose_inf)
        {
            diagnose_inf->MakeWlanProfileForInterfaces(ssid,auth,cipher,pwd);
        }
#endif

    }
    return QVariant();
}

IPluginInf2* GeniePlugin_Wireless::single_instance()
{
    return m_single_instance;
}

QString GeniePlugin_Wireless::translateText(int idx)
{
    return GENIE2_GET_TEXT(idx);
}

//void GeniePlugin_Wireless::function_usage_statistics(const QString &fun ,int value)
//{
//    if(-1 == value)
//    {
//        GENIE2_STATISTICS(fun);
//    }
//    else
//    {
//        GENIE2_STATISTICS2(fun,value);
//    }
//}

void GeniePlugin_Wireless::output_log(const QString &log)
{
    GENIE2_LOG(log);
}

#ifdef Q_OS_MACX
void GeniePlugin_Wireless::reconnect_airport(const QString &ssid,const QString &pwd)
{
    mReconnectThread->reconnect(ssid,pwd);
}
#endif


