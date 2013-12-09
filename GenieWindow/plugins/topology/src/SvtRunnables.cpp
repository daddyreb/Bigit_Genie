#include "topology.h"
#include "SvtNetgearSpecificHelper.h"
#include "SvtTracer.h"
/////////////////////////////////////////////////
#include <QObject>

extern quint32 sMapFlags;

SVT_RunnableInterface::SVT_RunnableInterface(SVT_TopoMethod *method)
    :mTopoMethod(method)
{
    setAutoDelete(false);
}

SVT_NodePool *SVT_RunnableInterface::getPool()
{
    return mTopoMethod->mNodePool;
}

////////////////////////////////////////////////////////
SVT_SeedRunnable::SVT_SeedRunnable(SVT_TopoMethod *method,SVT_SeedFactoryInterface *factory,int looptimes)
    :mFactory(factory)
    ,mLoopTimes(looptimes)
    ,SVT_RunnableInterface(method)
{
}

void SVT_SeedRunnable::run()
{
    qDebug()<<"ThreadRuning";
    QList<quint32> iplist;
    bool &bExit=getPool()->mPoolExit;
    int curloop=0;
    while(!bExit && (mLoopTimes <0 || (curloop++ < mLoopTimes)) )
    {
        int n=0;
        while(!bExit && n<=0 )
        {
            SLEEP(100);
            n=mFactory->getSeed(iplist);
        }
        SVT_NetworkInterface &iface=getPool()->mInterface;
        quint32 ipmask=iface.getIp().asLong()& iface.getMask().asLong();
        for(int i=0;i<n;i++)
        {
            if(bExit)
            {
                break;
            }
            processSeed(iplist[i]|ipmask);
        }
    }
}

//////////////////////////////////////////////

SVT_NetScanRunnable::SVT_NetScanRunnable(
    SVT_TopoMethod *method,SVT_SeedFactoryInterface * factory
    ,bool bUseSubscribedSeed
    ,int looptimes)
    :SVT_SeedRunnable(method,factory,looptimes)
    ,mUseSubscribedSeed(bUseSubscribedSeed)
{

}

SVT_NetScanRunnable::~SVT_NetScanRunnable()
{
}

void SVT_NetScanRunnable::whenDataGetted(const SVT_MacAddress &mac, const SVT_IpAddress &ip)
{
    //qDebug()<<ip.toString()<<mac.toString();
    getPool()->nodeDetected(mTopoMethod,ip,mac,SVT_NodeInfoRef(NULL));
}

void SVT_NetScanRunnable::whenDataNotGetted(const SVT_IpAddress &ip)
{
    getPool()->setNodeOffLine(ip);
}

void SVT_NetScanRunnable::processSeed(quint32 ip)
{
    SVT_MacAddress _mac;
    SVT_IpAddress _ip;
    const SVT_IpAddress &_srcip=getPool()->mInterface.getIp();
    _ip.Set(ip);
    bool btemp;
  //  qDebug()<<"SVT_NetScanRunnable::processSeed"<<SVT_IpAddress(ip).toString();
    SVT_QtUtils &util=CORE_UTIL();
    _mac=util.netscanGetMacForIp(_ip,_srcip,&btemp);
    if(sMapFlags & MF_IN_DBG_MOD)
    {
        SVT_Tracer *tracer=SVT_Tracer::GetSingletonPtr();
        if(tracer)
        {
            tracer->whenArp();
        }

    }
    if(btemp)
    {
        whenDataGetted(_mac,_ip);
    }
    else if(mUseSubscribedSeed)
    {
        whenDataNotGetted(_ip);
    }
    /*
//    mLoopTimes >0 的位刚启动时的helper，所以不需要等待
    if(!mUseSubscribedSeed && (mLoopTimes <0))
    {
        for(int i=0;i<5&&!(mTopoMethod->mNodePool->mPoolExit);i++)
        {
            SLEEP(100);
        }
    }
    */
//    return btemp;
}

////////////////////////////////////////////////////////////////
SVT_NetbiosRunnable::SVT_NetbiosRunnable(SVT_TopoMethod *method, SVT_SeedFactoryInterface *factory)
    :SVT_SeedRunnable(method,factory)
{

}

SVT_NetbiosRunnable::~SVT_NetbiosRunnable()
{

}

