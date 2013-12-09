#ifndef __oputil_h__
#define __oputil_h__

#include "AsyncOp.h"
#include <QByteArray>
#include <QList>
#include <QMap>
#include <QStringList>
#include <QVariant>
#include <QFile>
#include <QTextStream>

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

//kai.yan add
class Outfile
{
public:
    Outfile();
    void writeFile(QString key,QString value);
    QString readFile(QString key);
};

#endif // __oputil_h__
