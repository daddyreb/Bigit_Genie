#include "common.h"
#include "logger.h"
#include "oputil.h"
#include "router.h"
#include <vector>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QtEndian>
#include <QStringList>
#include <QUrl>

AsyncOp *Invoker::beginSoapPost(const QString& url, const QString& ns, const QString& cmd, const QStringList& paramNames, const QStringList& paramValues, QObject *parent)
{
	return impl::SoapPostOp::begin(url, ns, cmd, paramNames, paramValues, parent);
}

AsyncOp *Invoker::beginCheckRouterStatus(const QString& username, const QString& password, QNetworkAccessManager *nam, QObject *parent)
{
	return new impl::CheckRouterStatusOp(username, password, nam, parent);
}

AsyncOp *Invoker::beginRouterLogin(const QString& username, const QString& password, QNetworkAccessManager *nam, QObject *parent)
{
	return new impl::RouterLoginOp(username, password, nam, parent);
//	return new impl::RouterLoginOp2(username, password, parent);
}

AsyncOp *Invoker::beginRouterConfigurationStart(const QString& action, QNetworkAccessManager *nam, QObject *parent)
{
	return new impl::RouterConfigurationStart(action, nam, parent);
}

AsyncOp *Invoker::beginRouterGetDnsMasqDeviceId(const QString& action, const QString& macAddress, QNetworkAccessManager *nam, QObject *parent)
{
	return new impl::RouterGetDnsMasqDeviceId(action, macAddress, nam, parent);
}

AsyncOp *Invoker::beginRouterConfigurationFinish(const QString& action, QNetworkAccessManager *nam, QObject *parent)
{
	return new impl::RouterConfigurationFinish(action, nam, parent);
}

bool getGatewayMacAddress(quint32 ipv4Host, QByteArray& mac);

namespace impl {

static const QString MAGIC_VALUE = QString::fromUtf8("99999999999999999999");

static const QString CHECK_DNS_1 = QString::fromUtf8("using.netgear.opendns.com");
static const QString CHECK_DNS_2 = QString::fromUtf8("routerlogin.net");
static const QString CURRENT_SETTINGS = QString::fromUtf8("http://routerlogin.net/currentsetting.htm");
static const QByteArray SOAP_TEMPLATE_P1 = "\xef\xbb\xbf<?xml version=\"1.0\" encoding=\"utf-8\" ?> \r\n<env:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\r\n        xmlns:env=\"http://schemas.xmlsoap.org/soap/envelope/\">\r\n  <env:Body>\r\n    ";
static const QByteArray SOAP_TEMPLATE_P2 = "\r\n  </env:Body> \r\n</env:Envelope>";
static const QString NS_DEVICE_CONFIG = QString::fromUtf8("urn:NETGEAR-ROUTER:service:DeviceConfig:1");
static const QString NS_PARENTAL_CONTROL = QString::fromUtf8("urn:NETGEAR-ROUTER:service:ParentalControl:1");
static const QString NS_DEVICE_INFO = QString::fromUtf8("urn:NETGEAR-ROUTER:service:DeviceInfo:1");
static const QString NS_WANIP_CONNECTION = QString::fromUtf8("urn:NETGEAR-ROUTER:service:WANIPConnection:1");

static const QString CMD_CONFIGURATION_STARTED = QString::fromUtf8("ConfigurationStarted");
static const QString CMD_CONFIGURATION_FINISHED = QString::fromUtf8("ConfigurationFinished");
static const QString CMD_GET_DNS_MASQ_DEVICE_ID = QString::fromUtf8("GetDNSMasqDeviceID");
static const QString CMD_SET_DNS_MASQ_DEVICE_ID = QString::fromUtf8("SetDNSMasqDeviceID");
static const QString CMD_DELETE_MAC_ADDRESS = QString::fromUtf8("DeleteMACAddress");
static const QString CMD_GET_ENABLED_STATUS = QString::fromUtf8("GetEnableStatus");
static const QString CMD_GET_DEVICE_INFO = QString::fromUtf8("GetInfo_DeviceInfo");
static const QString CMD_GET_WANIP = QString::fromUtf8("GetInfo_WANIPConnection");
static const QString CMD_AUTHENTICATE = QString::fromUtf8("Authenticate");
static const QString CMD_ENABLE_PARENTAL_CONTROL = QString::fromUtf8("EnableParentalControl");
static const QString SOAP_ENDPOINT_1 = QString::fromUtf8("http://routerlogin.net/soap/server_sa/");
static const QString SOAP_ENDPOINT_2 = QString::fromUtf8("http://routerlogin.net:5000/soap/server_sa/");

/*
 ROUTER_LOGIN = "routerlogin.net";
    CURRENT_SETTINGS = "http://routerlogin.net/currentsetting.htm";
    DEVICE_CONFIG_NS = "urn:NETGEAR-ROUTER:service:DeviceConfig:1";
    PARENTAL_CONTROL_NS = "urn:NETGEAR-ROUTER:service:ParentalControl:1";
    DEVICE_INFO_NS = "urn:NETGEAR-ROUTER:service:DeviceInfo:1";
    WANIP_CONNECTION_NS = "urn:NETGEAR-ROUTER:service:WANIPConnection:1";
    CMD_CONFIGURATION_STARTED = "ConfigurationStarted";
    CMD_CONFIGURATION_FINISHED = "ConfigurationFinished";
    CMD_GET_DNS_MASQ_DEVICE_ID = "GetDNSMasqDeviceID";
    CMD_SET_DNS_MASQ_DEVICE_ID = "SetDNSMasqDeviceID";
    CMD_DELETE_MAC_ADDRESS = "DeleteMACAddress";
    CMD_GET_ENABLED_STATUS = "GetEnableStatus";
    CMD_GET_DEVICE_INFO = "GetInfo_DeviceInfo";
    CMD_GET_WANIP = "GetInfo_WANIPConnection";
    MAGIC_VALUE = "99999999999999999999";
    CMD_AUTHENTICATE = "Authenticate";
    SOAP_ENDPOINT_1 = "http://routerlogin.net/soap/server_sa/";
    SOAP_ENDPOINT_2 = "http://routerlogin.net:5000/soap/server_sa/";
    SOAP_ENDPOINT_NONE_WORKS = "no soap endpoint found";
    soapEndpointToUse = null;
    SoapTemplate = "<?xml version=\"1.0\" encoding=\"utf-8\" ?> \r\n<env:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\r\n        xmlns:env=\"http://schemas.xmlsoap.org/soap/envelope/\">\r\n  <env:Body>\r\n{0}\r\n  </env:Body> \r\n</env:Envelope>";
    strictHttpParsingDisabled = false;
    CMD_ENABLE_PARENTAL_CONTROL = "EnableParentalControl";
*/

SoapPostOp *SoapPostOp::begin(const QString& url, const QString& ns, const QString& cmd, const QStringList& paramNames, const QStringList& paramValues, QObject *parent)
{
	if (paramNames.length() != paramValues.length()) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("invalid arguments: paramNames.length() != paramValues.length(), VERY BAD!!!"));
		return NULL;
	}

	QByteArray cmdUtf8 = cmd.toUtf8();

	QByteArray body;
	body.append("<");
	body.append(cmdUtf8);
	body.append(">\r\n");

	for (int i = 0; i < paramNames.length(); i++) {
		QByteArray nameUtf8 = paramNames.at(i).toUtf8();
		body.append("<");
		body.append(nameUtf8);
		body.append(">");

		body.append(paramValues.at(i).toUtf8());

		body.append("</");
		body.append(nameUtf8);
		body.append(">\r\n");
	}

	body.append("</");
	body.append(cmdUtf8);
	body.append(">");

	QByteArray xml = SOAP_TEMPLATE_P1 + body + SOAP_TEMPLATE_P2;
	DBGLOG(LOG_DEBUG, 20, QString::fromUtf8("soapPost xml: %1").arg(QString::fromUtf8(xml.data())));

	return new SoapPostOp(url, ns, cmd, xml, parent);
}

