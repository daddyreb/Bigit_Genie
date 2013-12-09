#ifndef SVTCORE_H
#define SVTCORE_H

class SVT_NodePool;
class SVT_Core;
class SVT_ControlProxy;
class SVT_AllNetFactory;
class SVT_SubscribedFactory;
class SVT_AllResultsFactory;
class SVT_ProgardThread;
class SVT_NodeFilterForFactory;
//class SVT_NetgearInfoThread;
class SVT_ThreadMother;
#include <QDataStream>


typedef SVT_Reference<QMap<int,QString> > SVT_NodeInfoRef;
typedef QList<QMap<int,QString> > SVT_NotifyType;




bool mergeMap(QMap<int,QString> &dest,const QMap<int,QString> &src);
bool mergeMap(SVT_NodeInfoRef &dest,const SVT_NodeInfoRef &src);
QList<QMap<int,QString> >::Iterator searchNotifyData(SVT_NotifyType &data,const int &iface,int &idx);

class SVT_ProtocalSpecificData
{
public:
    SVT_ProtocalSpecificData();
    QDateTime mLastUpdate;
    int mRetryTimes;
};

class SVT_Node {

public:
    enum NODE_FLAGS
    {	
        NF_SUBSCRIBED=1<<0,
        NF_CHANGED_AFTER_SUBSCRIB=1<<1,
        NF_DELETE_AFTER_SUBSCRIB=1<<2,
        NF_OFFLINE=1<<3,
        NF_ISHOST=1<<4,
        NF_ISWIRED=1<<5,
        NF_ISGATEWAY=1<<6,
        NF_ISDEFAULTGATEWAY=1<<7,
        NF_ISVIRTUALMACHINE=1<<8,
        NF_ISNEW=1<<9,
		//add by caiyujun
		NF_ISEXTENDER=1<<10,
		NF_ISEXTENDER_ATTATCHDEVICE=1<<11
		//modify end
    };

    SVT_Node()
        :mFlags(0)
		//add by caiyujun
		,mParentIp(0)
		//add end
    {
        for(int i=0;i<TPT_NUM;i++)
            mInfos[i]=NULL;
    };
    SVT_Node(const SVT_IpAddress &ip,const SVT_MacAddress &mac)
        :mIp(ip),mMac(mac),mFlags(0),mParentIp(0)
    {
        for(int i=0;i<TPT_NUM;i++)
            mInfos[i]=NULL;
    };

    void serialize(QDataStream &stream,bool load)
    {
        if(load)
        {
            QString ip,mac;

            stream>>ip>>mac>>mFlags;
            mIp.Set(ip);
            mMac.SetAddress(mac);
            for(int i=0;i<TPT_NUM;i++)
            {
                SVT_NodeInfoRef ref(new QMap<int,QString>);
                stream>> *(ref.asPointer());
                mInfos[i]=ref;
            }
        }
        else
        {
            quint32 saveflag_filter=NF_OFFLINE|NF_ISHOST|NF_ISWIRED|NF_ISGATEWAY|NF_ISDEFAULTGATEWAY
                                    |NF_ISVIRTUALMACHINE;
            stream<<mIp.toString()<<mMac.toString()<<(mFlags&saveflag_filter);
            for(int i=0;i<TPT_NUM;i++)
            {
                QMap<int,QString> *nodeinfo=mInfos[i].asPointer();
                if(nodeinfo)
                {
                    QMap<int,QString> tosave=*nodeinfo;
                    const int cDontSave[]={
                        NIE_CURRENTSETTING_OK,
//                        NIE_CURRENTSETTING_REGIONTAG,
//                        NIE_CURRENTSETTING_REGION,
                        NIE_ATTACHDEVICE_BLOCKED
                    };
                    for(int i=0;i<sizeof(cDontSave)/sizeof(cDontSave[0]);i++)
                    {
                        tosave.remove(cDontSave[i]);
                    }
                    stream<<tosave;
                }
                else
                {
                    stream<< QMap<int,QString>();
                }
            }
        }
    }

