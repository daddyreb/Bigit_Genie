#include "QGenieWzcMonitorThread.h"
#include "GeniePlugin_Wireless.h"
#include "DiagnoseInterface.h"
#include <QStringList>

extern DiagnoseInterface *diagnose_inf;

QGenieWzcMonitorThread::QGenieWzcMonitorThread(QObject *parent) :
        QThread(parent),m_iwzc_runningstate(-1),m_iwzc_autoconfstate(-1),m_bexit(false)
{
    //this->setTerminationEnabled(true);
}

QGenieWzcMonitorThread::~QGenieWzcMonitorThread()
{
    this->wait();
    qDebug("QGenieWzcMonitorThread destructed");
    GeniePlugin_Wireless::output_log(QString("QGenieWzcMonitorThread destructed"));
}

void QGenieWzcMonitorThread::end_monitor()
{
    m_bexit = true;
}

void QGenieWzcMonitorThread::run()
{
    if(diagnose_inf)
    {
        while(!m_bexit)
        {
            int ires = diagnose_inf->IsWzcServiceRunning2();
            if(ires!=-1)
            {
                if(ires!=m_iwzc_runningstate)
                {
                    m_iwzc_runningstate = ires;
                    emit wzc_runningstate_changed(1==m_iwzc_runningstate);
                }

                if(0==ires)
                {
                    if(diagnose_inf->StartWzcService())
                    {
                        if(m_iwzc_runningstate!=1)
                        {
                            m_iwzc_runningstate = 1;
                            emit wzc_runningstate_changed(true);
                        }
                    }
                }
            }

            if(1==m_iwzc_runningstate)
            {
                QStringList nic_list;
                int         temp_res = -1;
                if(diagnose_inf->GetWiFiInterfaces(nic_list))
                {
                    for(int i=0;i<nic_list.size();++i)
                    {
                        temp_res = diagnose_inf->IsWlanAutoConfigEnabled(nic_list[i]);
                        if(temp_res!=-1)
                        {
                            if(temp_res!=m_iwzc_autoconfstate)
                            {
                                m_iwzc_autoconfstate = temp_res;
                                emit wzc_autoconfstate_changed(nic_list[i],1 == m_iwzc_autoconfstate);
                            }

                            if(0==temp_res)
                            {
                                if(diagnose_inf->EableWlanAutoConfig(nic_list[i]))
                                {
                                    if(m_iwzc_autoconfstate!=1)
                                    {
                                        m_iwzc_autoconfstate = 1;
                                        emit wzc_autoconfstate_changed(nic_list[i],true);
                                    }
                                }
                            }
                        }
                    }//for
                }

            }

            int times = 4;
            do
            {
                if(m_bexit)
                {
                    break;
                }
                msleep(500);
            }while(--times);

            if(m_bexit)
            {
                break;
            }
        }//while
    }

    qDebug("wzc monitor thread exit ok.");
    //GeniePlugin_Wireless::output_log(QString("wzc monitor thread exit ok."));
}
