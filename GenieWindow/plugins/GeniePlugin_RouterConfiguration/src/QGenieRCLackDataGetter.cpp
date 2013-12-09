#include "QGenieRCLackDataGetter.h"
#include "QGenieRCWirelessObject.h"
#include "genie2_interface.h"
#include "genie2_netgear.h"
#include "QPiObject.h"
#include "QGenieGaccObject.h"

extern quint64 sRCFlags;
QGenieRCLackDataGetter::QGenieRCLackDataGetter(QGenieRCWirelessObject *rcwo)
    :mRcwo(rcwo)
    ,mSession24(-1)
    ,mSession5(-1)
{

    updatePointers();
}

void QGenieRCLackDataGetter::updatePointers()
{
    if(mRcwo->mSavedData.isNull() || mRcwo->mSavedData_5G.isNull())
    {
        _24Ssid=_24Pwd=_5Ssid=_5Pwd=NULL;
        return;
    }
    _24Ssid=&(mRcwo->mSavedData->mSavedSsid);
    _24Pwd=&(mRcwo->mSavedData->mSavedPwd);
    _5Ssid=&(mRcwo->mSavedData_5G->mSavedSsid);
    _5Pwd=&(mRcwo->mSavedData_5G->mSavedPwd);
}

void QGenieRCLackDataGetter::slot_DataReturned(int session, const QStringList &data)
{
    bool btemp;
    int c=data.count();
    bool soapOk=(c>1 && (0==data[0].toInt(&btemp)) && btemp);

    if(!(sRCFlags & RCF_LOCAL_LOGEDIN))
    {
        return;
    }
    updatePointers();
    Q_ASSERT(_24Ssid&&_24Pwd&&_5Ssid&&_5Pwd);
    if(mSession24==session)
    {
        if(soapOk)
        {
            (*_24Ssid)=(c>1)?data[1]:"";
            (*_24Pwd)=(c>8)?data[8]:"";
            mStatues |= EDS_24OK;
        }
        mSession24=-1;
    }
    else if(mSession5==session)
    {
        if(soapOk)
        {
            (*_5Ssid)=(c>1)?data[1]:"";
            (*_5Pwd)=(c>8)?data[8]:"";
            mStatues |= EDS_5OK;
        }
        mSession5=-1;
    }

    if((mSession5<0) &&(mSession24<0))
    {
        if((mStatues&EDS_24OK)&&(mStatues&EDS_5OK))
        {
            if(mReson==ER_WIRELESS)
            {
                mRcwo->mIndicatorWireless->stopAnimation();
                mRcwo->mStackWC->setCurrentIndex(1);
            }
            else
            {
                QGenieGaccObject *gacco=QGenieGaccObject::GetSingletonPtr();
                Q_ASSERT(gacco);
                gacco->mPi_1->stopAnimation();
                gacco->stack_gacc->setCurrentIndex(1);
            }
        }
        sRCFlags &=~RCF_IN_LOCAL_REFRESHING;
        mRcwo->_updateLockState();
    }
}

bool QGenieRCLackDataGetter::tryUpdateLackedData(ENUM_RESON reson)
{

    mSession24=mSession5=-1;
    if(IS_REMOTE_UI())
    {
        return false;
    }
    mStatues=0;
    mReson=reson;
    updatePointers();
    Q_ASSERT(_24Ssid&&_24Pwd&&_5Ssid&&_5Pwd);
    if((*_24Ssid).isEmpty())
    {
        mSession24 = QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                ->netgearCommand(INetgearInterface::IC_GET_WIRELESS,QStringList(),this,SLOT(slot_DataReturned(int,QStringList)));

    }
    else
    {
        mStatues |= EDS_24OK;
    }
    if((sRCFlags &RCF_LOCAL_WIRELESS_5GUNLOCKED)&&_5Ssid->isEmpty())
    {
        mSession5 = QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                ->netgearCommand(INetgearInterface::IC_GET_GACC,QStringList(),this,SLOT(slot_DataReturned(int,QStringList)));
    }
    else
    {
        mStatues |= EDS_5OK;
    }
    if((mSession24>=0) || (mSession5>=0))
    {
        sRCFlags |=RCF_IN_LOCAL_REFRESHING;
        if(reson==ER_WIRELESS)
        {
            mRcwo->mIndicatorWireless->startAnimation();
        }
        else
        {
            QGenieGaccObject *gacco=QGenieGaccObject::GetSingletonPtr();
            Q_ASSERT(gacco);
            gacco->mPi_1->startAnimation();
        }
        mRcwo->_updateLockState();
        return true;
    }
    return false;
}