    SVT_ProtocalSpecificData mProtocalSpecificData[TPT_NUM];
    SVT_NodeInfoRef mInfos[TPT_NUM];
    SVT_IpAddress mIp;
    SVT_MacAddress mMac;
	//add by caiyujun
	unsigned long mParentIp;
	//add end
    quint32 mFlags;
} ;
typedef SVT_Reference<SVT_Node> SVT_NodeRef;



class SVT_TopoMethod
{
public:
    SVT_TopoMethod(const qint32 &protocalidx,SVT_NodePool *pool)
         :mProtocalIdx(protocalidx)
         ,mNodePool(pool)
    {};
    virtual ~SVT_TopoMethod(){};

    virtual void start()=0;
    virtual void stop()=0;
    qint32 mProtocalIdx;
    SVT_NodePool *mNodePool;
    friend class SVT_NodePool;
};
typedef SVT_Reference<SVT_TopoMethod> SVT_TopoMethodRef;




class cmpUseingIp{
public:
    cmpUseingIp(){};
    int operator()(SVT_NodeRef a,SVT_NodeRef b)
    {
        return a.asPointer()->mIp.asLong()-b.asPointer()->mIp.asLong();
    };
};
class cmpUsingMac
{
public:
    cmpUsingMac(){};
    int operator()(SVT_NodeRef a,SVT_NodeRef b)
    {
        int ret=0;
        for(int i=0;i<6;i++)
        {
            ret=a.asPointer()->mMac.asBytes()[i]-b.asPointer()->mMac.asBytes()[i];
            if(0!=ret)
            {
                return ret;
            }
        }
        return 0;
    };
};


class SVT_NodePool{
public:
    enum NODE_POOL_FLAGS
    {
        NPF_SUBSCRIBED=1<<0,
        NPF_ON=1<<1,
        NPF_REFRESH_ALL=1<<2
    };

    SVT_NodePool(const SVT_NetworkInterface &iface);
    ~SVT_NodePool();
//    QThreadPool mThreadPool;

    void start();
    void reload(bool loadolddata);
    void stop();
    void serialize(bool load);
	//modify by caiyujun
    /*void nodeDetected(const SVT_TopoMethod * method,const SVT_IpAddress &ip,
                        const SVT_MacAddress &mac,const SVT_NodeInfoRef &info,
                        quint32 setflags=0);
    void _nodeDetectedWhenIpMacAllOk(const SVT_TopoMethod * method,const SVT_IpAddress &ip,
                                     const SVT_MacAddress &mac,const SVT_NodeInfoRef &info,
                                     quint32 setflags);
    void _nodeDetectedWhenJustIporMacOk(const SVT_TopoMethod * method,const SVT_IpAddress &ip,
                                     const SVT_MacAddress &mac,const SVT_NodeInfoRef &info,
                                   quint32 setflags);*/

	void nodeDetected(const SVT_TopoMethod * method,const SVT_IpAddress &ip,
                        const SVT_MacAddress &mac,const SVT_NodeInfoRef &info,
                        unsigned long ulIp = 0, unsigned int status = 0, quint32 setflags=0);
    void _nodeDetectedWhenIpMacAllOk(const SVT_TopoMethod * method,const SVT_IpAddress &ip,
                                     const SVT_MacAddress &mac,const SVT_NodeInfoRef &info,
                                     quint32 setflags, unsigned long ulIp, unsigned int status);
    void _nodeDetectedWhenJustIporMacOk(const SVT_TopoMethod * method,const SVT_IpAddress &ip,
                                     const SVT_MacAddress &mac,const SVT_NodeInfoRef &info,
									quint32 setflags, unsigned long ulIp, unsigned int status);



	//modify end
    bool _initNodeFlags(SVT_NodeRef nodeRef);
    void _genNoNormalNodes();

    void setNodeOffLine(const SVT_MacAddress & mac);
    void setNodeOffLine(const SVT_IpAddress & ip);

//    inline void setNetworkInterface(const SVT_NetworkInterface & iface)
//    {
//        mInterface=iface;
//    }



