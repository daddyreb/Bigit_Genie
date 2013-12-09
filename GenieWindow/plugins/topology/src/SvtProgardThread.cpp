#include "topology.h"

#define NO_NORMAL_NODECOUNT 2

bool clipNotifyData(SVT_NotifyType &data,const QSet<quint32> &set)
{
    bool ret=false;
    for(int i=data.count()-1;i>=0;i--)
    {
        bool btemp;
        int itemp=data[i].value(IIE_IDX,"null").toUInt(&btemp);
        Q_ASSERT(btemp);
        if(btemp && !set.contains(itemp))
        {
            data.removeAt(i);
            ret=true;
        }
    }
    return ret;
}

SVT_ProgardThread::SVT_ProgardThread(SVT_Core *core)
    :mCore(core)
    //    ,mLastNotifyCount(0)
    ,mProgardFlags(0)
    ,mSubscribStartIdx(-1)
    ,mSubscribCount(-1)
    ,mSbuscribNetInterfaceIdx(-1)
    ,mCountCache(-1)
	,mExtenderIp(0)
	//add by caiyujun
	//add end
{

    //    setAutoDelete(false);
}

SVT_ProgardThread::~SVT_ProgardThread()
{
//    if(!mCore->mPool.isNull())
//    {
//        mCore->mPool->mThreadPool.waitForDone();
//    }
//    if(!mCore->mLazyDeletePool.isNull())
//    {
//        mCore->mLazyDeletePool->mThreadPool.waitForDone();
//    }
    //    mCore->mCoreThreadPool.waitForDone();
}

#ifdef NOTIFY_ADDED_DEVICE
void SVT_ProgardThread::_notifyNewDevice()
{
    //mCoreMutex must be locked here
    _TreeNode<SVT_NodeRef,cmpUseingIp> *pNode;
    SVT_NodePoolRef pool=mCore->mPool;
    Q_ASSERT(!mCore->mPool.isNull());
    SVT_NotifyType data;

    pNode=pool->mIpIndex.pFirst;

    while(pNode)
    {
        if(pNode->data->mFlags & SVT_Node::NF_ISNEW)
        {
            _addNodeToNotifyData(&data,pNode->data);
            pNode->data->mFlags &=~SVT_Node::NF_ISNEW;

        }
        pNode=pNode->pRSibling;
    }
    if(!data.isEmpty() && mStartTime.secsTo(QDateTime::currentDateTime())>NOTIFY_NEWDEVICE_INTERVAL)
    {
        mCore->mControlInterface->funcNewDeviceAdded(data);
    }
}
#endif
#ifdef Q_OS_MAC
#define MUST_REFRESH_IFACE() (0==(counter%200))
#else
#define MUST_REFRESH_IFACE() (0==(counter%100))
#endif

