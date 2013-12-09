
#include "SmartWifiParser.h"
#include <QDomDocument>
#include <QDomElement>

SmartWifiParser::SmartWifiParser(void)
{
}


SmartWifiParser::~SmartWifiParser(void)
{
}

bool SmartWifiParser::parser_get_enable(QString& xml, bool& enable, QString& status_name, QString& error)
{
	QDomDocument doc;
	doc.setContent(xml);
	QDomElement root = doc.documentElement();
	QString staus_flag;

	if (! root.isNull())
	{
		QDomElement ele1 = root.firstChildElement("SOAP-ENV:Body");
		if (! ele1.isNull())
		{
			QDomElement ele2 = ele1.firstChildElement("svpn:get-statusResponse");
			if (! ele2.isNull())
			{
				QDomElement ele3 = ele2.firstChildElement("status");
				if (! ele3.isNull())
				{
					QDomElement ele4 = ele3.firstChildElement("staFlag");
					ele4 = ele3.firstChildElement("errCode");
					if (! ele4.isNull())
					{
						error = ele4.text();
					}
					if (error == "0")
					{
						if (! ele4.isNull())
						{
							staus_flag = ele4.text();
						}
						ele4 = ele3.firstChildElement("staName");
						if (! ele4.isNull())
						{
							status_name = ele4.text();
						}
						error = "";
					}
				}
			}
		}
	}

	if (staus_flag.isEmpty())
		return false;

	enable = (staus_flag == "0"? false:true);
	return true;
}

bool SmartWifiParser::parser_set_enable(QString& xml, QString& error)
{
	QDomDocument doc;
	doc.setContent(xml);
	QDomElement root = doc.documentElement();
	QString str_success;

	if (! root.isNull())
	{
		QDomElement ele1 = root.firstChildElement("SOAP-ENV:Body");
		if (! ele1.isNull())
		{
			QDomElement ele2 = ele1.firstChildElement("svpn:set-enableResponse");
			if (! ele2.isNull())
			{
				QDomElement ele3 = ele2.firstChildElement("result");
				if (! ele3.isNull())
				{
					//find node enable value
					str_success = ele3.text();
				}
			}
		}
	}

	if (str_success.isEmpty())
		return false;

	error = str_success;
	return true;
}

bool SmartWifiParser::parser_get_timeout(QString& xml, QString& sec)
{
	QDomDocument doc;
	doc.setContent(xml);
	QDomElement root = doc.documentElement();
	QString str_sec;

	if (root.isNull())
		return false;

	QDomElement ele1 = root.firstChildElement("SOAP-ENV:Body");
	if (ele1.isNull())
		return false;

	QDomElement ele2 = ele1.firstChildElement("svpn:get-timeoutResponse");
	if (ele2.isNull())
		return false;

	QDomElement ele3 = ele2.firstChildElement("timeout");
	if (ele3.isNull())
		return false;

	sec = ele3.text();
	return true;
}

bool SmartWifiParser::parser_get_vpn_type(QString& xml, QString& type)
{
	QDomDocument doc;
	doc.setContent(xml);
	QDomElement root = doc.documentElement();
	QString str_type;

	if (! root.isNull())
	{
		QDomElement ele1 = root.firstChildElement("SOAP-ENV:Body");
		if (! ele1.isNull())
		{
			QDomElement ele2 = ele1.firstChildElement("svpn:get-vpn-typeResponse");
			if (! ele2.isNull())
			{
				QDomElement ele3 = ele2.firstChildElement("vpnType");
				if (! ele3.isNull())
				{
					//find node enable value
					str_type = ele3.text();
				}
			}
		}
	}

	if (str_type.isEmpty())
		return false;

	type = str_type;
	return true;

}

