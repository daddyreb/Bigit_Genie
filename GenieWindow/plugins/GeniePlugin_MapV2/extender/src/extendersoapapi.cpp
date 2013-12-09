#include <QTextCodec>
#include <QtNetwork/QNetworkRequest>
#include <QString>

#include "extendersoapapi.h"
#include "genie2_interface.h"

QString g_host = "www.mywifiext.com";

ExtenderSoapApi::ExtenderSoapApi(QObject *parent) :
    QObject(parent)
{

    soapSend = "";
}

void ExtenderSoapApi::Authenticate(QNetworkAccessManager *manager, QString port){
    //qDebug() << "=========Authenticate Begin";
    QString username = GENIE2_GET_SETTING(PLUGIN_MAP_EXTENDER_USERNAME).toString();
    QString password = GENIE2_GET_SETTING(PLUGIN_MAP_EXTENDER_PASSWORD).toString();

    qDebug() << "ExtenderSoapApi:: " << username << password;
    QString AXML = "";
    AXML.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");
    AXML.append("<SOAP-ENV:Envelope \r\n");
    AXML.append("xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"\r\n");
    AXML.append("xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\" \r\n");
    AXML.append("xmlns:xsd=\"http://www.w3.org/1999/XMLSchema\">\r\n");
    AXML.append("<SOAP-ENV:Body>\r\n");
    AXML.append("<Authenticate>\r\n");
    AXML.append("<NewPassword xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">" + password + "</NewPassword>\r\n");
    AXML.append("<NewUsername xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">" + username + "</NewUsername>\r\n");
    AXML.append("</Authenticate>\r\n");
    AXML.append("</SOAP-ENV:Body>\r\n");
    AXML.append("</SOAP-ENV:Envelope>\r\n");

    QString SOAPAction = "urn:NETGEAR_ROUTER:service:DeviceConfig:1#Authenticate";
    QString data = AXML;
    qDebug() << data;

    QNetworkRequest networkReq;
    networkReq.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("text/xml;charset=utf-8"));
    networkReq.setRawHeader("SOAPAction", SOAPAction.toUtf8());
    networkReq.setUrl(QUrl("http://" + g_host + ":" + port + "/soap/server_sa/"));

    manager->post(networkReq, data.toUtf8());
}

void ExtenderSoapApi::GetAttachedDevice(QNetworkAccessManager *manager, QString port){
    //qDebug() << "=========Authenticate Begin";
    QString AXML = "";
    AXML.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");
    AXML.append("<SOAP-ENV:Envelope \r\n");
    AXML.append("xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"\r\n");
    AXML.append("xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\" \r\n");
    AXML.append("xmlns:xsd=\"http://www.w3.org/1999/XMLSchema\">\r\n");
    AXML.append("<SOAP-ENV:Body>\r\n");
    AXML.append("<GetAttachedDevice>\r\n");
    AXML.append("</GetAttachedDevice>\r\n");
    AXML.append("</SOAP-ENV:Body>\r\n");
    AXML.append("</SOAP-ENV:Envelope>\r\n");

    QString SOAPAction = "urn:NETGEAR_ROUTER:service:DeviceInfo:1#GetAttachedDevice";
    QString data = AXML;
    qDebug() << data;

    QNetworkRequest networkReq;
    networkReq.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("text/xml;charset=utf-8"));
    networkReq.setRawHeader("SOAPAction", SOAPAction.toUtf8());
    networkReq.setUrl(QUrl("http://" + g_host + ":" + port + "/soap/server_sa/"));

    manager->post(networkReq, data.toUtf8());
}

void ExtenderSoapApi::GetEnable(QNetworkAccessManager *manager, QString port){
    QString AXML = "";
    AXML.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    AXML.append("<SOAP-ENV:Envelope");
    AXML.append("xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"");
    AXML.append("xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\"");
    AXML.append("xmlns:xsd=\"http://www.w3.org/1999/XMLSchema\">");
    AXML.append("<SOAP-ENV:Body>");
    AXML.append("<GetEnable>");
    AXML.append("</GetEnable>");
    AXML.append("</SOAP-ENV:Body>");
    AXML.append("</SOAP-ENV:Envelope>");

    QString SOAPAction = "urn:NETGEAR-ROUTER:service:DeviceConfig:1#GetEnable";
    QString data = AXML;
    qDebug() << data;

    QNetworkRequest networkReq;
    networkReq.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("text/xml;charset=utf-8"));
    networkReq.setRawHeader("SOAPAction", SOAPAction.toUtf8());
    networkReq.setUrl(QUrl("http://" + g_host + ":" + port + "/soap/server_sa/"));

    manager->post(networkReq, data.toUtf8());
}

void ExtenderSoapApi::GetIsExtender(QNetworkAccessManager *manager, QString port){
    QString AXML = "";
    AXML.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    AXML.append("<SOAP-ENV:Envelope");
    AXML.append("xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"");
    AXML.append("xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\"");
    AXML.append("xmlns:xsd=\"http://www.w3.org/1999/XMLSchema\">");
    AXML.append("<SOAP-ENV:Body>");
    AXML.append("<IsExtender>");
    AXML.append("</IsExtender>");
    AXML.append("</SOAP-ENV:Body>");
    AXML.append("</SOAP-ENV:Envelope>");

    QString SOAPAction = "urn:NETGEAR-ROUTER:service:DeviceInfo:1#IsExtender";
    QString data = AXML;
    qDebug() << data;

    soapSend = data;

    QNetworkRequest networkReq;
    networkReq.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("text/xml;charset=utf-8"));
    networkReq.setRawHeader("SOAPAction", SOAPAction.toUtf8());
    networkReq.setUrl(QUrl("http://" + g_host + ":" + port + "/soap/server_sa/"));

    manager->post(networkReq, data.toUtf8());
}

void ExtenderSoapApi::Is5GSupported(QNetworkAccessManager *manager, QString port){
    QString AXML = "";
    AXML.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");
    AXML.append("<SOAP-ENV:Envelope \r\n");
    AXML.append("xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"\r\n");
    AXML.append("xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\" \r\n");
    AXML.append("xmlns:xsd=\"http://www.w3.org/1999/XMLSchema\">\r\n");
    AXML.append("<SOAP-ENV:Body>\r\n");
    AXML.append("<Is5GSupported>\r\n");
    AXML.append("</Is5GSupported>\r\n");
    AXML.append("</SOAP-ENV:Body>\r\n");
    AXML.append("</SOAP-ENV:Envelope>\r\n");

    QString SOAPAction = "urn:NETGEAR_ROUTER:service:WLANConfiguration:1#Is5GSupported";
    QString data = AXML;
    qDebug() << data;

    QNetworkRequest networkReq;
    networkReq.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("text/xml;charset=utf-8"));
    networkReq.setRawHeader("SOAPAction", SOAPAction.toUtf8());
    networkReq.setUrl(QUrl("http://" + g_host + ":" + port + "/soap/server_sa/"));

    manager->post(networkReq, data.toUtf8());
}

void ExtenderSoapApi::GetAPInfo(QNetworkAccessManager *manager, QString port, QString NewRadio){
    QString AXML = "";
    AXML.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    AXML.append("<SOAP-ENV:Envelope ");
    AXML.append("xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"");
    AXML.append("xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\" ");
    AXML.append("xmlns:xsd=\"http://www.w3.org/1999/XMLSchema\">");
    AXML.append("<SOAP-ENV:Body>");
    AXML.append("<GetAPInfo>");
    AXML.append("<NewRadio xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewRadio+"</NewRadio>");
    AXML.append("</GetAPInfo>");
    AXML.append("</SOAP-ENV:Body>");
    AXML.append("</SOAP-ENV:Envelope>");

    QString SOAPAction = "urn:NETGEAR-ROUTER:service:WLANConfiguration:1#GetAPInfo";
    QString data = AXML;
    qDebug() << data;

    soapSend = data;

    QNetworkRequest networkReq;
    networkReq.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("text/xml;charset=utf-8"));
    networkReq.setRawHeader("SOAPAction", SOAPAction.toUtf8());

    networkReq.setUrl(QUrl("http://" + g_host + ":" + port + "/soap/server_sa/"));

    manager->post(networkReq, data.toUtf8());

}

void ExtenderSoapApi::SetExtenderMode(QNetworkAccessManager *manager, QString port,QString NewExtenderMode,QString New2GRadioMode,
                              QString New5GRadioMode,QString NewBondEthernet)
{
    //    NewExtenderMode="Internet Surfing";
    //    New2GRadioMode="Extender";
    //    New5GRadioMode="";
    //    NewBondEthernet="";
    QString AXML = "";
    AXML.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    AXML.append("<SOAP-ENV:Envelope");
    AXML.append("xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"");
    AXML.append("xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\" ");
    AXML.append("xmlns:xsd=\"http://www.w3.org/1999/XMLSchema\">");
    AXML.append("<SOAP-ENV:Body>");
    AXML.append("<SetExtenderMode>");
    AXML.append("<NewExtenderMode xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewExtenderMode+"</NewExtenderMode>");
    AXML.append("<New2GRadioMode xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+New2GRadioMode+"</New2GRadioMode>");
    AXML.append("<New5GRadioMode xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+New5GRadioMode+"</New5GRadioMode>");
    AXML.append("<NewBondEthernet xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewBondEthernet+"</NewBondEthernet>");
    AXML.append("</SetExtenderMode>");
    AXML.append("</SOAP-ENV:Body>");
    AXML.append("</SOAP-ENV:Envelope>");

    QString SOAPAction = "urn:NETGEAR-ROUTER:service:WLANConfiguration:1#SetExtenderMode";
    QString data = AXML;
    qDebug() << data;

    soapSend = data;

    QNetworkRequest networkReq;
    networkReq.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("text/xml;charset=utf-8"));
    networkReq.setRawHeader("SOAPAction", SOAPAction.toUtf8());
    networkReq.setUrl(QUrl("http://" + g_host + ":" + port + "/soap/server_sa/"));


    manager->post(networkReq, data.toUtf8());
}

void ExtenderSoapApi::ConfigurationStarted(QNetworkAccessManager *manager, QString port){
    QString AXML = "";
    AXML.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    AXML.append("<SOAP-ENV:Envelope");
    AXML.append("xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"");
    AXML.append("xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\"");
    AXML.append("xmlns:xsd=\"http://www.w3.org/1999/XMLSchema\">");
    AXML.append("<SOAP-ENV:Body>");
    AXML.append("<ConfigurationStarted>");
    AXML.append("</ConfigurationStarted>");
    AXML.append("</SOAP-ENV:Body>");
    AXML.append("</SOAP-ENV:Envelope>");

    QString SOAPAction = "urn:NETGEAR-ROUTER:service:DeviceConfig:1#ConfigurationStarted";
    QString data = AXML;
    qDebug() << data;

    soapSend = data;

    QNetworkRequest networkReq;
    networkReq.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("text/xml;charset=utf-8"));
    networkReq.setRawHeader("SOAPAction", SOAPAction.toUtf8());
    networkReq.setUrl(QUrl("http://" + g_host + ":" + port + "/soap/server_sa/"));


    manager->post(networkReq, data.toUtf8());
}
void ExtenderSoapApi::SetRouterWLANNoSecurity(QNetworkAccessManager *manager, QString port,QString NewRadio,QString NewSSID,
                                      QString NewChannel,QString NewWirelessMode,
                                      int NewVerify)
{
    QString AXML = "";
    AXML.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    AXML.append("<SOAP-ENV:Envelope ");
    AXML.append("xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"");
    AXML.append("xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\" ");
    AXML.append("xmlns:xsd=\"http://www.w3.org/1999/XMLSchema\">");
    AXML.append("<SOAP-ENV:Body>");
    AXML.append("<SetRouterWLANNoSecurity>");
    AXML.append("<NewRadio xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewRadio+"</NewRadio>");
    AXML.append("<NewSSID xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewSSID+"</NewSSID>");
    AXML.append("<NewChannel xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewChannel+"</NewChannel>");
    AXML.append("<NewWirelessMode xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewWirelessMode+"</NewWirelessMode>");
    AXML.append("<NewVerify xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+ QString::number(NewVerify) +"</NewVerify>");
    AXML.append("</SetRouterWLANNoSecurity>");
    AXML.append("</SOAP-ENV:Body>");
    AXML.append("</SOAP-ENV:Envelope>");

    QString SOAPAction = "urn:NETGEAR-ROUTER:service:WLANConfiguration:1#SetRouterWLANNoSecurity";
    QString data = AXML;
    qDebug() << data;

    QNetworkRequest networkReq;
    networkReq.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("text/xml;charset=utf-8"));
    networkReq.setRawHeader("SOAPAction", SOAPAction.toUtf8());
    networkReq.setUrl(QUrl("http://" + g_host + ":" + port + "/soap/server_sa/"));


    manager->post(networkReq, data.toUtf8());
}
void ExtenderSoapApi::SetRouterWLANWEPByKeys(QNetworkAccessManager *manager, QString port,QString NewRadio,QString NewSSID,
                                     QString NewChannel,QString NewWirelessMode,
                                     QString NewWEPAuthType,int NewWEPLength,
                                     int NewKeyIndex,QString NewWEPPassphrase,
                                     int NewVerify)
{
    QString AXML = "";
    AXML.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    AXML.append("<SOAP-ENV:Envelope ");
    AXML.append("xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"");
    AXML.append("xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\" ");
    AXML.append("xmlns:xsd=\"http://www.w3.org/1999/XMLSchema\">");
    AXML.append("<SOAP-ENV:Body>");
    AXML.append("<SetRouterWLANWEPByKeys>");
    AXML.append("<NewRadio xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewRadio+"</NewRadio>");
    AXML.append("<NewSSID xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewSSID+"</NewSSID>");
    AXML.append("<NewChannel xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewChannel+"</NewChannel>");
    AXML.append("<NewWirelessMode xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewWirelessMode+"</NewWirelessMode>");
    AXML.append("<NewWEPAuthType xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewWEPAuthType+"</NewWEPAuthType>");
    AXML.append("<NewWEPLength xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+ QString::number(NewWEPLength) +"</NewWEPLength>");
    AXML.append("<NewKeyIndex xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+ QString::number(NewKeyIndex) +"</NewKeyIndex>");
    AXML.append("<NewWEPPassphrase xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewWEPPassphrase+"</NewWEPPassphrase>");
    AXML.append("<NewVerify xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+ QString::number(NewVerify) +"</NewVerify>");
    AXML.append("</SetRouterWLANWEPByKeys>");
    AXML.append("</SOAP-ENV:Body>");
    AXML.append("</SOAP-ENV:Envelope>");

    QString SOAPAction = "urn:NETGEAR-ROUTER:service:WLANConfiguration:1#SetRouterWLANWEPByKeys";
    QString data = AXML;
    qDebug() << data;

    QNetworkRequest networkReq;
    networkReq.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("text/xml;charset=utf-8"));
    networkReq.setRawHeader("SOAPAction", SOAPAction.toUtf8());
    networkReq.setUrl(QUrl("http://" + g_host + ":" + port + "/soap/server_sa/"));


    manager->post(networkReq, data.toUtf8());
}
void ExtenderSoapApi::SetRouterWLANWPAPSKByPassphrase(QNetworkAccessManager *manager, QString port,QString NewRadio,QString NewSSID,QString NewChannel,
                                              QString NewWirelessMode,QString NewWPAEncryptionModes ,
                                              QString NewWPAPassphrase ,int NewVerify){ 
    QString AXML = "";
    AXML.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    AXML.append("<SOAP-ENV:Envelope ");
    AXML.append("xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"");
    AXML.append("xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\" ");
    AXML.append("xmlns:xsd=\"http://www.w3.org/1999/XMLSchema\">");
    AXML.append("<SOAP-ENV:Body>");
    AXML.append("<SetRouterWLANWPAPSKByPassphrase>");
    AXML.append("<NewRadio xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewRadio+"</NewRadio>");
    AXML.append("<NewSSID xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewSSID+"</NewSSID>");
    AXML.append("<NewChannel xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewChannel+"</NewChannel>");
    AXML.append("<NewWirelessMode xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewWirelessMode+"</NewWirelessMode>");
    AXML.append("<NewWPAEncryptionModes xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewWPAEncryptionModes+"</NewWPAEncryptionModes>");
    AXML.append("<NewWPAPassphrase xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewWPAPassphrase+"</NewWPAPassphrase>");
    AXML.append("<NewVerify xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+ QString::number(NewVerify) +"</NewVerify>");
    AXML.append("</SetRouterWLANWPAPSKByPassphrase>");
    AXML.append("</SOAP-ENV:Body>");
    AXML.append("</SOAP-ENV:Envelope>");

    QString SOAPAction = "urn:NETGEAR-ROUTER:service:WLANConfiguration:1#SetRouterWLANWPAPSKByPassphrase";
    QString data = AXML;
    qDebug() << data;
    QNetworkRequest networkReq;
    networkReq.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("text/xml;charset=utf-8"));
    networkReq.setRawHeader("SOAPAction", SOAPAction.toUtf8());
    networkReq.setUrl(QUrl("http://" + g_host + ":" + port + "/soap/server_sa/"));


    manager->post(networkReq, data.toUtf8());
}

void ExtenderSoapApi::GetRouterWLANVerifyResult(QNetworkAccessManager *manager, QString port, QString NewRadio)
{
    QString AXML = "";
    AXML.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    AXML.append("<SOAP-ENV:Envelope ");
    AXML.append("xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"");
    AXML.append("xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\" ");
    AXML.append("xmlns:xsd=\"http://www.w3.org/1999/XMLSchema\">");
    AXML.append("<SOAP-ENV:Body>");
    AXML.append("<GetRouterWLANVerifyResult>");
    AXML.append("<NewRadio xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewRadio+"</NewRadio>");
    AXML.append("</GetRouterWLANVerifyResult>");
    AXML.append("</SOAP-ENV:Body>");
    AXML.append("</SOAP-ENV:Envelope>");

    QString SOAPAction = "urn:NETGEAR-ROUTER:service:WLANConfiguration:1#GetRouterWLANVerifyResult";
    QString data = AXML;
    qDebug() << data;

    soapSend = data;

    QNetworkRequest networkReq;
    networkReq.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("text/xml;charset=utf-8"));
    networkReq.setRawHeader("SOAPAction", SOAPAction.toUtf8());
    networkReq.setUrl(QUrl("http://" + g_host + ":" + port + "/soap/server_sa/"));


    manager->post(networkReq, data.toUtf8());
}

void ExtenderSoapApi::SetWLANNoSecurity(QNetworkAccessManager *manager, QString port,QString NewRadio,QString NewSSID,
                                QString NewChannel,QString NewWirelessMode)
{    
    QString AXML = "";
    AXML.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    AXML.append("<SOAP-ENV:Envelope ");
    AXML.append("xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"");
    AXML.append("xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\" ");
    AXML.append("xmlns:xsd=\"http://www.w3.org/1999/XMLSchema\">");
    AXML.append("<SOAP-ENV:Body>");
    AXML.append("<SetWLANNoSecurity>");
    AXML.append("<NewRadio xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewRadio+"</NewRadio>");
    AXML.append("<NewSSID xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewSSID+"</NewSSID>");
    AXML.append("<NewChannel xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewChannel+"</NewChannel>");
    AXML.append("<NewWirelessMode xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewWirelessMode+"</NewWirelessMode>");
    AXML.append("</SetWLANNoSecurity>");
    AXML.append("</SOAP-ENV:Body>");
    AXML.append("</SOAP-ENV:Envelope>");

    QString SOAPAction = "urn:NETGEAR-ROUTER:service:WLANConfiguration:1#SetWLANNoSecurity";
    QString data = AXML;
    qDebug() << data;

    QNetworkRequest networkReq;
    networkReq.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("text/xml;charset=utf-8"));
    networkReq.setRawHeader("SOAPAction", SOAPAction.toUtf8());
    networkReq.setUrl(QUrl("http://" + g_host + ":" + port + "/soap/server_sa/"));


    manager->post(networkReq, data.toUtf8());
}
void ExtenderSoapApi::SetWLANWEPByKeys(QNetworkAccessManager *manager, QString port,QString NewRadio ,QString NewSSID, QString NewChannel,
                               QString NewWirelessMode,QString NewWEPAuthType,
                               int NewWEPLength, int NewKeyIndex,
                               QString NewKey1 ,QString NewKey2,
                               QString NewKey3,QString NewKey4)
{
    QString AXML = "";
    AXML.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    AXML.append("<SOAP-ENV:Envelope ");
    AXML.append("xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"");
    AXML.append("xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\"");
    AXML.append("xmlns:xsd=\"http://www.w3.org/1999/XMLSchema\">");
    AXML.append("<SOAP-ENV:Body>");
    AXML.append("<SetWLANWEPByKeys>");
    AXML.append("<NewRadio xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewRadio+"</NewRadio>");
    AXML.append("<NewSSID xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewSSID+"</NewSSID>");
    AXML.append("<NewChannel xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewChannel+"</NewChannel>");
    AXML.append("<NewWirelessMode xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewWirelessMode+"</NewWirelessMode>");
    AXML.append("<NewWEPAuthType xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewWEPAuthType+"</NewWEPAuthType>");
    AXML.append("<NewWEPLength xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+QString::number(NewWEPLength) +"</NewWEPLength>");
    AXML.append("<NewKeyIndex xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+ QString::number(NewKeyIndex) +"</NewKeyIndex>");
    AXML.append("<NewKey1 xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewKey1+"</NewKey1>");
    AXML.append("<NewKey2 xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewKey2+"</NewKey2>");
    AXML.append("<NewKey3 xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewKey3+"</NewKey3>");
    AXML.append("<NewKey4 xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewKey4+"</NewKey4>");
    AXML.append("</SetWLANWEPByKeys>");
    AXML.append("</SOAP-ENV:Body>");
    AXML.append("</SOAP-ENV:Envelope>");

    QString SOAPAction = "urn:NETGEAR-ROUTER:service:WLANConfiguration:1#SetWLANWEPByKeys";
    QString data = AXML;
    qDebug() << data;

    QNetworkRequest networkReq;
    networkReq.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("text/xml;charset=utf-8"));
    networkReq.setRawHeader("SOAPAction", SOAPAction.toUtf8());
    networkReq.setUrl(QUrl("http://" + g_host + ":" + port + "/soap/server_sa/"));


    manager->post(networkReq, data.toUtf8());
}
void ExtenderSoapApi::SetWLANWPAPSKByPassphrase(QNetworkAccessManager *manager, QString port,QString NewRadio,QString NewSSID,
                                        QString NewChannel,QString NewWirelessMode,
                                        QString NewWPAEncryptionModes,QString NewWPAPassphrase)
{
    QString AXML = "";
    AXML.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    AXML.append("<SOAP-ENV:Envelope ");
    AXML.append("xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"");
    AXML.append("xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\" ");
    AXML.append("xmlns:xsd=\"http://www.w3.org/1999/XMLSchema\">");
    AXML.append("<SOAP-ENV:Body>");
    AXML.append("<SetWLANWPAPSKByPassphrase>");
    AXML.append("<NewRadio xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewRadio+"</NewRadio>");
    AXML.append("<NewSSID xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewSSID+"</NewSSID>");
    AXML.append("<NewChannel xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewChannel+"</NewChannel>");
    AXML.append("<NewWirelessMode xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewWirelessMode+"</NewWirelessMode>");
    AXML.append("<NewWPAEncryptionModes xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewWPAEncryptionModes+"</NewWPAEncryptionModes>");
    AXML.append("<NewWPAPassphrase xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewWPAPassphrase+"</NewWPAPassphrase>");
    AXML.append("</SetWLANWPAPSKByPassphrase>");
    AXML.append("</SOAP-ENV:Body>");
    AXML.append("</SOAP-ENV:Envelope>");

    QString SOAPAction = "urn:NETGEAR-ROUTER:service:WLANConfiguration:1#SetWLANWPAPSKByPassphrase";
    QString data = AXML;
    qDebug() << data;
    QNetworkRequest networkReq;
    networkReq.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("text/xml;charset=utf-8"));
    networkReq.setRawHeader("SOAPAction", SOAPAction.toUtf8());
    networkReq.setUrl(QUrl("http://" + g_host + ":" + port + "/soap/server_sa/"));


    manager->post(networkReq, data.toUtf8());
}
void ExtenderSoapApi::ConfigurationFinished(QNetworkAccessManager *manager, QString port, QString NewSessionID)
{
    QString AXML = "";
    AXML.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    AXML.append("<SOAP-ENV:Envelope ");
    AXML.append("xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"");
    AXML.append("xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\" ");
    AXML.append("xmlns:xsd=\"http://www.w3.org/1999/XMLSchema\">");
    AXML.append("<SOAP-ENV:Body>");
    AXML.append("<ConfigurationFinished>");
    AXML.append("<NewSessionID xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewSessionID+"</NewSessionID>");
    AXML.append("<NewConfigStatus xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">1</NewConfigStatus>");
    AXML.append("</ConfigurationFinished>");
    AXML.append("</SOAP-ENV:Body>");
    AXML.append("</SOAP-ENV:Envelope>");

    QString SOAPAction = "urn:NETGEAR-ROUTER:service:DeviceConfig:1#ConfigurationFinished";
    QString data = AXML;
    qDebug() << data;

    soapSend = data;

    QNetworkRequest networkReq;
    networkReq.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("text/xml;charset=utf-8"));
    networkReq.setRawHeader("SOAPAction", SOAPAction.toUtf8());
    networkReq.setUrl(QUrl("http://" + g_host + ":" + port + "/soap/server_sa/"));


    manager->post(networkReq, data.toUtf8());
}

void ExtenderSoapApi::SetEnable(QNetworkAccessManager *manager, QString port)
{
    QString AXML = "";
    AXML.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    AXML.append("<SOAP-ENV:Envelope ");
    AXML.append("xmlns:SOAP-EN  V=\"http://schemas.xmlsoap.org/soap/envelope/\" ");
    AXML.append("xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\"");
    AXML.append("xmlns:xsd=\"http://www.w3.org/1999/XMLSchema\">");
    AXML.append("<SOAP-ENV:Body>");
    AXML.append("<SetEnable>");
    AXML.append("<NewEnable xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">0</NewEnable>");
    AXML.append("</SetEnable>");
    AXML.append("</SOAP-ENV:Body>");
    AXML.append("</SOAP-ENV:Envelope>");

    QString SOAPAction = "urn:NETGEAR-ROUTER:service:DeviceConfig:1#SetEnable";
    QString data = AXML;
    qDebug() << data;

    soapSend = data;

    QNetworkRequest networkReq;
    networkReq.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("text/xml;charset=utf-8"));
    networkReq.setRawHeader("SOAPAction", SOAPAction.toUtf8());
    networkReq.setUrl(QUrl("http://" + g_host + ":" + port + "/soap/server_sa/"));

    manager->post(networkReq, data.toUtf8());
}

// get configuration for advanced set
void ExtenderSoapApi::GetRouterWLANInfo(QNetworkAccessManager *manager, QString port, QString NewRadio)
{
    QString AXML = "";
    AXML.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    AXML.append("<SOAP-ENV:Envelope ");
    AXML.append("xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"");
    AXML.append("xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\" ");
    AXML.append("xmlns:xsd=\"http://www.w3.org/1999/XMLSchema\">");
    AXML.append("<SOAP-ENV:Body>");
    AXML.append("<GetRouterWLANInfo>");
    AXML.append("<NewRadio xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewRadio+"</NewRadio>");
    AXML.append("</GetRouterWLANInfo>");
    AXML.append("</SOAP-ENV:Body>");
    AXML.append("</SOAP-ENV:Envelope>");


    qDebug()<< AXML;
    QString SOAPAction = "urn:NETGEAR-ROUTER:service:WLANConfiguration:1#GetRouterWLANInfo";
    QString data = AXML;

    QNetworkRequest networkReq;
    networkReq.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("text/xml;charset=utf-8"));
    networkReq.setRawHeader("SOAPAction", SOAPAction.toUtf8());
    networkReq.setUrl(QUrl("http://" + g_host + ":" + port + "/soap/server_sa/"));

    manager->post(networkReq, data.toUtf8());
}

void ExtenderSoapApi::GetRouterWLANWPAInfo(QNetworkAccessManager *manager, QString port, QString NewRadio)
{
    QString AXML = "";
    AXML.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    AXML.append("<SOAP-ENV:Envelope ");
    AXML.append("xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"");
    AXML.append("xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\" ");
    AXML.append("xmlns:xsd=\"http://www.w3.org/1999/XMLSchema\">");
    AXML.append("<SOAP-ENV:Body>");
    AXML.append("<GetRouterWLANWPAInfo>");
    AXML.append("<NewRadio xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewRadio+"</NewRadio>");
    AXML.append("</GetRouterWLANWPAInfo>");
    AXML.append("</SOAP-ENV:Body>");
    AXML.append("</SOAP-ENV:Envelope>");


    qDebug()<< AXML;
    QString SOAPAction = "urn:NETGEAR-ROUTER:service:WLANConfiguration:1#GetRouterWLANWPAInfo";
    QString data = AXML;

    QNetworkRequest networkReq;
    networkReq.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("text/xml;charset=utf-8"));
    networkReq.setRawHeader("SOAPAction", SOAPAction.toUtf8());
    networkReq.setUrl(QUrl("http://" + g_host + ":" + port + "/soap/server_sa/"));

    manager->post(networkReq, data.toUtf8());
}

void ExtenderSoapApi::GetRouterWLANWEPInfo(QNetworkAccessManager *manager, QString port, QString NewRadio)
{
    QString AXML = "";
    AXML.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    AXML.append("<SOAP-ENV:Envelope ");
    AXML.append("xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"");
    AXML.append("xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\" ");
    AXML.append("xmlns:xsd=\"http://www.w3.org/1999/XMLSchema\">");
    AXML.append("<SOAP-ENV:Body>");
    AXML.append("<GetWLANWEPInfo>");
    AXML.append("<NewRadio xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewRadio+"</NewRadio>");
    AXML.append("</GetWLANWEPInfo>");
    AXML.append("</SOAP-ENV:Body>");
    AXML.append("</SOAP-ENV:Envelope>");


    qDebug()<< AXML;
    QString SOAPAction = "urn:NETGEAR-ROUTER:service:WLANConfiguration:1#GetWLANWEPInfo";
    QString data = AXML;

    QNetworkRequest networkReq;
    networkReq.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("text/xml;charset=utf-8"));
    networkReq.setRawHeader("SOAPAction", SOAPAction.toUtf8());
    networkReq.setUrl(QUrl("http://" + g_host + ":" + port + "/soap/server_sa/"));

    manager->post(networkReq, data.toUtf8());
}

void ExtenderSoapApi::GetWLANInfo(QNetworkAccessManager *manager, QString port, QString NewRadio)
{
    QString AXML = "";
    AXML.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    AXML.append("<SOAP-ENV:Envelope ");
    AXML.append("xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"");
    AXML.append("xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\" ");
    AXML.append("xmlns:xsd=\"http://www.w3.org/1999/XMLSchema\">");
    AXML.append("<SOAP-ENV:Body>");
    AXML.append("<GetWLANInfo>");
    AXML.append("<NewRadio xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewRadio+"</NewRadio>");
    AXML.append("</GetWLANInfo>");
    AXML.append("</SOAP-ENV:Body>");
    AXML.append("</SOAP-ENV:Envelope>");


    qDebug()<< AXML;
    QString SOAPAction = "urn:NETGEAR-ROUTER:service:WLANConfiguration:1#GetWLANInfo";
    QString data = AXML;

    QNetworkRequest networkReq;
    networkReq.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("text/xml;charset=utf-8"));
    networkReq.setRawHeader("SOAPAction", SOAPAction.toUtf8());
    networkReq.setUrl(QUrl("http://" + g_host + ":" + port + "/soap/server_sa/"));

    manager->post(networkReq, data.toUtf8());
}

void ExtenderSoapApi::GetWLANWPAInfo(QNetworkAccessManager *manager, QString port, QString NewRadio)
{
    QString AXML = "";
    AXML.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    AXML.append("<SOAP-ENV:Envelope ");
    AXML.append("xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"");
    AXML.append("xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\" ");
    AXML.append("xmlns:xsd=\"http://www.w3.org/1999/XMLSchema\">");
    AXML.append("<SOAP-ENV:Body>");
    AXML.append("<GetWLANWPAInfo>");
    AXML.append("<NewRadio xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewRadio+"</NewRadio>");
    AXML.append("</GetWLANWPAInfo>");
    AXML.append("</SOAP-ENV:Body>");
    AXML.append("</SOAP-ENV:Envelope>");


    qDebug()<< AXML;
    QString SOAPAction = "urn:NETGEAR-ROUTER:service:WLANConfiguration:1#GetWLANWPAInfo";
    QString data = AXML;

    QNetworkRequest networkReq;
    networkReq.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("text/xml;charset=utf-8"));
    networkReq.setRawHeader("SOAPAction", SOAPAction.toUtf8());
    networkReq.setUrl(QUrl("http://" + g_host + ":" + port + "/soap/server_sa/"));

    manager->post(networkReq, data.toUtf8());
}

void ExtenderSoapApi::GetWLANWEPInfo(QNetworkAccessManager *manager, QString port, QString NewRadio)
{
    QString AXML = "";
    AXML.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    AXML.append("<SOAP-ENV:Envelope ");
    AXML.append("xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"");
    AXML.append("xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\" ");
    AXML.append("xmlns:xsd=\"http://www.w3.org/1999/XMLSchema\">");
    AXML.append("<SOAP-ENV:Body>");
    AXML.append("<GetWLANWEPInfo>");
    AXML.append("<NewRadio xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewRadio+"</NewRadio>");
    AXML.append("</GetWLANWEPInfo>");
    AXML.append("</SOAP-ENV:Body>");
    AXML.append("</SOAP-ENV:Envelope>");


    qDebug()<< AXML;
    QString SOAPAction = "urn:NETGEAR-ROUTER:service:WLANConfiguration:1#GetWLANWEPInfo";
    QString data = AXML;

    QNetworkRequest networkReq;
    networkReq.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("text/xml;charset=utf-8"));
    networkReq.setRawHeader("SOAPAction", SOAPAction.toUtf8());
    networkReq.setUrl(QUrl("http://" + g_host + ":" + port + "/soap/server_sa/"));

    manager->post(networkReq, data.toUtf8());
}

void ExtenderSoapApi::ResetToFactoryDefault(QNetworkAccessManager *manager, QString port, QString NewSerialNumber)
{
    QString AXML = "";
    AXML.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    AXML.append("<SOAP-ENV:Envelope ");
    AXML.append("xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"");
    AXML.append("xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\" ");
    AXML.append("xmlns:xsd=\"http://www.w3.org/1999/XMLSchema\">");
    AXML.append("<SOAP-ENV:Body>");
    AXML.append("<ResetToFactoryDefault>");
    AXML.append("<SerialNumber xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">"+NewSerialNumber+"</SerialNumber>");
    AXML.append("</ResetToFactoryDefault>");
    AXML.append("</SOAP-ENV:Body>");
    AXML.append("</SOAP-ENV:Envelope>");

    QString SOAPAction = "urn:NETGEAR-ROUTER:service:DeviceConfig:1#ResetToFactoryDefault";
    QString data = AXML;
    qDebug() << data;

    soapSend = data;

    QNetworkRequest networkReq;
    networkReq.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("text/xml;charset=utf-8"));
    networkReq.setRawHeader("SOAPAction", SOAPAction.toUtf8());
    networkReq.setUrl(QUrl("http://" + g_host + ":" + port + "/soap/server_sa/"));


    manager->post(networkReq, data.toUtf8());
}

/**********
 *DeviceInfo
 **********/

void ExtenderSoapApi::GetInfo(QNetworkAccessManager *manager, QString port)
{
    QString AXML = "";
    AXML.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    AXML.append("<SOAP-ENV:Envelope ");
    AXML.append("xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" ");
    AXML.append("xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\"");
    AXML.append("xmlns:xsd=\"http://www.w3.org/1999/XMLSchema\">");
    AXML.append("<SOAP-ENV:Body>");
    AXML.append("</SOAP-ENV:Body>");
    AXML.append("</SOAP-ENV:Envelope>");

    QString SOAPAction = "urn:NETGEAR-ROUTER:service:DeviceInfo:1#GetInfo";
    QString data = AXML;
    qDebug() << data;

    soapSend = data;

    QNetworkRequest networkReq;
    networkReq.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("text/xml;charset=utf-8"));
    networkReq.setRawHeader("SOAPAction", SOAPAction.toUtf8());
    networkReq.setUrl(QUrl("http://" + g_host + ":" + port + "/soap/server_sa/"));


    manager->post(networkReq, data.toUtf8());
}

void ExtenderSoapApi::GetCurrentSetting(QNetworkAccessManager *manager, QString port)
{
    qDebug() << "==GetCurrentSetting";
    manager->get(QNetworkRequest(QUrl("http://" + g_host + ":" + port + "/currentsetting.htm")));
}

QString ExtenderSoapApi::showSend()
{
    return soapSend;
}
