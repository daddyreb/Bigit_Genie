#include "topology.h"
#include "SvtCore.h"
#include <QDesktopServices>






#define CHECK_OK(x) do{bool b;b=x;Q_ASSERT(b);}while(0)

bool mergeMap(QMap<int,QString> &dest,const QMap<int,QString> &src)
{
    bool ret=false;
    QMap<int, QString>::const_iterator i = src.constBegin();
    while (i != src.constEnd()) {
        if(!dest.contains(i.key()) || dest.value(i.key())!=i.value())
        {
            ret=true;
            dest.insert(i.key(),i.value());
        }
        ++i;
    }
    return ret;
}

bool mergeMap(SVT_NodeInfoRef &dest,const SVT_NodeInfoRef &src)
{
    if( src.isNull())
    {
        return false;
    }
    if(dest.isNull())
    {
        dest=src;
        return true;
    }
    return mergeMap(*dest,*src);
}

QList<QMap<int,QString> >::Iterator searchNotifyData(SVT_NotifyType &data,const int &iface,int &idx)
{
    QList<QMap<int,QString> >::Iterator it=data.begin();
    QString qstemp=QString::number(iface);
    int ret=-1;
    while(it!=data.end())
    {
        ++ret;
        if((*it).value(IIE_IDX,"null")==qstemp)
        {
            break;
        }
        it++;
    }
    idx=ret;
    return it;
}

SVT_ProtocalSpecificData::SVT_ProtocalSpecificData()
{
    mLastUpdate=QDateTime::currentDateTime().addDays(-1);
    mRetryTimes=0;
}

//static int sPoolCounter=0;
//#ifdef TEST_VALIDATE
//int SVT_NodePool::mAllCount=0;
//#endif
SVT_NodePool::SVT_NodePool(const SVT_NetworkInterface &iface)
    :mStarted(false)
    ,mFlags(0)
    //    ,mCore(core)
    //    ,mSubscribedList(core->mSubscribedList)
    //    ,mCoreMutex(core->mCoreMutex)
    //    ,mCoreThreadPool(core->mCoreThreadPool)
    ,mFilterForNetscan(new SVT_FilterDemo(SVT_FilterDemo::FDT_NETSCAN))
    ,mFilterForNetbios(new SVT_FilterDemo(SVT_FilterDemo::FDT_NETBIOS))
    ,mFilterForAfp(new SVT_FilterDemo(SVT_FilterDemo::FDT_AFP))
    ,mFilterForNetgearSpecific(new SVT_FilterDemo(SVT_FilterDemo::FDT_NETGEARSPECIFIC))
    ,mBasicFactory_Netscan(new SVT_AllNetFactory(this))
    ,mSubscribedFactory_Netscan(new SVT_SubscribedFactory(this,mFilterForNetscan))
    ,mSubscribedFactory_Netbios(new SVT_SubscribedFactory(this,mFilterForNetbios))
    ,mAllResultsFactory_Netbios(new SVT_AllResultsFactory(this,mFilterForNetbios))
    ,mSubscribedFactory_Afp(new SVT_SubscribedFactory(this,mFilterForAfp))
    ,mAllResultsFactory_Afp(new SVT_AllResultsFactory(this,mFilterForAfp))
#ifdef USE_RDP	
    ,mFilterForRdp(new SVT_FilterDemo(SVT_FilterDemo::FDT_RDP))
    ,mSubscribedFactory_Rdp(new SVT_SubscribedFactory(this,mFilterForRdp))
    ,mAllResultsFactory_Rdp(new SVT_AllResultsFactory(this,mFilterForRdp))
#endif
#ifdef USE_PUTTY
    ,mFilterForPutty(new SVT_FilterDemo(SVT_FilterDemo::FDT_PUTTY))
    ,mSubscribedFactory_Putty(new SVT_SubscribedFactory(this,mFilterForPutty))
    ,mAllResultsFactory_Putty(new SVT_AllResultsFactory(this,mFilterForPutty))
