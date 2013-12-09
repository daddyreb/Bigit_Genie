#ifndef __webapi_p1_h__
#define __webapi_p1_h__

#include "oputil.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace impl {

class WebsiteLoginOp
	: public AsyncOp
{
	Q_OBJECT

public:
	WebsiteLoginOp(const QString& username, const QString& password, const QString& routerUsername, const QString& routerPassword, QNetworkAccessManager *nam, QObject *parent);
	virtual ~WebsiteLoginOp();

signals:
	void start();

protected:
	virtual void abortImpl();

private slots:
	void stageStarted();
	void stageSignInFinished();
	void stageRouterAuthFinished();
	void stageRouterConfig1Finished();
	void stageRouterConfig2Finished();
	void stageRouterConfig3Finished();

private:
	void continueDetail();

private:
	enum Stage {
		Stage_Constructed,
		Stage_SignIn,
		Stage_RouterAuth,
		Stage_RouterConfig1,
		Stage_RouterConfig2,
		Stage_RouterConfig3,
	};

	QString m_username;
	QString m_password;
	QString m_routerUsername;
	QString m_routerPassword;
	QString m_soapAction;
	QNetworkAccessManager *m_nam;
	Stage m_stage;

	QByteArray m_deviceId;

	QNetworkReply *m_reply;

	AsyncOp *m_op;
};

} // namespace impl

#endif // __webapi_p1_h__
