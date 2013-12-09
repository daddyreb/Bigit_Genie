
#include "SvtSoap.h"
#include <QDebug>
#include "SvtNetworkTool.h"


#define CAN_EXIT (mExitFlag && *mExitFlag)
#define DO_AND_CHECKEXIT(x) ((x) && (!CAN_EXIT) )

int getHttpHeaderBound(const QByteArray &array)
{
    int itemp1=array.indexOf("\r\n\r\n");
    int itemp2=array.indexOf("\n\n");
	if (itemp1 <0 && itemp2 <0)
	{
		return -1;
	}
	else if(itemp1 >=0 && itemp2 >=0)
	{
		return (itemp1<itemp2)?(itemp1+4):(itemp2+2);
	}
	else
	{
		return (itemp1 <0)?(itemp2+2):(itemp1+4);
	}
	Q_ASSERT(0);
	return -1;
                                
}

SVT_IpAddress SVT_NETWORKTOOL_EXPORT getDNS(const QString &addr)
{
    QHostInfo hinfo = QHostInfo::fromName(addr);
    quint32 ip=0;
    if ( hinfo.error() == QHostInfo::NoError && !hinfo.addresses().isEmpty())
    {
        for(int i = 0; i < hinfo.addresses().length(); i++)
		{
			ip=hinfo.addresses()[i].toIPv4Address();
			if(ip != 0)
				break;
		}
    }
    SVT_IpAddress ret=SVT_IpAddress(ip);
    qDebug()<<ret.toString();
    return ret;
}

SVT_HttpInterface::SVT_HttpInterface(bool *exitflag)
    :mExitFlag(exitflag)
    ,mTcpSock(NULL)
    ,mPreferedPort(80)
{
}

SVT_HttpInterface::~SVT_HttpInterface()
{
    if(mTcpSock)
    {
        mTcpSock->close();
        delete mTcpSock;
        mTcpSock=NULL;
    }
}
QByteArray &SVT_HttpInterface::getHttpResult(const QString &address)
{
    QUrl url(address);
    return getHttpResult(url.host(),url.path(),url.port(mPreferedPort));
}

QByteArray &SVT_HttpInterface::getHttpResult(const QString &host,const QString &path,quint16 port)
{
    if(port==0)
    {
        port=mPreferedPort;
    }
    mArrayBuffer.clear();
	SVT_IpAddress ip;
	if(0 == host.compare("routerlogin.net",Qt::CaseInsensitive)||host.contains(".com",Qt::CaseInsensitive))
	{
		ip = getDNS(host);
	}else
	{
		ip.Set(host);
		ip = SVT_IpAddress(ip.asLong());
	}
   
//     mTcpSock;
    char revc_buff[1024];
    QString request= "GET %1 HTTP/1.1\r\nConnection: Keep-Alive\r\nHost: %2\r\n\r\n";
    request=request.arg(path,host);
    const QByteArray reqarray=request.toLatin1();
    int readedtemp,itemp;

    if(!ip.isValid())
    {
        goto get_http_result_end;
    }




    if(
            DO_AND_CHECKEXIT(mTcpSock->create())
            &&DO_AND_CHECKEXIT(mTcpSock->connect(ip,port))
            &&(DO_AND_CHECKEXIT(mTcpSock->waitForWriteReady(1000))
               ||DO_AND_CHECKEXIT(mTcpSock->waitForWriteReady(1000))
               )
            &&DO_AND_CHECKEXIT(reqarray.size()==mTcpSock->sendData(reqarray.data(),reqarray.size()) )
            )
    {
        for(int i=0;i<100 &&!CAN_EXIT;i++)
        {
            if(mTcpSock->waitForReadReady(1000))
            {
                readedtemp=mTcpSock->recvData(revc_buff,sizeof(revc_buff));
//                qDebug()<<"readedtemp"<<readedtemp;
                if(readedtemp <=0)
                {
                    break;
                }
                mArrayBuffer.append(revc_buff,readedtemp);
            }
        }
    }

    mTcpSock->close();


    itemp=getHttpHeaderBound(mArrayBuffer);
    if(itemp<0 )
    {
        mArrayBuffer.clear();
        goto get_http_result_end;
    }
    mArrayBuffer.remove(0,itemp);


get_http_result_end:
    return mArrayBuffer;
}

QByteArray& SVT_HttpInterface::rawRequest(const QString &address, const QString &request,quint16 port)
{
    if(port==0)
    {
        port=mPreferedPort;
    }
    mArrayBuffer.clear();
//    QUrl url(address);
//    qDebug()<<url.host();
	SVT_IpAddress ip;
	if(0 == address.compare("routerlogin.net",Qt::CaseInsensitive)||address.contains(".com"))
	{
	   ip=SVT_IpAddress::fromDNS(address);
	}else
	{
		ip.Set(address);
		ip = SVT_IpAddress(ip.asLong());
	}
    
//     mTcpSock;
    QByteArray requestarray=request.toLatin1();

    char revc_buff[1024];
    int readedtemp;
    if(!ip.isValid())
    {
        qDebug()<<"ip not valid"<<__FILE__<<__LINE__;
        goto raw_quest_ret;
    }




    if(
            DO_AND_CHECKEXIT(mTcpSock->create())
            &&DO_AND_CHECKEXIT(mTcpSock->connect(ip,port))
           // &&DO_AND_CHECKEXIT(mTcpSock->waitForWriteReady(1000))
            &&DO_AND_CHECKEXIT(requestarray.size()==mTcpSock->sendData(requestarray.data(),requestarray.size()))
            )
    {
        for(int i=0;i<100 &&!CAN_EXIT;i++)
        {
            if(mTcpSock->waitForReadReady(1000))
            {
                readedtemp=mTcpSock->recvData(revc_buff,sizeof(revc_buff));
//                qDebug()<<"readedtemp"<<readedtemp;
                if(readedtemp <=0)
                {
                    break;
                }
                mArrayBuffer.append(revc_buff,readedtemp);
            }
        }
    }
    mTcpSock->close();
raw_quest_ret:
    return mArrayBuffer;

}

QByteArray SVT_HttpInterface::getBody(const QByteArray &buffer)
{
    int itemp=getHttpHeaderBound(buffer);
    if(itemp <0 )
    {
        return QByteArray();
    }
    return buffer.right(buffer.length()-itemp);
}

QByteArray SVT_HttpInterface::getHeader(const QByteArray &buffer)
{
    int itemp=getHttpHeaderBound(buffer);
    if(itemp <0 )
    {
        return QByteArray();
    }
    return buffer.left(itemp);

}

//////////////////////////////////////////////////////
SVT_Http::SVT_Http(bool *exitflag)
    :SVT_HttpInterface(exitflag)
{
    mTcpSock=new SVT_TcpSocket(exitflag);
	mPreferedPort=80;
}

SVT_Https::SVT_Https(bool *exitflag)
    :SVT_HttpInterface(exitflag)
{
    mTcpSock=new SVT_SSLSocket(exitflag);
		mPreferedPort=443;
}
