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
	//ѭ���̣߳���ʱ��ȡvpn�����Ϣ
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
	void UpdateServerInfo();   //����һ�η�������Ϣ

private:
	void run();

	volatile bool m_isUpdating;
	StatusNoticed *m_noticed;
	QUrl m_url;
	QTimer *m_timer;
	QTimer *m_timerActiceRefresh;
};

