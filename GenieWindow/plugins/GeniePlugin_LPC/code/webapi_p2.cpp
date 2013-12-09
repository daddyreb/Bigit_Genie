#include "common.h"
#include "logger.h"
#include "webapi_p2.h"
#include <QStringList>
#include <QMap>
#include <QtCore/QXmlStreamReader>
#include <QtCore/QDebug>

void getRidOfSslErrors(QNetworkReply *reply);

extern const char *API_KEY;
extern QString SERIALNUM;
namespace WebApiImpl {

BaseOp::BaseOp(WebApi *owner)
: SimpleOp(NULL), m_stage(Stage_Constructed), m_owner(owner), m_reply(NULL)
{
	internalStart();
}

BaseOp::~BaseOp()
{
	if (m_reply) {
		m_reply->deleteLater();
		m_reply = NULL;
	}
}

void BaseOp::startImpl()
{
	if (isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	m_reply = webApiPost(m_owner->networkAccessManager(), m_command, m_paramNames, m_paramValues);
	if (!m_reply) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("webApiPost failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	getRidOfSslErrors(m_reply);

	connect(m_reply, SIGNAL(finished()), SLOT(stagePostFinished()));
	m_stage = Stage_Post;
}

void BaseOp::abortImpl()
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

void BaseOp::stagePostFinished()
{
	if (isAborted() || m_reply == NULL) {
		DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Aborted at 'post' stage"));
		return report(WTFStatus_Aborted);
	}

	if (m_reply->error() != QNetworkReply::NoError) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("Network error, code=%1").arg(m_reply->error()));
		setProperty("varNetworkError", m_reply->error());
		m_reply->deleteLater();
		m_reply = NULL;
		return report(WTFStatus_NetworkError);
	}

	QByteArray responseData = m_reply->readAll();
	m_reply->deleteLater();
	m_reply = NULL;

	DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("response data: %1").arg(QString::fromUtf8(responseData)));
        qDebug()<<"***BaseOp::stagePostFinished():response data:"<<responseData;
	QVariantMap result;
	if (!parseJsonLite(responseData, result)) {
		report(WTFStatus_UnexpectedError);
	}

	report(processImpl(result));
}

CheckNameAvailableOp::CheckNameAvailableOp(const QString& username, WebApi *owner)
: BaseOp(owner)
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

CreateAccountOp::CreateAccountOp(const QString& username, const QString& password, const QString& email, WebApi *owner)
: BaseOp(owner)
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

LoginOp::LoginOp(const QString& username, const QString& password, WebApi *owner)
: BaseOp(owner)
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

GetLabelOp::GetLabelOp(const QString& token, const QString& deviceId, WebApi *owner)
: BaseOp(owner)
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

GetDeviceOp::GetDeviceOp(const QString& token, const QString& deviceKey, WebApi *owner)
: BaseOp(owner)
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

CreateDeviceOp::CreateDeviceOp(const QString& token, const QString& deviceKey, WebApi *owner)
: BaseOp(owner)
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

GetFiltersOp::GetFiltersOp(const QString& token, const QString& deviceId, WebApi *owner)
: BaseOp(owner)
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
			it = result.find(QString::fromUtf8("response.categories"));
			if(it != result.end())
			{
				setProperty("varBundle", it.value());
			}
			
		} else {
			setProperty("varBundle", it.value());
                }
		qDebug()<<"***GetFiltersOp::processImpl()**varBundle:"<<property("varBundle");
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
//		case 4003:
//			// seems important!!!
//			return WebApiStatus_DeviceIdNotMine;
		default:
			DBGLOG(LOG_ERROR, 2, QString::fromUtf8("GetFiltersOp::processImpl error code %1").arg(code));
			return WTFStatus_Failed;
		}
	} else {
		return WTFStatus_UnexpectedError;
	}

	return WTFStatus_NoError;
}

