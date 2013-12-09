
#include "LaunchThread.h"
#include "SmartWifiConnector.h"
#include <QUrl>

const int SMART_VPN_REFRESH_INTERNAL = 8000;

LaunchThread::LaunchThread(QUrl& url):m_isUpdating(false),m_timerActiceRefresh(NULL),m_connector(NULL)
{
	this->moveToThread(this);
	m_url = url;
	m_timerActiceRefresh = new QTimer();
	connect(m_timerActiceRefresh, SIGNAL(timeout()), this, SIGNAL(ActiveUpdateSignal()));
	m_connector = new SmartWifiConnector();
}

LaunchThread::~LaunchThread(void)
{
	if (m_timerActiceRefresh)
	{
		delete m_timerActiceRefresh;
		m_timerActiceRefresh = NULL;
	}
}

void LaunchThread::SetUrl(QUrl& url)
{
	m_url = url;
}

void LaunchThread::StopUpdate(int waitTime)
{
	//等待waitTime秒再重新启动刷新
	m_timerActiceRefresh->setSingleShot(true);
	m_timerActiceRefresh->start(waitTime*1000); 
}

void LaunchThread::run()
{	
	UpdateServerInfo();

	QTimer timer;
	connect(this, SIGNAL(ActiveUpdateSignal()), &timer, SLOT(stop()));
	connect(this, SIGNAL(ActiveUpdateSignal()), &timer, SIGNAL(timeout()));
	connect(m_timerActiceRefresh, SIGNAL(timeout()), &timer, SLOT(start()));
	connect(&timer, SIGNAL(timeout()), this, SLOT(UpdateServerInfo()));
	timer.start(SMART_VPN_REFRESH_INTERNAL);
	exec();
	timer.stop();
}

void LaunchThread::UpdateServerInfo()
{
	//更新还未返回
	if (m_isUpdating)
		return;

	m_isUpdating = true;

	QString status("");
	QString waittime;
	QString error;
	bool enable;

	m_connector->GetEnable(m_url, enable, status, waittime, error);
	emit ServerStatus(CheckStatus(status));

	if((status != SMART_VPN_CONNECTED) && (status != SMART_VPN_DISABLE))
	{
		m_isUpdating = false;
		return;
	}
		
	//连接成功，获取套餐信息
	ServiceInfo info;
	QString limit_type(QStringLiteral("无套餐信息"));
	QString limit = "0";
	QString used_type;
	QString used = "1";

	bool getvpninfo = m_connector->GetVPNInfo(m_url, limit_type, limit, error);
	if (error == "5" || error == "4")
	{
		//套餐已过期
		emit ServiceOverdue();
		m_isUpdating = false;
		return;
	}

	getvpninfo &= m_connector->GetUsed(m_url, used_type, used, error);

	info.serviceType = limit_type;
	info.limitString = limit;
	info.used = used;

	if (getvpninfo)
		emit GetServiceInfo(info);

	m_isUpdating = false;
}

void LaunchThread::Exit()
{
	this->exit();
	this->wait();
}

STATUS LaunchThread::CheckStatus(const QString& status_str)
{
	if (status_str == STATUS_ENABLE)
	{
		return CONNECTED;
	}
	else if (status_str == STATUS_DiSABLE)
	{
		return DISCONNECT;
	}
	else
	{
		return CONNECTING;
	}
}