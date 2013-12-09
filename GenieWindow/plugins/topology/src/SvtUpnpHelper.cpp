#include "topology.h"


extern SVT_IpAddress getDNS(const QString &addr);

SVT_UpnpHelper::SVT_UpnpHelper(SVT_TopoMethod *method)
    :mTopoMethod(method)
//    ,mStarted(false)
{
    connect(&mUdpSock,SIGNAL(readyRead()),this,SLOT(slot_UdpDataRet()));
//    connect(&mHttp,SIGNAL(requestFinished(int,bool)),this,SLOT(slot_HttpReqFinished(int,bool)));
//    connect(&mHttp,SIGNAL(done(bool)),this,SLOT(slot_HttpDone(bool)));
    connect(&mTimer,SIGNAL(timeout()),this,SLOT(slot_Timeout()));
    SVT_NodePool *pool=mTopoMethod->mNodePool;
	quint32 iface_ip=pool->mInterface.getIp().asLong();

	// mUdpSock.bind(5826,QUdpSocket::ReuseAddressHint | QUdpSocket::ShareAddress);
	for(int i=0;i<10;i++)
	{
		QHostAddress bindaddr(iface_ip);
		qDebug()<<SVT_IpAddress(iface_ip).toString();
		if(mUdpSock.bind(bindaddr,5826,QUdpSocket::ShareAddress))
		{
			break;
		}
	}
}
SVT_UpnpHelper::~SVT_UpnpHelper()
{
    disconnect(&mUdpSock,SIGNAL(readyRead()),0,0);
//    disconnect(&mHttp,SIGNAL(requestFinished(int,bool)),0,0);
//    disconnect(&mHttp,SIGNAL(done(bool)),0,0);
    disconnect(&mTimer,SIGNAL(timeout()),0,0);
    mTimer.stop();
    mUdpSock.close();
//    mHttp.close();
    foreach(GenieHttp *http,mHttpMapIp.keys())
    {
        disconnect(http,SIGNAL(requestFinished(int,bool)),0,0);
        disconnect(http,SIGNAL(done(bool)),0,0);
        http->abort();
        delete http;
    }
}