#endif
    //,mSubscribedFactory_NetgearSpecific(new SVT_SubscribedFactory(this,mFilterForNetgearSpecific))
    ,mAllResultsFactory_NetgearSpecific(new SVT_AllResultsFactory(this,mFilterForNetgearSpecific))
    ,mInterface(iface)
    ,mPoolExit(true)
{
//    mThreadPool.setMaxThreadCount(100);
    mAllResultsFactory_NetgearSpecific->setWaitTimeWhenNoNode(0);
#ifdef Q_OS_MACX
    mSubscribedFactory_Netscan->setWaitTimeWhenNoNode(0);
#endif
//#ifdef TEST_VALIDATE
//    mAllCount++;
//#endif

}

SVT_NodePool::~SVT_NodePool()
{
    stop();
    delete mBasicFactory_Netscan;
    delete mSubscribedFactory_Netscan;
    delete mAllResultsFactory_Netbios;
    delete mSubscribedFactory_Netbios;
    delete mAllResultsFactory_Afp;
    delete mSubscribedFactory_Afp;
   // delete mSubscribedFactory_NetgearSpecific;
    delete mAllResultsFactory_NetgearSpecific;
    delete mFilterForNetscan;
    delete mFilterForNetbios;
    delete mFilterForNetgearSpecific;
    delete mFilterForAfp;
//#ifdef TEST_VALIDATE
//    mAllCount--;
//#endif
#ifdef USE_RDP
    delete mFilterForRdp;
    delete mSubscribedFactory_Rdp;
    delete mAllResultsFactory_Rdp;
#endif
#ifdef USE_PUTTY
    delete mFilterForPutty;
    delete mSubscribedFactory_Putty;
    delete mAllResultsFactory_Putty;
#endif
}
void SVT_NodePool::reload(bool loadolddata)
{
    //mCoreMutex must be locked here
    mPoolCount=GET_CORE().getPoolCounter(1);
    _genNoNormalNodes();
    if(loadolddata)
    {
        serialize(true);
    }
    else
    {
        mFlags |= NPF_REFRESH_ALL;
    }
}

void SVT_NodePool::start()
{

//#ifdef TEST_VALIDATE
//    Q_ASSERT(mAllCount==1);
//#endif
    Q_ASSERT(!mStarted);
    mPoolExit=false;

    foreach(SVT_TopoMethodRef methodref,mMethods)
    {
        methodref->start();
    }
    mStarted=true;
}

void SVT_NodePool::stop()
{

    if (mStarted)
    {
        mPoolExit=true;
        foreach(SVT_TopoMethodRef methodref,mMethods)
        {
            methodref->stop();
        }
//        qDebug()<<mThreadPool.activeThreadCount();
  //      GET_CORE().mThreadMother->mThreadPool.waitForDone();
//        mThreadPool.waitForDone();
    }

    mStarted=false;
}

void SVT_NodePool::serialize(bool load)
{
    CORE_LOCK();
    Q_ASSERT(QThread::currentThread()==GET_CORE().mProguard);
    QString localmac;
    SVT_MacAddress routermac;
    _TreeNode<SVT_NodeRef,cmpUseingIp> *ipnode=mIpIndex.pFirst;
    SVT_NodeRef router_node;
    while(ipnode)
    {
        SVT_NodeRef data=ipnode->data;
        if(data->mFlags &SVT_Node::NF_ISHOST)
        {
            localmac=data->mMac.toString();
        }
        if(data->mFlags &SVT_Node::NF_ISGATEWAY)
        {
            router_node=ipnode->data;
            routermac=data->mMac;
        }
        ipnode=ipnode->pRSibling;
    }
    QFile f;
    if(localmac.isEmpty()||!routermac.isValid()||router_node.isNull())
    {
        goto serialize_end;
    }
    localmac=localmac.replace(":","");
    //localmac=QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/NETGEARGenie/"
    localmac=QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/"
    +localmac+".mapsave";
    f.setFileName(localmac);


    if(load)
    {

        if(f.open(QIODevice::ReadOnly))
        {
            int count=0;
            QDataStream in(&f);
            SVT_NodeRef loaded_router(new SVT_Node());
            loaded_router->serialize(in,true);
            if(loaded_router->mMac ==routermac)
            {
                for(int i=0;i<TPT_NUM;i++)
                {
                    router_node->mInfos[i]=loaded_router->mInfos[i];
                }

                while(!in.atEnd())
                {
                    SVT_NodeRef ref(new SVT_Node());
                    ref->serialize(in,true);
                    _TreeNode<SVT_NodeRef,cmpUseingIp> *ipnode=mIpIndex.find(ref);
                    _TreeNode<SVT_NodeRef,cmpUsingMac> *macnode=mMacIndex.find(ref);
                    if(!ipnode &&!macnode)
                    {
                        CHECK_OK(mIpIndex.insert(ref));
                        CHECK_OK(mMacIndex.insert(ref));
                    }

                    count++;
                }
            }
            qDebug()<<"pool loaded "<<count;
        }
    }
    else
    {
        if(f.open(QIODevice::WriteOnly))
        {
            QDataStream out(&f);
            ipnode=mIpIndex.pFirst;
            int count =0;
            router_node->serialize(out,false);
            while(ipnode)
            {
                if(!(ipnode->data->mFlags&(SVT_Node::NF_ISHOST|SVT_Node::NF_ISGATEWAY)))
                {
                    ipnode->data->serialize(out,false);
                    count++;
                }
                ipnode=ipnode->pRSibling;
            }
            qDebug()<<"pool saved "<<count;
    }

    }
serialize_end:
    CORE_UNLOCK();
}

