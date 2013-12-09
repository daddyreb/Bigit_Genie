#ifndef _ADD_WHITEURLTHREAD_H_
#define _ADD_WHITEURLTHREAD_H_
#include <QThread>
#include "SmartWifiConnector.h"

class AddWhiteUrlThread : public QThread
{
	//添加白名单线程
	Q_OBJECT
public:
	AddWhiteUrlThread(QUrl server_url)
	{ 
		m_server_url = server_url; 
		m_url = "";
		m_type = ADD_UNFORCE;
	};
	void SetUrl(QString url) 
	{ 
		m_url = url;
	};
	void SetAddType(ADD_TYPE type)
	{
		m_type = type;
	}
signals:
	void FinishAddWhiteUrl(bool success, QString url, QString error);

private:
	void run()
	{
		SmartWifiConnector connector;
		connector.SetReplyTimeout(100);
		QString error;
		bool success = connector.AddWhiteUrl(m_server_url, m_type, m_url, error);
		emit FinishAddWhiteUrl(success, m_url, error);
	}
	QString m_url;
	QUrl m_server_url;
	ADD_TYPE m_type;
};
#endif