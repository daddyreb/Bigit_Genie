#include "common.h"
#include "coreapi.h"
#include "coreapi_p1.h"
#include "logger.h"
#include "webapi.h"
#include <QHostAddress>
#include <QNetworkRequest>
#include <QSslError>

void getRidOfSslErrors(QNetworkReply *reply);

namespace CoreApiImpl {

static const QString CHECK_DNS_1 = QString::fromUtf8("using.netgear.opendns.com");
static const QString CHECK_DNS_2 = QString::fromUtf8("routerlogin.net");
static const QString CURRENT_SETTINGS = QString::fromUtf8("http://routerlogin.net/currentsetting.htm");
static const QString SOAP_ENDPOINT_1 = QString::fromUtf8("http://routerlogin.net/soap/server_sa/");
static const QString SOAP_ENDPOINT_2 = QString::fromUtf8("http://routerlogin.net:5000/soap/server_sa/");

LookupHostOp::LookupHostOp(const QString& hostname, QObject *parent)
: SimpleOp(parent), m_hostname(hostname), m_lookupId(0), m_lookup(false)
{
	internalStart();
}

LookupHostOp::~LookupHostOp()
{
	if (m_lookup) {
		QHostInfo::abortHostLookup(m_lookupId);
		m_lookup = false;
	}
}

void LookupHostOp::startImpl()
{
	if (isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	m_lookupId = QHostInfo::lookupHost(m_hostname, this, SLOT(lookupHostFinished(QHostInfo)));
	m_lookup = true;
}

void LookupHostOp::abortImpl()
{
	if (m_lookup) {
		QHostInfo::abortHostLookup(m_lookupId);
		m_lookup = false;
	}
}

void LookupHostOp::lookupHostFinished(QHostInfo hostInfo)
{
	if (!m_lookup) {
		return report(WTFStatus_Aborted);
	}

	if (hostInfo.error() != QHostInfo::NoError) {
		DBGLOG(LOG_ERROR, 3, QString::fromUtf8("host lookup failed [%1] error=%2").arg(m_hostname).arg(hostInfo.error()));
		return report(WTFStatus_Failed);
	}

	if (hostInfo.addresses().size() < 1) {
		DBGLOG(LOG_ERROR, 3, QString::fromUtf8("host lookup get ZERO address! [%1]").arg(m_hostname));
		return report(WTFStatus_UnexpectedError);
	}

	QHostAddress ipv4Addr;

	int count = hostInfo.addresses().size();
	for (int i = 0; i < count; i++) {
		const QHostAddress& addr = hostInfo.addresses().at(i);
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("host lookup result[%1]: [%2]").arg(i).arg(addr.toString()));
		if (addr.protocol() == QAbstractSocket::IPv4Protocol) {
			ipv4Addr = addr;
			break;
		}
	}

	if (ipv4Addr.isNull()) {
		DBGLOG(LOG_ERROR, 3, QString::fromUtf8("host lookup failed, no ipv4 address! [%1]").arg(m_hostname));
		return report(WTFStatus_UnexpectedError);
	}

	setProperty("varIpv4", ipv4Addr.toIPv4Address());
	return report(WTFStatus_NoError);
}

SSLTestOp::SSLTestOp(const QString& url, QNetworkAccessManager *nam, QObject *parent)
: SimpleOp(parent), m_url(url), m_nam(nam), m_reply(NULL)
{
	internalStart();
}

SSLTestOp::~SSLTestOp()
{
	if (m_reply) {
		m_reply->deleteLater();
		m_reply = NULL;
	}
}

void SSLTestOp::startImpl()
{
	QNetworkRequest req(m_url);
	m_reply = m_nam->get(req);
	if (!m_reply) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("QNetworkAccessManager::get failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_reply, SIGNAL(finished()), SLOT(replyFinished()));
	connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(replyError(QNetworkReply::NetworkError)));
	connect(m_reply, SIGNAL(sslErrors(const QList<QSslError>&)), SLOT(replySslErrors(const QList<QSslError>&)));

	getRidOfSslErrors(m_reply);
}

void SSLTestOp::abortImpl()
{
	if (m_reply) {
		m_reply->deleteLater();
		m_reply = NULL;
	}
}

void SSLTestOp::replyFinished()
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

