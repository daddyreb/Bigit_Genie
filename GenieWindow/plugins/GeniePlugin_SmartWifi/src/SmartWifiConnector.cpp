#include <QEventLoop>
#include <QNetworkReply>
#include <QMutex>

#include <QStringList>
#include <QTimer>
#include <QThread>
#include "SmartWifiConnector.h"
#include "SmartWifiParser.h"

SmartWifiConnector::SmartWifiConnector(void):m_req_index(0),m_replyTimeout(5000)
{
}


SmartWifiConnector::~SmartWifiConnector(void)
{
}


void SmartWifiConnector::SetReplyTimeout(int sec)
{
	m_replyTimeout = sec*1000;
}

void SmartWifiConnector::InitRequest(QUrl url, QNetworkRequest &req, int contentLength)
{
//	url.setHost("www.routerlogin.net");
	req.setUrl(url);
	req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	req.setHeader(QNetworkRequest::ContentLengthHeader, contentLength);
}

QString SmartWifiConnector::SendReq(QUrl url, QString& xml)
{
	QNetworkAccessManager manager;
	QNetworkRequest req;
	QEventLoop loop;
	QTimer timer;
	timer.setSingleShot(true);
	connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
	connect(&manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
	InitRequest(url, req, xml.length());

	timer.start(m_replyTimeout);
	QNetworkReply *pReply = manager.post(req, xml.toUtf8());	
	loop.exec();

	QString res;
	if (timer.isActive())
	{
		timer.stop();
		int error = pReply->error();
		if (!pReply || (error != QNetworkReply::NoError))
			return "";

		res =  pReply->readAll();
	}
	else
	{
		disconnect(&manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
		pReply->abort();
		res = "";
	}	

	return res;
}

bool SmartWifiConnector::GetEnable(QUrl url, bool &enable, QString& status_name, QString& waittime, QString& error)
{
	QString protocol = pro_get_enable;
	QString xml = SendReq(url, protocol);
	if (xml.isEmpty())
	{
		error = "No Response"; 
		return false;
	}

	if (! m_parser.parser_get_enable(xml, enable, status_name, waittime, error))
	{
		return false;
	}

	return true;
}

bool SmartWifiConnector::SetEnable(QUrl url, bool enable, QString& error)
{	
	QString s = pro_set_enbale;
	int value = enable? 1:0;
	QString xml = s.arg(value);
	QString res_xml = SendReq(url, xml);
	if (res_xml.isEmpty())
	{
		error = "No Response";
		return false;
	}

	QString vpn_error = false;
	if (! m_parser.parser_set_enable(res_xml, vpn_error))
		return false;
	if (vpn_error == "0")
		return true;
	
	error = vpn_error;
	return false;
}

bool SmartWifiConnector::GetTimeout(QUrl url, QString& sec, QString& error)
{
	QString req = pro_get_timeout;
	QString xml = SendReq(url, req);
	if (xml.isEmpty())
	{
		error = "No Response";
		return false;
	}

	if (! m_parser.parser_get_timeout(xml, sec))
		return false;
	return true;
}

bool SmartWifiConnector::SetTimeout(QUrl url, int sec, QString& error)
{
	QString s = pro_set_timeout;
	QString req = pro_set_timeout.arg(sec);
	QString xml = SendReq(url, req);
	if (xml.isEmpty())
	{
		error = "No Response";
		return false;

	}
	if (! m_parser.parser_set_timeout(xml, error))
		return false;
	return true;
}

 bool SmartWifiConnector::GetVPNInfo(QUrl url, QString& type, QString& info, QString& error)
 {
	 QString req = pro_get_vpn_info;
	 QString res_xml = SendReq(url, req);
	 if (res_xml.isEmpty())
	 {
		 error = "No Response";
		 return false;
	 }

	 QString vpn_type, vpn_info, vpn_error;
	 if (! m_parser.parser_get_vpn_info(res_xml, vpn_type, vpn_info, error))
		 return false;

	 type = vpn_type;
	 info = vpn_info;
	 error = vpn_error;
	 return true;
 }

 bool SmartWifiConnector::GetWhiteUrl(QUrl url, WHITE_URL_TYPE type, QStringList& while_url, QString& error)
 {
	 QString s = pro_get_white_list_page;
	 for (int i=0; i<100000; ++i)
	 {
		 QString req = s.arg(type == WHITE_URL_PRIVATE? 0:1).arg(while_url.count());
		 QString res_xml = SendReq(url, req);
		 if (res_xml.isEmpty())
		 {
			 error = "GetWhiteUrl: No Response";
			 return false;
		 }

		 QString error;
		 QStringList white_url_temp;
		 if(! m_parser.parser_get_white_url_page(res_xml, white_url_temp, error))
		 {
			 while_url.append(white_url_temp);
			 break;
		 }		 
		 while_url.append(white_url_temp);
	 }
	 return true;
 }

bool SmartWifiConnector::DelWhiteUrl(QUrl url, QString& white_url, QString& error)
{
	QString s = pro_del_white_url;
	QString req = s.arg(white_url);

	QString res_xml = SendReq(url, req);
	if (res_xml.isEmpty())
	{
		error = "No Response";
		return false;
	}

	if (! m_parser.parser_del_white_url(res_xml, error))
		return false;
	return true;
}

bool SmartWifiConnector::AddWhiteUrl(QUrl url, ADD_TYPE type, QString& white_url, QString& error)
 {
	 QString s = pro_set_white_url;
	 QString req = s.arg(type).arg(white_url);

	 QString res_xml = SendReq(url, req);
	 if (res_xml.isEmpty())
	 {
		 error = "No Response";
		 return false;
	 }

	 if (! m_parser.parser_add_white_url(res_xml, error))
		 return false;
	 return true;
 }

bool SmartWifiConnector::GetUsed(QUrl url, QString& type, QString& used, QString& error)
{
	QString req = pro_get_used;
	QString res_xml = SendReq(url, req);

	if (res_xml.isEmpty())
	{
		error = "No Response";
		return false;
	}
		
	QString vpn_type, vpn_used, vpn_error;
	if (! m_parser.parser_get_used(res_xml, vpn_type, vpn_used, error))
		return false;
	used = vpn_used;
	type = vpn_type;
	error = vpn_error;
	return true;
}

bool SmartWifiConnector::GetVPNUserName(QUrl url, QString& user_name, QString& error)
{
	QString req = pro_get_user_name;
	QString res_xml = SendReq(url, req);

	if (res_xml.isEmpty())
	{
		error = "No Response";
		return false;
	}

	QString vpn_user_name;
	if (! m_parser.parser_get_user_name(res_xml, vpn_user_name, error))
		return false;
	user_name = vpn_user_name;
	return true;
}

bool SmartWifiConnector::Test(QUrl url)
{
	QString protocol = pro_get_enable;

	QNetworkAccessManager manager;
	QNetworkRequest req;
	QEventLoop loop;
	QTimer timer;
	timer.setSingleShot(true);
	connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
	connect(&manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
	
	req.setUrl(url);
	req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	req.setHeader(QNetworkRequest::ContentLengthHeader, protocol.length());

	timer.start(m_replyTimeout);
	QNetworkReply *pReply = manager.post(req, protocol.toUtf8());	
	loop.exec();

	QString res;
	if (timer.isActive())
	{
		timer.stop();
		int error = pReply->error();
		if (!pReply || (error != QNetworkReply::NoError))
			return "";

		res =  pReply->readAll();
	}
	else
	{
		disconnect(&manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
		pReply->abort();
		res = "";
	}	

	if (! res.isEmpty())
	{
		return true;
	}
	return false;

}
