#ifndef SVTNETSCANMETHOD_H
#define SVTNETSCANMETHOD_H




class SVT_AllNetFactory;
class SVT_NetscanMethod;
class SVT_Core;
class SVT_NodePool;
class SVT_SeedFactoryInterface;
class SVT_TopoMethod;



class SVT_NetscanMethod:public SVT_TopoMethod
{
public:
    SVT_NetscanMethod(SVT_NodePool *pool);
    ~SVT_NetscanMethod();
    void start();
    void stop();

//    QThreadPool &mCoreThreadPool;
#ifdef Q_OS_WIN32
     SVT_NetScanRunnableRef mNetScanRunnables[NETSCAN_BASIC_RUNNABLE_NUM];
    SVT_NetScanRunnableRef mNetScanSubscribedRunnable[NETSCAN_SUBSCRIB_RUNNABLE_NUM];
#else
    SVT_NetScanRunnable_UnixRef mNetScanRunnable_UnixRef;
#endif

};

#endif // SVTNETSCANMETHOD_H
