#include "GenieHttp.h"
#include <QNetworkAccessManager>
#include <QNetWorkRequest>
#include <QList>
#include <QPair>
#include <stdio.h>
#include <QDebug>
const QString HTTP_HEAD = "http://";
const qint64  MAX_READ_SIZE = 10*1024;

//====GenieHttpResponseHeader

QString GenieHttpResponseHeader::toString()
{
    if(! isFinished())
        return "";

    QList<RawHeaderPair> pairs_list = rawHeaderPairs();
    QString ret;
    foreach(RawHeaderPair pair, pairs_list)
    {
        ret += pair.first;
        ret += pair.second;
        ret += "\r\n";
    }
    return ret;
}

int GenieHttpResponseHeader::statusCode() const
{
    QString str_code = rawHeader("Status Code");
    if(! str_code.isNull())
        return str_code.toInt();
    return -1;
}

QString GenieHttpResponseHeader::reasonPhrase()
{
    return rawHeader("Response Phrase");
}

//====GenieHttpRequestHeader

GenieHttpRequestHeader::GenieHttpRequestHeader(const QString & method, const QString & path,
                                               int majorVer, int minorVer):QNetworkRequest(path)
{
    _method = method;
    _path = path;
    _version = "HTTP/" + QString::number(majorVer)
            + '.' + QString::number(minorVer);
}

GenieHttpRequestHeader::GenieHttpRequestHeader(const GenieHttpRequestHeader& other):QNetworkRequest(other)
{
    _method = other._method;
    _path = other._path;
    _version = other._version;
}

QString GenieHttpRequestHeader::method()
{
    return _method;
}

QString GenieHttpRequestHeader::path()
{
    return _path;
}

QString GenieHttpRequestHeader::version()
{
    return _version;
}

void GenieHttpRequestHeader::setRequest(const QString &method, const QString &path, int majorVer, int minorVer)
{
    _method = method;
    _path = path;
    _version = "HTTP/" + QString::number(majorVer) +
            '.' + QString::number(minorVer);
}

void GenieHttpRequestHeader::setValue(const QString & key, const QString & value)
{
    QString str = value.toLatin1();
    setRawHeader(key.toLatin1(), str.toLatin1());
}

void GenieHttpRequestHeader::setValues(const QList<QPair<QString, QString> > & values)
{
    QPair<QString, QString> pair;
    foreach(pair, values)
    {
        setRawHeader(pair.first.toLatin1(), pair.second.toLatin1());
    }
}

//====GenieHttp

GenieHttp::GenieHttp(QObject *parent):_seq(0),_port(80),_close_keep_alive(false),_error(NoError)
{
    _manager = new QNetworkAccessManager(parent);
    Q_ASSERT(_manager != NULL);

    init_error_map();

    connect(_manager, SIGNAL(authenticationRequired(QNetworkReply *, QAuthenticator *)),
            this, SLOT(slot_nanager_authenticationRequired(QNetworkReply*, QAuthenticator*)));
    connect(_manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(slot_manager_finish(QNetworkReply*)));

}

GenieHttp::~GenieHttp()
{
    if(_manager)
    {
        delete _manager;
        _manager = NULL;
    }
}

int	GenieHttp::get(const QString & path)
{
    if(_host.isEmpty())
       return -1;

    QNetworkRequest request;
    setUrl(request, path);
    setRequestAttribute(request);

    QNetworkReply *reply = _manager->get(request);
    return seq(reply);
}

int	GenieHttp::head( const QString & path )
{
    if(_host.isEmpty())
       return -1;

    QNetworkRequest request;
    setUrl(request, path);
    setRequestAttribute(request);

    QNetworkReply *reply = _manager->head(request);
    return seq(reply);
}

int	GenieHttp::post(const QString & path, const QByteArray & data)
{
    if(_host.isEmpty())
       return -1;

    QNetworkRequest request;
    setUrl(request, path);
    setRequestAttribute(request);

    QNetworkReply *reply = _manager->post(request, data);
    return seq(reply);
}

int	GenieHttp::post( const QString & path)
{
    if(_host.isEmpty())
       return -1;

    QNetworkRequest request;
    setUrl(request, path);
    setRequestAttribute(request);

    QNetworkReply *reply = _manager->post(request, QByteArray());
    return seq(reply);
}

