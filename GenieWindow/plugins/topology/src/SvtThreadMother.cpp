#include "topology.h"

extern quint32 sMapFlags;
SVT_ThreadMother::SVT_ThreadMother(SVT_Core *core)
    :mCore(core)
{
    mThreadPool.setMaxThreadCount(100);
}

SVT_ThreadMother::~SVT_ThreadMother()
{
    wait();
}

void SVT_ThreadMother::run()
{
    int counter=0;
    while(!mCore->mCoreExit)
    {
        counter++;
        SLEEP(200);
//        continue;
        SVT_NodePoolRef lazyDelete;
        mMutex.lock();
        SVT_NodePoolRef waitpool=mNodePoolWaited;
        mNodePoolWaited=NULL;
        mMutex.unlock();

        if(!waitpool.isNull() && mNodePool!=waitpool)
        {
//            wantStart=true;

            if(!mNodePool.isNull())
            {
                mNodePool->mPoolExit=true;
                mNodePool->stop();
            }
            waitpool->mPoolExit=true;
            waitpool->stop();
            mThreadPool.waitForDone();
            lazyDelete=mNodePool;
            mNodePool=waitpool;
            CORE_LOCK();
#if USE_NETBIOS_METHOD
            if(sMapFlags &MF_USE_NETBIOS_SCAN)
            {
                SVT_TopoMethodRef method1(new SVT_NetbiosMethod(mNodePool.asPointer()));
                mNodePool->addMethod(method1);
            }
#endif
            if(sMapFlags & MF_USE_ARP_SCAN)
            {
                SVT_TopoMethodRef method2(new SVT_NetscanMethod(mNodePool.asPointer()));
                mNodePool->addMethod(method2);
            }
#if USE_UPNP_METHOD
            if(sMapFlags & MF_USE_UPNP_SCAN)
            {
                SVT_TopoMethodRef method3(new SVT_UpnpMethod(mNodePool.asPointer()));
                mNodePool->addMethod(method3);
            }
#endif
#if USE_NETGEARSPECIFIC_METHOD
            if(sMapFlags & MF_USE_NETGEARSPECIFIC_SCAN)
            {
                SVT_TopoMethodRef method4(new SVT_NetgearSpecificMethod(mNodePool.asPointer()));
                mNodePool->addMethod(method4);
            }
#endif
#if USE_AFP_METHOD
            if(sMapFlags & MF_USE_AFP_SCAN)
            {
                SVT_TopoMethodRef method5(new SVT_AfpMethod(mNodePool.asPointer()));
                mNodePool->addMethod(method5);
            }
#endif
#ifdef USE_RDP
            if(sMapFlags & MF_USE_RDP_SCAN)
            {
                SVT_TopoMethodRef method6(new SVT_RdpMethod(mNodePool.asPointer()));
                mNodePool->addMethod(method6);
            }
#endif
#ifdef USE_PUTTY
            if(sMapFlags & MF_USE_PUTTY_SCAN)
            {
                SVT_TopoMethodRef method7(new SVT_PuttyMethod(mNodePool.asPointer()));
                mNodePool->addMethod(method7);
            }
#endif


            mNodePool->start();
            CORE_UNLOCK();
        }
        lazyDelete=NULL;

    }

    mMutex.lock();
    if(!mNodePool.isNull())
    {
        mNodePool->mPoolExit=true;
    }
    if(!mNodePoolWaited.isNull())
    {
        mNodePoolWaited->mPoolExit=true;
    }
    mMutex.unlock();
    mThreadPool.waitForDone();

}

void SVT_ThreadMother::stop()
{

}

void SVT_ThreadMother::setPool(SVT_NodePoolRef ref)
{
    mMutex.lock();
    mNodePoolWaited=ref;
    mMutex.unlock();
}
