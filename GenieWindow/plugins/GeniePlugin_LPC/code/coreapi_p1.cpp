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

BaseOp::BaseOp(CoreApi *owner)
	: SimpleOp(NULL), m_owner(owner)
{
}

BaseOp::~BaseOp()
{
}

CoreApi *BaseOp::owner() const
{
	return m_owner;
}

bool BaseOp::isFcmlMode() const
{
	return m_owner->isFcmlMode();
}

LookupHostOp::LookupHostOp(const QString& hostname, CoreApi *owner)
: BaseOp(owner), m_hostname(hostname), m_lookupId(0), m_lookup(false)
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
		setProperty("varHostError", hostInfo.error());
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

LookupHostOpV2::LookupHostOpV2(const QString& hostname, CoreApi *owner)
	: BaseOp(owner), m_hostname(hostname), m_workerThread(NULL), m_finished(false)
{
	internalStart();
}

LookupHostOpV2::~LookupHostOpV2()
{
	if (m_workerThread) {
		m_workerThread->wait();
		delete m_workerThread;
		m_workerThread = NULL;
	}
}

void LookupHostOpV2::startImpl()
{
	m_workerThread = new WorkerThread(this);
	m_workerThread->start();
}

void LookupHostOpV2::abortImpl()
{
	onFinished(true);
}

void LookupHostOpV2::onWorkerThreadFinished()
{
	onFinished(false);
}

void LookupHostOpV2::onFinished(bool aborted)
{
	if (m_finished) {
		return;
	}

	m_finished = true;

	if (aborted) {
		return report(WTFStatus_Aborted);
	}

	const QHostInfo& hostInfo = m_hostInfo;

	if (hostInfo.error() != QHostInfo::NoError) {
		DBGLOG(LOG_ERROR, 3, QString::fromUtf8("host lookup failed [%1] error=%2").arg(m_hostname).arg(hostInfo.error()));
		setProperty("varHostError", hostInfo.error());
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

void LookupHostOpV2::workerThreadProc()
{
	m_hostInfo = QHostInfo::fromName(m_hostname);
	QMetaObject::invokeMethod(this, "onWorkerThreadFinished", Qt::QueuedConnection);
}

LookupHostOpV2::WorkerThread::WorkerThread(LookupHostOpV2 *owner)
	: m_owner(owner)
{
}

void LookupHostOpV2::WorkerThread::run()
{
	m_owner->workerThreadProc();
}

SSLTestOp::SSLTestOp(const QString& url, CoreApi *owner)
: BaseOp(owner), m_url(url), m_reply(NULL)
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
	m_reply = owner()->networkAccessManager()->get(req);
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
        qDebug()<<"***SSLTestOp::replyFinished()***response data:"<<responseData;
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

AutoRouterAuthenticateOp::AutoRouterAuthenticateOp(const QString& username, const QString& password, CoreApi *owner)
: BaseOp(owner), m_stage(Stage_Start), m_op(NULL), m_username(username), m_password(password)
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
		setProperty("UURouterError", UURouterError_Unknown);
		return report(WTFStatus_Aborted);
	}

	m_op = owner()->routerApi()->beginAuthenticate(m_username, m_password);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("owner()->routerApi()->beginAuthenticate failed!"));
		setProperty("UURouterError", UURouterError_Unknown);
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
		setProperty("UURouterError", UURouterError_Unknown);
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	int err = op->result().toInt();
	if (err == WTFStatus_NetworkError) {
		DBGLOG(LOG_INFO, 4, QString::fromUtf8("try another auth url"));
		m_op = owner()->routerApi()->beginAuthenticate(m_username, m_password);
		if (!m_op) {
			DBGLOG(LOG_ERROR, 2, QString::fromUtf8("owner()->routerApi()->beginAuthenticate failed!"));
			setProperty("UURouterError", UURouterError_Unknown);
			return report(WTFStatus_UnexpectedError);
		}

		connect(m_op, SIGNAL(finished()), SLOT(stageAuth2Finished()));
		m_stage = Stage_Auth2;
		return;
	} else if (err != WTFStatus_NoError) {
		DBGLOG(LOG_DEBUG, 2, QString::fromUtf8("router auth1 failed!"));
		setProperty("UURouterError", op->property("UURouterError"));
		return report(err);
	}

	setProperty("UURouterError", UURouterError_NoError);
	return report(WTFStatus_NoError);
}

