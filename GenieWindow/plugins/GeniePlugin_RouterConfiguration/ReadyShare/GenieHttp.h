#ifndef GENIEHTTP_H
#define GENIEHTTP_H
#include <QObject>
#include <QString>
#include <QByteArray>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMap>
#include <QMutex>

class QAuthenticator;

class GenieHttpResponseHeader : public QNetworkReply
{
    Q_OBJECT
public:
    virtual ~GenieHttpResponseHeader(){};

    QString toString();
    int statusCode () const;
    QString reasonPhrase();
};

class GenieHttpRequestHeader : public QNetworkRequest
{
public:
    GenieHttpRequestHeader(){};
    GenieHttpRequestHeader(const QString &method, const QString &path,
                    int majorVer = 1, int minorVer = 1);
    GenieHttpRequestHeader(const QString & str);
    GenieHttpRequestHeader(const GenieHttpRequestHeader& other);

    QString method();
    QString path();
    QString version();
    void setRequest(const QString &method, const QString &path, int majorVer = 1, int minorVer = 1);
    void setValue(const QString & key, const QString & value);
    void setValues(const QList<QPair<QString, QString> > & values);

private:
    QString _method;
    QString _version;
    QString _path;
};

class GenieHttp : public QObject
{
    Q_OBJECT
public:
    explicit GenieHttp(QObject *parent = 0);
    virtual ~GenieHttp();

    enum Error {
        NoError = 0,
        UnknownError,
        HostNotFound,
        ConnectionRefused,
        UnexpectedClose,
        InvalidResponseHeader,
        WrongContentLength,
        Aborted,
        AuthenticationRequiredError,
        ProxyAuthenticationRequiredError
    };

    int	get( const QString & path );
    int	head( const QString & path );
    int	post( const QString & path, const QByteArray & data );
    int	post( const QString & path);
    int post(GenieHttpRequestHeader &head, const QByteArray &data);
    qint64 read( char *data, qint64 maxlen);
    QByteArray readAll ();
    int	setHost( const QString & hostName, quint16 port = 80 );
    int close();   //close keep-alive
    int request(GenieHttpRequestHeader & header);
    int request(GenieHttpRequestHeader & header, const QByteArray & data);
    Error error();

signals:
    void authenticationRequired (const QString & hostname, quint16 port, QAuthenticator * authenticator);
    void done (bool error);
    void requestFinished(int id, bool error);
    void readyRead(QNetworkReply *response);

public slots:
    void abort();

private slots:
    void slot_nanager_authenticationRequired(QNetworkReply * reply, QAuthenticator * authenticator);
    void slot_manager_finish(QNetworkReply *reply);

private:
    void init_error_map();
    int get_std_error(int manager_error);
    int seq(QNetworkReply *reply);
    void setRequestAttribute(QNetworkRequest &request) const;
    void setUrl(QNetworkRequest &request, QString path);

    QMap<int, int> _error_map;
    QNetworkAccessManager *_manager;
    QString _host;
    int _port;

    QMutex _seq_mutex;
    int _seq;  //return id
    QMap<QNetworkReply*, int> _seq_reply_map;

    QString _read_all_content;
    bool _close_keep_alive;

    Error _error;
};

#endif // GENIEHTTP_H
