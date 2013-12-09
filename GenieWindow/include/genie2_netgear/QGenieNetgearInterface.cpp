#include "QGenieNetgearInterface.h"
//#include "INetgearInterface.h"
//#include "QGenieNetgearInterface_OldSoap.h"
#include "QGenieNetgearInfoThread.h"
#include "QGenieSmartnetworkThread.h"
//#include "QGenieNetgearInterface_todelete.h"
#include "SvtNetworkTool.h"

#ifdef GENIE2_MAINFRAME
#include "genie2_interface.h"
#endif

//static int sNodeIdx;
_SoapNode::_SoapNode(QGenieNetgearInterface *manager)
    :mManager(manager)
{
    mManager->mMutex.lock();
    session=-1;
    Q_ASSERT(QThread::currentThread()==qApp->thread());

//    otherData=NULL;
//    otherData2=NULL;
    receiver=NULL;
    flag=0;
    cmd=-1;
    mManager->mNodeList.append(this);
    mManager->mMutex.unlock();
}

_SoapNode::~_SoapNode()
{
    mManager->mMutex.lock();
    Q_ASSERT(mManager && mManager->mNodeList.contains(this));
    Q_ASSERT(QThread::currentThread()==qApp->thread());
    mManager->mNodeList.removeOne(this);
    mManager->mMutex.unlock();
}

QGenieNetgearInterface::QGenieNetgearInterface(QObject *parent,bool dbg_mode)
    :QObject(parent)
//    ,mIface(NULL)
    ,mMutex(QMutex::Recursive)
    ,mExitFlag(false)
    ,mInterfaceFlags(0)
{
    if(dbg_mode)
    {
        mInterfaceFlags |= NIF_IN_DEBUG_MODE;
    }
    mThreads.resize(TT_NUM);
    mThreads[TT_LOCAL]=new QGenieNetgearInfoThread(this);
    mThreads[TT_REMOTE]=new QGenieSmartnetworkThread(this);
    for(int i=0;i<TT_NUM;i++)
    {

        connect(mThreads[i],SIGNAL(signal_DataReturn(_SoapNode*,QStringList))
                ,this,SLOT(slot_ThreadDataReturned(_SoapNode*,QStringList)));
    }


//    mIface=new QGenieNetgearInterface_todelete(this);
    QGenieSmartnetworkThread *sm_thr=qobject_cast<QGenieSmartnetworkThread *>
                                     (mThreads[TT_REMOTE]);
    Q_ASSERT(sm_thr);
    mFcml=&sm_thr->mFcml;
    for(int i=0;i<TT_NUM;i++)
    {
        mThreads[i]->start();
    }

}

QGenieNetgearInterface::~QGenieNetgearInterface()
{
    mExitFlag=true;
    for(int i=0;i<TT_NUM;i++)
    {
        mThreads[i]->mSemaphore.release();
        mThreads[i]->wait();
        delete mThreads[i];
    }
    mMutex.lock();
    Q_ASSERT(QThread::currentThread()==qApp->thread());

    foreach(_SoapNode *node,mNodeList)
    {
        Q_ASSERT(node);
        delete node;
    }
    mMutex.unlock();
}

int QGenieNetgearInterface::netgearCommand(IFACE_COMMAND cmd, const QStringList &cmd_paras, QObject *receiver, const char *slot)
{
    qDebug()<<"QGenieNetgearInterface::netgearCommand"<<cmd<<cmd_paras<<receiver;
    mMutex.lock();
    _SoapNode *node=new _SoapNode(this);
    node->cmd=cmd;
    node->paralist=cmd_paras;
    node->receiver=receiver;
    if(receiver)
    {
        connect(receiver,SIGNAL(destroyed(QObject*)),this,SLOT(slot_ReceiverDeleted(QObject*)));
    }
    node->slot=slot;

//    if(netgearCommand(node)<0)
//    {
//        Q_ASSERT(0);
//        delete node;
//    }
    static int sIdx=0;
    node->session=sIdx++;
    int ret=node->session;
    for(int i=0;i<TT_NUM;i++)
    {
        if(mThreads[i]->canProcess(cmd))
        {
            mThreads[i]->mSemaphore.release(1);
        }
    }

    mMutex.unlock();
    return ret;
}

void QGenieNetgearInterface::setConfiguration(IFACE_SETTINGS cmd, const QStringList &seting_paras)
{
    int length=seting_paras.length();
    if(cmd==IS_SM_CPLOGIN)
    {
        Q_ASSERT(length >=3);
        mFcml->setAuthenData(seting_paras[0],seting_paras[1],seting_paras[2]);
    }
    else if(cmd==IS_SM_REMOVE_SECURE)
    {
        mFcml->removeAllAuthenData();
    }
}

