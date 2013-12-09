#include "topology.h"
#include "SvtNetbiosMethod.h"

SVT_NetbiosMethod::SVT_NetbiosMethod(SVT_NodePool *pool)
    :SVT_TopoMethod(TPT_NETBIOS,pool)
//    ,mCoreThreadPool(pool->mCoreThreadPool)
{

}

void SVT_NetbiosMethod::start()
{

    for(int i=0;i<NETBIOS_BASIC_RUNNABLE_NUM;i++)
    {

        SVT_NetbiosRunnableRef Runnable(new SVT_NetbiosRunnable(this,mNodePool->mAllResultsFactory_Netbios));
        mNetbiosRunnables[i]=Runnable;
    }
    for(int i=0;i<NETBIOS_BASIC_RUNNABLE_NUM;i++)
    {
        //        SVT_NetScanRunnableInterface *test=mNetScanRunnables[i].asPointer();
        CORE_STARTRUNNABLE(mNetbiosRunnables[i].asPointer())  ;
    }

    for(int i=0;i<NETBIOS_SUBSCRIB_RUNNABLE_NUM;i++)
    {
        SVT_NetbiosRunnableRef Runnable
                (new SVT_NetbiosRunnable(this,mNodePool->mSubscribedFactory_Netbios));
          mNetbiosSubscribedRunnable[i]=Runnable;
    }

    for(int i=0;i<NETBIOS_SUBSCRIB_RUNNABLE_NUM;i++)
    {
        CORE_STARTRUNNABLE(mNetbiosSubscribedRunnable[i].asPointer());
    }
}

void SVT_NetbiosMethod::stop()
{

}
