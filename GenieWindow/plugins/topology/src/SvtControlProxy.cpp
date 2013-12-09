#include "topology.h"

//SVT_ControlProxy::SVT_ControlProxy()
//{
//}
#include "SvtNetgearSpecificHelper.h"
#include "SvtTracer.h"

using namespace BLOCKDEVICE_FSM;
extern quint32 sMapFlags;

SVT_ControlProxy::SVT_ControlProxy()
    :mCore(new SVT_Core)
    ,mInnerFSMController(new _InnerFSMController)
    ,mTracer(NULL)
{
    qRegisterMetaType<SVT_NotifyType>("SVT_NotifyType");
    qRegisterMetaType<QVariant>("QVariant");
    qRegisterMetaType<QMap<int,QString> >("QMap<int,QString>");
    mCore->mControlInterface=this;
    if(sMapFlags & MF_IN_DBG_MOD)
    {
        mTracer=new SVT_Tracer();
    }

}

SVT_ControlProxy::~SVT_ControlProxy()
{
    delete mCore;
    delete mInnerFSMController;
    delete mTracer;
}

void SVT_ControlProxy::startControl()
{
    mCore->start();
}

QWidget *SVT_ControlProxy::getDbgUi()
{
    Q_ASSERT(sMapFlags & MF_IN_DBG_MOD);
    Q_ASSERT(mTracer);
    return mTracer->getUi();

}

void SVT_ControlProxy::endControl()
{
    Q_ASSERT(0);
}

int SVT_ControlProxy::refresh()
{
    int ret=GET_CORE().getPoolCounter()+1;
    GET_PROGUARD()->restart();
    return ret;
}

//modify by caiyujun
//int SVT_ControlProxy::subscrib(int adpIdx,int startIdx,int count)
int SVT_ControlProxy::subscrib(int adpIdx,int startIdx,int count,bool bExtendMap,unsigned long extenderIp)
{
    int ret=GET_CORE().getPoolCounter();
    CORE_LOCK();
    if(mCore->mPool.isNull() || mCore->mPool->mInterface.m_idx !=adpIdx)
    {
        ret++;
    }
    CORE_UNLOCK();
    Q_ASSERT(mCore && mCore->mProguard);
    GET_PROGUARD()->mProgardMutex.lock();
    if(GET_PROGUARD()->mSbuscribNetInterfaceIdx!=adpIdx)
    {
        GET_PROGUARD()->mSbuscribNetInterfaceIdx=adpIdx;
        GET_PROGUARD()->mProgardFlags |= SVT_ProgardThread::PF_LOOP_SPEEDUP;
    }
    GET_PROGUARD()->mSubscribStartIdx=startIdx;
    GET_PROGUARD()->mSubscribCount=count;
    GET_PROGUARD()->mProgardFlags |= SVT_ProgardThread::PF_NEED_REGEN_SUBSCRIB_LIST;
	GET_PROGUARD()->mExtenderIp=extenderIp;
	if (bExtendMap)
	{
		GET_PROGUARD()->mProgardFlags |= SVT_ProgardThread::PF_NEED_REGEN_EXTENDER_LIST;
	}
	else
	{
		GET_PROGUARD()->mProgardFlags &= ~SVT_ProgardThread::PF_NEED_REGEN_EXTENDER_LIST;
	}

    GET_PROGUARD()->mProgardMutex.unlock();
    return ret;
}
//modify end

//void SVT_ControlProxy::refreshNetgearInfo()
//{
//    if(mCore->mCoreExit)
//    {
//        return;
//    }
//    Q_ASSERT(mCore && mCore->mNetgearInfoThread);
//    if( !mCore->mNetgearInfoThread->isRunning())
//    {
//        mCore->mNetgearInfoThread->start();
//    }
//}

void SVT_ControlProxy::setExitFlag()
{
    mCore->mCoreExit=true;
}

void SVT_ControlProxy::blockDeviceCmd(int cmd, QVariant para)
{
    mInnerFSMController->setCmd(cmd,para);
}

void SVT_ControlProxy::funcDataReturned( SVT_NotifyType &data)
{
    emit signal_nodeDataReturned(data);
}

void SVT_ControlProxy::countChanged( int count)
{
    emit signal_countChanged(count);
}

void SVT_ControlProxy::otherDeviceCountChanged(QString deviceIp, int count)
{
	emit signal_otherDevCountChanged(deviceIp,count);
}

void SVT_ControlProxy::funcIfaceReturned( SVT_NotifyType &data)
{
    emit signal_ifaceDataReturned(data);
}

void SVT_ControlProxy::funcNewDeviceAdded(SVT_NotifyType &data)
{
    emit signal_newDeviceAdded(data);
}

void SVT_ControlProxy::funcGatewayChanged(const QString &gateway)
{
    emit signal_gatewayChanged(gateway);
}

void SVT_ControlProxy::funcNetgearInfoReturned(const QMap<int,QString> data)
{
    emit signal_NetgearInfo(data);
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

_InnerFSMController::_InnerFSMController()
    :mFSM(NULL)
{

}

void _InnerFSMController::setFSM(SVT_NetgearSpecificFSM *fsm)
{
   // Q_ASSERT(!mFSM || !fsm);
    mSetContentMutex.lock();
    mFSM=fsm;
    mSetContentMutex.unlock();
}

void _InnerFSMController::setCmd(int cmd, const QVariant &para)
{
    mSetContentMutex.lock();
    if(mFSM)
    {
        mFSM->setCmd(cmd,para);
    }
    mSetContentMutex.unlock();
}

//void _InnerFSMController::FSMCmd(int cmd, const QVariant &para)
//{
//    emit signal_FSMCmd(cmd,para);
//}
