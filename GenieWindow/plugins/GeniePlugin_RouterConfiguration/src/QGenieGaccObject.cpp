#include "QGenieGaccObject.h"
#include <QUiLoader>
#include <QtCore>
#include <QtGui>
#include "genie2_interface.h"
#include "language_RouterConfig.h"
#include "genie2_netgear.h"
#include "QPiObject.h"
#include "QGenieRCWirelessObject.h"
#include "QGenieTimelineLocker.h"
#include "QGenieTimeGenerator.h"
#include "QGenieDelayProcessLogic.h"
#include "SvtFcmlError.h"
#include "QGenieQRCodeManager.h"
#include "QGenieRCLackDataGetter.h"

const int cPiGeometry[]={300,20,40,40};
const int cErrorLabGeometry[]={10,20,400,20};
extern quint64 sRCFlags;

#define DISABLING_LOGIC_INTERVAL (60*60)

#define ERROR_MODE "<center><font color='red'>%1</font>"\
"<a href=\"http://www.netgear.com/landing/en-us/netgear-genie-faq.aspx\"><img src=\":/images/faq.png\" /></a></center>"

QString autoWrap(const QString &s, int limit=34)
{
    if(s.length()<limit)
    {
        return s;
    }
    int ruler=limit*5;
    QString ret=s;
    while(ruler>0)
    {
        if(ret.length()>ruler)
        {
            ret.insert(ruler,"\n");
        }
        ruler-=limit;
    }

    return ret;
}

int convertSeucureString(const QString &para)
{
    QString lower=para.trimmed().toLower();
    if(0 == lower.compare("none",Qt::CaseInsensitive))
    {
        return 0;
    }
    else if((0 == lower.compare("wpa2-psk",Qt::CaseInsensitive)) || (0 == lower.compare("wep64",Qt::CaseInsensitive)) || (0 == lower.compare("wep128",Qt::CaseInsensitive)))
    {
        return 1;
    }
    else if((0 == lower.compare("wpa-psk/wpa2-psk",Qt::CaseInsensitive)) || (0 == lower.compare("wpa-psk",Qt::CaseInsensitive)) || (0 == lower.compare("mixed wpa",Qt::CaseInsensitive)))
    {
        return 2;
    }
    else
    {
        return 1;
    }
}

QGenieGaccDisableingLogic::QGenieGaccDisableingLogic(QObject *parent)
    :QObject(parent)
    ,mSessionCheck24Mac(-1)
    ,mSessionCheckSsidAlive(-1)
    ,mState(STATE_END)
    ,mRetryTimes(1)
    ,mCountDown(-1)
//    ,mCountDownSsid(NULL)
    ,mDbglabel(NULL)
{
    connect(&mTimer,SIGNAL(timeout()),this,SLOT(slot_Timeout()));
    //connect(&mDisablingCheckMacTimer,SIGNAL(timeout()),this,SLOT(slot_DisablingCheckMacTimeout()));
    //connect(&mDisablingCheckGaccTimer,SIGNAL(timeout()),this,SLOT(slot_DisablingCheckGaccTimeout()));
//    LoadMap(mData);
    Q_ASSERT(!mData.contains(""));
    mTimer.setSingleShot(true);
//    mDialogWaitTimer.setSingleShot(true);
//    mDisablingCheckGaccTimer.setSingleShot(true);
#ifdef DBG_GACC_DISABLING_PROCESS
    if(sRCFlags & RCF_DEBUG_GACC_COUNTDOUN)
    {
        mDbglabel=new QLabel();
        mDbglabel->setWordWrap(true);
        //    mDbglabel->show();
        connect(&mDbgTimer,SIGNAL(timeout()),this,SLOT(slot_DbgTimeout()));
        mDbgTimer.start(5000);
    }
#endif
}

QGenieGaccDisableingLogic::~QGenieGaccDisableingLogic()
{
    mTimer.stop();
//    mDisablingCheckMacTimer.stop();
//    mDisablingCheckGaccTimer.stop();

    disconnect(&mTimer,SIGNAL(timeout()),NULL,NULL);
//    disconnect(&mDisablingCheckMacTimer,SIGNAL(timeout()),NULL,NULL);
//    disconnect(&mDisablingCheckGaccTimer,SIGNAL(timeout()),NULL,NULL);
#ifdef DBG_GACC_DISABLING_PROCESS
    if(sRCFlags&RCF_DEBUG_GACC_COUNTDOUN)
    {
        mDbgTimer.stop();
        disconnect(&mDbgTimer,SIGNAL(timeout()),NULL,NULL);
    }
//    GENIE2_SAFEDELETE(mDbglabel);
#endif
   ClearMap(mData);

}

#ifdef DBG_GACC_DISABLING_PROCESS
void QGenieGaccDisableingLogic::slot_DbgTimeout()
{

    if(sRCFlags &RCF_DEBUG_GACC_COUNTDOUN)
    {
        static const char *cStates[]={
                "STATE_INIT"
                ,"STATE_CHECKING_SSID_ALIVE"
                ,"STATE_COUNTDOWN"
                ,"STATE_WAITTO_POPUP"
                ,"STATE_END"
        };
        QString showing=QString("state: ")+cStates[mState];
        showing+=QString("\nmac: ")+mCurMac;
        showing+=QString("\ntimer: ")+(mTimer.isActive()?"Active":"NotActive");

        if(mData.contains(mCurMac))
        {
            QMap<int,QVariant > *data=mData[mCurMac];
            QString _24ssid=data->value(DE_SSID).toString();
            QString _5ssid=data->value(DE_SSID_5G).toString();

            showing+=QString("\nsaved 2.4gSsid: ")+_24ssid+" "+data->value(DE_DEADLINE).toDateTime().toString();
            showing+=QString("\nsaved 5gSsid: ")+_5ssid+" "+data->value(DE_DEADLINE_5G).toDateTime().toString();


        }


        mDbglabel->setText(showing);


    }
//    QString showing;
//    QGenieRCWirelessObject *rcwo=QGenieRCWirelessObject::GetSingletonPtr();
//    Q_ASSERT(rcwo);
//    bool b_enabled=mTimer.isActive();
//    showing +=b_enabled?"enabled":"disabled";
//    const char *cPara[]={"STATE_INIT","STATE_WAIT_TO_CHECKMAC"
//                                ,"STATE_WAIT_TO_CHECKENABLE"
//                                ,"STATE_WAIT_TO_POPDIALOG"
//                            };
//    showing+="\nMac: ";
//    showing+=rcwo->mSavedMac;
//    showing+="\n";
//    showing+=cPara[mState];
//    if(b_enabled)
//    {
//        QDateTime disabletime=mData.value(rcwo->mSavedMac).second;
//        showing+="\ndisabling time: ";
//        showing+=disabletime.toString("dd.MM.yyy hh:mm:ss.zzz");
//        showing+="\nremain: ";
//        showing+=QString::number(QDateTime::currentDateTime().secsTo(disabletime));

//    }
//    mDbglabel->setText(showing);

}

#endif

void QGenieGaccDisableingLogic::whenLogout()
{
    if(!(sRCFlags &RCF_REMOTE))
    {
        tryEnd(true);
        ClearMap(mData);
        mCurMac="";
    }
}

void QGenieGaccDisableingLogic::whenLogIn()
{
    if(!(sRCFlags &RCF_REMOTE))
    {
        start();
        LoadMap(mData);
    }
}



