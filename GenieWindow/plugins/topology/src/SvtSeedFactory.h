#ifndef SVTSEEDFACTORY_H
#define SVTSEEDFACTORY_H
class SVT_NodePool;
class SVT_Node;
class SVT_NodeFilterForFactory
{
public:
    SVT_NodeFilterForFactory();
    virtual bool canAddThisNode(SVT_Node *node)=0;
};

class SVT_FilterDemo:public SVT_NodeFilterForFactory
{
public:
    enum FILTER_DEMO_TYPE
    {
        FDT_NETSCAN,
        FDT_NETBIOS,
#ifdef USE_RDP
        FDT_RDP,
#endif
#ifdef USE_PUTTY
        FDT_PUTTY,
#endif
        FDT_AFP,
        FDT_NETGEARSPECIFIC,

    };

    SVT_FilterDemo(FILTER_DEMO_TYPE type);
    virtual bool canAddThisNode(SVT_Node *node);
protected:
    FILTER_DEMO_TYPE mFilterType;
};

class SVT_SeedFactoryInterface
{
public:
    SVT_SeedFactoryInterface(SVT_NodePool *pool);
    virtual int getSeed(QList<quint32> &retlst)=0;
protected:
    SVT_NodePool *mPool;
    QMutex mMutex;
};



class SVT_SeedPoint{
public:


    SVT_SeedPoint(qint32 data)
    {
        //mProducer=producer;
        mLeftZone=data;
        mRightZone=data+1;
//        mValid=true;
    }

    SVT_SeedPoint(const SVT_SeedPoint &other)
    {
        mLeftZone=other.mLeftZone;
        mRightZone=other.mRightZone;
//        mValid=other.mValid;
    }

    qint32 mLeftZone;
    qint32 mRightZone;
//    SVT_SeedProducer *mProducer;
//    bool mValid;
};
bool operator <(const SVT_SeedPoint &a,const SVT_SeedPoint &b);



class SVT_AllNetFactory
        :public SVT_SeedFactoryInterface
{
public:
    SVT_AllNetFactory(SVT_NodePool *pool);
    void reGenSeeds();
    int getSeed(QList<quint32> &retlst);
    QList<SVT_SeedPoint> mSeedPoints;
    bool mReinitFlag;
    quint32 mSavedPos;
    quint32 mMask;
protected:

#ifdef TEST_VALIDATE
    QSet<quint32> mTestSet;
#endif
    int mCallTimes;
};

class SVT_SubscribedFactory:public SVT_SeedFactoryInterface
{
public :
    SVT_SubscribedFactory(SVT_NodePool *pool,SVT_NodeFilterForFactory*,SVT_SubscribedFactory*otehr=NULL);
    int getSeed(QList<quint32> &retlst);
    inline void setWaitTimeWhenNoNode(int t)
    {
        mWaitTime=t;
    }

    QVector<quint32> mCache;
    quint32 mSavedPos;
    SVT_NodeFilterForFactory *mFilter;
    SVT_SubscribedFactory *mOtherFactory;
protected:
    int mWaitTime;
};

class SVT_AllResultsFactory:public SVT_SeedFactoryInterface
{
public :
    SVT_AllResultsFactory(SVT_NodePool *pool,SVT_NodeFilterForFactory *);
    int getSeed(QList<quint32> &retlst);
    inline void setWaitTimeWhenNoNode(int t)
    {
        mWaitTime=t;
    }
    QVector<quint32> mCache;
    quint32 mSavedPos;
    SVT_NodeFilterForFactory *mFilter;
protected:
    int mWaitTime;
};

#endif // SVTSEEDFACTORY_H
