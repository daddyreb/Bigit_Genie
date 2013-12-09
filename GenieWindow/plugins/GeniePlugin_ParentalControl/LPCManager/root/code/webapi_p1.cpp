#include "common.h"
#include "logger.h"
#include "webapi_p1.h"
#include <QMap>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStringList>

AsyncOp *Invoker::beginWebsiteLogin(const QString& username, const QString& password, const QString& routerUsername, const QString& routerPassword, QNetworkAccessManager *nam, QObject *parent)
{
	return new impl::WebsiteLoginOp(username, password, routerUsername, routerPassword, nam, parent);
}

namespace impl {

WebsiteLoginOp::WebsiteLoginOp(const QString& username, const QString& password, const QString& routerUsername, const QString& routerPassword, QNetworkAccessManager *nam, QObject *parent)
: AsyncOp(parent), m_username(username), m_password(password)
, m_routerUsername(routerUsername), m_routerPassword(routerPassword)
, m_nam(nam), m_stage(Stage_Constructed), m_reply(NULL), m_op(NULL)
{
	connect(this, SIGNAL(start()), SLOT(stageStarted()), Qt::QueuedConnection);
	emit start();
}

WebsiteLoginOp::~WebsiteLoginOp()
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

void WebsiteLoginOp::abortImpl()
{
	DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Abort request at stage %1").arg(m_stage));

	switch (m_stage) {
	case Stage_Constructed:
		break;
	case Stage_SignIn:
		if (m_reply) {
			m_reply->abort();
			m_reply->deleteLater();
			m_reply = NULL;
		} else {
			DBGLOG(LOG_DEBUG, 3, QString::fromUtf8("Unexpected m_reply == NULL, stage = %1").arg(m_stage));
		}
		break;
	case Stage_RouterAuth:
	case Stage_RouterConfig1:
	case Stage_RouterConfig2:
	case Stage_RouterConfig3:
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

void WebsiteLoginOp::stageStarted()
{
	if (isAborted()) {
		DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Aborted at 'constructed' stage"));
		return report(WTFStatus_Aborted);
	}

	QStringList paramNames;
	paramNames << QString::fromUtf8("username") << QString::fromUtf8("password");
	QStringList paramValues;
	paramValues << m_username << m_password;

	m_reply = webApiPost(m_nam, "account_signin", paramNames, paramValues);
	if (!m_reply) {
		DBGLOG(LOG_ERROR, 4, QString::fromUtf8("webApiPost failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_reply, SIGNAL(finished()), SLOT(stageSignInFinished()));
	m_stage = Stage_SignIn;
}

void WebsiteLoginOp::stageSignInFinished()
{
	if (isAborted() || m_reply == NULL) {
		DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Aborted at 'signin' stage"));
		return report(WTFStatus_Aborted);
	}

	m_reply->deleteLater();

	switch (m_reply->error()) {
	case QNetworkReply::NoError:
		break;
	default:
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("Signin failed, NetworkError = %1").arg(m_reply->error()));
		m_reply = NULL;
		return report(WTFStatus_UnknownError);
	}

	QByteArray response = m_reply->readAll();
	m_reply = NULL;
	DBGLOG(LOG_DEBUG, 8, QString::fromUtf8("signin response JSON: %1").arg(QString::fromUtf8(response.data())));

	int pos1 = response.indexOf("\"status\"");
	if (pos1 < 0) {
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("parsing json 'status' failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	pos1 += 8; // "status"

	pos1 = response.indexOf(':', pos1);
	if (pos1 < 0) {
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("parsing json 'status' ':' failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	pos1 += 1; // ":"

	int pos2 = response.indexOf(',', pos1);
	if (pos2 < 0) {
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("parsing json 'status' ':' ',' failed!"));
		return report(WTFStatus_UnexpectedError);
	}

	QByteArray token;

	QByteArray statusValue = response.mid(pos1, pos2 - pos1).trimmed().toLower();
	if (statusValue == "\"success\"") {
		int pos3 = response.indexOf("\"token\":\"");
		if (pos3 < 0) {
			DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("parsing json can't find 'token'!!"));
			return report(WTFStatus_UnexpectedError);
		}

		pos3 += 8; // "token":
		token = response.mid(pos3, 32);
		if (token.length() != 32) {
			DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("parsing json bad token (len != 32)!!"));
			return report(WTFStatus_UnexpectedError);
		}
	} else if (statusValue == "\"failure\"") {
		// Ugly!!!
		if (response.indexOf("\"error\":1004") >= 0) {
			return report(WTFStatus_AuthenticationFailed);
		} else {
			return report(WTFStatus_UnknownError);
		}
	} else {
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("parsing json unexpected 'status' value %1").arg(QString::fromUtf8(statusValue)));
		return report(WTFStatus_UnexpectedError);
	}

	DBGLOG(LOG_DEBUG, 8, QString::fromUtf8("login successed, token [%1]").arg(QString::fromUtf8(token)));

	m_op = Invoker::beginRouterLogin(m_routerUsername, m_routerPassword, m_nam, this);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 4, QString::fromUtf8("Invoker::beginRouterLogin failed, unexpected!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageRouterAuthFinished()));
	m_stage = Stage_RouterAuth;
}

void WebsiteLoginOp::stageRouterAuthFinished()
{
	if (m_op == NULL || m_op->isAborted()) {
		DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Aborted at 'routerauth' stage"));
		return report(WTFStatus_Aborted);
	}

	m_op->deleteLater();

	int err = m_op->result().toInt();
	switch (err) {
	case WTFStatus_NoError:
		m_soapAction = m_op->property("varSoapAction").toString();
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("soap action: %1").arg(m_soapAction));
		break;
	case WTFStatus_AuthenticationFailed:
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("router auth failed, bad username/password"));
		m_op = NULL;
		return report(WebApiStatus_RouterAuthenticationFailed);
	default:
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("Unknown error at 'routerauth', err = %1").arg(err));
		m_op = NULL;
		return report(err);
	}

	m_op = NULL;

	m_op = Invoker::beginRouterConfigurationStart(m_soapAction, m_nam, this);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 4, QString::fromUtf8("Invoker::beginRouterConfigurationStart failed, unexpected!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageRouterConfig1Finished()));
	m_stage = Stage_RouterConfig1;
}

void WebsiteLoginOp::stageRouterConfig1Finished()
{
	if (m_op == NULL || m_op->isAborted()) {
		DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Aborted at 'routerconfig1' stage"));
		return report(WTFStatus_Aborted);
	}

	m_op->deleteLater();

	int err = m_op->result().toInt();
	switch (err) {
	case WTFStatus_NoError:
		break;
	default:
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("Unknown error at 'routerauth', err = %1").arg(err));
		m_op = NULL;
		return report(err);
	}

	m_op = NULL;

	m_op = Invoker::beginRouterGetDnsMasqDeviceId(m_soapAction, QString::fromUtf8("default"), m_nam, this);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 4, QString::fromUtf8("Invoker::beginRouterGetDnsMasqDeviceId failed, unexpected!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageRouterConfig2Finished()));
	m_stage = Stage_RouterConfig2;
}

void WebsiteLoginOp::stageRouterConfig2Finished()
{
	if (m_op == NULL || m_op->isAborted()) {
		DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Aborted at 'routerconfig2' stage"));
		return report(WTFStatus_Aborted);
	}

	m_op->deleteLater();

	int err = m_op->result().toInt();
	switch (err) {
	case WTFStatus_NoError:
		break;
	default:
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("Unknown error at 'routerauth', err = %1").arg(err));
		m_op = NULL;
		return report(err);
	}

	m_deviceId = m_op->property("varDeviceID").toByteArray();

	m_op = NULL;

	m_op = Invoker::beginRouterConfigurationFinish(m_soapAction, m_nam, this);
	if (!m_op) {
		DBGLOG(LOG_ERROR, 4, QString::fromUtf8("Invoker::beginRouterConfigurationFinish failed, unexpected!"));
		return report(WTFStatus_UnexpectedError);
	}

	connect(m_op, SIGNAL(finished()), SLOT(stageRouterConfig3Finished()));
	m_stage = Stage_RouterConfig3;
}

void WebsiteLoginOp::stageRouterConfig3Finished()
{
	if (m_op == NULL || m_op->isAborted()) {
		DBGLOG(LOG_DEBUG, 10, QString::fromUtf8("Aborted at 'routerconfig3' stage"));
		return report(WTFStatus_Aborted);
	}

	m_op->deleteLater();

	int err = m_op->result().toInt();
	switch (err) {
	case WTFStatus_NoError:
		break;
	default:
		DBGLOG(LOG_DEBUG, 4, QString::fromUtf8("Unknown error at 'routerauth', err = %1").arg(err));
		m_op = NULL;
		return report(err);
	}

	m_op = NULL;

	// TODO:

	if (!m_deviceId.isEmpty()) {
	}

	continueDetail();
}

void WebsiteLoginOp::continueDetail()
{
	if (m_deviceId.isEmpty()) {
	}
}


} // namespace impl
