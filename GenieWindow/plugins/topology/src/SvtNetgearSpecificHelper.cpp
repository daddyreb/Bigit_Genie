#include "topology.h"
//#include "netgearsoapschedule.h"
#include "SvtNetgearSpecificHelper.h"
#include "SvtProgardThread.h"
#include <QDesktopServices>

#define DUMMY_WHEN_ATTACH_DEVICE_ERR 60000
#define ATTACH_DEVICE_RETRY_TIME 5
#define DUMMY_WHEN_ENABLE_STAT_ERR 60000
#define ENABLE_STAT_RETRY_TIME 5
#define FSM_RESET_TIMEOUT 15 //minites
#define REFRESH_AFTER_DEVICE_DETECTED 5
using namespace BLOCKDEVICE_FSM;

SVT_NetgearSpecificFSM::SVT_NetgearSpecificFSM(SVT_NetgearSpecificHelper *helper)
    :mState(FS_INIT)
    ,mOldState(FS_INIT)
//    ,mSoapSchedule(new NetgearSoapSchedule)
    ,mHelper(helper)
    ,mRetryTimes(0)
    ,mCurrentCmd(BDC_NONE)
    ,mlogedIn(false)
	    ,mQuickPassthrough(false)
{
    mDummyBeforThisTime=QDateTime::currentDateTime().addYears(-1);
//#ifdef REFRESH_ATTACHDEVICE_WHEN_NEW_DEV_DETECTED
//    connect(&mRefreshTimer,SIGNAL(timeout()),this,SLOT(slot_timeoutRefreshAttachDevice()));
//#endif
    mSoap=new SVT_Soap(&mHelper->mMethod->mNodePool->mPoolExit);
    mSoap->setAutoChangePort(true);
}

SVT_NetgearSpecificFSM::~SVT_NetgearSpecificFSM()
{
    delete mSoap;
}

void SVT_NetgearSpecificFSM::start(const QString &ip, const QString &routetype)
{
    if(mState==FS_INIT)
    {
        mHost=ip;
        mRouteType=routetype;
        changeState(FS_GETTING_ATTACHDEVICE);
    }
}

bool SVT_NetgearSpecificFSM::busy()
{
   int cmd=getCmd();

   if(mState==FS_INIT || mState==FS_END)
   {
       return false;
   }

   if(cmd !=BDC_NONE)
   {
       return true;
   }
   if(QDateTime::currentDateTime()<mDummyBeforThisTime)
   {
	   return false;
   }

   if(mState==FS_GETTING_ATTACHDEVICE ||mState==FS_GETTING_ENABLESTAT
           ||mState==FS_LOGING
           ||mState==FS_ENABLING
           ||mState==FS_DISABLING
           ||mState==FS_ENABLE_DISABLE_MAC )
   {
       return true;
   }

   return false;
}

void SVT_NetgearSpecificFSM::update()
{

    if(updateResetFSM())
    {
        changeState(FS_GETTING_ATTACHDEVICE);
        return;
    }
#ifdef REFRESH_ATTACHDEVICE_WHEN_NEW_DEV_DETECTED
    if(checkIfNeedRefresh())
    {
        changeState(FS_GETTING_ATTACHDEVICE);
        return;
    }
#endif
    if(QDateTime::currentDateTime()<mDummyBeforThisTime)
    {
        return;
    }
    switch(mState)
    {
    case FS_INIT:
        // do nothing but wait start function
        break;
    case FS_GETTING_ATTACHDEVICE:
		updateGettingAttacheDevice();	
		//add by caiyujun
		//updateGettingExtenderAttatcheDevice();
		//add end
        break;
    case FS_GETTING_ENABLESTAT:
        updateGettingEnableStat();
        break;
    case FS_RUNNING:
        updateRunning();
        break;
    case FS_CLOSED:
        updateClosed();
        break;
    case FS_CLOSED_ADMIN:
        updateClosedADM();
        break;
    case FS_LOGING:
        updateLoging();
        break;
    case FS_RUNNING_ADMIN:
        updateRunningADM();
        break;
    case FS_ENABLING:
        updateEnabling();
        break;
    case FS_DISABLING:
        updateDisabling();
        break;
    case FS_ENABLE_DISABLE_MAC:
        updateEnableDisablingMac();
        break;
    case FS_END:
		//updateGettingAttacheDevice();
		//updateGettingExtenderAttatcheDevice();	
        break;
    default:
        Q_ASSERT(0);
    }
}

