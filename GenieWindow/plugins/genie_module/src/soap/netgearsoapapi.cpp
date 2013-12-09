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
     m_port = port;
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

//void NetgearSoapAPI::submitsoapmessage()
//{
//    QString actionstr;
//    actionstr = QString("urn:NETGEAR-ROUTER:service:%1:1#%2").arg(modulename).arg(methodname);
//    http.setAction(actionstr);
//
//    QtSoapMessage request;
//
//    QString uristr;
//    if(methodname.compare("ConfigurationStarted"))
//    {
//        request.addHeaderItem(new QtSoapSimpleType(QtSoapQName("SessionID", uristr), "E6A88AE69687E58D9A00"));
//    }
//
//    //QString uristr = QString("urn:NETGEAR-ROUTER:service:%1:1").arg(modulename);
//
////    request.setFlag(m_port == 80);
////    request.setFlag(modulename.compare("ParentalControl"));
//
////    request.setMethod(QtSoapQName(methodname, uristr));
////    request.setMethod(QtSoapQName(QString("M1:GetInfo xmlns:M1=\"urn:NETGEAR-ROUTER:service:DeviceInfo:1\""), uristr));
//    if(modulename.compare("ParentalControl"))
//    {
//        request.setMethod(QtSoapQName(QString("M1:%1 xmlns:M1=\"urn:NETGEAR-ROUTER:service:%2:1\"").arg(methodname).arg(modulename), uristr));
//    }else
//    {
//        request.setMethod(QtSoapQName(methodname, uristr));
//    }
//
//    QMapIterator<QString, QString> i(params);
//    while (i.hasNext()) {
//        i.next();
//        request.addMethodArgument(i.key(), "", i.value());
//    }
//
//    http.submitRequest(request, "/soap/server_sa/");
//}

void NetgearSoapAPI::submitsoapmessage()
{
    QString actionstr;
    actionstr = QString("urn:NETGEAR-ROUTER:service:%1:1#%2").arg(modulename).arg(methodname);
    http.setAction(actionstr);

    if(modulename.compare("ParentalControl"))
    {
        QString request;
        QString paramstr;
        QString headerstr;

        if(methodname.compare("ConfigurationStarted"))
        {
            headerstr = QString("<SOAP-ENV:Header><SessionID>E6A88AE69687E58D9A00</SessionID></SOAP-ENV:Header>");
        }

        QMapIterator<QString, QString> i(params);
        while (i.hasNext()) {
            i.next();
            QString tmp;
            tmp = QString("<%1>%2</%1>").arg(i.key()).arg(i.value());
            paramstr += tmp;
        }
        request = QString("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><SOAP-ENV:Envelope xmlns:SOAPSDK1=\"http://www.w3.org/2001/XMLSchema\" xmlns:SOAPSDK2=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:SOAPSDK3=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\">%4<SOAP-ENV:Body><M1:%2 xmlns:M1=\"urn:NETGEAR-ROUTER:service:%1:1\">%3</M1:%2></SOAP-ENV:Body></SOAP-ENV:Envelope>").arg(modulename).arg(methodname).arg(paramstr).arg(headerstr);

        http.submitRequest(request, "/soap/server_sa/");
    }else
    {
        QtSoapMessage request;

        QString uristr;
        request.addHeaderItem(new QtSoapSimpleType(QtSoapQName("SessionID", uristr), "E6A88AE69687E58D9A00"));

        //QString uristr = QString("urn:NETGEAR-ROUTER:service:%1:1").arg(modulename);

        request.setFlag(m_port == 80);

        request.setMethod(QtSoapQName(methodname, uristr));

        QMapIterator<QString, QString> i(params);
        while (i.hasNext()) {
            i.next();
            request.addMethodArgument(i.key(), "", i.value());
        }

        http.submitRequest(request, "/soap/server_sa/");
    }
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