bool SmartWifiParser::parser_get_vpn_info(QString& xml, QString& type, QString& info, QString& error)
{
	QDomDocument doc;
	doc.setContent(xml);
	QDomElement root = doc.documentElement();
	QString str_info;

	if (root.isNull())
		return false;

	QDomElement ele1 = root.firstChildElement("SOAP-ENV:Body");
	if (ele1.isNull())
		return false;

	QDomElement ele2 = ele1.firstChildElement("svpn:get-vpn-infoResponse");
	if (ele2.isNull())
		return false;

	QDomElement ele3 = ele2.firstChildElement("vpnInfo");
	if (ele3.isNull())
		return false;

	QDomElement ele_result = ele3.firstChildElement("result");
	if (ele_result.isNull())
		return false;
		
	if (ele_result.text() != "0")
	{
		//VPN return error
		error = ele_result.text();
		return false;
	}
		
	QDomElement ele_type = ele3.firstChildElement("type");
	if (ele_type.isNull())
		return false;
	type = ele_type.text();

	QDomElement ele_info = ele3.firstChildElement("info");
	if (ele_info.isNull())
		return false;

	//get info
	if (type == "eHour")
	{
		//包小时
		QDomElement ele_hours = ele_info.firstChildElement("hours");
		if (ele_hours.isNull())
			return false;
		info = ele_hours.text();
	}
	else if(type == "eFlow")
	{
		//包流量
		QDomElement ele_flow = ele_info.firstChildElement("flow");
		if (ele_flow.isNull())
			return false;
		info = ele_flow.text();
	}
	else if(type == "eMonth")
	{
		//包月
		QDomElement ele_month_type = ele_info.firstChildElement("month");
		if (ele_month_type.isNull())
			return false;
		QDomElement ele_year = ele_month_type.firstChildElement("year");
		if (ele_year.isNull())
			return false;
		info = ele_year.text();

		QDomElement ele_month = ele_month_type.firstChildElement("month");
		if (ele_month.isNull())
			return false;
		info += QStringLiteral("年");
		info += ele_month.text();

		QDomElement ele_day = ele_month_type.firstChildElement("day");
		if(ele_day.isNull())
			return false;
		info += QStringLiteral("月");;
		info += ele_day.text();
		info += QStringLiteral("日");
	}
	else
	{
		error = "unknown vpn package type";
		return false;
	}
	return true;
}

bool SmartWifiParser::parser_get_client_count(QString& xml, int& count)
{
	QDomDocument doc;
	doc.setContent(xml);
	QDomElement root = doc.documentElement();
	QString str_count;

	if (! root.isNull())
	{
		QDomElement ele1 = root.firstChildElement("SOAP-ENV:Body");
		if (! ele1.isNull())
		{
			QDomElement ele2 = ele1.firstChildElement("svpn:get-client-countResponse");
			if (! ele2.isNull())
			{
				QDomElement ele3 = ele2.firstChildElement("count");
				if (! ele3.isNull())
				{
					//find node enable value
					str_count = ele3.text();
				}
			}
		}
	}

	if (str_count.isEmpty())
		return false;

	count = str_count.toInt();
	return true;
}

bool SmartWifiParser::parser_get_user_name(QString& xml, QString& name, QString& error)
{
	QDomDocument doc;
	doc.setContent(xml);
	QDomElement root = doc.documentElement();
	QString user_name;

	if (root.isNull())
		return false;

	QDomElement ele1 = root.firstChildElement("SOAP-ENV:Body");
	if (ele1.isNull())
		return false;

	QDomElement ele2 = ele1.firstChildElement("svpn:get-usernameResponse");
	if (ele2.isNull())
		return false;

	QDomElement ele3 = ele2.firstChildElement("username");
	if (ele3.isNull())
		return false;

	//Get white url
	QDomElement ele_result = ele3.firstChildElement("result");
	if (ele_result.isNull())
		return false;
	if ("0" != ele_result.text())
	{
		error = ele_result.text();
		return false;
	}

	QDomElement ele_name = ele3.firstChildElement("string");
	if (ele_name.isNull())
		return false;

	name = ele_name.text();
	return true;
}

bool SmartWifiParser::parser_get_vpn_password(QString& xml, QString& password)
{
	QDomDocument doc;
	doc.setContent(xml);
	QDomElement root = doc.documentElement();
	QString str_password;

	if (! root.isNull())
	{
		QDomElement ele1 = root.firstChildElement("SOAP-ENV:Body");
		if (! ele1.isNull())
		{
			QDomElement ele2 = ele1.firstChildElement("svpn:get-vpn-passwordResponse");
			if (! ele2.isNull())
			{
				QDomElement ele3 = ele2.firstChildElement("result");
				if (! ele3.isNull())
				{
					QDomElement ele4 = ele2.firstChildElement("ret");
					if (! ele4.isNull())
					{
						//find node enable value
						str_password = ele4.text();
					}
				}
			}
		}
	}

	if (str_password.isEmpty())
		return false;

	password = str_password;
	return true;
}