void SVT_ProgardThread::run()
{
    //    QList<SVT_NetworkInterface> ifaces;
    //    mCore->mCoreUtils.netscanGetAllAdpInfo(ifaces);

    int counter=-1;
    while(!mCore->mCoreExit)
    {
        counter++;
        mProgardMutex.lock();
        bool speedup=mProgardFlags&PF_LOOP_SPEEDUP;
        mProgardFlags&=~PF_LOOP_SPEEDUP;
        mProgardMutex.unlock();
        SLEEP(speedup?0:100);

        if(speedup || MUST_REFRESH_IFACE() )
        {
            checkInterface();
        }


        //if no nonormal nodes start
        quint32 tempflag=0;
        CORE_LOCK();
        if(!mCore->mPool.isNull() && (mCore->mSubscribedHost.isNull() || mCore->mSubscribedRouter.isNull()))
        {
            mCore->mPool->_genNoNormalNodes();
            tempflag |= SVT_ProgardThread::PF_NEED_REGEN_SUBSCRIB_LIST;
        }
        CORE_UNLOCK();
        if(tempflag)
        {
            setFlag(tempflag);
        }
        //if no nonormal nodes end

        qint32 flags,scbStart,scbCount,scbIfaceIdx;
        mProgardMutex.lock();
        flags=mProgardFlags;
        mProgardFlags &= ~(SVT_ProgardThread::PF_NEED_NOTIFY_SUBSCRIB_CHANGE
                           |SVT_ProgardThread::PF_NEED_REGEN_SUBSCRIB_LIST
                           |SVT_ProgardThread::PF_NEWDEVICE_ADDED
                           /*|SVT_ProgardThread::PF_NEED_REGEN_POOL*/);
        scbStart=mSubscribStartIdx;
        scbCount=mSubscribCount;
        scbIfaceIdx=mSbuscribNetInterfaceIdx;
        mProgardMutex.unlock();

//        int itemp=0;
//        qDebug()<<"t: flags is"<<flags;
//        printf("t: flags is %d %d\n",flags,itemp++);
//        if(!flags)
//        {
//            for(int i=0;(i<20)&&!mCore->mCoreExit;i++)
//            {
//                mProgardMutex.lock();
//                flags=mProgardFlags;
//                mProgardMutex.unlock();
//                if(flags||MUST_REFRESH_IFACE())
//                {
//                    break;
//                }
//                counter++;
//                SLEEP(500);
//            }
//        }

        if(!flags || (flags&(PF_LOOP_SPEEDUP|PF_NEED_REGEN_POOL)) )
        {

            continue;
        }

        CORE_LOCK();
        if(scbStart<0 || scbCount<=0 ||scbIfaceIdx<0)
        {
            mCore->clearSubscribList();
        }
        else
        {
            if( flags &SVT_ProgardThread::PF_NEED_REGEN_SUBSCRIB_LIST && !mCore->mPool.isNull())
            {
                regenSubscribList(scbStart,scbCount);
            }
#ifdef NOTIFY_ADDED_DEVICE
            if(flags & SVT_ProgardThread::PF_NEWDEVICE_ADDED && !mCore->mPool.isNull())
            {
                _notifyNewDevice();
            }
#endif

            if(flags & (SVT_ProgardThread::PF_NEED_NOTIFY_SUBSCRIB_CHANGE|SVT_ProgardThread::PF_NEED_REGEN_SUBSCRIB_LIST) )
            {
                _notifySubscribed();
            }

            _notifyDeviceNum();


        }
        CORE_UNLOCK();
//        mCore->mLazyDeletePool=NULL;
    }//while(!mCore->mCoreExit)
    qDebug()<<"SVT_ProgardThread exit"<<QThread::currentThreadId();
}

void SVT_ProgardThread::_notifyDeviceNum()
{
	//modify by caiyujun
    int allCount=mCore->mPool.isNull()?0:mCore->mPool->mIpIndex.count();
	int currentCount = 0;
	int routeCount = 0;
	if (!mCore->mPool.isNull())
	{
		int tmpCount = 0;
		_TreeNode<SVT_NodeRef,cmpUseingIp> *pNode;
		SVT_NodePoolRef pool=mCore->mPool;
		pNode=pool->mIpIndex.pFirst;
		int allcount=pool->mIpIndex.count();
		for(int i=0;i<allcount;i++)
		{
		
			if (mProgardFlags & PF_NEED_REGEN_EXTENDER_LIST)
			{
				if (pNode->data->mParentIp == mExtenderIp) 
					//|| (pNode->data->mFlags & SVT_Node::NF_ISDEFAULTGATEWAY))
				{
					tmpCount++;
				}

				if (pNode->data->mParentIp == 0)
				{
					routeCount++;
				}
			}
			else
			{
				if (pNode->data->mParentIp == 0)
				{
					tmpCount++;
					routeCount++;
				}
			}
			pNode=pNode->pRSibling;
		}
		currentCount = tmpCount;
	}
	//modify end
    if(mCore->mSubscribedHost.isNull()|| mCore->mSubscribedRouter.isNull())
    {
        currentCount=0;
    }
    if(mCountCache!=currentCount)
    {
        mCountCache=currentCount;
        if(mCore->mControlInterface)
        {
			if (mProgardFlags & PF_NEED_REGEN_EXTENDER_LIST)
			{
				currentCount++;
			}
			mCore->mControlInterface->countChanged(currentCount);
		}
	}

	/*if (!mCore->mSubscribedRouter.isNull())
	{
		QString strRouter = mCore->mSubscribedRouter->mIp.toString();
		mCore->mControlInterface->otherDeviceCountChanged(strRouter,routeCount-1);
	}

	_ListNode<SVT_NodeRef> *pLoop=GET_CORE().mExtenderList.pFirst;
	while(pLoop)
	{	
		if(!pLoop->data.isNull() && (!mCore->mPool.isNull()))
		{
			QString strIp = pLoop->data->mIp.toString();
			unsigned long extenderIp = inet_addr(strIp.toStdString().c_str());
			_TreeNode<SVT_NodeRef,cmpUseingIp> *pNode;
			SVT_NodePoolRef pool=mCore->mPool;

			pNode=pool->mIpIndex.pFirst;
			int allcount=pool->mIpIndex.count();
			int iCount = 0;
			for(int i=0;i<allcount;i++)
			{
				if (pNode->data->mParentIp == extenderIp) 
				{
					iCount++;
				}
				pNode=pNode->pRSibling;
			}
			mCore->mControlInterface->otherDeviceCountChanged(strIp,iCount);

		}
		pLoop = pLoop->pNext;
	}*/

}

