#ifndef __webapi_p2_h__
#define __webapi_p2_h__

#include "oputil.h"
#include "webapi.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QList>

namespace WebApiImpl {

class BaseOp
	: public SimpleOp
{
	Q_OBJECT

public:
	BaseOp(WebApi *owner);
	virtual ~BaseOp();

protected:
	virtual void startImpl();
	virtual void abortImpl();
	virtual int processImpl(const QVariantMap& result) = 0;

	QByteArray m_command;
	QStringList m_paramNames;
	QStringList m_paramValues;

private slots:
	void stagePostFinished();

private:
	enum Stage {
		Stage_Constructed,
		Stage_Post,
	};

	Stage m_stage;
	WebApi *m_owner;
	QNetworkReply *m_reply;
};

class CheckNameAvailableOp
	: public BaseOp
{
public:
	CheckNameAvailableOp(const QString& username, WebApi *owner);

protected:
	virtual int processImpl(const QVariantMap& result);
};

class CreateAccountOp
	: public BaseOp
{
public:
	CreateAccountOp(const QString& username, const QString& password, const QString& email, WebApi *owner);

protected:
	virtual int processImpl(const QVariantMap& result);
};

class LoginOp
	: public BaseOp
{
public:
	LoginOp(const QString& username, const QString& password, WebApi *owner);

protected:
	virtual int processImpl(const QVariantMap& result);
};

class GetLabelOp
	: public BaseOp
{
public:
	GetLabelOp(const QString& token, const QString& deviceId, WebApi *owner);

protected:
	virtual int processImpl(const QVariantMap& result);
};

class GetDeviceOp
	: public BaseOp
{
public:
	GetDeviceOp(const QString& token, const QString& deviceKey, WebApi *owner);

protected:
	virtual int processImpl(const QVariantMap& result);
};

class CreateDeviceOp
	: public BaseOp
{
public:
	CreateDeviceOp(const QString& token, const QString& deviceKey, WebApi *owner);

protected:
	virtual int processImpl(const QVariantMap& result);
};

class GetFiltersOp
	: public BaseOp
{
public:
	GetFiltersOp(const QString& token, const QString& deviceId, WebApi *owner);

protected:
	virtual int processImpl(const QVariantMap& result);
};

class SetFiltersOp
	: public BaseOp
{
public:
	SetFiltersOp(const QString& token, const QString& deviceId, const QStringList& bundle, WebApi *owner);

protected:
	virtual int processImpl(const QVariantMap& result);
};

class AccountRelayOp
	: public BaseOp
{
public:
	AccountRelayOp(const QString& token, WebApi *owner);

protected:
	virtual int processImpl(const QVariantMap& result);
};

class GetUsersForDeviceIdOp
	: public BaseOp
{
public:
	GetUsersForDeviceIdOp(const QString& deviceId, WebApi *owner);

protected:
	virtual int processImpl(const QVariantMap& result);
};

class GetDeviceChildOp
	: public BaseOp
{
public:
	GetDeviceChildOp(const QString& parentDeviceId, const QString& username, const QString& password, WebApi *owner);

protected:
	virtual int processImpl(const QVariantMap& result);
};

class GetUserForChildDeviceIdOp
	: public BaseOp
{
public:
	GetUserForChildDeviceIdOp(const QString& childDeviceId, WebApi *owner);

protected:
	virtual int processImpl(const QVariantMap& result);
};

} // namespace WebApiImpl

