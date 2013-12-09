#ifndef __coreapi_h__
#define __coreapi_h__

#include "webapi.h"
#include <QObject>
#include <QString>

class AsyncOp;
class QNetworkAccessManager;

class CoreApi
	: public QObject
{
public:
	CoreApi(QNetworkAccessManager *nam);
	~CoreApi();

	QNetworkAccessManager *networkAccessManager() const;
	RouterApi *routerApi() const;
	WebApi *webApi() const;
	bool isFcmlMode() const;

	AsyncOp *beginLookupHost(const QString& hostname);
	AsyncOp *beginAutoRouterAuthenticate(const QString& username, const QString& password);
	AsyncOp *beginCheckRouterStatus(const QString& username, const QString& password);
	AsyncOp *beginGetSetDeviceId(const QString& userToken, const QString& routerUsername, const QString& routerPassword);
	AsyncOp *beginSignIn(const QString& username, const QString& password, const QString& routerUsername, const QString& routerPassword);
	AsyncOp *beginQuerySettings(const QString& userToken, const QString& deviceId, const QString& routerUsername, const QString& routerPassword);
	AsyncOp *beginToggleParentalControl(bool enable, const QString& deviceId);
	AsyncOp *beginSetFiltersAndEnableParentalControl(const QString& token, const QString& deviceId, const QStringList& bundle);
	AsyncOp *beginSSLTest(const QString& url);

	AsyncOp *beginDetectRouter();

	AsyncOp *beginWrappedGetDNSMasqDeviceID(const QString& macAddress);
	AsyncOp *beginWrappedSetDNSMasqDeviceID(const QString& macAddress, const QString& deviceId);
	AsyncOp *beginWrappedGetParentalControlEnableStatus();
	AsyncOp *beginWrappedSetParentalControlEnableStatus(bool enable);
	AsyncOp *beginWrappedDeleteMacAddress(const QString& macAddress);

	AsyncOp *beginAuthenticateAndGetDeviceID(const QString& routerUsername, const QString& routerPassword);//kai.yan add
	AsyncOp *beginAuthenticateAndDeleteMacAddress(const QString& routerUsername, const QString& routerPassword, const QString& macAddress);//kai.yan add

	void setRouterUsername(const QString& username);
	void setRouterPassword(const QString& password);
	void setSmartNetworkUsername(const QString& username);
	void setSmartNetworkPassword(const QString& password);
	void setControlPointId(const QString& cpid);
	void setFcmlMode(bool fcmlMode);

private:
	QNetworkAccessManager *m_nam;
	RouterApi *m_routerApi;
	WebApi *m_webApi;
	bool m_fcmlMode;
};

#endif // __coreapi_h__