void QGenieGaccDisableingLogic::LoadMap(QMap<QString, QMap<int, QVariant> *> &map)
{
    QString sfname=GENIE2_HANDLER_CMD(HCMD_HOMEDIR_NAME,0).toString()+"gacc.map";
    QFile f(sfname);
    if(f.open(QIODevice::ReadOnly))
    {
        QDataStream s(&f);
        int count;
        s>>count;
        for(int i=0;i<count;i++)
        {
            QString key;
            QMap<int, QVariant> *submap=new QMap<int, QVariant>;
            s>>key>>*(submap);
			qDebug()<<key<<*(submap);
            if(!key.isEmpty())
            {
                map.insert(key,submap);
            }
        }

        f.close();
    }
    else
    {
        GENIE2_LOG("error QGenieGaccDisableingLogic::LoadMap");
    }
}
void QGenieGaccDisableingLogic::ClearMap(QMap<QString, QMap<int, QVariant> *> &map)
{
    QMap<QString, QMap<int, QVariant> *>::Iterator it=map.begin();
    while(it != map.end())
    {
        GENIE2_SAFEDELETE(it.value());
        it++;
    }
    map.clear();
}

void QGenieGaccDisableingLogic::SaveMap(const QMap<QString, QMap<int, QVariant> *> &map)
{
    QString sfname=GENIE2_HANDLER_CMD(HCMD_HOMEDIR_NAME,0).toString()+"gacc.map";
    QFile f(sfname);
    if(f.open(QIODevice::WriteOnly))
    {
        QDataStream s(&f);
        QMap<QString, QMap<int, QVariant> *>::ConstIterator it=map.constBegin();
        s<<map.count();
        while(it != map.constEnd())
        {
			qDebug()<<it.key()<<it.value();
            s<<it.key()<<*(it.value());
            it++;
        }
        s<<map;
        f.close();
    }
    else
    {
        GENIE2_LOG("error QGenieGaccDisableingLogic::SaveMap");
    }
}

void simpleMac(QString &mac)
{
    mac=mac.replace(":","").replace("-","");
}

//when login or refresh wireless settings
void QGenieGaccDisableingLogic::notifyMac(const QString &_mac)
{
    if(mState!=STATE_INIT)
    {
        return;
    }

    QString mac=_mac;
    simpleMac(mac);
    mCurMac=mac;

    if(mData.contains(mac))
    {
        QMap<int,QVariant > *data=mData[mac];
        QString _24ssid=data->value(DE_SSID).toString();
        QString _5ssid=data->value(DE_SSID_5G).toString();
        if(_24ssid.isEmpty()&&_5ssid.isEmpty())
        {
            tryEnd(true);
        }
        else
        {
            mState=STATE_CHECKING_SSID_ALIVE;
            mTimer.setSingleShot(true);
            mTimer.start(0);
        }
    }
	else
	{
		tryEnd(true);
	}

}

void QGenieGaccDisableingLogic::setDisablingTimer(const QString &_mac, const QString ssid, const QDateTime &disable_time, int idx, bool _5G)
{
    QString mac=_mac;
    simpleMac(mac);
    if(!mData.contains(mac))
    {
        mData.insert(mac,new QMap<int ,QVariant>);
    }
    QMap<int,QVariant> *data=mData[mac];
    if(_5G)
    {
        data->insert(DE_COB_IDX_5G,idx);
        data->insert(DE_SSID_5G,ssid);
        data->insert(DE_DEADLINE_5G,disable_time);
    }
    else
    {
        data->insert(DE_COB_IDX,idx);
        data->insert(DE_SSID,ssid);
        data->insert(DE_DEADLINE,disable_time);
    }
    SaveMap(mData);
    tryEnd(true);
}

void QGenieGaccDisableingLogic::RemoveSsid(bool _5G)
{
    if(!mCurMac.isEmpty() && mData.contains(mCurMac))
    {
        QMap<int ,QVariant> *data=mData[mCurMac];
        if(_5G)
        {
            data->insert(DE_SSID_5G,QString());
        }
        else
        {
            data->insert(DE_SSID,QString());
        }
        if(data->value(DE_SSID).toString().isEmpty()&& data->value(DE_SSID_5G).toString().isEmpty())
        {
            mData.remove(mCurMac);
            delete data;
        }
        SaveMap(mData);
    }
}

//void QGenieGaccDisableingLogic::clearMac(const QString &_mac)
//{
//    QString mac=_mac;
//    simpleMac(mac);
//    if(mData.contains(mac))
//    {
//        mData.remove(mac);
//        SaveMap(mData);
//    }
//}

int QGenieGaccDisableingLogic::getSavedIdx(const QString &mac, bool _5g)
{
    QString stemp=mac;
    simpleMac(stemp);
    if(mData.contains(mac))
    {
        if(_5g)
        {
			return mData[mac]->value(DE_COB_IDX_5G).toInt();
        }
        else
        {
			return mData[mac]->value(DE_COB_IDX).toInt();
        }
    }
    return 0;
}

void QGenieGaccDisableingLogic::slot_SoapDataReturned(int session, const QStringList &data)
{
    bool btemp;
    int c=data.count();
    bool soapOk=(c>0)&&(data[0].toInt(&btemp)==0)&&btemp;
    QGenieRCWirelessObject *rcwo=QGenieRCWirelessObject::GetSingletonPtr();
    Q_ASSERT(rcwo);
    if(session==mSessionCheck24Mac)
    {

        if(c>1)
        {
            const QString &mac=data[1];
            if(!mac.isEmpty())
            {
                notifyMac(mac);
            }

            rcwo->mSavedData->mSavedMac=mac;
        }
    }
    else if(session==mSessionCheckSsidAlive)
    {
        if(!soapOk)
        {
            tryEnd(false);
        }
        int _24alive=(c>1)?data[1].toInt():false;
        int _5alive=(c>2)?data[2].toInt():false;
        Q_ASSERT(!rcwo->mSavedData.isNull());
        QString &mac=rcwo->mSavedData->mSavedMac;
        QMap<int ,QVariant> *disabling_data=mData.value(mac);
        QDateTime _24dt,_5dt;

        if(!_24alive)
        {
            disabling_data->insert(DE_SSID,"");
        }
        else
        {
            _24dt=disabling_data->value(DE_DEADLINE).toDateTime();
        }

        if(!_5alive)
        {
            disabling_data->insert(DE_SSID_5G,"");
        }
        else
        {
            _5dt=disabling_data->value(DE_DEADLINE_5G).toDateTime();
        }

        SaveMap(mData);
        if(!_24dt.isValid()&&!_5dt.isValid())
        {
            tryEnd(true);
        }
        else
        {
            mIs5GFirstTimer=(_24dt.isValid()&&_5dt.isValid())?(_24dt>_5dt):(_5dt.isValid());
            QDateTime _dt=mIs5GFirstTimer?_5dt:_24dt;
            Q_ASSERT(!rcwo->mSavedData.isNull()&&!rcwo->mSavedData_5G.isNull());
//            ddddd
//            mCountDownSsid=mIs5GFirstTimer?&(rcwo->mSavedData_5G->mSavedSsid):&(rcwo->mSavedData->mSavedSsid);
            mCountDown=1;
            int seconds=QDateTime::currentDateTime().secsTo(_dt);
            seconds=(seconds<0)?0:seconds;
            while(seconds>DISABLING_LOGIC_INTERVAL)
            {
                seconds=(seconds>>1);
                mCountDown=(mCountDown<<1);
            }
            mTimer.setSingleShot(false);
            mTimer.start(1000*seconds);
            mState=STATE_COUNTDOWN;

        }
    }

};

