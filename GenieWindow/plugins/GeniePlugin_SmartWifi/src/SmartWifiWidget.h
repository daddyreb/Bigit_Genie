#ifndef _SMARTWIFI_WIDGET_H_
#define _SMARTWIFI_WIDGET_H_
#include <QWidget>
#include <QLabel>
#include <QSettings>
#include <QProgressBar>
#include "SmartWifiProtocol.h"
#include "LaunchThread.h"
#include "global.h"

namespace Ui {
	class SmartWifiWidget;
}

class LaunchThread;
class GetWhiteListThread;
class SmartWifiConnector;
class AddWhiteUrlThread;
class WaittingDlg;

class SmartWifiWidget : public QWidget
{
	Q_OBJECT
public:
	SmartWifiWidget();
	~SmartWifiWidget();
	QUrl GetRouterUrl();

private slots:
	void ServerStatus(STATUS status);
	void GetServiceInfo(ServiceInfo info);
	void FinishGetWhiteList(bool success, QStringList list_private, QStringList list_public, QString error);
	void ClickConnect();
	void ClickWhiteList();
	void ClickAccountInfo();
	void ClickPurChases();
	void ClickAddUrl(ADD_TYPE type = ADD_UNFORCE);
	void ClickEnforceAddUrl();
	void ShowPrivateItems();
	void ShowPublicItems();
	void FilterWList(const QString& filter_str);
	void FinishAddWhiteUrl(bool success, QString url, QString error);
	void SelectWhiteurlType(int index);
private:
	void ShowNotConnectDevice();
	void ShowConnectedUI();
	void ShowConnectingUI();
	void ShowDisconnectedUI();
	QString GetServiceType();
	void ChangeQCompleter(QStringList& list);

private:
	QSettings *m_setting;
	Ui::SmartWifiWidget *ui;
	LaunchThread *m_launchThread;
	GetWhiteListThread *m_getWhiteListThread;
	AddWhiteUrlThread *m_add_wl_thead;
	WaittingDlg *dlg;

	QString m_ip;
	STATUS m_status;
	QString m_service_type;

	QStringList m_cur_wl;
	QStringList m_private_wl; //private white list
	QStringList m_public_wl;  //public white list
};

#endif