void SVT_NetbiosRunnable::processSeed(quint32 ip)
{
    SVT_QtUtils &util=CORE_UTIL();

    SVT_IpAddress _ip(ip);
    SVT_NodeInfoRef infoRef(new QMap<int,QString>);

    util.netbiosGetDevInfo(_ip,*(infoRef.asPointer()),&(mTopoMethod->mNodePool->mPoolExit));
    if(!infoRef.isNull())
    {
        SVT_MacAddress nullmac;
        getPool()->nodeDetected(mTopoMethod,_ip,nullmac,infoRef);
    }
}

#ifdef USE_RDP
///////////////////////////////////////////////////////////////
SVT_RdpRunnable::SVT_RdpRunnable(SVT_TopoMethod *method, SVT_SeedFactoryInterface *factory1, SVT_SeedFactoryInterface *factory2)
    :SVT_PortTestRunnable(method,3389,factory1,factory2)
{

}

void SVT_RdpRunnable::whenPortOk(const SVT_IpAddress &ip)
{
    SVT_MacAddress nullmac;
    SVT_NodeInfoRef infoRef(new QMap<int,QString>);
    infoRef->insert(NIE_RDP_ENABLE,"y");
    getPool()->nodeDetected(mTopoMethod,ip,nullmac,infoRef);
}
#endif


SVT_PortTestRunnable::SVT_PortTestRunnable(SVT_TopoMethod *method
                                    ,quint16 port
                                     ,SVT_SeedFactoryInterface *factory1, SVT_SeedFactoryInterface * factory2)
    :SVT_RunnableInterface(method)
    ,mPort(port)
    ,mFactory1(factory1)
    ,mFactory2(factory2)
{
    Q_ASSERT(mFactory1 || mFactory2);
}

SVT_PortTestRunnable::~SVT_PortTestRunnable()
{

}

void SVT_PortTestRunnable::run()
{
    bool &bExit=getPool()->mPoolExit;
    QList<quint32> list_subscribed;
    QList<quint32> list_allnet;
    SVT_IpAddress ips[64];
    int curIdx=0;
    QList<quint32> * const cLists[]={
        &list_subscribed,&list_subscribed,&list_subscribed,&list_allnet,&list_allnet
            };
    SVT_NetworkInterface &iface=getPool()->mInterface;
    quint32 ipmask=iface.getIp().asLong()& iface.getMask().asLong();

    while(!bExit)
    {
        if(list_subscribed.isEmpty() && mFactory1)
        {
            mFactory1->getSeed(list_subscribed);
        }
        if(list_allnet.isEmpty() && mFactory2)
        {
            mFactory2->getSeed(list_allnet);
        }

        while((!list_subscribed.isEmpty() || !list_allnet.isEmpty())&&!bExit)
        {
            int count=0;
            while( (count < sizeof(ips)/sizeof(ips[0]))&&!bExit)
            {
                curIdx=(curIdx +1)%(sizeof(cLists)/sizeof(cLists[0]));
                if(list_subscribed.isEmpty()&&list_allnet.isEmpty())
                {
                    break;
                }
                if(cLists[curIdx]->isEmpty())
                {
                    continue;
                }

                ips[count]=SVT_IpAddress(ipmask|(cLists[curIdx]->value(0)));
                cLists[curIdx]->pop_front();
                count++;

            }
			if(count<=0)
			{
				SLEEP(100);
				continue;
			}

            quint64 retFlags=SVT_TcpSocket::testPort((SVT_IpAddress *)ips,count,mPort,&bExit);
            for(int i=0;i<count;i++)
            {
                if(retFlags &( ((quint64)1)<<i )   )
                {
                    whenPortOk(ips[i]);

                }
            }
        }

    }//while
}
#ifdef USE_PUTTY
SVT_PuttyRunnable::SVT_PuttyRunnable(SVT_TopoMethod *method, SVT_SeedFactoryInterface *facroty1, SVT_SeedFactoryInterface *factory2)
    :SVT_PortTestRunnable(method,22,facroty1,factory2)
{

}

void SVT_PuttyRunnable::whenPortOk(const SVT_IpAddress &ip)
{
    SVT_MacAddress nullmac;
    SVT_NodeInfoRef infoRef(new QMap<int,QString>);
    infoRef->insert(NIE_PUTTY_ENABLE,"y");
    getPool()->nodeDetected(mTopoMethod,ip,nullmac,infoRef);
}