void SVT_NetgearSpecificFSM::updateGettingAttacheDevice()
{
    SVT_Soap &soap=*mSoap;
    soap.setRouteType(mRouteType);
    QMap<QString,QString> soapret;
    soap.doFunc(mHost,"DeviceInfo","GetAttachDevice",NULL,&soapret);
//    qDebug()<<soapret;

    QString resultstr = soapret.value("NewAttachDevice");
    qDebug()<<resultstr;
    QStringList strlst=resultstr.split("@");
    bool dataGetted=false;
    for(int i=1;i<strlst.count();i++)
    {
        SVT_NodeInfoRef nodeinfoRef(new QMap<int,QString>);
        QStringList slinelist=strlst[i].split(";");
        SVT_IpAddress ip;
        SVT_MacAddress mac;
        if(slinelist.count()>=4)
        {
            ip.Set(slinelist[1]);
            mac.SetAddress(slinelist[3]);
            if(0!=slinelist[2].compare("<unknown>",Qt::CaseInsensitive)
                    && 0!=slinelist[2].compare("--",Qt::CaseInsensitive))
            {
                nodeinfoRef->insert(NIE_ATTACHDEVICE_NAME,slinelist[2]);
            }

        }
        if(slinelist.count()>=5)
        {
            nodeinfoRef->insert(NIE_ATTACHDEVICE_ISWIRED
                                ,(slinelist[4].toLower()=="wireless")?"n":"y");
        }
        if(slinelist.count()>=7)
        {
            nodeinfoRef->insert(NIE_ATTACHDEVICE_LINKSPEED,slinelist[5]);
            nodeinfoRef->insert(NIE_ATTACHDEVICE_SIGNALSTRENGTH,slinelist[6]);
        }
        if(slinelist.count() >=8)
        {
            nodeinfoRef->insert(NIE_ATTACHDEVICE_BLOCKED,slinelist[7]=="Block"?"y":"n");
        }
        if(ip.isValid() && mac.isValid())
        {
            mHelper->mMethod->mNodePool->nodeDetected(mHelper->mMethod,ip,mac,nodeinfoRef);
//                    mSoapState=SS_END;

            dataGetted=true;
        }
    }
    if(dataGetted )
    {

#ifdef BLOCK_DEVICE

#ifdef REFRESH_ATTACHDEVICE_WHEN_NEW_DEV_DETECTED
        CORE_LOCK();
        GET_PROGUARD()->mProgardFlags &= ~SVT_ProgardThread::PF_REFRESH_ATTACHDEVICE;
        CORE_UNLOCK();
//        mRefreshTimer.stop();
        mstartRefreshDT=QDateTime::currentDateTime().addYears(20);
#endif

 //        changeState(FS_END);
       changeState(FS_GETTING_ENABLESTAT);
#else
       changeState(FS_END);
#endif
    }
    else
    {
        if(mQuickPassthrough || ++mRetryTimes>ATTACH_DEVICE_RETRY_TIME )
        {
            changeState(FS_END);
        }
        else
        {
            mDummyBeforThisTime=QDateTime::currentDateTime().addMSecs(DUMMY_WHEN_ATTACH_DEVICE_ERR);
        }
    }

}