//modify by caiyujun
void SVT_NodePool::nodeDetected(const SVT_TopoMethod *method, const SVT_IpAddress &ip,
                                const SVT_MacAddress &mac, const SVT_NodeInfoRef &info,
                                unsigned long ulIp, unsigned int status, quint32 setflags)
{
//    Q_ASSERT(ip.isValid());
    if(mac.isValid() &&ip.isValid())
    {
        //_nodeDetectedWhenIpMacAllOk(method,ip,mac,info,setflags);
		_nodeDetectedWhenIpMacAllOk(method,ip,mac,info,setflags,ulIp,status);
    }
    else if(!mac.isValid() && !ip.isValid())
    {
        //do nothing
    }
    else
    {
        //_nodeDetectedWhenJustIporMacOk(method,ip,mac,info,setflags);
		_nodeDetectedWhenJustIporMacOk(method,ip,mac,info,setflags,ulIp,status);
    }
}

void SVT_NodePool::_nodeDetectedWhenJustIporMacOk(const SVT_TopoMethod * method,const SVT_IpAddress &ip,
                                             const SVT_MacAddress &mac,const SVT_NodeInfoRef &info
                                             ,quint32 setflags, unsigned long ulIp, unsigned int status)
{
    //just update info
    Q_ASSERT(mac.isValid()||ip.isValid());
    int protocalIdx=method?method->mProtocalIdx:TPT_LOCAL;
    if(protocalIdx < 0)
    {
        return;
    }
    Q_ASSERT(!info.isNull());
    quint32 flagCache=0;
    CORE_LOCK();
    SVT_NodeRef nodeRef_Test(new SVT_Node(ip,mac));
    _TreeNode<SVT_NodeRef,cmpUseingIp> *ipnode=mIpIndex.find(nodeRef_Test);
    _TreeNode<SVT_NodeRef,cmpUsingMac> *macnode=mMacIndex.find(nodeRef_Test);

    if(ipnode || macnode)
    {
        SVT_NodeRef nodeRef_Ip=ipnode?ipnode->data:macnode->data;
        if( ! nodeRef_Ip->mInfos[protocalIdx].sameContent(info))
        {
            bool bMerged=mergeMap(nodeRef_Ip->mInfos[protocalIdx],info);
            //            nodeRef_Ip->mInfos[protocalidx]=info;
            if(bMerged && nodeRef_Ip->mFlags & SVT_Node::NF_SUBSCRIBED)
            {
#ifdef TEST_VALIDATE
                Q_ASSERT(GET_CORE().mSubscribedList.contains(nodeRef_Ip)
                         ||GET_CORE().mSubscribedHost==nodeRef_Ip
                         ||GET_CORE().mSubscribedRouter==nodeRef_Ip);
#endif
                nodeRef_Ip->mFlags |= SVT_Node::NF_CHANGED_AFTER_SUBSCRIB;
                flagCache |=SVT_ProgardThread::PF_NEED_NOTIFY_SUBSCRIB_CHANGE;

				//add by caiyujun
				const char* keypatterns =  "^wn[0-9]{0,4}(rpt|rp).*";
				QRegExp regExp(keypatterns,Qt::CaseInsensitive);
				if (protocalIdx&& TPT_NETGEARSPECIFIC == protocalIdx)
				{
					if (!(nodeRef_Ip->mFlags & SVT_Node::NF_ISEXTENDER))
					{
						if (regExp.exactMatch(info->value(NIE_CURRENTSETTING_MODEL)))
						{
							nodeRef_Ip->mFlags |= SVT_Node::NF_ISEXTENDER;
							GET_CORE().setSubscribedExtender(nodeRef_Ip);
						}
					}
				}

				//add end
            }
        }
        nodeRef_Ip->mFlags |= setflags;
    }
    if(GET_CORE().mPool.asPointer()!=this)
    {
        flagCache=0;
    }
    CORE_UNLOCK();
    if(flagCache )
    {
        GET_PROGUARD()->setFlag(flagCache);
    }
}
//modify end