void SVT_ProgardThread::setFlag(quint32 flags)
{
    mProgardMutex.lock();
    mProgardFlags |= flags;
    mProgardMutex.unlock();
}

void SVT_ProgardThread::restart()
{
    mProgardMutex.lock();
    if(0==(mProgardFlags & PF_INRESTARTING))
    {
        mProgardFlags |=(PF_INRESTARTING|PF_NEED_REGEN_POOL|PF_LOOP_SPEEDUP);
    }
    mProgardMutex.unlock();
}

void SVT_ProgardThread::stop()
{
    mCore->mPool=NULL;
//    if(!mCore->mPool.isNull())
//    {
//        mCore->mPool->mThreadPool.waitForDone();
//    }
//    if(!mCore->mLazyDeletePool.isNull())
//    {
//        mCore->mLazyDeletePool->mThreadPool.waitForDone();
//    }
}



void SVT_ProgardThread::checkInterface()
{
    //mCoreMutex must be locked here
    int scbIface;
    mProgardMutex.lock();
    scbIface=mSbuscribNetInterfaceIdx;
    mProgardMutex.unlock();

    //step1 check if network interface changed
    QList<SVT_NetworkInterface> ifaces;

    for(int i=0;(i<5)&&!mCore->mCoreExit;i++)
    {
        CORE_UTIL().netscanGetAllAdpInfo(ifaces);
        if(!ifaces.isEmpty())
        {
            break;
        }
        SLEEP(500);
    }
    _checkDefaultGatewayChange(ifaces);

    static SVT_NotifyType sIfaceData;
    bool bNeedNotify=false;
    int scbListIdx=-1;
    int tempidx;
    QSet<quint32> ifaceSet;
    bool bMustRegenPool=false;
    for(int i=0;i<ifaces.count();i++)
    {
        ifaceSet.insert(ifaces[i].m_idx);
        if(ifaces[i].m_idx==scbIface)
        {
            scbListIdx=i;
            if(!mCore->mPool.isNull() &&
               (
                       !( mCore->mPool->mInterface.getMac()==ifaces[i].getMac())
                       || !(mCore->mPool->mInterface.getIp()==ifaces[i].getIp())
                       )
                )
                {
                bMustRegenPool=true;
            }
        }

        QList<QMap<int,QString> >::Iterator it=
                searchNotifyData(sIfaceData,ifaces[i].m_idx,tempidx);
        QMap<int,QString> tempmap;
        tempmap.insert(IIE_IDX,QString::number(ifaces[i].m_idx));
        tempmap.insert(IIE_ISDEFAULT,ifaces[i].isDefault()?"y":"n");
        tempmap.insert(IIE_NAME,ifaces[i].m_name);
        tempmap.insert(IIE_ISON,(ifaces[i].isOn()&& ifaces[i].isValid())?"y":"n");
        tempmap.insert(IIE_ISVISUAL,ifaces[i].isVisual()?"y":"n");
        tempmap.insert(IIE_WIRED,ifaces[i].isWireless()?"n":"y");
        tempmap.insert(IIE_IP,ifaces[i].getIp().toString());
        tempmap.insert(IIE_MAC,ifaces[i].getMac().toString());
        tempmap.insert(IIE_GATEWAY,ifaces[i].getGateway().toString());


        if(it==sIfaceData.end())
        {
            sIfaceData.append(tempmap);
            bNeedNotify=true;
        }
        else
        {
            if(mergeMap((*it),tempmap) )
            {
                bNeedNotify=true;
            }
        }
    }
    bNeedNotify=clipNotifyData(sIfaceData,ifaceSet)||bNeedNotify;



    if(bNeedNotify && mCore && mCore->mControlInterface)
    {
        mCore->mControlInterface->funcIfaceReturned(sIfaceData);
    }

    //step2 regen pool if needed
    bool ifacereopen=false;
    QList<QMap<int,QString> >::Iterator it2=
            searchNotifyData(sIfaceData,scbIface,tempidx);
    mProgardMutex.lock();
    bMustRegenPool=bMustRegenPool || (mProgardFlags&SVT_ProgardThread::PF_NEED_REGEN_POOL);
    mProgardFlags &= ~SVT_ProgardThread::PF_NEED_REGEN_POOL;
    mProgardMutex.unlock();

    if(it2==sIfaceData.end() ||!(ifaces[scbListIdx].isValid() && ifaces[scbListIdx].isOn()) )
    {
        ifacereopen=!mCore->mPool.isNull();
        mCore->mPool=NULL;

    }
    else if(mCore->mPool.isNull() || mCore->mPool->mInterface.m_idx!= scbIface || bMustRegenPool)
    {
        Q_ASSERT(scbListIdx>=0);
        Q_ASSERT(scbIface>=0);
        bool brefresh=!mCore->mPool.isNull()&& ( mCore->mPool->mInterface.m_idx== scbIface);


        ifacereopen=true;
        SVT_NodePoolRef pool(new SVT_NodePool(ifaces[scbListIdx]));
        //                pool->setNetworkInterface(ifaces[i]);

        CORE_LOCK();
        if(!mCore->mPool.isNull())
        {
            mCore->mPool->serialize(false);
            mCore->mPool->mPoolExit=true;
//            mCore->mLazyDeletePool=mCore->mPool;
        }
        mCore->mPool=pool;
        pool->reload(!brefresh);
        mCore->mThreadMother->setPool(pool);
        CORE_UNLOCK();

        //在等待结束过程中，可能这个值又被设置上了，所以再次清零
        mProgardMutex.lock();
        mProgardFlags &=~SVT_ProgardThread::PF_NEED_REGEN_POOL;
        mProgardMutex.unlock();


        mProgardMutex.lock();
        mProgardFlags |= SVT_ProgardThread::PF_NEED_REGEN_SUBSCRIB_LIST;
        mProgardFlags &= ~SVT_ProgardThread::PF_INRESTARTING;
        mProgardMutex.unlock();
        mStartTime=QDateTime::currentDateTime();

    }
    else if(!mCore->mPool.isNull())
    {
        Q_ASSERT(scbListIdx>=0);
        Q_ASSERT(scbIface>=0);
        mCore->mPool->mInterface=ifaces[scbListIdx];
    }

    //step3 reinit subscrib message when pool chaged
    if(ifacereopen)
    {
        mProgardMutex.lock();
        //     mProgardFlags=0;
        //        mSubscribStartIdx=-1;
        //        mSubscribCount=-1;
        //        mSbuscribNetInterfaceIdx=-1;
        mCountCache=-1;
        mProgardMutex.unlock();
        Q_ASSERT(mCore && mCore->mControlInterface);
        SVT_NotifyType nulldata;
        mCore->clearSubscribList();
        mCore->mControlInterface->funcDataReturned(nulldata);
        mCore->mControlInterface->countChanged(0);
        setFlag(PF_NEED_REGEN_SUBSCRIB_LIST);
    }
}