	return report(WTFStatus_NoError);
}

void SSLTestOp::replyError(QNetworkReply::NetworkError error)
{
	DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("Network error reported: %1").arg(error));
}

void SSLTestOp::replySslErrors(const QList<QSslError>& errors)
{
	DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("SSL Error, total %1 error(s)").arg(errors.length()));
	for (int i = 0; i < errors.length(); i++) {
		const QSslError& e = errors.at(i);
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("[%1] code=%2 [%3]").arg(i).arg(e.error()).arg(e.errorString()));
	}
}

AutoRouterAuthenticateOp::AutoRouterAuthenticateOp(const QString& username, const QString& password, QNetworkAccessManager *nam, QObject *parent)
: SimpleOp(parent), m_stage(Stage_Start), m_op(NULL), m_username(username), m_password(password), m_nam(nam)
{
	internalStart();
}

AutoRouterAuthenticateOp::~AutoRouterAuthenticateOp()
{
	if (m_op) {
		delete m_op;
		m_op = NULL;
	}
}

void AutoRouterAuthenticateOp::startImpl()
{
	if (isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("Trying to auth to %1").arg(SOAP_ENDPOINT_1));

	m_op = RouterApi::beginAuthenticate(m_username, m_password, SOAP_ENDPOINT_1, m_nam, this);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("RouterApi::beginAuthenticate failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageAuth1Finished()));
	m_stage = Stage_Auth1;
}

void AutoRouterAuthenticateOp::abortImpl()
{
	if (m_op) {
		m_op->abort();
		m_op->deleteLater();
		m_op = NULL;
	}
}

void AutoRouterAuthenticateOp::stageAuth1Finished()
{
	if (m_op == NULL || m_op->isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	int err = op->result().toInt();
	if (err == WTFStatus_NetworkError) {
		DBGLOG(LOG_INFO, 4, QString::fromUtf8("try another auth url"));
		m_op = RouterApi::beginAuthenticate(m_username, m_password, SOAP_ENDPOINT_2, m_nam, this);
		if (!m_op) {
			DBGLOG(LOG_ERROR, 2, QString::fromUtf8("RouterApi::beginAuthenticate failed!"));
			return report(WTFStatus_UnexpectedError);
		}

		connect(m_op, SIGNAL(finished()), SLOT(stageAuth2Finished()));
		m_stage = Stage_Auth2;
		return;
	} else if (err != WTFStatus_NoError) {
		DBGLOG(LOG_DEBUG, 2, QString::fromUtf8("router auth1 failed!"));
		return report(err);
	}

	setProperty("varSoapAction", SOAP_ENDPOINT_1);
	return report(WTFStatus_NoError);
}

void AutoRouterAuthenticateOp::stageAuth2Finished()
{
	if (m_op == NULL || m_op->isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	int err = op->result().toInt();
	if (err != WTFStatus_NoError) {
		DBGLOG(LOG_DEBUG, 2, QString::fromUtf8("router auth2 failed!"));
		return report(err);
	}

	setProperty("varSoapAction", SOAP_ENDPOINT_2);
	return report(WTFStatus_NoError);
}

WrappedRouterOp::WrappedRouterOp(const QString& url, QNetworkAccessManager *nam, QObject *parent)
: SimpleOp(parent), m_stage(Stage_Start), m_op(NULL), m_url(url), m_nam(nam)
{
	internalStart();
}

WrappedRouterOp::~WrappedRouterOp()
{
	if (m_op) {
		m_op->deleteLater();
		m_op = NULL;
	}
}

void WrappedRouterOp::startImpl()
{
	if (isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	m_op = RouterApi::beginConfigurationStart(m_url, m_nam, this);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("RouterApi::beginConfigurationStart failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageBeginCallFinished()));
	m_stage = Stage_BeginCall;
}

void WrappedRouterOp::abortImpl()
{
	if (m_op) {
		m_op->abort();
		m_op->deleteLater();
		m_op = NULL;
	}
}

void WrappedRouterOp::stageBeginCallFinished()
{
	if (m_op == NULL || m_op->isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	int err = op->result().toInt();
	if (err != WTFStatus_NoError) {
		return report(err);
	}

	m_op = beginOp(m_url, m_nam);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("beginOp failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageCallFinished()));
	m_stage = Stage_Call;
}

void WrappedRouterOp::stageCallFinished()
{
	if (m_op == NULL || m_op->isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	// TODO:
	//
	//
	m_errorCode = op->result().toInt();

	QList<QByteArray> dynPropNames = op->dynamicPropertyNames();
	for (int i = 0; i < dynPropNames.length(); i++) {
		setProperty(dynPropNames.at(i), op->property(dynPropNames.at(i)));
	}


	m_op = RouterApi::beginConfigurationFinish(m_url, m_nam, this);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("RouterApi::beginConfigurationFinish failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageEndCallFinished()));
	m_stage = Stage_EndCall;
}

void WrappedRouterOp::stageEndCallFinished()
{
	if (m_op == NULL || m_op->isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	m_op->deleteLater();
	m_op = NULL;

	return report(m_errorCode);
}

WrappedGetDNSMasqDeviceID::WrappedGetDNSMasqDeviceID(const QString& macAddress, const QString& url, QNetworkAccessManager *nam, QObject *parent)
: WrappedRouterOp(url, nam, parent), m_macAddress(macAddress)
{
}

AsyncOp *WrappedGetDNSMasqDeviceID::beginOp(const QString& url, QNetworkAccessManager *nam)
{
	return RouterApi::beginGetDNSMasqDeviceID(m_macAddress, url, nam, this);
}

WrappedSetDNSMasqDeviceID::WrappedSetDNSMasqDeviceID(const QString& macAddress, const QString& deviceId, const QString& url, QNetworkAccessManager *nam, QObject *parent)
: WrappedRouterOp(url, nam, parent), m_macAddress(macAddress), m_deviceId(deviceId)
{
}

AsyncOp *WrappedSetDNSMasqDeviceID::beginOp(const QString& url, QNetworkAccessManager *nam)
{
	return RouterApi::beginSetDNSMasqDeviceID(m_macAddress, m_deviceId, url, nam, this);
}

WrappedGetParentalControlEnableStatus::WrappedGetParentalControlEnableStatus(const QString& url, QNetworkAccessManager *nam, QObject *parent)
: WrappedRouterOp(url, nam, parent)
{
}

AsyncOp *WrappedGetParentalControlEnableStatus::beginOp(const QString& url, QNetworkAccessManager *nam)
{
	return RouterApi::beginGetParentalControlEnableStatus(url, nam, this);
}

WrappedSetParentalControlEnableStatus::WrappedSetParentalControlEnableStatus(bool enable, const QString& url, QNetworkAccessManager *nam, QObject *parent)
: WrappedRouterOp(url, nam, parent), m_enable(enable)
{
}

AsyncOp *WrappedSetParentalControlEnableStatus::beginOp(const QString& url, QNetworkAccessManager *nam)
{
	return RouterApi::beginSetParentalControlEnableStatus(m_enable, url, nam, this);
}

CheckRouterStatusOp::CheckRouterStatusOp(const QString& username, const QString& password, QNetworkAccessManager *nam, QObject *parent)
: SimpleOp(parent), m_stage(Stage_Start), m_op(NULL), m_username(username), m_password(password), m_nam(nam)
{
	internalStart();
}

CheckRouterStatusOp::~CheckRouterStatusOp()
{
	if (m_op) {
		delete m_op;
		m_op = NULL;
	}
}

void CheckRouterStatusOp::startImpl()
{
	if (isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("Looking up %1").arg(CHECK_DNS_1));

	m_op = CoreApi::beginLookupHost(CHECK_DNS_1, this);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("CoreApi::beginLookupHost failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageDns1Finished()));
	m_stage = Stage_Dns1;
}

void CheckRouterStatusOp::abortImpl()
{
	if (m_op) {
		m_op->abort();
		m_op->deleteLater();
		m_op = NULL;
	}
}

void CheckRouterStatusOp::stageDns1Finished()
{
	if (m_op == NULL || m_op->isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	int err = op->result().toInt();
	if (err != WTFStatus_NoError) {
		return report(RouterStatus_NoNetwork);
	}

	quint32 ipv4 = op->property("varIpv4").toInt();
	DBGLOG(LOG_INFO, 4, QString::fromUtf8("%1: %2").arg(CHECK_DNS_1).arg(QHostAddress(ipv4).toString()));

	DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("Looking up %1").arg(CHECK_DNS_2));

	m_op = CoreApi::beginLookupHost(CHECK_DNS_2, this);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("CoreApi::beginLookupHost failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageDns2Finished()));
	m_stage = Stage_Dns2;
}

void CheckRouterStatusOp::stageDns2Finished()
{
	if (m_op == NULL || m_op->isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	int err = op->result().toInt();
	if (err != WTFStatus_NoError) {
		return report(RouterStatus_NoRouter);
	}

	quint32 ipv4 = op->property("varIpv4").toInt();
	DBGLOG(LOG_INFO, 4, QString::fromUtf8("Gateway: %1").arg(QHostAddress(ipv4).toString()));

	if (!isPrivateAddress(ipv4)) {
		DBGLOG(LOG_DEBUG, 3, QString::fromUtf8("no private address"));
		return report(WTFStatus_UnexpectedError);
	}

	QByteArray mac;
	if (getGatewayMacAddress(ipv4, mac)) {
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("Gateway MAC: %1:%2:%3:%4:%5:%6")
			.arg(static_cast<uchar>(mac[0]), 2, 16, QChar::fromAscii('0'))
			.arg(static_cast<uchar>(mac[1]), 2, 16, QChar::fromAscii('0'))
			.arg(static_cast<uchar>(mac[2]), 2, 16, QChar::fromAscii('0'))
			.arg(static_cast<uchar>(mac[3]), 2, 16, QChar::fromAscii('0'))
			.arg(static_cast<uchar>(mac[4]), 2, 16, QChar::fromAscii('0'))
			.arg(static_cast<uchar>(mac[5]), 2, 16, QChar::fromAscii('0')));
	} else {
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("getGatewayMacAddress failed, ignore"));
	}

	DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("Authenticating to router now..."));

	m_op = CoreApi::beginAutoRouterAuthenticate(m_username, m_password, m_nam, this);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("CoreApi::beginAutoRouterAuthenticate failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageAuthFinished()));
	m_stage = Stage_Auth;
}

void CheckRouterStatusOp::stageAuthFinished()
{
	if (m_op == NULL || m_op->isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	int err = op->result().toInt();
	if (err != WTFStatus_NoError) {
		return report(err);
	}

	m_soapAction = op->property("varSoapAction").toString();
	setProperty("varSoapAction", m_soapAction);

	DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("soap action: %1").arg(m_soapAction));

	DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("Querying current settings..."));

	m_op = RouterApi::beginGetCurrentSettings(m_nam, this);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("RouterApi::beginGetCurrentSettings failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageCheckFinished()));
	m_stage = Stage_Check;
}

void CheckRouterStatusOp::stageCheckFinished()
{
	if (m_op == NULL || m_op->isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	int err = op->result().toInt();
	if (err != WTFStatus_NoError) {
		return report(RouterStatus_NoRouter);
	}

	if (op->property("varSupportLPC").toInt() != 1) {
		return report(RouterStatus_ParentalControlNotEnabled);
	}

	DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("Checking OpenDNS API availablity..."));

	m_op = WebApi::beginCheckNameAvailable(QString::fromUtf8("netgear"), m_nam, this);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("WebApi::beginCheckNameAvailable failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageCheck2Finished()));
	m_stage = Stage_Check2;
}

void CheckRouterStatusOp::stageCheck2Finished()
{
	if (m_op == NULL || m_op->isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	int err = op->result().toInt();
	if (err != WTFStatus_NoError) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("OpenDNS API failed, looks like ssl not available?"));
		return report(WTFStatus_UnexpectedError);
	}

	return report(WTFStatus_NoError);
}

GetSetDeviceIdOp::GetSetDeviceIdOp(const QString& userToken, const QString& routerUsername, const QString& routerPassword, QNetworkAccessManager *nam, QObject *parent)
: SimpleOp(parent), m_stage(Stage_Start), m_op(NULL), m_userToken(userToken), m_routerUsername(routerUsername), m_routerPassword(routerPassword), m_nam(nam)
{
	internalStart();
}

GetSetDeviceIdOp::~GetSetDeviceIdOp()
{
	if (m_op) {
		delete m_op;
		m_op = NULL;
	}
}

void GetSetDeviceIdOp::startImpl()
{
	if (isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	m_op = CoreApi::beginAutoRouterAuthenticate(m_routerUsername, m_routerPassword, m_nam, this);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("CoreApi::beginAutoRouterAuthenticate failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageRouterAuthFinished()));
	m_stage = Stage_RouterAuth;
}

void GetSetDeviceIdOp::abortImpl()
{
	if (m_op) {
		m_op->abort();
		m_op->deleteLater();
		m_op = NULL;
	}
}

void GetSetDeviceIdOp::stageRouterAuthFinished()
{
	if (m_op == NULL || m_op->isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	int err = op->result().toInt();
	if (err == WTFStatus_AuthenticationFailed) {
		return report(WebApiStatus_RouterAuthenticationFailed);
	} else if (err != WTFStatus_NoError) {
		return report(err);
	}

	m_soapAction = op->property("varSoapAction").toString();
	DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("soap action: %1").arg(m_soapAction));
	setProperty("varSoapAction", m_soapAction);

	m_op = CoreApi::beginWrappedGetDNSMasqDeviceID(QString::fromUtf8("default"), m_soapAction, m_nam, this);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("CoreApi::beginWrappedGetDNSMasqDeviceID failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageGetDnsFinished()));
	m_stage = Stage_GetDns;

}

void GetSetDeviceIdOp::stageGetDnsFinished()
{
	if (m_op == NULL || m_op->isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	int err = op->result().toInt();
	if (err != WTFStatus_NoError) {
		return report(err);
	}

	m_deviceId = op->property("varDeviceID").toString();

	// TODO: test only
	//m_deviceId.clear();

	DBGLOG(LOG_DEBUG, 12, QString::fromUtf8("device id is [%1]").arg(m_deviceId));

	if (!m_deviceId.isEmpty()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("router has deviceid set, verifying ownership..."));
		m_op = WebApi::beginGetLabel(m_userToken, m_deviceId, m_nam, this);
		if (!m_op) {
			DBGLOG(LOG_ERROR, 2, QString::fromUtf8("WebApi::beginGetLabel failed!"));
			return report(WTFStatus_UnexpectedError);
		}

		connect(m_op, SIGNAL(finished()), SLOT(stageGetLabelFinished()));
		m_stage = Stage_GetLabel;
	} else {
		processWithDeviceId();
	}
}

void GetSetDeviceIdOp::stageGetLabelFinished()
{
	if (m_op == NULL || m_op->isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	int err = op->result().toInt();
	if (err == WTFStatus_Failed) {
		switch (op->property("varErrorCode").toInt()) {
		case 4001: // device not mine
			return report(WebApiStatus_DeviceIdNotMine);
		case 4003: // WTF ?
			m_deviceId.clear();
			break;
		default:
			break;
		}
	}

	processWithDeviceId();
}

void GetSetDeviceIdOp::processWithDeviceId()
{
	if (!m_deviceId.isEmpty()) {
		setProperty("varDeviceID", m_deviceId);
		return report(WTFStatus_NoError);
	}

	DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("trying to generate deviceid for router..."));

	m_op = RouterApi::beginGetModelName(m_soapAction, m_nam, this);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("RouterApi::beginGetModelName failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageGetModelNameFinished()));
	m_stage = Stage_GetModelName;
}

void GetSetDeviceIdOp::stageGetModelNameFinished()
{
	if (m_op == NULL || m_op->isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	int err = op->result().toInt();
	if (err != WTFStatus_NoError) {
		return report(err);
	}

	m_modelName = op->property("varModelName").toString();
	DBGLOG(LOG_DEBUG, 8, QString::fromUtf8("model name: [%1]").arg(m_modelName));

	m_op = RouterApi::beginGetWan(m_soapAction, m_nam, this);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("RouterApi::beginGetWan failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageGetWanFinished()));
	m_stage = Stage_GetWan;
}

void GetSetDeviceIdOp::stageGetWanFinished()
{
	if (m_op == NULL || m_op->isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	int err = op->result().toInt();
	if (err != WTFStatus_NoError) {
		return report(err);
	}

	QString mac;
	mac = op->property("varMACAddress").toString();
	DBGLOG(LOG_DEBUG, 8, QString::fromUtf8("wan mac: [%1]").arg(mac));

	m_deviceKey = QString::fromUtf8("%1-%2").arg(m_modelName).arg(mac);

	m_op = WebApi::beginGetDevice(m_userToken, m_deviceKey, m_nam, this);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("WebApi::beginGetDevice failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageGetDeviceFinished()));
	m_stage = Stage_GetDevice;

}

void GetSetDeviceIdOp::stageGetDeviceFinished()
{
	if (m_op == NULL || m_op->isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	int err = op->result().toInt();
	if (err == WTFStatus_NoError) {
		m_deviceId = op->property("varDeviceID").toString();
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("webapi device_get finished with a valid deviceid [%1]").arg(m_deviceId));
		return processWithNewDeviceId();
	}

	DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("trying to create a new device for router"));

	m_op = WebApi::beginCreateDevice(m_userToken, m_deviceKey, m_nam, this);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("WebApi::beginCreateDevice failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageCreateDeviceFinished()));
	m_stage = Stage_CreateDevice;
}

void GetSetDeviceIdOp::stageCreateDeviceFinished()
{
	if (m_op == NULL || m_op->isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	int err = op->result().toInt();
	if (err != WTFStatus_NoError) {
		return report(WebApiStatus_DeviceIdNotMine);
	}

	m_deviceId = op->property("varDeviceID").toString();
	DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("got new device id from webapi: [%1]").arg(m_deviceId));

	processWithNewDeviceId();
}

void GetSetDeviceIdOp::processWithNewDeviceId()
{
	DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("applying new device id to router"));

	m_op = CoreApi::beginWrappedSetDNSMasqDeviceID(QString::fromUtf8("default"), m_deviceId, m_soapAction, m_nam, this);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("CoreApi::beginWrappedSetDNSMasqDeviceID failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageSetDnsFinished()));
	m_stage = Stage_SetDns;
}

void GetSetDeviceIdOp::stageSetDnsFinished()
{
	if (m_op == NULL || m_op->isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	int err = op->result().toInt();
	if (err != WTFStatus_NoError) {
		return report(err);
	}

	setProperty("varDeviceID", m_deviceId);

	return report(WTFStatus_NoError);
}

SignInOp::SignInOp(const QString& username, const QString& password, const QString& routerUsername, const QString& routerPassword, QNetworkAccessManager *nam, QObject *parent)
: SimpleOp(parent), m_stage(Stage_Start), m_op(NULL), m_username(username), m_password(password), m_routerUsername(routerUsername), m_routerPassword(routerPassword), m_nam(nam)
{
	internalStart();
}

SignInOp::~SignInOp()
{
	if (m_op) {
		delete m_op;
		m_op = NULL;
	}
}

void SignInOp::startImpl()
{
	if (isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	m_op = WebApi::beginLogin(m_username, m_password, m_nam, this);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("WebApi::beginLogin failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageAuthFinished()));
	m_stage = Stage_Auth;
}

void SignInOp::abortImpl()
{
	if (m_op) {
		m_op->abort();
		m_op->deleteLater();
		m_op = NULL;
	}
}

void SignInOp::stageAuthFinished()
{
	if (m_op == NULL || m_op->isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	int err = op->result().toInt();
	if (err != WTFStatus_NoError) {
		return report(err);
	}

	m_userToken = op->property("varToken").toString();
	DBGLOG(LOG_DEBUG, 12, QString::fromUtf8("user token: [%1]").arg(m_userToken));

	setProperty("varToken", m_userToken);

	m_op = CoreApi::beginGetSetDeviceId(m_userToken, m_routerUsername, m_routerPassword, m_nam, this);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("CoreApi::beginGetSetDeviceId failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageGetSetFinished()));
	m_stage = Stage_GetSet;
}

void SignInOp::stageGetSetFinished()
{
	if (m_op == NULL || m_op->isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	int err = op->result().toInt();
	if (err != WTFStatus_NoError) {
		return report(err);
	}

	setProperty("varDeviceID", op->property("varDeviceID"));
	setProperty("varSoapAction", op->property("varSoapAction"));

	return report(WTFStatus_NoError);
}

QuerySettingsOp::QuerySettingsOp(const QString& userToken, const QString& deviceId, const QString& routerUsername, const QString& routerPassword, QNetworkAccessManager *nam, QObject *parent)
: SimpleOp(parent), m_stage(Stage_Start), m_op(NULL), m_userToken(userToken), m_deviceId(deviceId), m_routerUsername(routerUsername), m_routerPassword(routerPassword), m_nam(nam)
{
	internalStart();
}

QuerySettingsOp::~QuerySettingsOp()
{
	if (m_op) {
		delete m_op;
		m_op = NULL;
	}
}

void QuerySettingsOp::startImpl()
{
	if (isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	m_op = WebApi::beginGetFilters(m_userToken, m_deviceId, m_nam, this);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("WebApi::beginGetFilters failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageGetFiltersFinished()));
	m_stage = Stage_GetFilters;
}

void QuerySettingsOp::abortImpl()
{
	if (m_op) {
		m_op->abort();
		m_op->deleteLater();
		m_op = NULL;
	}
}

void QuerySettingsOp::stageGetFiltersFinished()
{
	if (m_op == NULL || m_op->isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	int err = op->result().toInt();
	if (err != WTFStatus_NoError) {
		return report(err);
	}

	setProperty("varBundle", op->property("varBundle"));

	m_op = CoreApi::beginGetSetDeviceId(m_userToken, m_routerUsername, m_routerPassword, m_nam, this);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("CoreApi::beginGetSetDeviceId failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageGetSetFinished()));
	m_stage = Stage_GetSet;
}

void QuerySettingsOp::stageGetSetFinished()
{
	if (m_op == NULL || m_op->isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	int err = op->result().toInt();
	if (err != WTFStatus_NoError) {
		return report(err);
	}

	QString soapAction = op->property("varSoapAction").toString();

	m_op = CoreApi::beginWrappedGetParentalControlEnableStatus(soapAction, m_nam, this);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("CoreApi::beginWrappedGetParentalControlEnableStatus failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageGetLPCStatusFinished()));
	m_stage = Stage_GetLPCStatus;
}

void QuerySettingsOp::stageGetLPCStatusFinished()
{
	if (m_op == NULL || m_op->isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	int err = op->result().toInt();
	if (err != WTFStatus_NoError) {
		return report(err);
	}

	setProperty("varParentalControlStatus", op->property("varParentalControlStatus"));
	return report(WTFStatus_NoError);
}

ToggleParentalControlOp::ToggleParentalControlOp(bool enable, const QString& deviceId, const QString& url, QNetworkAccessManager *nam, QObject *parent)
: SimpleOp(parent), m_stage(Stage_Start), m_op(NULL), m_enable(enable), m_deviceId(deviceId), m_url(url), m_nam(nam)
{
	internalStart();
}

ToggleParentalControlOp::~ToggleParentalControlOp()
{
	if (m_op) {
		delete m_op;
		m_op = NULL;
	}
}

void ToggleParentalControlOp::startImpl()
{
	if (isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	m_op = CoreApi::beginWrappedGetParentalControlEnableStatus(m_url, m_nam, this);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("CoreApi::beginWrappedGetParentalControlEnableStatus failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageGetStatusFinished()));
	m_stage = Stage_GetStatus;
}

void ToggleParentalControlOp::abortImpl()
{
	if (m_op) {
		m_op->abort();
		m_op->deleteLater();
		m_op = NULL;
	}
}

void ToggleParentalControlOp::stageGetStatusFinished()
{
	if (m_op == NULL || m_op->isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	int err = op->result().toInt();
	if (err != WTFStatus_NoError) {
		return report(err);
	}

	m_lpcEnabled = op->property("varParentalControlStatus").toBool();
	if (m_enable == m_lpcEnabled) {
		DBGLOG(LOG_INFO, 4, QString::fromUtf8("LPC alreay in requested state, cancelled"));
		setProperty("varParentalControlStatus", m_enable);
		return report(WTFStatus_NoError);
	}

	// revert
	m_lpcEnabled = !m_lpcEnabled;

	m_op = CoreApi::beginWrappedSetParentalControlEnableStatus(m_lpcEnabled, m_url, m_nam, this);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("CoreApi::beginWrappedSetParentalControlEnableStatus failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageSetStatusFinished()));
	m_stage = Stage_SetStatus;
}

void ToggleParentalControlOp::stageSetStatusFinished()
{
	if (m_op == NULL || m_op->isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	int err = op->result().toInt();
	if (err != WTFStatus_NoError) {
		return report(err);
	}

	if (!m_lpcEnabled) {
		setProperty("varParentalControlStatus", false);
		return report(WTFStatus_NoError);
	}

	m_op = CoreApi::beginWrappedSetDNSMasqDeviceID(QString::fromUtf8("default"), m_deviceId, m_url, m_nam, this);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("CoreApi::beginWrappedSetDNSMasqDeviceID failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageSetMasqFinished()));
	m_stage = Stage_SetMasq;
}

void ToggleParentalControlOp::stageSetMasqFinished()
{
	if (m_op == NULL || m_op->isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	int err = op->result().toInt();
	if (err != WTFStatus_NoError) {
		return report(err);
	}

	setProperty("varParentalControlStatus", true);
	return report(WTFStatus_NoError);
}

SetFiltersAndEnableParentalControlOp::SetFiltersAndEnableParentalControlOp(const QString& url, const QString& token, const QString& deviceId, const QString& bundle, QNetworkAccessManager *nam, QObject *parent)
: SimpleOp(parent), m_stage(Stage_Start), m_op(NULL), m_url(url), m_token(token), m_deviceId(deviceId), m_bundle(bundle), m_nam(nam)
{
	internalStart();
}

SetFiltersAndEnableParentalControlOp::~SetFiltersAndEnableParentalControlOp()
{
	if (m_op) {
		delete m_op;
		m_op = NULL;
	}
}

void SetFiltersAndEnableParentalControlOp::startImpl()
{
	if (isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	m_op = WebApi::beginSetFilters(m_token, m_deviceId, m_bundle, m_nam, this);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("WebApi::beginSetFilters failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageSetFiltersFinished()));
	m_stage = Stage_SetFilters;
}

void SetFiltersAndEnableParentalControlOp::abortImpl()
{
	if (m_op) {
		m_op->abort();
		m_op->deleteLater();
		m_op = NULL;
	}
}

void SetFiltersAndEnableParentalControlOp::stageSetFiltersFinished()
{
	if (m_op == NULL || m_op->isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	int err = op->result().toInt();
	if (err != WTFStatus_NoError) {
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("set filters failed, err=%1").arg(err));
		return report(err);
	}

	m_op = CoreApi::beginToggleParentalControl(true, m_deviceId, m_url, m_nam, this);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("CoreApi::beginToggleParentalControl failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageEnableLPCFinished()));
	m_stage = Stage_EnableLPC;
}

void SetFiltersAndEnableParentalControlOp::stageEnableLPCFinished()
{
	if (m_op == NULL || m_op->isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	int err = op->result().toInt();
	if (err != WTFStatus_NoError) {
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("enable LPC failed, err=%1").arg(err));
		return report(err);
	}

	return report(WTFStatus_NoError);
}

} // namespace CoreApiImpl