bool SVT_NodePool::_initNodeFlags(SVT_NodeRef nodeRef)
{
    //mCoreUtils must locked here
    Q_ASSERT(!nodeRef.isNull());
    if(nodeRef->mIp==mInterface.getIp())
    {
        nodeRef->mFlags |= SVT_Node::NF_ISHOST;
        GET_CORE().setSubscribedHost(nodeRef);
    }

	if ("0.0.0.0" == mInterface.getGateway().toString())
	{
		return false;
	}
	if(nodeRef->mIp==mInterface.getGateway())
	{
		nodeRef->mFlags |= SVT_Node::NF_ISGATEWAY;
		if(mInterface.isDefault())
		{
			nodeRef->mFlags |= SVT_Node::NF_ISDEFAULTGATEWAY;
		}
		GET_CORE().setSubscribedRouter(nodeRef);
	}
	
    if(!mInterface.isWireless())
    {
        nodeRef->mFlags |= SVT_Node::NF_ISWIRED;
    }
    const unsigned char *macaddress=nodeRef->mMac.asBytes();

    if((macaddress[0]=='\x00') && (macaddress[1]=='\x0c')&& (macaddress[2]=='\x29'))
    {
        nodeRef->mFlags |= SVT_Node::NF_ISVIRTUALMACHINE;
    }
	return true;
}

void SVT_NodePool::_genNoNormalNodes()
{
    qDebug()<<"SVT_NodePool::_genNoNormalNodes()";
    CORE_LOCK();
    SVT_NodeInfoRef nodeinfoRef(new QMap<int,QString>);

    char stemp[128];
#ifdef Q_OS_WIN32
	WSAData   wsaData;
	WSAStartup(MAKEWORD(2,2),   &wsaData); //!!! <-你忘了用这个 
#endif
    if(0==gethostname(stemp,sizeof(stemp)))
    {
        stemp[sizeof(stemp)-1]='\0';
        nodeinfoRef->insert(NIE_LOCALNAME,QString(stemp));
    }
    nodeinfoRef->insert(NIE_LOCALPOOLNUMBER,QString::number(mPoolCount));
    nodeinfoRef->insert(NIE_LOCAL_NICIDX,QString::number(mInterface.m_idx));
#ifdef Q_OS_WIN32
    WSACleanup();
#endif
    nodeinfoRef->insert(NIE_LOCALOSVERSION,CORE_UTIL().getOSVersion());
    nodeDetected(NULL,mInterface.getIp(),mInterface.getMac(),nodeinfoRef);
    bool btemp;
    quint32 srcip=mInterface.getIp().asLong();
    SVT_MacAddress mac=CORE_UTIL().netscanGetMacForRouter(mInterface.getGateway(),srcip,&btemp);

    if(btemp)
    {
        nodeDetected(NULL,mInterface.getGateway(),mac,SVT_NodeInfoRef(NULL));
    }
    CORE_UNLOCK();


}