void SVT_NetgearSpecificFSM::updateGettingExtenderAttatcheDevice()
{
    QMap<QString,QString> soapret;
	QMap<QString,QString> soapparas;
	soapparas.insert("NewUsername","admin");
    soapparas.insert("NewPassword","password");

	QMap<unsigned long,QString> extenderDevMap;
	int iDeviceCount = 0;
	_ListNode<SVT_NodeRef> *pLoop=GET_CORE().mExtenderList.pFirst;
	while(pLoop)
	{
		if(!pLoop->data.isNull())
		{
			SVT_Soap &soap=*mSoap;
			soap.setRouteType(mRouteType);

			QString strIp = pLoop->data->mIp.toString();

			unsigned long ulIp = inet_addr(strIp.toStdString().c_str());
			//QString strTemp = strIp + ":Extender";

			bool btemp=soap.doFunc(strIp,"DeviceConfig","Authenticate",&soapparas,&soapret);
			if (btemp)
			{
				soapret.clear();
				soap.doFunc(strIp,"DeviceInfo","GetAttachedDevice",NULL,&soapret);

				QString resultstr = soapret.value("NewAttachDevice");
				qDebug()<<resultstr;
				if (0 == resultstr.compare(""))
				{
					resultstr = soapret.value("AttachededDeviceList");
				}
				if (0 != resultstr.compare(""))
				{
					extenderDevMap.insert(ulIp,resultstr);
					QStringList strlst=resultstr.split("@");

					for(int j=1;j<strlst.count();j++)
					{
						QStringList slinelist=strlst[j].split(";");
						SVT_IpAddress ip;
						SVT_MacAddress mac;
						if(slinelist.count()>=4)
						{
							ip.Set(slinelist[1]);
							mac.SetAddress(slinelist[3]);

							if(ip.isValid() && mac.isValid())
							{
								iDeviceCount++;
							}
						}
					}
				}

			}		
		}
		pLoop=pLoop->pNext;
	}

	if (iDeviceCount > 0)
	{
		int iTempCount = 0;
		QMap<unsigned long,QString>::iterator it = extenderDevMap.begin();
		for (;it != extenderDevMap.end(); it++) 
		{
			unsigned long ulIp = it.key();
			QString strTemp = it.value();
			QStringList strlst = strTemp.split("@");

			for(int i=1;i<strlst.count();i++)
			{
				SVT_NodeInfoRef nodeinfoRef(new QMap<int,QString>);
				QStringList slinelist=strlst[i].split(";");
				SVT_IpAddress ip;
				SVT_MacAddress mac;
				if(slinelist.count()>=4)
				{
					ip.Set(slinelist[1]);
					mac.SetAddress(slinelist[3]);
					if(0!=slinelist[2].compare("<unknown>",Qt::CaseInsensitive)
						&& 0!=slinelist[2].compare("--",Qt::CaseInsensitive))
					{
						nodeinfoRef->insert(NIE_ATTACHDEVICE_NAME,slinelist[2]);
					}
				}
				if(slinelist.count()>=5)
				{
					nodeinfoRef->insert(NIE_ATTACHDEVICE_ISWIRED
						,(slinelist[5].toLower()=="wireless")?"n":"y");
				}
				if(slinelist.count()>=7)
				{
					nodeinfoRef->insert(NIE_ATTACHDEVICE_LINKSPEED,slinelist[5]);
					nodeinfoRef->insert(NIE_ATTACHDEVICE_SIGNALSTRENGTH,slinelist[6]);
				}
				if(slinelist.count() >=8)
				{
					nodeinfoRef->insert(NIE_ATTACHDEVICE_BLOCKED,slinelist[7]=="Block"?"y":"n");
				}
				if(ip.isValid() && mac.isValid())
				{
					nodeinfoRef->insert(NIE_ISEXTENDER_ATTATCHDEVICE,"y");
					//unsigned long ip = strIp.
					iTempCount++;
					if (1 == iTempCount && 1 == iDeviceCount)
					{
						mHelper->mMethod->mNodePool->nodeDetected(mHelper->mMethod,ip,mac,nodeinfoRef,ulIp,EXTENDER_STARTEND);
					}
					else if (1 == iTempCount && 1 != iDeviceCount)
					{
						mHelper->mMethod->mNodePool->nodeDetected(mHelper->mMethod,ip,mac,nodeinfoRef,ulIp,EXTENDER_START);
					}
					else if (1 < iTempCount && iTempCount != iDeviceCount)
					{
						mHelper->mMethod->mNodePool->nodeDetected(mHelper->mMethod,ip,mac,nodeinfoRef,ulIp,EXTENDER_MID);
					}
					else if (1 < iTempCount && iTempCount == iDeviceCount)
					{
						mHelper->mMethod->mNodePool->nodeDetected(mHelper->mMethod,ip,mac,nodeinfoRef,ulIp,EXTENDER_END);
					}
				}
			}
		}
	}
}

