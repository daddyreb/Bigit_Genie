#include "QGenieTimelineLocker.h"
#include "QGenieRCWirelessObject.h"
#include "genie2_interface.h"
#include "language_RouterConfig.h"
#include "QGenieDelayProcessLogic.h"
#include "genie2_netgear.h"
#include "QPiObject.h"
#include <QUiLoader>

#define NOTIFY_SSID_TIME 20
#define RECONNECT_TIME 5
//#define SINGLE_FINISH_TIME 5
extern quint64 sRCFlags;


QGenieTimelineLocker::QGenieTimelineLocker()
    :mLabel(NULL)
    ,mLockerUi(NULL)
    ,mFlags(0)
    ,mPi(NULL)
    ,mEndObj(NULL)
    ,mEndSLOT(NULL)
    ,mDelayFinishTime(0)
{
//    mTimeLine.setDuration(GACC_DISABLE_DLG_DURATION);
    mTimeLine.setFrameRange(0,5000);
    mTimeLine.setCurveShape(QTimeLine::LinearCurve);
    mTimeLine.setUpdateInterval(400);
//    ui->label->setText(QString("%1(%2)").arg(msg,QString::number(GACC_DISABLE_DLG_FRAME_COUNT/1000)));


    connect(&mTimeLine,SIGNAL(valueChanged(qreal)),this,SLOT(slot_ValueChanged(qreal)));
    connect(&mTimeLine,SIGNAL(finished()),this,SLOT(slot_Finished()));
//    mTimeLine.start();
//    mDelayFinishTimer.setSingleShot(true);
//    connect(&mDelayFinishTimer,SIGNAL(timeout()),this,SLOT(slot_Finished()));
}

QGenieTimelineLocker::~QGenieTimelineLocker()
{
//    GENIE2_SAFEDELETE(mPi);
}


//////////////////////////////////////////////////////////////////////////
///kai.yan modify 
///flag:等待时显示的文本信息标志 
///0:Receiving router response information
///1:Rebooting router
///2:Upgrading firmware 
//////////////////////////////////////////////////////////////////////////
void QGenieTimelineLocker::startTimer(int duration, QObject *obj, const char *slot, int flag)
{
//    mPi=pi;
//    if(pi)
//    {
//        pi->startAnimation();
//    }
    Q_ASSERT((duration/1000>qMax(NOTIFY_SSID_TIME,RECONNECT_TIME)) || !(mFlags&TF_NEED_RECONNECT));
    QGenieRCWirelessObject *rcwo=QGenieRCWirelessObject::GetSingletonPtr();
    Q_ASSERT(rcwo && rcwo->mWirelessW);
    if(!mLockerUi)
    {
        QUiLoader loader;
        bool btemp;
        QFile f(GENIE2_RES("ui/rccountdown.ui"));
        btemp=f.open(QFile::ReadOnly);
        Q_ASSERT(btemp);

        mLockerUi=loader.load(&f,rcwo->mWirelessW->parentWidget());
        QRect geo=rcwo->mWirelessW->geometry();
        mLockerUi->move(geo.center()-QPoint(mLockerUi->width(),mLockerUi->height())/2);

        QFile fqss(GENIE2_RES("ui/framelessframe.qss"));
        btemp=fqss.open(QIODevice::ReadOnly);
        Q_ASSERT(btemp);
        QString stemp=fqss.readAll();
        mLockerUi->setStyleSheet(stemp);
//        GENIE2_SET_FRAMELESSSTYLE(mLockerUi);
        fqss.close();

        Q_ASSERT(mLockerUi);
        mLabel=mLockerUi->findChild<QLabel *>("lab_content");
        Q_ASSERT(mLabel);
        mLabel->setStyleSheet("color:red;");

        mPi=new QPiObject(mLockerUi,"pi");
        mPi->startAnimation();

        mLockerUi->raise();
        mLockerUi->show();
    }
    mTimeLine.setDuration(duration);
    mTimeLine.setStartFrame(0);
    mTimeLine.setDirection(QTimeLine::Forward);
    mTimeLine.start();
	if(flag==0)
	{
		mMode=GENIE2_GET_TEXT(L_RTCT_MSG_COMMITWAIT);
	}else if(flag==1)
	{
		mMode=GENIE2_GET_TEXT(L_OHTERS_MSG_COMMITWAIT);
	}else if(flag ==2)
	{
		mMode=GENIE2_GET_TEXT(L_UPDATELAB_FIRMWARE_NEEDFEWTIME);
	}
	else
	{
		mMode=GENIE2_GET_TEXT(L_RTCT_MSG_COMMITWAIT);
	}
    sRCFlags |=RCF_TIMELINELOCKER_WORKING;
    if(sRCFlags &RCF_REMOTE)
    {
        sRCFlags |=RCF_IN_REMOTE_REFRESHING;
    }
    else
    {
        sRCFlags |=RCF_IN_LOCAL_REFRESHING;
    }

    mEndObj=obj;
    mEndSLOT=slot;
    setNeedReconnect();


}

void QGenieTimelineLocker::setNotifySsidData(const QString &ssid, const QString &pwd, const QString &wpa, const QString &rz)
{
    mFlags |= TF_NEED_NOTIFY_SSID;
    mNotifySsid=ssid;
    mNotifyPwd=pwd;
    mNotifyWpa=wpa;
    mNotifyRZ=rz;
}

