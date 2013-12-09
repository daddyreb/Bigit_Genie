#ifndef __coreapi_p1_h__
#define __coreapi_p1_h__

#include "oputil.h"
#include <QHostInfo>
#include <QNetworkReply>

namespace CoreApiImpl {

class LookupHostOp
	: public SimpleOp
{
	Q_OBJECT

public:
	LookupHostOp(const QString& hostname, QObject *parent);
	virtual ~LookupHostOp();

protected:
	virtual void startImpl();
	virtual void abortImpl();

private slots:
	void lookupHostFinished(QHostInfo hostInfo);

private:
	QString m_hostname;
	int m_lookupId;
	bool m_lookup;
};

class SSLTestOp
	: public SimpleOp
{
	Q_OBJECT

public:
	SSLTestOp(const QString& url, QNetworkAccessManager *nam, QObject *parent);
	virtual ~SSLTestOp();

protected:
	virtual void startImpl();
	virtual void abortImpl();

private slots:
	void replyFinished();
	void replyError(QNetworkReply::NetworkError error);
	void replySslErrors(const QList<QSslError>& errors);

private:
	QString m_url;
	QNetworkAccessManager *m_nam;
	QNetworkReply *m_reply;
};

class AutoRouterAuthenticateOp
	: public SimpleOp
{
	Q_OBJECT

public:
	AutoRouterAuthenticateOp(const QString& username, const QString& password, QNetworkAccessManager *nam, QObject *parent);
	virtual ~AutoRouterAuthenticateOp();

protected:
	virtual void startImpl();
	virtual void abortImpl();

private slots:
	void stageAuth1Finished();
	void stageAuth2Finished();

private:
	enum Stage {
		Stage_Start,
		Stage_Auth1,
		Stage_Auth2,
	};

	Stage m_stage;
	AsyncOp *m_op;
	QString m_username;
	QString m_password;
	QNetworkAccessManager *m_nam;
};

class WrappedRouterOp
	: public SimpleOp
{
	Q_OBJECT

public:
	WrappedRouterOp(const QString& url, QNetworkAccessManager *nam, QObject *parent);
	virtual ~WrappedRouterOp();

protected:
	virtual void startImpl();
	virtual void abortImpl();
	virtual AsyncOp *beginOp(const QString& url, QNetworkAccessManager *nam) = 0;

private slots:
	void stageBeginCallFinished();
	void stageCallFinished();
	void stageEndCallFinished();

private:
	enum Stage {
		Stage_Start,
		Stage_BeginCall,
		Stage_Call,
		Stage_EndCall,
	};

	Stage m_stage;
	AsyncOp *m_op;
	QString m_url;
	QNetworkAccessManager *m_nam;
	int m_errorCode;
};

class WrappedGetDNSMasqDeviceID
	: public WrappedRouterOp
{
public:
	WrappedGetDNSMasqDeviceID(const QString& macAddress, const QString& url, QNetworkAccessManager *nam, QObject *parent);

protected:
	virtual AsyncOp *beginOp(const QString& url, QNetworkAccessManager *nam);

private:
	QString m_macAddress;
};

class WrappedSetDNSMasqDeviceID
	: public WrappedRouterOp
{
public:
	WrappedSetDNSMasqDeviceID(const QString& macAddress, const QString& deviceId, const QString& url, QNetworkAccessManager *nam, QObject *parent);

protected:
	virtual AsyncOp *beginOp(const QString& url, QNetworkAccessManager *nam);

private:
	QString m_macAddress;
	QString m_deviceId;
};

class WrappedGetParentalControlEnableStatus
	: public WrappedRouterOp
{
public:
	WrappedGetParentalControlEnableStatus(const QString& url, QNetworkAccessManager *nam, QObject *parent);

protected:
	virtual AsyncOp *beginOp(const QString& url, QNetworkAccessManager *nam);
};

class WrappedSetParentalControlEnableStatus
	: public WrappedRouterOp
{
public:
	WrappedSetParentalControlEnableStatus(bool enable, const QString& url, QNetworkAccessManager *nam, QObject *parent);

protected:
	virtual AsyncOp *beginOp(const QString& url, QNetworkAccessManager *nam);

private:
	bool m_enable;
};

class CheckRouterStatusOp
	: public SimpleOp
{
	Q_OBJECT

public:
	CheckRouterStatusOp(const QString& username, const QString& password, QNetworkAccessManager *nam, QObject *parent);
	virtual ~CheckRouterStatusOp();

protected:
	virtual void startImpl();
	virtual void abortImpl();

private slots:
	void stageDns1Finished();
	void stageDns2Finished();
	void stageAuthFinished();
	void stageCheckFinished();
	void stageCheck2Finished();

private:
	enum Stage {
		Stage_Start,
		Stage_Dns1,
		Stage_Dns2,
		Stage_Auth,
		Stage_Check,
		Stage_Check2,
	};

	Stage m_stage;
	AsyncOp *m_op;
	QString m_username;
	QString m_password;
	QNetworkAccessManager *m_nam;
	QString m_soapAction;
};

class GetSetDeviceIdOp
	: public SimpleOp
{
	Q_OBJECT

public:
	GetSetDeviceIdOp(const QString& userToken, const QString& routerUsername, const QString& routerPassword, QNetworkAccessManager *nam, QObject *parent);
	virtual ~GetSetDeviceIdOp();

protected:
	virtual void startImpl();
	virtual void abortImpl();

private slots:
	void stageRouterAuthFinished();
	void stageGetDnsFinished();
	void stageGetLabelFinished();
	void stageGetModelNameFinished();
	void stageGetWanFinished();
	void stageGetDeviceFinished();
	void stageCreateDeviceFinished();
	void stageSetDnsFinished();

private:
	void processWithDeviceId();
	void processWithNewDeviceId();

private:
	enum Stage {
		Stage_Start,
		Stage_RouterAuth,
		Stage_GetDns,
		Stage_GetLabel,
		Stage_GetModelName,
		Stage_GetWan,
		Stage_GetDevice,
		Stage_CreateDevice,
		Stage_SetDns,
	};

	Stage m_stage;
	AsyncOp *m_op;
	QString m_username;
	QString m_password;
	QString m_userToken;
	QString m_routerUsername;
	QString m_routerPassword;
	QNetworkAccessManager *m_nam;
	QString m_soapAction;
	QString m_deviceId;
	QString m_modelName;
	QString m_deviceKey;
};

class SignInOp
	: public SimpleOp
{
	Q_OBJECT

public:
	SignInOp(const QString& username, const QString& password, const QString& routerUsername, const QString& routerPassword, QNetworkAccessManager *nam, QObject *parent);
	virtual ~SignInOp();

protected:
	virtual void startImpl();
	virtual void abortImpl();

private slots:
	void stageAuthFinished();
	void stageGetSetFinished();

private:
	enum Stage {
		Stage_Start,
		Stage_Auth,
		Stage_GetSet,
	};

	Stage m_stage;
	AsyncOp *m_op;
	QString m_username;
	QString m_password;
	QString m_routerUsername;
	QString m_routerPassword;
	QString m_userToken;
	QNetworkAccessManager *m_nam;
};

class QuerySettingsOp
	: public SimpleOp
{
	Q_OBJECT

public:
	QuerySettingsOp(const QString& userToken, const QString& deviceId, const QString& routerUsername, const QString& routerPassword, QNetworkAccessManager *nam, QObject *parent);
	virtual ~QuerySettingsOp();

protected:
	virtual void startImpl();
	virtual void abortImpl();

private slots:
	void stageGetFiltersFinished();
	void stageGetSetFinished();
	void stageGetLPCStatusFinished();

private:
	enum Stage {
		Stage_Start,
		Stage_GetFilters,
		Stage_GetSet,
		Stage_GetLPCStatus,
	};

	Stage m_stage;
	AsyncOp *m_op;
	QString m_userToken;
	QString m_deviceId;
	QString m_routerUsername;
	QString m_routerPassword;
	QNetworkAccessManager *m_nam;
};

class ToggleParentalControlOp
	: public SimpleOp
{
	Q_OBJECT

public:
	ToggleParentalControlOp(bool enable, const QString& deviceId, const QString& url, QNetworkAccessManager *nam, QObject *parent);
	virtual ~ToggleParentalControlOp();

protected:
	virtual void startImpl();
	virtual void abortImpl();

private slots:
	void stageGetStatusFinished();
	void stageSetStatusFinished();
	void stageSetMasqFinished();

private:
	enum Stage {
		Stage_Start,
		Stage_GetStatus,
		Stage_SetStatus,
		Stage_SetMasq,
	};

	Stage m_stage;
	AsyncOp *m_op;
	bool m_enable;
	QString m_deviceId;
	QString m_url;
	QNetworkAccessManager *m_nam;
	bool m_lpcEnabled;
};

class SetFiltersAndEnableParentalControlOp
	: public SimpleOp
{
	Q_OBJECT

public:
	SetFiltersAndEnableParentalControlOp(const QString& url, const QString& token, const QString& deviceId, const QString& bundle, QNetworkAccessManager *nam, QObject *parent);
	virtual ~SetFiltersAndEnableParentalControlOp();

protected:
	virtual void startImpl();
	virtual void abortImpl();

private slots:
	void stageSetFiltersFinished();
	void stageEnableLPCFinished();

private:
	enum Stage {
		Stage_Start,
		Stage_SetFilters,
		Stage_EnableLPC,
	};

	Stage m_stage;
	AsyncOp *m_op;
	QString m_url;
	QString m_token;
	QString m_deviceId;
	QString m_bundle;
	QNetworkAccessManager *m_nam;
};

} // namespace CoreApiImpl

#endif // __coreapi_p1_h__