void SVT_NetgearSpecificFSM::updateGettingEnableStat()
{
    SVT_Soap &soap=*mSoap;
    soap.setRouteType(mRouteType);
    QMap<QString,QString> soapret;
    soap.doFunc(mHost,"DeviceConfig","GetBlockDeviceEnableStatus",NULL,&soapret);
    bool btemp;
    if(0==soapret.value(RCODE_TAG).toInt(&btemp) && btemp && soapret.contains("NewBlockDeviceEnable"))
    {
        if(1==soapret.value("NewBlockDeviceEnable").toInt())
        {
            changeState(mlogedIn?FS_RUNNING_ADMIN:FS_RUNNING);
        }
        else
        {
            changeState(mlogedIn?FS_CLOSED_ADMIN:FS_CLOSED);
        }
    }
    else
    {
        if(mQuickPassthrough || ++mRetryTimes >ENABLE_STAT_RETRY_TIME)
        {
            changeState(FS_END);
        }
        else
        {
            mDummyBeforThisTime=QDateTime::currentDateTime().addMSecs(DUMMY_WHEN_ENABLE_STAT_ERR);
        }
    }
    setCmd(BDC_NONE);
//    mCurrentCmd=BDC_NONE;
}

void SVT_NetgearSpecificFSM::updateRunning()
{
    Q_ASSERT(!mlogedIn);
    int cmd=getCmd();
#ifdef REFRESH_ATTACHDEVICE_WHEN_NEW_DEV_DETECTED
    checkIfNeedRefresh();
#endif
    if(cmd==BDC_LOGIN_TO_RUNNING_ADMIN
            ||cmd==BDC_LOGIN_TO_DISABLING
            ||cmd==BDC_LOGIN_TO_ENABLINGDISABLING_MAC
            )
    {
        changeState(FS_LOGING);
    }
}

void SVT_NetgearSpecificFSM::updateClosed()
{
    Q_ASSERT(!mlogedIn);
    int cmd=getCmd();
    if(cmd==BDC_LOGIN_TO_CLOSED_ADMIN
            ||cmd==BDC_LOGIN_TO_ENABLING)
    {
        changeState(FS_LOGING);
    }
}

void SVT_NetgearSpecificFSM::updateRunningADM()
{

    int cmd=getCmd();
#ifdef REFRESH_ATTACHDEVICE_WHEN_NEW_DEV_DETECTED
    checkIfNeedRefresh();
#endif
    if(cmd==BDC_DISABLE)
    {
        changeState(FS_DISABLING);
    }
    else if(cmd==BDC_ENABLINGDISABLING_MAC)
    {
        changeState(FS_ENABLE_DISABLE_MAC);
    }
}

void SVT_NetgearSpecificFSM::updateClosedADM()
{
    if(getCmd()==BDC_ENABLE)
    {
        changeState(FS_ENABLING);
    }
}

