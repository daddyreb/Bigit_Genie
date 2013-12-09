#ifndef __webapi_h__
#define __webapi_h__

#include <QObject>
#include <QString>
#include <QList>

class AsyncOp;
class QNetworkAccessManager;

class WebApi
	: public QObject
{
public:
	WebApi(QNetworkAccessManager *nam);
	~WebApi();

	QNetworkAccessManager *networkAccessManager() const;

	AsyncOp *beginCheckNameAvailable(const QString& username);
	AsyncOp *beginCreateAccount(const QString& username, const QString& password, const QString& email);
	AsyncOp *beginLogin(const QString& username, const QString& password);
	AsyncOp *beginGetLabel(const QString& token, const QString& deviceId);
	AsyncOp *beginGetDevice(const QString& token, const QString& deviceKey);
	AsyncOp *beginCreateDevice(const QString& token, const QString& deviceKey);
	AsyncOp *beginGetFilters(const QString& token, const QString& deviceId);
	AsyncOp *beginSetFilters(const QString& token, const QString& deviceId, const QStringList& bundle);
	AsyncOp *beginAccountRelay(const QString& token);
	AsyncOp *beginGetUsersForDeviceId(const QString& deviceId);
	AsyncOp *beginGetDeviceChild(const QString& parentDeviceId, const QString& username, const QString& password);
	AsyncOp *beginGetUserForChildDeviceId(const QString& childDeviceId);

private:
	QNetworkAccessManager *m_nam;
};

namespace RouterApiImpl {

class BaseOp;

} // namespace RouterApiImpl

class RouterApi
	: public QObject
{
public:
	RouterApi(QNetworkAccessManager *nam);
	~RouterApi();

	QNetworkAccessManager *networkAccessManager() const;

	AsyncOp *beginConfigurationStart();
	AsyncOp *beginConfigurationFinish();
	AsyncOp *beginGetDNSMasqDeviceID(const QString& macAddress);
	AsyncOp *beginSetDNSMasqDeviceID(const QString& macAddress, const QString& deviceId);
	AsyncOp *beginGetModelName();
	AsyncOp *beginGetParentalControlEnableStatus();
	AsyncOp *beginSetParentalControlEnableStatus(bool enable);
	AsyncOp *beginGetWan();
	AsyncOp *beginAuthenticate(const QString& username, const QString& password);
	AsyncOp *beginGetCurrentSettings();
	AsyncOp *beginDeleteMacAddress(const QString& macAddress);

	void setRouterUsername(const QString& username);
	void setRouterPassword(const QString& password);
	void setSmartNetworkUsername(const QString& username);
	void setSmartNetworkPassword(const QString& password);
	void setControlPointId(const QString& cpid);
	void setFcmlMode(bool fcmlMode);
	void setSmartNetworkBaseUrl(const QString& baseUrl);

private:
	friend class RouterApiImpl::BaseOp;

	QList<int> portList() const;
	void raisePort(int port);

	QNetworkAccessManager *m_nam;
	QList<int> m_portList;

	QString m_domain;
	QString m_uiid;
	QString m_cpid;
	QString m_sessionId;
	QString m_smartNetworkUsername;
	QString m_smartNetworkPassword;
	QString m_routerUsername;
	QString m_routerPassword;
	QString m_smartNetworkBaseUrl;

	bool m_fcmlMode;
};

#endif // __webapi_h__