void SVT_ProgardThread::regenSubscribList(const int &start,const int &count)
{
    //mCoreMutex must be locked here
    _TreeNode<SVT_NodeRef,cmpUseingIp> *pNode;
    SVT_NodePoolRef pool=mCore->mPool;
    Q_ASSERT(!mCore->mPool.isNull());
	int iCount = count;
    //    Q_ASSERT(mSubScribedPool);
	//add by caiyujun
	if (mProgardFlags & PF_NEED_REGEN_EXTENDER_LIST)
	{
		if (!GET_CORE().mSubscribedHost.isNull() && mExtenderIp != GET_CORE().mSubscribedHost->mParentIp)
		{
			iCount++;
		}
	}
	else
	{
		if (!GET_CORE().mSubscribedHost.isNull() && 0 != GET_CORE().mSubscribedHost->mParentIp)
		{
			iCount++;
		}
	}
	//add end
    mCore->clearSubscribList();
    int allcount=pool->mIpIndex.count();
    if(start> allcount-NO_NORMAL_NODECOUNT)
    {
        qDebug()<<"start index too large";
        return;
    }
    pNode=pool->mIpIndex.pFirst;

    int zeroToStart=0;
    int retNum=0;
    for(int i=0;i<allcount;i++)
    {
        bool isNormal=(0==(pNode->data->mFlags&(SVT_Node::NF_ISHOST|SVT_Node::NF_ISGATEWAY)));
		//add by caiyujun
		bool isExtenderDevice = pNode->data->mFlags&(SVT_Node::NF_ISEXTENDER_ATTATCHDEVICE);


#ifdef TEST_VALIDATE

        Q_ASSERT(pNode && !pNode->data.isNull());
        if(pNode->data->mFlags & SVT_Node::NF_ISDEFAULTGATEWAY)
        {
            Q_ASSERT(pNode->data->mFlags & SVT_Node::NF_ISGATEWAY);
        }
#endif

		//modify by caiyujun
        if(zeroToStart>=start && isNormal && retNum<iCount)
        {
			if (mProgardFlags & PF_NEED_REGEN_EXTENDER_LIST)
			{
				if (isExtenderDevice && (mExtenderIp == pNode->data->mParentIp))
				{
					mCore->mSubscribedList.append(pNode->data);
					pNode->data->mFlags |= SVT_Node::NF_SUBSCRIBED;
					retNum++;
				}
			}
			else
			{
				if (!isExtenderDevice)
				{
					mCore->mSubscribedList.append(pNode->data);
					pNode->data->mFlags |= SVT_Node::NF_SUBSCRIBED;
					retNum++;
				}
			}
            
        }
		//modify end
        //        qDebug()<<"taonode"<<pNode->data->mIp.toString()<<pNode->data->mFlags;

        if(!isNormal)
        {
            if(pNode->data->mFlags& SVT_Node::NF_ISHOST)
            {
                GET_CORE().setSubscribedHost(pNode->data);
                pNode->data->mFlags|=SVT_Node::NF_SUBSCRIBED;
            }
            else
            {
#ifdef TEST_VALIDATE
                Q_ASSERT(pNode->data->mFlags& SVT_Node::NF_ISGATEWAY);
#endif
                GET_CORE().setSubscribedRouter(pNode->data);
                pNode->data->mFlags|=SVT_Node::NF_SUBSCRIBED;

            }
        }
        if(zeroToStart<start &&isNormal)
        {
			//modify by caiyujun
			if (mProgardFlags & PF_NEED_REGEN_EXTENDER_LIST)
			{
				if (isExtenderDevice && (mExtenderIp == pNode->data->mParentIp))
				{
					zeroToStart++;
				}
			}
			else
			{
				if (!isExtenderDevice)
				{
					zeroToStart++;
				}
			}
			//modify end
        }
        pNode=pNode->pRSibling;
    }//for(int i=0;i<allcount;i++)

    //    if(mCore->mSubscribedHost.isNull() || mCore->mSubscribedRouter.isNull())
    //    {
    //        qDebug()<<"mCore->mPool->_genNoNormalNodes();";
    //        mCore->mPool->_genNoNormalNodes();
    //    }

#if 0
    for(int i=0;i<start;i++)
    {
        Q_ASSERT(pNode);
        pNode=pNode->pRSibling;
    }
    int subscribed_count=0;
    while(pNode)
    {
        mCore->mSubscribedList.append(pNode->data);
        pNode->data->mFlags |= SVT_Node::NF_SUBSCRIBED;
        if((++subscribed_count)>=count)
        {
            break;
        }
        pNode=pNode->pRSibling;
    }
#endif
}

