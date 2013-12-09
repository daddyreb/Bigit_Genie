#pragma once
#include <QThread>
#include <QUrl>
#include <QTimer>
#include <QMutex>
#include "smartWifiProtocol.h"
#include "global.h"

class SmartWifiConnector;

class LaunchThread : public QThread
{
	//ѭ���̣߳���ʱ��ȡVPN·���������Ϣ
	Q_OBJECT
public:
	LaunchThread(QUrl& url);
	~LaunchThread(void);
	
	void Exit();
	void StopRefresh();
	void SetUrl(QUrl& url);
	void StopUpdate(int waitTime = 0);

signals:
	void ServerStatus(STATUS status);
	void GetServiceInfo(ServiceInfo info);
	void ServiceOverdue();
	void ActiveUpdateSignal();
	void StartTimerSignal();

private slots:
	void UpdateServerInfo();   //����һ�η�������Ϣ

private:
	void run();
	STATUS CheckStatus(const QString& status_str);
	
	volatile bool m_isUpdating;
	QUrl m_url;
	QTimer *m_timer;
	QTimer *m_timerActiceRefresh;
	SmartWifiConnector *m_connector;
};

