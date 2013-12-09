#ifndef _GLOBAL_H_
#define _GLOBAL_H_


#define MAINFRAME_WIDTH 450
#define MAINFRAME_HEIGH 300
#define TITLEBAR_HEIGH 30
#define WIDGET_LEFT_MARGIN 80

typedef enum _STATUS
{
	CONNECTED,
	CONNECTING,
	DISCONNECT
}STATUS;


const QString STATUS_ENABLE = "connected";  //表示VPN已启用
const QString STATUS_DiSABLE = "disable";   //表示VPN已禁用

const QString ROUTER_IP = "10.0.0.1";
const QString APPLICATION_TITLE = "SmartWiFi";
const int PORT = 13000;
const QString SETTING_ORGANIZATION_NAME = "SMART_WIFI";
const QString SETTING_IP = "SMART_WIFI_IP";
const QString SETTING_AUTO = "AUTO_GATEWAY_ADDRESS";
const QString RENEW_URL = "http://smartwifi.bigit.com/smartvpn/topayment?mac=";
const QString RENEW_URL_PACKAGE = "&packagetype=";
//&packagetype = "flow" "hour" "month" "none"



#endif