#include "browser.h"
#include <QtDebug>


Browser * Browser::m_pThis = NULL;

Browser::Browser(QObject *parent) :
    QThread(parent)
{
    m_bSrvStarted = false;

	m_bBrowser = false;
}

Browser * Browser::GetInStance()
{
    if (m_pThis == NULL)
    {
        m_pThis = new Browser();
    }

    return m_pThis;
}

sw_result Browser::my_resolver(
                                sw_discovery			discovery,
                                sw_discovery_oid		oid,
                                sw_uint32				interface_index,
                                sw_const_string		name,
                                sw_const_string		type,
                                sw_const_string		domain,
                                sw_ipv4_address		address,
                                sw_port					port,
                                sw_octets				text_record,
                                sw_uint32				text_record_len,
                                sw_opaque_t				extra)
{
	Q_UNUSED(extra);
	Q_UNUSED(port);
	Q_UNUSED(domain);
	Q_UNUSED(type);
	Q_UNUSED(interface_index);

	qDebug() << "resolver name:" << name;

	sw_discovery_cancel(discovery, oid);

	sw_text_record_iterator	it;
	sw_int8	name_buf[16];
	sw_result err = SW_OKAY;

	emit m_pThis->addService(name, sw_ipv4_address_name(address, (char *)name_buf, 16));
	if ((text_record_len > 0) && (text_record) && (*text_record != '\0'))
	{
		err = sw_text_record_iterator_init(&it, text_record, text_record_len);
		sw_check_okay(err, exit);

		err = sw_text_record_iterator_fina(it);
		sw_check_okay(err, exit);
	}
exit:
	return err;
}


sw_result Browser::my_browser(
                                sw_discovery						discovery,
                                sw_discovery_oid					oid,
                                sw_discovery_browse_status		status,
                                sw_uint32							interface_index,
                                sw_const_string					name,
                                sw_const_string					type,
                                sw_const_string					domain,
                                sw_opaque_t							extra)
{
	
	Q_UNUSED(extra);
	Q_UNUSED(oid);

	sw_discovery_resolve_id rid;

	qDebug() << "browser name:" << name << " type:" << type;
	switch (status)
	{
	case SW_DISCOVERY_BROWSE_ADD_SERVICE:
		{
			sw_discovery_resolve(discovery, interface_index, name, type, domain, my_resolver, NULL, &rid);
		}
		break;
	case SW_DISCOVERY_BROWSE_REMOVE_SERVICE:
		{
			emit m_pThis->removeService(name);
		}
		break;
	default:
		break;
	}

	return SW_OKAY;
}

void Browser::run()
{
    int iTimes = 0;
    while(true)
    {
        qDebug() << "start browser";
        if (m_bSrvStarted)
        {
            if (SW_OKAY == sw_discovery_init(&discovery))
            {
                if (SW_OKAY == sw_discovery_browse(discovery, 0, "_ros-master._tcp", NULL, my_browser, NULL, &oid))
                {
					m_bBrowser = true;
                    if (SW_OKAY == sw_discovery_run(discovery))
                    {
                        break;
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
       
        Sleep(1000);
    }
}

void Browser::onSrvStarted(bool bSrvStarted)
{
    m_bSrvStarted = bSrvStarted;
}

void Browser::onRebootService()
{
//	m_bSrvStarted = false;
//	if (m_bBrowser && discovery)
//	{
//		sw_discovery_stop_run(discovery);
//		m_bBrowser = false;
//		start();
//	}
}