bool SmartWifiParser::parser_get_used(QString& xml, QString& type, QString& used, QString& error)
{
	QDomDocument doc;
	doc.setContent(xml);
	QDomElement root = doc.documentElement();
	QString str_info;

	if (root.isNull())
		return false;

	QDomElement ele1 = root.firstChildElement("SOAP-ENV:Body");
	if (ele1.isNull())
		return false;

	QDomElement ele2 = ele1.firstChildElement("svpn:get-usedResponse");
	if (ele2.isNull())
		return false;

	QDomElement ele3 = ele2.firstChildElement("vpnInfo");
	if (ele3.isNull())
		return false;

	QDomElement ele_result = ele3.firstChildElement("result");
	if (ele_result.isNull())
		return false;

	if (ele_result.text() != "0")
	{
		//VPN return error
		error = ele_result.text();
		return false;
	}

	QDomElement ele_type = ele3.firstChildElement("type");
	if (ele_type.isNull())
		return false;
	type = ele_type.text();

	QDomElement ele_info = ele3.firstChildElement("info");
	if (ele_info.isNull())
		return false;

	//get info
	if (type == "eHour")
	{
		//包小时
		QDomElement ele_hours = ele_info.firstChildElement("hours");
		if (ele_hours.isNull())
			return false;
		used = ele_hours.text();
	}
	else if(type == "eFlow")
	{
		//包流量
		QDomElement ele_flow = ele_info.firstChildElement("flow");
		if (ele_flow.isNull())
			return false;
		used = ele_flow.text();
	}
	else if(type == "eMonth")
	{
		//包月
		QDomElement ele_month_type = ele_info.firstChildElement("month");
		if (ele_month_type.isNull())
			return false;
		QDomElement ele_year = ele_month_type.firstChildElement("year");
		if (ele_year.isNull())
			return false;
		used = ele_year.text();

		QDomElement ele_month = ele_month_type.firstChildElement("month");
		if (ele_month.isNull())
			return false;
		used += QStringLiteral("年");
		used += ele_month.text();

		QDomElement ele_day = ele_month_type.firstChildElement("day");
		if(ele_day.isNull())
			return false;
		used += QStringLiteral("月");
		used += ele_day.text();
		used += QStringLiteral("日");
	}
	else
	{
		error = "unknown type";
	}
	return true;
}

bool SmartWifiParser::parser_get_white_url(QString& xml, QString& white_url)
{
	QDomDocument doc;
	doc.setContent(xml);
	QDomElement root = doc.documentElement();
	QString str_info;

	if (root.isNull())
		return false;

	QDomElement ele1 = root.firstChildElement("SOAP-ENV:Body");
	if (ele1.isNull())
		return false;

	QDomElement ele2 = ele1.firstChildElement("svpn:get-whitelistResponse");
	if (ele2.isNull())
		return false;

	QDomElement ele3 = ele2.firstChildElement("retString");
	if (ele3.isNull())
		return false;

	//Get white url
	QDomElement ele_result = ele3.firstChildElement("result");
	if (ele_result.isNull())
		return false;
	if ("0" != ele_result.text())
		return false;
	
	QDomElement ele_url = ele3.firstChildElement("string");
	if (ele_url.isNull())
		return false;
	
	white_url = ele_url.text();
	if (white_url.isEmpty())
		return false;
	return true;
}


bool SmartWifiParser::parser_set_timeout(QString& xml, QString& error)
{
	QDomDocument doc;
	doc.setContent(xml);
	QDomElement root = doc.documentElement();
	QString str_info;

	if (root.isNull())
		return false;

	QDomElement ele1 = root.firstChildElement("SOAP-ENV:Body");
	if (ele1.isNull())
		return false;

	QDomElement ele2 = ele1.firstChildElement("svpn:set-timeoutResponse");
	if (ele2.isNull())
		return false;

	QDomElement ele3 = ele2.firstChildElement("result");
	if (ele3.isNull())
		return false;

	error = ele3.text();
	return true;
}

bool SmartWifiParser::parser_add_white_url(QString& xml, QString& error)
{
	QDomDocument doc;
	doc.setContent(xml);
	QDomElement root = doc.documentElement();
	QString result;

	if (root.isNull())
		return false;

	QDomElement ele1 = root.firstChildElement("SOAP-ENV:Body");
	if (ele1.isNull())
		return false;

	QDomElement ele2 = ele1.firstChildElement("svpn:set-whitelistResponse");
	if (ele2.isNull())
		return false;

	QDomElement ele3 = ele2.firstChildElement("result");
	if (ele3.isNull())
		return false;

	result = ele3.text();
	if (result == "0")
		return true;

	error = result;
	return false;
}