void QGenieGaccDisableingLogic::slot_Timeout()
{
    if(!(sRCFlags&RCF_LOCAL_LOGEDIN))
    {
        return;
    }
    if(mState==STATE_INIT)
    {
        QGenieRCWirelessObject *rcwo=QGenieRCWirelessObject::GetSingletonPtr();
        Q_ASSERT(rcwo);
        if(!rcwo->mSavedData.isNull() && !rcwo->mSavedData->mSavedMac.isEmpty())
        {
            notifyMac(rcwo->mSavedData->mSavedMac);
        }
        else
        {
            mSessionCheck24Mac=QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                    ->netgearCommand(INetgearInterface::IC_GET_MACADDR,QStringList(),this,SLOT(slot_SoapDataReturned(int,QStringList)));
        }
    }
    else if(mState==STATE_CHECKING_SSID_ALIVE)
    {
        if(mData.contains(mCurMac))
        {
            QMap<int,QVariant > *data=mData[mCurMac];
            QString _24ssid=data->value(DE_SSID).toString();
            QString _5ssid=data->value(DE_SSID_5G).toString();
            mSessionCheckSsidAlive=QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                    ->netgearCommand(INetgearInterface::IC_GET_GACCSSID_ALIVE
                                     ,QStringList()<<_24ssid<<_5ssid
                                     ,this,SLOT(slot_SoapDataReturned(int,QStringList)));
        }
        else
        {
            tryEnd(true);
        }
    }
    else if(mState==STATE_COUNTDOWN)
    {
        if((--mCountDown)<=0)
        {
            mTimer.stop();
            mTimer.setSingleShot(false);
            mTimer.start(15*1000);
            mState=STATE_WAITTO_POPUP;
        }
    }
    else if(mState==STATE_WAITTO_POPUP)
    {
        QGenieRCWirelessObject *rcwo=QGenieRCWirelessObject::GetSingletonPtr();
        Q_ASSERT(rcwo);
        bool bBussy=(sRCFlags & (RCF_IN_LOCAL_REFRESHING|RCF_MODAL_DIALOG_ON));
        if(!bBussy)
        {
            GENIE2_STATISTICS("R9");
            QMap<int,QVariant> *disabling_data=mData.value(mCurMac,NULL);
            if(disabling_data)
            {
                QString countdownSSid=mIs5GFirstTimer?disabling_data->value(DE_SSID_5G).toString()
                                                    :disabling_data->value(DE_SSID).toString();
//            if(mCountDownSsid &&!(*mCountDownSsid).isEmpty())
//            {
                QStringList dialog_lans;
                dialog_lans<<GENIE2_GET_TEXT(L_GACC_BIGBT)<<(GENIE2_GET_TEXT(L_RTCF_MSG_NOTIFYCUT)/*+"\t"+(countdownSSid)*/);
                sRCFlags |= RCF_MODAL_DIALOG_ON;
                bool dialogOk=GENIE2_UI_COMMAND(UICMD_DIALOG_MODAL,dialog_lans).toBool();
                sRCFlags &=~RCF_MODAL_DIALOG_ON;
                if(dialogOk)
                {
                    if(sRCFlags & RCF_LOCAL_LOGEDIN)
                    {
                        if(mIs5GFirstTimer)
                        {
                            sRCFlags |=RCF_MUST_REFRESH_GACC_5G;
                            QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                                    ->netgearCommand(INetgearInterface::IC_SET_5G_GACC,QStringList()<<"0",NULL,NULL);
                        }
                        else
                        {
                            sRCFlags |=RCF_MUST_REFRESH_GACC;
                            QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                                    ->netgearCommand(INetgearInterface::IC_SET_GACC,QStringList()<<"0",NULL,NULL);
                        }

                        QGenieTimeGenerator genner;
                        int t=genner.gaccSettingWaitTime(rcwo->mModel);

                        QGenieTimelineLocker *locker=QGenieTimelineLocker::GetSingletonPtr();
                        Q_ASSERT(locker);
                        locker->setNeedSingleFinish(true);
                        ///////
                        locker->setNeedReconnect();

                        locker->startTimer(t*1000,this,SLOT(slot_SettingProcessEnd())/*,mPi_2*/);
                        RemoveSsid(mIs5GFirstTimer);
                        tryEnd(true);//end it then start ti at slot_SettingProcessEnd()
                        rcwo->updateUi();

                    }
                    else //loged out
                    {
                        tryEnd(true);
                    }

                }
                else  //pop up dialog answer no
                {
                    RemoveSsid(mIs5GFirstTimer);
                    start();
                }
            }
            else //count down ssid not ok
            {
                start();
            }

        }

    }
//    else if(mState == STATE_WAIT_TO_CHECKMAC)
//    {
//        QDateTime current=QDateTime::currentDateTime();
//        if(mData.contains(mCurrentMac) )
//        {
//            if(current > mData[mCurrentMac].second )
//            {
//                //if timeout
//                mSessionCheckMac=QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
//                                 ->netgearCommand(INetgearInterface::IC_GET_WIRELESS,QStringList(),this,SLOT(slot_SoapDataReturned(int,QStringList)));
//            }
//            else
//            {
//                int timer=current.secsTo(mData[mCurrentMac].second)*1000/2;
//                timer=qMin(timer,60000);
//                mTimer.start(timer);
//                GENIE2_LOG(QString("restart timmer to check gacc timout (%1)").arg(timer));
//            }
//        }
//        else
//        {
//            GENIE2_LOG("gacc timeout at STATE_WAIT_TO_CHECKMAC,but macaddress not setted,so never start timmer again");
//        }
//    }
//    else if(mState == STATE_WAIT_TO_CHECKENABLE)
//    {
//        mSessionCheckGacc=QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
//                          ->netgearCommand(INetgearInterface::IC_GET_GACC,QStringList(),this,SLOT(slot_SoapDataReturned(int,QStringList)));
//    }
//    else if(mState ==STATE_WAIT_TO_POPDIALOG)
//    {
//        if(!(sRCFlags & RCF_LOCAL_LOGEDIN)||(sRCFlags & RCF_REMOTE_LOGEDIN))
//        {
//            return;
//        }
//        QGenieRCWirelessObject *rcwo=QGenieRCWirelessObject::GetSingletonPtr();
//        Q_ASSERT(rcwo);
//        bool bBussy=(sRCFlags & RCF_IN_LOCAL_REFRESHING);
//        if(bBussy)
//        {
//            mTimer.start(POPDIALOG_CHECK_INTERVAL);
//        }
//        else
//        {
//            GENIE2_STATISTICS("R9");
//            bool bLogedIn=(rcwo->mStackTotal->currentIndex()==1);
//            if(bLogedIn)
//            {
//            //todo popup dialog
//                QStringList dialog_lans;
//                dialog_lans<<GENIE2_GET_TEXT(L_GACC_BIGBT)<<GENIE2_GET_TEXT(L_RTCF_MSG_NOTIFYCUT);
//                if(GENIE2_UI_COMMAND(UICMD_DIALOG_MODAL,dialog_lans).toBool())
//                {
//                    //todo disable gacc,block ui
//                    if(sRCFlags & RCF_LOCAL_LOGEDIN)
//                    {
//                        QGenieGaccObject *gaccobj=QGenieGaccObject::GetSingletonPtr();
//                        Q_ASSERT(gaccobj);
//                        QRadioButton *rdt_disable=qFindChild<QRadioButton *>(gaccobj->stack_gacc,"rdt_disable");
//                        QFrame *fr_set_down=qFindChild<QFrame *>(gaccobj->mW,"fr_set_down");
//                        QStackedWidget *stack_gacc=qFindChild<QStackedWidget *>(gaccobj->mW,"stack_gacc");
//                        Q_ASSERT(rdt_disable && fr_set_down);
//                        rdt_disable->setChecked(true);
//                        fr_set_down->setVisible(false);
//                        rcwo->mStackTotal->setCurrentWidget(gaccobj->mW);
//                        stack_gacc->setCurrentIndex(1);
////                        gaccobj->mRefreshAfterLogin=false;
//                        sRCFlags &= ~RCF_MUST_REFRESH_GACC;
//                        gaccobj->setGaccInfo();
//                    }
//                }
//                mState=STATE_INIT;
//            }
//            else
//            {
//                GENIE2_LOG("ready to pop dialog ,but rc loged out!!");
//            }

//        }
//    }


}

