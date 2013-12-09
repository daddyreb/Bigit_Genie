#include "topology.h"
#include "SvtPuttyMethod.h"
#ifdef USE_PUTTY
SVT_PuttyMethod::SVT_PuttyMethod(SVT_NodePool *pool)
    :SVT_TopoMethod(TPT_PUTTY,pool)
//    ,mCoreThreadPool(pool->mCoreThreadPool)
{

}

void SVT_PuttyMethod::start()
{
//    mPuttyRunnable_AllResults=new SVT_PuttyRunnable(this,mNodePool->mAllResultsFactory_Putty);
    mPuttyRunnable=new SVT_PuttyRunnable(this,mNodePool->mSubscribedFactory_Putty
                                                    ,mNodePool->mAllResultsFactory_Putty);
//    CORE_STARTRUNNABLE(mPuttyRunnable_AllResults.asPointer());
    CORE_STARTRUNNABLE(mPuttyRunnable.asPointer());

}

void SVT_PuttyMethod::stop()
{

}
#endif