    void addMethod(const SVT_TopoMethodRef &methodref);
//    QList<SVT_NodeRef> mNodeList;
    SVT_AvlTree<SVT_NodeRef,cmpUseingIp> mIpIndex;
    SVT_AvlTree<SVT_NodeRef,cmpUsingMac> mMacIndex;
    QList<SVT_TopoMethodRef> mMethods;
    SVT_NetworkInterface mInterface;
//    SVT_List<SVT_NodeRef> &mSubscribedList;
//    QThreadPool &mCoreThreadPool;
//    SVT_Core *mCore;
//    QMutex &mCoreMutex;
    SVT_NodeFilterForFactory *mFilterForNetscan;
    SVT_NodeFilterForFactory *mFilterForNetbios;

    SVT_NodeFilterForFactory *mFilterForAfp;
    SVT_NodeFilterForFactory *mFilterForNetgearSpecific;


    SVT_AllNetFactory     *mBasicFactory_Netscan;
    SVT_SubscribedFactory *mSubscribedFactory_Netscan;

    SVT_SubscribedFactory *mSubscribedFactory_Netbios;
    SVT_AllResultsFactory *mAllResultsFactory_Netbios;

    SVT_SubscribedFactory *mSubscribedFactory_Afp;
    SVT_AllResultsFactory *mAllResultsFactory_Afp;
#ifdef USE_RDP
    SVT_NodeFilterForFactory *mFilterForRdp;
    SVT_SubscribedFactory *mSubscribedFactory_Rdp;
    SVT_AllResultsFactory *mAllResultsFactory_Rdp;
#endif
#ifdef USE_PUTTY
    SVT_NodeFilterForFactory *mFilterForPutty;
    SVT_SubscribedFactory *mSubscribedFactory_Putty;
    SVT_AllResultsFactory *mAllResultsFactory_Putty;
#endif

  //  SVT_SubscribedFactory *mSubscribedFactory_NetgearSpecific;
    SVT_AllResultsFactory *mAllResultsFactory_NetgearSpecific;
//#ifdef TEST_VALIDATE
//    static int mAllCount;
//#endif


    bool mStarted;
    quint32 mFlags;
    bool mPoolExit;
    int mPoolCount;

};
typedef SVT_Reference<SVT_NodePool> SVT_NodePoolRef;



class SVT_Core:public Singleton<SVT_Core>
{
public:
    SVT_Core();
    ~SVT_Core();
    void start();
    void stop();
    void clearSubscribList();
    void setSubscribedHost(const SVT_NodeRef &ref);
    void setSubscribedRouter(const SVT_NodeRef &ref);
	//add by caiyujun
    void setSubscribedExtender(const SVT_NodeRef &ref);
	//add end
    SVT_NodePoolRef mPool;
//    SVT_NodePoolRef mLazyDeletePool;
    SVT_List<SVT_NodeRef> mSubscribedList;
    SVT_NodeRef mSubscribedHost;
    SVT_NodeRef mSubscribedRouter;
	//add by caiyujun
    SVT_List<SVT_NodeRef> mExtenderList;
	SVT_NodeRef mExtender;
	//add end
    SVT_ProgardThread *mProguard;
    SVT_ThreadMother *mThreadMother;
    SVT_ControlProxy *mControlInterface;
    QMutex mCoreMutex;
//    SVT_NodePool * mSubScribedPool;
    SVT_QtUtils mCoreUtils;
//    SVT_NetgearInfoThread *mNetgearInfoThread;
    bool mCoreExit;

    int mPoolCounter;
    QMutex mPoolCounterMutex;
    inline int getPoolCounter(int change=0)
    {
        int ret;
        mPoolCounterMutex.lock();
        mPoolCounter+=change;
        ret=mPoolCounter;
        mPoolCounterMutex.unlock();
        return ret;
    }

};

#endif // SVTCORE_H