void SVT_NodePool::_nodeDetectedWhenIpMacAllOk(const SVT_TopoMethod * method,const SVT_IpAddress &ip,
                                               const SVT_MacAddress &mac,const SVT_NodeInfoRef &info,
                                               quint32 setflags, unsigned long ulIp, unsigned int status)											
{
    // kai.yan connect extender local new device refresh
//    if(ip==mInterface.getIp())
//    {
//        if(!(mac==mInterface.getMac()))
//        {
//            return;
//        }
//    }//end
    int protocalIdx=method?method->mProtocalIdx:TPT_LOCAL;
    SVT_NodeRef nodeRef_Test(new SVT_Node(ip,mac));
    if(protocalIdx >=0)
    {
        nodeRef_Test->mInfos[protocalIdx]=info;
    }
    quint32 flagCache=0;
    CORE_LOCK();


    _TreeNode<SVT_NodeRef,cmpUseingIp> *ipnode=mIpIndex.find(nodeRef_Test);
    _TreeNode<SVT_NodeRef,cmpUsingMac> *macnode=mMacIndex.find(nodeRef_Test);

    SVT_NodeRef nodeRef_Ip=ipnode?ipnode->data:SVT_NodeRef(NULL);
    SVT_NodeRef nodeRef_Mac=macnode?macnode->data:SVT_NodeRef(NULL);

	//modify by caiyujun
    //if(!nodeRef_Ip.isNull() && !nodeRef_Mac.isNull() && nodeRef_Ip==nodeRef_Mac)
	if((!nodeRef_Ip.isNull() && !nodeRef_Mac.isNull() && nodeRef_Ip==nodeRef_Mac) || (!nodeRef_Ip.isNull() && TPT_NETGEARSPECIFIC == protocalIdx))
	//modify end
    {
        //可以直接更新节点
        bool protocalinfoChanged=(protocalIdx>=0 )
                &&(!nodeRef_Ip->mInfos[protocalIdx].sameContent(info));
        bool reOnline=(nodeRef_Ip->mFlags&SVT_Node::NF_OFFLINE);
        bool bMerged=false;
        if(protocalinfoChanged )
        {
            Q_ASSERT(protocalIdx>=0);
			if ((TPT_NETGEARSPECIFIC == protocalIdx) 
				&& (EXTENDER_STARTEND == status || EXTENDER_START == status))
			{
				_TreeNode<SVT_NodeRef,cmpUseingIp> *pNode;
				pNode=mIpIndex.pFirst;
				int allcount = mIpIndex.count();
				for (int i=0;i<allcount;i++)
				{
					pNode->data->mParentIp = 0;
					pNode->data->mFlags &= ~SVT_Node::NF_ISEXTENDER_ATTATCHDEVICE;
					pNode=pNode->pRSibling;
				}
			}
            //            nodeRef_Ip->mInfos[method->mProtocalIdx]=info;
            bMerged=mergeMap(nodeRef_Ip->mInfos[protocalIdx],info);
        }
        //add by caiyujun
        const char* keypatterns =  "^wn[0-9]{0,4}(rpt|rp).*";
        QRegExp regExp(keypatterns,Qt::CaseInsensitive);
        if (protocalinfoChanged&& TPT_NETGEARSPECIFIC == protocalIdx)
        {
            if (!(nodeRef_Ip->mFlags & SVT_Node::NF_ISEXTENDER))
            {
                if (regExp.exactMatch(info->value(NIE_ATTACHDEVICE_NAME)))
                {
                    nodeRef_Ip->mFlags |= SVT_Node::NF_ISEXTENDER;
                    GET_CORE().setSubscribedExtender(nodeRef_Ip);
					flagCache |= SVT_ProgardThread::PF_NEED_NOTIFY_SUBSCRIB_CHANGE;
                }
            }

			if (!(nodeRef_Ip->mFlags & SVT_Node::NF_ISEXTENDER_ATTATCHDEVICE))
            {
                if ("y" == info->value(NIE_ISEXTENDER_ATTATCHDEVICE))
                {
                    nodeRef_Ip->mFlags |= SVT_Node::NF_ISEXTENDER_ATTATCHDEVICE;
					nodeRef_Ip->mParentIp = ulIp;
					if (!(GET_PROGUARD()->mProgardFlags & SVT_ProgardThread::PF_NEED_REGEN_EXTENDER_LIST))
					{
						flagCache |= SVT_ProgardThread::PF_NEED_REGEN_SUBSCRIB_LIST;
						flagCache |= SVT_ProgardThread::PF_NEED_NOTIFY_SUBSCRIB_CHANGE;
					}
                }
            }

        }
        //add end
        if(reOnline)
        {
            nodeRef_Ip->mFlags &=~SVT_Node::NF_OFFLINE;
        }
        if((nodeRef_Ip->mFlags & SVT_Node::NF_SUBSCRIBED)&&((protocalinfoChanged&&bMerged) || reOnline))
        {
#ifdef TEST_VALIDATE
         Q_ASSERT(GET_CORE().mSubscribedList.contains(nodeRef_Ip)
             ||GET_CORE().mSubscribedHost==nodeRef_Ip
             ||GET_CORE().mSubscribedRouter==nodeRef_Ip);
#endif
            nodeRef_Ip->mFlags |= SVT_Node::NF_CHANGED_AFTER_SUBSCRIB;
            flagCache |=SVT_ProgardThread::PF_NEED_NOTIFY_SUBSCRIB_CHANGE;
        }
		//add by caiyujun
	/*	if (TPT_NETGEARSPECIFIC == protocalIdx && bMerged && (GET_PROGUARD()->mProgardFlags & SVT_ProgardThread::PF_NEED_REGEN_EXTENDER_LIST))
		{
			flagCache |=SVT_ProgardThread::PF_NEED_REGEN_SUBSCRIB_LIST;
		}*/

		if(protocalinfoChanged )
		{
			if (TPT_NETGEARSPECIFIC == protocalIdx	
				&& EXTENDER_END == status  
				&& (GET_PROGUARD()->mProgardFlags & SVT_ProgardThread::PF_NEED_REGEN_EXTENDER_LIST))
			{
				flagCache |=SVT_ProgardThread::PF_NEED_REGEN_SUBSCRIB_LIST;
			}
		}
		//add end
        nodeRef_Ip->mFlags |=setflags;

    }
    else
    {
        //step1 delete old nodes

        // kai.yan
    /*    if (TPT_NETGEARSPECIFIC == protocalIdx && !nodeRef_Ip.isNull() && nodeRef_Mac.isNull())
        {
            CORE_UNLOCK();
            return ;
		}*////end
        if(!nodeRef_Ip.isNull())
        {
            if(nodeRef_Ip->mFlags & SVT_Node::NF_SUBSCRIBED)
            {
                nodeRef_Ip->mFlags |= SVT_Node::NF_DELETE_AFTER_SUBSCRIB;
            }
            CHECK_OK(mIpIndex.remove(nodeRef_Ip));
            CHECK_OK(mMacIndex.remove(nodeRef_Ip));
        }

        if(!nodeRef_Mac.isNull())
        {
            if(nodeRef_Mac->mFlags & SVT_Node::NF_SUBSCRIBED)
            {
                nodeRef_Mac->mFlags |= SVT_Node::NF_DELETE_AFTER_SUBSCRIB;
            }
            CHECK_OK(mIpIndex.remove(nodeRef_Mac));
            CHECK_OK(mMacIndex.remove(nodeRef_Mac));
        }

        else
        {
#ifdef NOTIFY_ADDED_DEVICE
            nodeRef_Test->mFlags |= SVT_Node::NF_ISNEW;
            flagCache |= SVT_ProgardThread::PF_NEWDEVICE_ADDED;
#endif
#ifdef REFRESH_ATTACHDEVICE_WHEN_NEW_DEV_DETECTED
            flagCache |= SVT_ProgardThread::PF_REFRESH_ATTACHDEVICE;
#endif

        }



        //step2 insert new node

		//add by caiyujun
		const char* keypatterns =  "^wn[0-9]{0,4}(rpt|rp).*";
		QRegExp regExp(keypatterns,Qt::CaseInsensitive);
		if (protocalIdx&& TPT_NETGEARSPECIFIC == protocalIdx)
		{
			if (!(nodeRef_Test->mFlags & SVT_Node::NF_ISEXTENDER))
			{
				if (regExp.exactMatch(info->value(NIE_ATTACHDEVICE_NAME)))
				{
					nodeRef_Test->mFlags |= SVT_Node::NF_ISEXTENDER;
					GET_CORE().setSubscribedExtender(nodeRef_Test);
				}
			}

			if (!(nodeRef_Test->mFlags & SVT_Node::NF_ISEXTENDER_ATTATCHDEVICE))
            {
                if ("y" == info->value(NIE_ISEXTENDER_ATTATCHDEVICE))
                {
                    nodeRef_Test->mFlags |= SVT_Node::NF_ISEXTENDER_ATTATCHDEVICE;
                }
            }
		}
		//add end

        CHECK_OK(mIpIndex.insert(nodeRef_Test));
        CHECK_OK(mMacIndex.insert(nodeRef_Test));
        if (!_initNodeFlags(nodeRef_Test))
		{
			flagCache |= SVT_ProgardThread::PF_NEED_REGEN_POOL;
		}
        nodeRef_Test->mFlags |=setflags;
        flagCache |= SVT_ProgardThread::PF_NEED_REGEN_SUBSCRIB_LIST;
    }
    if(ip==mInterface.getIp())
    {
        if(!(mac==mInterface.getMac()))
        {
            //flagCache |= SVT_ProgardThread::PF_NEED_REGEN_POOL;///kai.yan connect ext always refresh
        }
    }
    if(GET_CORE().mPool.asPointer()!=this)
    {
        flagCache=0;
    }

    CORE_UNLOCK();

    //put this function behind the mCoreMutex.unlock();
    if(flagCache )
    {
        GET_PROGUARD()->setFlag(flagCache);
    }
}