//void QGenieGaccDisableingLogic::slot_DisablingCheckMacTimeout()
//{

//}


void QGenieGaccDisableingLogic::slot_SettingProcessEnd()
{
    QGenieRCWirelessObject *rcwo=QGenieRCWirelessObject::GetSingletonPtr();
    Q_ASSERT(rcwo);
    sRCFlags &=~RCF_IN_LOCAL_REFRESHING;
    rcwo->tryUpdateData();
    start();
}

void QGenieGaccDisableingLogic::start()
{
    mTimer.stop();
    mTimer.setSingleShot(true);
    mTimer.start(5000);
    mRetryTimes=3;
    mCountDown=0;
    mSessionCheck24Mac=mSessionCheckSsidAlive=-1;
    mState=STATE_INIT;
}

void QGenieGaccDisableingLogic::tryEnd(bool force)
{
    if(force)
    {
        mRetryTimes=0;
    }
    if((--mRetryTimes)<=0)
    {
        mTimer.stop();
        mSessionCheck24Mac=-1;
        mSessionCheckSsidAlive=-1;
        mCountDown=0;
  //      mCountDownSsid=NULL;
        mState=STATE_END;
    }
    else
    {
        mTimer.setSingleShot(true);
        mTimer.start(5000);
    }
}


/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

QGenieGaccObject::tGaccData::tGaccData()
{
    mSavedRdt=NULL;
}

QGenieGaccObject::tGaccData::~tGaccData()
{

}

QGenieGaccObject::QGenieGaccObject(QObject *parent)
    :QObject(parent)
    ,mW(NULL)
    ,mRefreshSession(-1)
    ,mRefreshSession5G(-1)
    //,mPi(NULL)
//    ,mLanEnableState(-1)
    ,mLanError(-1)
//    ,mRdtCurrent(NULL)
	,mPi_1(NULL)
	,mPi_2(NULL)
{
    mDisablingLogic=new QGenieGaccDisableingLogic(this);
}
QGenieGaccObject::~QGenieGaccObject()
{
    mSavedGaccData.clear();
    mSavedGaccData_5G.clear();
}
QWidget *QGenieGaccObject::getGaccUi()
{
    if(!mW)
    {
        bool btemp;
        QUiLoader loader;
        QFile f(GENIE2_RES("ui/gacc.ui"));
        btemp=f.open(QFile::ReadOnly);
        Q_ASSERT(btemp);
        QWidget *w=loader.load(&f,QGenieIface2ShareObject::sSingleInstance->mMainFrame);
        Q_ASSERT(w);
        w->hide();


//        deployIndicators(w);

        QList<QPushButton *> bts=w->findChildren<QPushButton *>();
        int c=bts.count();
        for(int i=0;i<c;i++)
        {
            connect(bts[i],SIGNAL(clicked()),this,SLOT(slot_buttonClicked()));
        }

		
        //mPi=new QProgressIndicator(w);
        //mPi->setGeometry(cPiGeometry[0],cPiGeometry[1],cPiGeometry[2],cPiGeometry[3]);
        //mPi->show();
        //mErrorLabel=new QLabel(w);
        //mErrorLabel->setGeometry(cErrorLabGeometry[0],cErrorLabGeometry[1],cErrorLabGeometry[2],cErrorLabGeometry[3]);
        //mErrorLabel->show();
		deployIndicators(w);
        mErrorLabel1 = w->findChild<QLabel *>("label_info_err1");
        mErrorLabel2 = w->findChild<QLabel *>("label_info_err2");
		Q_ASSERT(mErrorLabel1 && mErrorLabel2);


        mW=w;
        translateGaccUi();

        lab_enable_r=mW->findChild<QLabel *>("lab_enable_r");
        lab_ssid_r=mW->findChild<QLabel *>("lab_ssid_r");
        lab_pwd_r=mW->findChild<QLabel *>("lab_pwd_r");

        lab_enable_l=mW->findChild<QLabel *>("lab_enable_l");
        lab_ssid_l=mW->findChild<QLabel *>("lab_ssid_l");
        lab_pwd_l=mW->findChild<QLabel *>("lab_pwd_l");
        lab_set_period=mW->findChild<QLabel *>("lab_set_period");

        rdt_enable=mW->findChild<QRadioButton *>("rdt_enable");
        rdt_disable=mW->findChild<QRadioButton *>("rdt_disable");

        edt_ssid=mW->findChild<QLineEdit *>("edt_ssid");
        edt_pwd=mW->findChild<QLineEdit *>("edt_pwd");
        cob_time=mW->findChild<QComboBox *>("cob_time");
        rdt_none=mW->findChild<QRadioButton*>("rdt_none");
        rdt_aes=mW->findChild<QRadioButton *>("rdt_aes");
        rdt_mix=mW->findChild<QRadioButton *>("rdt_mix");
        but_modify=mW->findChild<QPushButton *>("but_modify");
        fr_set_down=mW->findChild<QFrame *>("fr_set_down");
        fr_pwd=mW->findChild<QFrame *>("fr_pwd");
        but_apply=mW->findChild<QPushButton *>("but_apply");



        stack_gacc=mW->findChild<QStackedWidget*>("stack_gacc");
        Q_ASSERT(lab_enable_r && lab_ssid_r && lab_pwd_r &&
                 lab_enable_l && lab_ssid_l && lab_pwd_l
                 && rdt_enable
                 && rdt_disable && edt_ssid && cob_time && rdt_none
                 && rdt_aes && rdt_mix && but_modify && stack_gacc
                 && fr_set_down && but_apply && edt_pwd
                 && fr_pwd &&lab_set_period);
        connect(edt_ssid,SIGNAL(textChanged(QString)),this,SLOT(slot_UpdateButEnableState()));
        connect(edt_pwd,SIGNAL(textChanged(QString)),this,SLOT(slot_UpdateButEnableState()));

        connect(rdt_none,SIGNAL(clicked()),this,SLOT(slot_UpdateButEnableState()));
        connect(rdt_aes,SIGNAL(clicked()),this,SLOT(slot_UpdateButEnableState()));
        connect(rdt_mix,SIGNAL(clicked()),this,SLOT(slot_UpdateButEnableState()));
        connect(rdt_disable,SIGNAL(clicked()),this,SLOT(slot_UpdateButEnableState()));
        connect(rdt_enable,SIGNAL(clicked()),this,SLOT(slot_UpdateButEnableState()));
        connect(cob_time,SIGNAL(currentIndexChanged(int)),this,SLOT(slot_UpdateButEnableState()));
#ifdef USE_5GFUNC
        QGenieRCWirelessObject *rcwo=QGenieRCWirelessObject::GetSingletonPtr();
        Q_ASSERT(rcwo);
        connect(stack_gacc,SIGNAL(currentChanged(int)),rcwo,SLOT(slot_Refresh5GUiWhenModifyModeChanged()));
#endif
    }
    return mW;
}

