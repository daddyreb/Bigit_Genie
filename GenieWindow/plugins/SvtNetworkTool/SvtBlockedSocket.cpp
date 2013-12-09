//#include "topology.h"
#include <QtCore>



#include "SvtNetworkTool.h"
#include "openssl/ssl.h"
#include "openssl/rand.h"
//#include <winsock2.h>

#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif
#ifdef Q_OS_WIN32
#define  socklen_t int
#endif
//int SVT_SocketInterface::sCount=0;
#ifdef Q_OS_WIN32
class WSAHelper
{
    public:
    WSAHelper(){
        WSADATA wsaData;
        WORD wVersionRequested;
        wVersionRequested=MAKEWORD(2,2);
        Q_ASSERT(WSAStartup(wVersionRequested,&wsaData)==0);
    }
    ~WSAHelper()
    {
        WSACleanup();
    }
};
static WSAHelper s_wsahelper;
#endif
//static const SSL_METHOD *sMethod;//SSL协议
class SSLHelper
{
public:
    SSLHelper()
    {
        SSL_library_init();
        SSL_load_error_strings();
    }
    ~SSLHelper()
    {
    
    }
};
static SSLHelper s_sslhelper;



SVT_SocketInterface::SVT_SocketInterface()
    :mSock(INVALID_SOCKET)
{

}

SVT_SocketInterface::~SVT_SocketInterface()
{
    close();

}

bool SVT_SocketInterface::waitForReadReady(quint32 t)
{
    fd_set fd;
    FD_ZERO(&fd);
    FD_SET(mSock,&fd);
    struct timeval tv={t/1000,t%1000};
    int itemp=select(mSock+1,&fd,0,0,&tv);
    return (itemp>0 && FD_ISSET(mSock,&fd));
}

bool SVT_SocketInterface::waitForWriteReady(quint32 t)
{
    fd_set fd;
    FD_ZERO(&fd);
    FD_SET(mSock,&fd);
    struct timeval tv={t/1000,t%1000};
    int itemp=select(mSock+1,0,&fd,0,&tv);
    return (itemp>0 && FD_ISSET(mSock,&fd));
}

void SVT_SocketInterface::close()
{
#ifdef Q_OS_WIN32
	closesocket(mSock);
#else
    ::close(mSock);
#endif
    mSock=INVALID_SOCKET;
}

///////////////////////////////////////
SVT_TcpSocket::SVT_TcpSocket(bool *bexit)
{
    mbExit=bexit;
}

bool SVT_TcpSocket::create()
{
    if(mSock!=INVALID_SOCKET)
    {
        close();
    }
    mSock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    return (mSock != INVALID_SOCKET);
}

bool SVT_TcpSocket::connect(const SVT_IpAddress &ip, quint16 port)
{
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr =htonl(ip.asLong());
    addr.sin_port = htons (port);
    u_long noblock=1;
    bool ret=false;
    struct timeval tval;
#ifdef Q_OS_WIN32
    ioctlsocket(mSock,FIONBIO,&noblock);
#else
     ioctl(mSock, FIONBIO, &noblock); //设置为非阻塞模式
#endif

	::connect(mSock,(struct sockaddr *)&addr, sizeof (struct sockaddr_in));
        for(int i=0;i<10&&(!mbExit || !(*mbExit));i++)
        {
            fd_set rset, wset;
            FD_ZERO(&rset);
            FD_SET(mSock,&rset);
            wset=rset;
            tval.tv_sec = 2;
            tval.tv_usec = 0;
            if(select(mSock+1, &rset, &wset, NULL,&tval) > 0)
            {
                if(FD_ISSET(mSock, &rset) || FD_ISSET(mSock, &wset))
                {
                    int err;
                    socklen_t len=sizeof(err);
                    if(getsockopt(mSock, SOL_SOCKET, SO_ERROR, (char *)&err,&len) >=0)
                    {
                        ret=true;
                        break;
                    }
                }
            }
        }
    noblock=0;
#ifdef Q_OS_WIN32
    ioctlsocket(mSock,FIONBIO,&noblock);
#else
    ioctl(mSock, FIONBIO, &noblock); //设置为阻塞模式
#endif

    return ret;
    //
}
qint32 SVT_TcpSocket::sendData(const char *buf,quint32 siz)
{
//	Sleep(100);
    qint32 ret = send(mSock,buf,siz,0);
    return ret;
}