#endif

////////////////////////////////////////////////////////////////
SVT_AfpRunnable::SVT_AfpRunnable(SVT_TopoMethod *method, SVT_SeedFactoryInterface *factory)
    :SVT_SeedRunnable(method,factory)
{

}

SVT_AfpRunnable::~SVT_AfpRunnable()
{
}

void SVT_AfpRunnable::processSeed(quint32 ip)
{
    SVT_QtUtils &util=CORE_UTIL();

    SVT_IpAddress _ip(ip);
    SVT_NodeInfoRef infoRef(new QMap<int,QString>);

    util.afpGetDevInfo(_ip,*(infoRef.asPointer()),&(mTopoMethod->mNodePool->mPoolExit));
    if(!infoRef.isNull())
    {
        SVT_MacAddress nullmac;
        getPool()->nodeDetected(mTopoMethod,_ip,nullmac,infoRef);
    }
}

//////////////////////////////////////////////////////////////////////
SVT_UpnpRunnable::SVT_UpnpRunnable(SVT_TopoMethod *method)
    :SVT_RunnableInterface(method)
{
}

SVT_UpnpRunnable::~SVT_UpnpRunnable()
{

}

void SVT_UpnpRunnable::run()
{
    SVT_UpnpHelper helper(mTopoMethod);
    helper.start();
}

////////////////////////////////////////////////

SVT_NetgearSpecificRunnable::SVT_NetgearSpecificRunnable(SVT_TopoMethod*method,SVT_SeedFactoryInterface *factory)
    :SVT_RunnableInterface(method)
    ,mFactory(factory)
{

}

SVT_NetgearSpecificRunnable::~SVT_NetgearSpecificRunnable()
{

}

void SVT_NetgearSpecificRunnable::run()
{
    SVT_NetgearSpecificHelper helper(mTopoMethod,mFactory);
    helper.run();
}

/////////////////////////////////////////////////////////////////////////////////
#ifdef Q_OS_MACX
SVT_NetScanRunnable_Unix::SVT_NetScanRunnable_Unix(SVT_TopoMethod *method
                         ,SVT_SeedFactoryInterface *factory1,SVT_SeedFactoryInterface *factory2)
                             :SVT_RunnableInterface(method)
                             ,mFactory_Subscribed(factory1)
                             ,mFactory_AllNet(factory2)

{

}

SVT_NetScanRunnable_Unix::~SVT_NetScanRunnable_Unix()
{

}