void QGenieGaccObject::translateGaccUi()
{
    if(!mW)
    {
        return;
    }
    //0.label   1.button    2.checkbox  3.radiobutton
    const struct _{const char *name;int lan;int type;}
    cPara[]={
        {"lab_enable_l", L_GACC_ENABLE_TITLE, 0}
        ,{"lab_ssid_l",L_GACC_GUESTWIRELESS_SSID,0}
        ,{"lab_pwd_l",L_RTCF_PASSWORD,0}
        ,{"lab_set_ssid",L_GACC_GUESTWIRELESS_SSID,0}
        ,{"lab_set_period",L_GACC_WORKTIME,0}
        ,{"lab_set_secure",L_RTCF_SETTING_SECURITY,0}
        ,{"rdt_enable",L_GACC_ENABLEGUEST,3}
        ,{"rdt_disable",L_GACC_DISABLEGUEST,3}
        ,{"but_refresh",L_RTCF_INFO_REFRESH,1}
        ,{"but_modify",L_RTCF_INFO_MODIFY,1}
        ,{"but_logout",L_RTCF_INFO_LOGINOUT,1}
        ,{"but_apply",L_RTCF_SETTING_APPLY,1}
        ,{"but_cancel",L_RTCF_CANCEL,1}
        ,{"lab_set_pwd",L_RTCF_INFO_PASSWOED,0}
		,{"label_note", L_RTCF_SETTING_NOTE, 0}
		,{"but_refresh2", L_RTCF_INFO_REFRESH, 1}
    };
    for(int i=0;i<sizeof(cPara)/sizeof(cPara[0]);i++)
    {
        if(0==cPara[i].type)
        {
            QLabel *lab=mW->findChild<QLabel *>(cPara[i].name);
            Q_ASSERT(lab);
            lab->setText(GENIE2_GET_TEXT(cPara[i].lan));
        }
        if(1==cPara[i].type)
        {
            QPushButton *but=mW->findChild<QPushButton *>(cPara[i].name);
            Q_ASSERT(but);
            but->setText(GENIE2_GET_TEXT(cPara[i].lan));
        }
        if(3==cPara[i].type)
        {
            QRadioButton *rdt=mW->findChild<QRadioButton *>(cPara[i].name);
            Q_ASSERT(rdt);
            rdt->setText(GENIE2_GET_TEXT(cPara[i].lan));
        }
    }

    QLabel *lab_enable_r=mW->findChild<QLabel *>("lab_enable_r");
    Q_ASSERT(lab_enable_r);
    QSharedPointer<tGaccData> data=IS_GACC_5G()?mSavedGaccData_5G:mSavedGaccData;

    int lan_enable=(!data.isNull() &&(data->mSavedEnable))?L_GACC_ENABLED:L_GACC_NOTENABLED;
    lab_enable_r->setText(GENIE2_GET_TEXT(lan_enable));
//    lab_enable_r->setText((!data.isNull()&&(data->mSavedEnable))?GENIE2_GET_TEXT(mLanEnableState):"N/A");

    Q_ASSERT(mErrorLabel1 && mErrorLabel2);
    mErrorLabel1->setText((mLanError<0)?"":QString(ERROR_MODE).arg(GENIE2_GET_TEXT(mLanError)));
	mErrorLabel2->setText((mLanError<0)?"":QString(ERROR_MODE).arg(GENIE2_GET_TEXT(mLanError)));

    QComboBox *cob_time=mW->findChild<QComboBox *>("cob_time");
    Q_ASSERT(cob_time);
    int itemp=cob_time->currentIndex();

    cob_time->clear();
    cob_time->insertItems(0,QStringList()
                          <<GENIE2_GET_TEXT(L_GACC_ALWAYS)
                          <<GENIE2_GET_TEXT(L_GACC_1HOUR)
                          <<GENIE2_GET_TEXT(L_GACC_5HOUR)
                          <<GENIE2_GET_TEXT(L_GACC_10HOUR)
                          <<GENIE2_GET_TEXT(L_GACC_1DAY)
                          <<GENIE2_GET_TEXT(L_GACC_1WEEK)
                          );
    itemp=(itemp >=0&& itemp < cob_time->count())?itemp:0;

    cob_time->setCurrentIndex(itemp);
}

//void QGenieGaccObject::deployIndicators(QWidget *w)
//{

//}
void QGenieGaccObject::slot_RefreshGaccInfo()
{
    Q_ASSERT(stack_gacc);
    stack_gacc->setCurrentIndex(0);
    if(IS_GACC_5G())
    {
        sRCFlags &= ~RCF_MUST_REFRESH_GACC_5G;
        refreshGaccInfo(sRCFlags &RCF_REMOTE,true);
    }
    else
    {
        sRCFlags &= ~RCF_MUST_REFRESH_GACC;
        refreshGaccInfo(sRCFlags &RCF_REMOTE,false);
    }
    mDisablingLogic->start();
}

void QGenieGaccObject::refreshGaccInfo(bool remote,bool _5G)
{
	Q_ASSERT(mPi_1 && mPi_2);

    mPi_1->startAnimation();
	mPi_2->startAnimation();
    setError(-1);
    QGenieRCWirelessObject *rcwo=QGenieRCWirelessObject::GetSingletonPtr();
    Q_ASSERT(rcwo);
    sRCFlags |=(remote?RCF_IN_REMOTE_REFRESHING:RCF_IN_LOCAL_REFRESHING);
    rcwo->mWirelessW->setEnabled(false);
    QString na="N/A";
    lab_enable_r->setText(na);
    lab_ssid_r->setText(na);
    lab_pwd_r->setText(na);
    edt_pwd->setText(na);
    edt_ssid->setText(na);
    mRefreshSession=-1;
    mRefreshSession5G=-1;
    sRCFlags &=~(_5G?RCF_MUST_REFRESH_GACC_5G:RCF_MUST_REFRESH_GACC);
    if(remote)
    {
        if(_5G)
        {
            mRefreshSession5G=QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                    ->netgearCommand(INetgearInterface::IC_SM_GET_5GGACC,QStringList()<<rcwo->getCpName()
                                     ,this,SLOT(slot_SMDataRreturned(int,QStringList)));

        }
        else
        {
            mRefreshSession=QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                    ->netgearCommand(INetgearInterface::IC_SM_GACC,QStringList()<<rcwo->getCpName()
                                     ,this,SLOT(slot_SMDataRreturned(int,QStringList)));
        }
    }
    else
    {
        if(_5G)
        {
            mRefreshSession5G=QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                    ->netgearCommand(INetgearInterface::IC_GET_5G_GACC,QStringList()
                                     ,this,SLOT(slot_DataRreturned(int,QStringList)));
        }
        else
        {
            mRefreshSession=QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                    ->netgearCommand(INetgearInterface::IC_GET_GACC,QStringList()
                                     ,this,SLOT(slot_DataRreturned(int,QStringList)));
        }

    }
    QGenieQRCodeManager *qrcm=QGenieQRCodeManager::GetSingletonPtr();
    Q_ASSERT(qrcm);
    qrcm->hideWidget(QGenieQRCodeManager::QWT_GACC);
}

