#include "topology.h"
#define SLEEP_WHEN_NOSEED 1000
#define DEFINE_FILTER_DATALEN 5
bool operator <(const SVT_SeedPoint &a,const SVT_SeedPoint &b)
{
    return a.mLeftZone<b.mLeftZone;

}

SVT_NodeFilterForFactory::SVT_NodeFilterForFactory()
{

}


SVT_FilterDemo::SVT_FilterDemo(FILTER_DEMO_TYPE type)
    :mFilterType(type)
{

}

bool SVT_FilterDemo::canAddThisNode(SVT_Node *node)
{
 //   const int tempidx=(mProtocalType==TPT_NETBIOS)?0:1;
    const int cWaitTime[][DEFINE_FILTER_DATALEN]={
        {1,2,4,8,26},            //FDT_NETSCAN
        {10,10,20,60,120}, //FDT_NETBIOS
#ifdef USE_RDP
        {60,120,500,500,600},//FDT_RDP
#endif
#ifdef USE_PUTTY
       {60,120,500,500,600},//FDT_PUTTY
#endif
        {10,60,120,120,5*60}, //FDT_AFP
        {10,60,120,120,5*60}, //FDT_NETGEARSPECIFIC

    };
    const int cCheckFlags[][DEFINE_FILTER_DATALEN]=
    {
        {-1,-1,-1,-1,-1}
        ,{NIE_NETBIOS_VERSION,NIE_NETBIOS_HOSTNAME,-1,-1,-1}
#ifdef USE_RDP
        ,{NIE_RDP_ENABLE,-1,-1,-1,-1}
#endif
    #ifdef USE_PUTTY
        ,{NIE_PUTTY_ENABLE,-1,-1,-1,-1}
    #endif
        ,{NIE_NETBIOS_MACVERSION,NIE_NETBIOS_MACHOSTNAME,-1,-1,-1}
        ,{NIE_CURRENTSETTING_OK,-1,-1,-1,-1}
    };
    const int cProtocalTypes[]={TPT_NETSCAN
	,TPT_NETBIOS
#ifdef USE_RDP
	,TPT_RDP
#endif
#ifdef USE_PUTTY
                                ,TPT_PUTTY
#endif
	,TPT_AFP
	,TPT_NETGEARSPECIFIC
	};


    const int (&cCurCheckFlag)[DEFINE_FILTER_DATALEN]=cCheckFlags[mFilterType];
    const int (&cCurWaitTime)[DEFINE_FILTER_DATALEN] =cWaitTime[mFilterType];
    SVT_ProtocalSpecificData &protocalData=node->mProtocalSpecificData[mFilterType];

    SVT_NodeInfoRef nodeInfoRef=node->mInfos[cProtocalTypes[mFilterType]];

    bool allInfoExist=!nodeInfoRef.isNull();
    if(allInfoExist)
    {
        for(int i=0;i<DEFINE_FILTER_DATALEN;i++)
        {
			if(cCurCheckFlag[i]<0)
			{
				break;
			}
            if(cCurCheckFlag[i]>=0 && !nodeInfoRef->contains(cCurCheckFlag[i]))
            {
                allInfoExist=false;
            }
        }
    }
    if(allInfoExist && (cCurCheckFlag[0]>=0) )
    {
        return false;
    }
    int retrytime=protocalData.mRetryTimes;


    retrytime=qMin(DEFINE_FILTER_DATALEN-1,retrytime);

    Q_ASSERT(cCurWaitTime[retrytime] >0);
    //if( protocalData.mLastUpdate.msecsTo(QDateTime::currentDateTime())<1000*cCurWaitTime[retrytime])
    if(protocalData.mLastUpdate.addMSecs(1000*cCurWaitTime[retrytime]) >QDateTime::currentDateTime() )
    {
        return false;
    }
    protocalData.mRetryTimes++;
    protocalData.mLastUpdate=QDateTime::currentDateTime();

    return true;
}

SVT_SeedFactoryInterface::SVT_SeedFactoryInterface(SVT_NodePool *pool)
    :mPool(pool)
    ,mMutex(QMutex::Recursive)
{
}