void SVT_NetgearSpecificFSM::updateLoging()
{
    Q_ASSERT(mOldState==FS_RUNNING || mOldState==FS_CLOSED);
    QList<QVariant> paralist=mCurrentCmdPara.toList();
    QString usrname=paralist.value(0).toString();
    QString pwd=paralist.value(1).toString();
    SVT_Soap &soap=*mSoap;
    soap.setRouteType(mRouteType);
    QMap<QString,QString> soappara;
    soappara.insert("NewUsername",usrname);
    soappara.insert("NewPassword",pwd);
    QMap<QString,QString> soapret;
    soap.doFunc(mHost,"ParentalControl","Authenticate",&soappara,&soapret);
    bool btemp;
    if(0==soapret.value(RCODE_TAG).toInt(&btemp) && btemp )
    {
        int cmd=getCmd();
        switch(cmd)
        {
        case BDC_LOGIN_TO_RUNNING_ADMIN:
            setCmd(BDC_NONE);
            changeState(FS_RUNNING_ADMIN);
            break;
        case BDC_LOGIN_TO_CLOSED_ADMIN:
            setCmd(BDC_NONE);
            changeState(FS_CLOSED_ADMIN);
            break;
        case BDC_LOGIN_TO_DISABLING:
            setCmd(BDC_DISABLE,paralist.value(2));
            changeState(FS_DISABLING);
            break;
        case BDC_LOGIN_TO_ENABLING:
            setCmd(BDC_ENABLE,paralist.value(2));
            changeState(FS_ENABLING);
            break;
        case BDC_LOGIN_TO_ENABLINGDISABLING_MAC:
            setCmd(BDC_ENABLINGDISABLING_MAC,paralist.value(2));
            changeState(FS_ENABLE_DISABLE_MAC);
            break;
        default:
            Q_ASSERT(0);
        }
        mlogedIn=true;
    }
    else
    {
        setCmd(BDC_NONE);
        changeState(mOldState);
    }


}

void SVT_NetgearSpecificFSM::updateEnabling()
{
//    Q_ASSERT(getCmd()==BDC_ENABLE);
    SVT_Soap &soap=*mSoap;
    soap.setRouteType(mRouteType);
    QMap<QString,QString> soapret;
    QMap<QString,QString> paras;

    soap.doFunc(mHost,"DeviceConfig","EnableBlockDeviceForAll",NULL,&soapret);
    bool btemp;
    BLOCK_DEVICE_FSM_STATE laststate=FS_CLOSED_ADMIN;
    if(0==soapret.value(RCODE_TAG).toInt(&btemp) && btemp)
    {
        paras.insert("NewBlockDeviceEnable","1" );
        soap.doFunc(mHost,"DeviceConfig","SetBlockDeviceEnable",&paras,&soapret);

        if(0==soapret.value(RCODE_TAG).toInt(&btemp) && btemp)
        {
            laststate=FS_GETTING_ATTACHDEVICE;
        }
    }
    changeState(laststate);
    setCmd(BDC_NONE);


}

void SVT_NetgearSpecificFSM::updateDisabling()
{
//    Q_ASSERT(mCurrentCmd==BDC_DISABLE);
    SVT_Soap &soap=*mSoap;
    soap.setRouteType(mRouteType);
    QMap<QString,QString> soapret;
    QMap<QString,QString> paras;
    paras.insert("NewBlockDeviceEnable","0" );
    soap.doFunc(mHost,"DeviceConfig","SetBlockDeviceEnable",&paras,&soapret);
    bool btemp;
    if(!(0==soapret.value(RCODE_TAG).toInt(&btemp) && btemp))
    {
        //changeState(FS_RUNNING_ADMIN);
		changeState(FS_GETTING_ENABLESTAT);
    }
    else
    {
        changeState(FS_GETTING_ATTACHDEVICE);
    }
    setCmd(BDC_NONE);
}

