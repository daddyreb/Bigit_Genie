#ifndef _SMARTWIFI_WIDGET_H_
#define _SMARTWIFI_WIDGET_H_
#include <QWidget>
#include <QLabel>
#include <QSettings>
#include <QProgressBar>
#include "SmartWifiProtocol.h"
#include "LaunchThread.h"


namespace Ui {
	class SmartWifiWidget;
}

class SmartWifiWidget : public QWidget, public StatusNoticed
{
	Q_OBJECT
public:
	SmartWifiWidget();
	~SmartWifiWidget();
	QUrl GetRouterUrl();

private slots:
	void ServerStatus(QString, QString);
	void GetServiceInfo(ServiceInfo );
	void ClickEnableSmartWifi();

private:
	void LoadFlowServiceInfoUI(QString& rest, QString usage);
	void LoadHoursServiceInfoUI(QString& rest, QString usage);
	void LoadMonthServiceInfoUI(QString& overdueDate);
	void LoadUnkonowServiceTypeUI();
	void SetProgressValue(QProgressBar* proBar, QString& limit, QString& used);

	bool m_status;
	QSettings *m_setting;
	Ui::SmartWifiWidget *ui;
	LaunchThread *m_launchThread;
	QString m_ip;
};

#endif
