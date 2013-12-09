#include "SvtSoap.h"
#include <QDebug>
#include "SvtNetworkTool.h"
#include <QDomDocument>
#include <QString>

extern QString ROUTER_HOST;

SVT_Soap::SVT_Soap(bool *exitFlag)
:mExitFlag(exitFlag)
,mFlags(0)
,mAutoChangePort(false)
{
//    mHost="routerlogin.net";
}

void SVT_Soap::setExitFlag(bool *para)
{
    mExitFlag=para;
}

void SVT_Soap::setAutoChangePort(bool b)
{
    mAutoChangePort=b;
}

//void SVT_Soap::setHost(const QString &host)
//{
//    mHost=host;
//}


void parseNode(QDomNode &node,QMap<QString, QString> *retmap)
{
    if (node.isNull() || !node.isElement())
    {
      return;
    }
    QDomElement elem = node.toElement();
    if(node.firstChild().isElement())
    {
        QDomNodeList children=node.childNodes();
        int c=children.count();
        for(int i=0;i<c;i++)
        {
            QDomNode cn=children.item(i);
            parseNode(cn,retmap);

        }
        return;
    }

    QString tagname=node.toElement().tagName();
    int pos=tagname.indexOf(":");
    if(pos >=0)
    {
        tagname=tagname.right(tagname.length() - pos - 1);
    }

    retmap->insert(tagname,node.toElement().text().replace("&amp;","&"));
}

bool SVT_Soap::_doFunc(const QString &host, const QString &module, const QString &method, quint32 port, const QMap<QString, QString> *paras, QMap<QString, QString> *retmap){
    if((mFlags&IS_CGDG) ||isSetApi(module,method))
    {
        sendStart(host,port);
    }
    if(retmap)
    {
        retmap->clear();
    }
    const char *cHeadMode="POST /soap/server_sa/ HTTP/1.0\r\n"
            "SOAPAction: urn:NETGEAR-ROUTER:service:%1:1#%2\r\n"
            "content-type: text/xml;charset=utf-8\r\n"
            "HOST: www.routerlogin.com\r\n"
            "User-Agent: SOAP Toolkit 3.0\r\n"
            "connection: keep-Alive\r\n"
            "Cache-Control: no-cache\r\n"
            "Pragma: no-cache\r\n"
            "content-length: %3\r\n\r\n";
    const char *cBodyMode=
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
                "</SOAP-ENV:Envelope>";
    QString s_para="";
    if(paras)
    {
        QMap<QString, QString>::const_iterator i = paras->begin();
        while (i != paras->end()) {
            s_para+=QString("<%1>%2</%1>").arg(i.key(),i.value());
            ++i;
        }
    }

    QString body=QString(cBodyMode).arg(module,method,s_para);
    QString head=QString(cHeadMode).arg(module,method).arg(body.length());

    if(0==module.compare("ParentalControl",Qt::CaseInsensitive))
    {
        cHeadMode="POST /soap/server_sa/ HTTP/1.0\r\n"
                  "SOAPAction: urn:NETGEAR-ROUTER:service:%1:1#%2\r\n"
                  "content-type: text/xml;charset=utf-8\r\n"
                  "HOST: www.routerlogin.com\r\n"
                  "User-Agent: SOAP Toolkit 3.0\r\n"
                  "connection: keep-Alive\r\n"
                  "Cache-Control: no-cache\r\n"
                  "Pragma: no-cache\r\n"
                  "content-length: %3\r\n\r\n";
        cBodyMode="<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\">\n"
                  "<SOAP-ENV:Header>\n"
                  "<SessionID xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\">E6A88AE69687E58D9A00</SessionID>\n"
                  "</SOAP-ENV:Header>\n"
                  "<SOAP-ENV:Body>\n"
                  "<%1>\n"
          "%2"
                  "</%1>\n"
                  "</SOAP-ENV:Body>\n"
                  "</SOAP-ENV:Envelope>\n";
        const char *cParaString=" xsi:type=\"xsd:string\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\"";
        s_para="";
        if(paras)
        {
            QMap<QString, QString>::const_iterator i = paras->begin();
            while (i != paras->end()) {
                s_para+=QString("<%1%3>%2</%1>\n").arg(i.key(),i.value(),cParaString);
                ++i;
            }
        }
        body=QString(cBodyMode).arg(method,s_para);
        head=QString(cHeadMode).arg(module,method).arg(body.length());

    }

    SVT_Http http(mExitFlag);
    QByteArray &retarray=http.rawRequest(host,head+body,port);

    qDebug()<<retarray;


    int retv=getRetValue(retarray);
    bool retOk=(200==retv);
    if(retOk && retmap)
    {
        qDebug()<<"start parse";
        QDomDocument doc;
        QByteArray body=SVT_Http::getBody(retarray);

		body = body.replace('&',"&amp;");

        doc.setContent(body,true);
//        QDomNode root=doc.namedItem()
//        doc.
        //QString tagname=method+"Response";
        QDomNode rnode = doc.firstChild();
        if (!rnode.isElement())
            rnode = rnode.nextSibling();
        parseNode(rnode,retmap);



    }
    if((retOk)&&((mFlags&IS_CGDG) ||isSetApi(module,method)))
    {
        sendFinish(host,port);
    }
    if(!retOk && retmap)
    {
        retmap->insert("error",QString::number(retv));
    }

    return retOk;
}

bool SVT_Soap::doFunc(const char *host, const char *module, const char *method
                      ,const QMap<QString,QString> *paras,QMap<QString,QString> *retmap)
{
    return doFunc(QString(host),QString(module),QString(method),paras,retmap);
}

