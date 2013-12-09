#ifndef __oputil_h__
#define __oputil_h__

#include "AsyncOp.h"
#include <QByteArray>
#include <QList>
#include <QMap>
#include <QStringList>
#include <QVariant>

class QByteArray;
class QNetworkAccessManager;
class QNetworkReply;
class QStringList;

class SimpleOp
	: public AsyncOp
{
	Q_OBJECT

public:
	virtual ~SimpleOp();

signals:
	void internalStartSignal();

protected:
	SimpleOp(QObject *parent);
	void internalStart();
	virtual void startImpl();

private slots:
	void internalStartSlot();
};

QNetworkReply *webApiPost(QNetworkAccessManager *nam, const QByteArray& function, const QStringList& paramNames, const QStringList& paramValues);
bool parseJsonLite(const QByteArray& json, QVariantMap& result);

QNetworkReply *routerApiPost(QNetworkAccessManager *nam, const QString& action, const QByteArray& ns, const QByteArray& cmd, const QStringList& paramNames, const QStringList& paramValues);

bool isPrivateAddress(quint32 ipv4);
bool getGatewayMacAddress(quint32 ipv4Host, QByteArray& mac);

QList<QByteArray> nice_split(const QByteArray& data);

namespace WebApiImpl {

class BaseOp
	: public SimpleOp
{
	Q_OBJECT

public:
	BaseOp(QNetworkAccessManager *nam, QObject *parent);
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
	QNetworkAccessManager *m_nam;
	QNetworkReply *m_reply;
};

} // namespace WebApiImpl

namespace RouterApiImpl {

class BaseOp
	: public SimpleOp
{
	Q_OBJECT

public:
	BaseOp(const QString& url, QNetworkAccessManager *nam, QObject *parent);
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
	QNetworkAccessManager *m_nam;
	QNetworkReply *m_reply;
	QString m_url;
};

} // namespace RouterApiImpl

#endif // __oputil_h__
