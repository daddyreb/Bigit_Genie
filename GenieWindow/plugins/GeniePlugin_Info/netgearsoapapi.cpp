#include "netgearsoapapi.h"

NetgearSoapAPI::NetgearSoapAPI()
{
    connect(&http, SIGNAL(responseReady()), SLOT(getresponse()));
}

NetgearSoapAPI::NetgearSoapAPI(QString &hostName):hostName(hostName)
{
    setHost(hostName);

    connect(&http, SIGNAL(responseReady()), SLOT(getresponse()));
}

void NetgearSoapAPI::setHost(const QString &hostName, quint16 port)
{
     http.setHost(hostName, port);
}

void NetgearSoapAPI::setmodulename(QString &modulename)
{
    this->modulename = modulename;
}

void NetgearSoapAPI::setmethodname(QString &methodname)
{
    this->methodname = methodname;
}

void NetgearSoapAPI::setparams(QMap<QString, QString> &params)
{
    this->params = params;
}

void NetgearSoapAPI::submitsoapmessage()
{
    QString actionstr;
    actionstr = QString("urn:NETGEAR-ROUTER:service:%1:1#%2").arg(modulename).arg(methodname);
    http.setAction(actionstr);

    QtSoapMessage request;

    QString uristr;
    request.addHeaderItem(new QtSoapSimpleType(QtSoapQName("SessionID", uristr), "99999999999"));

    //QString uristr = QString("urn:NETGEAR-ROUTER:service:%1:1").arg(modulename);
    request.setMethod(QtSoapQName(methodname, uristr));

    QMapIterator<QString, QString> i(params);
    while (i.hasNext()) {
        i.next();
        request.addMethodArgument(i.key(), "", i.value());
    }

    http.submitRequest(request, "/soap/server_sa/");
}

void NetgearSoapAPI::submitrequest(QString &modulename, QString &methodname, QMap<QString, QString> &params)
{
    setmodulename(modulename);
    setmethodname(methodname);
    setparams(params);
    submitsoapmessage();
}

void NetgearSoapAPI::configurationstarted()
{
    QtSoapMessage request;
    http.setAction("urn:NETGEAR-ROUTER:service:DeviceConfig:1#ConfigurationStarted");
    request.setMethod("ConfigurationStarted", "urn:NETGEAR-ROUTER:service:DeviceConfig:1");
    http.submitRequest(request, "/soap/server_sa/");
}

void NetgearSoapAPI::configurationfinished()
{
    QtSoapMessage request;
    http.setAction("urn:NETGEAR-ROUTER:service:DeviceConfig:1#ConfigurationFinished");
    request.setMethod("ConfigurationFinished", "urn:NETGEAR-ROUTER:service:DeviceConfig:1");
    http.submitRequest(request, "/soap/server_sa/");
}

void NetgearSoapAPI::getresponse()
{
    // Get a reference to the response message.
    const QtSoapMessage &message = http.getResponse();
    QtSoapMessage m = message;
    //QtSoapMessage *m = new QtSoapMessage();
    //*m = message;
    emit getResponse(m);
}