namespace RouterApiImpl {

#if 0

class BaseOp
	: public SimpleOp
{
	Q_OBJECT

public:
	BaseOp(RouterApi *owner);
	virtual ~BaseOp();

protected:
	virtual void startImpl();
	virtual void abortImpl();
	virtual int processImpl(const QByteArray& data, const QVariantMap& result) = 0;

	QByteArray m_command;
	QByteArray m_commandResponse;
	QByteArray m_namespace;
	QStringList m_paramNames;
	QStringList m_paramValues;

private slots:
	void stagePostFinished();

private:
	bool parseResultXml(const QByteArray& data, QVariantMap& result);

private:
	enum Stage {
		Stage_Constructed,
		Stage_Post,
	};

	Stage m_stage;
	RouterApi *m_owner;
	QNetworkReply *m_reply;
	QList<int> m_portList;
	int m_activePort;
};

#else

class HttpGetPostOp
	: public AsyncOp
{
	Q_OBJECT

public:
	static AsyncOp *startGet(QNetworkAccessManager *nam, const QString& url);
	static AsyncOp *startPost(QNetworkAccessManager *nam, const QString& url, const QByteArray& data);

protected:
	HttpGetPostOp(QNetworkAccessManager *nam, const QString& url);
	HttpGetPostOp(QNetworkAccessManager *nam, const QString& url, const QByteArray& data);
	virtual ~HttpGetPostOp();
	virtual void abortImpl();
	void start();

private Q_SLOTS:
	void onReplyFinished();

private:
	QNetworkAccessManager *m_nam;
	QString m_url;
	QByteArray m_data;
	bool m_post;
	QNetworkReply *m_reply;
};

class InitAuthOp
	: public AsyncOp
{
	Q_OBJECT

public:
	static AsyncOp *start(QNetworkAccessManager *nam, const QString& baseUrl, const QString& username, const QString& password);

protected:
	InitAuthOp(QNetworkAccessManager *nam, const QString& baseUrl, const QString& username, const QString& password);
	virtual ~InitAuthOp();
	void startOp();
	virtual void abortImpl();

private Q_SLOTS:
	void onAuthOpFinished();
	void onInitOpFinished();

private:
	QNetworkAccessManager *m_nam;
	AsyncOp *m_op;
	QString m_xml;
	QString m_baseUrl;
};

class FcmlOp
	: public AsyncOp
{
	Q_OBJECT

public:
	static AsyncOp *start(QNetworkAccessManager *nam, const QString& baseUrl, const QString& toA, const QString& toB, const QString& fromA, const QString& fromB, const QString& object, const QString& method, const QStringList& names, const QStringList& values);

protected:
	FcmlOp(QNetworkAccessManager *nam, const QString& baseUrl, const QString& toA, const QString& toB, const QString& fromA, const QString& fromB, const QString& object, const QString& method, const QStringList& names, const QStringList& values);
	virtual ~FcmlOp();
	void startOp();
	virtual void abortImpl();

private Q_SLOTS:
	void onOpFinished();
	void onReceiveOpFinished();

private:
	void receiveNext();

private:
	QNetworkAccessManager *m_nam;
	QString m_fcml;
	QString m_baseUrl;
	QString m_fromA;
	QString m_methodTagName;
	AsyncOp *m_op;
};

class BaseOp
	: public SimpleOp
{
	Q_OBJECT

public:
	BaseOp(RouterApi *owner);
	virtual ~BaseOp();

protected:
	virtual void startImpl();
	virtual void abortImpl();
	virtual int processImpl(const QByteArray& data, const QVariantMap& result) = 0;

private Q_SLOTS:
	void onSoapFinished();
	void onFcmlFinished();
	void onInitAuthFinished();
	void onStartSessionFinished();

private:
	void startFcml();
	void startSoap();
	bool parseResultXml(const QByteArray& data, QVariantMap& result);

protected:
	QByteArray m_command;
	QByteArray m_commandResponse;
	QByteArray m_namespace;
	QStringList m_paramNames;
	QStringList m_paramValues;

private:
	RouterApi *m_owner;
	AsyncOp *m_op;

	QNetworkReply *m_reply;
	QList<int> m_portList;
	int m_activePort;
	bool m_fcmlMode;
};

#endif

class ConfigurationStartOp
	: public BaseOp
{
public:
	ConfigurationStartOp(RouterApi *owner);

protected:
	virtual int processImpl(const QByteArray& data, const QVariantMap& result);
};

class ConfigurationFinishOp
	: public BaseOp
{
public:
	ConfigurationFinishOp(RouterApi *owner);

protected:
	virtual int processImpl(const QByteArray& data, const QVariantMap& result);
};

class GetDNSMasqDeviceIDOp
	: public BaseOp
{
public:
	GetDNSMasqDeviceIDOp(const QString& macAddress, RouterApi *owner);

protected:
	virtual int processImpl(const QByteArray& data, const QVariantMap& result);
};

class SetDNSMasqDeviceIDOp
	: public BaseOp
{
public:
	SetDNSMasqDeviceIDOp(const QString& macAddress, const QString& deviceId, RouterApi *owner);

protected:
	virtual int processImpl(const QByteArray& data, const QVariantMap& result);
};

class GetModelNameOp
	: public BaseOp
{
public:
	GetModelNameOp(RouterApi *owner);

protected:
	virtual int processImpl(const QByteArray& data, const QVariantMap& result);
};

class GetParentalControlEnableStatusOp
	: public BaseOp
{
public:
	GetParentalControlEnableStatusOp(RouterApi *owner);

protected:
	virtual int processImpl(const QByteArray& data, const QVariantMap& result);
};

class SetParentalControlEnableStatusOp
	: public BaseOp
{
public:
	SetParentalControlEnableStatusOp(bool enable, RouterApi *owner);

protected:
	virtual int processImpl(const QByteArray& data, const QVariantMap& result);
};

class GetWanOp
	: public BaseOp
{
public:
	GetWanOp(RouterApi *owner);

protected:
	virtual int processImpl(const QByteArray& data, const QVariantMap& result);
};

class AuthenticateOp
	: public BaseOp
{
public:
	AuthenticateOp(const QString& username, const QString& password, RouterApi *owner);

protected:
	virtual int processImpl(const QByteArray& data, const QVariantMap& result);
};

class GetCurrentSettingsOp
	: public SimpleOp
{
	Q_OBJECT

public:
	GetCurrentSettingsOp(RouterApi *owner);
	virtual ~GetCurrentSettingsOp();

protected:
	virtual void startImpl();
	virtual void abortImpl();

private slots:
	void requestFinished();

private:
	RouterApi *m_owner;
	QNetworkReply *m_reply;
};

class DeleteMacAddressOp
	: public BaseOp
{
	Q_OBJECT

public:
	DeleteMacAddressOp(const QString& macAddress, RouterApi *owner);

protected:
	virtual int processImpl(const QByteArray& data, const QVariantMap& result);
};

} // namespace RouterApiImpl

#endif // __webapi_p2_h__