SVT_AllNetFactory::SVT_AllNetFactory(SVT_NodePool *pool)
    :SVT_SeedFactoryInterface(pool)
    ,mReinitFlag(false)
    ,mSavedPos(0)
    ,mCallTimes(0)
{
    mMask=~(mPool->mInterface.getMask().asLong());
    //    mNetScanMethod=netscanmethod;
}

//void SVT_BasicFactory::clearAll()
//{
//    mMutex.lock();
//    mSeedPoints.clear();
//    mInitialSeeds.clear();
//    mMutex.unlock();
//}

void SVT_AllNetFactory::reGenSeeds()
{
#ifdef TEST_VALIDATE
    qDebug()<<"TEST_VALIDATE SVT_BasicFactory::reGenSeeds "<<mTestSet.count()<<mMask;
    Q_ASSERT(mMask-1==mTestSet.count() || mTestSet.isEmpty());
    mTestSet.clear();
#endif
    QList<quint32> templist;

    //    QMutex &mCoreMutex=mPool->mCore->mCoreMutex;
#ifdef Q_OS_WIN32
    if(0==mCallTimes++)
    {
        QList<SVT_IpAddress> tempmap;
        CORE_UTIL().netscanLoopArpTable(mPool->mInterface.m_idx,tempmap);
        foreach(SVT_IpAddress ip,tempmap)
        {
            templist.append(ip.asLong());
        }
    }
    else
#endif
    {
        CORE_LOCK();
        Q_ASSERT(mPool);
        templist.append(mPool->mInterface.m_ip.asLong());
        _TreeNode<SVT_NodeRef,cmpUseingIp> *pNode=mPool->mIpIndex.pFirst;
        while(pNode)
        {
            templist.append(pNode->data->mIp.asLong());
            pNode=pNode->pRSibling;
        }
        CORE_UNLOCK();
    }
    mMutex.lock();
    foreach(quint32 ip,templist)
    {
        SVT_SeedPoint point(ip%mMask);
        mSeedPoints.append(point);
        //  mInitialSeeds.append((ip&mMask)%mMask);
    }
    qSort(mSeedPoints);
    mSavedPos=0;
    mMutex.unlock();

}

SVT_SubscribedFactory::SVT_SubscribedFactory
(SVT_NodePool *pool,SVT_NodeFilterForFactory*filter,SVT_SubscribedFactory *other)
    :SVT_SeedFactoryInterface(pool)
    ,mSavedPos(0)
    ,mFilter(filter)
    ,mOtherFactory(other)
    ,mWaitTime(SLEEP_WHEN_NOSEED)
{

}


int SVT_AllNetFactory::getSeed(QList<quint32> &retlst)
{

    mMutex.lock();

    //    bool havedata=true;
    //    quint32 ret=0;
    retlst.clear();

    while (/* havedata && */retlst.count()<SEED_ALLNET_ONETIME && !mSeedPoints.isEmpty())
    {
        int ignore_count=0;
        //        if(mSeedPoints[mSavedPos].mValid)
        //        {
        if(mSeedPoints[mSavedPos].mLeftZone <=0 ||
                (mSavedPos>0 &&
                 (mMask & mSeedPoints[mSavedPos].mLeftZone) < (mMask &mSeedPoints[mSavedPos-1].mRightZone)
                 )
                )
        {
            //不用处理左边
            ignore_count++;
        }
        else
        {
            retlst.append(mMask &mSeedPoints[mSavedPos].mLeftZone);
            mSeedPoints[mSavedPos].mLeftZone--;
            if(retlst.count() >= SEED_ALLNET_ONETIME)
            {
                break;
            }
        }

        if(mSeedPoints[mSavedPos].mRightZone >= mMask ||
                (mSavedPos<mSeedPoints.count()-1&&
                 (mMask & mSeedPoints[mSavedPos].mRightZone)>(mMask & mSeedPoints[mSavedPos+1].mLeftZone)
                 )
                )
        {
            //不处理右边
            ignore_count++;
        }
        else
        {
            retlst.append(mMask & mSeedPoints[mSavedPos].mRightZone);
            //                qDebug()<<(mask & mSeedPoints[i].mRightZone);
            mSeedPoints[mSavedPos].mRightZone++;
        }
        if(ignore_count >=2)
        {
            if(mSavedPos<mSeedPoints.count()-1)
            {
                mSeedPoints[mSavedPos+1].mLeftZone=-1;
            }
            if(mSavedPos>0)
            {
                mSeedPoints[mSavedPos-1].mRightZone=mMask;
            }

            mSeedPoints.removeAt(mSavedPos);
            mSavedPos=mSeedPoints.count()?(mSavedPos%mSeedPoints.count()):0;
        }
        //    }
        if(ignore_count<2)
        {
            mSavedPos=(mSavedPos+1)%(mSeedPoints.count());
        }
    }


#ifdef TEST_VALIDATE
    foreach(quint32 t,retlst)
    {
        Q_ASSERT(!mTestSet.contains(t));
        mTestSet.insert(t);
    }

#endif
    if(retlst.count() < SEED_ALLNET_ONETIME)
    {
        reGenSeeds();
    }
    mMutex.unlock();
    return retlst.count();
}



