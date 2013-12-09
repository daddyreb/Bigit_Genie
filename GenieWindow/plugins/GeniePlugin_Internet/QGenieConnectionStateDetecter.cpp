#include "QGenieConnectionStateDetecter.h"
#include <QUdpSocket>
#include <QHostAddress>
#include <QtEndian>
#include <QtDebug>
#include <assert.h>
#ifdef Q_OS_WIN
#include <DiagnoseInterface.h>
extern DiagnoseInterface *diagnose_inf;
#elif defined Q_OS_MACX
#include "INetApiInterface.h"
extern INetApiInterface *inet_api;
#endif

#include "genie2_interface.h" //kai.yan add
#include "Genieplugin_Internet.h" //kai.yan add
#include "language_Internet.h"

#define TEST_DOMAINNAME "www.netgear.com"
#define DNSSERVER_IPS "208.67.222.222","208.67.220.220","174.35.39.40" //,"8.8.8.8","8.8.4.4","222.247.26.109"//updates1.netgear.com
#ifdef Q_OS_MACX
#define DNSSERVER_PORT 53

#define LOOKUP_TIMEOUT_MS 20000
#endif

static const char * dns_servers[] = {DNSSERVER_IPS};

QGenieConnectionStateDetecter::QGenieConnectionStateDetecter()
    :m_server_index(0)
	,m_isDNS_OK(false)
{
}

QGenieConnectionStateDetecter::~QGenieConnectionStateDetecter()
{
	if(m_DNS_loop)
	{
		m_DNS_loop->exit(0);
	}
}

#ifdef Q_OS_MACX
void QGenieConnectionStateDetecter::generate_request(const QString& dns_name,QByteArray &request){
    request.clear();

    QByteArray  name_bytes;
    QStringList name_list = dns_name.split(".");
    foreach(QString name,name_list)
    {
        name_bytes.append(char(name.length()));
        name_bytes.append(name.toAscii());
    }

    request.fill(char(0),17 + name_bytes.length());

    request[0]                  = char(0xff);
    request[2]                  = char(0x01);
    request[5]                  = char(0x01);
    request[request.length()-3] = char(0x01);
    request[request.length()-1] = char(0x01);

    request.replace(12,name_bytes.length(),name_bytes);

}

bool QGenieConnectionStateDetecter::is_wlan_ok()
{
    //assert (m_socket);

    QUdpSocket      udp_socket;
    udp_socket.bind();

    QHostAddress    peerAddr;
    quint16         peerPort;

    QByteArray request;
    generate_request(TEST_DOMAINNAME,request);

    qDebug () << "destination address" << dns_servers[m_server_index] << "destination port " << DNSSERVER_PORT;
    if(udp_socket.writeDatagram(request,QHostAddress(dns_servers[m_server_index]),DNSSERVER_PORT) < 0)
    {
        return false;
    }

    m_server_index = ((++m_server_index)%(sizeof(dns_servers)/sizeof(dns_servers[0])));

    if(!udp_socket.waitForReadyRead(LOOKUP_TIMEOUT_MS))
    {
        return false;
    }

    if( !udp_socket.hasPendingDatagrams() || udp_socket.pendingDatagramSize() < 0 )
    {
        return false;
    }

    udp_socket.readDatagram(NULL,0,&peerAddr,&peerPort);
    udp_socket.close();

    qDebug() << "peer address" << peerAddr.toString() << "peer port" << peerPort;

        for(unsigned int k=0;k<(sizeof(dns_servers)/sizeof(dns_servers[0]));++k)
        {
    if(!peerAddr.toString().compare(dns_servers[k/*0*/]))
    {
        return true;
    }
        }

    qDebug() << "peer address not right";

    return false;
}

