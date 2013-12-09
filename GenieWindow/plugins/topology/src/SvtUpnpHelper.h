#ifndef SVTUPNPHELPER_H
#define SVTUPNPHELPER_H

#include <QtCore>
#include <QtNetwork>
#include "GenieHttp.h"
class SVT_IpAddress;
class SVT_TopoMethod;

class SVT_UpnpHelper:public QObject
{
    Q_OBJECT
public:
    SVT_UpnpHelper(SVT_TopoMethod *method);
    ~SVT_UpnpHelper();
protected slots:
    void slot_UdpDataRet();
    void slot_HttpReqFinished( int id, bool err );
    void slot_HttpDone(bool err);
    void slot_Timeout();
public:
    void start();
//    void stop();
protected:
    void processTheDatagram(QByteArray &datagram,const quint32 &ip);
    void parseHttpData(QByteArray &data, QMap<int, QString> &retmap);
    QUdpSocket mUdpSock;
//    QHttp mHttp;
//    QSet<QHttp *> mHttpPool;
    QTimer mTimer;
    QEventLoop mEventLoop;
    QMap<GenieHttp *,quint32> mHttpMapIp;
    QMap<GenieHttp *,int> mHttpMapReq;
    QMap<GenieHttp *,QString> mHttpMapLocation;
    QMap<quint32,QMap<int,QString> > mIpMapInfo;
    bool mergeUpnpInfo(GenieHttp *http,const QMap<int,QString> &map);
    SVT_TopoMethod *mTopoMethod;
//    QMutex mMutex;
//    bool mStarted;
    friend class SVT_UpnpMethod;
    friend class SVT_UpnpRunnable;
};


#endif // SVTUPNPHELPER_H
