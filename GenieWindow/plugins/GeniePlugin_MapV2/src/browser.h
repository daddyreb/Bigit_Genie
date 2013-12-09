#ifndef BROWSER_H
#define BROWSER_H

#include <QThread>
#include <QString>
#include "howl.h"
#include <salt/debug.h>
#include <stdio.h>


class Browser : public QThread
{
    Q_OBJECT
public:
	explicit Browser(QObject *parent = 0);

	static Browser * GetInStance();

	static sw_result HOWL_API
		my_browser(
		sw_discovery						discovery,
		sw_discovery_oid					oid,
		sw_discovery_browse_status		status,
		sw_uint32							interface_index,
		sw_const_string					name,
		sw_const_string					type,
		sw_const_string					domain,
		sw_opaque_t							extra);

	static sw_result HOWL_API
		my_resolver(
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
		sw_opaque_t				extra);

	void run();

signals:
	void addService(QString strServiceName, QString strIp);
	void removeService(QString strServiceName);
	void rebootService();

public slots:
	void onSrvStarted(bool bSrvStarted);
	void onRebootService();

private:
	static Browser * m_pThis;
	bool m_bSrvStarted;
	bool m_bBrowser;
	sw_discovery discovery;
	sw_discovery_oid oid;
};

#endif // BROWSER_H