void AutoRouterAuthenticateOp::stageAuth2Finished()
{
	if (m_op == NULL || m_op->isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		setProperty("UURouterError", UURouterError_Unknown);
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	int err = op->result().toInt();
	if (err != WTFStatus_NoError) {
		DBGLOG(LOG_DEBUG, 2, QString::fromUtf8("router auth2 failed!"));
		setProperty("UURouterError", op->property("UURouterError"));
		return report(err);
	}

	setProperty("UURouterError", UURouterError_NoError);
	return report(WTFStatus_NoError);
}

WrappedRouterOp::WrappedRouterOp(CoreApi *owner)
: BaseOp(owner), m_stage(Stage_Start), m_op(NULL)
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

	m_op = owner()->routerApi()->beginConfigurationStart();
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("owner()->routerApi()->beginConfigurationStart failed!"));
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

	m_op = beginOp();
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


	m_op = owner()->routerApi()->beginConfigurationFinish();
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("owner()->routerApi()->beginConfigurationFinish failed!"));
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

WrappedGetDNSMasqDeviceID::WrappedGetDNSMasqDeviceID(const QString& macAddress, CoreApi *owner)
: WrappedRouterOp(owner), m_macAddress(macAddress)
{
}

AsyncOp *WrappedGetDNSMasqDeviceID::beginOp()
{
	return owner()->routerApi()->beginGetDNSMasqDeviceID(m_macAddress);
}

WrappedSetDNSMasqDeviceID::WrappedSetDNSMasqDeviceID(const QString& macAddress, const QString& deviceId, CoreApi *owner)
: WrappedRouterOp(owner), m_macAddress(macAddress), m_deviceId(deviceId)
{
}

AsyncOp *WrappedSetDNSMasqDeviceID::beginOp()
{
	return owner()->routerApi()->beginSetDNSMasqDeviceID(m_macAddress, m_deviceId);
}

WrappedGetParentalControlEnableStatus::WrappedGetParentalControlEnableStatus(CoreApi *owner)
: WrappedRouterOp(owner)
{
}

AsyncOp *WrappedGetParentalControlEnableStatus::beginOp()
{
	return owner()->routerApi()->beginGetParentalControlEnableStatus();
}

WrappedSetParentalControlEnableStatus::WrappedSetParentalControlEnableStatus(bool enable, CoreApi *owner)
: WrappedRouterOp(owner), m_enable(enable)
{
}

AsyncOp *WrappedSetParentalControlEnableStatus::beginOp()
{
	return owner()->routerApi()->beginSetParentalControlEnableStatus(m_enable);
}

WrappedDeleteMacAddress::WrappedDeleteMacAddress(const QString& macAddress, CoreApi *owner)
	: WrappedRouterOp(owner), m_macAddress(macAddress)
{
}

AsyncOp *WrappedDeleteMacAddress::beginOp()
{
	return owner()->routerApi()->beginDeleteMacAddress(m_macAddress);
}

