#ifndef __coreapi_h__
#define __coreapi_h__

#include <QObject>
#include <QString>

class AsyncOp;
class QNetworkAccessManager;

class CoreApi
{
public:
	static AsyncOp *beginLookupHost(const QString& hostname, QObject *parent = NULL);
	static AsyncOp *beginAutoRouterAuthenticate(const QString& username, const QString& password, QNetworkAccessManager *nam, QObject *parent = NULL);
	static AsyncOp *beginCheckRouterStatus(const QString& username, const QString& password, QNetworkAccessManager *nam, QObject *parent = NULL);
	static AsyncOp *beginGetSetDeviceId(const QString& userToken, const QString& routerUsername, const QString& routerPassword, QNetworkAccessManager *nam, QObject *parent = NULL);
	static AsyncOp *beginSignIn(const QString& username, const QString& password, const QString& routerUsername, const QString& routerPassword, QNetworkAccessManager *nam, QObject *parent = NULL);
	static AsyncOp *beginQuerySettings(const QString& userToken, const QString& deviceId, const QString& routerUsername, const QString& routerPassword, QNetworkAccessManager *nam, QObject *parent = NULL);
	static AsyncOp *beginToggleParentalControl(bool enable, const QString& deviceId, const QString& url, QNetworkAccessManager *nam, QObject *parent = NULL);
	static AsyncOp *beginSetFiltersAndEnableParentalControl(const QString& url, const QString& token, const QString& deviceId, const QString& bundle, QNetworkAccessManager *nam, QObject *parent = NULL);
	static AsyncOp *beginSSLTest(const QString& url, QNetworkAccessManager *nam, QObject *parent = NULL);

	static AsyncOp *beginWrappedGetDNSMasqDeviceID(const QString& macAddress, const QString& url, QNetworkAccessManager *nam, QObject *parent = NULL);
	static AsyncOp *beginWrappedSetDNSMasqDeviceID(const QString& macAddress, const QString& deviceId, const QString& url, QNetworkAccessManager *nam, QObject *parent = NULL);
	static AsyncOp *beginWrappedGetParentalControlEnableStatus(const QString& url, QNetworkAccessManager *nam, QObject *parent = NULL);
	static AsyncOp *beginWrappedSetParentalControlEnableStatus(bool enable, const QString& url, QNetworkAccessManager *nam, QObject *parent = NULL);
};

#endif // __coreapi_h__