void QGenieGaccObject::setGaccInfo()
{
    Q_ASSERT(stack_gacc);
    QGenieRCWirelessObject *rcwo=QGenieRCWirelessObject::GetSingletonPtr();
    Q_ASSERT(rcwo);

    if(rcwo->mStackTotal->currentIndex()==0)
    {
        //if not logedin
        return;
    }

    stack_gacc->setCurrentIndex(1);
    rcwo->mTab->setCurrentWidget(mW);

    QString ssid=edt_ssid->text();
    QString pwd=(rdt_none->isChecked())?"":(edt_pwd->text());
    QString secure=(rdt_none->isChecked())?"None":
                   (rdt_aes->isChecked()?"WPA2-PSK":"WPA-PSK/WPA2-PSK");

    QSharedPointer<tGaccData> data=IS_GACC_5G()?mSavedGaccData_5G:mSavedGaccData;
    QStringList paras;
    if(rdt_enable->isChecked())
    {
        if(!data->mSavedEnable)
        {
            GENIE2_STATISTICS("R7");
        }
        paras<<"1"<<ssid<<secure<<pwd;
        int idx=cob_time->currentIndex();
        if(idx >0 && !IS_REMOTE_UI())
        {
            const int onehour=60*60;
            //always,1 hour, 5 hour , 10 hour ,1 day, 1week
            const int cPara[]={0,onehour,5*onehour,10*onehour,24*onehour,7*24*onehour};
            Q_ASSERT(idx < (sizeof(cPara)/sizeof(cPara[0])));
//            Q_ASSERT(!rcwo->mSavedMac.isEmpty());
            QGenieGaccDisableingLogic::GetSingleton()
                    .setDisablingTimer(rcwo->mSavedData->mSavedMac,ssid
                                       ,QDateTime::currentDateTime().addSecs((sRCFlags&RCF_DEBUG_GACC_COUNTDOUN)?(5*60):cPara[idx])
                                       ,idx,IS_GACC_5G());
        }
    }
    else
    {
        GENIE2_STATISTICS("R8");
//        QGenieGaccDisableingLogic::GetSingleton().clearMac(rcwo->mSavedMac);
        paras<<"0";
    }
    if(IS_REMOTE_UI())
    {
        paras.insert(0,rcwo->getCpName());
        if(IS_GACC_5G())
        {
            QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                    ->netgearCommand(INetgearInterface::IC_SM_SET_5GGACC,paras,NULL,NULL);
        }
        else
        {
            QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                    ->netgearCommand(INetgearInterface::IC_SM_SET_GACC,paras,NULL,NULL);
        }

    }
    else
    {
        if(IS_GACC_5G())
        {
            QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                    ->netgearCommand(INetgearInterface::IC_SET_5G_GACC,paras,NULL,NULL);
        }
        else
        {
            QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                    ->netgearCommand(INetgearInterface::IC_SET_GACC,paras,NULL,NULL);
        }

    }

    QGenieTimeGenerator genner;
    int t=IS_REMOTE_UI()?SMARTNETWORK_WAIT_TIME:genner.gaccSettingWaitTime(rcwo->mModel);

    //set disabling data to hard disk




    QGenieTimelineLocker *locker=QGenieTimelineLocker::GetSingletonPtr();
    Q_ASSERT(locker);
    locker->setNeedSingleFinish(!IS_REMOTE_UI());
    if(!IS_REMOTE_UI())
    {
//        locker->setNeedReconnect(rcwo->mSavedSsid,rcwo->mSavedPwd);
    }
    else
    {
        locker->setNeedReconnect();
    }
    locker->startTimer(t*1000,this,SLOT(slot_RefreshGaccInfo())/*,mPi_2*/);
}

void QGenieGaccObject::whenLogin()
{
    stack_gacc->setCurrentIndex(0);
    QString na="N/A";
    lab_enable_r->setText(na);
    lab_ssid_r->setText(na);
    lab_pwd_r->setText(na);
    if(mSavedGaccData.isNull())
    {
        mSavedGaccData=QSharedPointer<tGaccData>(new tGaccData);
    }
    if(mSavedGaccData_5G.isNull())
    {
        mSavedGaccData_5G=QSharedPointer<tGaccData>(new tGaccData);
    }
    mDisablingLogic->whenLogIn();
//    mDataOk=false;
//    mRefreshAfterLogin=true;
}

void QGenieGaccObject::setError(int lan)
{
    mLanError=lan;
    Q_ASSERT(mErrorLabel1 && mErrorLabel2);
    QString s=(mLanError<0)?"":QString(ERROR_MODE).arg(GENIE2_GET_TEXT(mLanError));
	mErrorLabel2->setText(s);
    mErrorLabel1->setText(s);
}

void QGenieGaccObject::updateShowingPage()
{
    QSharedPointer<tGaccData> data=IS_GACC_5G()?mSavedGaccData_5G:mSavedGaccData;
    if(!data.isNull())
    {
        Q_ASSERT(lab_enable_r && lab_ssid_r && lab_pwd_r &&
                 lab_enable_l && lab_ssid_l && lab_pwd_l
                 && rdt_enable
                 && rdt_disable && edt_ssid && cob_time && rdt_none
                 && rdt_aes && rdt_mix && but_modify && stack_gacc
                 && fr_set_down && edt_pwd);
        bool b_enable=data->mSavedEnable;
        lab_ssid_l->setVisible(b_enable);
        lab_ssid_r->setVisible(b_enable);
        lab_pwd_l->setVisible(b_enable);
        lab_pwd_r->setVisible(b_enable);

//        if(
//                !(IS_REMOTE_UI()&&(sRCFlags & RCF_IN_REMOTE_REFRESHING))
//                &&!(!IS_REMOTE_UI()&&(sRCFlags & RCF_IN_LOCAL_REFRESHING))

//                )
//        {

            int lan_enable=b_enable?L_GACC_ENABLED:L_GACC_NOTENABLED;
            lab_enable_r->setText(GENIE2_GET_TEXT(lan_enable));

            lab_ssid_r->setText(data->mSavedSsid);
            //                edt_ssid->setText((c>2)?lst[2]:"......");
            lab_pwd_r->setText(data->mSavedPwd);
//        }

//        fr_pwd->setVisible(rdt_none!=data->mSavedRdt);
            bool dataok=false;
            if(IS_GACC_5G())
            {
                dataok=!(sRCFlags&RCF_MUST_REFRESH_GACC_5G);
                but_modify->setEnabled(dataok);
            }
            else
            {
                dataok=!(sRCFlags&RCF_MUST_REFRESH_GACC);
                but_modify->setEnabled(dataok);
            }
            QGenieQRCodeManager *qrcm=QGenieQRCodeManager::GetSingletonPtr();
            Q_ASSERT(qrcm);
            if(dataok && b_enable)
            {
                qrcm->setData(data->mSavedSsid,data->mSavedPwd,QGenieQRCodeManager::QWT_GACC);
            }
            else
            {
                qrcm->hideWidget(QGenieQRCodeManager::QWT_GACC);
            }
    }

}

