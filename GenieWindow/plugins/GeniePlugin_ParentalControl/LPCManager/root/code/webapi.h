#ifndef __webapi_h__
#define __webapi_h__

#include <QObject>
#include <QString>

class AsyncOp;
class QNetworkAccessManager;

class WebApi
{
public:
	static AsyncOp *beginCheckNameAvailable(const QString& username, QNetworkAccessManager *nam, QObject *parent = NULL);
	static AsyncOp *beginCreateAccount(const QString& username, const QString& password, const QString& email, QNetworkAccessManager *nam, QObject *parent = NULL);
	static AsyncOp *beginLogin(const QString& username, const QString& password, QNetworkAccessManager *nam, QObject *parent = NULL);
	static AsyncOp *beginGetLabel(const QString& token, const QString& deviceId, QNetworkAccessManager *nam, QObject *parent = NULL);
	static AsyncOp *beginGetDevice(const QString& token, const QString& deviceKey, QNetworkAccessManager *nam, QObject *parent = NULL);
	static AsyncOp *beginCreateDevice(const QString& token, const QString& deviceKey, QNetworkAccessManager *nam, QObject *parent = NULL);
	static AsyncOp *beginGetFilters(const QString& token, const QString& deviceId, QNetworkAccessManager *nam, QObject *parent = NULL);
	static AsyncOp *beginSetFilters(const QString& token, const QString& deviceId, const QString& bundle, QNetworkAccessManager *nam, QObject *parent = NULL);
	static AsyncOp *beginAccountRelay(const QString& token, QNetworkAccessManager *nam, QObject *parent = NULL);
};

class RouterApi
{
public:
	static AsyncOp *beginConfigurationStart(const QString& url, QNetworkAccessManager *nam, QObject *parent = NULL);
	static AsyncOp *beginConfigurationFinish(const QString& url, QNetworkAccessManager *nam, QObject *parent = NULL);
	static AsyncOp *beginGetDNSMasqDeviceID(const QString& macAddress, const QString& url, QNetworkAccessManager *nam, QObject *parent = NULL);
	static AsyncOp *beginSetDNSMasqDeviceID(const QString& macAddress, const QString& deviceId, const QString& url, QNetworkAccessManager *nam, QObject *parent = NULL);
	static AsyncOp *beginGetModelName(const QString& url, QNetworkAccessManager *nam, QObject *parent = NULL);
	static AsyncOp *beginGetParentalControlEnableStatus(const QString& url, QNetworkAccessManager *nam, QObject *parent = NULL);
	static AsyncOp *beginSetParentalControlEnableStatus(bool enable, const QString& url, QNetworkAccessManager *nam, QObject *parent = NULL);
	static AsyncOp *beginGetWan(const QString& url, QNetworkAccessManager *nam, QObject *parent = NULL);
	static AsyncOp *beginAuthenticate(const QString& username, const QString& password, const QString& url, QNetworkAccessManager *nam, QObject *parent = NULL);
	static AsyncOp *beginGetCurrentSettings(QNetworkAccessManager *nam, QObject *parent = NULL);
};

#endif // __webapi_h__