void SVT_NodePool::setNodeOffLine(const SVT_MacAddress & mac)
{
    Q_ASSERT(0);
    CORE_LOCK();
    SVT_NodeRef testnode(new SVT_Node);
    testnode->mMac=mac;

    _TreeNode<SVT_NodeRef,cmpUsingMac> *pnode=mMacIndex.find(testnode);
    if(pnode)
    {
        pnode->data->mFlags|=SVT_Node::NF_OFFLINE;
    }
    CORE_UNLOCK();
}

void SVT_NodePool::setNodeOffLine(const SVT_IpAddress & ip)
{
    quint32 flagCache=0;
#ifdef RETRY_BEFORE_OFFLINE
    bool btemp=false;
    for(int i=0;i<NETSCAN_OFFLINE_RETRYTIMES&&!btemp &&!mPoolExit;i++)
    {
        CORE_UTIL().netscanGetMacForIp(ip,mInterface.getIp(),&btemp);
        SLEEP(100);
    }
    if(btemp)
    {
        return;
    }
#endif
    CORE_LOCK();
    SVT_NodeRef testnode(new SVT_Node);
    testnode->mIp=ip;

    _TreeNode<SVT_NodeRef,cmpUseingIp> *pnode=mIpIndex.find(testnode);
    if(pnode)
    {
        SVT_NodeRef nodeRef_Ip=pnode->data;
        if((nodeRef_Ip->mFlags & SVT_Node::NF_SUBSCRIBED) &&!(nodeRef_Ip->mFlags &SVT_Node::NF_OFFLINE))
        {
            nodeRef_Ip->mFlags|=SVT_Node::NF_OFFLINE;
            flagCache|= SVT_ProgardThread::PF_NEED_NOTIFY_SUBSCRIB_CHANGE;
        }
    }
    if(ip==mInterface.getIp())
    {
        flagCache |= SVT_ProgardThread::PF_NEED_REGEN_POOL;
    }
    if(GET_CORE().mPool.asPointer()!=this)
    {
        flagCache=0;
    }
    CORE_UNLOCK();
    if(flagCache)
    {
        GET_PROGUARD()->setFlag(flagCache);
    }


}