void QGenieGaccObject::updateSettingPage()
{
    mCobTimeChanged=false;
    QGenieRCWirelessObject *rcwo=QGenieRCWirelessObject::GetSingletonPtr();
    Q_ASSERT(rcwo);

    if(sRCFlags & RCF_REMOTE_LOGEDIN)
    {
        cob_time->setVisible(false);
//        cob_time->setEnabled(false);
//        cob_time->setCurrentIndex(0);
        lab_set_period->setVisible(false);
    }
    else
    {
        QSharedPointer<QGenieRCWirelessObject::tRCData> data=rcwo->mSavedData;
        Q_ASSERT(!data.isNull());
        int itemp=mDisablingLogic->getSavedIdx(data->mSavedMac,IS_GACC_5G());
        cob_time->setVisible(true);
        cob_time->setEnabled(true);
        cob_time->setCurrentIndex(itemp);
        lab_set_period->setVisible(true);
    }
    QSharedPointer<tGaccData> data=IS_GACC_5G()?mSavedGaccData_5G:mSavedGaccData;
    fr_set_down->setVisible(data->mSavedEnable);
    rdt_enable->setChecked(data->mSavedEnable);
    rdt_disable->setChecked(!data->mSavedEnable);
    but_apply->setEnabled(false);
    edt_ssid->setText(lab_ssid_r->text());
    edt_pwd->setText(lab_pwd_r->text());
    edt_ssid->setCursorPosition(0);
    edt_pwd->setCursorPosition(0);
    if(data->mSavedRdt)
    {
        data->mSavedRdt->setChecked(true);
    }
    fr_pwd->setVisible(!rdt_none->isChecked());
}

//void QGenieGaccObject::whenRaise()
//{

//    if(sRCFlags & RCF_MUST_REFRESH_GACC)
//    {
//        slot_RefreshGaccInfo();
//    }

//}

void QGenieGaccObject::whenLogout()
{
    mRefreshSession=-1;
    mRefreshSession5G=-1;
    mDisablingLogic->whenLogout();
    if(!(sRCFlags&(RCF_REMOTE_LOGEDIN|RCF_LOCAL_LOGEDIN)))
    {
        mSavedGaccData.clear();
        mSavedGaccData_5G.clear();
    }
}

void QGenieGaccObject::slot_DataRreturned(int session, const QStringList &lst)
{
    QGenieRCWirelessObject *rcwo=QGenieRCWirelessObject::GetSingletonPtr();
    Q_ASSERT(rcwo);
    if(mRefreshSession==session||mRefreshSession5G==session)
    {
        but_apply->setEnabled(false);
        mPi_1->stopAnimation();
        mPi_2->stopAnimation();
        bool btemp;
        int c=lst.count();
        bool soapOk=(c>1 && (0==lst[0].toInt(&btemp)) && btemp);

        Q_ASSERT(lab_enable_r && lab_ssid_r && lab_pwd_r &&
                 lab_enable_l && lab_ssid_l && lab_pwd_l
                 && rdt_enable
                 && rdt_disable && edt_ssid && cob_time && rdt_none
                 && rdt_aes && rdt_mix && but_modify && stack_gacc
                 && fr_set_down && edt_pwd);
		
        but_modify->setEnabled(soapOk);
        QSharedPointer<tGaccData> data=(mRefreshSession5G==session)?mSavedGaccData_5G:mSavedGaccData;
        if(soapOk)
        {
            bool b_enabled=(lst[1].toInt()==1);
            data->mSavedEnable=b_enabled;

            data->mSavedSsid=(c>2)?lst[2]:"";
            data->mSavedPwd=(c>4)?autoWrap(lst[4]):"";


            if(c >3)
            {
                const QString &secure_mode =lst[3];
                int itemp=convertSeucureString(secure_mode);
                QRadioButton *tempArray[]={rdt_none,rdt_aes,rdt_mix};
                data->mSavedRdt=tempArray[itemp];
            }
            else
            {
                data->mSavedRdt=NULL;
            }
//            fr_pwd->setVisible(!rdt_none->isChecked());

//            but_modify->setEnabled(true);
            if(b_enabled )
            {
                //start a timmer to diable gacc
//                QGenieGaccDisableingLogic::GetSingleton().notifyMac(rcwo->mSavedMac);
            }
            updateShowingPage();
            updateSettingPage();
            setError(-1);
        }
        else //soap not Ok
        {
            if(session==mRefreshSession) 
            {
                sRCFlags |= RCF_MUST_REFRESH_GACC;
            }
            else
            {
                sRCFlags |= RCF_MUST_REFRESH_GACC_5G;
            }
            stack_gacc->setCurrentIndex(0);
            int itemp;

            if(c>0
               && ((itemp=lst[0].toInt(&btemp))/100==4 || itemp/100==5)
               && btemp)
            {
                setError(L_RTCT_MSG_GACC_NOTSUPPORTED);
            }
            else
            {
                setError(L_RTCF_MSG_TIMEOUT);
            }
        }

        sRCFlags &= ~RCF_IN_LOCAL_REFRESHING;
        rcwo->mWirelessW->setEnabled(true);
//        QGenieDelayProcessLogic::GetSingleton().whenGetGaccLocalInfoEnd(soapOk);
		if(soapOk)
		{
			rcwo->updateUi();
		}
    }
	
  
}