void SVT_NetScanRunnable_Unix::run()
{
    QMap<quint32,int> retrymap;
    QList<quint32> list_subscribed;
    QList<quint32> list_allnet;
    QList<quint32> current_reqs;
    QList<quint32> list_needretry;
    bool &bExit=getPool()->mPoolExit;
    SVT_NetworkInterface &iface=getPool()->mInterface;
    quint32 ipmask=iface.getIp().asLong()& iface.getMask().asLong();
    quint32 subnetmask=(iface.getMask().asLong());
    int counter=0;
    int itemp=0;
    bool in_start_process=true;
    while(!bExit)
    {
#ifdef TEST_VALIDATE
        Q_ASSERT(retrymap.count()<300);
#endif
        if(in_start_process)
        {
//            printf("mask %d\n",((~subnetmask)>>1));
            in_start_process=(counter < ((~subnetmask)<<1));
        }

        //qDebug()<<"in_start_process"<<in_start_process<<((~subnetmask)>>1)<<counter;
        //step1 process seeds
        if(list_subscribed.isEmpty())
        {
            mFactory_Subscribed->getSeed(list_subscribed);
        }
        if(list_allnet.isEmpty())
        {
            mFactory_AllNet->getSeed(list_allnet);
        }
        current_reqs.clear();
        list_needretry.clear();

        if(!retrymap.isEmpty())
        {
            QList<quint32> keys=retrymap.keys();
            foreach(quint32 retryIp,keys)
            {
                int retrytime=retrymap.value(retryIp);
                if(retrytime > 0)
                {
                    qDebug()<<"retry offline"<<SVT_IpAddress(retryIp).toString()<<retrytime;
                    retrymap.insert(retryIp,retrytime-1);
                    current_reqs.append(retryIp);
                    if(current_reqs.count()>=UNIX_CHECKIP_ONETIME)
                    {
                        break;
                    }
                }
            }
        }


        QList<quint32> * const cLists[]={
            &list_subscribed,&list_subscribed,&list_subscribed,&list_allnet,&list_allnet
                };
        const bool cNeedRetry[]={true,true,true,false,false};

        const int cListCount=sizeof(cLists)/sizeof(cLists[0]);
        int temp_base=in_start_process?2:0;
        int curlistIdx=temp_base;
#ifdef TEST_VALIDATE
        Q_ASSERT(sizeof(cLists)/sizeof(cLists[0])==sizeof(cNeedRetry)/sizeof(cNeedRetry[0]));
        Q_ASSERT(temp_base < cListCount);
#endif


        while(current_reqs.count() <UNIX_CHECKIP_ONETIME
              && !(list_subscribed.isEmpty() && list_allnet.isEmpty()))
        {
            if(!cLists[curlistIdx]->isEmpty())
            {
                quint32 iptemp=cLists[curlistIdx]->front()|ipmask;
                cLists[curlistIdx]->pop_front();
                if(
                        iptemp != getPool()->mInterface.getIp().asLong()
                        && iptemp != getPool()->mInterface.getGateway().asLong()
                        )
                {
                    if(cNeedRetry[curlistIdx])
                    {
                        list_needretry.append(iptemp);
                    }
                    current_reqs.append(iptemp);
                }
            }
            curlistIdx=(curlistIdx+1-temp_base)%(cListCount-temp_base)+temp_base;
        }

        //step2 do command
        if(!current_reqs.isEmpty())
        {
            QList<quint32> offline_ips,online_ips;
            QList<SVT_MacAddress> online_macs;
            bool btemp=CORE_UTIL().netscanCheckIps(current_reqs,getPool()->mInterface.m_idx
                                        ,online_ips,offline_ips,online_macs);
            if(btemp)
            {
                ///////////////////////////

                for(int i=0;i<online_ips.count();i++)
                {
                    if(
                         (   (online_ips[i] & ~subnetmask) !=(0xffffff & ~subnetmask) )
                        &&( (online_ips[i] & ~subnetmask) != 0 )
                        &&( (online_ips[i] & subnetmask)==ipmask)
                        )
                    {
                        getPool()->nodeDetected(mTopoMethod,online_ips[i],online_macs[i],SVT_NodeInfoRef(NULL));
                    }
                }
                ////////////////////////////
                QList<quint32> keys=retrymap.keys();
                foreach(quint32 retryIp,keys)
                {
                    if(online_ips.contains(retryIp))
                    {
#ifdef TEST_VALIDATE
                        Q_ASSERT(!offline_ips.contains(retryIp));
#endif
                        retrymap.remove(retryIp);

                    }
                    else if(retrymap.value(retryIp)==0)
                    {
                        getPool()->setNodeOffLine(retryIp);
                        retrymap.remove(retryIp);
                    }
                }
                ////////////////////////////////
                foreach(quint32 testIp,list_needretry)
                {
                    if(!online_ips.contains(testIp) && !retrymap.contains(testIp))
                    {
                        retrymap.insert(testIp,NETSCAN_OFFLINE_RETRYTIMES);
                    }
                }
                /////////////////////////////////
//                foreach(quint32 testIp,offline_ips)
//                {
//                    if(!retrymap.contains(testIp))
//                    {
//                        retrymap.insert(testIp,NETSCAN_OFFLINE_RETRYTIMES);
//                    }
//                }
            }
            if(in_start_process)
            {
                counter+=(current_reqs.count()-retrymap.count());
            }
//            printf("process seed %d %d\n",current_reqs.count(),itemp++);
        }
        if(in_start_process  )
        {
//            printf("sleep 20\n");
            SLEEP(20);
        }
        else if(!retrymap.isEmpty())
        {
            SLEEP(200);
        }
        else
        {
//            printf("sleep 400 %d\n",itemp++);
            for(int i=0;(i<5)&&!(mTopoMethod->mNodePool->mPoolExit);i++)
            {
                SLEEP(400);
            }
        }
    } //while



}
#endif