bool QGenieConnectionStateDetecter::is_dnscfg_ok()
{
    QUdpSocket      udp_socket;
    QHostAddress    peerAddr;
    quint16         peerPort;
    QByteArray      request;
    QStringList     dns_list;
    bool            write_ok = false;

    get_dnsservers(dns_list);

    if(dns_list.isEmpty())
    {
        qDebug () << "is_dnscfg_ok dns list is empty!";
        return false;
    }


    //send datagram
    generate_request(TEST_DOMAINNAME,request);
    udp_socket.bind();

    foreach(QString dns_server,dns_list)
    {
        if(udp_socket.writeDatagram(request,QHostAddress(dns_server),DNSSERVER_PORT) < 0)
        {
            continue;
        }

        write_ok = true;
    }

    if(!write_ok)
    {
        qDebug () << "is_dnscfg_ok send datagram failed!";
        return false;
    }

    if(!udp_socket.waitForReadyRead(LOOKUP_TIMEOUT_MS))
    {
        return false;
    }

    if( !udp_socket.hasPendingDatagrams() || udp_socket.pendingDatagramSize() < 0 )
    {
        return false;
    }

    udp_socket.readDatagram(NULL,0,&peerAddr,&peerPort);
    udp_socket.close();

    qDebug() << "is_dnscfg_ok peer address" << peerAddr.toString() << "peer port" << peerPort;

    foreach(QString dns_server,dns_list)
    {
        if(!peerAddr.toString().compare(dns_server))
        {
            return true;
        }
    }

    qDebug() << "is_dnscfg_ok peer address not right";

    return false;

}

void QGenieConnectionStateDetecter::get_dnsservers(QStringList &dns_list)
{
    assert (inet_api);
    inet_api->GetPrimaryServiceDNS(dns_list);
}
#endif

#ifdef Q_OS_WIN
bool QGenieConnectionStateDetecter::is_wlan_ok2()
{
    assert (diagnose_inf);

    bool bresult = false;
	qDebug()<<"dns_servers:"<<QString(dns_servers[m_server_index]);
    bresult = diagnose_inf->IsHostReachble(QString(dns_servers[m_server_index]),3000);
    
//    if(!bresult)
//    {
//        GENIE2_LOG(QString("dns_servers:") + QString(dns_servers[m_server_index]));
//        GENIE2_LOG(QString("is_wlan_ok2 is false"));
//    }
    m_server_index = ((++m_server_index)%(sizeof(dns_servers)/sizeof(dns_servers[0])));
  //  qDebug()<<"is_wlan_ok2 is "<< bresult;

    return bresult;
}

bool QGenieConnectionStateDetecter::is_dnscfg_ok2()
{
//    QString domainname = GeniePlugin_Internet::translateText(L_INTERNET_PING_DOMAIN);
//    qDebug()<<"domainname:"<<domainname;
//    if(domainname == "")
//    {
//        domainname = "www.netgear.com";
//    }
	qDebug()<<"---TEST_DOMAINNAME---- ";
   // QHostInfo hinfo = QHostInfo::fromName(/*domainname*/QString(TEST_DOMAINNAME));
	 // bool isDnsOK =(hinfo.error() == QHostInfo::NoError);

    m_DNS_loop = new QEventLoop();
	QTimer timer;
	timer.setSingleShot(true);
	connect(&timer, SIGNAL(timeout()), m_DNS_loop, SLOT(quit()));
	timer.start(3000); 

	//m_lookupThread = new LookUpThread();
	connect(&m_lookupThread, SIGNAL(SendStatus(bool)), this, SLOT(lookedUp(bool)));
	m_lookupThread.start();
	//int id = QHostInfo::lookupHost(TEST_DOMAINNAME,this, SLOT(lookedUp(QHostInfo))); 

	m_DNS_loop->exec();
	if(!timer.isActive()/*!m_DNS_Result*/)
	{

		qDebug()<<"----lookUp timeout------";
		m_isDNS_OK = false;
		//QHostInfo::abortHostLookup(id);
	}

	 m_DNS_loop->deleteLater();
//    if(!isDnsOK)
//    {
//        GENIE2_LOG(QString("is_dnscfg_ok2 is false"));
//    }
    return m_isDNS_OK;
}
#endif

void QGenieConnectionStateDetecter::lookedUp(bool result)
{
	//m_isDNS_OK = (host.error() ==QHostInfo::NoError);
	m_isDNS_OK = result;
	qDebug()<<"--------Dns is :"<<m_isDNS_OK;
	if(m_DNS_loop)
	{
		m_DNS_loop->quit();
	}
	
	
}

bool QGenieConnectionStateDetecter::is_connection_ok()
{
#ifdef Q_OS_MACX
    return (is_wlan_ok() && is_dnscfg_ok());
#elif defined Q_OS_WIN
	int dnsCount = sizeof(dns_servers)/sizeof(dns_servers[0]);
	bool isOK = is_wlan_ok2();
	if((!isOK) && (m_server_index == dnsCount-1))
	{
		isOK = is_dnscfg_ok2();
	}
    return isOK;
#endif
}