void QGenieGaccObject::slot_SMDataRreturned(int session, const QStringList &lst)
{
    QGenieRCWirelessObject *rcwo=QGenieRCWirelessObject::GetSingletonPtr();
    Q_ASSERT(rcwo);
//    QStackedWidget *stack=qFindChild<QStackedWidget *>(mWirelessW,"stack_total");
    bool btemp=false;
    int length=lst.count();
    int rvalue=(length>0)?lst[0].toInt(&btemp):-1;
    bool requestOk=((0==rvalue) && btemp);
    bool wantReLoginCP=((FE_WRONG_USER_OR_PWD==rvalue) && btemp);
    bool wantReAuthen=((FE_NOT_AUTHENTICATIED==rvalue)&&btemp);
    if(requestOk)
    {
        QTellLPCHelper::GetSingleton().tryTellLPC();
    }

    if(wantReAuthen)
    {
        rcwo->setLoginError(L_RTCF_MSG_PLEASERELOGIN,true);
        rcwo->logOut(true);
        rcwo->updateUi();
        return;
    }
    if(wantReLoginCP )
    {
        if(rcwo->tryReloginCP())
        {
            if(session==mRefreshSession)
            {
                mRefreshSession=QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                        ->netgearCommand(INetgearInterface::IC_SM_GACC
                                         ,QStringList()<<rcwo->getCpName(),this
                                         ,SLOT(slot_SMDataRreturned(int,QStringList)));
            }
            else if(session==mRefreshSession5G)
            {
                mRefreshSession5G=QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                        ->netgearCommand(INetgearInterface::IC_SM_GET_5GGACC
                                         ,QStringList()<<rcwo->getCpName(),this
                                         ,SLOT(slot_SMDataRreturned(int,QStringList)));
            }
        }
        else
        {
            if(session==mRefreshSession ||session==mRefreshSession5G)
            {
                if(session==mRefreshSession)
                {
                    sRCFlags|=RCF_MUST_REFRESH_GACC;
                }
                else
                {
                    sRCFlags|=RCF_MUST_REFRESH_GACC_5G;
                }
                sRCFlags &= ~RCF_IN_REMOTE_REFRESHING;
                but_modify->setEnabled(false);
                mPi_1->stopAnimation();
                mPi_2->stopAnimation();
            }

        }
        rcwo->updateUi();
        return;
    }











    if(mRefreshSession==session || mRefreshSession5G==session)
    {
        but_apply->setEnabled(false);
        mPi_1->stopAnimation();
        mPi_2->stopAnimation();
        bool btemp;
        int c=lst.count();

        Q_ASSERT(lab_enable_r && lab_ssid_r && lab_pwd_r &&
                 lab_enable_l && lab_ssid_l && lab_pwd_l
                 && rdt_enable
                 && rdt_disable && edt_ssid && cob_time && rdt_none
                 && rdt_aes && rdt_mix && but_modify && stack_gacc
                 && fr_set_down && edt_pwd);

        but_modify->setEnabled(requestOk);
        QSharedPointer<tGaccData> data=(mRefreshSession5G==session)?mSavedGaccData_5G:mSavedGaccData;
        if(requestOk)
        {
            bool b_enabled=(lst[1].toInt()==1);
            data->mSavedEnable=b_enabled;




            data->mSavedSsid=(c>2)?lst[2]:"";
            data->mSavedPwd=(c>4)?autoWrap(lst[4]):"";




            if(c >3)
            {
                const QString &secure_mode =lst[3];
                int itemp=convertSeucureString(secure_mode);
                QRadioButton *tempArray[]={rdt_none,rdt_aes,rdt_mix};
                data->mSavedRdt=tempArray[itemp];
            }
            else
            {
                data->mSavedRdt=NULL;
            }


//            but_modify->setEnabled(true);
//            if(b_enabled )
//            {
//                //start a timmer to diable gacc
//                QGenieGaccDisableingLogic::GetSingleton().notifyMac(rcwo->mSavedMac);
//            }
            updateShowingPage();
            updateSettingPage();
            setError(-1);
			rcwo->tryUpdateFirmware();
        }
        else //soap not Ok
        {
            if(session==mRefreshSession)
            {
                sRCFlags |= RCF_MUST_REFRESH_GACC;
            }
            else
            {
                sRCFlags |= RCF_MUST_REFRESH_GACC_5G;
            }
            stack_gacc->setCurrentIndex(0);
            int itemp;

            if(c>0
               && ((itemp=lst[0].toInt(&btemp))/100==4 || itemp/100==5)
               && btemp)
            {
                setError(L_RTCT_MSG_GACC_NOTSUPPORTED);
            }
            else
            {
                setError(L_RTCF_MSG_SMLOGIN_ERROR);
            }
			rcwo->mWirelessW->setEnabled(true);///kai.yan modify remote get wireless info timeout
        }

        sRCFlags &= ~RCF_IN_REMOTE_REFRESHING;

//        rcwo->mWirelessW->setEnabled(true);
//        QGenieDelayProcessLogic::GetSingleton().whenGetGaccInfoEnd(requestOk);
    }
	if(requestOk)
	{
		rcwo->updateUi();
	}
}

void QGenieGaccObject::slot_buttonClicked()
{
    QPushButton *but=qobject_cast<QPushButton *>(sender());
    Q_ASSERT(but);
    QGenieRCWirelessObject *rcwo=QGenieRCWirelessObject::GetSingletonPtr();
    Q_ASSERT(rcwo);
    QString objname=but->objectName();
    if(objname=="but_cancel")
    {
        stack_gacc->setCurrentIndex(0);
    }
    else if(objname=="but_refresh" || objname=="but_refresh2")
    {
        refreshGaccInfo(IS_REMOTE_UI(),IS_GACC_5G());
    }
    else if(objname=="but_modify")
    {

        updateSettingPage();
        QGenieRCLackDataGetter *ldgetter=QGenieRCLackDataGetter::GetSingletonPtr();
        Q_ASSERT(ldgetter);
        if(!ldgetter->tryUpdateLackedData(QGenieRCLackDataGetter::ER_GACC))
        {
            stack_gacc->setCurrentIndex(1);
        }
    }
    else if(objname=="but_apply")
    {
        QStringList dialog_lans;
        dialog_lans<<GENIE2_GET_TEXT(L_GACC_BIGBT)<<GENIE2_GET_TEXT(L_RTCF_MSG_NOTIFYCUT);
        sRCFlags |= RCF_MODAL_DIALOG_ON;
        bool dialogOk=GENIE2_UI_COMMAND(UICMD_DIALOG_MODAL,dialog_lans).toBool();
        sRCFlags &=~RCF_MODAL_DIALOG_ON;
        if(!dialogOk)
        {
            return;
        }
        setGaccInfo();
    }
    else if(objname=="but_logout")
    {
        rcwo->logOut(IS_REMOTE_UI());
    }
    rcwo->updateUi();

}

void QGenieGaccObject::slot_UpdateButEnableState()
{
    mCobTimeChanged=mCobTimeChanged||(cob_time->currentIndex()!=0);

    QSharedPointer<tGaccData> data=IS_GACC_5G()?mSavedGaccData_5G:mSavedGaccData;
    if(data.isNull())
    {
        return;
    }
    bool b_changed=(!data.isNull() && data->mSavedRdt && !data->mSavedRdt->isChecked())
                   ||(edt_ssid->text()!=lab_ssid_r->text())
                   ||(edt_pwd->text()!=lab_pwd_r->text()
                      ||mCobTimeChanged);
    b_changed=b_changed ||(data->mSavedRdt && !data->mSavedRdt->isChecked());
    bool text_valid=((edt_pwd->text().length()>=8 ||rdt_none->isChecked())&& !edt_ssid->text().isEmpty());

    bool b_applybut_enable=(
                            (data->mSavedEnable && rdt_disable->isChecked())
                            ||(!data->mSavedEnable && rdt_enable->isChecked() && text_valid)
                            ||(data->mSavedEnable && rdt_enable->isChecked() && text_valid && b_changed)
                            );
    but_apply->setEnabled(b_applybut_enable);


	////////2012-3-7 chensi/////////////
	fr_pwd->setVisible(!rdt_none->isChecked());
}



void QGenieGaccObject::deployIndicators(QWidget *w)
{
	Q_ASSERT(w);
    const struct _{const char *name;QPiObject**p;}
    cParas[]={
        {"fr_indicator1",&mPi_1}
        ,{"fr_indicator2",&mPi_2}
    };
    for(int i=0;i<sizeof(cParas)/sizeof(cParas[0]);i++)
    {
        Q_ASSERT(*(cParas[i].p)==NULL);
        *(cParas[i].p)=new QPiObject(w,cParas[i].name);

    }
}
