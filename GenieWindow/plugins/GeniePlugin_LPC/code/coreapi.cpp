#include "coreapi.h"
#include "coreapi_p1.h"

CoreApi::CoreApi(QNetworkAccessManager *nam)
	: m_nam(nam)
{
	m_routerApi = new RouterApi(m_nam);
	m_webApi = new WebApi(m_nam);
	m_fcmlMode = false;
}

CoreApi::~CoreApi()
{
	delete m_routerApi;
	delete m_webApi;
}

void CoreApi::setRouterUsername(const QString& username)
{
	m_routerApi->setRouterUsername(username);
}

void CoreApi::setRouterPassword(const QString& password)
{
	m_routerApi->setRouterPassword(password);
}

void CoreApi::setSmartNetworkUsername(const QString& username)
{
	m_routerApi->setSmartNetworkUsername(username);
}

void CoreApi::setSmartNetworkPassword(const QString& password)
{
	m_routerApi->setSmartNetworkPassword(password);
}

void CoreApi::setControlPointId(const QString& cpid)
{
	m_routerApi->setControlPointId(cpid);
}

void CoreApi::setFcmlMode(bool fcmlMode)
{
	m_routerApi->setFcmlMode(fcmlMode);
	m_fcmlMode = fcmlMode;
}

QNetworkAccessManager *CoreApi::networkAccessManager() const
{
	return m_nam;
}

RouterApi *CoreApi::routerApi() const
{
	return m_routerApi;
}

WebApi *CoreApi::webApi() const
{
	return m_webApi;
}

bool CoreApi::isFcmlMode() const
{
	return m_fcmlMode;
}

AsyncOp *CoreApi::beginLookupHost(const QString& hostname)
{
	return new CoreApiImpl::LookupHostOpV2(hostname, this);
}

AsyncOp *CoreApi::beginAutoRouterAuthenticate(const QString& username, const QString& password)
{
	return new CoreApiImpl::AutoRouterAuthenticateOp(username, password, this);
}

AsyncOp *CoreApi::beginCheckRouterStatus(const QString& username, const QString& password)
{
	return new CoreApiImpl::CheckRouterStatusOp(username, password, this);
}

AsyncOp *CoreApi::beginGetSetDeviceId(const QString& userToken, const QString& routerUsername, const QString& routerPassword)
{
	return new CoreApiImpl::GetSetDeviceIdOp(userToken, routerUsername, routerPassword, this);
}

AsyncOp *CoreApi::beginSignIn(const QString& username, const QString& password, const QString& routerUsername, const QString& routerPassword)
{
	return new CoreApiImpl::SignInOp(username, password, routerUsername, routerPassword, this);
}

AsyncOp *CoreApi::beginQuerySettings(const QString& userToken, const QString& deviceId, const QString& routerUsername, const QString& routerPassword)
{
	return new CoreApiImpl::QuerySettingsOp(userToken, deviceId, routerUsername, routerPassword, this);
}

AsyncOp *CoreApi::beginToggleParentalControl(bool enable, const QString& deviceId)
{
	return new CoreApiImpl::ToggleParentalControlOp(enable, deviceId, this);
}

AsyncOp *CoreApi::beginSetFiltersAndEnableParentalControl(const QString& token, const QString& deviceId, const QStringList& bundle)
{
	return new CoreApiImpl::SetFiltersAndEnableParentalControlOp(token, deviceId, bundle, this);
}

AsyncOp *CoreApi::beginSSLTest(const QString& url)
{
	return new CoreApiImpl::SSLTestOp(url, this);
}

AsyncOp *CoreApi::beginDetectRouter()
{
	return new CoreApiImpl::DetectRouterOp(this);
}

AsyncOp *CoreApi::beginWrappedGetDNSMasqDeviceID(const QString& macAddress)
{
	return new CoreApiImpl::WrappedGetDNSMasqDeviceID(macAddress, this);
}

AsyncOp *CoreApi::beginWrappedSetDNSMasqDeviceID(const QString& macAddress, const QString& deviceId)
{
	return new CoreApiImpl::WrappedSetDNSMasqDeviceID(macAddress, deviceId, this);
}

AsyncOp *CoreApi::beginWrappedGetParentalControlEnableStatus()
{
	return new CoreApiImpl::WrappedGetParentalControlEnableStatus(this);
}

AsyncOp *CoreApi::beginWrappedSetParentalControlEnableStatus(bool enable)
{
	return new CoreApiImpl::WrappedSetParentalControlEnableStatus(enable, this);
}

AsyncOp *CoreApi::beginWrappedDeleteMacAddress(const QString& macAddress)
{
	return new CoreApiImpl::WrappedDeleteMacAddress(macAddress, this);
}

///认证并获取DeviceID
AsyncOp *CoreApi::beginAuthenticateAndGetDeviceID(const QString& routerUsername, const QString& routerPassword)
{
	return new CoreApiImpl::AuthenticateAndGetDeviceIDOp(routerUsername, routerPassword, this);
}

///认证并Delete mac address
AsyncOp *CoreApi::beginAuthenticateAndDeleteMacAddress(const QString& routerUsername, const QString& routerPassword, const QString& macAddress)
{
	return new CoreApiImpl::AuthenticateAndDeleteMacAddressOp(routerUsername, routerPassword, macAddress, this);
}