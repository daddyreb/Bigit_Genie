#include "topology.h"
#include "SvtNetgearSpecificHelper.h"



//////////////////////////////////////////////////
SVT_NetgearSpecificMethod::SVT_NetgearSpecificMethod(SVT_NodePool *pool)
    :SVT_TopoMethod(TPT_NETGEARSPECIFIC,pool)
//    ,mCoreThreadPool(pool->mCoreThreadPool)
{
}

void SVT_NetgearSpecificMethod::start()
{
    //smart pointer, no need delete
    mNetgearRunableRef_AllResult=new SVT_NetgearSpecificRunnable(this,mNodePool->mAllResultsFactory_NetgearSpecific);
    CORE_STARTRUNNABLE(mNetgearRunableRef_AllResult.asPointer());

    //mNetgearRunableRef_Subscribed=new SVT_NetgearSpecificRunnable(this,mNodePool->mSubscribedFactory_NetgearSpecific);
   // CORE_STARTRUNNABLE(mNetgearRunableRef_Subscribed.asPointer());
}

void SVT_NetgearSpecificMethod::stop()
{
//    if(!mUpnpRunableRef.isNull() && mUpnpRunableRef->mHelper)
//    {
//        mUpnpRunableRef->mHelper->stop();
//    }
}
