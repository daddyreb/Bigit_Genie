#include "common.h"
#include "logger.h"
#include "webapi_p2.h"
#include <QStringList>

void getRidOfSslErrors(QNetworkReply *reply);

extern const char *API_KEY;

namespace WebApiImpl {

CheckNameAvailableOp::CheckNameAvailableOp(const QString& username, QNetworkAccessManager *nam, QObject *parent)
: BaseOp(nam, parent)
{
	m_command = "check_username";
	m_paramNames << QString::fromUtf8("username");
	m_paramValues << username;
}

int CheckNameAvailableOp::processImpl(const QVariantMap& result)
{
	QVariantMap::const_iterator it;
	it = result.find(QString::fromUtf8("status"));
	if (it == result.end()) {
		return WTFStatus_UnexpectedError;
	}

	QByteArray statusCode = it.value().toString().toUtf8().toLower();
	if (statusCode == "success") {
		it = result.find(QString::fromUtf8("response.available"));
		if (it == result.end()) {
			return WTFStatus_UnexpectedError;
		}
		QByteArray avail = it.value().toString().toUtf8().toLower();
		if (avail == "yes") {
			setProperty("varAvailable", true);
		} else {
			setProperty("varAvailable", false);
		}
	} else {
		return WTFStatus_UnexpectedError;
	}

	return WTFStatus_NoError;
}

CreateAccountOp::CreateAccountOp(const QString& username, const QString& password, const QString& email, QNetworkAccessManager *nam, QObject *parent)
: BaseOp(nam, parent)
{
	m_command = "account_create";
	m_paramNames << QString::fromUtf8("username") << QString::fromUtf8("password") << QString::fromUtf8("email");
	m_paramValues << username << password << email;
}

int CreateAccountOp::processImpl(const QVariantMap& result)
{
	QVariantMap::const_iterator it;
	it = result.find(QString::fromUtf8("status"));
	if (it == result.end()) {
		return WTFStatus_UnexpectedError;
	}

	QByteArray statusCode = it.value().toString().toUtf8().toLower();
	if (statusCode == "success") {
	} else if (statusCode == "failure") {
		it = result.find(QString::fromUtf8("error"));
		if (it == result.end()) {
			return WTFStatus_UnexpectedError;
		}
		setProperty("varErrorCode", it.value().toInt());
		return WTFStatus_Failed;
	}

	return WTFStatus_NoError;
}

LoginOp::LoginOp(const QString& username, const QString& password, QNetworkAccessManager *nam, QObject *parent)
: BaseOp(nam, parent)
{
	m_command = "account_signin";
	m_paramNames << QString::fromUtf8("username") << QString::fromUtf8("password");
	m_paramValues << username << password;
}

int LoginOp::processImpl(const QVariantMap& result)
{
	QVariantMap::const_iterator it;
	it = result.find(QString::fromUtf8("status"));
	if (it == result.end()) {
		return WTFStatus_UnexpectedError;
	}

	QByteArray statusCode = it.value().toString().toUtf8().toLower();
	if (statusCode == "success") {
		it = result.find(QString::fromUtf8("response.token"));
		if (it == result.end()) {
			return WTFStatus_UnexpectedError;
		}
		setProperty("varToken", it.value().toString());
	} else if (statusCode == "failure") {
		it = result.find(QString::fromUtf8("error"));
		if (it == result.end()) {
			return WTFStatus_UnexpectedError;
		}
		int code = it.value().toInt();
		setProperty("varErrorCode", code);
		switch (code) {
		case 1004:
			return WTFStatus_AuthenticationFailed;
		default:
			return WTFStatus_Failed;
		}
	} else {
		return WTFStatus_UnexpectedError;
	}

	return WTFStatus_NoError;
}

GetLabelOp::GetLabelOp(const QString& token, const QString& deviceId, QNetworkAccessManager *nam, QObject *parent)
: BaseOp(nam, parent)
{
	m_command = "label_get";
	m_paramNames << QString::fromUtf8("token") << QString::fromUtf8("device_id");
	m_paramValues << token << deviceId;
}

int GetLabelOp::processImpl(const QVariantMap& result)
{
	QVariantMap::const_iterator it;
	it = result.find(QString::fromUtf8("status"));
	if (it == result.end()) {
		return WTFStatus_UnexpectedError;
	}

	QByteArray statusCode = it.value().toString().toUtf8().toLower();
	if (statusCode == "success") {
		/*it = result.find(QString::fromUtf8("response.token"));
		if (it == result.end()) {
			return WTFStatus_UnexpectedError;
		}
		setProperty("varToken", it.value().toString());*/
	} else if (statusCode == "failure") {
		it = result.find(QString::fromUtf8("error"));
		if (it == result.end()) {
			return WTFStatus_UnexpectedError;
		}
		setProperty("varErrorCode", it.value().toInt());
		return WTFStatus_Failed;
	} else {
		return WTFStatus_UnexpectedError;
	}

	return WTFStatus_NoError;
}

GetDeviceOp::GetDeviceOp(const QString& token, const QString& deviceKey, QNetworkAccessManager *nam, QObject *parent)
: BaseOp(nam, parent)
{
	m_command = "device_get";
	m_paramNames << QString::fromUtf8("token") << QString::fromUtf8("device_key");
	m_paramValues << token << deviceKey;
}

int GetDeviceOp::processImpl(const QVariantMap& result)
{
	QVariantMap::const_iterator it;
	it = result.find(QString::fromUtf8("status"));
	if (it == result.end()) {
		return WTFStatus_UnexpectedError;
	}

	QByteArray statusCode = it.value().toString().toUtf8().toLower();
	if (statusCode == "success") {
		it = result.find(QString::fromUtf8("response.device_id"));
		if (it == result.end()) {
			return WTFStatus_UnexpectedError;
		}
		setProperty("varDeviceID", it.value().toString());
	} else if (statusCode == "failure") {
		it = result.find(QString::fromUtf8("error"));
		if (it == result.end()) {
			return WTFStatus_UnexpectedError;
		}
		setProperty("varErrorCode", it.value().toInt());
		return WTFStatus_Failed;
	} else {
		return WTFStatus_UnexpectedError;
	}

	return WTFStatus_NoError;
}

CreateDeviceOp::CreateDeviceOp(const QString& token, const QString& deviceKey, QNetworkAccessManager *nam, QObject *parent)
: BaseOp(nam, parent)
{
	m_command = "device_create";
	m_paramNames << QString::fromUtf8("token") << QString::fromUtf8("device_key");
	m_paramValues << token << deviceKey;
}

int CreateDeviceOp::processImpl(const QVariantMap& result)
{
	QVariantMap::const_iterator it;
	it = result.find(QString::fromUtf8("status"));
	if (it == result.end()) {
		return WTFStatus_UnexpectedError;
	}

	QByteArray statusCode = it.value().toString().toUtf8().toLower();
	if (statusCode == "success") {
		it = result.find(QString::fromUtf8("response.device_id"));
		if (it == result.end()) {
			return WTFStatus_UnexpectedError;
		}
		setProperty("varDeviceID", it.value().toString());
	} else if (statusCode == "failure") {
		it = result.find(QString::fromUtf8("error"));
		if (it == result.end()) {
			return WTFStatus_UnexpectedError;
		}
		setProperty("varErrorCode", it.value().toInt());
		return WTFStatus_Failed;
	} else {
		return WTFStatus_UnexpectedError;
	}

	return WTFStatus_NoError;
}

GetFiltersOp::GetFiltersOp(const QString& token, const QString& deviceId, QNetworkAccessManager *nam, QObject *parent)
: BaseOp(nam, parent)
{
	m_command = "filters_get";
	m_paramNames << QString::fromUtf8("token") << QString::fromUtf8("device_id");
	m_paramValues << token << deviceId;
}

int GetFiltersOp::processImpl(const QVariantMap& result)
{
	QVariantMap::const_iterator it;
	it = result.find(QString::fromUtf8("status"));
	if (it == result.end()) {
		return WTFStatus_UnexpectedError;
	}

	QByteArray statusCode = it.value().toString().toUtf8().toLower();
	if (statusCode == "success") {
		it = result.find(QString::fromUtf8("response.bundle"));
		if (it == result.end()) {
			return WTFStatus_UnexpectedError;
		}
		setProperty("varBundle", it.value().toString());
	} else if (statusCode == "failure") {
		it = result.find(QString::fromUtf8("error"));
		if (it == result.end()) {
			return WTFStatus_UnexpectedError;
		}
		int code = it.value().toInt();
		setProperty("varErrorCode", code);
		switch (code) {
		case 4001:
			return WebApiStatus_DeviceIdNotMine;
		default:
			return WTFStatus_Failed;
		}
	} else {
		return WTFStatus_UnexpectedError;
	}

	return WTFStatus_NoError;
}

SetFiltersOp::SetFiltersOp(const QString& token, const QString& deviceId, const QString& bundle, QNetworkAccessManager *nam, QObject *parent)
: BaseOp(nam, parent)
{
	m_command = "filters_set";
	m_paramNames << QString::fromUtf8("token") << QString::fromUtf8("device_id") << QString::fromUtf8("bundle");
	m_paramValues << token << deviceId << bundle;
}

int SetFiltersOp::processImpl(const QVariantMap& result)
{
	QVariantMap::const_iterator it;
	it = result.find(QString::fromUtf8("status"));
	if (it == result.end()) {
		return WTFStatus_UnexpectedError;
	}

	QByteArray statusCode = it.value().toString().toUtf8().toLower();
	if (statusCode == "success") {
	} else if (statusCode == "failure") {
		it = result.find(QString::fromUtf8("error"));
		if (it == result.end()) {
			return WTFStatus_UnexpectedError;
		}
		setProperty("varErrorCode", it.value().toInt());
		return WTFStatus_Failed;
	} else {
		return WTFStatus_UnexpectedError;
	}

	return WTFStatus_NoError;
}

AccountRelayOp::AccountRelayOp(const QString& token, QNetworkAccessManager *nam, QObject *parent)
: BaseOp(nam, parent)
{
	m_command = "account_relay";
	m_paramNames << QString::fromUtf8("token");
	m_paramValues << token;
}

int AccountRelayOp::processImpl(const QVariantMap& result)
{
	QVariantMap::const_iterator it;
	it = result.find(QString::fromUtf8("status"));
	if (it == result.end()) {
		return WTFStatus_UnexpectedError;
	}

	QByteArray statusCode = it.value().toString().toUtf8().toLower();
	if (statusCode == "success") {
		it = result.find(QString::fromUtf8("response.relay_token"));
		if (it == result.end()) {
			return WTFStatus_UnexpectedError;
		}

		static const QString BASE_URL = QString::fromUtf8("http://netgear.opendns.com/sign_in.php");

		setProperty("varRelayToken", it.value().toString());
		setProperty("varApiKey", QByteArray(API_KEY));
		setProperty("varBaseUrl", BASE_URL);
	} else if (statusCode == "failure") {
		it = result.find(QString::fromUtf8("error"));
		if (it == result.end()) {
			return WTFStatus_UnexpectedError;
		}
		setProperty("varErrorCode", it.value().toInt());
		return WTFStatus_Failed;
	} else {
		return WTFStatus_UnexpectedError;
	}

	return WTFStatus_NoError;
}

} // namespace WebApiImpl