SoapPostOp::SoapPostOp(const QString& url, const QString& ns, const QString& cmd, const QByteArray& xml, QObject *parent)
: AsyncOp(parent), m_stage(Stage_Constructed), m_socket(NULL), m_url(url), m_ns(ns), m_cmd(cmd), m_xml(xml)
{
	connect(this, SIGNAL(start()), SLOT(stageStarted()), Qt::QueuedConnection);
	emit start();
}

SoapPostOp::~SoapPostOp()
{
	if (m_socket) {
		delete m_socket;
		m_socket = NULL;
	}
}

void SoapPostOp::abortImpl()
{
	switch (m_stage) {
	case Stage_Connecting:
	case Stage_Posting:
		if (m_socket) {
			m_socket->abort();
			m_socket->deleteLater();
			m_socket = NULL;
		} else {
			DBGLOG(LOG_DEBUG, 3, QString::fromUtf8("Unexpected m_socket == NULL, stage = %1").arg(m_stage));
		}
		break;
	default:
		break;
	}
}

void SoapPostOp::stageStarted()
{
	QUrl url(m_url);

	if (url.scheme() != QString::fromUtf8("http")) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("must be http!"));
		return report(WTFStatus_UnexpectedError);
	}

	m_socket = new QTcpSocket(this);
	connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(socketError(QAbstractSocket::SocketError)));
	connect(m_socket, SIGNAL(connected()), SLOT(socketConnected()));

	// QT: On some operating systems the connected() signal may be directly emitted from the connectToHost() call for connections to the localhost.
	m_connected = false;
	m_inConnectToHost = true;
	int port = url.port();
	if (port == -1) {
		port = 80;
	}
	m_socket->connectToHost(url.host(), port);
	m_inConnectToHost = false;

	if (m_connected) {
		sendRequest();
	} else {
		m_stage = Stage_Connecting;
	}
}

void SoapPostOp::socketError(QAbstractSocket::SocketError error)
{
	switch (m_stage) {
	case Stage_Connecting:
		return report(WTFStatus_SocketConnectFaild);
	default:
		break;
	}
}

