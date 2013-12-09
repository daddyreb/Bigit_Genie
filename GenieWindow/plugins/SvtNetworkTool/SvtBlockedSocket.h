#ifndef SVTSSOCKET_H
#define SVTSSOCKET_H
class SVT_IpAddress;
#ifdef Q_OS_WIN32
#include <stdio.h>
#include <windows.h>
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h> /* inet_aton, sockaddr_in, etc */
#include <sys/ioctl.h>
#include <sys/sysctl.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_types.h>
#include <net/route.h>
    #ifndef SOCKET
    #define SOCKET int
    #endif
#endif
#include "SvtNetwork.h"
#include "openssl/ssl.h"
//struct     SSL_CTX;
//struct      SSL;

class SVT_NETWORKTOOL_EXPORT SVT_SocketInterface
{
public:
    SVT_SocketInterface();
    virtual ~SVT_SocketInterface();
    virtual bool create()=0;
    virtual bool waitForReadReady(quint32 t);
    virtual bool waitForWriteReady(quint32 t);
    virtual void close();
protected:
    SOCKET mSock;
//    static int sCount;
};

class SVT_NETWORKTOOL_EXPORT SVT_TcpSocket:public SVT_SocketInterface
{
public:
    SVT_TcpSocket(bool *bexit=NULL);
    virtual bool create();
    virtual bool connect(const SVT_IpAddress &ip,quint16 port);
    virtual qint32 sendData(const char *buf,quint32 siz);
    virtual qint32 recvData(char *buf,quint32 siz);
    static quint64 testPort(const SVT_IpAddress *fromIp,int count,quint16 port,bool *bExit=NULL,int wait=3);
    static quint64 testPort(const SVT_IpAddress &ip,quint16 fromPort,quint16 toPort,bool *bExit=NULL,int wait=3);
    bool *mbExit;

};

class SVT_NETWORKTOOL_EXPORT SVT_SSLSocket:public SVT_TcpSocket
{
public:
    SVT_SSLSocket(bool *bexit=NULL);
    ~SVT_SSLSocket();
    virtual bool create();
    virtual bool connect(const SVT_IpAddress &ip,quint16 port);
    virtual qint32 sendData(const char *buf,quint32 siz);
    virtual qint32 recvData(char *buf,quint32 siz);
    virtual void close();
protected:
    SSL_CTX *mCtx;
    SSL *mSsl;

};

class SVT_NETWORKTOOL_EXPORT SVT_UdpSocket:public SVT_SocketInterface
{
public:
    SVT_UdpSocket();
    virtual bool create();
    qint32 sendTo(const SVT_IpAddress &ip,quint16 port,const char *buf,quint32 siz);
    qint32 recvFrom(char *buf,quint32 size,SVT_IpAddress *recv_ip=0,quint16 *recv_port=0);
};

#endif // SVTSSOCKET_H
