#ifndef __SVTRDP_H
#define __SVTRDP_H
#ifdef USE_RDP
class SVT_RdpMethod:public SVT_TopoMethod
{
public:
    SVT_RdpMethod(SVT_NodePool *pool);
    void start();
    void stop();
protected:
//    QThreadPool &mCoreThreadPool;
    SVT_RdpRunnableRef mRdtRunnable;
//    SVT_RdpRunnableRef mRdtRunnable_Subscribed;


};
#endif

#endif // SVTNETBIOSMETHOD_H