CheckRouterStatusOp::CheckRouterStatusOp(const QString& username, const QString& password, CoreApi *owner)
: BaseOp(owner), m_stage(Stage_Start), m_op(NULL), m_username(username), m_password(password)
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

	if (isFcmlMode()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("Authenticating to router now..."));

		m_op = owner()->beginAutoRouterAuthenticate(m_username, m_password);
		if (!m_op) {
			DBGLOG(LOG_ERROR, 2, QString::fromUtf8("CoreApi::beginAutoRouterAuthenticate failed!"));
			return report(WTFStatus_UnexpectedError);
		}

		connect(m_op, SIGNAL(finished()), SLOT(stageAuthFinished()));
		m_stage = Stage_Auth;
	} else {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("Looking up %1").arg(CHECK_DNS_1));

		m_op = owner()->beginLookupHost(CHECK_DNS_1);
		if (!m_op) {
			DBGLOG(LOG_ERROR, 2, QString::fromUtf8("CoreApi::beginLookupHost failed!"));
			return report(WTFStatus_UnexpectedError);
		}

		connect(m_op, SIGNAL(finished()), SLOT(stageDns1Finished()));
		m_stage = Stage_Dns1;
	}
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

	m_op = owner()->beginLookupHost(CHECK_DNS_2);
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
/*
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
*/
	DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("Authenticating to router now..."));

	m_op = owner()->beginAutoRouterAuthenticate(m_username, m_password);
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

	if (isFcmlMode()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("trying to generate deviceid for router..."));

		m_op = owner()->routerApi()->beginGetModelName();
		if (!m_op) {
			DBGLOG(LOG_ERROR, 2, QString::fromUtf8("owner()->routerApi()->beginGetModelName failed!"));
			return report(WTFStatus_UnexpectedError);
		}

		connect(m_op, SIGNAL(finished()), SLOT(stageGetModelNameFinished()));
		m_stage = Stage_Check;
	} else{
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("Querying current settings..."));

		m_op = owner()->routerApi()->beginGetCurrentSettings();
		if (!m_op) {
			DBGLOG(LOG_ERROR, 2, QString::fromUtf8("owner()->routerApi()->beginGetCurrentSettings failed!"));
			return report(WTFStatus_UnexpectedError);
		}

		connect(m_op, SIGNAL(finished()), SLOT(stageCheckFinished()));
		m_stage = Stage_Check;
	}
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

	setProperty("varModel", op->property("varModel"));

	DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("Checking OpenDNS API availablity..."));

	m_op = owner()->webApi()->beginCheckNameAvailable(QString::fromUtf8("netgear"));
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("owner()->webApi()->beginCheckNameAvailable failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageCheck2Finished()));
	m_stage = Stage_Check2;
}

void CheckRouterStatusOp::stageGetModelNameFinished()
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

	setProperty("varModel", op->property("varModelName"));

	DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("Checking OpenDNS API availablity..."));

	m_op = owner()->webApi()->beginCheckNameAvailable(QString::fromUtf8("netgear"));
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("owner()->webApi()->beginCheckNameAvailable failed!"));
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

	DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("Get device id for default mac..."));
        m_op = owner()->beginWrappedGetDNSMasqDeviceID(QString::fromUtf8("default"));
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("CoreApi::beginWrappedGetDNSMasqDeviceID failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageCheck3Finished()));
	m_stage = Stage_Check3;
}

void CheckRouterStatusOp::stageCheck3Finished()
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
		setProperty("varDeviceID", op->property("varDeviceID"));
	}

	return report(WTFStatus_NoError);
}

GetSetDeviceIdOp::GetSetDeviceIdOp(const QString& userToken, const QString& routerUsername, const QString& routerPassword, CoreApi *owner)
: BaseOp(owner), m_stage(Stage_Start), m_op(NULL), m_userToken(userToken), m_routerUsername(routerUsername), m_routerPassword(routerPassword)
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

	m_op = owner()->beginAutoRouterAuthenticate(m_routerUsername, m_routerPassword);
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

        m_op = owner()->beginWrappedGetDNSMasqDeviceID(QString::fromUtf8("default"));
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
		m_op = owner()->webApi()->beginGetLabel(m_userToken, m_deviceId);
		if (!m_op) {
			DBGLOG(LOG_ERROR, 2, QString::fromUtf8("owner()->webApi()->beginGetLabel failed!"));
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

	m_op = owner()->routerApi()->beginGetModelName();
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("owner()->routerApi()->beginGetModelName failed!"));
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

	m_op = owner()->routerApi()->beginGetWan();
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("owner()->routerApi()->beginGetWan failed!"));
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

	m_op = owner()->webApi()->beginGetDevice(m_userToken, m_deviceKey);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("owner()->webApi()->beginGetDevice failed!"));
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

	m_op = owner()->webApi()->beginCreateDevice(m_userToken, m_deviceKey);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("owner()->webApi()->beginCreateDevice failed!"));
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

	m_op = owner()->beginWrappedSetDNSMasqDeviceID(QString::fromUtf8("default"), m_deviceId);
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