void SoapPostOp::socketConnected()
{
	if (m_inConnectToHost) {
		m_connected = true;
		return;
	}

	sendRequest();
}

void SoapPostOp::socketReadyRead()
{
	QByteArray data = m_socket->readAll();
	m_socket->deleteLater();
	m_socket = NULL;

	DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("response: %1").arg(QString::fromUtf8(data)));

	//setProperty("var

	report(WTFStatus_NoError);
}

void SoapPostOp::sendRequest()
{
	QUrl url(m_url);

	QByteArray data;
	data.append("POST ");
	data.append(url.path());
	data.append(" HTTP/1.0\r\nContent-Type: text/xml; charset=utf-8\r\nAccept: text/xml\r\n");
	data.append("SOAPAction: \"");
	data.append(m_ns.toUtf8());
	data.append("#");
	data.append(m_cmd.toUtf8());
	data.append("\"\r\n");
	data.append("Host: ");
	data.append(url.host());
	data.append("\r\n");
	data.append("Content-Length: ");
	data.append(QString::fromUtf8("%1").arg(m_xml.length()).toUtf8());
	data.append("\r\n\r\n");
//	data.append(m_xml);

	DBGLOG(LOG_DEBUG, 8, QString::fromUtf8("full post data: %1").arg(QString::fromUtf8(data)));

	const char *pos = data.data();
	qint64 bytesRemain = data.length();
	while (bytesRemain > 0) {
		qint64 bytesWritten = m_socket->write(pos, bytesRemain);
		if (bytesWritten < 0) {
			DBGLOG(LOG_DEBUG, 3, QString::fromUtf8("write failed!"));
			return report(WTFStatus_SocketWriteFailed);
		} else if (bytesWritten > 0) {
			bytesRemain -= bytesWritten;
			pos += bytesWritten;
		} else {
			DBGLOG(LOG_DEBUG, 3, QString::fromUtf8("write returns ZERO!!!"));
			// TODO: sleep ?
		}
	}

	connect(m_socket, SIGNAL(readyRead()), SLOT(socketReadyRead()));
	m_stage = Stage_Posting;
}

QNetworkReply *soapPost(QNetworkAccessManager *nam, const QString& action, const QString& ns, const QString& cmd, const QStringList& paramNames, const QStringList& paramValues)
{
	if (paramNames.length() != paramValues.length()) {
		DBGLOG(LOG_ERROR, 1, QString::fromUtf8("Invalid arguments! paramNames.length() != paramValues.length(), VERY BAD!!"));
		return NULL;
	}

	QByteArray cmdUtf8 = cmd.toUtf8();

	QByteArray body;
	body.append("<");
	body.append(cmdUtf8);
	body.append(">\r\n");

	for (int i = 0; i < paramNames.length(); i++) {
		QByteArray nameUtf8 = paramNames.at(i).toUtf8();
		body.append("<");
		body.append(nameUtf8);
		body.append(">");

		body.append(paramValues.at(i).toUtf8());

		body.append("</");
		body.append(nameUtf8);
		body.append(">\r\n");
	}

	body.append("</");
	body.append(cmdUtf8);
	body.append(">");

	QByteArray xml = SOAP_TEMPLATE_P1 + body + SOAP_TEMPLATE_P2;
	DBGLOG(LOG_DEBUG, 20, QString::fromUtf8("soapPost xml: %1").arg(QString::fromUtf8(xml.data())));

	QNetworkRequest req(action);
	req.setHeader(QNetworkRequest::ContentTypeHeader, QString::fromUtf8("text/xml; charset=utf-8"));
	req.setHeader(QNetworkRequest::ContentLengthHeader, xml.length());
	req.setRawHeader("Accept", "text/xml");
	req.setRawHeader("SOAPAction", QString::fromUtf8("\"%1#%2\"").arg(ns).arg(cmd).toUtf8());
	//req.setRawHeader("Connection", "close");

	return nam->post(req, xml);
};

QNetworkReply *soapRouterAuth(QNetworkAccessManager *nam, const QString& action, const QString& username, const QString& password)
{
	QStringList paramNames;
	paramNames << QString::fromUtf8("NewUsername") << QString::fromUtf8("NewPassword");
	QStringList paramValues;
	paramValues << username << password;
	return soapPost(nam, action, NS_PARENTAL_CONTROL, CMD_AUTHENTICATE, paramNames, paramValues);
}

AsyncOp *soapRouterAuth2(const QString& action, const QString& username, const QString& password)
{
	QStringList paramNames;
	paramNames << QString::fromUtf8("NewUsername") << QString::fromUtf8("NewPassword");
	QStringList paramValues;
	paramValues << username << password;
	return Invoker::beginSoapPost(action, NS_PARENTAL_CONTROL, CMD_AUTHENTICATE, paramNames, paramValues);
}

