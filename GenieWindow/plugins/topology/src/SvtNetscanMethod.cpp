#include "topology.h"



SVT_NetscanMethod::SVT_NetscanMethod(SVT_NodePool *pool)
    :SVT_TopoMethod(-1,pool)
//    ,mCoreThreadPool(pool->mCoreThreadPool)
{


}

SVT_NetscanMethod::~SVT_NetscanMethod()
{

}

void SVT_NetscanMethod::start()
{

//    Q_ASSERT(mCoreThreadPool.activeThreadCount()==0);
#ifdef Q_OS_WIN32
    for(int i=0;i<NETSCAN_BASIC_RUNNABLE_NUM;i++)
    {
        SVT_NetScanRunnableRef Runnable(new SVT_NetScanRunnable(this,mNodePool->mBasicFactory_Netscan,false));
        mNetScanRunnables[i]=Runnable;
    }
    for(int i=0;i<NETSCAN_BASIC_RUNNABLE_NUM;i++)
    {
        //        SVT_NetScanRunnableInterface *test=mNetScanRunnables[i].asPointer();
        CORE_STARTRUNNABLE(mNetScanRunnables[i].asPointer())  ;
    }
    //////////////////////////////////////////////

#ifdef USE_NETSCAN_HELPER_THREAD
    for(int i=0;i<NETSCAN_HELPER_RUNNABLE_NUM;i++)
    {
        SVT_NetScanRunnable *runnable=new SVT_NetScanRunnable(this,mNodePool->mBasicFactory_Netscan,false
                                                             ,NETSCAN_HELPER_LOOPTIME);
        runnable->setAutoDelete(true);
        //        SVT_NetScanRunnableInterface *test=mNetScanRunnables[i].asPointer();
        CORE_STARTRUNNABLE(runnable)  ;
    }
#endif
    ///////////////////////////////////////////////////

    for(int i=0;i<NETSCAN_SUBSCRIB_RUNNABLE_NUM;i++)
    {
        SVT_NetScanRunnableRef Runnable
                (new SVT_NetScanRunnable(this,mNodePool->mSubscribedFactory_Netscan,true));
          mNetScanSubscribedRunnable[i]=Runnable;
    }

    for(int i=0;i<NETSCAN_SUBSCRIB_RUNNABLE_NUM;i++)
    {
        CORE_STARTRUNNABLE(mNetScanSubscribedRunnable[i].asPointer());
    }
#else
    mNetScanRunnable_UnixRef=SVT_NetScanRunnable_UnixRef(
            new SVT_NetScanRunnable_Unix(this,mNodePool->mSubscribedFactory_Netscan,mNodePool->mBasicFactory_Netscan)
            );
    CORE_STARTRUNNABLE(mNetScanRunnable_UnixRef.asPointer());
#endif


   // mCoreThreadPool.waitForDone();


}

void SVT_NetscanMethod::stop()
{

}