void SVT_NetgearSpecificFSM::updateEnableDisablingMac()
{
//    Q_ASSERT(mCurrentCmd==BDC_ENABLINGDISABLING_MAC);
    SVT_Soap &soap=*mSoap;
    soap.setRouteType(mRouteType);
    QMap<QString,QVariant> paralist=mCurrentCmdPara.toMap();
    QMap<QString,QString> soapret;
    QMap<QString,QString> paras;
    QMap<QString,QVariant>::const_iterator i=paralist.begin();
//    bool soapsuccess=false;
    bool btemp;
    QMap<QString,int> setted_macs;
    while(i!=paralist.end())
    {
        QString mac=i.key();
        QString allow=(i.value().toInt()==0)?"Allow":"Block";
        paras.insert("NewMACAddress",mac);
        paras.insert("NewAllowOrBlock",allow);
        soap.doFunc(mHost,"DeviceConfig","SetBlockDeviceByMAC",&paras,&soapret);
        bool soapsuccess=(0==soapret.value(RCODE_TAG).toInt(&btemp) && btemp);
        if(soapsuccess)
        {
            setted_macs.insert(i.key(),(i.value().toInt()));
        }
        i++;
    }

    if(!setted_macs.isEmpty())
    {
        changeState(FS_GETTING_ATTACHDEVICE);
        QMap<QString,int>::const_iterator i_mac=setted_macs.begin();
        while(i_mac!=setted_macs.end())
        {
            SVT_NodeInfoRef nodeinfoRef(new QMap<int,QString>);
            nodeinfoRef->insert(NIE_ATTACHDEVICE_BLOCKED,(i_mac.value()==0)?"n":"y");
            SVT_MacAddress mac;
            mac.SetAddress(i_mac.key());
            SVT_IpAddress null_ip;
            mHelper->mMethod->mNodePool->nodeDetected(mHelper->mMethod,null_ip,mac,nodeinfoRef);

            i_mac++;
        }
    }
    else
    {
        changeState(FS_RUNNING_ADMIN);
    }
    setCmd(BDC_NONE);
}

bool SVT_NetgearSpecificFSM::updateResetFSM()
{
    const BLOCK_DEVICE_FSM_STATE cStates[]={
      FS_RUNNING,FS_RUNNING_ADMIN,FS_CLOSED,FS_CLOSED_ADMIN,FS_END
    };
    bool stateOk=false;
    for(int i=0;i<sizeof(cStates)/sizeof(cStates[0]);i++)
    {
        if(cStates[i]==mState)
        {
            stateOk=true;
            break;
        }
    }
    if(stateOk &&
            mAttachDeviceDT.addSecs(FSM_RESET_TIMEOUT*60)<QDateTime::currentDateTime())
    {
        return true;
    }
    return false;
}

#ifdef REFRESH_ATTACHDEVICE_WHEN_NEW_DEV_DETECTED
bool SVT_NetgearSpecificFSM::checkIfNeedRefresh()
{
    //check state ok
    const BLOCK_DEVICE_FSM_STATE cStates[]={
      FS_GETTING_ENABLESTAT,FS_RUNNING,FS_RUNNING_ADMIN,FS_CLOSED,FS_CLOSED_ADMIN,FS_END
    };
    bool stateOk=false;
    for(int i=0;i<sizeof(cStates)/sizeof(cStates[0]);i++)
    {
        if(cStates[i]==mState)
        {
            stateOk=true;
            break;
        }
    }
    if(!stateOk)
    {
        return false;
    }
    //check state ok end


    if(mstartRefreshDT.addSecs(REFRESH_AFTER_DEVICE_DETECTED)<QDateTime::currentDateTime())
    {
        mstartRefreshDT=QDateTime::currentDateTime().addYears(20);
        return true;
    }
    CORE_LOCK();
    SVT_ProgardThread *proguard=GET_PROGUARD();
    if(proguard->mProgardFlags & SVT_ProgardThread::PF_REFRESH_ATTACHDEVICE)
    {
       proguard->mProgardFlags &=~SVT_ProgardThread::PF_REFRESH_ATTACHDEVICE;
       mstartRefreshDT=QDateTime::currentDateTime();
//       mRefreshTimer.stop();
//       mRefreshTimer.setSingleShot(true);
//       mRefreshTimer.start(REFRESH_AFTER_DEVICE_DETECTED);

    }
    CORE_UNLOCK();
    return false;
}

void SVT_NetgearSpecificFSM::slot_timeoutRefreshAttachDevice()
{
    changeState(FS_GETTING_ATTACHDEVICE);
}


