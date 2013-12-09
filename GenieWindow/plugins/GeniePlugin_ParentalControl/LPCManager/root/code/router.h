#ifndef __router_h__
#define __router_h__

#include "AsyncOp.h"
#include <QHostInfo>
#include <QNetworkReply>
#include <QTcpSocket>

class QNetworkReply;

namespace impl {

class SoapPostOp
	: public AsyncOp
{
	Q_OBJECT

public:
	static SoapPostOp *begin(const QString& url, const QString& ns, const QString& cmd, const QStringList& paramNames, const QStringList& paramValues, QObject *parent);
	SoapPostOp(const QString& url, const QString& ns, const QString& cmd, const QByteArray& xml, QObject *parent);
	virtual ~SoapPostOp();

signals:
	void start();

protected:
	virtual void abortImpl();

private slots:
	void stageStarted();
	void socketError(QAbstractSocket::SocketError error);
	void socketConnected();
	void socketReadyRead();

private:
	void sendRequest();

private:
	enum Stage {
		Stage_Constructed,
		Stage_Connecting,
		Stage_Posting,
	};

	Stage m_stage;

	QTcpSocket *m_socket;
	bool m_inConnectToHost;
	bool m_connected;

	QString m_url;
	QString m_ns;
	QString m_cmd;
	QByteArray m_xml;
};

class CheckRouterStatusOp
	: public AsyncOp
{
	Q_OBJECT

public:
	CheckRouterStatusOp(const QString& username, const QString& password, QNetworkAccessManager *nam, QObject *parent);
	virtual ~CheckRouterStatusOp();

signals:
	void start();

protected:
	virtual void abortImpl();

private slots:
	void stageStarted();
	void stageDns1Finished(QHostInfo hostInfo);
	void stageDns2Finished(QHostInfo hostInfo);
	void stageRouterLoginFinished();
	void stageCheck1Finished();

private:
	void parseAuthAndContinue(const QByteArray& response);

private:
	enum Stage {
		Stage_Constructed,
		Stage_Dns1,
		Stage_Dns2,
		Stage_RouterLogin,
		Stage_Check1,
	};

	QString m_username;
	QString m_password;
	QNetworkAccessManager *m_nam;
	Stage m_stage;

	int m_dnsLookupId;
	QNetworkReply *m_reply;
	AsyncOp *m_op;
};

class RouterLoginOp
	: public AsyncOp
{
	Q_OBJECT

public:
	RouterLoginOp(const QString& username, const QString& password, QNetworkAccessManager *nam, QObject *parent);
	virtual ~RouterLoginOp();

signals:
	void start();

protected:
	virtual void abortImpl();

private slots:
	void stageStarted();
	void stageAuth1Finished();
	void stageAuth2Finished();

private:
	void completeAuth(const QByteArray& response);

private:
	enum Stage {
		Stage_Constructed,
		Stage_Auth1,
		Stage_Auth2,
	};

	QString m_username;
	QString m_password;
	QNetworkAccessManager *m_nam;
	Stage m_stage;

	QNetworkReply *m_reply;
};

class RouterLoginOp2
	: public AsyncOp
{
	Q_OBJECT

public:
	RouterLoginOp2(const QString& username, const QString& password, QObject *parent);
	virtual ~RouterLoginOp2();

signals:
	void start();

protected:
	virtual void abortImpl();

private slots:
	void stageStarted();
	void stageAuth1Finished();
	void stageAuth2Finished();

private:
	void completeAuth(const QByteArray& response);

private:
	enum Stage {
		Stage_Constructed,
		Stage_Auth1,
		Stage_Auth2,
	};

	QString m_username;
	QString m_password;
	Stage m_stage;
	AsyncOp *m_op;
};

class RouterConfigurationStart
	: public AsyncOp
{
	Q_OBJECT

public:
	RouterConfigurationStart(const QString& action, QNetworkAccessManager *nam, QObject *parent);
	virtual ~RouterConfigurationStart();

signals:
	void start();

protected:
	virtual void abortImpl();

private slots:
	void stageStarted();
	void stagePostFinished();

private:
	enum Stage {
		Stage_Constructed,
		Stage_Post,
	};

	QString m_action;
	QNetworkAccessManager *m_nam;
	Stage m_stage;

	QNetworkReply *m_reply;
};

class RouterConfigurationFinish
	: public AsyncOp
{
	Q_OBJECT

public:
	RouterConfigurationFinish(const QString& action, QNetworkAccessManager *nam, QObject *parent);
	virtual ~RouterConfigurationFinish();

signals:
	void start();

protected:
	virtual void abortImpl();

private slots:
	void stageStarted();
	void stagePostFinished();

private:
	enum Stage {
		Stage_Constructed,
		Stage_Post,
	};

	QString m_action;
	QNetworkAccessManager *m_nam;
	Stage m_stage;

	QNetworkReply *m_reply;
};

class RouterGetDnsMasqDeviceId
	: public AsyncOp
{
	Q_OBJECT

public:
	RouterGetDnsMasqDeviceId(const QString& action, const QString& macAddress, QNetworkAccessManager *nam, QObject *parent);
	virtual ~RouterGetDnsMasqDeviceId();

signals:
	void start();

protected:
	virtual void abortImpl();

private slots:
	void stageStarted();
	void stagePostFinished();

private:
	enum Stage {
		Stage_Constructed,
		Stage_Post,
	};

	QString m_action;
	QString m_macAddress;
	QNetworkAccessManager *m_nam;
	Stage m_stage;

	QNetworkReply *m_reply;
};

} // namespace impl

#endif // __router_h__
