#include "howlpublish.h"
#include <QtDebug>

extern sw_uint16 g_serviceport;

static sw_result HOWL_API
my_service_reply(
                                sw_discovery						discovery,
                                sw_discovery_oid					oid,
                                sw_discovery_publish_status	status,
                                sw_opaque							extra)
{
	Q_UNUSED(extra);
	Q_UNUSED(oid);
	Q_UNUSED(status);
	Q_UNUSED(discovery);
	static sw_string
		status_text[] =
	{
		"Started",
		"Stopped",
		"Name Collision",
		"Invalid"
	};

	//fprintf(stderr, "publish reply: %s\n", status_text[status]);
	return SW_OKAY;
}


howlpublish::howlpublish(QObject *parent) :
    QThread(parent)
{
    m_bSrvStarted = false;
    m_bPublished = false;
}


void howlpublish::run()
{
    int iTimes = 0;
    sw_result result;
    while(true)
    {
        qDebug() << "start publish";
        if (m_bSrvStarted)
        {
            if (SW_OKAY == sw_discovery_init(&discovery))
            {
                sw_text_record  text_record;
                if (SW_OKAY == sw_text_record_init(&text_record))
                {
                    char ac[80];
                    gethostname(ac, sizeof(ac));
                    strcat_s(ac, "[Windows]");
                    if ((result = sw_discovery_publish(discovery, 0, ac, "_ros-master._tcp", NULL, NULL, g_serviceport, sw_text_record_bytes(text_record), sw_text_record_len(text_record), my_service_reply, NULL, &id)) == SW_OKAY)
                    {
                        qDebug() << "publish success";
                        m_bPublished = true;
                        break;
                    }
					else
					{
						iTimes++;
					}
                }
				else
				{
					iTimes++;
				}
            }
			else
			{
				iTimes++;
			}
        }
        Sleep(2000);
    }
}

void howlpublish::onSrvStarted(bool bSrvStarted)
{
    m_bSrvStarted = bSrvStarted;
}

void howlpublish::onRebootService()
{
	m_bSrvStarted = false;
	if (m_bPublished)
	{
		cancelPublish();
		start();
	}
}

bool howlpublish::cancelPublish()
{
    qDebug() << "cancel publish"; 
    if (NULL != discovery && m_bPublished)
    {
        sw_discovery_stop_run(discovery);
        if (SW_OKAY == sw_discovery_cancel(discovery, id))
        {
            m_bPublished = false;
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }

}