namespace RouterApiImpl {

static const QString MAGIC_VALUE = QString::fromUtf8("99999999999999999999");
static const QString CURRENT_SETTINGS = QString::fromUtf8("http://routerlogin.net/currentsetting.htm");

static const QByteArray NS_DEVICE_CONFIG = "urn:NETGEAR-ROUTER:service:DeviceConfig:1";
static const QByteArray NS_PARENTAL_CONTROL = "urn:NETGEAR-ROUTER:service:ParentalControl:1";
static const QByteArray NS_DEVICE_INFO = "urn:NETGEAR-ROUTER:service:DeviceInfo:1";
static const QByteArray NS_WANIP_CONNECTION = "urn:NETGEAR-ROUTER:service:WANIPConnection:1";

static const QByteArray CMD_CONFIGURATION_STARTED = "ConfigurationStarted";
static const QByteArray CMD_CONFIGURATION_FINISHED = "ConfigurationFinished";
static const QByteArray CMD_GET_DNS_MASQ_DEVICE_ID = "GetDNSMasqDeviceID";
static const QByteArray CMD_SET_DNS_MASQ_DEVICE_ID = "SetDNSMasqDeviceID";
static const QByteArray CMD_DELETE_MAC_ADDRESS = "DeleteMACAddress";
static const QByteArray CMD_GET_ENABLED_STATUS = "GetEnableStatus";
static const QByteArray CMD_GET_DEVICE_INFO = "GetInfo_DeviceInfo";
static const QByteArray CMD_GET_WANIP = "GetInfo_WANIPConnection";
static const QByteArray CMD_AUTHENTICATE = "Authenticate";
static const QByteArray CMD_ENABLE_PARENTAL_CONTROL = "EnableParentalControl";

ConfigurationStartOp::ConfigurationStartOp(const QString& url, QNetworkAccessManager *nam, QObject *parent)
: BaseOp(url, nam, parent)
{
	m_namespace = NS_DEVICE_CONFIG;
	m_command = CMD_CONFIGURATION_STARTED;
	m_paramNames << QString::fromUtf8("NewSessionID");
	m_paramValues << MAGIC_VALUE;
}

int ConfigurationStartOp::processImpl(const QByteArray& data, const QVariantMap& result)
{
	Q_UNUSED(data)
	Q_UNUSED(result)
	return WTFStatus_NoError;
}

ConfigurationFinishOp::ConfigurationFinishOp(const QString& url, QNetworkAccessManager *nam, QObject *parent)
: BaseOp(url, nam, parent)
{
	m_namespace = NS_DEVICE_CONFIG;
	m_command = CMD_CONFIGURATION_FINISHED;
}

int ConfigurationFinishOp::processImpl(const QByteArray& data, const QVariantMap& result)
{
	Q_UNUSED(data)
	Q_UNUSED(result)
	return WTFStatus_NoError;
}

GetDNSMasqDeviceIDOp::GetDNSMasqDeviceIDOp(const QString& macAddress, const QString& url, QNetworkAccessManager *nam, QObject *parent)
: BaseOp(url, nam, parent)
{
	m_namespace = NS_PARENTAL_CONTROL;
	m_command = CMD_GET_DNS_MASQ_DEVICE_ID;
	m_paramNames << QString::fromUtf8("NewMACAddress");
	m_paramValues << macAddress;
}

int GetDNSMasqDeviceIDOp::processImpl(const QByteArray& data, const QVariantMap& result)
{
	Q_UNUSED(data)

	QVariantMap::const_iterator it;
	it = result.find(QString::fromUtf8("ResponseCode"));
	if (it == result.end()) {
		return WTFStatus_UnexpectedError;
	}

	bool ok;
	int code = it.value().toInt(&ok);
	if (!ok) {
		return WTFStatus_UnexpectedError;
	}

	if (code == 0) {
		it = result.find(QString::fromUtf8("NewDeviceID"));
		if (it == result.end()) {
			return WTFStatus_UnexpectedError;
		}

		DBGLOG(LOG_DEBUG, 8, QString::fromUtf8("device id: [%1]").arg(it.value().toString()));
		setProperty("varDeviceID", it.value().toString());
	}

	return WTFStatus_NoError;
}

SetDNSMasqDeviceIDOp::SetDNSMasqDeviceIDOp(const QString& macAddress, const QString& deviceId, const QString& url, QNetworkAccessManager *nam, QObject *parent)
: BaseOp(url, nam, parent)
{
	m_namespace = NS_PARENTAL_CONTROL;
	m_command = CMD_SET_DNS_MASQ_DEVICE_ID;
	m_paramNames << QString::fromUtf8("NewMACAddress") << QString::fromUtf8("NewDeviceID");
	m_paramValues << macAddress << deviceId;
}

int SetDNSMasqDeviceIDOp::processImpl(const QByteArray& data, const QVariantMap& result)
{
	Q_UNUSED(data)

	QVariantMap::const_iterator it;
	it = result.find(QString::fromUtf8("ResponseCode"));
	if (it == result.end()) {
		return WTFStatus_UnexpectedError;
	}

	bool ok;
	int code = it.value().toInt(&ok);
	if (!ok) {
		return WTFStatus_UnexpectedError;
	}

	if (code != 0) {
		setProperty("varResponseCode", code);
		return WTFStatus_Failed;
	}

	return WTFStatus_NoError;
}

GetModelNameOp::GetModelNameOp(const QString& url, QNetworkAccessManager *nam, QObject *parent)
: BaseOp(url, nam, parent)
{
	m_namespace = NS_DEVICE_INFO;
	m_command = CMD_GET_DEVICE_INFO;
	m_commandResponse = "GetInfoResponse";
}

int GetModelNameOp::processImpl(const QByteArray& data, const QVariantMap& result)
{
	Q_UNUSED(data)

	QVariantMap::const_iterator it;
	it = result.find(QString::fromUtf8("ResponseCode"));
	if (it == result.end()) {
		return WTFStatus_UnexpectedError;
	}

	bool ok;
	int code = it.value().toInt(&ok);
	if (!ok) {
		return WTFStatus_UnexpectedError;
	}

	if (code != 0) {
		setProperty("varResponseCode", code);
		return WTFStatus_Failed;
	}

	it = result.find(QString::fromUtf8("ModelName"));
	if (it == result.end()) {
		return WTFStatus_UnexpectedError;
	}

	setProperty("varModelName", it.value().toString());

	return WTFStatus_NoError;
}

GetParentalControlEnableStatusOp::GetParentalControlEnableStatusOp(const QString& url, QNetworkAccessManager *nam, QObject *parent)
: BaseOp(url, nam, parent)
{
	m_namespace = NS_PARENTAL_CONTROL;
	m_command = CMD_GET_ENABLED_STATUS;
}

int GetParentalControlEnableStatusOp::processImpl(const QByteArray& data, const QVariantMap& result)
{
	Q_UNUSED(data)

	QVariantMap::const_iterator it;
	it = result.find(QString::fromUtf8("ResponseCode"));
	if (it == result.end()) {
		return WTFStatus_UnexpectedError;
	}

	bool ok;
	int code = it.value().toInt(&ok);
	if (!ok) {
		return WTFStatus_UnexpectedError;
	}

	if (code != 0) {
		setProperty("varResponseCode", code);
		return WTFStatus_Failed;
	}

	it = result.find(QString::fromUtf8("ParentalControl"));
	if (it == result.end()) {
		return WTFStatus_UnexpectedError;
	}

	setProperty("varParentalControlStatus", it.value().toBool());

	return WTFStatus_NoError;
}

SetParentalControlEnableStatusOp::SetParentalControlEnableStatusOp(bool enable, const QString& url, QNetworkAccessManager *nam, QObject *parent)
: BaseOp(url, nam, parent)
{
	m_namespace = NS_PARENTAL_CONTROL;
	m_command = CMD_ENABLE_PARENTAL_CONTROL;
	m_paramNames << QString::fromUtf8("NewEnable");
	m_paramValues << QString::fromUtf8(enable ? "1" : "0");
}

int SetParentalControlEnableStatusOp::processImpl(const QByteArray& data, const QVariantMap& result)
{
	Q_UNUSED(data)

	QVariantMap::const_iterator it;
	it = result.find(QString::fromUtf8("ResponseCode"));
	if (it == result.end()) {
		return WTFStatus_UnexpectedError;
	}

	bool ok;
	int code = it.value().toInt(&ok);
	if (!ok) {
		return WTFStatus_UnexpectedError;
	}

	if (code != 0) {
		setProperty("varResponseCode", code);
		return WTFStatus_Failed;
	}

	return WTFStatus_NoError;
}

GetWanOp::GetWanOp(const QString& url, QNetworkAccessManager *nam, QObject *parent)
: BaseOp(url, nam, parent)
{
	m_namespace = NS_WANIP_CONNECTION;
	m_command = CMD_GET_WANIP;
	m_commandResponse = "GetInfoResponse";
}

int GetWanOp::processImpl(const QByteArray& data, const QVariantMap& result)
{
	Q_UNUSED(data)

	QVariantMap::const_iterator it;
	it = result.find(QString::fromUtf8("ResponseCode"));
	if (it == result.end()) {
		return WTFStatus_UnexpectedError;
	}

	bool ok;
	int code = it.value().toInt(&ok);
	if (!ok) {
		return WTFStatus_UnexpectedError;
	}

	if (code != 0) {
		setProperty("varResponseCode", code);
		return WTFStatus_Failed;
	}

	it = result.find(QString::fromUtf8("NewMACAddress"));
	if (it == result.end()) {
		return WTFStatus_UnexpectedError;
	}

	setProperty("varMACAddress", it.value().toString());

	return WTFStatus_NoError;
}

AuthenticateOp::AuthenticateOp(const QString& username, const QString& password, const QString& url, QNetworkAccessManager *nam, QObject *parent)
: BaseOp(url, nam, parent)
{
	m_namespace = NS_PARENTAL_CONTROL;
	m_command = CMD_AUTHENTICATE;
	m_paramNames << QString::fromUtf8("NewUsername") << QString::fromUtf8("NewPassword");
	m_paramValues << username << password;
}

int AuthenticateOp::processImpl(const QByteArray& data, const QVariantMap& result)
{
	Q_UNUSED(data)

	QVariantMap::const_iterator it;
	// TODO:
	it = result.find(QString::fromUtf8("AuthenticateResponseCode"));
	if (it == result.end()) {
		it = result.find(QString::fromUtf8("ResponseCode"));
		if (it == result.end()) {
			return WTFStatus_UnexpectedError;
		}
	}

	bool ok;
	int code = it.value().toInt(&ok);
	if (!ok) {
		return WTFStatus_UnexpectedError;
	}

	if (code == 401) {
		return WTFStatus_AuthenticationFailed;
	} else if (code != 0) {
		setProperty("varResponseCode", code);
		return WTFStatus_Failed;
	}

	return WTFStatus_NoError;
}

GetCurrentSettingsOp::GetCurrentSettingsOp(QNetworkAccessManager *nam, QObject *parent)
: SimpleOp(parent), m_nam(nam)
{
	internalStart();
}

GetCurrentSettingsOp::~GetCurrentSettingsOp()
{
	if (m_reply) {
		m_reply->abort();
		m_reply->deleteLater();
		m_reply = NULL;
	}
}

void GetCurrentSettingsOp::startImpl()
{
	if (isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	QNetworkRequest req(CURRENT_SETTINGS);
	m_reply = m_nam->get(req);
	if (!m_reply) {
		DBGLOG(LOG_ERROR, 4, QString::fromUtf8("QNetworkAccessManager::get failed, unexpected!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_reply, SIGNAL(finished()), SLOT(requestFinished()));

	getRidOfSslErrors(m_reply);
}

void GetCurrentSettingsOp::abortImpl()
{
	if (m_reply) {
		m_reply->abort();
		m_reply->deleteLater();
		m_reply = NULL;
	}
}

void GetCurrentSettingsOp::requestFinished()
{
	if (isAborted() || m_reply == NULL) {
		DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	QNetworkReply *reply = m_reply;
	m_reply->deleteLater();
	m_reply = NULL;

	if (reply->error() != QNetworkReply::NoError) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("network error: %1").arg(reply->error()));
		return report(WTFStatus_NetworkError);
	}

	QByteArray response = reply->readAll();

	QString firmware;
	QString supportLPC;
	QString model;

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
		return report(WTFStatus_UnexpectedError);
	}

	setProperty("varFirmware", firmware);
	setProperty("varModel", model);
	setProperty("varSupportLPC", supportLPC);
	return report(WTFStatus_NoError);
}

} // namespace RouterApiImpl