bool getResponseCode(const QByteArray& response, int& code)
{
	const char *tag1 = "ResponseCode>";
	const int tagLen1 = 13;
	int pos1 = response.indexOf(tag1);
	if (pos1 < 0) {
		return false;
	}

	pos1 += tagLen1;

	int pos2 = response.indexOf("</", pos1);
	if (pos2 < 0) {
		return false;
	}

	QByteArray d(response.data() + pos1, pos2 - pos1);
	bool ok;
	code = d.toInt(&ok);
	if (!ok) {
		return false;
	}

	return true;
}

CheckRouterStatusOp::CheckRouterStatusOp(const QString& username, const QString& password, QNetworkAccessManager *nam, QObject *parent)
: AsyncOp(parent), m_username(username), m_password(password), m_nam(nam), m_stage(Stage_Constructed), m_reply(NULL), m_op(NULL)
{
	connect(this, SIGNAL(start()), SLOT(stageStarted()), Qt::QueuedConnection);
	emit start();
}

CheckRouterStatusOp::~CheckRouterStatusOp()
{
	if (m_reply) {
		m_reply->deleteLater();
		m_reply = NULL;
	}

	if (m_op) {
		m_op->deleteLater();
		m_op = NULL;
	}
}

void CheckRouterStatusOp::abortImpl()
{
	DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Abort request at stage %1").arg(m_stage));

	switch (m_stage) {
	case Stage_Constructed:
		break;
	case Stage_Dns1:
	case Stage_Dns2:
		QHostInfo::abortHostLookup(m_dnsLookupId);
		break;
	case Stage_RouterLogin:
		if (m_op) {
			m_op->abort();
			m_op->deleteLater();
			m_op = NULL;
		} else {
			DBGLOG(LOG_DEBUG, 3, QString::fromUtf8("Unexpected m_op == NULL, stage = %1").arg(m_stage));
		}
		break;
	case Stage_Check1:
		if (m_reply) {
			m_reply->abort();
			m_reply->deleteLater();
			m_reply = NULL;
		} else {
			DBGLOG(LOG_DEBUG, 3, QString::fromUtf8("Unexpected m_reply == NULL, stage = %1").arg(m_stage));
		}
		break;
	}
}

void CheckRouterStatusOp::stageStarted()
{
	if (isAborted()) {
		DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Aborted at 'constructed' stage"));
		return report(WTFStatus_Aborted);
	}

	m_dnsLookupId = QHostInfo::lookupHost(CHECK_DNS_1, this, SLOT(stageDns1Finished(QHostInfo)));
	m_stage = Stage_Dns1;
}

void CheckRouterStatusOp::stageDns1Finished(QHostInfo hostInfo)
{
	if (isAborted()) {
		DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Aborted at 'dns1' stage"));
		return report(WTFStatus_Aborted);
	}

	switch (hostInfo.error()) {
	case QHostInfo::NoError:
		break;
	default:
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("Resolving %1 failed").arg(CHECK_DNS_1));
		return report(RouterStatus_NoNetwork);
	}

	if (hostInfo.addresses().size() != 1) {
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("Resolving %1 failed, unexpected").arg(CHECK_DNS_1));
		return report(WTFStatus_UnexpectedError);
	}

	DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("Resolving %1 got %2").arg(CHECK_DNS_1).arg(hostInfo.addresses().at(0).toString()));

	m_dnsLookupId = QHostInfo::lookupHost(CHECK_DNS_2, this, SLOT(stageDns2Finished(QHostInfo)));
	m_stage = Stage_Dns2;
}

void CheckRouterStatusOp::stageDns2Finished(QHostInfo hostInfo)
{
	if (isAborted()) {
		DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Aborted at 'dns2' stage"));
		return report(WTFStatus_Aborted);
	}

	switch (hostInfo.error()) {
	case QHostInfo::NoError:
		break;
	case QHostInfo::HostNotFound:
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("Resolving %2 failed, host not found").arg(CHECK_DNS_2));
		return report(RouterStatus_NoRouter);
	default:
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("Resolving %2 failed").arg(CHECK_DNS_2));
		return report(RouterStatus_NoNetwork);
	}

	if (hostInfo.addresses().size() != 1) {
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("Resolving %2 failed, unexpected").arg(CHECK_DNS_2));
		return report(WTFStatus_UnexpectedError);
	}

	const QHostAddress& address = hostInfo.addresses().at(0);

	if (address.protocol() != QAbstractSocket::IPv4Protocol) {
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("Resolving %2 failed, unexpected").arg(CHECK_DNS_2));
		return report(WTFStatus_UnexpectedError);
	}

	DBGLOG(LOG_INFO, 4, QString::fromUtf8("Gateway IP: %1").arg(address.toString()));

	quint32 ipv4 = address.toIPv4Address();
	if (!isPrivateAddress(ipv4)) {
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("Not a private IPV4, unexpected").arg(CHECK_DNS_2));
		return report(WTFStatus_UnexpectedError);
	}

	QByteArray mac;
	if (!getGatewayMacAddress(ipv4, mac)) {
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("getGatewayMacAddress failed"));
		return report(WTFStatus_UnexpectedError);
	}

	DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("Gateway MAC: %1:%2:%3:%4:%5:%6")
		.arg(static_cast<uchar>(mac[0]), 2, 16, QChar::fromAscii('0'))
		.arg(static_cast<uchar>(mac[1]), 2, 16, QChar::fromAscii('0'))
		.arg(static_cast<uchar>(mac[2]), 2, 16, QChar::fromAscii('0'))
		.arg(static_cast<uchar>(mac[3]), 2, 16, QChar::fromAscii('0'))
		.arg(static_cast<uchar>(mac[4]), 2, 16, QChar::fromAscii('0'))
		.arg(static_cast<uchar>(mac[5]), 2, 16, QChar::fromAscii('0')));

	m_op = Invoker::beginRouterLogin(m_username, m_password, m_nam, this);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 4, QString::fromUtf8("Invoker::beginRouterLogin failed, unexpected!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageRouterLoginFinished()));
	m_stage = Stage_RouterLogin;
}