void SVT_ProgardThread::_addNodeToNotifyData(SVT_NotifyType *data,SVT_NodeRef &noderef)
{
    SVT_NodeInfoRef info(new QMap<int,QString>);
    //        QMap<int,QString> info;
    info->insert(NIE_IP,noderef->mIp.toString());
    info->insert(NIE_MAC,noderef->mMac.toString());
    quint32 flags=noderef->mFlags;
    info->insert(NIE_ONLINE,
                 (flags&SVT_Node::NF_OFFLINE)?"n":"y");

    const struct _{
        quint32 checkflags;
        int setflag;
    } cLogics[]=
    {
        {SVT_Node::NF_ISHOST,NIE_ISHOST},
        {SVT_Node::NF_ISWIRED,NIE_ISWIRED},
        {SVT_Node::NF_ISGATEWAY,NIE_ISGATEWAY},
        {SVT_Node::NF_ISDEFAULTGATEWAY,NIE_ISDEFAULT_GATEWAY},
        {SVT_Node::NF_ISVIRTUALMACHINE,NIE_ISVIRTUALMACHINE}
    };

    for(int i=0;i<sizeof(cLogics)/sizeof(cLogics[0]);i++)
    {
        if(flags & cLogics[i].checkflags)
        {
            info->insert(cLogics[i].setflag,"y");
        }
    }

    for(int i=0;i<TPT_NUM;i++)
    {
        mergeMap(info,noderef->mInfos[i]);
    }
    Q_ASSERT(!info->isEmpty());
    data->append(*info);

}