void SVT_NodePool::addMethod(const SVT_TopoMethodRef &methodref)
{
    Q_ASSERT(!mStarted);
    methodref->mNodePool=this;
    mMethods.append(methodref);
}


SVT_Core::SVT_Core()
    :mCoreExit(false)
    ,mProguard(new SVT_ProgardThread(this))
    ,mThreadMother(new SVT_ThreadMother(this))
    ,mControlInterface(NULL)
    ,mCoreMutex(QMutex::Recursive)
    //    ,mSubScribedPool(NULL)
    ,mCoreUtils(this)
//    ,mNetgearInfoThread(new SVT_NetgearInfoThread)
    ,mPoolCounter(0)
{

    //QString sdir=QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/NETGEARGenie/";
    QString sdir=QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/";
    QDir tempdir=QDir(sdir);
    if (!tempdir.exists())
    {
        qDebug("can not find the plugin path");
        return;
    }
    foreach (QString fileName, tempdir.entryList(QStringList()<<"*.mapsave",QDir::Files))
    {
        tempdir.remove(fileName);
    }



}
SVT_Core::~SVT_Core()
{
    stop();
//    delete mNetgearInfoThread;
    delete mThreadMother;
    delete mProguard;
}


void SVT_Core::start()
{
    stop();
    //    Q_ASSERT(mCoreThreadPool.activeThreadCount()==0);
    mCoreExit=false;
    mProguard->start();
    mThreadMother->start();
}