void CheckRouterStatusOp::stageRouterLoginFinished()
{
	if (m_op == NULL || m_op->isAborted()) {
		DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Aborted at 'routerlogin' stage"));
		return report(WTFStatus_Aborted);
	}

	m_op->deleteLater();

	int err = m_op->result().toInt();
	switch (err) {
	case WTFStatus_NoError:
		setProperty("varSoapAction", m_op->property("varSoapAction"));
		break;
	default:
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("Unknown error at 'routerauth', err = %1").arg(err));
		m_op = NULL;
		return report(err);
	}

	m_op = NULL;

	QNetworkRequest req(CURRENT_SETTINGS);
	m_reply = m_nam->get(req);
	if (!m_reply) {
		DBGLOG(LOG_ERROR, 4, QString::fromUtf8("QNetworkAccessManager::get failed, unexpected!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_reply, SIGNAL(finished()), SLOT(stageCheck1Finished()));
	m_stage = Stage_Check1;

}

void CheckRouterStatusOp::stageCheck1Finished()
{
	if (isAborted() || m_reply == NULL) {
		DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Aborted at 'check1' stage"));
		return report(WTFStatus_Aborted);
	}

	m_reply->deleteLater();

	switch (m_reply->error()) {
	case QNetworkReply::NoError:
		break;
	default:
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("Check1 failed, NetworkError = %1").arg(m_reply->error()));
		m_reply = NULL;
		return report(WTFStatus_UnknownError);
	}

	QByteArray response = m_reply->readAll();
	m_reply = NULL;

	DBGLOG(LOG_DEBUG, 8, QString::fromUtf8("current settings: %1").arg(QString::fromUtf8(response.data())));

	QString firmware;
	QString supportLPC;
	QString model;

	//QList<QByteArray> settings = response.split('\r');
	QList<QByteArray> settings = nice_split(response);
	for (int i = 0; i < settings.length(); i++) {
		QByteArray item = settings.at(i);
		QList<QByteArray> parts = item.split('=');
		if (parts.length() == 2) {
			QByteArray key = parts.at(0);
			if (key == "ParentalControlSupported") {
				supportLPC = QString::fromUtf8(parts.at(1).trimmed());
			} else if (key == "Model") {
				model = QString::fromUtf8(parts.at(1).trimmed());
			} else if (key == "Firmware") {
				firmware = QString::fromUtf8(parts.at(1).trimmed());
			}
		}
	}

	if (firmware.isEmpty() || supportLPC.isEmpty() || model.isEmpty()) {
		return report(RouterStatus_ParentalControlNotEnabled);
	}

	if (supportLPC.at(0).toAscii() != '1') {
		return report(RouterStatus_ParentalControlNotEnabled);
	}

	setProperty("varFirmware", firmware);
	setProperty("varModel", model);
	return report(WTFStatus_NoError);
}

RouterLoginOp::RouterLoginOp(const QString& username, const QString& password, QNetworkAccessManager *nam, QObject *parent)
: AsyncOp(parent), m_username(username), m_password(password), m_nam(nam), m_reply(NULL)
{
	connect(this, SIGNAL(start()), SLOT(stageStarted()), Qt::QueuedConnection);
	emit start();
}

RouterLoginOp::~RouterLoginOp()
{
	if (m_reply) {
		m_reply->deleteLater();
		m_reply = NULL;
	}
}

void RouterLoginOp::abortImpl()
{
	DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Abort request at stage %1").arg(m_stage));

	switch (m_stage) {
	case Stage_Constructed:
		break;
	case Stage_Auth1:
	case Stage_Auth2:
		if (m_reply) {
			m_reply->abort();
			m_reply->deleteLater();
			m_reply = NULL;
		} else {
			DBGLOG(LOG_DEBUG, 3, QString::fromUtf8("Unexpected m_reply == NULL, stage = %1").arg(m_stage));
		}
		break;
	}
}

void RouterLoginOp::stageStarted()
{
	if (isAborted()) {
		DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Aborted at 'constructed' stage"));
		return report(WTFStatus_Aborted);
	}

	m_reply = soapRouterAuth(m_nam, SOAP_ENDPOINT_1, m_username, m_password);
	if (!m_reply) {
		DBGLOG(LOG_ERROR, 4, QString::fromUtf8("soapPost failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_reply, SIGNAL(finished()), SLOT(stageAuth1Finished()));
	m_stage = Stage_Auth1;
}

void RouterLoginOp::stageAuth1Finished()
{
	if (isAborted() || m_reply == NULL) {
		DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Aborted at 'auth1' stage"));
		return report(WTFStatus_Aborted);
	}

	m_reply->deleteLater();

	switch (m_reply->error()) {
	case QNetworkReply::NoError:
		setProperty("varSoapAction", SOAP_ENDPOINT_1);
		break;
	case QNetworkReply::ConnectionRefusedError:
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("Auth1 refused, try another url"));
		m_reply = soapRouterAuth(m_nam, SOAP_ENDPOINT_2, m_username, m_password);
		if (!m_reply) {
			DBGLOG(LOG_ERROR, 4, QString::fromUtf8("soapPost failed!"));
			return report(WTFStatus_UnexpectedError);
		}
		connect(m_reply, SIGNAL(finished()), SLOT(stageAuth2Finished()));
		m_stage = Stage_Auth2;
		return;
	default:
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("Auth1 failed, NetworkError = %1").arg(m_reply->error()));
		m_reply = NULL;
		return report(WTFStatus_UnknownError);
	}

	QByteArray response = m_reply->readAll();
	m_reply = NULL;
	completeAuth(response);
}

void RouterLoginOp::stageAuth2Finished()
{
	if (isAborted() || m_reply == NULL) {
		DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Aborted at 'auth2' stage"));
		return report(WTFStatus_Aborted);
	}

	m_reply->deleteLater();

	switch (m_reply->error()) {
	case QNetworkReply::NoError:
		setProperty("varSoapAction", SOAP_ENDPOINT_2);
		break;
	default:
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("Auth2 failed, NetworkError = %1").arg(m_reply->error()));
		m_reply = NULL;
		return report(WTFStatus_UnknownError);
	}

	QByteArray response = m_reply->readAll();
	m_reply = NULL;
	completeAuth(response);
}

void RouterLoginOp::completeAuth(const QByteArray& response)
{
	DBGLOG(LOG_DEBUG, 8, QString::fromUtf8("auth response xml: %1").arg(QString::fromUtf8(response.data())));
	int code;
	if (!getResponseCode(response, code)) {
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("getResponseCode failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	switch (code) {
	case 0:
		break;
	case 401:
		return report(WTFStatus_AuthenticationFailed);
	default:
		return report(WTFStatus_UnknownError);
	}

	report(WTFStatus_NoError);
}

RouterLoginOp2::RouterLoginOp2(const QString& username, const QString& password, QObject *parent)
: AsyncOp(parent), m_username(username), m_password(password), m_op(NULL)
{
	connect(this, SIGNAL(start()), SLOT(stageStarted()), Qt::QueuedConnection);
	emit start();
}

RouterLoginOp2::~RouterLoginOp2()
{
	if (m_op) {
		m_op->deleteLater();
		m_op = NULL;
	}
}

void RouterLoginOp2::abortImpl()
{
	DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Abort request at stage %1").arg(m_stage));

	switch (m_stage) {
	case Stage_Constructed:
		break;
	case Stage_Auth1:
	case Stage_Auth2:
		if (m_op) {
			m_op->abort();
			m_op->deleteLater();
			m_op = NULL;
		} else {
			DBGLOG(LOG_DEBUG, 3, QString::fromUtf8("Unexpected m_op == NULL, stage = %1").arg(m_stage));
		}
		break;
	}
}

void RouterLoginOp2::stageStarted()
{
	if (isAborted()) {
		DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Aborted at 'constructed' stage"));
		return report(WTFStatus_Aborted);
	}

	m_op = soapRouterAuth2(SOAP_ENDPOINT_1, m_username, m_password);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 4, QString::fromUtf8("soapPost failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageAuth1Finished()));
	m_stage = Stage_Auth1;
}

void RouterLoginOp2::stageAuth1Finished()
{
	if (isAborted() || m_op == NULL) {
		DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Aborted at 'auth1' stage"));
		return report(WTFStatus_Aborted);
	}

	m_op->deleteLater();

	int error = m_op->result().toInt();
	switch (error) {
	case WTFStatus_NoError:
		setProperty("varSoapAction", SOAP_ENDPOINT_1);
		break;
	case WTFStatus_SocketConnectFaild:
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("Auth1 refused, try another url"));
		m_op = soapRouterAuth2(SOAP_ENDPOINT_2, m_username, m_password);
		if (!m_op) {
			DBGLOG(LOG_ERROR, 4, QString::fromUtf8("soapPost failed!"));
			return report(WTFStatus_UnexpectedError);
		}
		connect(m_op, SIGNAL(finished()), SLOT(stageAuth2Finished()));
		m_stage = Stage_Auth2;
		return;
	default:
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("Auth1 failed, NetworkError = %1").arg(error));
		m_op = NULL;
		return report(WTFStatus_UnknownError);
	}

	QByteArray response = m_op->property("varResponseContent").toByteArray();
	m_op = NULL;
	completeAuth(response);
}

void RouterLoginOp2::stageAuth2Finished()
{
	if (isAborted() || m_op == NULL) {
		DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Aborted at 'auth2' stage"));
		return report(WTFStatus_Aborted);
	}

	m_op->deleteLater();

	int error = m_op->result().toInt();
	switch (error) {
	case WTFStatus_NoError:
		setProperty("varSoapAction", SOAP_ENDPOINT_2);
		break;
	default:
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("Auth2 failed, error = %1").arg(error));
		m_op = NULL;
		return report(WTFStatus_UnknownError);
	}

	QByteArray response = m_op->property("varResponseContent").toByteArray();
	m_op = NULL;
	completeAuth(response);
}

