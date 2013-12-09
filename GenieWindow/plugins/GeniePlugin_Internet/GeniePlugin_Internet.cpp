#include "GeniePlugin_Internet.h"
#include "genie2_interface.h"
#include "QGenieWlanKeyPersistent.h"
#include "version.h"

#include <QtPlugin>
#include <QDebug>
#include <assert.h>

IPluginInf2* GeniePlugin_Internet::m_single_instance  = 0;

#ifdef GENIE_ENABLE_NET_MONITOR
void GeniePlugin_Internet::checkNetMonitor()
{
    if (!m_netMonitor.testLoop()) {
        qDebug("NetMonitor::testLoop() failed, restarting...");
        m_netMonitor.stop();
        m_netMonitor.start();
    }
}

void GeniePlugin_Internet::forceRestartNetMonitor()
{
    qDebug("NetMonitor::testLoop() force restarting...");
    m_netMonitor.stop();
    m_netMonitor.start();
}

NetMonitor *GeniePlugin_Internet::getNetMonitor()
{
    return &static_cast<GeniePlugin_Internet*>(m_single_instance)->m_netMonitor;
}
#endif // GENIE_ENABLE_NET_MONITOR

GeniePlugin_Internet::GeniePlugin_Internet()
{
    if(0 == m_single_instance)
    {
        m_single_instance = this;
    }
    net_diagnose = 0;

#ifdef GENIE_ENABLE_NET_MONITOR
    connect(&m_nmTestTimer, SIGNAL(timeout()), SLOT(checkNetMonitor()));
    connect(&m_nmForceTimer, SIGNAL(timeout()), SLOT(forceRestartNetMonitor()));
    m_nmTestTimer.setInterval(1000);
    m_nmForceTimer.setInterval(20 * 1000);
    m_nmTestTimer.start();
    m_nmForceTimer.start();
    m_netMonitor.start();
#endif // GENIE_ENABLE_NET_MONITOR
}

QVariant GeniePlugin_Internet::command(int cmd,const QVariant &paras)
{
    if(cmd == I2CMD_UUID){
        return QString(PLUGIN_UUID);
    }
    else if(cmd == I2CMD_VERSIONNUMBER){
        return QString(VERSION_STRING);
    }
    else if(cmd == I2CMD_TIMETO_CREATEUI){
        net_diagnose = new QGenieNetworkDiagnose();
        GENIE2_REGISTER_UI(net_diagnose,QString::number(L_INTERNET_BIGBT) + ":1000:Internet:cmd1");
        GENIE2_SUBSCRIB_MESSAGE(SCCMD_ROUTER_MODEL);
    }
    else if(cmd == I2CMD_TIMETO_START){
        GENIE2_UI_COMMAND(UICMD_LEFTBUTTON_LANGUAGE,L_INTERNET_LEFTBT);
        if(net_diagnose)
        {
            net_diagnose->start_refresh_status_thread();
        }
    }
    else if(cmd ==I2CMD_UI_ACTION){
        if(net_diagnose)
        {
            net_diagnose->notify_raise();
        }
    }
    else if(cmd == I2CMD_UI_LOWER){
        if (net_diagnose)
            net_diagnose->notify_lower();
    }
    else if(cmd ==I2CMD_NOTIFY_MESSAGE){
        QList<QVariant> list = paras.toList();
        if(net_diagnose && (list.size() > 0)){

            bool convertOk = false;
            if((list[0].toInt(&convertOk) == SCCMD_ROUTER_MODEL) && convertOk){
                output_log(QString("command :cmd ==I2CMD_NOTIFY_MESSAGE#SCCMD_ROUTER_MODEL"));
                net_diagnose->update_routertype();
            }
        }
        qDebug()<<"I2CMD_NOTIFY_MESSAGE"<<paras;
    }
    return QVariant();
}

IPluginInf2 *GeniePlugin_Internet::single_instance()
{
    return m_single_instance;
}

QString GeniePlugin_Internet::translateText(int idx)
{
    return GENIE2_GET_TEXT(idx);
}

void GeniePlugin_Internet::function_usage_statistics(const QString &fun)
{
    GENIE2_STATISTICS(fun);
}

void GeniePlugin_Internet::output_log(const QString &log)
{
    GENIE2_LOG(log);
}

//QString GeniePlugin_Internet::infomanager_getinfo(int what ,int para)
//{
//    if(m_infomanager)
//    {
//        return m_infomanager->getInfo(what,para);
//    }
//    else
//    {
//        return QString();
//    }
//}


