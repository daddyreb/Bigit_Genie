#include "SmartWifiWidget.h"
#include "ui_smartwifiwidget.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QUrl>
#include <QFile>
#include <QDesktopServices>
#include <QCompleter>
#include <QMessageBox>
#include "SmartWifiConnector.h"
#include "GetWhiteListThread.h"
#include "AddWhiteUrlThread.h"
#include "WaittingDlg.h"
#include "genie2_interface.h"

const QString SMART_WIFI_SETTINGS = "Smart_Wifi_Settings";
const QString SMART_WIFI_SETTINGS_IP = "Smart_Wifi_Settings_IP";
const int PRIVATE_SEL = 0;
const int PUBLIC_SEL = 1;

SmartWifiWidget::SmartWifiWidget():
	ui(new Ui::SmartWifiWidget)	
{
	m_ip = "192.168.5.1";
	ui->setupUi(this);

	//qss
	QFile qss(":/qss/smartwifi.qss");
	if (qss.open(QIODevice::ReadOnly))
	{
		QString str = qss.readAll();
		this->setStyleSheet(str);
		qss.close();
	}

	ui->server_name->setStyleSheet("QLabel { background-color: transparent;}");


	//LaunchThread
	m_launchThread = new LaunchThread(GetRouterUrl());
	qRegisterMetaType<STATUS>("STATUS");
	connect(m_launchThread, SIGNAL(ServerStatus(STATUS)), this, SLOT(ServerStatus(STATUS)));
	qRegisterMetaType<ServiceInfo>("ServiceInfo");
	connect(m_launchThread, SIGNAL(GetServiceInfo(ServiceInfo)), this, SLOT(GetServiceInfo(ServiceInfo)));
	
	//GetWhiteListThread
	m_getWhiteListThread = new GetWhiteListThread(GetRouterUrl());
	connect(m_getWhiteListThread, SIGNAL(FinishUpdateWhiteList(bool, QStringList, QStringList, QString)), 
		this, SLOT(FinishGetWhiteList(bool, QStringList, QStringList, QString)));

	//
	m_add_wl_thead = new AddWhiteUrlThread(GetRouterUrl());
	connect(m_add_wl_thead, SIGNAL(FinishAddWhiteUrl(bool, QString, QString)), this, SLOT(FinishAddWhiteUrl(bool, QString, QString)));

	//watting dlg
	dlg =  new WaittingDlg();

	connect(ui->whitelist_btn, SIGNAL(clicked()), this, SLOT(ClickWhiteList()));
	connect(ui->account_btn, SIGNAL(clicked()), this, SLOT(ClickAccountInfo()));
	connect(ui->connect_btn, SIGNAL(clicked()), this, SLOT(ClickConnect()));
	connect(ui->purchases_btn, SIGNAL(clicked()), this,  SLOT(ClickPurChases()));
	m_launchThread->start();

	m_getWhiteListThread->start();
}

SmartWifiWidget::~SmartWifiWidget() 
{

}

void SmartWifiWidget::ClickConnect()
{
	QString error;
	SmartWifiConnector con;
	bool enable = (m_status == CONNECTED)? false:true;
	if (! con.SetEnable(GetRouterUrl(), enable, error))
	{	
		return;
	}
}

void SmartWifiWidget::ClickPurChases()
{
	QString user_name, error;
	SmartWifiConnector connector;
	if (! connector.GetVPNUserName(GetRouterUrl(), user_name, error))
		return;

	QString url = RENEW_URL + user_name + RENEW_URL_PACKAGE + GetServiceType();
	QDesktopServices::openUrl(url);
}

void SmartWifiWidget::ClickWhiteList()
{
	m_getWhiteListThread->start();
}

void SmartWifiWidget::ClickAccountInfo()
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


void SmartWifiWidget::ServerStatus(STATUS status)
{
	m_status = status;
	switch(m_status)
	{
	case CONNECTED:
		ShowConnectedUI();
		break;
	case CONNECTING:
		ShowConnectingUI();
		break;
	case DISCONNECT:
		ShowDisconnectedUI();
		break;
	}
}

void SmartWifiWidget::GetServiceInfo(ServiceInfo info)
{
	//m_service_type = info.serviceType;
	//if (info.serviceType == SERVICE_TYPE_FLOWS)
	//{
	//	ui->actype->setText("FLOW");
	//	ui->residual_flow->setText(info.limitString + "M");
	//	ui->use_of->setText(info.used + "M");
	//	ui->ac_stackWidget->setCurrentIndex(0);
	//}
	//else if (info.serviceType == SERVICE_TYPE_HOURS)
	//{
	//	ui->actype->setText("HOUR");
	//	ui->residual_flow->setText(info.limitString + " Hours");
	//	ui->use_of->setText(info.used + " Hours");
	//	ui->ac_stackWidget->setCurrentIndex(0);
	//}
	//else if (info.serviceType == SERVICE_TYPE_MONTHS)
	//{
	//	ui->actype->setText("MONTH");
	//	ui->expiry_date->setText(info.limitString);
	//	ui->use_progressbar->hide();
	//	ui->ac_stackWidget->setCurrentIndex(1);
	//}
}

