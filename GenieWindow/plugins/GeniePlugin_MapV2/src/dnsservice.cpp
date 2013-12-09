#include "dnsservice.h"
#include "howl.h"

#include "mDNSServant.h"

#include <QtDebug>

static HANDLE	g_console_event	=	NULL;
static sw_mdns_servant	g_servant;

sw_uint16 g_serviceport = 5335;

static DWORD
start_mDNSResponder()
{
        //sw_uint16 port	= 5335;

		while(true)
		{
            if (sw_mdns_servant_new(&g_servant, g_serviceport, NULL, 0) != SW_OKAY)
			{
                g_serviceport++;
				continue;
			}
			else
			{
                break;
			}
        }

        return S_OK;
}

static DWORD
stop_mDNSResponder()
{
        DWORD ret = S_OK;

        if (sw_mdns_servant_delete(g_servant) != SW_OKAY)
        {
                ret = ERROR;
        }

        return ret;
}

dnsservice::dnsservice(QObject *parent) :
    QThread(parent)
{
}


void dnsservice::run()
{
    qDebug() << "-------------------start dnsservice";
    while(true)
    {
        if (start_mDNSResponder() != S_OK)
        {
            qDebug() << "-----------------------failed";
            continue;
        }
        else
        {
            emit srvStarted(true);
            qDebug() << "--------------------------dnsservice success";
            break;
        }
    }
}

void dnsservice::stopService()
{
    int iTimes = 0;
    while(iTimes < 10)
    {
        if (stop_mDNSResponder() != S_OK)
        {
            qDebug() << "stop failed";
            continue;
        }
        else
        {
            qDebug() << "stop success";
            break;
        }
        iTimes++;
    }
}

void dnsservice::onRebootService()
{
    g_serviceport++;
	stopService();
	start();
}
