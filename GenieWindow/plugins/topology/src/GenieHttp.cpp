#include "GenieHttp.h"
#include <QNetworkAccessManager>
#include <QList>

const QString HTTP_HEAD = "http://";
const qint64  MAX_READ_SIZE = 10*1024;

//====GenieHttpResponseHeader

GenieHttpResponseHeader::~GenieHttpResponseHeader()
{

}

QString GenieHttpResponseHeader::toString()
{
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
                       int majorVer, int minorVer)
{
    setRawHeader("Request Method", "method");
    setRawHeader("Request URI", path.toLatin1());

    QString version = "HTTP/" + QString::number(majorVer) + '.' + QString::number(minorVer);
    setRawHeader("Request Version", version.toLatin1());
}

QString GenieHttpRequestHeader::method()
{
    return rawHeader("Request Method");
}

QString GenieHttpRequestHeader::path()
{
    return rawHeader("Request URI");
}

void GenieHttpRequestHeader::setRequest(const QString &method, const QString &path, int majorVer, int minorVer)
{
    setRawHeader("Request Method", "method");
    setRawHeader("Request URI", path.toLatin1());

    QString version = "HTTP/" + QString::number(majorVer) + '.' + QString::number(minorVer);
    setRawHeader("Request Version", version.toLatin1());
}

//====GenieHttp

GenieHttp::GenieHttp(QObject *parent):_seq(0),_port(80),_close_keep_alive(false)
{
    _manager = new QNetworkAccessManager(parent);
    Q_ASSERT(_manager != NULL);

    init_error_map();
    connect(_manager, SIGNAL(authenticationRequired(QNetworkReply *, QAuthenticator *)),
            this, SLOT(slot_nanager_authenticationRequired(QNetworkReply*,QAuthenticator*)));
    connect(_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slot_manager_finish()));
    connect(_manager, SIGNAL(authenticationRequired),this,
            SLOT(slot_nanager_authenticationRequired(QNetworkReply *, QAuthenticator *)));

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
    setUrl(&request, path);
    setRequestAttribute(&request);

    QNetworkReply *reply = _manager->get(request);

    if(reply)
    {
        _seq_mutex.lock();
        _seq_reply_map.insert(reply, seq());
        int seq = _seq;
        _seq_mutex.unlock();
        return seq;
    }

    return -1;
}

int	GenieHttp::head( const QString & path )
{
    if(_host.isEmpty())
       return -1;

    QNetworkRequest request;
    setUrl(&request, path);
    setRequestAttribute(&request);

    QNetworkReply *reply = _manager->head(request);

    if(reply)
    {
        _seq_mutex.lock();
        _seq_reply_map.insert(reply, seq());
        int seq = _seq;
        _seq_mutex.unlock();
        return seq;
    }

    return -1;
}

int	GenieHttp::post(const QString & path, const QByteArray & data)
{
    if(_host.isEmpty())
       return -1;

    QNetworkRequest request;
    setUrl(&request, path);
    setRequestAttribute(&request);

    QNetworkReply *reply = _manager->post(request, data);

    if(reply)
    {
        _seq_mutex.lock();
        _seq_reply_map.insert(reply, seq());
        int seq = _seq;
        _seq_mutex.unlock();
        return seq;
    }

    return -1;
}

int	GenieHttp::post( const QString & path)
{
    if(_host.isEmpty())
       return -1;

    QNetworkRequest request;
    setUrl(&request, path);
    setRequestAttribute(&request);

    QNetworkReply *reply = _manager->post(request, QByteArray());

    if(reply)
    {
        _seq_mutex.lock();
        _seq_reply_map.insert(reply, seq());
        int seq = _seq;
        _seq_mutex.unlock();
        return seq;
    }

    return -1;
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

void GenieHttp::init_error_map()
{
    _error_map[QNetworkReply::NoError]                = NoError;
    _error_map[QNetworkReply::ConnectionRefusedError] = ConnectionRefused;
    _error_map[QNetworkReply::RemoteHostClosedError]  = HostNotFound;
    _error_map[QNetworkReply::HostNotFoundError]      = HostNotFound;
    _error_map[QNetworkReply::OperationCanceledError] = UnexpectedClose;
    _error_map[QNetworkReply::ProtocolFailure]        = InvalidResponseHeader;
    _error_map[QNetworkReply::AuthenticationRequiredError] = AuthenticationRequiredError;
}

int GenieHttp::get_std_error(int manager_error)
{
    if(_error_map.contains(manager_error))
        return _error_map[manager_error];
    return UnknownError;
}

void GenieHttp::setRequestAttribute(QNetworkRequest *request) const
{
    if(request)
       return;

    if(_close_keep_alive)
        request->setRawHeader("Connection", "close");
}

void GenieHttp::setUrl(QNetworkRequest *request, const QString path) const
{
    if(request)
       return;

    QString url = HTTP_HEAD + _host + ":" + QString::number(_port) + path;
    request->setUrl(QUrl(url));
}

void GenieHttp::slot_manager_finish(QNetworkReply *reply)
{
    if(! _seq_reply_map.contains(reply))
        return;

    int id = _seq_reply_map[reply];
    bool error = (get_std_error(reply->error()) != NoError);

    _read_all_content = "";
    if(! error)
        _read_all_content = reply->readAll();

    emit requestFinished(id, error);
    emit readyRead(qobject_cast<GenieHttpResponseHeader*>(reply));
    emit done(error);
}

void GenieHttp::slot_nanager_authenticationRequired(QNetworkReply * reply,
                                         QAuthenticator * authenticator)
{
    emit authenticationRequired (_host, _port, authenticator);
}
