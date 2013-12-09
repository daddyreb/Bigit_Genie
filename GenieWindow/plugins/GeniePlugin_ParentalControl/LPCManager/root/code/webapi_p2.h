#ifndef __webapi_p2_h__
#define __webapi_p2_h__

#include "oputil.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

namespace WebApiImpl {

class CheckNameAvailableOp
	: public BaseOp
{
public:
	CheckNameAvailableOp(const QString& username, QNetworkAccessManager *nam, QObject *parent);

protected:
	virtual int processImpl(const QVariantMap& result);
};

class CreateAccountOp
	: public BaseOp
{
public:
	CreateAccountOp(const QString& username, const QString& password, const QString& email, QNetworkAccessManager *nam, QObject *parent);

protected:
	virtual int processImpl(const QVariantMap& result);
};

class LoginOp
	: public BaseOp
{
public:
	LoginOp(const QString& username, const QString& password, QNetworkAccessManager *nam, QObject *parent);

protected:
	virtual int processImpl(const QVariantMap& result);
};

class GetLabelOp
	: public BaseOp
{
public:
	GetLabelOp(const QString& token, const QString& deviceId, QNetworkAccessManager *nam, QObject *parent);

protected:
	virtual int processImpl(const QVariantMap& result);
};

class GetDeviceOp
	: public BaseOp
{
public:
	GetDeviceOp(const QString& token, const QString& deviceKey, QNetworkAccessManager *nam, QObject *parent);

protected:
	virtual int processImpl(const QVariantMap& result);
};

class CreateDeviceOp
	: public BaseOp
{
public:
	CreateDeviceOp(const QString& token, const QString& deviceKey, QNetworkAccessManager *nam, QObject *parent);

protected:
	virtual int processImpl(const QVariantMap& result);
};

class GetFiltersOp
	: public BaseOp
{
public:
	GetFiltersOp(const QString& token, const QString& deviceId, QNetworkAccessManager *nam, QObject *parent);

protected:
	virtual int processImpl(const QVariantMap& result);
};

class SetFiltersOp
	: public BaseOp
{
public:
	SetFiltersOp(const QString& token, const QString& deviceId, const QString& bundle, QNetworkAccessManager *nam, QObject *parent);

protected:
	virtual int processImpl(const QVariantMap& result);
};

class AccountRelayOp
	: public BaseOp
{
public:
	AccountRelayOp(const QString& token, QNetworkAccessManager *nam, QObject *parent);

protected:
	virtual int processImpl(const QVariantMap& result);
};

} // namespace WebApiImpl

namespace RouterApiImpl {

class ConfigurationStartOp
	: public BaseOp
{
public:
	ConfigurationStartOp(const QString& url, QNetworkAccessManager *nam, QObject *parent);

protected:
	virtual int processImpl(const QByteArray& data, const QVariantMap& result);
};

class ConfigurationFinishOp
	: public BaseOp
{
public:
	ConfigurationFinishOp(const QString& url, QNetworkAccessManager *nam, QObject *parent);

protected:
	virtual int processImpl(const QByteArray& data, const QVariantMap& result);
};

class GetDNSMasqDeviceIDOp
	: public BaseOp
{
public:
	GetDNSMasqDeviceIDOp(const QString& macAddress, const QString& url, QNetworkAccessManager *nam, QObject *parent);

protected:
	virtual int processImpl(const QByteArray& data, const QVariantMap& result);
};

class SetDNSMasqDeviceIDOp
	: public BaseOp
{
public:
	SetDNSMasqDeviceIDOp(const QString& macAddress, const QString& deviceId, const QString& url, QNetworkAccessManager *nam, QObject *parent);

protected:
	virtual int processImpl(const QByteArray& data, const QVariantMap& result);
};

class GetModelNameOp
	: public BaseOp
{
public:
	GetModelNameOp(const QString& url, QNetworkAccessManager *nam, QObject *parent);

protected:
	virtual int processImpl(const QByteArray& data, const QVariantMap& result);
};

class GetParentalControlEnableStatusOp
	: public BaseOp
{
public:
	GetParentalControlEnableStatusOp(const QString& url, QNetworkAccessManager *nam, QObject *parent);

protected:
	virtual int processImpl(const QByteArray& data, const QVariantMap& result);
};

class SetParentalControlEnableStatusOp
	: public BaseOp
{
public:
	SetParentalControlEnableStatusOp(bool enable, const QString& url, QNetworkAccessManager *nam, QObject *parent);

protected:
	virtual int processImpl(const QByteArray& data, const QVariantMap& result);
};

class GetWanOp
	: public BaseOp
{
public:
	GetWanOp(const QString& url, QNetworkAccessManager *nam, QObject *parent);

protected:
	virtual int processImpl(const QByteArray& data, const QVariantMap& result);
};

class AuthenticateOp
	: public BaseOp
{
public:
	AuthenticateOp(const QString& username, const QString& password, const QString& url, QNetworkAccessManager *nam, QObject *parent);

protected:
	virtual int processImpl(const QByteArray& data, const QVariantMap& result);
};

class GetCurrentSettingsOp
	: public SimpleOp
{
	Q_OBJECT

public:
	GetCurrentSettingsOp(QNetworkAccessManager *nam, QObject *parent);
	virtual ~GetCurrentSettingsOp();

protected:
	virtual void startImpl();
	virtual void abortImpl();

private slots:
	void requestFinished();

private:
	QNetworkAccessManager *m_nam;
	QNetworkReply *m_reply;
};

} // namespace RouterApiImpl

#endif // __webapi_p2_h__
