#include "webapi.h"
#include "webapi_p1.h"
#include "webapi_p2.h"

AsyncOp *WebApi::beginCheckNameAvailable(const QString& username, QNetworkAccessManager *nam, QObject *parent)
{
	return new WebApiImpl::CheckNameAvailableOp(username, nam, parent);
}

AsyncOp *WebApi::beginCreateAccount(const QString& username, const QString& password, const QString& email, QNetworkAccessManager *nam, QObject *parent)
{
	return new WebApiImpl::CreateAccountOp(username, password, email, nam, parent);
}

AsyncOp *WebApi::beginLogin(const QString& username, const QString& password, QNetworkAccessManager *nam, QObject *parent)
{
	return new WebApiImpl::LoginOp(username, password, nam, parent);
}

AsyncOp *WebApi::beginGetLabel(const QString& token, const QString& deviceId, QNetworkAccessManager *nam, QObject *parent)
{
	return new WebApiImpl::GetLabelOp(token, deviceId, nam, parent);
}

AsyncOp *WebApi::beginGetDevice(const QString& token, const QString& deviceKey, QNetworkAccessManager *nam, QObject *parent)
{
	return new WebApiImpl::GetDeviceOp(token, deviceKey, nam, parent);
}

AsyncOp *WebApi::beginCreateDevice(const QString& token, const QString& deviceKey, QNetworkAccessManager *nam, QObject *parent)
{
	return new WebApiImpl::CreateDeviceOp(token, deviceKey, nam, parent);
}

AsyncOp *WebApi::beginGetFilters(const QString& token, const QString& deviceId, QNetworkAccessManager *nam, QObject *parent)
{
	return new WebApiImpl::GetFiltersOp(token, deviceId, nam, parent);
}

AsyncOp *WebApi::beginSetFilters(const QString& token, const QString& deviceId, const QString& bundle, QNetworkAccessManager *nam, QObject *parent)
{
	return new WebApiImpl::SetFiltersOp(token, deviceId, bundle, nam, parent);
}

AsyncOp *WebApi::beginAccountRelay(const QString& token, QNetworkAccessManager *nam, QObject *parent)
{
	return new WebApiImpl::AccountRelayOp(token, nam, parent);
}

AsyncOp *RouterApi::beginConfigurationStart(const QString& url, QNetworkAccessManager *nam, QObject *parent)
{
	return new RouterApiImpl::ConfigurationStartOp(url, nam, parent);
}

AsyncOp *RouterApi::beginConfigurationFinish(const QString& url, QNetworkAccessManager *nam, QObject *parent)
{
	return new RouterApiImpl::ConfigurationFinishOp(url, nam, parent);
}

AsyncOp *RouterApi::beginGetDNSMasqDeviceID(const QString& macAddress, const QString& url, QNetworkAccessManager *nam, QObject *parent)
{
	return new RouterApiImpl::GetDNSMasqDeviceIDOp(macAddress, url, nam, parent);
}

AsyncOp *RouterApi::beginSetDNSMasqDeviceID(const QString& macAddress, const QString& deviceId, const QString& url, QNetworkAccessManager *nam, QObject *parent)
{
	return new RouterApiImpl::SetDNSMasqDeviceIDOp(macAddress, deviceId, url, nam, parent);
}

AsyncOp *RouterApi::beginGetModelName(const QString& url, QNetworkAccessManager *nam, QObject *parent)
{
	return new RouterApiImpl::GetModelNameOp(url, nam, parent);
}

AsyncOp *RouterApi::beginGetParentalControlEnableStatus(const QString& url, QNetworkAccessManager *nam, QObject *parent)
{
	return new RouterApiImpl::GetParentalControlEnableStatusOp(url, nam, parent);
}

AsyncOp *RouterApi::beginSetParentalControlEnableStatus(bool enable, const QString& url, QNetworkAccessManager *nam, QObject *parent)
{
	return new RouterApiImpl::SetParentalControlEnableStatusOp(enable, url, nam, parent);
}

AsyncOp *RouterApi::beginGetWan(const QString& url, QNetworkAccessManager *nam, QObject *parent)
{
	return new RouterApiImpl::GetWanOp(url, nam, parent);
}

AsyncOp *RouterApi::beginAuthenticate(const QString& username, const QString& password, const QString& url, QNetworkAccessManager *nam, QObject *parent)
{
	return new RouterApiImpl::AuthenticateOp(username, password, url, nam, parent);
}

AsyncOp *RouterApi::beginGetCurrentSettings(QNetworkAccessManager *nam, QObject *parent)
{
	return new RouterApiImpl::GetCurrentSettingsOp(nam, parent);
}