void SVT_ProgardThread::_checkDefaultGatewayChange(const QList<SVT_NetworkInterface>& ifaces)
{
    static SVT_IpAddress sIp;
    static SVT_MacAddress sMac;
    static QDateTime sLastUpdate=QDateTime::currentDateTime().addDays(-1);
    bool bNeedNotify=false;
    //    bool bNeedRestart=false;
    int defaultIdx=-1;

    for(int i=0;i<ifaces.count();i++)
    {
        if(ifaces[i].isDefault())
        {
            defaultIdx=i;
            if(!(sIp==ifaces[i].getGateway()) )
            {
                sIp=ifaces[i].getGateway();
                bNeedNotify=true;
                sMac=CORE_UTIL().netscanGetMacForRouter(sIp,ifaces[defaultIdx].getIp());
                //                bNeedRestart=true;
            }
            break;
        }
    }
    if(defaultIdx <0 && sIp.isValid())
    {
        sIp=SVT_IpAddress();
        sMac=SVT_MacAddress();
        bNeedNotify=true;
        //        bNeedRestart=true;
    }
    if(sLastUpdate.addMSecs(CHECK_GATEWAY_INTERVAL) < QDateTime::currentDateTime() )
        //     sLastUpdate.msecsTo(QDateTime::currentDateTime()) > CHECK_GATEWAY_INTERVAL)
    {
        sLastUpdate=QDateTime::currentDateTime();
        if(!bNeedNotify && defaultIdx >=0)
        {
            bool btemp;
            SVT_MacAddress mac=CORE_UTIL().netscanGetMacForRouter(sIp,ifaces[defaultIdx].getIp(),&btemp);
            bNeedNotify=(btemp && !(mac ==sMac) && sMac.isValid());
            sMac=mac;
        }
    }
    if(bNeedNotify)
    {
        //        setFlag(SVT_ProgardThread::PF_NEED_REGEN_POOL);
        mCore->mControlInterface->funcGatewayChanged(sIp.toString()+" "+sMac.toString());
    }

}