void QGenieNetgearInterface::doReturn(_SoapNode *node)
{
    qDebug()<<"do return "<<node->cmd<<node->slot;
    mMutex.lock();
#ifdef GENIE2_MAINFRAME
    QStringList &slist=node->retlist;
    if(node->cmd==IC_CURRENTSETTING)
    {
        GENIE2_NOTIFY_MESSAGE(SCCMD_CURRENTSETTING,node->retlist);
        QStringList &slist=node->retlist;
        bool btemp;
        const QString &dbg_routemodel=QGenieIface2ShareObject::sSingleInstance->mDBGRouteModel;
        bool requestOk=(0==slist[0].toInt(&btemp)) && btemp;
        if(dbg_routemodel.isEmpty())
        {
            if(slist.count()>1 && requestOk)
            {
                GENIE2_NOTIFY_MESSAGE(SCCMD_ROUTER_MODEL,slist[1]);
            }
            else
            {
                GENIE2_NOTIFY_MESSAGE(SCCMD_ROUTER_MODEL,"");
            }
        }
        else
        {
            GENIE2_NOTIFY_MESSAGE(SCCMD_ROUTER_MODEL,dbg_routemodel);
        }

        if(slist.count()>2 && requestOk)
        {
            GENIE2_NOTIFY_MESSAGE(SCCMD_FIRMWARE,slist[2]);
        }
        else
        {
            GENIE2_NOTIFY_MESSAGE(SCCMD_FIRMWARE,"");
        }

       // qDebug()<<nodeIC_CURRENTSETTING;
    }
    else if(node->cmd==IC_LOGIN)
    {
        QStringList &slist=node->retlist;
        bool btemp;
        qDebug()<<slist;
        const QString &dbg_routemodel=QGenieIface2ShareObject::sSingleInstance->mDBGRouteModel;
        bool noErrorOfCurrentSetting=(-2!=slist[0].toInt(&btemp)) && btemp;
        if(dbg_routemodel.isEmpty() /*&& dbg_routemodel!=""*/)
        {
            if(slist.count()>1 &&noErrorOfCurrentSetting)
            {
                GENIE2_NOTIFY_MESSAGE(SCCMD_ROUTER_MODEL,slist[1]);
            }
            else
            {
                GENIE2_NOTIFY_MESSAGE(SCCMD_ROUTER_MODEL,"");
            }
        }
        else
        {
            GENIE2_NOTIFY_MESSAGE(SCCMD_ROUTER_MODEL,dbg_routemodel);
        }

        if(slist.count()>2 && noErrorOfCurrentSetting)
        {
            GENIE2_NOTIFY_MESSAGE(SCCMD_FIRMWARE,slist[2]);
        }
        else
        {
            GENIE2_NOTIFY_MESSAGE(SCCMD_FIRMWARE,"");
        }
    }
    else if(node->cmd==IC_GET_WIRELESS)
    {
		bool btemp;
        if(slist.count()>5 &&(0==slist[0].toInt(&btemp)) && btemp)
        {
            GENIE2_NOTIFY_MESSAGE(SCCMD_ROUTER_MAC,slist[5]);
        }
    }
#endif

    if(node->slot)
    {
        connect(this,SIGNAL(signal_CommandReturn(int,QStringList)),node->receiver,node->slot);
        emit signal_CommandReturn(node->session,node->retlist);
        disconnect(this,SIGNAL(signal_CommandReturn(int,QStringList)),node->receiver,node->slot);
    }
    delete node;
    mMutex.unlock();
}

void QGenieNetgearInterface::slot_ReceiverDeleted(QObject *obj)
{
    qDebug()<<"QGenieNetgearInterface::slot_ReceiverDeleted";
    Q_ASSERT(QThread::currentThread()==qApp->thread());
    Q_ASSERT(obj);
    mMutex.lock();
    foreach(_SoapNode *node,mNodeList)
    {
        Q_ASSERT(node);
        if(node->receiver==obj)
        {
            delete node;
        }
    }
    mMutex.unlock();
}

void QGenieNetgearInterface::slot_ThreadDataReturned(_SoapNode *node,QStringList lst)
{
//    IThreadInterface *thread=qobject_cast<IThreadInterface *>(sender());
//    Q_ASSERT(thread);
    Q_ASSERT(QThread::currentThread()==qApp->thread());
    Q_ASSERT(node);
    mMutex.lock();
    if(mNodeList.contains(node))
    {
        node->retlist=lst;
    }
    doReturn(node);

    mMutex.unlock();

}
