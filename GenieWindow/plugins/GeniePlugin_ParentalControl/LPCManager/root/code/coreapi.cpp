#include "coreapi.h"
#include "coreapi_p1.h"

AsyncOp *CoreApi::beginLookupHost(const QString& hostname, QObject *parent)
{
	return new CoreApiImpl::LookupHostOp(hostname, parent);
}

AsyncOp *CoreApi::beginAutoRouterAuthenticate(const QString& username, const QString& password, QNetworkAccessManager *nam, QObject *parent)
{
	return new CoreApiImpl::AutoRouterAuthenticateOp(username, password, nam, parent);
}

AsyncOp *CoreApi::beginCheckRouterStatus(const QString& username, const QString& password, QNetworkAccessManager *nam, QObject *parent)
{
	return new CoreApiImpl::CheckRouterStatusOp(username, password, nam, parent);
}

AsyncOp *CoreApi::beginGetSetDeviceId(const QString& userToken, const QString& routerUsername, const QString& routerPassword, QNetworkAccessManager *nam, QObject *parent)
{
	return new CoreApiImpl::GetSetDeviceIdOp(userToken, routerUsername, routerPassword, nam, parent);
}

AsyncOp *CoreApi::beginSignIn(const QString& username, const QString& password, const QString& routerUsername, const QString& routerPassword, QNetworkAccessManager *nam, QObject *parent)
{
	return new CoreApiImpl::SignInOp(username, password, routerUsername, routerPassword, nam, parent);
}

AsyncOp *CoreApi::beginQuerySettings(const QString& userToken, const QString& deviceId, const QString& routerUsername, const QString& routerPassword, QNetworkAccessManager *nam, QObject *parent)
{
	return new CoreApiImpl::QuerySettingsOp(userToken, deviceId, routerUsername, routerPassword, nam, parent);
}

AsyncOp *CoreApi::beginToggleParentalControl(bool enable, const QString& deviceId, const QString& url, QNetworkAccessManager *nam, QObject *parent)
{
	return new CoreApiImpl::ToggleParentalControlOp(enable, deviceId, url, nam, parent);
}

AsyncOp *CoreApi::beginSetFiltersAndEnableParentalControl(const QString& url, const QString& token, const QString& deviceId, const QString& bundle, QNetworkAccessManager *nam, QObject *parent)
{
	return new CoreApiImpl::SetFiltersAndEnableParentalControlOp(url, token, deviceId, bundle, nam, parent);
}

AsyncOp *CoreApi::beginSSLTest(const QString& url, QNetworkAccessManager *nam, QObject *parent)
{
	return new CoreApiImpl::SSLTestOp(url, nam, parent);
}

AsyncOp *CoreApi::beginWrappedGetDNSMasqDeviceID(const QString& macAddress, const QString& url, QNetworkAccessManager *nam, QObject *parent)
{
	return new CoreApiImpl::WrappedGetDNSMasqDeviceID(macAddress, url, nam, parent);
}

AsyncOp *CoreApi::beginWrappedSetDNSMasqDeviceID(const QString& macAddress, const QString& deviceId, const QString& url, QNetworkAccessManager *nam, QObject *parent)
{
	return new CoreApiImpl::WrappedSetDNSMasqDeviceID(macAddress, deviceId, url, nam, parent);
}

AsyncOp *CoreApi::beginWrappedGetParentalControlEnableStatus(const QString& url, QNetworkAccessManager *nam, QObject *parent)
{
	return new CoreApiImpl::WrappedGetParentalControlEnableStatus(url, nam, parent);
}

AsyncOp *CoreApi::beginWrappedSetParentalControlEnableStatus(bool enable, const QString& url, QNetworkAccessManager *nam, QObject *parent)
{
	return new CoreApiImpl::WrappedSetParentalControlEnableStatus(enable, url, nam, parent);
}