void QGenieTimelineLocker::setNeedReconnect(const QString &new24ssid, const QString &new24pwd
                                            , const QString &new5ssid, const QString &new5pwd)
{
    QString curSsid=GENIE2_HANDLER_CMD(HCMD_ASK_SUBSCRIBED_MESSAGE,SCCMD_SSID).toString();
    mFlags &=~TF_NEED_RECONNECT;
    mNeedReconnectSsid=mNeedReconnectPwd="";

    if(!curSsid.isEmpty())
    {
        QGenieRCWirelessObject *rcwo=QGenieRCWirelessObject::GetSingletonPtr();
        Q_ASSERT(rcwo);
        Q_ASSERT(!rcwo->mSavedData.isNull()&&!rcwo->mSavedData_5G.isNull());
        if(curSsid==rcwo->mSavedData->mSavedSsid)
        {
            mFlags |= TF_NEED_RECONNECT;
            mNeedReconnectSsid=new24ssid.isEmpty()?rcwo->mSavedData->mSavedSsid:new24ssid;
            mNeedReconnectPwd=new24pwd.isEmpty()?rcwo->mSavedData->mSavedPwd:new24pwd;
        }
        else if(curSsid==rcwo->mSavedData_5G->mSavedSsid)
        {
            mFlags |= TF_NEED_RECONNECT;
            mNeedReconnectSsid=new5ssid.isEmpty()?rcwo->mSavedData_5G->mSavedSsid:new5ssid;
            mNeedReconnectPwd=new5pwd.isEmpty()?rcwo->mSavedData_5G->mSavedPwd:new5pwd;
        }
    }

}

void QGenieTimelineLocker::setNeedSingleFinish(bool bneed)
{
    if(bneed)
    {
        mFlags |= TF_NEET_SEND_SINGLEFINISH;
    }
    else
    {
        mFlags &= ~TF_NEET_SEND_SINGLEFINISH;
    }
}

void QGenieTimelineLocker::forceStop()
{
    mEndObj=NULL;
    mEndSLOT=NULL;
    mFlags=0;
    mDelayFinishTime=0;
    mTimeLine.stop();
//    mDelayFinishTimer.stop();
    real_Finish();
}

void QGenieTimelineLocker::whenUpdateUi()
{
    if(mLockerUi)
    {
        mLockerUi->setVisible(!(sRCFlags&RCF_READYSHARE_MODE));
    }
}

void QGenieTimelineLocker::setDelayFinishTime(int secs)
{
    mDelayFinishTime=secs;
}
void QGenieTimelineLocker::slot_Finished()
{
        real_Finish();
}

void QGenieTimelineLocker::real_Finish()
{

    if(mDelayFinishTime > 0)
    {
        QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                ->netgearCommand(INetgearInterface::IC_BLOCKINTERFACE
                                 ,QStringList()<<QString::number(mDelayFinishTime),NULL,NULL);

    }
    mDelayFinishTime=0;


    if(mFlags & TF_NEET_SEND_SINGLEFINISH)
    {
        mFlags &= ~TF_NEET_SEND_SINGLEFINISH;
        QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                ->netgearCommand(INetgearInterface::IC_SEND_SINGLE_FINISH,QStringList(),NULL,NULL);
        GENIE2_LOG("send single finish");  

    }

    slot_ValueChanged(0.0);
    QGenieRCWirelessObject *rcwo=QGenieRCWirelessObject::GetSingletonPtr();
    Q_ASSERT(rcwo && rcwo->mWirelessW);

 //   rcwo->mWirelessW->setEnabled(true);
    GENIE2_SAFEDELETE(mLockerUi);
    mLabel=NULL;
    mPi=NULL;
    if(mEndObj && mEndSLOT)
    {
        QTimer::singleShot(0,mEndObj,mEndSLOT);
    }
	else
	{
        if(sRCFlags &RCF_REMOTE)
        {
            sRCFlags &=~RCF_IN_REMOTE_REFRESHING;
        }
        else
        {
            sRCFlags &=~RCF_IN_LOCAL_REFRESHING;
        }
        rcwo->updateUi();
	}
    sRCFlags &=~RCF_TIMELINELOCKER_WORKING;
    mEndObj=NULL;
    mEndSLOT=NULL;


    Q_ASSERT(mFlags==0);
    QGenieDelayProcessLogic::GetSingleton().whenSettingProcessEnd();

//    GENIE2_SAFEDELETE(mPi);

}

void QGenieTimelineLocker::slot_ValueChanged(qreal v)
{
//    Q_ASSERT(mLabel);
    qreal remain=mTimeLine.duration()*(1.0-v);
    int iremain=(int)(remain/1000);
    if(mLabel)
    {
        mLabel->setText(iremain?mMode.arg(iremain):"");
        if(!iremain)
        {
            mLockerUi->hide();
        }
    }
    if((iremain < NOTIFY_SSID_TIME ) && (mFlags&TF_NEED_NOTIFY_SSID))
    {
        mFlags &=~TF_NEED_NOTIFY_SSID;
        GENIE2_TELL_OTHERPLUGIN(PLUGIN_UUID_WIRELESS,EPCC_SSID_KEY
                                ,QVariantList()<<mNotifySsid<<mNotifyPwd<<mNotifyWpa<<mNotifyRZ);
    }
    if((iremain<RECONNECT_TIME)&&(mFlags&TF_NEED_RECONNECT))
    {
        mFlags &=~TF_NEED_RECONNECT;
        QString curSsid=GENIE2_HANDLER_CMD(HCMD_ASK_SUBSCRIBED_MESSAGE,SCCMD_SSID).toString();
        if(curSsid!=mNeedReconnectSsid)
        {
            GENIE2_TELL_OTHERPLUGIN(PLUGIN_UUID_WIRELESS,EPCC_CONNECT_TO_SSID
                                    ,QStringList()<<mNeedReconnectSsid<<mNeedReconnectPwd);
            setDelayFinishTime(9);
        }
    }


}
