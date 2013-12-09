#pragma once
#include <QString>

class SmartWifiParser
{
public:
	SmartWifiParser(void);
	~SmartWifiParser(void);

public:
	bool parser_get_enable(QString& xml, bool& enable, QString& status_name, QString& error);
	bool parser_set_enable(QString& xml, QString& error);
	bool parser_get_timeout(QString& xml, QString& sec);
	bool parser_set_timeout(QString& xml, QString& error);
	bool parser_get_vpn_type(QString& xml, QString& type);
	bool parser_get_client_count(QString& xml, int& count);
	bool parser_get_user_name(QString& xml, QString& name, QString& error);
	bool parser_get_vpn_info(QString& xml, QString& type, QString& info, QString& error);
	bool parser_get_vpn_password(QString& xml, QString& password);
	bool parser_get_used(QString& xml, QString& type, QString& used, QString& error);
	bool parser_get_white_url(QString& xml, QString& white_url);
	bool parser_add_white_url(QString& xml, QString& error);
};

