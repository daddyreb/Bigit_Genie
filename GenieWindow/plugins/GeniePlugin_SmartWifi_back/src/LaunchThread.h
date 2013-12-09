#pragma once
#include <QThread>
#include <QUrl>
#include <QTimer>
#include <QMutex>
#include "smartWifiProtocol.h"

class StatusNoticed
{
public:
	virtual void ServerStatus(QString status, QString error) = 0;
	virtual void GetServiceInfo(ServiceInfo info) = 0; 
};


class LaunchThread : public QThread
{
	//循环线程，定时获取vpn相关信息
	Q_OBJECT
public:
	LaunchThread(QUrl& url, StatusNoticed *noticed);
	~LaunchThread(void);
	
	void Exit();
	void StopRefresh();
	void SetUrl(QUrl& url);
	void StopUpdate(int waitTime = 0);

signals:
	void ServerStatus(QString status, QString error);
	void GetServiceInfo(ServiceInfo info);
	void ServiceOverdue();
	void ActiveUpdateSignal();
	void StartTimerSignal();

private slots:
	void UpdateServerInfo();   //更新一次服务器信息

private:
	void run();

	volatile bool m_isUpdating;
	StatusNoticed *m_noticed;
	QUrl m_url;
	QTimer *m_timer;
	QTimer *m_timerActiceRefresh;
};