void SmartWifiWidget::ShowNotConnectDevice()
{
	ui->connect_note->setText("Can not connect device, please check your network");
}
void SmartWifiWidget::ShowConnectedUI()
{
	ui->connect_flag->setPixmap(QPixmap(":/smartWiFi/img/connected_flag.png"));
	ui->connect_note->setText("SmartWiFi is enabled");
	ui->connect_btn->setText("disconnect");
}

void SmartWifiWidget::ShowConnectingUI()
{
	ui->connect_flag->setPixmap(QPixmap(":/smartWiFi/img/disconnected_flag.png"));
	ui->connect_note->setText("SmartWiFi is disabled");
	ui->connect_btn->setText("connect");
}

void SmartWifiWidget::ShowDisconnectedUI()
{
	ui->connect_flag->setPixmap(QPixmap(":/smartWiFi/img/disconnected_flag.png"));
	ui->connect_note->setText("SmartWiFi is starting, please wait.");
	ui->connect_btn->setText("SmartWiFi is disabled");
}

QString SmartWifiWidget::GetServiceType()
{
	//生成访问付费网站的URL的type 
	//&packagetype = "flow" "hour" "month" "none"
	if (m_service_type == "eFlow")
	{
		return "flow";
	}
	else if (m_service_type == "eHours")
	{
		return "hour";
	}
	else if (m_service_type == "eMonth")
	{
		return "month";
	}
	else
	{
		return "none";
	}
}

void SmartWifiWidget::FinishGetWhiteList(bool success, QStringList list_private, QStringList list_public, QString error)
{
	m_private_wl = list_private;
	m_public_wl = list_public;

	if (! success)
	{
		//show fail
		return;
	}

	m_private_wl = list_private;
	m_public_wl = list_public;
	ShowPrivateItems();
}

void SmartWifiWidget::ShowPrivateItems()
{
//	ui->url_table->Clear();
//	ui->url_table->AddPrivateUrlList(m_private_wl);
	m_cur_wl = m_private_wl;
	ChangeQCompleter(m_cur_wl);	
}

void SmartWifiWidget::ShowPublicItems()
{
//	ui->url_table->Clear();
//	ui->url_table->AddPublicUrlList(m_public_wl);
	m_cur_wl = m_public_wl;
	ChangeQCompleter(m_cur_wl);
}

void SmartWifiWidget::FilterWList(const QString& filter_str)
{
//	ui->url_table->setEnabled(false);
	QStringList list = m_cur_wl.filter(filter_str);

//	ui->url_table->Clear();
//	ui->url_table->setRowCount(0);
//	if (ui->wl_type->currentIndex() == PRIVATE_SEL)
//	{
//		ui->url_table->AddPrivateUrlList(list);
//	}
//	else
//	{
//		ui->url_table->AddPublicUrlList(list);
//	}
	
//	ui->url_table->setEnabled(true);
}

void SmartWifiWidget::ClickAddUrl(ADD_TYPE type)
{
//	if (ui->white_url_edit->text().isEmpty())
//		return;

//	m_add_wl_thead->SetUrl(ui->white_url_edit->text());
	m_add_wl_thead->SetAddType(ADD_UNFORCE);
	m_add_wl_thead->start();

	GENIE2_SET_FRAMELESSSTYLE(dlg);
	dlg->StartWaitting("Please wait for analyzing the url...");
}

void SmartWifiWidget::ClickEnforceAddUrl()
{
	ClickAddUrl(ADD_FORCE);
}

void SmartWifiWidget::ChangeQCompleter(QStringList& list)
{
//	QCompleter *src_completer = ui->white_url_edit->completer();
//	src_completer->deleteLater();

	QCompleter *completer = new QCompleter(list);
//	completer->setCompletionMode(QCompleter::InlineCompletion);
	completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
//	ui->white_url_edit->setCompleter(completer);
}


void SmartWifiWidget::FinishAddWhiteUrl(bool success, QString url, QString error)
{
	if(success)
	{
		dlg->Finish("Add White Url Success!");
	}
	else
	{
		dlg->Finish("Add Fail, error:" + error);
	}	
}

void SmartWifiWidget::SelectWhiteurlType(int index)
{
	if (index == PRIVATE_SEL)
	{
		ShowPrivateItems();
	}
	else if (index == PUBLIC_SEL)
	{
		ShowPublicItems();
	}
}