SignInOp::SignInOp(const QString& username, const QString& password, const QString& routerUsername, const QString& routerPassword, CoreApi *owner)
: BaseOp(owner), m_stage(Stage_Start), m_op(NULL), m_username(username), m_password(password), m_routerUsername(routerUsername), m_routerPassword(routerPassword)
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

	m_op = owner()->webApi()->beginLogin(m_username, m_password);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("owner()->webApi()->beginLogin failed!"));
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

	m_op = owner()->beginGetSetDeviceId(m_userToken, m_routerUsername, m_routerPassword);
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

	return report(WTFStatus_NoError);
}

QuerySettingsOp::QuerySettingsOp(const QString& userToken, const QString& deviceId, const QString& routerUsername, const QString& routerPassword, CoreApi *owner)
: BaseOp(owner), m_stage(Stage_Start), m_op(NULL), m_userToken(userToken), m_deviceId(deviceId), m_routerUsername(routerUsername), m_routerPassword(routerPassword)
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

	m_op = owner()->webApi()->beginGetFilters(m_userToken, m_deviceId);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("owner()->webApi()->beginGetFilters failed!"));
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
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("QuerySettingsOp::stageGetFiltersFinished error code %1").arg(op->property("varErrorCode").toInt()));
		setProperty("varErrorCode", op->property("varErrorCode"));
		return report(err);
	}

	setProperty("varBundle", op->property("varBundle"));

	m_op = owner()->beginGetSetDeviceId(m_userToken, m_routerUsername, m_routerPassword);
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

	m_op = owner()->beginWrappedGetParentalControlEnableStatus();
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

ToggleParentalControlOp::ToggleParentalControlOp(bool enable, const QString& deviceId, CoreApi *owner)
: BaseOp(owner), m_stage(Stage_Start), m_op(NULL), m_enable(enable), m_deviceId(deviceId)
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

	m_op = owner()->beginWrappedGetParentalControlEnableStatus();
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

	m_op = owner()->beginWrappedSetParentalControlEnableStatus(m_lpcEnabled);
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

	m_op = owner()->beginWrappedSetDNSMasqDeviceID(QString::fromUtf8("default"), m_deviceId);
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

SetFiltersAndEnableParentalControlOp::SetFiltersAndEnableParentalControlOp(const QString& token, const QString& deviceId, const QStringList& bundle, CoreApi *owner)
: BaseOp(owner), m_stage(Stage_Start), m_op(NULL), m_token(token), m_deviceId(deviceId), m_bundle(bundle)
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

	m_op = owner()->webApi()->beginSetFilters(m_token, m_deviceId, m_bundle);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("owner()->webApi()->beginSetFilters failed!"));
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

	m_op = owner()->beginToggleParentalControl(true, m_deviceId);
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

DetectRouterOp::DetectRouterOp(CoreApi *owner)
	: BaseOp(owner), m_stage(Stage_Start), m_op(NULL)
{
	internalStart();
}

DetectRouterOp::~DetectRouterOp()
{
	if (m_op) {
		delete m_op;
		m_op = NULL;
	}
}

void DetectRouterOp::startImpl()
{
	if (isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		setProperty("UURouterError", UURouterError_NoRouter);
		return report(WTFStatus_Aborted);
	}

	m_op = owner()->beginLookupHost(CHECK_DNS_2);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("CoreApi::beginLookupHost failed!"));
		setProperty("UURouterError", UURouterError_NoRouter);
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageDnsFinished()));
	m_stage = Stage_Dns;
}

void DetectRouterOp::abortImpl()
{
	if (m_op) {
		m_op->abort();
		m_op->deleteLater();
		m_op = NULL;
	}
}

static QString macToString(const QByteArray& mac)
{
	QString s;
	s.sprintf("%02x%02x%02x%02x%02x%02x", (unsigned char)mac[0], (unsigned char)mac[1], (unsigned char)mac[2], (unsigned char)mac[3], (unsigned char)mac[4], (unsigned char)mac[5]);
	return s;
}

