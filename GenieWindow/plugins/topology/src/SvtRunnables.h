#ifndef SVTRUNNABLES_H
#define SVTRUNNABLES_H
class SVT_TopoMethod;
class SVT_SeedFactoryInterface;
class SVT_UpnpHelper;
class SVT_NetgearSpecificHelper;
class SVT_NodePool;
class SVT_NodeFilterForFactory;
#include <QtCore>

class SVT_RunnableInterface:public QRunnable
{
public:
    SVT_RunnableInterface(SVT_TopoMethod*);
    SVT_NodePool *getPool();
    SVT_TopoMethod *mTopoMethod;
};

class SVT_SeedRunnable:public SVT_RunnableInterface
{
public:
    SVT_SeedRunnable(SVT_TopoMethod*,SVT_SeedFactoryInterface *,int looptimes=-1);
    void run();
    virtual void processSeed(quint32 ip)=0;
    SVT_SeedFactoryInterface *mFactory;
    QVector<quint32> mIpList;
    int mLoopTimes;
};


class SVT_NetScanRunnable:public SVT_SeedRunnable
{
public:
    SVT_NetScanRunnable(SVT_TopoMethod*, SVT_SeedFactoryInterface *, bool bUseSubscribedSeed, int looptimes=-1);
        ~SVT_NetScanRunnable();
    void whenDataGetted(const SVT_MacAddress &mac,const SVT_IpAddress &ip);
    void whenDataNotGetted(const SVT_IpAddress &ip);
    virtual void processSeed(quint32 ip);
    bool mUseSubscribedSeed;
};

class SVT_NetbiosRunnable:public SVT_SeedRunnable
{
public:
    SVT_NetbiosRunnable(SVT_TopoMethod*,SVT_SeedFactoryInterface *);
        ~SVT_NetbiosRunnable();
    virtual void processSeed(quint32 ip);
};



class SVT_PortTestRunnable:public SVT_RunnableInterface
{
public :
    SVT_PortTestRunnable(SVT_TopoMethod *,quint16 port
                         ,SVT_SeedFactoryInterface *facroty1,SVT_SeedFactoryInterface *factory2=NULL);
    ~SVT_PortTestRunnable();
    virtual void run();
    virtual void whenPortOk(const SVT_IpAddress &ip)=0;
    SVT_SeedFactoryInterface *mFactory1;
    SVT_SeedFactoryInterface *mFactory2;
    quint16 mPort;

};

#ifdef USE_PUTTY
class SVT_PuttyRunnable:public SVT_PortTestRunnable
{
public:
   SVT_PuttyRunnable (SVT_TopoMethod *,SVT_SeedFactoryInterface *factory1,SVT_SeedFactoryInterface *factory2=NULL);
    virtual void whenPortOk(const SVT_IpAddress &ip);
};

typedef SVT_Reference<SVT_PuttyRunnable> SVT_PuttyRunnableRef;
#endif
#ifdef USE_RDP
class SVT_RdpRunnable:public SVT_PortTestRunnable
{
public:
    SVT_RdpRunnable(SVT_TopoMethod*method,SVT_SeedFactoryInterface *factory1,SVT_SeedFactoryInterface *factory2);
    virtual void whenPortOk(const SVT_IpAddress &ip);
};
typedef SVT_Reference<SVT_RdpRunnable> SVT_RdpRunnableRef;

#endif

class SVT_AfpRunnable:public SVT_SeedRunnable
{
public:
    SVT_AfpRunnable(SVT_TopoMethod *,SVT_SeedFactoryInterface *);
    ~SVT_AfpRunnable();
    virtual void processSeed(quint32 ip);
};

class SVT_UpnpRunnable:public SVT_RunnableInterface
{
public:
    SVT_UpnpRunnable(SVT_TopoMethod *method);
    ~SVT_UpnpRunnable();
    void run();
};

class SVT_NetgearSpecificRunnable:public SVT_RunnableInterface
{
public:
    SVT_NetgearSpecificRunnable(SVT_TopoMethod *method,SVT_SeedFactoryInterface *);
    ~SVT_NetgearSpecificRunnable();
    void run();
    SVT_SeedFactoryInterface *mFactory;
};

#ifdef Q_OS_MACX
class SVT_NetScanRunnable_Unix:public SVT_RunnableInterface
{
public:
    SVT_NetScanRunnable_Unix(SVT_TopoMethod *method,SVT_SeedFactoryInterface *,SVT_SeedFactoryInterface *);
    ~SVT_NetScanRunnable_Unix();
    void run();
    SVT_SeedFactoryInterface *mFactory_Subscribed;
    SVT_SeedFactoryInterface *mFactory_AllNet;
};
#endif

typedef SVT_Reference<SVT_NetScanRunnable> SVT_NetScanRunnableRef;
typedef SVT_Reference<SVT_NetbiosRunnable> SVT_NetbiosRunnableRef;

typedef SVT_Reference<SVT_AfpRunnable> SVT_AfpRunnableRef;
typedef SVT_Reference<SVT_UpnpRunnable> SVT_UpnpRunnableRef;
typedef SVT_Reference<SVT_NetgearSpecificRunnable> SVT_NetgearSpecificRunnableRef;
#ifdef Q_OS_MACX
typedef SVT_Reference<SVT_NetScanRunnable_Unix> SVT_NetScanRunnable_UnixRef;
#endif
#endif // SVTRUNNABLES_H
