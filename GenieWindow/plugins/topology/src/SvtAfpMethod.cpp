#include "topology.h"
#include "SvtAfpMethod.h"

SVT_AfpMethod::SVT_AfpMethod(SVT_NodePool *pool)
    :SVT_TopoMethod(TPT_AFP,pool)
{

}

void SVT_AfpMethod::start()
{

    for(int i=0;i<AFP_BASIC_RUNNABLE_NUM;i++)
    {

        SVT_AfpRunnableRef Runnable(new SVT_AfpRunnable(this,mNodePool->mAllResultsFactory_Afp));
        mAfpRunnables[i]=Runnable;
    }
    for(int i=0;i<AFP_BASIC_RUNNABLE_NUM;i++)
    {
        //        SVT_NetScanRunnableInterface *test=mNetScanRunnables[i].asPointer();
        CORE_STARTRUNNABLE(mAfpRunnables[i].asPointer())  ;
    }

    for(int i=0;i<AFP_SUBSCRIB_RUNNABLE_NUM;i++)
    {
        SVT_AfpRunnableRef Runnable
                (new SVT_AfpRunnable(this,mNodePool->mSubscribedFactory_Afp));
          mAfpSubscribedRunnable[i]=Runnable;
    }

    for(int i=0;i<AFP_SUBSCRIB_RUNNABLE_NUM;i++)
    {
        CORE_STARTRUNNABLE(mAfpSubscribedRunnable[i].asPointer());
    }
}

void SVT_AfpMethod::stop()
{

}