int SVT_SubscribedFactory::getSeed(QList<quint32> &retlst)
{
    //    SVT_Core *core=mPool->mCore;
    //    Q_ASSERT(core);

    int ret=0;
    retlst.clear();
    mMutex.lock();
    while(ret <SEED_SUBSCRIB_ONETIME)
    {
        if(mSavedPos >= mCache.count())
        {
            break;
        }
        retlst.append(mCache[mSavedPos++]);

        ret++;
    }
    if(mSavedPos >= mCache.count())
    {
        mCache.clear();
        CORE_LOCK();
        SVT_Node *node=GET_CORE().mSubscribedHost.isNull()?NULL:GET_CORE().mSubscribedHost.asPointer();
        if(node && (!mFilter || mFilter->canAddThisNode(node)))
        {
            mCache.append(node->mIp.asLong());
        }
        node=GET_CORE().mSubscribedRouter.isNull()?NULL:GET_CORE().mSubscribedRouter.asPointer();
        if(node && (!mFilter || mFilter->canAddThisNode(node)))
        {
            mCache.append(node->mIp.asLong());
        }

        _ListNode<SVT_NodeRef> *pLoop=GET_CORE().mSubscribedList.pFirst;
        while(pLoop)
        {
            if(!mFilter|| mFilter->canAddThisNode(pLoop->data.asPointer()))
            {
                mCache.append(pLoop->data->mIp.asLong());
            }
            pLoop=pLoop->pNext;
        }
        CORE_UNLOCK();
        mSavedPos=0;
    }
    mMutex.unlock();

    if(ret<=0 && mOtherFactory)
    {
        ret=mOtherFactory->getSeed(retlst);
    }

    if(ret<=0 && mCache.isEmpty())
    {
        for(int i=0;i<mWaitTime&& !GET_CORE().mCoreExit;i+=100)
        {
            SLEEP(100);
        }
    }

    return ret;


    //........
}

SVT_AllResultsFactory::SVT_AllResultsFactory(SVT_NodePool *pool,SVT_NodeFilterForFactory*filter)
    :SVT_SeedFactoryInterface(pool)
    ,mSavedPos(0)
    ,mFilter(filter)
    ,mWaitTime(SLEEP_WHEN_NOSEED)
{

}


int SVT_AllResultsFactory::getSeed(QList<quint32> &retlst)
{
    //    SVT_Core *core=mPool->mCore;
    //    Q_ASSERT(core);

    int ret=0;
    retlst.clear();
    mMutex.lock();
    while(ret <SEED_ALLRESULT_ONTIME)
    {
        if(mSavedPos >= mCache.count())
        {
            break;
        }
        retlst.append(mCache[mSavedPos++]);

        ret++;
    }
    if(mSavedPos >= mCache.count())
    {
        mCache.clear();
        CORE_LOCK();

        _TreeNode<SVT_NodeRef,cmpUseingIp> *pLoop=mPool->mIpIndex.pFirst;
        while(pLoop)
        {
            if(!mFilter|| mFilter->canAddThisNode(pLoop->data.asPointer()))
            {
                mCache.append(pLoop->data->mIp.asLong());
            }
            pLoop=pLoop->pRSibling;
        }
        CORE_UNLOCK();
        mSavedPos=0;
    }
    mMutex.unlock();
    if(ret<=0 && mCache.isEmpty())
    {
        for(int i=0;i<mWaitTime&& !GET_CORE().mCoreExit;i+=100)
        {
            SLEEP(100);
        }
    }
    return ret;


    //........
}