qint32 SVT_TcpSocket::recvData(char *buf,quint32 siz)
{
//	Sleep(100);
    qint32 ret=recv(mSock, buf,siz,0) ;
    return ret;
}

quint64 SVT_TcpSocket::testPort(const SVT_IpAddress *fromIp, int count, quint16 port, bool *bExit,int wait)
{
//    unsigned long startIp=fromIp.asLong();
    Q_ASSERT(count >0);
    Q_ASSERT(count <=64);
    quint64 ret=0;
    SOCKET *sockets=new SOCKET[count];

    sockaddr_in addr;
    addr.sin_family = AF_INET;
//    addr.sin_addr.s_addr =htonl(ip.asLong());
    addr.sin_port = htons (port);
    u_long noblock=1;
    fd_set rset, wset ,eset;
    FD_ZERO(&rset);
    int maxsock=0;
    for(int i=0;i<count;i++)
    {
        sockets[i]=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		addr.sin_addr.s_addr =htonl(fromIp[i].asLong());
#ifdef Q_OS_WIN32
        ioctlsocket(sockets[i],FIONBIO,&noblock);
#else
        ioctl(sockets[i], FIONBIO, &noblock); //设置为非阻塞模式
#endif
        ::connect(sockets[i],(struct sockaddr *)&addr, sizeof (struct sockaddr_in));
        FD_SET(sockets[i],&rset);
        maxsock=qMax((int)sockets[i],maxsock);
    }
    wset=rset;
    eset=rset;
    wait*=1000;
    const int verytime=500;
    for(int i=0;i<wait&&(!bExit ||!(*bExit));i+=verytime)
    {
#ifdef Q_OS_WIN32
        Sleep(verytime);
#else
        usleep(1000*verytime);
#endif
    }
    struct timeval tval;
    tval.tv_sec = 0;
    tval.tv_usec = 10;
    if(select(maxsock+1, &rset, &wset, &eset,&tval) > 0)
    {
        for(int i=0;i<count;i++)
        {
            if((FD_ISSET(sockets[i],&rset)||FD_ISSET(sockets[i],&wset))
                    && !FD_ISSET(sockets[i],&eset))
            {
                ret |= (((quint64)1)<<i);
            }
        }
    }

    for(int i=0;i<count;i++)
    {
#ifdef Q_OS_WIN32
        closesocket(sockets[i]);
#else
        ::close(sockets[i]);
#endif
    }
    delete [] sockets;
    return ret;

}

quint64 SVT_TcpSocket::testPort(const SVT_IpAddress &ip, quint16 fromPort, quint16 toPort,bool *bExit, int wait)
{
    //    unsigned long startIp=fromIp.asLong();
    int count=toPort-fromPort;
    Q_ASSERT(count >0);
    Q_ASSERT(count <=64);
    quint64 ret=0;
    SOCKET *sockets=new SOCKET[count];

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr =htonl(ip.asLong());
//    addr.sin_port = htons (port);
    u_long noblock=1;
    fd_set rset, wset ,eset;
    FD_ZERO(&rset);
    int maxsock=0;
    for(int i=0;i<count;i++)
    {
        sockets[i]=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        addr.sin_port = htons (fromPort+i);
#ifdef Q_OS_WIN32
        ioctlsocket(sockets[i],FIONBIO,&noblock);
#else
        ioctl(sockets[i], FIONBIO, &noblock); //设置为非阻塞模式
#endif
        ::connect(sockets[i],(struct sockaddr *)&addr, sizeof (struct sockaddr_in));
        FD_SET(sockets[i],&rset);
        maxsock=qMax((int)sockets[i],maxsock);
    }
    wset=rset;
    eset=rset;
    wait*=1000;
    const int verytime=500;
    for(int i=0;i<wait&&(!bExit ||!(*bExit));i+=verytime)
    {
#ifdef Q_OS_WIN32
        Sleep(verytime);
#else
        usleep(1000*verytime);
#endif
    }

    struct timeval tval;
    tval.tv_sec = 0;
    tval.tv_usec = 10;
    if(select(maxsock+1, &rset, &wset, &eset,&tval) > 0)
    {
        for(int i=0;i<count;i++)
        {
            if((FD_ISSET(sockets[i],&rset)||FD_ISSET(sockets[i],&wset))
                    && !FD_ISSET(sockets[i],&eset))
            {
                ret |= (((quint64)1)<<i);
            }
        }
    }

    for(int i=0;i<count;i++)
    {
#ifdef Q_OS_WIN32
        closesocket(sockets[i]);
#else
        ::close(sockets[i]);
#endif
    }
    delete [] sockets;
    return ret;
}

