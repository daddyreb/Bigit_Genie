#ifndef __coreapi_p1_h__
#define __coreapi_p1_h__

#include "coreapi.h"
#include "oputil.h"
#include <QHostInfo>
#include <QNetworkReply>
#include <QThread>

namespace CoreApiImpl {

class BaseOp
	: public SimpleOp
{
public:
	BaseOp(CoreApi *owner);
	virtual ~BaseOp();
	CoreApi *owner() const;
	bool isFcmlMode() const;

private:
	CoreApi *m_owner;
};

class LookupHostOp
	: public BaseOp
{
	Q_OBJECT

public:
	LookupHostOp(const QString& hostname, CoreApi *owner);
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

class LookupHostOpV2
	: public BaseOp
{
	Q_OBJECT

public:
	LookupHostOpV2(const QString& hostname, CoreApi *owner);
	virtual ~LookupHostOpV2();

protected:
	virtual void startImpl();
	virtual void abortImpl();

private Q_SLOTS:
	void onWorkerThreadFinished();

private:
	class WorkerThread
		: public QThread
	{
	public:
		WorkerThread(LookupHostOpV2 *owner);

	protected:
		virtual void run();

	private:
		LookupHostOpV2 *m_owner;
	};

	void workerThreadProc();
	void onFinished(bool aborted);

	QString m_hostname;
	QHostInfo m_hostInfo;
	WorkerThread *m_workerThread;
	bool m_finished;
};

class SSLTestOp
	: public BaseOp
{
	Q_OBJECT

public:
	SSLTestOp(const QString& url, CoreApi *owner);
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
	QNetworkReply *m_reply;
};

class AutoRouterAuthenticateOp
	: public BaseOp
{
	Q_OBJECT

public:
	AutoRouterAuthenticateOp(const QString& username, const QString& password, CoreApi *owner);
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
};

class WrappedRouterOp
	: public BaseOp
{
	Q_OBJECT

public:
	WrappedRouterOp(CoreApi *owner);
	virtual ~WrappedRouterOp();

protected:
	virtual void startImpl();
	virtual void abortImpl();
	virtual AsyncOp *beginOp() = 0;

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
	int m_errorCode;
};

class WrappedGetDNSMasqDeviceID
	: public WrappedRouterOp
{
public:
	WrappedGetDNSMasqDeviceID(const QString& macAddress, CoreApi *owner);

protected:
	virtual AsyncOp *beginOp();

private:
	QString m_macAddress;
};

class WrappedSetDNSMasqDeviceID
	: public WrappedRouterOp
{
public:
	WrappedSetDNSMasqDeviceID(const QString& macAddress, const QString& deviceId, CoreApi *owner);

protected:
	virtual AsyncOp *beginOp();

private:
	QString m_macAddress;
	QString m_deviceId;
};

class WrappedGetParentalControlEnableStatus
	: public WrappedRouterOp
{
public:
	WrappedGetParentalControlEnableStatus(CoreApi *owner);

protected:
	virtual AsyncOp *beginOp();
};

class WrappedSetParentalControlEnableStatus
	: public WrappedRouterOp
{
public:
	WrappedSetParentalControlEnableStatus(bool enable, CoreApi *owner);

protected:
	virtual AsyncOp *beginOp();

private:
	bool m_enable;
};

class WrappedDeleteMacAddress
	: public WrappedRouterOp
{
public:
	WrappedDeleteMacAddress(const QString& macAddress, CoreApi *owner);

protected:
	virtual AsyncOp *beginOp();

private:
	QString m_macAddress;
};

class CheckRouterStatusOp
	: public BaseOp
{
	Q_OBJECT

public:
	CheckRouterStatusOp(const QString& username, const QString& password, CoreApi *owner);
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
	void stageCheck3Finished();
	void stageGetModelNameFinished();

private:
	enum Stage {
		Stage_Start,
		Stage_Dns1,
		Stage_Dns2,
		Stage_Auth,
		Stage_Check,
		Stage_Check2,
		Stage_Check3,
	};

	Stage m_stage;
	AsyncOp *m_op;
	QString m_username;
	QString m_password;
};

class GetSetDeviceIdOp
	: public BaseOp
{
	Q_OBJECT

public:
	GetSetDeviceIdOp(const QString& userToken, const QString& routerUsername, const QString& routerPassword, CoreApi *owner);
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
	QString m_deviceId;
	QString m_modelName;
	QString m_deviceKey;
};

class SignInOp
	: public BaseOp
{
	Q_OBJECT

public:
	SignInOp(const QString& username, const QString& password, const QString& routerUsername, const QString& routerPassword, CoreApi *owner);
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
};

class QuerySettingsOp
	: public BaseOp
{
	Q_OBJECT

public:
	QuerySettingsOp(const QString& userToken, const QString& deviceId, const QString& routerUsername, const QString& routerPassword, CoreApi *owner);
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
};

class ToggleParentalControlOp
	: public BaseOp
{
	Q_OBJECT

public:
	ToggleParentalControlOp(bool enable, const QString& deviceId, CoreApi *owner);
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
	bool m_lpcEnabled;
};

class SetFiltersAndEnableParentalControlOp
	: public BaseOp
{
	Q_OBJECT

public:
	SetFiltersAndEnableParentalControlOp(const QString& token, const QString& deviceId, const QStringList& bundle, CoreApi *owner);
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
	QString m_token;
	QString m_deviceId;
	QStringList m_bundle;
};

class DetectRouterOp
	: public BaseOp
{
	Q_OBJECT

public:
	DetectRouterOp(CoreApi *owner);
	virtual ~DetectRouterOp();

protected:
	virtual void startImpl();
	virtual void abortImpl();

private Q_SLOTS:
	void stageDnsFinished();
	//void stageAuth2Finished();

private:
	enum Stage {
		Stage_Start,
		Stage_Dns,
		Stage_Auth2,
	};

	Stage m_stage;
	AsyncOp *m_op;
};

/////////////////////////////////kai.yan add /////////////////////////////////////////
///AuthenticateAndGetDeviceIDOp 
///功能：认证并获取DeviceID
class AuthenticateAndGetDeviceIDOp
	:public BaseOp
{
	Q_OBJECT
public:
	AuthenticateAndGetDeviceIDOp(const QString& routerUsername, const QString& routerPassword, CoreApi *owner);
	virtual ~AuthenticateAndGetDeviceIDOp();
protected:
	virtual void startImpl();
	virtual void abortImpl();
	private slots:
		void stageRouterAuthFinished();
		void stageGetDeviceIDFinished();
private:
	AsyncOp *m_op;
	QString m_routerUsername;
	QString m_routerPassword;
	QString m_deviceId;
};

class AuthenticateAndDeleteMacAddressOp
	:public BaseOp
{
	Q_OBJECT
public:
	AuthenticateAndDeleteMacAddressOp(const QString& routerUsername, const QString& routerPassword,const QString &macAddress, CoreApi *owner);
	virtual ~AuthenticateAndDeleteMacAddressOp();
protected:
	virtual void startImpl();
	virtual void abortImpl();
	private slots:
		void stageRouterAuthFinished();
		void stageDeleteMacAddressFinished();
private:
	AsyncOp *m_op;
	QString m_routerUsername;
	QString m_routerPassword;
	QString m_macAddress;
};
///////////////////////////////// end /////////////////////////////////////////
} // namespace CoreApiImpl

#endif // __coreapi_p1_h__
