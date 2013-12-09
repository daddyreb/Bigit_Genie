#include "QGenieRefreshStatusThread.h"
#include "GeniePlugin_Internet.h"


#include "QGenieRefreshInternetStateHandler.h"

#include <QHostInfo>
#include <QStringList>
#include <QtDebug>

#ifdef Q_OS_WIN
#include "DiagnoseInterface.h"
extern DiagnoseInterface *diagnose_inf;
#elif defined Q_OS_MACX
#include "INetApiInterface.h"
extern INetApiInterface *inet_api;
#endif

#include "genie2_interface.h" //kai.yan for log output
#include "QGenieConnectionStateDetecter.h"
#ifdef Q_OS_MACX
#define DETECT_TIME_INTERVAL_MS     2000 //2 sencodes
#elif defined Q_OS_WIN
#define DETECT_TIME_INTERVAL_MS     30 * 1000 //1 seconds
#endif

#define MAX_ERROR_COUNT             3 * 4

QGenieRefreshStatusThread::QGenieRefreshStatusThread(QObject *parent):
        QThread(parent),
        m_bexit(false),
        m_state_changed(false),
        m_Internet_Status(INTERNET_CONNECTED),
        m_detecter(NULL)
{
    m_detecter = new QGenieConnectionStateDetecter();
}

QGenieRefreshStatusThread::~QGenieRefreshStatusThread()
{
    this->wait();
    delete m_detecter;
}


void QGenieRefreshStatusThread::end_refresh()
{
    m_bexit = true;
}


void QGenieRefreshStatusThread::run()
{
    static int err_count = 0;
	bool isMaxError;
    emit internet_state_changed(int(m_Internet_Status));
    while(!m_bexit
#ifdef Q_OS_WIN
          && (diagnose_inf != 0)
#endif
          )
    {
		isMaxError = false;
        if(m_detecter->is_connection_ok())
        {
            err_count = 0;
            //qDebug()<<"is_connection_ok:"<<true;
            if( m_Internet_Status != INTERNET_CONNECTED)
            {
                m_Internet_Status = INTERNET_CONNECTED;
                m_state_changed = true;
            }
        }
        else
        {
            ++err_count;
			qDebug()<<"err_count:"<<err_count;
            if(err_count >= MAX_ERROR_COUNT)
            {
				isMaxError = true;
                err_count = 0;
				//qDebug()<<"err_count:"<<err_count<<"INTERNET_DISCONNECTED:"<<false;

				if(m_Internet_Status != INTERNET_DISCONNECTED)
                {
                    m_Internet_Status = INTERNET_DISCONNECTED;
                    m_state_changed = true;
                }
            }
        }
		if(m_state_changed)
		{
			m_state_changed = false;
			emit internet_state_changed(int(m_Internet_Status));
		}
		if(err_count == 0)
		{
			if(isMaxError)
			{
				qDebug()<<"err_count >12 wait 2s....";
				for(int i = 0; i < 2000 && !m_bexit;i += 100)
				{
					QThread::msleep(100);
				}
			}else
			{
				qDebug()<<"wait 30s......";
				for(int i = 0; i < DETECT_TIME_INTERVAL_MS && !m_bexit;i += 100)
				{
					QThread::msleep(100);
				}
			}
			
		}else if( (4 == err_count) || (8 == err_count))
		{
			qDebug()<<"err_count =4 ||8 wait 1s......";
			for(int i=0; i< 1000 && !m_bexit;i+=100)
			{
				QThread::msleep(100);
			}
		}
    }

    //qDebug("Refresh internet state thread exit ok.");
    //GeniePlugin_Internet::output_log(QString("Refresh internet state thread exit ok."));
}