void DetectRouterOp::stageDnsFinished()
{
	if (m_op == NULL || m_op->isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		setProperty("UURouterError", UURouterError_NoRouter);
		return report(WTFStatus_Aborted);
	}

	AsyncOp *op = m_op;
	m_op->deleteLater();
	m_op = NULL;

	int err = op->result().toInt();
	if (err != WTFStatus_NoError) {
		QVariant varHostError = op->property("varHostError");
		if (varHostError.isValid() && varHostError.toInt() == QHostInfo::HostNotFound) {
			setProperty("UURouterError", UURouterError_NoRouter);
		} else {
			setProperty("UURouterError", UURouterError_NoNetwork);
		}
		return report(err);
	}

	quint32 ipv4 = op->property("varIpv4").toUInt();

	QByteArray mac;

#ifdef LPC_HAVE_GATEWAY_MAC

	if (!getGatewayMacAddress(ipv4, mac)) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("getGatewayMacAddress failed!"));
		return report(WTFStatus_UnexpectedError);
	}

#endif // LPC_HAVE_GATEWAY_MAC

	setProperty("varMac", macToString(mac));

	setProperty("UURouterError", UURouterError_NoError);
	return report(WTFStatus_NoError);
}

/////////////////////////////// kai.yan add ///////////////////////////////////////////
AuthenticateAndGetDeviceIDOp::AuthenticateAndGetDeviceIDOp(const QString& routerUsername, const QString& routerPassword, CoreApi *owner)
	:BaseOp(owner), m_op(NULL), m_routerUsername(routerUsername), m_routerPassword(routerPassword)
{
	internalStart();
}
AuthenticateAndGetDeviceIDOp::~AuthenticateAndGetDeviceIDOp()
{
	if (m_op) {
		delete m_op;
		m_op = NULL;
	}
}
void AuthenticateAndGetDeviceIDOp::startImpl()
{
	if (isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	m_op = owner()->beginAutoRouterAuthenticate(m_routerUsername, m_routerPassword);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("CoreApi::beginAutoRouterAuthenticate failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageRouterAuthFinished()));
}
void AuthenticateAndGetDeviceIDOp::abortImpl()
{
	if (m_op) {
		m_op->abort();
		m_op->deleteLater();
		m_op = NULL;
	}
}
void AuthenticateAndGetDeviceIDOp::stageRouterAuthFinished()
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

	m_op = owner()->beginWrappedGetDNSMasqDeviceID(QString::fromUtf8("default"));
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("CoreApi::beginWrappedGetDNSMasqDeviceID failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageGetDeviceIDFinished()));
}

void AuthenticateAndGetDeviceIDOp::stageGetDeviceIDFinished()
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
	return report(WTFStatus_NoError);
}

/// /Authenticate and Delete mac address
AuthenticateAndDeleteMacAddressOp::AuthenticateAndDeleteMacAddressOp(const QString& routerUsername, const QString& routerPassword, const QString& macAddress, CoreApi *owner)
	:BaseOp(owner), m_op(NULL), m_routerUsername(routerUsername), m_routerPassword(routerPassword),m_macAddress(macAddress)
{
	internalStart();
}
AuthenticateAndDeleteMacAddressOp::~AuthenticateAndDeleteMacAddressOp()
{
	if (m_op) {
		delete m_op;
		m_op = NULL;
	}
}
void AuthenticateAndDeleteMacAddressOp::startImpl()
{
	if (isAborted()) {
		DBGLOG(LOG_DEBUG, 6, QString::fromUtf8("aborted"));
		return report(WTFStatus_Aborted);
	}

	m_op = owner()->beginAutoRouterAuthenticate(m_routerUsername, m_routerPassword);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("CoreApi::beginAutoRouterAuthenticate failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageRouterAuthFinished()));
}
void AuthenticateAndDeleteMacAddressOp::abortImpl()
{
	if (m_op) {
		m_op->abort();
		m_op->deleteLater();
		m_op = NULL;
	}
}
void AuthenticateAndDeleteMacAddressOp::stageRouterAuthFinished()
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

	m_op = owner()->beginWrappedDeleteMacAddress(m_macAddress);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 2, QString::fromUtf8("CoreApi::beginWrappedDeleteMacAddress failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageDeleteMacAddressFinished()));
}

void AuthenticateAndDeleteMacAddressOp::stageDeleteMacAddressFinished()
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
	return report(WTFStatus_NoError);
}
/////////////////////////////// end ///////////////////////////////////////////
} // namespace CoreApiImpl