void SVT_ProgardThread::_notifySubscribed()
{
    //mCoreMutex must be locked here
    SVT_NotifyType data;
    if(mCore->mSubscribedHost.isNull()|| mCore->mSubscribedRouter.isNull())
    {
        qDebug()<<"_notifySubscribed but mCore->mSubscribedHost.isNull()|| mCore->mSubscribedRouter.isNull()!!!";
        qDebug()<<mCore->mSubscribedHost.isNull()<<mCore->mSubscribedRouter.isNull();
        return;
    }

	if (mProgardFlags & PF_NEED_REGEN_EXTENDER_LIST)
	{
		if (!mCore->mSubscribedHost.isNull() && mExtenderIp == mCore->mSubscribedHost->mParentIp)
		{
			_addNodeToNotifyData(&data,mCore->mSubscribedHost);
			mCore->mSubscribedHost->mFlags &= ~(SVT_Node::NF_CHANGED_AFTER_SUBSCRIB|SVT_Node::NF_DELETE_AFTER_SUBSCRIB);
		}
	}
	else
	{
		if (!mCore->mSubscribedHost.isNull() && 0 == mCore->mSubscribedHost->mParentIp)
		{
			_addNodeToNotifyData(&data,mCore->mSubscribedHost);
			mCore->mSubscribedHost->mFlags &= ~(SVT_Node::NF_CHANGED_AFTER_SUBSCRIB|SVT_Node::NF_DELETE_AFTER_SUBSCRIB);
		}
	}
	
    
    //modify by caiyujun
    if (mProgardFlags & PF_NEED_REGEN_EXTENDER_LIST)
    {
        //_addNodeToNotifyData(&data,mCore->mExtenderList.pFirst->data);
		//if (mExtenderIp

		_ListNode<SVT_NodeRef> *pLoop=GET_CORE().mExtenderList.pFirst;
		while(pLoop)
		{	
			if(!pLoop->data.isNull())
			{
				QString strIp = pLoop->data->mIp.toString();
				unsigned long extenderIp = inet_addr(strIp.toStdString().c_str());
				if (extenderIp == mExtenderIp)
				{
					_addNodeToNotifyData(&data,pLoop->data);
					break;
				}
			}
			pLoop = pLoop->pNext;
		}
    }
    else
    {
        _addNodeToNotifyData(&data,mCore->mSubscribedRouter);
        mCore->mSubscribedRouter->mFlags &= ~(SVT_Node::NF_CHANGED_AFTER_SUBSCRIB|SVT_Node::NF_DELETE_AFTER_SUBSCRIB);
    }
    //add end

	//modify by caiyujun
	if (mProgardFlags & PF_NEED_REGEN_EXTENDER_LIST)
	{
		_addNodeToNotifyData(&data,mCore->mSubscribedRouter);

		_ListNode<SVT_NodeRef> *pLoop=mCore->mSubscribedList.pFirst;
		while(pLoop)
		{
			_addNodeToNotifyData(&data,pLoop->data);
			pLoop->data->mFlags &= ~(SVT_Node::NF_CHANGED_AFTER_SUBSCRIB|SVT_Node::NF_DELETE_AFTER_SUBSCRIB);

			pLoop=pLoop->pNext;
		}
	}
	else
	{
		_ListNode<SVT_NodeRef> *pLoop=mCore->mSubscribedList.pFirst;
		while(pLoop)
		{
			_addNodeToNotifyData(&data,pLoop->data);
			pLoop->data->mFlags &= ~(SVT_Node::NF_CHANGED_AFTER_SUBSCRIB|SVT_Node::NF_DELETE_AFTER_SUBSCRIB);

			pLoop=pLoop->pNext;
		}
    }
	//modify end
    Q_ASSERT(mCore && mCore->mControlInterface);
    //qDebug()<<"funcDataReturned from topology!!!"<<data.count();
    if(!data.isEmpty())
    {

        mCore->mControlInterface->funcDataReturned(data);
    }

}