SetFiltersOp::SetFiltersOp(const QString& token, const QString& deviceId, const QStringList& bundle, WebApi *owner)
: BaseOp(owner)
{
	m_command = "filters_set";
	m_paramNames << QString::fromUtf8("token") << QString::fromUtf8("device_id");
	QString bundleType = bundle[0];
	if(bundleType == "bundle")
	{
		m_paramNames<< QString::fromUtf8("bundle");
	}else
	{
		m_paramNames<< QString::fromUtf8("categories");
	}
	m_paramValues << token << deviceId << bundle[1];
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

AccountRelayOp::AccountRelayOp(const QString& token, WebApi *owner)
: BaseOp(owner)
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
        qDebug()<<"***AccountRelayOp::processImpl()***"<<it.value().toString()<<" API_KEY:"<<API_KEY;
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


GetUsersForDeviceIdOp::GetUsersForDeviceIdOp(const QString& deviceId, WebApi *owner)
: BaseOp(owner)
{
	m_command = "device_children_get";
	m_paramNames << QString::fromUtf8("parent_device_id");
	m_paramValues << deviceId;
}

int GetUsersForDeviceIdOp::processImpl(const QVariantMap& result)
{
	QVariantMap::const_iterator it;
	it = result.find(QString::fromUtf8("status"));
	if (it == result.end()) {
		return WTFStatus_UnexpectedError;
	}

	QByteArray statusCode = it.value().toString().toUtf8().toLower();
	if (statusCode == "success") {
		QVariantMap::const_iterator it = result.find("response");
		if (it == result.end() || it.value().type() != QVariant::StringList) {
			return WTFStatus_UnexpectedError;
		}
		setProperty("varList", it.value().toStringList());
		return WTFStatus_NoError;
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

GetDeviceChildOp::GetDeviceChildOp(const QString& parentDeviceId, const QString& username, const QString& password, WebApi *owner)
	: BaseOp(owner)
{
	m_command = "device_child_get";
	m_paramNames << QString::fromUtf8("parent_device_id") << QString::fromUtf8("device_username") << QString::fromUtf8("device_password");
	m_paramValues << parentDeviceId << username << password;
}

int GetDeviceChildOp::processImpl(const QVariantMap& result)
{
	QVariantMap::const_iterator it;
	it = result.find(QString::fromUtf8("status"));
	if (it == result.end()) {
		return WTFStatus_UnexpectedError;
	}

	QByteArray statusCode = it.value().toString().toUtf8().toLower();
	if (statusCode == "success") {
		QVariantMap::const_iterator it = result.find("response");
		if (it == result.end() || it.value().type() != QVariant::String) {
			return WTFStatus_UnexpectedError;
		}
		setProperty("varChildDeviceId", it.value().toString());
		return WTFStatus_NoError;
	} else if (statusCode == "failure") {
		it = result.find(QString::fromUtf8("error"));
		if (it == result.end()) {
			return WTFStatus_UnexpectedError;
		}
		int code = it.value().toInt();
		setProperty("varErrorCode", code);
		switch (code) {
		case 3003:
			return WTFStatus_AuthenticationFailed;
		default:
			return WTFStatus_Failed;
		}
	} else {
		return WTFStatus_UnexpectedError;
	}

	return WTFStatus_NoError;
}

GetUserForChildDeviceIdOp::GetUserForChildDeviceIdOp(const QString& childDeviceId, WebApi *owner)
: BaseOp(owner)
{
	m_command = "device_child_username_get";
	m_paramNames << QString::fromUtf8("device_id");
	m_paramValues << childDeviceId;
}

int GetUserForChildDeviceIdOp::processImpl(const QVariantMap& result)
{
	QVariantMap::const_iterator it;
	it = result.find(QString::fromUtf8("status"));
	if (it == result.end()) {
		return WTFStatus_UnexpectedError;
	}

	QByteArray statusCode = it.value().toString().toUtf8().toLower();
        qDebug()<<"**GetUserForChildDeviceIdOp::processImpl()**"<<statusCode;
	if (statusCode == "success") {
		QVariantMap::const_iterator it = result.find("response");
		if (it == result.end() || it.value().type() != QVariant::String) {
			return WTFStatus_UnexpectedError;
		}
		setProperty("varUserName", it.value().toString());
		return WTFStatus_NoError;
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

static const QString MAGIC_VALUE = QString::fromUtf8("E6A88AE69687E58D9A88");
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
static const QByteArray CMD_GET_DEVICE_INFO = "GetInfo";
static const QByteArray CMD_GET_WANIP = "GetInfo";
static const QByteArray CMD_AUTHENTICATE = "Authenticate";
static const QByteArray CMD_ENABLE_PARENTAL_CONTROL = "EnableParentalControl";

#if 0

BaseOp::BaseOp(RouterApi *owner)
: SimpleOp(NULL), m_stage(Stage_Constructed), m_owner(owner), m_reply(NULL)
{
	m_portList = owner->portList();
	internalStart();
}

BaseOp::~BaseOp()
{
	if (m_reply) {
		m_reply->deleteLater();
		m_reply = NULL;
	}
}

void BaseOp::startImpl()
{
	if (isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	QString url;
	m_activePort = m_portList.front();
	m_portList.pop_front();
	if (m_activePort == 80) {
		url = QLatin1String("http://routerlogin.net/soap/server_sa/");
	} else {
		url = QString::fromUtf8("http://routerlogin.net:%1/soap/server_sa/").arg(m_activePort);
	}

	m_reply = routerApiPost(m_owner->networkAccessManager(), url, m_namespace, m_command, m_paramNames, m_paramValues);
	if (!m_reply) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("routerApiPost failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	getRidOfSslErrors(m_reply);

	connect(m_reply, SIGNAL(finished()), SLOT(stagePostFinished()));
	m_stage = Stage_Post;
}

void BaseOp::abortImpl()
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

void BaseOp::stagePostFinished()
{
	if (isAborted() || m_reply == NULL) {
		DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Aborted at 'post' stage"));
		return report(WTFStatus_Aborted);
	}

	if (m_reply->error() != QNetworkReply::NoError) {
		if (m_portList.isEmpty()) {
			DBGLOG(LOG_ERROR, 2, QString::fromUtf8("Network error, code=%1").arg(m_reply->error()));
			setProperty("varNetworkError", m_reply->error());
			m_reply->deleteLater();
			m_reply = NULL;
			return report(WTFStatus_NetworkError);
		} else {
			return startImpl();
		}
	} else {
		m_owner->raisePort(m_activePort);
	}

	QByteArray responseData = m_reply->readAll();
	m_reply->deleteLater();
	m_reply = NULL;

	DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("response data: %1").arg(QString::fromUtf8(responseData)));

	QVariantMap result;
	if (!parseResultXml(responseData, result)) {
		DBGLOG(LOG_DEBUG, 2, QString::fromUtf8("parseResultXml failed!"));
		result.clear();
	}

	report(processImpl(responseData, result));
}

bool BaseOp::parseResultXml(const QByteArray& data, QVariantMap& result)
{
	QXmlStreamReader xml(data);

	static const QString TAG_RESPONSE_CODE = QString::fromUtf8("ResponseCode");

	QString TAG_NS;
	if (m_commandResponse.isEmpty()) {
		TAG_NS = QString::fromUtf8(m_command) + QString::fromUtf8("Response");
	} else {
		TAG_NS = QString::fromUtf8(m_commandResponse);
	}

	int level = 0;
	QString str;

	while (!xml.atEnd()) {
		QXmlStreamReader::TokenType token = xml.readNext();
		switch (token) {
		case QXmlStreamReader::StartElement:
			++level;
			if (level == 3) {
				if (xml.name() == TAG_NS) {
				} else if (xml.name() == TAG_RESPONSE_CODE) {
				} else {
					DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("WTF start tag [%1]").arg(xml.name().toString()));
				}
			} else if (level > 3) {
			}
			break;
		case QXmlStreamReader::EndElement:
			if (level == 3) {
				if (xml.name() == TAG_NS) {
				} else if (xml.name() == TAG_RESPONSE_CODE) {
					DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("ResponseCode: %1").arg(str));
					result.insert(QString::fromUtf8("ResponseCode"), str);
				} else {
					DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("WTF end tag [%1]").arg(xml.name().toString()));
				}
			} else if (level > 3) {
				result.insert(xml.name().toString(), str);
			}
			--level;
			break;
		case QXmlStreamReader::Characters:
			str = xml.text().toString();
			break;
		default:
			break;
		}
	}

	if (xml.hasError()) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("xml parsing failed!!, raw data: %1").arg(QString::fromUtf8(data)));
		return false;
	}

	return true;
}

#else

QString parseXmlNode(const QByteArray& content, QMap<QString, QString>& attrMap)
{
	QString tagName;
	QXmlStreamReader reader(content);
	int level = 0;
	while (!reader.atEnd()) {
		switch (reader.readNext()) {
		case QXmlStreamReader::StartElement:
			++level;
			if (level == 1) {
				tagName = reader.name().toString();
				QXmlStreamAttributes attrList = reader.attributes();
				for (int i = 0; i < attrList.count(); i++) {
					const QXmlStreamAttribute& attr = attrList.at(i);
					attrMap.insert(attr.name().toString(), attr.value().toString());
				}
			}
			break;
		case QXmlStreamReader::EndElement:
			--level;
			break;
		}
	}
	if (reader.hasError()) {
		return QString();
	}
	return tagName;
}

AsyncOp *HttpGetPostOp::startGet(QNetworkAccessManager *nam, const QString& url)
{
	HttpGetPostOp *op = new HttpGetPostOp(nam, url);
	op->start();
	return op;
}

AsyncOp *HttpGetPostOp::startPost(QNetworkAccessManager *nam, const QString& url, const QByteArray& data)
{
	HttpGetPostOp *op = new HttpGetPostOp(nam, url, data);
	op->start();
	return op;
}

HttpGetPostOp::HttpGetPostOp(QNetworkAccessManager *nam, const QString& url)
	: AsyncOp(NULL), m_nam(nam), m_url(url), m_post(false), m_reply(NULL)
{
}

HttpGetPostOp::HttpGetPostOp(QNetworkAccessManager *nam, const QString& url, const QByteArray& data)
	: AsyncOp(NULL), m_nam(nam), m_url(url), m_data(data), m_post(true), m_reply(NULL)
{
}

void HttpGetPostOp::start()
{
	QNetworkRequest req(m_url);
	if (m_post) {
		m_reply = m_nam->post(req, m_data);
	} else {
		m_reply = m_nam->get(req);
	}
	m_reply->ignoreSslErrors();
	connect(m_reply, SIGNAL(finished()), SLOT(onReplyFinished()));
}

HttpGetPostOp::~HttpGetPostOp()
{
	if (m_reply) {
		m_reply->deleteLater();
		m_reply = NULL;
	}
}

void HttpGetPostOp::abortImpl()
{
	if (m_reply) {
		m_reply->abort();
		m_reply->deleteLater();
		m_reply = NULL;
	}
}

void HttpGetPostOp::onReplyFinished()
{
	if (isAborted() || m_reply == NULL) {
		DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Aborted at 'send' stage"));
		return report(WTFStatus_Aborted);
	}

	QNetworkReply *reply = m_reply;
	m_reply->deleteLater();
	m_reply = NULL;

	if (reply->error() != QNetworkReply::NoError) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("Network error, code=%1").arg(reply->error()));
		setProperty("varNetworkError", reply->error());
		return report(WTFStatus_NetworkError);
	}

	int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	switch (statusCode) {
	case 200:
		setProperty("varFinalURL", m_url);
                setProperty("varData", reply->readAll());
                qDebug()<<"***HttpGetPostOp::onReplyFinished():"<<reply->readAll();//yankai add
		setProperty("varStatusCode", statusCode);
		return report(WTFStatus_NoError);
	case 307:
	case 302:
		m_url = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toString();
		start();
		return;
	default:
		setProperty("varStatusCode", statusCode);
		return report(WTFStatus_Failed);
	}
}