void RouterLoginOp2::completeAuth(const QByteArray& response)
{
	DBGLOG(LOG_DEBUG, 8, QString::fromUtf8("auth response xml: %1").arg(QString::fromUtf8(response.data())));
	int code;
	if (!getResponseCode(response, code)) {
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("getResponseCode failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	switch (code) {
	case 0:
		break;
	case 401:
		return report(WTFStatus_AuthenticationFailed);
	default:
		return report(WTFStatus_UnknownError);
	}

	report(WTFStatus_NoError);
}

RouterConfigurationStart::RouterConfigurationStart(const QString& action, QNetworkAccessManager *nam, QObject *parent)
: AsyncOp(parent), m_action(action), m_nam(nam)
{
	connect(this, SIGNAL(start()), SLOT(stageStarted()), Qt::QueuedConnection);
	emit start();
}

RouterConfigurationStart::~RouterConfigurationStart()
{
	if (m_reply) {
		m_reply->deleteLater();
		m_reply = NULL;
	}
}

void RouterConfigurationStart::abortImpl()
{
	DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Abort request at stage %1").arg(m_stage));

	switch (m_stage) {
	case Stage_Constructed:
		break;
	case Stage_Post:
		if (m_reply) {
			m_reply->abort();
			m_reply->deleteLater();
			m_reply = NULL;
		} else {
			DBGLOG(LOG_DEBUG, 3, QString::fromUtf8("Unexpected m_reply == NULL, stage = %1").arg(m_stage));
		}
		break;
	}
}

void RouterConfigurationStart::stageStarted()
{
	if (isAborted()) {
		DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Aborted at 'constructed' stage"));
		return report(WTFStatus_Aborted);
	}

	QStringList paramNames;
	paramNames << QString::fromUtf8("NewSessionID");
	QStringList paramValues;
	paramValues << MAGIC_VALUE;
	m_reply = soapPost(m_nam, m_action, NS_DEVICE_CONFIG, CMD_CONFIGURATION_STARTED, paramNames, paramValues);
	if (!m_reply) {
		DBGLOG(LOG_ERROR, 4, QString::fromUtf8("soapPost failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_reply, SIGNAL(finished()), SLOT(stagePostFinished()));
	m_stage = Stage_Post;
}

void RouterConfigurationStart::stagePostFinished()
{
	if (isAborted() || m_reply == NULL) {
		DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Aborted at 'post' stage"));
		return report(WTFStatus_Aborted);
	}

	m_reply->deleteLater();

	switch (m_reply->error()) {
	case QNetworkReply::NoError:
		break;
	default:
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("Post failed, NetworkError = %1").arg(m_reply->error()));
		m_reply = NULL;
		return report(WTFStatus_UnknownError);
	}

	QByteArray response = m_reply->readAll();
	m_reply = NULL;

	DBGLOG(LOG_DEBUG, 8, QString::fromUtf8("post response xml: %1").arg(QString::fromUtf8(response.data())));
	int code;
	if (!getResponseCode(response, code)) {
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("getResponseCode failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	switch (code) {
	case 0:
		break;
	case 401:
		return report(WTFStatus_AuthenticationFailed);
	default:
		return report(WTFStatus_UnknownError);
	}

	report(WTFStatus_NoError);
}

RouterConfigurationFinish::RouterConfigurationFinish(const QString& action, QNetworkAccessManager *nam, QObject *parent)
: AsyncOp(parent), m_action(action), m_nam(nam)
{
	connect(this, SIGNAL(start()), SLOT(stageStarted()), Qt::QueuedConnection);
	emit start();
}

RouterConfigurationFinish::~RouterConfigurationFinish()
{
	if (m_reply) {
		m_reply->deleteLater();
		m_reply = NULL;
	}
}

void RouterConfigurationFinish::abortImpl()
{
	DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Abort request at stage %1").arg(m_stage));

	switch (m_stage) {
	case Stage_Constructed:
		break;
	case Stage_Post:
		if (m_reply) {
			m_reply->abort();
			m_reply->deleteLater();
			m_reply = NULL;
		} else {
			DBGLOG(LOG_DEBUG, 3, QString::fromUtf8("Unexpected m_reply == NULL, stage = %1").arg(m_stage));
		}
		break;
	}
}

void RouterConfigurationFinish::stageStarted()
{
	if (isAborted()) {
		DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Aborted at 'constructed' stage"));
		return report(WTFStatus_Aborted);
	}

	QStringList paramNames;
	QStringList paramValues;
	m_reply = soapPost(m_nam, m_action, NS_DEVICE_CONFIG, CMD_CONFIGURATION_FINISHED, paramNames, paramValues);
	if (!m_reply) {
		DBGLOG(LOG_ERROR, 4, QString::fromUtf8("soapPost failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_reply, SIGNAL(finished()), SLOT(stagePostFinished()));
	m_stage = Stage_Post;
}

void RouterConfigurationFinish::stagePostFinished()
{
	if (isAborted() || m_reply == NULL) {
		DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Aborted at 'post' stage"));
		return report(WTFStatus_Aborted);
	}

	m_reply->deleteLater();

	switch (m_reply->error()) {
	case QNetworkReply::NoError:
		break;
	default:
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("Post failed, NetworkError = %1").arg(m_reply->error()));
		m_reply = NULL;
		return report(WTFStatus_UnknownError);
	}

	QByteArray response = m_reply->readAll();
	m_reply = NULL;

	DBGLOG(LOG_DEBUG, 8, QString::fromUtf8("post response xml: %1").arg(QString::fromUtf8(response.data())));
	int code;
	if (!getResponseCode(response, code)) {
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("getResponseCode failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	switch (code) {
	case 0:
		break;
	case 401:
		return report(WTFStatus_AuthenticationFailed);
	default:
		return report(WTFStatus_UnknownError);
	}

	report(WTFStatus_NoError);
}

RouterGetDnsMasqDeviceId::RouterGetDnsMasqDeviceId(const QString& action, const QString& macAddress, QNetworkAccessManager *nam, QObject *parent)
: AsyncOp(parent), m_action(action), m_macAddress(macAddress), m_nam(nam)
{
	connect(this, SIGNAL(start()), SLOT(stageStarted()), Qt::QueuedConnection);
	emit start();
}

RouterGetDnsMasqDeviceId::~RouterGetDnsMasqDeviceId()
{
	if (m_reply) {
		m_reply->deleteLater();
		m_reply = NULL;
	}
}

void RouterGetDnsMasqDeviceId::abortImpl()
{
	DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Abort request at stage %1").arg(m_stage));

	switch (m_stage) {
	case Stage_Constructed:
		break;
	case Stage_Post:
		if (m_reply) {
			m_reply->abort();
			m_reply->deleteLater();
			m_reply = NULL;
		} else {
			DBGLOG(LOG_DEBUG, 3, QString::fromUtf8("Unexpected m_reply == NULL, stage = %1").arg(m_stage));
		}
		break;
	}
}

void RouterGetDnsMasqDeviceId::stageStarted()
{
	if (isAborted()) {
		DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Aborted at 'constructed' stage"));
		return report(WTFStatus_Aborted);
	}

	QStringList paramNames;
	paramNames << QString::fromUtf8("NewMACAddress");
	QStringList paramValues;
	paramValues << m_macAddress;
	m_reply = soapPost(m_nam, m_action, NS_PARENTAL_CONTROL, CMD_GET_DNS_MASQ_DEVICE_ID, paramNames, paramValues);
	if (!m_reply) {
		DBGLOG(LOG_ERROR, 4, QString::fromUtf8("soapPost failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_reply, SIGNAL(finished()), SLOT(stagePostFinished()));
	m_stage = Stage_Post;
}

void RouterGetDnsMasqDeviceId::stagePostFinished()
{
	if (isAborted() || m_reply == NULL) {
		DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Aborted at 'post' stage"));
		return report(WTFStatus_Aborted);
	}

	m_reply->deleteLater();

	switch (m_reply->error()) {
	case QNetworkReply::NoError:
		break;
	default:
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("Post failed, NetworkError = %1").arg(m_reply->error()));
		m_reply = NULL;
		return report(WTFStatus_UnknownError);
	}

	QByteArray response = m_reply->readAll();
	m_reply = NULL;

	DBGLOG(LOG_DEBUG, 8, QString::fromUtf8("post response xml: %1").arg(QString::fromUtf8(response.data())));
	int code;
	if (!getResponseCode(response, code)) {
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("getResponseCode failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	switch (code) {
	case 0:
		break;
	default:
		// nothing to do
		// no device id
		return report(WTFStatus_NoError);
	}

	int pos1 = response.indexOf("<NewDeviceID>");
	int pos2 = response.indexOf("</NewDeviceID>");
	if (pos1 < 0 || pos2 < 0 || pos1 + 13 >= pos2) {
		DBGLOG(LOG_ERROR, 4, QString::fromUtf8("failed to find <NewDeviceID>, unexpected!!"));
		return report(WTFStatus_UnexpectedError);
	}

	pos1 += 13;
	QByteArray deviceId = response.mid(pos1, pos2 - pos1).trimmed();
	setProperty("varDeviceID", deviceId);

	DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("DeviceID: [%1]").arg(QString::fromUtf8(deviceId)));

	report(WTFStatus_NoError);
}

} // namespace impl