////////////////////////////////////////////////
SVT_SSLSocket::SVT_SSLSocket(bool *bexit)
    :SVT_TcpSocket(bexit)
    ,mCtx(NULL)
    ,mSsl(NULL)
{

}

SVT_SSLSocket::~SVT_SSLSocket()
{
    close();
}

bool SVT_SSLSocket::create()
{
    return SVT_TcpSocket::create();

}

bool SVT_SSLSocket::connect(const SVT_IpAddress &ip, quint16 port)
{
    if(!SVT_TcpSocket::connect(ip,port))
    {
        return false;
    }
//    SSL_library_init();
//    SSL_load_error_strings();

    mCtx = SSL_CTX_new(SSLv23_client_method());
    if(!mCtx)
    {
        close();
        return false;
    }

    mSsl=SSL_new(mCtx);
    if(!mSsl)
    {
        close();
        return false;
    }

    if(0==SSL_set_fd(mSsl,mSock))
    {
        close();
        return false;
    }

//    RAND_poll();
//    while (RAND_status() == 0) {
//        unsigned short rand_ret = rand() % 65536;
//        RAND_seed(&rand_ret, sizeof(rand_ret));
//    }

    if(1 !=SSL_connect(mSsl))
    {
        close();
        return false;
    }

//    qDebug()<<"SSL connection using "<<SSL_get_cipher(mSsl);

//    X509 *server_cert=SSL_get_peer_certificate(mSsl);
//    if(!server_cert)
//    {
//        close();
//        return false;
//    }
//    qDebug()<<"server Certificate: ";

//    char *str=X509_NAME_oneline(X509_get_subject_name(server_cert),0,0);
//    if(!str)
//    {
//        close();
//        return false;
//    }
//    qDebug()<<"\t subject; "<< str;
//    OPENSSL_free(str);

//    str=X509_NAME_oneline(X509_get_issuer_name(server_cert),0,0);

//    if(!str)
//    {
//        close();
//        return false;
//    }

//    qDebug()<<"\t issuer; "<< str;
//    OPENSSL_free(str);
//    X509_free(server_cert);

    return true;
}

qint32 SVT_SSLSocket::sendData(const char *buf,quint32 siz)
{
    return SSL_write(mSsl,buf,siz);
}

qint32 SVT_SSLSocket::recvData(char *buf,quint32 siz)
{
    return SSL_read(mSsl, buf, siz);
}

void SVT_SSLSocket::close()
{
    qDebug()<<"SVT_SSLSocket::close()";
    SVT_SocketInterface::close();
    if(mSsl)
    {
        SSL_shutdown(mSsl); //发送SSL关闭消息
    }


    if(mSsl)
    {
        SSL_free(mSsl);
    }
    mSsl=NULL;
    if(mCtx)
    {
        SSL_CTX_free(mCtx);
        mCtx=NULL;
    }

}

/////////////////////////////////////////////

SVT_UdpSocket::SVT_UdpSocket()
{

}

bool SVT_UdpSocket::create()
{
    mSock=socket(AF_INET, SOCK_DGRAM, 0);
    return (mSock != INVALID_SOCKET);
}

qint32 SVT_UdpSocket::sendTo(const SVT_IpAddress &ip, quint16 port, const char *buf, quint32 siz)
{
    sockaddr_in dest;
    dest.sin_family=AF_INET;
    dest.sin_port=htons(port); ///监听端口
    dest.sin_addr.s_addr=htonl(ip.asLong()); ///本机

    //test=connect(sock,(sockaddr *)&dest,sizeof(dest));
    //printf("connect result %d\n",test);
    qint32 ret=sendto(mSock,buf,siz,0,(sockaddr *)&dest,sizeof(dest));
    return ret;
}

qint32 SVT_UdpSocket::recvFrom(char *buf, quint32 siz, SVT_IpAddress *recv_ip, quint16 *recv_port)
{
    sockaddr_in recv_add;
    socklen_t recv_add_len=sizeof(recv_add);

    qint32 ret =::recvfrom(mSock,buf,siz,0,(sockaddr *)&recv_add,&recv_add_len);
    if(recv_ip)
    {
        recv_ip->Set(ntohl(recv_add.sin_addr.s_addr));
    }
    if(recv_port)
    {
        *recv_port=ntohs(recv_add.sin_port);
    }
    return ret;


}
