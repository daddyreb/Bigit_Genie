#include "webapi.h"
#include "webapi_p2.h"

WebApi::WebApi(QNetworkAccessManager *nam)
	: m_nam(nam)
{
}

WebApi::~WebApi()
{
}

QNetworkAccessManager *WebApi::networkAccessManager() const
{
	return m_nam;
}

AsyncOp *WebApi::beginCheckNameAvailable(const QString& username)
{
	return new WebApiImpl::CheckNameAvailableOp(username, this);
}

AsyncOp *WebApi::beginCreateAccount(const QString& username, const QString& password, const QString& email)
{
	return new WebApiImpl::CreateAccountOp(username, password, email, this);
}

AsyncOp *WebApi::beginLogin(const QString& username, const QString& password)
{
	return new WebApiImpl::LoginOp(username, password, this);
}

AsyncOp *WebApi::beginGetLabel(const QString& token, const QString& deviceId)
{
	return new WebApiImpl::GetLabelOp(token, deviceId, this);
}

AsyncOp *WebApi::beginGetDevice(const QString& token, const QString& deviceKey)
{
	return new WebApiImpl::GetDeviceOp(token, deviceKey, this);
}

AsyncOp *WebApi::beginCreateDevice(const QString& token, const QString& deviceKey)
{
	return new WebApiImpl::CreateDeviceOp(token, deviceKey, this);
}

AsyncOp *WebApi::beginGetFilters(const QString& token, const QString& deviceId)
{
	return new WebApiImpl::GetFiltersOp(token, deviceId, this);
}

AsyncOp *WebApi::beginSetFilters(const QString& token, const QString& deviceId, const QStringList& bundle)
{
	return new WebApiImpl::SetFiltersOp(token, deviceId, bundle, this);
}

AsyncOp *WebApi::beginAccountRelay(const QString& token)
{
	return new WebApiImpl::AccountRelayOp(token, this);
}

AsyncOp *WebApi::beginGetUsersForDeviceId(const QString& deviceId)
{
	return new WebApiImpl::GetUsersForDeviceIdOp(deviceId, this);
}

AsyncOp *WebApi::beginGetDeviceChild(const QString& parentDeviceId, const QString& username, const QString& password)
{
	return new WebApiImpl::GetDeviceChildOp(parentDeviceId, username, password, this);
}

AsyncOp *WebApi::beginGetUserForChildDeviceId(const QString& childDeviceId)
{
	return new WebApiImpl::GetUserForChildDeviceIdOp(childDeviceId, this);
}

RouterApi::RouterApi(QNetworkAccessManager *nam)
	: m_nam(nam)
{
	m_portList.append(5000);
	m_portList.append(80);

	m_domain = QLatin1String("portal");
	//m_cpid = QLatin1String("cp171");
	//m_uiid = QLatin1String("ui3324");
	//m_smartNetworkUsername = QLatin1String("larkvm@gmail.com");
	//m_smartNetworkPassword = QLatin1String("12345678");
	//m_routerUsername = QLatin1String("admin");
	//m_routerPassword = QLatin1String("lenovot61p");
	//m_sessionId = QLatin1String("50f648aa3134784b");
	m_smartNetworkBaseUrl = QLatin1String("https://appgenie.netgear.com");

	m_fcmlMode = false;
}

RouterApi::~RouterApi()
{
}

void RouterApi::setRouterUsername(const QString& username)
{
	m_routerUsername = username;
}

void RouterApi::setRouterPassword(const QString& password)
{
	m_routerPassword = password;
}

void RouterApi::setSmartNetworkUsername(const QString& username)
{
	m_smartNetworkUsername = username;
}

void RouterApi::setSmartNetworkPassword(const QString& password)
{
	m_smartNetworkPassword = password;
}

void RouterApi::setControlPointId(const QString& cpid)
{
	m_cpid = cpid;
}

void RouterApi::setFcmlMode(bool fcmlMode)
{
	m_fcmlMode = fcmlMode;
}

void RouterApi::setSmartNetworkBaseUrl(const QString& baseUrl)
{
	m_smartNetworkBaseUrl = baseUrl;
	if (m_smartNetworkBaseUrl.indexOf(QLatin1String("://")) < 0) {
		m_smartNetworkBaseUrl = QLatin1String("https://") + m_smartNetworkBaseUrl;
	}
}

QNetworkAccessManager *RouterApi::networkAccessManager() const
{
	return m_nam;
}

QList<int> RouterApi::portList() const
{
	return m_portList;
}

void RouterApi::raisePort(int port)
{
	if (m_portList.front() != port) {
		m_portList.removeAll(port);
		m_portList.push_front(port);
	}
}

AsyncOp *RouterApi::beginConfigurationStart()
{
	return new RouterApiImpl::ConfigurationStartOp(this);
}

AsyncOp *RouterApi::beginConfigurationFinish()
{
	return new RouterApiImpl::ConfigurationFinishOp(this);
}

AsyncOp *RouterApi::beginGetDNSMasqDeviceID(const QString& macAddress)
{
	return new RouterApiImpl::GetDNSMasqDeviceIDOp(macAddress, this);
}

AsyncOp *RouterApi::beginSetDNSMasqDeviceID(const QString& macAddress, const QString& deviceId)
{
	return new RouterApiImpl::SetDNSMasqDeviceIDOp(macAddress, deviceId, this);
}

AsyncOp *RouterApi::beginGetModelName()
{
	return new RouterApiImpl::GetModelNameOp(this);
}

AsyncOp *RouterApi::beginGetParentalControlEnableStatus()
{
	return new RouterApiImpl::GetParentalControlEnableStatusOp(this);
}

AsyncOp *RouterApi::beginSetParentalControlEnableStatus(bool enable)
{
	return new RouterApiImpl::SetParentalControlEnableStatusOp(enable, this);
}

AsyncOp *RouterApi::beginGetWan()
{
	return new RouterApiImpl::GetWanOp(this);
}

AsyncOp *RouterApi::beginAuthenticate(const QString& username, const QString& password)
{
	return new RouterApiImpl::AuthenticateOp(username, password, this);
}

AsyncOp *RouterApi::beginGetCurrentSettings()
{
	return new RouterApiImpl::GetCurrentSettingsOp(this);
}

AsyncOp *RouterApi::beginDeleteMacAddress(const QString& macAddress)
{
	return new RouterApiImpl::DeleteMacAddressOp(macAddress, this);
}
