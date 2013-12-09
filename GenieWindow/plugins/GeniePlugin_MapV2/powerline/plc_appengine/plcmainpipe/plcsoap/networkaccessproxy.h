#ifndef NETWORKACCESSPROXY_H
#define NETWORKACCESSPROXY_H

#include <QObject>
#include <QNetworkRequest>
#include <QString>
#include <QByteArray>
#include <QMap>

class QNetworkAccessManager;
class NetworkTransaction;

class NetworkAccessProxy : public QObject
{
    Q_OBJECT
public:
    explicit NetworkAccessProxy(QObject *parent = 0);
    ~NetworkAccessProxy();

public:
    NetworkTransaction* PostRequest(const QString &module, const QString &method,
                                    const QMap<QString, QString> *param);
    NetworkTransaction* PostRequest(const QString &host,
                                    quint16 port,
                                    const QString &module,
                                    const QString &method,
                                    const QMap<QString, QString> *param);
private:
    void BuildRequest(const QString &module,
                      const QString &method,
                      const QMap<QString, QString> *param,
                      QNetworkRequest &retRequest,
                      QByteArray &retData);

private:
    QNetworkAccessManager *m_networkManager;
    QNetworkRequest m_cachedRequest;
};

#endif // NETWORKACCESSPROXY_H
