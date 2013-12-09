#include "topology.h"



//////////////////////////////////////////////////
SVT_UpnpMethod::SVT_UpnpMethod(SVT_NodePool *pool)
    :SVT_TopoMethod(TPT_UPNP,pool)
//    ,mCoreThreadPool(pool->mCoreThreadPool)
{
}

void SVT_UpnpMethod::start()
{
    //smart pointer, no need delete
    mUpnpRunableRef=new SVT_UpnpRunnable(this);
    CORE_STARTRUNNABLE(mUpnpRunableRef.asPointer());
}

void SVT_UpnpMethod::stop()
{
//    if(!mUpnpRunableRef.isNull() && mUpnpRunableRef->mHelper)
//    {
//        mUpnpRunableRef->mHelper->stop();
//    }
}
