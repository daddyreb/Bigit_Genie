#include "QGenieRefreshWirelessStateThread.h"
#include "GeniePlugin_Wireless.h"
#include <QtDebug>


#ifdef Q_OS_WIN
#include "DiagnoseInterface.h"
extern DiagnoseInterface *diagnose_inf ;
#elif defined Q_OS_MACX
#include "WiFiApiGlobal.h"
#include "WiFiApiInterface.h"
extern WiFiApiInterface  *wifiApi_inf;
#endif

#define WARNNING_LEVEL_SIGNALQUANLITY           80
#define ERROR_LEVEL_SIGNALQUANLITY              20

extern WIRELESS_STATE       g_Wireless_Status;

QGenieRefreshWirelessStateThread::QGenieRefreshWirelessStateThread(QObject *parent) :
        QThread(parent),
        m_bexit(false),
        m_cur_signalLevel(0)
{
}

QGenieRefreshWirelessStateThread::~QGenieRefreshWirelessStateThread()
{
    this->wait();
    qDebug("QGenieRefreshWirelessStateThread destructed");
    //GeniePlugin_Wireless::output_log(QString("QGenieRefreshWirelessStateThread destructed"));
}

bool QGenieRefreshWirelessStateThread::begin_refresh()
{
    if(this->isRunning())
    {
        GeniePlugin_Wireless::output_log("refresh wireless state thread is Running");
        return false;
    }
    else
    {
        m_bexit = false;
        this->start();
        return true;
    }
}

void QGenieRefreshWirelessStateThread::end_refresh()
{
    m_bexit = true;
}

void QGenieRefreshWirelessStateThread::run()
{
    QString nic_temp;

    if(
#ifdef Q_OS_WIN
            0 == diagnose_inf
#elif defined Q_OS_MACX
            0 == wifiApi_inf
#endif
            )
    {
        if(g_Wireless_Status != WIRELESS_NOTENABLED)
        {
            g_Wireless_Status = WIRELESS_NOTENABLED;
            emit wirelessstate_changed(QString(),g_Wireless_Status);
        }
        //GeniePlugin_Wireless::output_log("refresh wireless state thread 0 == diagnose_inf");
        return;
    }


    QStringList nic_list;

    while(!m_bexit)
    {
        nic_list.clear();
        nic_temp.clear();

        if(
#ifdef Q_OS_WIN
                !diagnose_inf->GetWiFiInterfaces(nic_list) || nic_list.size()==0
#elif defined Q_OS_MACX
                !wifiApi_inf->AllWLanInterfaces(nic_list) || nic_list.size()==0
#endif
                )
        {
            if(g_Wireless_Status != WIRELESS_NOTENABLED)
            {
                g_Wireless_Status = WIRELESS_NOTENABLED;
                emit wirelessstate_changed(QString(),g_Wireless_Status);
            }
            //GeniePlugin_Wireless::output_log("refresh wireless state thread enum interface failed ,or no adpter");
            goto wait_pos;
        }

        for(int y = 0;y < nic_list.size();++y)
        {
            if(
#ifdef Q_OS_WIN
                    (1==diagnose_inf->IsWlanNicConnected(nic_list[y]))
#elif defined Q_OS_MACX
                    wifiApi_inf->IsWLanNicConnected(nic_list[y])
#endif
                    && (y != 0))
            {
                nic_list.swap(0,y);
                break;
            }
        }

        for(int i=0 ; i < nic_list.size() ; ++i)
        {
            nic_temp = nic_list[i];

            if(
#ifdef Q_OS_WIN
                    ((diagnose_inf->IsWlanNicConnected(nic_list[i]) < 0))
#elif defined Q_OS_MACX
                    (!wifiApi_inf->IsWlanNicPowerOn(nic_list[i])) || (!wifiApi_inf->IsWlanNicActive(nic_list[i]))
#endif
                    )
            {
                if(g_Wireless_Status != WIRELESS_NOTENABLED)
                {
                    g_Wireless_Status = WIRELESS_NOTENABLED;
                    emit wirelessstate_changed(nic_temp,g_Wireless_Status);
                }
                //GeniePlugin_Wireless::output_log(QString("refresh wireless state thread nic %1:IsWlanNicConnected(nic_list[i].m_devguid) < 0").arg(nic_list[i].m_devdesc));
            }
            else
            {
                if(
#ifdef Q_OS_WIN
                        1 == diagnose_inf->IsWlanNicConnected(nic_list[i])
#elif defined Q_OS_MACX
                        wifiApi_inf->IsWLanNicConnected(nic_list[i])
#endif
                        )
                {
#ifdef Q_OS_WIN
                    int strength = diagnose_inf->NicConnectedNetworkSignal(nic_list[i]);
                    int signal_level =(strength<=0?0: (strength-1)/20 +1);
#elif defined Q_OS_MACX
                    int signal_level = wifiApi_inf->RunningStateWlanNicSignalLevel(nic_list[i]);
#endif

                    //not detect signal_level so set default value to 5
                    signal_level = (0 == signal_level?(0 == m_cur_signalLevel?5:m_cur_signalLevel):signal_level);
                    if(g_Wireless_Status != WIRELESS_CONNECTED || m_cur_signalLevel != signal_level)
                    {
                        g_Wireless_Status = WIRELESS_CONNECTED;
                        m_cur_signalLevel = signal_level;

                        emit wirelessstate_changed(nic_temp,g_Wireless_Status
                                                   ,m_cur_signalLevel);
                    }
                    //GeniePlugin_Wireless::output_log(QString("refresh wireless state thread nic %1 connected").arg(nic_list[i].m_devdesc));
                }
                else
                {
                    if(g_Wireless_Status != WIRELESS_NOTCONNECTED)
                    {
                        g_Wireless_Status = WIRELESS_NOTCONNECTED;
                        emit wirelessstate_changed(nic_temp,g_Wireless_Status);
                    }
                    //GeniePlugin_Wireless::output_log(QString("refresh wireless state thread  nic %1 not connected").arg(nic_list[i].m_devdesc));
                }
            }

            break;
        }

        if(0 == nic_list.size())//have no wireless adapter
        {
            if(g_Wireless_Status != WIRELESS_NOTENABLED)
            {
                g_Wireless_Status = WIRELESS_NOTENABLED;
                emit wirelessstate_changed(QString(),g_Wireless_Status);
            }
            //GeniePlugin_Wireless::output_log(QString("refresh wireless state thread  no wireless adpter"));
        }

        //qDebug()<<"wireless current state:"<<g_Wireless_Status;


        wait_pos:


		QString cur_ssid;

#ifdef Q_OS_WIN
		if(diagnose_inf && (!nic_temp.isEmpty()))
		{
			diagnose_inf->GetWlanNicCurConnectedSsid(nic_temp,cur_ssid);
		}
#elif defined Q_OS_MACX
		if(wifiApi_inf && (!nic.isEmpty()))
		{
			wifiApi_inf->GetWLanInterfaceCurSsid(nic_temp,cur_ssid);
		}
#endif

        emit update_ssid_connected(cur_ssid);

        //wait 0.5 seconds
        msleep(500);
    }

    qDebug()<<"refresh wireless state thread exit ok.";
    //GeniePlugin_Wireless::output_log(QString("refresh wireless state thread exit ok."));
}
