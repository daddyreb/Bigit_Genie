#include "SvtHttpDownload.h"
#include "SvtNetworkTool.h"
#include <QtCore>
#include <QThread>
#ifdef Q_OS_MAC
#include <time.h>
#include <sys/timeb.h>
#endif

#define CAN_EXIT (mbExitFlag)
#define DO_AND_CHECKEXIT(x) ((x) && (!CAN_EXIT))
extern int getHttpHeaderBound(const QByteArray &array);
extern SVT_IpAddress getDNS(const QString &addr);
quint64 getRecvFileSize(const QByteArray &data)
{
    int nPos = 0;
    QByteArray temp;
    const char *conL="Content-Length:";

    nPos = data.indexOf(conL);
    if(nPos != -1)
    {
        temp = data.mid(nPos + strlen(conL));
        nPos = temp.indexOf("\n");

        if(nPos != -1)
        {
            temp = temp.left(nPos);
            temp=temp.trimmed();

            return (temp.toULongLong());
        }
    }

    return -1;
}

#ifdef Q_OS_MAC
static qint64 getTimeStamp()
{
    struct timeb tp;
    ftime(&tp);
    return 1000*((int)tp.time)+tp.millitm;
}
#endif


class _SpeedCalculater
{
public:
    _SpeedCalculater(int buffersize){
        mDtArray.resize(buffersize);
        mSizeArray.resize(buffersize);
        init();
    };
    ~_SpeedCalculater(){};
    void init()
    {
        Q_ASSERT(mDtArray.size()==mSizeArray.size());
        int c=mDtArray.size();
#ifdef Q_OS_MAC
        qint64 cur=getTimeStamp();
#else
        QDateTime cur=QDateTime::currentDateTime();
#endif
        for(int i=0;i<c;i++)
        {
            mDtArray[i]=cur;
            mSizeArray[i]=0;
        }

        mCurIdx=0;
        mCachedSpeed=0;
        mNodeCount=0;
    }

    void addData(quint64 siz)
    {
        Q_ASSERT(mDtArray.size()==mSizeArray.size());
        int c=mDtArray.size();
        mCurIdx=(mCurIdx+1)%c;
#ifdef Q_OS_MAC
        mDtArray[mCurIdx]=getTimeStamp();
#else
        mDtArray[mCurIdx]=QDateTime::currentDateTime();
#endif
        mSizeArray[mCurIdx]=siz;
        mNodeCount=qMin(mNodeCount+1,c);


    }

    quint32 calculateSpeed()
    {
        if(mNodeCount<2)
        {
            return 0;
        }
        int c=mDtArray.size();
        int last_idx=(mCurIdx+c-(mNodeCount-1))%c;
#ifdef Q_OS_MAC
        int t=mDtArray[mCurIdx]-mDtArray[last_idx];
#else
        int t=mDtArray[last_idx].msecsTo(mDtArray[mCurIdx]);
#endif

        quint32 real_speed=(mSizeArray[mCurIdx]-mSizeArray[last_idx])*1000/((t==0)?1:t);
//        quint32 ret=(mCachedSpeed==0)?real_speed
//            :((real_speed*3+mCachedSpeed*1)/4);
//        mCachedSpeed=ret;
        qDebug()<<"speed"<<real_speed<<last_idx<<mCurIdx;
        return real_speed ;
    }

protected:
#ifdef Q_OS_MAC
    QVector<qint64 > mDtArray;
#else
    QVector<QDateTime> mDtArray;
#endif
    QVector<quint64> mSizeArray;
    int mCurIdx;
    quint32 mCachedSpeed;
    int mNodeCount;

};



SVT_HttpDownload::SVT_HttpDownload()
    :mbExitFlag(false)
    ,mPool(NULL)
{
}

SVT_HttpDownload::~SVT_HttpDownload()
{
    disconnect(this,SIGNAL(progress(quint64,quint64,quint32)),NULL,NULL);
    disconnect(this,SIGNAL(done(bool,QString)),NULL,NULL);
    mbExitFlag=true;
    mPool->mSet.remove(this);
}

void SVT_HttpDownload::startGetFile(const QString &url, quint16 port,
                                    const QString &localfile,
                                    quint64 startPos)
{
    Q_ASSERT(qApp->thread()==QThread::currentThread());
    mLocalFile=localfile;
    mUrl=url;
    mPort=port;
    mStartPos=startPos;
    mErrorStr.clear();
    Q_ASSERT(mPool);
    mPool->start(this);
}