AsyncOp *InitAuthOp::start(QNetworkAccessManager *nam, const QString& baseUrl, const QString& username, const QString& password)
{
	InitAuthOp *op = new InitAuthOp(nam, baseUrl, username, password);
	op->startOp();
	return op;
}

InitAuthOp::InitAuthOp(QNetworkAccessManager *nam, const QString& baseUrl, const QString& username, const QString& password)
	: AsyncOp(NULL), m_nam(nam), m_baseUrl(baseUrl), m_op(NULL)
{
	m_xml.append(QString::fromUtf8("<authenticate type=\"basic\" username=\"%1\" password=\"%2\"/>").arg(username).arg(password));
}

InitAuthOp::~InitAuthOp()
{
	if (m_op) {
		m_op->deleteLater();
		m_op = NULL;
	}
}

void InitAuthOp::startOp()
{
	QString url = QString::fromUtf8("%1/fcp/authenticate").arg(m_baseUrl);
	qDebug() << m_xml;
	qDebug() << url;
	m_op = HttpGetPostOp::startPost(m_nam, url, m_xml.toUtf8());
	connect(m_op, SIGNAL(finished()), SLOT(onAuthOpFinished()));
}

void InitAuthOp::abortImpl()
{
	if (m_op) {
		m_op->abort();
		m_op->deleteLater();
		m_op = NULL;
	}
}

