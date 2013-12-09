#ifndef  _SMARTVPN_PROTOCOL_H_
#define _SMARTVPN_PROTOCOL_H_
#include <QObject>
#include <QString>

const int SMARTWIFI_PORT = 13000;
const QString SMART_VPN_CONNECTED = "connected";
const QString SMART_VPN_DISABLE = "disable";
const QString SERVICE_TYPE_FLOWS = "eFlow";
const QString SERVICE_TYPE_HOURS = "eHour";
const QString SERVICE_TYPE_MONTHS = "eMonth";

//套餐信息
class ServiceInfo
{
public:
	QString serviceType;  //套餐类型
	QString limitString;  //限制（流量/小时/日期）
	QString used;		  //已使用
};

typedef enum _WHITE_URL_TYPE
{
	WHITE_URL_PRIVATE,
	WHITE_URL_PUBLIC
}WHITE_URL_TYPE;

typedef enum _ADD_TYPE
{
	ADD_UNFORCE,
	ADD_FORCE
}ADD_TYPE;

const QString pro_get_enable = "<?xml version=\"1.0\" encoding=\"UTF-8\"?> \
							   <SOAP-ENV:Envelope \
							   xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" \
							   xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" \
							   xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" \
							   xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" \
							   xmlns:svpn=\"urn:svpn\"> \
							   <SOAP-ENV:Body> \
							   <svpn:get-status> \
							   </svpn:get-status> \
							   </SOAP-ENV:Body> \
							   </SOAP-ENV:Envelope>";


const QString pro_get_user_name = "<?xml version=\"1.0\" encoding=\"UTF-8\"?> \
								  <SOAP-ENV:Envelope \
								  xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" \
								  xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" \
								  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" \
								  xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" \
								  xmlns:svpn=\"urn:svpn\"> \
								  <SOAP-ENV:Body> \
								  <svpn:get-username> \
								  </svpn:get-username> \
								  </SOAP-ENV:Body> \
								  </SOAP-ENV:Envelope>";

const QString pro_get_timeout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?> \
								<SOAP-ENV:Envelope \
								xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" \
								xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" \
								xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" \
								xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" \
								xmlns:svpn=\"urn:svpn\"> \
								<SOAP-ENV:Body> \
								<svpn:get-timeout>\
								</svpn:get-timeout>\
								</SOAP-ENV:Body> \
								</SOAP-ENV:Envelope>";

const QString pro_set_timeout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?> \
								<SOAP-ENV:Envelope \
								xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" \
								xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" \
								xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" \
								xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" \
								xmlns:svpn=\"urn:svpn\"> \
								<SOAP-ENV:Body> \
								<svpn:set-timeout>\
								<timeout>%1</timeout>\
								</svpn:set-timeout>\
								</SOAP-ENV:Body> \
								</SOAP-ENV:Envelope>";

const QString pro_get_vpn_type = "<?xml version=\"1.0\" encoding=\"UTF-8\"?> \
								 <SOAP-ENV:Envelope \
								 xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" \
								 xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" \
								 xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" \
								 xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" \
								 xmlns:svpn=\"urn:svpn\"> \
								 <SOAP-ENV:Body> \
								 <svpn:get-vpn-type>\
								 <username></username>\
								 </svpn:get-vpn-type>\
								 </SOAP-ENV:Body>\
								 </SOAP-ENV:Envelope>";



const QString pro_get_vpn_info = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
								 <SOAP-ENV:Envelope\
								 xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"\
								 xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\"\
								 xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\
								 xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"\
								 xmlns:svpn=\"urn:svpn\">\
								 <SOAP-ENV:Body>\
								 <svpn:get-vpn-info>\
								 <username></username>\
								 </svpn:get-vpn-info>\
								 </SOAP-ENV:Body>\
								 </SOAP-ENV:Envelope>";


const QString pro_get_vpn_log = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
								<SOAP-ENV:Envelope\
								xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"\
								xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\"\
								xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\
								xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"\
								xmlns:svpn=\"urn:svpn\">\
								<SOAP-ENV:Body>\
								<svpn:get-vpn-log>\
								<line>0</line>\
								</svpn:get-vpn-log>\
								</SOAP-ENV:Body>\
								</SOAP-ENV:Envelope>";


const QString pro_get_vpn_password = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
									 <SOAP-ENV:Envelope\
									 xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"\
									 xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\"\
									 xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\
									 xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"\
									 xmlns:svpn=\"urn:svpn\">\
									 <SOAP-ENV:Body>\
									 <svpn:get-vpn-password>\
									 <username></username>\
									 </svpn:get-vpn-password>\
									 </SOAP-ENV:Body>\
									 </SOAP-ENV:Envelope>";