bool SVT_HttpDownload::getHttpFileProcess()
{
    QFile f(mLocalFile);
    QIODevice::OpenMode mode=(mStartPos>0)?(QIODevice::WriteOnly|QIODevice::Append):QIODevice::WriteOnly;
    if(!f.open(mode))
    {
        mErrorStr="Can Not Open Local File";
        return false;
    }
    QByteArray arraybuffer;
    QUrl url(mUrl);
    SVT_IpAddress ip = getDNS(url.host());
    if(!ip.isValid())
    {
        mErrorStr="DNS Error";
        return false;
    }

    SVT_TcpSocket tcp_sock;
    char recv_buff[1024] = {0};
    QString request = "GET %1 HTTP/1.1\r\nConnection: Keep-Alive\r\nHost: %2\r\nRange: bytes=%3-\r\n\r\n";

    request = request.arg(url.path(), url.host()).arg(mStartPos);
    const QByteArray reqarray = request.toLatin1();
    _SpeedCalculater speedCalc(200);
    bool headOk=false;
    quint64 total=0;
    quint64 processed=0;

    if(DO_AND_CHECKEXIT(tcp_sock.create())
       &&DO_AND_CHECKEXIT(tcp_sock.connect(ip,url.port(80)))
       &&(DO_AND_CHECKEXIT(tcp_sock.waitForWriteReady(1000))
          ||DO_AND_CHECKEXIT(tcp_sock.waitForWriteReady(1000))
          ||DO_AND_CHECKEXIT(tcp_sock.waitForWriteReady(1000))
          ||DO_AND_CHECKEXIT(tcp_sock.waitForWriteReady(1000))

          )
       &&DO_AND_CHECKEXIT(reqarray.size()==tcp_sock.sendData(reqarray.data(),reqarray.size()) )
       )
    {
        QDateTime ti_last_signal=QDateTime::currentDateTime().addYears(-1);
        while(!CAN_EXIT)
        {
            int readedtemp ;
            if( (readedtemp= tcp_sock.recvData(recv_buff, sizeof(recv_buff)) ) <=0)
            {
                break;
            }
            if(!headOk)
            {
                arraybuffer.append(recv_buff,readedtemp);
                int bound=getHttpHeaderBound(arraybuffer);
                if(bound >=0)
                {
                    total=getRecvFileSize(arraybuffer);
                    arraybuffer.remove(0,bound);
                    f.write(arraybuffer);
                    processed=arraybuffer.count();
//                    if(total <= 0)
//                    {
//                        mErrorStr="Wrong Total Size";
//                        return false;
//                    }
                    speedCalc.init();
                    headOk=true;
                }
            }
            else
            {
                processed+=readedtemp;
                f.write(recv_buff,readedtemp);

                if(ti_last_signal.addMSecs(200) < QDateTime::currentDateTime())
                {
                    speedCalc.addData(processed);
                    ti_last_signal=QDateTime::currentDateTime();
                    emit progress(mStartPos+processed, total, speedCalc.calculateSpeed());
                }
            }
        }
    }
    f.close();
    return (!total ||total==processed);
}

void SVT_HttpDownload::run()
{
    qDebug()<<"SVT_HttpDownload::run"<<QThread::currentThreadId();
    bool b=getHttpFileProcess();
    emit done(!b,mErrorStr);
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

SVT_HttpDownloadPool::SVT_HttpDownloadPool()
{

}

SVT_HttpDownloadPool::~SVT_HttpDownloadPool()
{
    foreach(SVT_HttpDownload *http,mSet)
    {
        http->setExitFlag();
    }
    waitForDone();
}

SVT_HttpDownload * SVT_HttpDownloadPool::newHttp()
{
    SVT_HttpDownload *http=new SVT_HttpDownload;
//    connect(http,SIGNAL(destroyed(QObject*)),this,SLOT(slot_HttpDestroyed(QObject*)));
    mSet.insert(http);
    Q_ASSERT(mSet.count()<20);//防止调用时候，只创建而不启动
    http->mPool=this;
	return http;
}

void SVT_HttpDownloadPool::setExitFlag(SVT_HttpDownload *http)
{
    if(mSet.contains(http))
    {
        disconnect(http,SIGNAL(progress(quint64,quint64,quint32)),NULL,NULL);
        disconnect(http,SIGNAL(done(bool,QString)),NULL,NULL);
        http->setExitFlag();
    }
}

//void SVT_HttpDownloadPool::slot_HttpDestroyed(QObject *obj)
//{
//    SVT_HttpDownload *http=static_cast<SVT_HttpDownload *>(obj);
//    Q_ASSERT(http);
//    mSet.remove(http);
//}
