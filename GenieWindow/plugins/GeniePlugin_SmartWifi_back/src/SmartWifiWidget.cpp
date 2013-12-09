#include "SmartWifiWidget.h"
#include "ui_smartwifiwidget.h"
#include "SmartWifiConnector.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QUrl>

const QString SMART_WIFI_SETTINGS = "Smart_WIfi_Settings";
const QString SMART_WIFI_SETTINGS_IP = "Smart_WIfi_Settings_IP";
const QString SMART_WIFI_TYPE_FLOWS  = "eFlow";
const QString SMART_WIFI_TYPE_HOURS  = "eHour";
const QString SMART_WIFI_TYPE_MONTHS = "eMonth";

SmartWifiWidget::SmartWifiWidget():
	ui(new Ui::SmartWifiWidget)
{
	ui->setupUi(this);
	
	//setting
	m_setting = new QSettings(SMART_WIFI_SETTINGS);
	m_ip = m_setting->value(SMART_WIFI_SETTINGS_IP, QVariant(QString("10.0.0.1"))).toString();

	m_launchThread = new LaunchThread(GetRouterUrl(), this);
	connect(m_launchThread, SIGNAL(ServerStatus(QString, QString)), this, SLOT(ServerStatus(QString, QString)));
	connect(m_launchThread, SIGNAL(GetServiceInfo(ServiceInfo)), this, SLOT(GetServiceInfo(ServiceInfo)));
	m_launchThread->start();

	connect(ui->enableBtn, SIGNAL(clicked()), this, SLOT(ClickEnableSmartWifi()));
}

SmartWifiWidget::~SmartWifiWidget()
{

}

QUrl SmartWifiWidget::GetRouterUrl()
{
	QString url = "http://";
	url += m_ip;
	url += ":";
	url += QString::number(SMARTWIFI_PORT);
	return QUrl(url);
}

void SmartWifiWidget::ServerStatus(QString server_status, QString server_error)
{
	ui->routerStatusLabel->setText(server_status);
}

void SmartWifiWidget::GetServiceInfo(ServiceInfo info)
{
	if (info.serviceType == SMART_WIFI_TYPE_FLOWS)	
		LoadFlowServiceInfoUI(info.limitString, info.used); //按流量计费
	else if (info.serviceType == SMART_WIFI_TYPE_HOURS)	
		LoadHoursServiceInfoUI(info.limitString, info.used);//按小时计费
	else if (info.serviceType == SMART_WIFI_TYPE_MONTHS)    
		LoadMonthServiceInfoUI(info.limitString);           //按月计费
	else
		LoadUnkonowServiceTypeUI();
}

void SmartWifiWidget::ClickEnableSmartWifi()
{
	QThread EnableThead;
	SmartWifiConnector connector;
	connector.moveToThread(&EnableThead);
//	connect(&EnableThead, SIGNAL(started()), &connector, SLOT()));
//	connector.
}

void SmartWifiWidget::LoadFlowServiceInfoUI(QString& rest, QString used)
{
	ui->serviceInfoStack->setCurrentIndex(0);
	ui->servieceFlowLabel->setText(QStringLiteral("按流量计费"));
	ui->restFlowLabel->setText(rest + "Hours");
	ui->usedFlowLabel->setText(used + "Hours");

	SetProgressValue(ui->hourPBar, rest, used);
}

void SmartWifiWidget::LoadHoursServiceInfoUI(QString& rest, QString used)
{
	ui->serviceInfoStack->setCurrentIndex(1);
	ui->servieceFlowLabel->setText(QStringLiteral("按流量计费"));
	ui->restFlowLabel->setText(rest + "MB");
	ui->usedFlowLabel->setText(used + "MB");

	SetProgressValue(ui->flowPBar, rest, used);
}

void SmartWifiWidget::LoadMonthServiceInfoUI(QString& overdueDate)
{
	ui->serviceInfoStack->setCurrentIndex(2);
	ui->servieceMonthLabel->setText(QStringLiteral("按月计费"));
	ui->overdueLabel->setText(overdueDate);
}

void SmartWifiWidget::LoadUnkonowServiceTypeUI()
{
	ui->serviceInfoStack->setCurrentIndex(3);
}

void SmartWifiWidget::SetProgressValue(QProgressBar* proBar, QString& limit, QString& used)
{
	if (! proBar)
		return;

	double n = used.toDouble();
	double m = limit.toDouble();
	double s = 0;

	if (m < 0.01)
	{
		m = 0;
	}
	else
	{
		double percent = n/m;
		s = percent*100;
	}

	QString format = QString::number(s, 'f', 2)+ "%"; //保留两位小数
	proBar->setFormat(format);
	proBar->setValue(s);
}