#endif
//void SVT_NetgearSpecificFSM::updateInit(QtSoapMessage *soapmessage)
//{
//    Q_ASSERT(mState==FS_INIT);
//}

void SVT_NetgearSpecificFSM::changeState(int state)
{
    Q_ASSERT(FS_INIT<=state && state <=FS_END);
    if(state==FS_INIT)
    {
        mlogedIn=false;
    }
    if(mOldState==FS_GETTING_ATTACHDEVICE)
    {
        mAttachDeviceDT=QDateTime::currentDateTime();
    }
    if(mState==FS_GETTING_ATTACHDEVICE && mOldState != FS_INIT)
    {
        mQuickPassthrough=true;
    }
    if(mOldState==FS_GETTING_ENABLESTAT)
    {
        mQuickPassthrough=false;
    }

    qDebug()<<"SVT_NetgearSpecificFSM::changeState"<<state;
    mOldState=mState;
    mState=state;
    mRetryTimes=0;
    mDummyBeforThisTime=QDateTime::currentDateTime().addYears(-1);
//#ifdef REFRESH_ATTACHDEVICE_WHEN_NEW_DEV_DETECTED
//    mRefreshTimer.stop();
//#endif
//    mCurrentCmd=BDC_NONE;
    emit signal_FSMStateChanged(state,mOldState);
}

void SVT_NetgearSpecificFSM::setCmd(int cmd,const QVariant &para)
{
//    Q_ASSERT(QThread::currentThread()==qApp->thread());
    mCmdMutex.lock();
    mCurrentCmd=cmd;
    mCurrentCmdPara=para;
    mCmdMutex.unlock();
//    mHelper->mSemaphore.release();
}

void SVT_NetgearSpecificFSM::serialize(bool load)
{
    CORE_LOCK();
    SVT_NodePool *pool=mHelper->mMethod->mNodePool;

    QFile f;
    QString localmac=pool->mInterface.getMac().toString();
    if(load &&(pool->mFlags&SVT_NodePool::NPF_REFRESH_ALL) )
    {
        goto fsm_serialize_end;
    }

    localmac.replace(":","");

    //localmac=QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/NETGEARGenie/"
    localmac=QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/"
    +localmac+".fssave";
    f.setFileName(localmac);


    if(load)
    {
        if(f.open(QIODevice::ReadOnly))
        {
            QDataStream in(&f);
            in>>mState;
            mOldState=FS_INIT;

        }
    }
    else
    {
        if(f.open(QIODevice::WriteOnly))
        {
            const int cParas[]={
                FS_INIT// FS_INIT
                ,FS_INIT// ,FS_GETTING_ATTACHDEVICE
                ,FS_INIT// ,FS_GETTING_ENABLESTAT
                ,FS_RUNNING// ,FS_RUNNING
                ,FS_CLOSED//,FS_CLOSED
                ,FS_RUNNING_ADMIN//,FS_RUNNING_ADMIN
                ,FS_CLOSED_ADMIN//,FS_CLOSED_ADMIN
                ,FS_INIT//,FS_LOGING
                ,FS_CLOSED_ADMIN//,FS_ENABLING
                ,FS_RUNNING_ADMIN//,FS_DISABLING
                ,FS_RUNNING_ADMIN//,FS_ENABLE_DISABLE_MAC
                ,FS_END
            };
            QDataStream out(&f);
            Q_ASSERT(mState>=0 && mState<(sizeof(cParas)/sizeof(cParas[0])));
            out<<cParas[mState];
        }
    }

fsm_serialize_end:
    CORE_UNLOCK();

}

int SVT_NetgearSpecificFSM::getCmd()
{
    int ret;
    mCmdMutex.lock();
    ret=mCurrentCmd;
    mCmdMutex.unlock();
    return ret;
}






/*************************************************************************/
/*************************************************************************/
/*************************************************************************/
/*************************************************************************/



