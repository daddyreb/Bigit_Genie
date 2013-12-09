#include "topology.h"
#include "SvtRdpMethod.h"
#ifdef USE_RDP
SVT_RdpMethod::SVT_RdpMethod(SVT_NodePool *pool)
    :SVT_TopoMethod(TPT_RDP,pool)
//    ,mCoreThreadPool(pool->mCoreThreadPool)
{

}

void SVT_RdpMethod::start()
{
    mRdtRunnable=new SVT_RdpRunnable(this,mNodePool->mAllResultsFactory_Rdp
                                     ,mNodePool->mSubscribedFactory_Rdp);
//    mRdtRunnable_Subscribed=new SVT_RdpRunnable(this,mNodePool->mSubscribedFactory_Rdp);
    CORE_STARTRUNNABLE(mRdtRunnable.asPointer());
//    CORE_STARTRUNNABLE(mRdtRunnable_Subscribed.asPointer());

}

void SVT_RdpMethod::stop()
{

}
#endif