bool SVT_Soap::doFunc(const QString &host, const QString &module, const QString &method, const QMap<QString, QString> *paras, QMap<QString, QString> *retmap)
{
    //auto change port from port 80 and 5000
    int port=(mFlags & IS_5000PORT)?5000:80;
    if(_doFunc(host,module,method,port,paras,retmap))
    {
        return true;
//        bool btemp;
//        if(!mAutoChangePort)
//        {
//            return true;
//        }
//        if(retmap->count()>0 && retmap->contains("ResponseCode")
//            && (0==(*retmap)["ResponseCode"].toInt(&btemp)) &&btemp )
//            {
//            return true;
//        }
    }

    if(mAutoChangePort)
    {
        port=(mFlags & IS_5000PORT)?80:5000;
        if(_doFunc(host,module,method,port,paras,retmap))
        {
            mFlags=mFlags^IS_5000PORT;
            return true;
        }
    }

    return false;
}


void SVT_Soap::setRouteType(const QString &routetype)
{
    QRegExp rx("^CG");
    QRegExp rx2("^DG");

    //which router use 5000 port
    const char * cPara[]={"WNDR3400","WNDR4500"};
    if(routetype.contains(rx)||routetype.contains(rx2))
    {
        mFlags |= IS_CGDG;
    }
    for(int i=0;i<sizeof(cPara)/sizeof(cPara[0]);i++)
    {
        if(routetype.contains(cPara[i],Qt::CaseInsensitive))
        {
            mFlags |= IS_5000PORT;
        }
    }
}

int SVT_Soap::getRetValue(const QByteArray &retarray)
{
    QByteArray header=SVT_Http::getHeader(retarray);
//    qDebug()<<header<<QString(header).split(QRegExp("\r|\n|\r\n"));
    QString stemp=QString(header).split(QRegExp("\r|\n|\n")).value(0,"")
            .split(QRegExp("\\s+"), QString::SkipEmptyParts).value(1,"");
    bool btemp;
    int ret;
    //qDebug()<<stemp;
    ret=stemp.toInt(&btemp);
    return btemp?ret:-99;

}

void SVT_Soap::sendStart(const QString &host, quint16 port)
{
    const char *cDatas=
            "POST /soap/server_sa/ HTTP/1.0\r\n"
            "SOAPAction: urn:NETGEAR-ROUTER:service:DeviceConfig:1#ConfigurationStarted\r\n"
            "content-type: text/xml;charset=utf-8\r\n"
            "HOST: www.routerlogin.com\r\n"
            "User-Agent: SOAP Toolkit 3.0\r\n"
            "connection: keep-Alive\r\n"
            "Cache-Control: no-cache\r\n"
            "Pragma: no-cache\r\n"
            "content-length: 504\r\n"
            "\r\n"
            "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><SOAP-ENV:Envelope xmlns:SOAPSDK1=\"http://www.w3.org/2001/XMLSchema\" xmlns:SOAPSDK2=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:SOAPSDK3=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"><SOAP-ENV:Body><M1:ConfigurationStarted xmlns:M1=\"urn:NETGEAR-ROUTER:service:DeviceConfig:1\"><NewSessionID>58DEE6006A88A967E89A</NewSessionID></M1:ConfigurationStarted></SOAP-ENV:Body></SOAP-ENV:Envelope>"
            ;
    SVT_Http http(mExitFlag);
    /*QByteArray &retarray=*/http.rawRequest(host,cDatas,port);
}

void SVT_Soap::sendFinish(const QString &host, quint16 port)
{
    const char *cDatas="POST /soap/server_sa/ HTTP/1.0\r\n"
    "SOAPAction: urn:NETGEAR-ROUTER:service:DeviceConfig:1#ConfigurationFinished\r\n"
            "content-type: text/xml;charset=utf-8\r\n"
    "HOST: www.routerlogin.com\r\n"
    "User-Agent: SOAP Toolkit 3.0\r\n"
    "connection: keep-Alive\r\n"
    "Cache-Control: no-cache\r\n"
    "Pragma: no-cache\r\n"
    "content-length: 572\r\n"
            "\r\n"
    "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><SOAP-ENV:Envelope xmlns:SOAPSDK1=\"http://www.w3.org/2001/XMLSchema\" xmlns:SOAPSDK2=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:SOAPSDK3=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\"><SOAP-ENV:Header><SessionID>58DEE6006A88A967E89A</SessionID></SOAP-ENV:Header><SOAP-ENV:Body><M1:ConfigurationFinished xmlns:M1=\"urn:NETGEAR-ROUTER:service:DeviceConfig:1\"><NewStatus>ChangesApplied</NewStatus></M1:ConfigurationFinished></SOAP-ENV:Body></SOAP-ENV:Envelope>"
	;

    SVT_Http http(mExitFlag);
    /*QByteArray &retarray=*/http.rawRequest(host,cDatas,port);

}

bool isSetApi(const QString &module, const QString &method)
{
	Q_UNUSED(module);
    QRegExp rx("^Set");
    QRegExp rx2("^Enable");
	QRegExp rx3("^Add"); ///kai.yan add 
    return (method.contains(rx) || method.contains(rx2) || method.contains(rx3) || method.contains("UpdateNewFirmware") || method.contains("PressWPSPBC")
         ||method=="Reboot"
         ||method=="UpdateNewFirmware"
		 ||method=="UpdateQoSPriority"
		 ||method == "DeleteQoSRule");

}
