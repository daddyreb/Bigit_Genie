#include "networkaccessproxy.h"
#include "networktransaction.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QtDebug>


/*static const char *g_soapBodyTemplate =
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
        "<SOAP-ENV:Envelope xmlns:SOAPSDK1=\"http://www.w3.org/2001/XMLSchema\" "
        "xmlns:SOAPSDK2=\"http://www.w3.org/2001/XMLSchema-instance\" "
        "xmlns:SOAPSDK3=\"http://schemas.xmlsoap.org/soap/encoding/\" "
        "xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\">"
        "<SOAP-ENV:Header>"
        "<SessionID>58DEE6006A88A967E89A</SessionID>"
        "</SOAP-ENV:Header>"
        "<SOAP-ENV:Body>"
        "<M1:%2 xmlns:M1=\"urn:NETGEAR-ROUTER:service:%1:1\">"
        "%3"
        "</M1:%2>"
        "</SOAP-ENV:Body>"
        "</SOAP-ENV:Envelope>";*/

static const char *g_PLCSoapBodyTemplate =
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"
        "<SOAP-ENV:Envelope SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" "
        "xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\" "
        "xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:xsd=\"http://www.w3.org/1999/XMLSchema\">"
        "<SOAP-ENV:Header>"
        "<dslf:SessionID xmlns:dslf=\"dslforum-org\" SOAP-ENV:mustUnderstand=\"1\">0123456789</dslf:SessionID>"
        "</SOAP-ENV:Header>"
        "<SOAP-ENV:Body>"
        "<method:%2 xmlns:method=\"urn:NETGEAR-ROUTER:service:%1:1\">"
        "%3"
        "</method:%2>"
        "</SOAP-ENV:Body>"
        "</SOAP-ENV:Envelope>";


NetworkAccessProxy::NetworkAccessProxy(QObject *parent) :
    QObject(parent)
  ,m_networkManager(0)
{
    m_networkManager = new QNetworkAccessManager();

    m_cachedRequest.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
    m_cachedRequest.setHeader(QNetworkRequest::ContentTypeHeader, QString("text/xml;charset=utf-8").toUtf8());
    //m_cachedRequest.setRawHeader(QString("User-Agent").toUtf8(), QString("SOAP Toolkit 3.0").toUtf8());
    m_cachedRequest.setRawHeader(QString("User-Agent").toUtf8(), QString("SOAP Sdk").toUtf8());

    m_cachedRequest.setRawHeader(QString("connection").toUtf8(), QString("keep-Alive").toUtf8());
    m_cachedRequest.setRawHeader(QString("Cache-Control").toUtf8(), QString("no-cache").toUtf8());
    //m_cachedRequest.setRawHeader(QString("Pragma").toUtf8(), QString("no-cache").toUtf8());

    QUrl requestUrl(QString("http://www.routerlogin.net:%1/soap/server_sa/").arg(80));
    m_cachedRequest.setUrl(requestUrl);


}


NetworkAccessProxy::~NetworkAccessProxy()
{
    delete m_networkManager;
}

NetworkTransaction* NetworkAccessProxy::PostRequest(const QString &module, const QString &method,
                                                    const QMap<QString, QString> *param)
{
    QNetworkRequest postRequest;
    QByteArray *requestData = new QByteArray();

    BuildRequest(module, method, param, postRequest, *requestData);

    QNetworkReply *reply = m_networkManager->post(postRequest, *requestData);

    return new NetworkTransaction(reply, requestData);
}

NetworkTransaction* NetworkAccessProxy::PostRequest(const QString &host,
                                                    quint16 port,
                                                    const QString &module,
                                                    const QString &method,
                                                    const QMap<QString, QString> *param)
{
    QNetworkRequest postRequest;
    QByteArray *requestData = new QByteArray();

    BuildRequest(module, method, param, postRequest, *requestData);

    QUrl requestUrl = postRequest.url();
    requestUrl.setHost(host);
    requestUrl.setPort(port);
    postRequest.setUrl(requestUrl);

    QNetworkReply *reply = m_networkManager->post(postRequest, *requestData);

    return new NetworkTransaction(reply, requestData);
}

void NetworkAccessProxy::BuildRequest(const QString &module
                                      ,const QString &method
                                      ,const QMap<QString, QString> *param
                                      ,QNetworkRequest &retRequest
                                      ,QByteArray &retData)
{
    retRequest = m_cachedRequest;

    QString paramString;

    if(param){
        QMap<QString, QString>::const_iterator iter = param->begin();
        while(iter != param->end()){
            //replace '<' and '>' ,otherwise they will make xml parser confusing
            QString value = iter.value();
            value = value.replace("<", "&lt;");
            value = value.replace(">", "&gt;");
            paramString += QString("<%1>%2</%1>").arg(iter.key(), value);
			++iter;
        }
    }


    QString bodyDataString(g_PLCSoapBodyTemplate);
    bodyDataString = bodyDataString.arg(module, method, paramString);
    retData = bodyDataString.toUtf8();

    retRequest.setHeader(QNetworkRequest::ContentLengthHeader, QString("%1").arg(retData.length()).toUtf8());
    QString soapActionHeader("\"urn:NETGEAR-ROUTER:service:%1:1#%2\"");
    soapActionHeader = soapActionHeader.arg(module, method);
    retRequest.setRawHeader(QString("SOAPAction").toUtf8(), soapActionHeader.toUtf8());

}