void InitAuthOp::onAuthOpFinished()
{
	if (isAborted() || m_op == NULL) {
		DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Aborted at 'auth' stage"));
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	int err = op->result().toInt();
	if (err != WTFStatus_NoError) {
		return report(err);
	}

	QByteArray data = op->property("varData").toByteArray();
	qDebug() << data;

	QMap<QString,QString> attrMap;
	QString tagName = parseXmlNode(data, attrMap);
	if (tagName != QLatin1String("authenticate")) {
		DBGLOG(LOG_DEBUG, 2, QString::fromUtf8("unexpected tag '%1'").arg(tagName));
		return report(WTFStatus_UnexpectedError);
	}

	QMap<QString,QString>::const_iterator it = attrMap.find(QLatin1String("authenticated"));
	if (it == attrMap.end()) {
		DBGLOG(LOG_DEBUG, 2, QString::fromUtf8("'authenticated' not found!!"));
		return report(WTFStatus_UnexpectedError);
	}

	if (it.value().compare(QLatin1String("true"), Qt::CaseInsensitive) != 0) {
		return report(WTFStatus_SmartNetworkAuthFailed);
	}

	QString url = QString::fromUtf8("%1/fcp/init").arg(m_baseUrl);
	m_op = HttpGetPostOp::startPost(m_nam, url, "<init type=\"ui\" fcmb=\"true\"/>");
	connect(m_op, SIGNAL(finished()), SLOT(onInitOpFinished()));
}

void InitAuthOp::onInitOpFinished()
{
	if (isAborted() || m_op == NULL) {
		DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Aborted at 'init' stage"));
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	int err = op->result().toInt();
	if (err != WTFStatus_NoError) {
		return report(err);
	}

	QByteArray data = op->property("varData").toByteArray();
	qDebug() << data;

	QMap<QString,QString> attrMap;
	QString tagName = parseXmlNode(data, attrMap);
	if (tagName != QLatin1String("init")) {
		DBGLOG(LOG_DEBUG, 2, QString::fromUtf8("unexpected tag '%1'").arg(tagName));
		return report(WTFStatus_UnexpectedError);
	}

	QMap<QString,QString>::const_iterator it = attrMap.find(QLatin1String("domain"));
	if (it == attrMap.end()) {
		DBGLOG(LOG_DEBUG, 2, QString::fromUtf8("'domain' not found!!"));
		return report(WTFStatus_UnexpectedError);
	}

	QString domain = it.value();

	it = attrMap.find(QLatin1String("name"));
	if (it == attrMap.end()) {
		DBGLOG(LOG_DEBUG, 2, QString::fromUtf8("'name' not found!!"));
		return report(WTFStatus_UnexpectedError);
	}

	QString name = it.value();

	setProperty("varName", name);
	setProperty("varDomain", domain);
	return report(WTFStatus_NoError);
}

AsyncOp *FcmlOp::start(QNetworkAccessManager *nam, const QString& baseUrl, const QString& toA, const QString& toB, const QString& fromA, const QString& fromB, const QString& object, const QString& method, const QStringList& names, const QStringList& values)
{
	FcmlOp *op = new FcmlOp(nam, baseUrl, toA, toB, fromA, fromB, object, method, names, values);
	op->startOp();
	return op;
}

FcmlOp::FcmlOp(QNetworkAccessManager *nam, const QString& baseUrl, const QString& toA, const QString& toB, const QString& fromA, const QString& fromB, const QString& object, const QString& method, const QStringList& names, const QStringList& values)
	: AsyncOp(NULL), m_nam(nam), m_baseUrl(baseUrl), m_fromA(fromA), m_op(NULL)
{
	m_methodTagName = QString::fromUtf8("%1.%2.%3.%4").arg(toB).arg(toA).arg(object).arg(method);
	m_fcml.append(QString::fromUtf8("<fcml to=\"%1@%2\" from=\"%3@%4\">").arg(toA).arg(toB).arg(fromA).arg(fromB));
	m_fcml.append(QString::fromUtf8("<%1").arg(m_methodTagName));
	for (int i = 0; i < names.count(); i++) {
		m_fcml.append(QString::fromUtf8(" %1=\"%2\"").arg(names.at(i)).arg(values.at(i)));
	}
	m_fcml.append(QString::fromUtf8("/></fcml>"));
}

FcmlOp::~FcmlOp()
{
	if (m_op) {
		m_op->deleteLater();
		m_op = NULL;
	}
}

void FcmlOp::startOp()
{
	QString url = QString::fromUtf8("%1/fcp/send?n=%2").arg(m_baseUrl).arg(m_fromA);
	qDebug() << m_fcml;
	qDebug() << url;
	m_op = HttpGetPostOp::startPost(m_nam, url, m_fcml.toUtf8());
	connect(m_op, SIGNAL(finished()), SLOT(onOpFinished()));
}

void FcmlOp::abortImpl()
{
	if (m_op) {
		m_op->abort();
		m_op->deleteLater();
		m_op = NULL;
	}
}

void FcmlOp::onOpFinished()
{
	if (isAborted() || m_op == NULL) {
		DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Aborted at 'send' stage"));
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	int err = op->result().toInt();
	if (err != WTFStatus_NoError) {
		return report(err);
	}

	QByteArray data = op->property("varData").toByteArray();
	qDebug() << data;

	QMap<QString,QString> attrMap;
	QString tagName = parseXmlNode(data, attrMap);
	if (tagName.compare(QLatin1String("authenticate")) == 0) {
		return report(WTFStatus_AuthenticationFailed);
	} else {
		receiveNext();
	}
}

void FcmlOp::receiveNext()
{
	QString url = QString::fromUtf8("%1/fcp/receive?n=%2").arg(m_baseUrl).arg(m_fromA);
	qDebug() << url;
	m_op = HttpGetPostOp::startGet(m_nam, url);
	connect(m_op, SIGNAL(finished()), SLOT(onReceiveOpFinished()));
}

void FcmlOp::onReceiveOpFinished()
{
	if (isAborted() || m_op == NULL) {
		DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Aborted at 'receive' stage"));
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	int err = op->result().toInt();
	if (err != WTFStatus_NoError) {
		return report(err);
	}

	QByteArray data = op->property("varData").toByteArray();
	qDebug() << data;

	QXmlStreamReader xml(data);
	int level = 0;
	int fcmlLevel = 1;
	bool foundResponse = false;
	bool inErrorTag = false;
	bool errorFlag = false;
	QXmlStreamAttributes errAttrs0;
	QXmlStreamAttributes resultAttrs;
	while (!xml.atEnd()) {
		switch (xml.readNext()) {
		case QXmlStreamReader::StartElement:
			++level;
			if (level == 1) {
				if (xml.name().compare(QLatin1String("fcmb")) == 0) {
					++fcmlLevel;
				} else if (xml.name().compare(QLatin1String("fcml")) != 0) {
					DBGLOG(LOG_DEBUG, 2, QString::fromUtf8("unexpected tag '%1'!!").arg(xml.name().toString()));
					return report(WTFStatus_UnexpectedError);
				}
			}

			if (level == fcmlLevel) {
				if (xml.name().compare(QLatin1String("fcml")) == 0) {
				} else {
					DBGLOG(LOG_DEBUG, 2, QString::fromUtf8("unexpected tag '%1'!!").arg(xml.name().toString()));
					return report(WTFStatus_UnexpectedError);
				}
			} else if (level == fcmlLevel+1) {
				if (xml.name().compare(QLatin1String("error")) == 0) {
					inErrorTag = true;
					errAttrs0 = xml.attributes();
				} else if (xml.name().compare(m_methodTagName) == 0) {
					resultAttrs = xml.attributes();
					foundResponse = true;
				}
			}

			if (level == fcmlLevel+2) {
				if (inErrorTag) {
					if (xml.name().compare(m_methodTagName) == 0) {
						resultAttrs = errAttrs0;
						foundResponse = true;
						errorFlag = true;
					}
				}
			}

			break;
		case QXmlStreamReader::EndElement:
			if (level == fcmlLevel+1) {
				inErrorTag = false;
			}
			--level;
			break;
		}
	}

	if (xml.hasError()) {
		return report(WTFStatus_UnexpectedError);
	}

	if (!foundResponse) {
		receiveNext();
		return;
	}

	if (!errorFlag) {
		for (int i = 0; i < resultAttrs.count(); i++) {
			const QXmlStreamAttribute& attr = resultAttrs.at(i);
			setProperty(attr.name().toString().toUtf8(), attr.value().toString());
		}
		return report(WTFStatus_NoError);
	} else {
		for (int i = 0; i < resultAttrs.count(); i++) {
			const QXmlStreamAttribute& attr = resultAttrs.at(i);
			setProperty(attr.name().toString().toUtf8(), attr.value().toString());
		}
		return report(WTFStatus_Failed);
	}
}

BaseOp::BaseOp(RouterApi *owner)
: SimpleOp(NULL), m_owner(owner), m_op(NULL), m_reply(NULL)
{
	m_fcmlMode = m_owner->m_fcmlMode;
	m_portList = owner->portList();
	internalStart();
}

BaseOp::~BaseOp()
{
	if (m_op) {
		m_op->deleteLater();
		m_op = NULL;
	}
	if (m_reply) {
		m_reply->deleteLater();
		m_reply = NULL;
	}
}

void BaseOp::startImpl()
{
	if (isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	if (m_fcmlMode) {
		startFcml();
	} else {
		startSoap();
	}
}

void BaseOp::abortImpl()
{
	if (m_op) {
		m_op->abort();
		m_op->deleteLater();
		m_op = NULL;
	}
	if (m_reply) {
		m_reply->abort();
		m_reply->deleteLater();
		m_reply = NULL;
	}
}

void BaseOp::startSoap()
{
	QString url;
	m_activePort = m_portList.front();
	m_portList.pop_front();
	if (m_activePort == 80) {
		url = QLatin1String("http://routerlogin.net/soap/server_sa/");
	} else {
		url = QString::fromUtf8("http://routerlogin.net:%1/soap/server_sa/").arg(m_activePort);
	}

	m_reply = routerApiPost(m_owner->networkAccessManager(), url, m_namespace, m_command, m_paramNames, m_paramValues);
	if (!m_reply) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("routerApiPost failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	getRidOfSslErrors(m_reply);

	connect(m_reply, SIGNAL(finished()), SLOT(onSoapFinished()));
}

void BaseOp::onSoapFinished()
{
	if (isAborted() || m_reply == NULL) {
		DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Aborted at 'post' stage"));
		return report(WTFStatus_Aborted);
	}

	QNetworkReply *reply = m_reply;
	m_reply->deleteLater();
	m_reply = NULL;

	if (reply->error() != QNetworkReply::NoError) {
		if (m_portList.isEmpty()) {
			DBGLOG(LOG_ERROR, 2, QString::fromUtf8("Network error, code=%1").arg(reply->error()));
			setProperty("varNetworkError", reply->error());
			return report(WTFStatus_NetworkError);
		} else {
			return startImpl();
		}
	} else {
		m_owner->raisePort(m_activePort);
	}

	QByteArray responseData = reply->readAll();

	DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("response data: %1").arg(QString::fromUtf8(responseData)));
        qDebug()<<"***BaseOp::onSoapFinished():response data:"<<responseData;
	QVariantMap result;
	if (!parseResultXml(responseData, result)) {
		DBGLOG(LOG_DEBUG, 2, QString::fromUtf8("parseResultXml failed!"));
		result.clear();
	}

        report(processImpl(responseData, result));
}

bool BaseOp::parseResultXml(const QByteArray& data, QVariantMap& result)
{
	QXmlStreamReader xml(data);

	static const QString TAG_RESPONSE_CODE = QString::fromUtf8("ResponseCode");

	QString TAG_NS;
	if (m_commandResponse.isEmpty()) {
		TAG_NS = QString::fromUtf8(m_command) + QString::fromUtf8("Response");
	} else {
		TAG_NS = QString::fromUtf8(m_commandResponse);
	}

	int level = 0;
	QString str;

	while (!xml.atEnd()) {
		QXmlStreamReader::TokenType token = xml.readNext();
		switch (token) {
		case QXmlStreamReader::StartElement:
			++level;
			if (level == 3) {
				if (xml.name() == TAG_NS) {
				} else if (xml.name() == TAG_RESPONSE_CODE) {
				} else {
					DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("WTF start tag [%1]").arg(xml.name().toString()));
				}
			} else if (level > 3) {
			}
			break;
		case QXmlStreamReader::EndElement:
			if (level == 3) {
				if (xml.name() == TAG_NS) {
				} else if (xml.name() == TAG_RESPONSE_CODE) {
					DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("ResponseCode: %1").arg(str));
					result.insert(QString::fromUtf8("ResponseCode"), str);
				} else {
					DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("WTF end tag [%1]").arg(xml.name().toString()));
				}
			} else if (level > 3) {
				result.insert(xml.name().toString(), str);
			}
			--level;
			break;
		case QXmlStreamReader::Characters:
			str = xml.text().toString();
			break;
		default:
			break;
		}
	}

	if (xml.hasError()) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("xml parsing failed!!, raw data: %1").arg(QString::fromUtf8(data)));
		return false;
	}

	return true;
}

