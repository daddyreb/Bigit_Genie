#ifndef __common_h__
#define __common_h__

#include <QObject>
#include <QString>

class AsyncOp;
class QNetworkAccessManager;

enum WTFStatus
{
	WTFStatus_NoError,
	WTFStatus_Aborted,
	WTFStatus_Timeout,
	WTFStatus_UnexpectedError,
	WTFStatus_UnknownError,
	WTFStatus_Failed,
	WTFStatus_NetworkError,
	WTFStatus_AuthenticationFailed,
	WTFStatus_SocketConnectFaild,
	WTFStatus_SocketWriteFailed,
	WTFStatus_SocketReadFailed,
	WTFStatus_SmartNetworkAuthFailed,

	WebApiStatus_DeviceKeyError = 400,
	WebApiStatus_DeviceIdNotMine,
	WebApiStatus_RouterAuthenticationFailed,

	RouterStatus_NoNetwork = 600,
	RouterStatus_NoRouter,
	RouterStatus_ParentalControlNotEnabled,
	RouterStatus_NoDefaultDeviceId,
};

enum UURouterError
{
	UURouterError_NoError,
	UURouterError_NoNetwork,
	UURouterError_NoRouter,
	UURouterError_ParentalControlNotEnabled,
	UURouterError_NoDefaultDeviceId,
	UURouterError_InvalidRouterPassword,
	UURouterError_Unknown
};

enum UUWebsiteError
{
	UUWebsiteError_NoError,
	UUWebsiteError_Unknown,
	UUWebsiteError_NoNetwork,
	UUWebsiteError_NoRouter,
	UUWebsiteError_ParentalControlNotEnabled,
	UUWebsiteError_NoDefaultDeviceId,
	UUWebsiteError_FailedDownloadUserAccounts,
	UUWebsiteError_NoUserAccountsConfigured,
	UUWebsiteError_FailedToDisableParentalControls
};

class Invoker
{
public:
	static AsyncOp *beginCheckRouterStatus(const QString& username, const QString& password, QNetworkAccessManager *nam, QObject *parent = NULL);
	static AsyncOp *beginWebsiteLogin(const QString& username, const QString& password, const QString& routerUsername, const QString& routerPassword, QNetworkAccessManager *nam, QObject *parent = NULL);

	static AsyncOp *beginRouterLogin(const QString& username, const QString& password, QNetworkAccessManager *nam, QObject *parent = NULL);
	static AsyncOp *beginRouterConfigurationStart(const QString& action, QNetworkAccessManager *nam, QObject *parent = NULL);
	static AsyncOp *beginRouterGetDnsMasqDeviceId(const QString& action, const QString& macAddress, QNetworkAccessManager *nam, QObject *parent = NULL);
	static AsyncOp *beginRouterConfigurationFinish(const QString& action, QNetworkAccessManager *nam, QObject *parent = NULL);

	// low level
	static AsyncOp *beginSoapPost(const QString& url, const QString& ns, const QString& cmd, const QStringList& paramNames, const QStringList& paramValues, QObject *parent = NULL);
};

#endif // __common_h__