void SVT_UpnpHelper::slot_UdpDataRet()
{
   // qDebug()<<"void SVT_UpnpHelper::slot_UdpDataRet()";
    while (mUdpSock.hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(mUdpSock.pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        mUdpSock.readDatagram(datagram.data(), datagram.size(),
                              &sender, &senderPort);
        SVT_NodePool *pool=mTopoMethod->mNodePool;
        quint32 _ip=sender.toIPv4Address();
        quint32 iface_mask=pool->mInterface.getMask().asLong();
        quint32 iface_ip=pool->mInterface.getIp().asLong();

        if((iface_mask&_ip)==(iface_mask&iface_ip))
        {
            processTheDatagram(datagram,_ip);
        }
    }
}

void SVT_UpnpHelper::processTheDatagram(QByteArray &datagram,const quint32 &ip)
{
    static int i=0;
    QStringList lines;
    QString data = QString(datagram);
    lines = data.split("\r\n");
    QStringList locationes = lines.filter("Location:", Qt::CaseInsensitive);
    if(locationes.size() > 0)
    {
        QString location = locationes.value(0);
        location.remove("Location:", Qt::CaseInsensitive).remove(" ");

        if(!mHttpMapLocation.values().contains(location)
                /*&& !mIpMapInfo.contains(location)*/)
        {
            QUrl url(location);
            qDebug()<<url.host()<<url.port()<<url.toString();
            SVT_IpAddress verify_ip=getDNS(url.host());
            if(verify_ip.asLong()==ip)
            {
                GenieHttp *http=new GenieHttp;
                // mHttpPool.insert(http);
                connect(http,SIGNAL(requestFinished(int,bool)),this,SLOT(slot_HttpReqFinished(int,bool)));
                connect(http,SIGNAL(done(bool)),this,SLOT(slot_HttpDone(bool)));
                http->setHost(url.host(),url.port());
                int req=http->get(url.path());
                mHttpMapIp.insert(http,ip);
                mHttpMapReq.insert(http,req);
                mHttpMapLocation.insert(http,location);
            }
            else
            {
                qDebug()<<"upnp error of device"<<SVT_IpAddress(ip).toString();
            }
        }
     }
}

void SVT_UpnpHelper::parseHttpData(QByteArray &data, QMap<int, QString> &retmap)
{
    retmap.clear();
    QDomDocument doc;
    int errorLine, errorColumn;
    QString errorMsg;

     if (!doc.setContent(data, true, &errorMsg,
                          &errorLine, &errorColumn)) {
          return ;
      }

     QDomElement root = doc.documentElement();
     if(root.tagName().compare("root", Qt::CaseInsensitive) != 0)
     {
         return ;
     }
     QDomNode child = root.firstChild();
     const struct _t{
         const char *tagname;
         int targetidx;
     } cParseParas[]=
     {
         {"devicetype",NIE_UPNP_DEVICETYPE},
         {"friendlyname",NIE_UPNP_FRIENDLYNAME},
         {"manufacturer",NIE_UPNP_MANUFACTURER},
         {"manufacturerURL",NIE_UPNP_MANUFACTURER_URL},
         {"modelDescription",NIE_UPNP_MODELDISCRIPTION},
         {"modelName",NIE_UPNP_MODELNAME}
     };

     while(!child.isNull())
     {
         if(child.toElement().tagName().compare("device", Qt::CaseInsensitive) == 0)
         {

             QDomNode childchild = child.firstChild();
             QString qstemp;
             while(!childchild.isNull())
             {
                 QString tagname = childchild.toElement().tagName();
                 for(int i=0;i<sizeof(cParseParas)/sizeof(cParseParas[0]);i++)
                 {
                     if(tagname.compare(cParseParas[i].tagname,Qt::CaseInsensitive)==0)
                     {
                         qstemp=childchild.toElement().text();
                         if(!qstemp.isEmpty())
                         {
                             retmap.insert(cParseParas[i].targetidx,qstemp);
                         }
                         break;
                     }
                 }

                 childchild = childchild.nextSibling();
             }
             return;
         }
        else
         {
             child = child.nextSibling();
         }
     }


}

int compareUpnpDeviceType(const QString &d1,const QString &d2)
{
    const char *cDevs[]={
        "urn:schemas-upnp-org:device:MediaServer:1",
        "urn:schemas-upnp-org:device:networkstoragedevice:1",
        "urn:schemas-wifialliance-org:device:WFADevice:1"
    };
    int id1=-1;
    int id2=-1;
    for(int i=0;i<sizeof(cDevs)/sizeof(cDevs[0]);i++)
    {
        if(d1==cDevs[i])
        {
            id1=i;
        }
        if(d2==cDevs[i])
        {
            id2=i;
        }
        if(id1>=0 && id2>=0)
        {
            break;
        }
    }
    return id1-id2;
}

bool SVT_UpnpHelper::mergeUpnpInfo(GenieHttp *http,const QMap<int,QString> &map)
{
    Q_ASSERT(mHttpMapLocation.contains(http));
    Q_ASSERT(mHttpMapIp.contains(http));
    quint32 ip=mHttpMapIp.value(http);
    if(map.isEmpty())
    {
        return false;
    }

    if(!mIpMapInfo.contains(ip))
    {
        mIpMapInfo.insert(ip,map);
        return true;
    }
    else
    {
        QMap<int,QString> &oldmap=mIpMapInfo[ip];
        if(compareUpnpDeviceType(map.value(NIE_UPNP_DEVICETYPE),oldmap.value(NIE_UPNP_DEVICETYPE))>0 )
        {
            mIpMapInfo[ip]=map;
            return true;
        }
    }
    return false;
}

void SVT_UpnpHelper::slot_HttpReqFinished( int id, bool err )
{

    GenieHttp *http=qobject_cast<GenieHttp *>(sender());

    if(err || !mHttpMapIp.contains(http) ||id != mHttpMapReq.value(http,-1))
    {
        return;
    }


    QByteArray data=http->readAll();


    SVT_NodeInfoRef nodeInfoRef(new QMap<int,QString>);
    parseHttpData(data,*nodeInfoRef);

    Q_ASSERT(mTopoMethod);
    if(!nodeInfoRef->isEmpty() && mergeUpnpInfo(http,*(nodeInfoRef.asPointer())))
    {
        quint32 _ip=mHttpMapIp.value(http);
        //            const SVT_IpAddress &srcip=pool->mInterface.getIp();

        SVT_NodePool *pool=mTopoMethod->mNodePool;
//        quint32 iface_mask=pool->mInterface.getMask().asLong();
        quint32 iface_ip=pool->mInterface.getIp().asLong();

//        if((iface_mask&_ip)==(iface_mask&iface_ip))
//        {
            SVT_IpAddress ip;
            ip.Set(_ip);
            SVT_QtUtils &util=CORE_UTIL();
            bool btemp;
#ifdef Q_OS_WIN32
            SVT_MacAddress mac=util.netscanGetMacForIp(ip,iface_ip,&btemp);
#else
            SVT_MacAddress mac;
#endif

            if(btemp)
            {
                pool->nodeDetected(mTopoMethod,ip,mac,nodeInfoRef);
            }
       // }
    }
    mHttpMapIp.remove(http);
    mHttpMapReq.remove(http);
    mHttpMapLocation.remove(http);


}

void SVT_UpnpHelper::slot_HttpDone(bool err)
{
	Q_UNUSED(err)
    GenieHttp *http=qobject_cast<GenieHttp *>(sender());
    disconnect(http,SIGNAL(requestFinished(int,bool)),0,0);
    disconnect(http,SIGNAL(done(bool)),0,0);
    http->deleteLater();
    mHttpMapIp.remove(http);
    mHttpMapReq.remove(http);
    mHttpMapLocation.remove(http);
}

void SVT_UpnpHelper::start()
{
    slot_Timeout();
    mTimer.start(1000);
    mEventLoop.exec();
}

void SVT_UpnpHelper::slot_Timeout()
{
    static int sCounter=0;
    SVT_NodePool *pool=mTopoMethod->mNodePool;
    if(pool->mPoolExit)
    {
        mEventLoop.exit();
    }
    if((sCounter++)%UPNP_LOOP_INTERVAL!=0)
    {
        return;
    }
    QHostAddress host;//=;



    host.setAddress("239.255.255.250");
    const static char request[]=
            "\x4D\x2D\x53\x45\x41\x52\x43\x48\x20\x2A\x20\x48\x54\x54\x50\x2F"
            "\x31\x2E\x31\x0D\x0A\x4D\x58\x3A\x20\x35\x0D\x0A\x53\x54\x3A\x20"
            "\x75\x70\x6E\x70\x3A\x72\x6F\x6F\x74\x64\x65\x76\x69\x63\x65\x0D"
            "\x0A\x4D\x41\x4E\x3A\x20\x22\x73\x73\x64\x70\x3A\x64\x69\x73\x63"
            "\x6F\x76\x65\x72\x22\x0D\x0A\x48\x6F\x73\x74\x3A\x20\x32\x33\x39"
            "\x2E\x32\x35\x35\x2E\x32\x35\x35\x2E\x32\x35\x30\x3A\x31\x39\x30"
            "\x30\x0D\x0A\x43\x6F\x6E\x74\x65\x6E\x74\x2D\x4C\x65\x6E\x67\x74"
            "\x68\x3A\x20\x30\x0D\x0A\x0D\x0A";


    mUdpSock.writeDatagram(request,sizeof(request)-1, host, 1900);
    mUdpSock.writeDatagram(request,sizeof(request)-1, host, 1900);
    mUdpSock.writeDatagram(request,sizeof(request)-1, host, 1900);
}

//void SVT_UpnpHelper::stop()
//{
//    mEventLoop.quit();
//}
