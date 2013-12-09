#ifndef SVTHTTP_H
#define SVTHTTP_H

#include <QtCore>
#include "SvtNetworkTool.h"
#include "SvtBlockedSocket.h"
class SVT_NETWORKTOOL_EXPORT SVT_HttpInterface
{
public:
    explicit SVT_HttpInterface(bool *exitflag=NULL);
    virtual ~SVT_HttpInterface();
    QByteArray &getHttpResult(const QString &address);
    QByteArray &getHttpResult(const QString &host,const QString &path,quint16 port=0);
    QByteArray &rawRequest(const QString &address, const QString &request,quint16 port=0);
    static QByteArray getHeader(const QByteArray &buffer);
    static QByteArray getBody(const QByteArray &buffer);
protected:
    bool *mExitFlag;
    QByteArray mArrayBuffer;
    SVT_TcpSocket *mTcpSock;
    quint16 mPreferedPort;


};

class SVT_NETWORKTOOL_EXPORT SVT_Http:public SVT_HttpInterface
{
public:
    SVT_Http(bool *exitflag);
};

class SVT_NETWORKTOOL_EXPORT SVT_Https:public SVT_HttpInterface
{
public:
    SVT_Https(bool *exitflag);
};
#endif // SVTHTTP_H
