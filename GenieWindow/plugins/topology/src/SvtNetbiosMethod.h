#ifndef SVTNETBIOSMETHOD_H
#define SVTNETBIOSMETHOD_H

class SVT_NetbiosMethod:public SVT_TopoMethod
{
public:
    SVT_NetbiosMethod(SVT_NodePool *pool);
    void start();
    void stop();
protected:

//    QThreadPool &mCoreThreadPool;
    SVT_NetbiosRunnableRef mNetbiosRunnables[NETBIOS_BASIC_RUNNABLE_NUM];
    SVT_NetbiosRunnableRef mNetbiosSubscribedRunnable[NETBIOS_SUBSCRIB_RUNNABLE_NUM];


};

#endif // SVTNETBIOSMETHOD_H