SVT_NetgearSpecificHelper::SVT_NetgearSpecificHelper(SVT_TopoMethod *method,SVT_SeedFactoryInterface *factory)
    :mMethod(method)
    ,mFactory(factory)
   // ,mSoapSchedule(new NetgearSoapSchedule)
//    ,mSoapState(SS_INIT)
    ,mLastSoapDT(QDateTime::currentDateTime().addDays(-1))
	,mFSM(this)
{
//    connect(&mTimer,SIGNAL(timeout()),this,SLOT(slot_Timeout()));
    //connect(mSoapSchedule,SIGNAL(getResponse(QtSoapMessage&,QUuid&,QString&))
    //        ,this,SLOT(slot_SoapResponse(QtSoapMessage&,QUuid&,QString&)));
    SVT_ControlProxy *controlproxy=GET_CORE().mControlInterface;
    connect(&mFSM,SIGNAL(signal_FSMStateChanged(int,int)),controlproxy,SIGNAL(signal_FSMStateChanged(int,int)));
    controlproxy->mInnerFSMController->setFSM(&mFSM);
//    connect(controlproxy->mInnerFSMController,SIGNAL(signal_FSMCmd(int,QVariant))
//            ,&mFSM,SLOT(slot_CmdReceived(int,QVariant)));
    mFSM.changeState(FS_INIT);
}

SVT_NetgearSpecificHelper::~SVT_NetgearSpecificHelper()
{
//    mTimer.stop();
//    disconnect(&mTimer,SIGNAL(timeout()),0,0);
    disconnect(&mFSM,SIGNAL(signal_FSMStateChanged(int,int)),0,0);
    SVT_ControlProxy *controlproxy=GET_CORE().mControlInterface;
    controlproxy->mInnerFSMController->setFSM(NULL);
}


void SVT_NetgearSpecificHelper::run()
{
//    mTimer.start(100);
    while(!mMethod->mNodePool->mPoolExit)
    {
        bool fsm_busy=mFSM.busy();
        mFSM.update();
        if(fsm_busy)
        {
            continue;
        }
        else
        {
            SLEEP(100);
        }


        slot_Timeout(fsm_busy);
    }
  //  mFSM.serialize(false);
    mFSM.changeState(FS_END);
//    mEventLoop.exec();
//    mTimer.stop();
}

void SVT_NetgearSpecificHelper::slot_Timeout(bool fsm_busy)
{
//    if(mMethod->mNodePool->mPoolExit)
//    {
//        mEventLoop.exit();
//    }


    QList<quint32> retlist;
//    trySubmitSoap();


    int n=mFactory->getSeed(retlist);

    //sleep when no seed
    if(n <=0)
    {
        const int t=100;
        const int a=2000;
        int c=0;
        while(!mFSM.busy() && c<a)
        {
            SLEEP(t);
            c+=t;
        }
    }

    foreach(quint32 ip,retlist)
    {
        if(mMethod->mNodePool->mPoolExit || mFSM.busy())
        {
            break;
        }
        SVT_IpAddress _ip;
        _ip.Set(ip);
        QMap<int,QString> *retmap=new QMap<int,QString>;
        SVT_NodeInfoRef nodeinfoRef(retmap);
        CORE_UTIL().netgearGetCurrentSetting(_ip,*retmap,&(mMethod->mNodePool->mPoolExit));


        SVT_MacAddress nullmac;
        if(!nodeinfoRef->isEmpty())
        {
            mMethod->mNodePool->nodeDetected(mMethod,ip,nullmac,nodeinfoRef);
        }
        //start attach device soap
        CORE_LOCK();
        const SVT_IpAddress gateway=mMethod->mNodePool->mInterface.getGateway();

        if(gateway.asLong()==ip && nodeinfoRef->contains(NIE_CURRENTSETTING_MODEL))
        {

            mFSM.start(gateway.toString(),nodeinfoRef->value(NIE_CURRENTSETTING_MODEL));
//            mLastSoapDT=QDateTime::currentDateTime().addDays(-1);


        }
        CORE_UNLOCK();

    }  //foreach

}