int GenieHttp::post(GenieHttpRequestHeader &head, const QByteArray &data)
{
    if(_host.isEmpty())
       return -1;

    setUrl(head, head.path());
    setRequestAttribute((QNetworkRequest&)head);
    QNetworkReply *reply = _manager->post(head, data);
    return seq(reply);
}

qint64 GenieHttp::read(char *data, qint64 maxlen)
{
    int len = maxlen < _read_all_content.length() ? maxlen : _read_all_content.length();
    qstrncpy(data, _read_all_content.toLatin1().constData(), len);
    return len;
}

QByteArray GenieHttp::readAll()
{
    return _read_all_content.toLatin1();
}


int	GenieHttp::setHost(const QString & hostName, quint16 port)
{
    _host = hostName;
    _port = port;
    return 0;
}

int GenieHttp::close ()
{
    _close_keep_alive = true;
    return 0;
}

void GenieHttp::abort()
{
    _manager->clearAccessCache();
}

int GenieHttp::request(GenieHttpRequestHeader & header)
{
    GenieHttpRequestHeader innerHeder(header);
    setUrl(innerHeder, innerHeder.path());
    setRequestAttribute(innerHeder);

    QNetworkReply *reply = _manager->sendCustomRequest(innerHeder, header.method().toLatin1());
    return seq(reply);
}

int GenieHttp::request(GenieHttpRequestHeader & header, const QByteArray & data)
{
    GenieHttpRequestHeader innerHeder(header);
    setUrl(innerHeder, innerHeder.path());
    setRequestAttribute(innerHeder);

    QNetworkReply *reply = _manager->sendCustomRequest(innerHeder, header.method().toLatin1());
    return seq(reply);

}

GenieHttp::Error GenieHttp::error()
{
    return _error;
}

void GenieHttp::init_error_map()
{
    _error_map[QNetworkReply::NoError]                = NoError;
    _error_map[QNetworkReply::ConnectionRefusedError] = ConnectionRefused;
    _error_map[QNetworkReply::RemoteHostClosedError]  = HostNotFound;
    _error_map[QNetworkReply::HostNotFoundError]      = HostNotFound;
    _error_map[QNetworkReply::OperationCanceledError] = UnexpectedClose;
    _error_map[QNetworkReply::ProtocolFailure]        = InvalidResponseHeader;
    _error_map[QNetworkReply::AuthenticationRequiredError] = AuthenticationRequiredError;
    _error_map[QNetworkReply::OperationCanceledError] = Aborted;
}

int GenieHttp::get_std_error(int manager_error)
{
    if(_error_map.contains(manager_error))
        return _error_map[manager_error];
    return UnknownError;
}

void GenieHttp::setRequestAttribute(QNetworkRequest &request) const
{
    if(_close_keep_alive)
        request.setRawHeader("Connection", "close");
}

void GenieHttp::setUrl(QNetworkRequest &request, QString path)
{
    QString url = HTTP_HEAD + _host + ":" + QString::number(_port) + path;
    request.setUrl(QUrl(url));
}

int GenieHttp::seq(QNetworkReply *reply)
{
    if(! reply)
        return -1;

    _seq_mutex.lock();

    //max int then back to 1
    if(_seq == 0xfffffff)
        _seq = 1;
    else
        ++_seq;

    _seq_reply_map.insert(reply, _seq);
    int seq = _seq;
    _seq_mutex.unlock();
    return _seq;
}

void GenieHttp::slot_manager_finish(QNetworkReply *reply)
{
    qDebug()<<"====slot_manager_finish";
    if(! _seq_reply_map.contains(reply))
        return;

    qDebug()<<"====slot_manager_finish22222";
    int id = _seq_reply_map[reply];
    int errorcode = get_std_error(reply->error());
    bool error = (errorcode != NoError);

    _read_all_content = "";
    if(! error)
        _read_all_content = reply->readAll();

    emit requestFinished(id, error);
    emit readyRead(reply);
    emit done(error);
}

void GenieHttp::slot_nanager_authenticationRequired(QNetworkReply * reply,
                                         QAuthenticator * authenticator)
{
    emit authenticationRequired (_host, _port, authenticator);
}