const QString pro_set_client_count = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
									 <SOAP-ENV:Envelope\
									 xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"\
									 xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\"\
									 xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\
									 xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"\
									 xmlns:svpn=\"urn:svpn\">\
									 <SOAP-ENV:Body>\
									 <svpn:set-client-count>\
									 <oper>0</oper>\
									 </svpn:set-client-count>\
									 </SOAP-ENV:Body>\
									 </SOAP-ENV:Envelope>";


const QString pro_set_debug = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
							  <SOAP-ENV:Envelope\
							  xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"\
							  xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\"\
							  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\
							  xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"\
							  xmlns:svpn=\"urn:svpn\">\
							  <SOAP-ENV:Body>\
							  <svpn:set-debug>\
							  <debug>0</debug>\
							  </svpn:set-debug>\
							  </SOAP-ENV:Body>\
							  </SOAP-ENV:Envelope>";


const QString pro_set_default_gw = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
								   <SOAP-ENV:Envelope\
								   xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"\
								   xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\"\
								   xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\
								   xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"\
								   xmlns:svpn=\"urn:svpn\">\
								   <SOAP-ENV:Body>\
								   <svpn:set-default-gw>\
								   <gateway></gateway>\
								   </svpn:set-default-gw>\
								   </SOAP-ENV:Body>\
								   </SOAP-ENV:Envelope>";


const QString pro_set_enbale = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
							   <SOAP-ENV:Envelope\
							   xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"\
							   xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\"\
							   xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\
							   xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"\
							   xmlns:svpn=\"urn:svpn\">\
							   <SOAP-ENV:Body>\
							   <svpn:set-enable>\
							   <enable>%1</enable>\
							   <type>0</type>\
							   </svpn:set-enable>\
							   </SOAP-ENV:Body>\
							   </SOAP-ENV:Envelope>";
							   //type 0使用白名单 1自动切换模式

const QString pro_set_unable = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
							   <SOAP-ENV:Envelope\
							   xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"\
							   xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\"\
							   xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\
							   xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"\
							   xmlns:svpn=\"urn:svpn\">\
							   <SOAP-ENV:Body>\
							   <svpn:set-enable>\
							   <enable>0</enable>\
							   </svpn:set-enable>\
							   </SOAP-ENV:Body>\
							   </SOAP-ENV:Envelope>";


const QString pro_set_white_url = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
								  <SOAP-ENV:Envelope\
								  xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"\
								  xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\"\
								  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\
								  xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"\
								  xmlns:svpn=\"urn:svpn\">\
								  <SOAP-ENV:Body>\
								  <svpn:set-whitelist>\
								  <act>0</act>\
								  <force>%1</force>\
								  <url>%2</url>\
								  </svpn:set-whitelist>\
								  </SOAP-ENV:Body>\
								  </SOAP-ENV:Envelope>";
								  //force 0非强制 1强制加入白名单

const QString pro_del_white_url = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
								  <SOAP-ENV:Envelope\
								  xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"\
								  xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\"\
								  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\
								  xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"\
								  xmlns:svpn=\"urn:svpn\">\
								  <SOAP-ENV:Body>\
								  <svpn:set-whitelist>\
								  <act>1</act>\
								  <force>0</force>\
								  <url>%1</url>\
								  </svpn:set-whitelist>\
								  </SOAP-ENV:Body>\
								  </SOAP-ENV:Envelope>";


const QString pro_get_used = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
							 <SOAP-ENV:Envelope\
							 xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"\
							 xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\"\
							 xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\
							 xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"\
							 xmlns:svpn=\"urn:svpn\">\
							 <SOAP-ENV:Body>\
							 <svpn:get-used>\
							 </svpn:get-used>\
							 </SOAP-ENV:Body>\
							 </SOAP-ENV:Envelope>";


const QString pro_get_white_list = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
								   <SOAP-ENV:Envelope\
								   xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"\
								   xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\"\
								   xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\
								   xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"\
								   xmlns:svpn=\"urn:svpn\">\
								   <SOAP-ENV:Body>\
								   <svpn:get-whitelist>\
								   <idx>%1</idx>\
								   </svpn:get-whitelist>\
								   </SOAP-ENV:Body>\
								   </SOAP-ENV:Envelope>";

const QString pro_get_white_list_page = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
										<SOAP-ENV:Envelope\
										xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"\
										xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\"\
										xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\
										xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"\
										xmlns:svpn=\"urn:svpn\">\
										<SOAP-ENV:Body>\
										<svpn:get-whitelist-pg>\
										<type>%1</type>\
										<start>%2</start>\
										<pgmax>1000</pgmax>\
										</svpn:get-whitelist-pg>\
										</SOAP-ENV:Body>\
										</SOAP-ENV:Envelope>";




#endif