void BaseOp::startFcml()
{
	QString objectName = QString::fromUtf8(m_namespace.split(':').at(3));
	QStringList paramNames(m_paramNames);
	QStringList paramValues(m_paramValues);
	paramNames.append("_sessionId");
	paramValues.append(m_owner->m_sessionId);
	m_op = FcmlOp::start(m_owner->networkAccessManager(), m_owner->m_smartNetworkBaseUrl, QLatin1String("netrouter"), m_owner->m_cpid, m_owner->m_uiid, m_owner->m_domain, objectName, QString::fromUtf8(m_command), paramNames, paramValues);
	connect(m_op, SIGNAL(finished()), SLOT(onFcmlFinished()));
}

void BaseOp::onFcmlFinished()
{
	if (isAborted() || m_op == NULL) {
		DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Aborted at 'send' stage"));
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	int err = op->result().toInt();
	if (err == WTFStatus_AuthenticationFailed) {
		m_op = InitAuthOp::start(m_owner->networkAccessManager(), m_owner->m_smartNetworkBaseUrl, m_owner->m_smartNetworkUsername, m_owner->m_smartNetworkPassword);
		connect(m_op, SIGNAL(finished()), SLOT(onInitAuthFinished()));
	} else if (err == WTFStatus_Failed) {
		QVariant varErrorCode = op->property("code");
		if (varErrorCode.toString().compare(QLatin1String("16")) == 0) {
			QStringList paramNames;
			paramNames << QLatin1String("username") << QLatin1String("password");
			QStringList paramValues;
			paramValues << m_owner->m_routerUsername << m_owner->m_routerPassword;
			m_op = FcmlOp::start(m_owner->networkAccessManager(), m_owner->m_smartNetworkBaseUrl, QLatin1String("netrouter"), m_owner->m_cpid, m_owner->m_uiid, m_owner->m_domain, QLatin1String("SessionManagement"), QLatin1String("startSession"), paramNames, paramValues);
			connect(m_op, SIGNAL(finished()), SLOT(onStartSessionFinished()));
		} else {
			return report(err);
		}
	} else if (err != WTFStatus_NoError) {
		return report(err);
	} else {
		QVariantMap result;
		QList<QByteArray> nameList = op->dynamicPropertyNames();
		for (int i = 0; i < nameList.count(); i++) {
			QString key = QString::fromUtf8(nameList.at(i));
			if (key.compare(QLatin1String("_responseCode")) == 0) {
				key = QLatin1String("ResponseCode");
			}
			QString value = op->property(nameList.at(i)).toString();
			result.insert(key, value);
		}
		return report(processImpl("", result));
	}
}

void BaseOp::onStartSessionFinished()
{
	if (isAborted() || m_op == NULL) {
		DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Aborted at 'auth' stage"));
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	int err = op->result().toInt();
	if (err == WTFStatus_Failed) {
		QVariant varErrorCode = op->property("code");
		if (varErrorCode.toString().compare(QLatin1String("17")) == 0) {
			return report(WTFStatus_AuthenticationFailed);
		} else {
			return report(err);
		}
	} else if (err != WTFStatus_NoError) {
		return report(err);
	}

	m_owner->m_sessionId = op->property("sessionId").toString();
	startFcml();
}

void BaseOp::onInitAuthFinished()
{
	if (isAborted() || m_op == NULL) {
		DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Aborted at 'auth' stage"));
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	int err = op->result().toInt();
	if (err != WTFStatus_NoError) {
		return report(err);
	}

	m_owner->m_uiid = op->property("varName").toString();
	m_owner->m_domain = op->property("varDomain").toString();
	startFcml();
}

#endif

ConfigurationStartOp::ConfigurationStartOp(RouterApi *owner)
: BaseOp(owner)
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

ConfigurationFinishOp::ConfigurationFinishOp(RouterApi *owner)
: BaseOp( owner)
{
	m_namespace = NS_DEVICE_CONFIG;
	m_command = CMD_CONFIGURATION_FINISHED;
	m_paramNames << QString::fromUtf8("NewStatus");
	m_paramValues << QString::fromUtf8("ChangesApplied");
}

int ConfigurationFinishOp::processImpl(const QByteArray& data, const QVariantMap& result)
{
	Q_UNUSED(data)
	Q_UNUSED(result)
	return WTFStatus_NoError;
}

GetDNSMasqDeviceIDOp::GetDNSMasqDeviceIDOp(const QString& macAddress, RouterApi *owner)
: BaseOp(owner)
{
	m_namespace = NS_PARENTAL_CONTROL;
	m_command = CMD_GET_DNS_MASQ_DEVICE_ID;
	m_paramNames << QString::fromUtf8("NewMACAddress");
	m_paramValues << macAddress;
}

int GetDNSMasqDeviceIDOp::processImpl(const QByteArray& data, const QVariantMap& result)
{
	Q_UNUSED(data)
qDebug()<<"GetDNSMasqDeviceIDOp::processImpl():"<<result;
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
	if(401 == code)
	{
		return WebApiStatus_RouterAuthenticationFailed;
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

SetDNSMasqDeviceIDOp::SetDNSMasqDeviceIDOp(const QString& macAddress, const QString& deviceId, RouterApi *owner)
: BaseOp(owner)
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

GetModelNameOp::GetModelNameOp(RouterApi *owner)
: BaseOp(owner)
{
	m_namespace = NS_DEVICE_INFO;
	m_command = CMD_GET_DEVICE_INFO;
	m_commandResponse = "GetInfoResponse";
}

int GetModelNameOp::processImpl(const QByteArray& data, const QVariantMap& result)
{
	Q_UNUSED(data)
        qDebug()<<"GetModelNameOp\n";
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
        qDebug()<<"varModelName:\n"<<it.value().toString();
	setProperty("varModelName", it.value().toString());
        //---kai.yan add
        it = result.find(QString::fromUtf8("SerialNumber"));
        if (it == result.end()) {
                return WTFStatus_UnexpectedError;
        }
         SERIALNUM =it.value().toString();
        setProperty("varSerialNumber", it.value().toString());
        //----
	return WTFStatus_NoError;
}

GetParentalControlEnableStatusOp::GetParentalControlEnableStatusOp(RouterApi *owner)
: BaseOp(owner)
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

SetParentalControlEnableStatusOp::SetParentalControlEnableStatusOp(bool enable, RouterApi *owner)
: BaseOp(owner)
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

GetWanOp::GetWanOp(RouterApi *owner)
: BaseOp(owner)
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

AuthenticateOp::AuthenticateOp(const QString& username, const QString& password, RouterApi *owner)
: BaseOp(owner)
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
			setProperty("UURouterError", UURouterError_Unknown);
			return WTFStatus_UnexpectedError;
		}
	}

	bool ok;
	int code = it.value().toInt(&ok);
	if (!ok) {
		setProperty("UURouterError", UURouterError_Unknown);
		return WTFStatus_UnexpectedError;
	}

	if (code == 401) {
		setProperty("UURouterError", UURouterError_InvalidRouterPassword);
		return WTFStatus_AuthenticationFailed;
	} else if (code != 0) {
		setProperty("UURouterError", UURouterError_Unknown);
		setProperty("varResponseCode", code);
		return WTFStatus_Failed;
	}

	setProperty("UURouterError", UURouterError_NoError);
	return WTFStatus_NoError;
}

DeleteMacAddressOp::DeleteMacAddressOp(const QString& macAddress, RouterApi *owner)
	: BaseOp(owner)
{
	m_namespace = NS_PARENTAL_CONTROL;
	m_command = CMD_DELETE_MAC_ADDRESS;
	m_paramNames << QString::fromUtf8("NewMACAddress");
	m_paramValues << macAddress;
}

int DeleteMacAddressOp::processImpl(const QByteArray& data, const QVariantMap& result)
{
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
		if(code == 401)
		{
			return WebApiStatus_RouterAuthenticationFailed;
		}else
		{
			return WTFStatus_Failed;
		}
	}

	return WTFStatus_NoError;
}

GetCurrentSettingsOp::GetCurrentSettingsOp(RouterApi *owner)
: SimpleOp(NULL), m_owner(owner)
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
	m_reply = m_owner->networkAccessManager()->get(req);
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

	QByteArray response = reply->readAll();
	if (reply->error() != QNetworkReply::NoError) {
		if (!response.isEmpty()) {
		} else {
			QByteArray dd = reply->readAll();
			DBGLOG(LOG_ERROR, 2, QString::fromUtf8("network error: %1").arg(reply->error()));
			return report(WTFStatus_NetworkError);
		}
	}

	QString firmware;
	QString supportLPC;
	QString model;
        qDebug()<<"***GetCurrentSettingsOp::requestFinished(): response:"<<response;
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

	if (firmware.isEmpty() || model.isEmpty()) {
		return report(WTFStatus_UnexpectedError);
	}

	if (supportLPC.isEmpty()) {
		// walkaround for some buggy WNR3700
		if (response.indexOf("ParentalControlSupported=1") >= 0) {
            supportLPC = QString::fromLatin1("1");
		} else {
			return report(WTFStatus_UnexpectedError);
		}
	}

	setProperty("varFirmware", firmware);
	setProperty("varModel", model);
	setProperty("varSupportLPC", supportLPC);
	return report(WTFStatus_NoError);
}

} // namespace RouterApiImpl
