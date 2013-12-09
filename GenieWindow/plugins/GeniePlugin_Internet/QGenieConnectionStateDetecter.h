#ifndef QGENIECONNECTIONSTATEDETECTER_H
#define QGENIECONNECTIONSTATEDETECTER_H
#include <QString>
#include <QByteArray>
#include <QStringList>
#include <QHostInfo>
#include <QEventLoop>
#include "LookUpThread.h"
class QGenieConnectionStateDetecter:QObject
{
	Q_OBJECT
public:
    QGenieConnectionStateDetecter();
    virtual ~QGenieConnectionStateDetecter();
protected:
    int             m_server_index;
	bool m_isDNS_OK;
	LookUpThread m_lookupThread;
private:
	QEventLoop* m_DNS_loop;
protected:
#ifdef Q_OS_MACX
    void generate_request(const QString& dns_name,QByteArray &request);
    bool is_wlan_ok();
    bool is_dnscfg_ok();
#endif

#ifdef Q_OS_WIN
    bool is_wlan_ok2();
    bool is_dnscfg_ok2();
#endif

private:
#ifdef Q_OS_MACX
    void get_dnsservers(QStringList &dns_list);
#endif
public:
    bool is_connection_ok();
private	slots:
	void lookedUp(bool);
};

#endif // QGENIECONNECTIONSTATEDETECTER_H