void SVT_Core::stop()
{
    mCoreExit=true;
//    mNetgearInfoThread->wait();
    mThreadMother->stop();
    mProguard->stop();
    mProguard->wait();
    mThreadMother->wait();
}

void SVT_Core::clearSubscribList()
{
    CORE_LOCK();
    _ListNode<SVT_NodeRef> *pLoop=mSubscribedList.pFirst;
    while(pLoop)
    {
        if(!pLoop->data.isNull())
        {
            pLoop->data->mFlags &= ~(SVT_Node::NF_CHANGED_AFTER_SUBSCRIB|SVT_Node::NF_DELETE_AFTER_SUBSCRIB
                                 |SVT_Node::NF_SUBSCRIBED);
        }
        pLoop=pLoop->pNext;
    }
    mSubscribedList.clear();

    GET_CORE().setSubscribedHost(SVT_NodeRef(NULL));
    GET_CORE().setSubscribedRouter(SVT_NodeRef(NULL));
    CORE_UNLOCK();
}

void SVT_Core::setSubscribedHost(const SVT_NodeRef &ref)
{
    CORE_LOCK();
    if(!mSubscribedHost.isNull() && !mSubscribedList.contains(mSubscribedHost))
    {
        mSubscribedHost->mFlags &= ~SVT_Node::NF_SUBSCRIBED;
    }
    if(!ref.isNull())
    {
        ref->mFlags |=SVT_Node::NF_SUBSCRIBED;
    }
    mSubscribedHost=ref;
    CORE_UNLOCK();
}

void SVT_Core::setSubscribedRouter(const SVT_NodeRef &ref)
{
    CORE_LOCK();
    if(!mSubscribedRouter.isNull() && !mSubscribedList.contains(mSubscribedRouter))
    {
        mSubscribedRouter->mFlags &= ~SVT_Node::NF_SUBSCRIBED;
    }
    if(!ref.isNull())
    {
        ref->mFlags |=SVT_Node::NF_SUBSCRIBED;
    }
    mSubscribedRouter=ref;
    CORE_UNLOCK();

}

//add by caiyujun
void SVT_Core::setSubscribedExtender(const SVT_NodeRef &ref)
{
    CORE_LOCK();
	_ListNode<SVT_NodeRef> *pLoop=mExtenderList.pFirst;
	while(pLoop)
	{	
		if(!pLoop->data.isNull())
		{
			QString strIp = pLoop->data->mIp.toString();
			QString strCurrentIp = ref->mIp.toString();
			unsigned long extenderIp = inet_addr(strIp.toStdString().c_str());
			unsigned long currentIp = inet_addr(strCurrentIp.toStdString().c_str());
			if (extenderIp == currentIp)
			{		
				CORE_UNLOCK();
				return;
			}
		}
		pLoop = pLoop->pNext;
	}

	mExtenderList.append(ref);
    CORE_UNLOCK();
}


//add end
