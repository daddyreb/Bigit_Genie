
#include <QtGui>
#include <QUiLoader>
#include "GeniePlugin_RouterConfiguration.h"
#include "version.h"
#include "genie2_interface.h"
#include "genie2_netgear.h"
#include "QGenieRCWirelessObject.h"
#include "QGenieGaccObject.h"
#ifdef USE_READYSHARE
#include "QGenieRSManager.h"
#include "QGenieRSBasic.h"
#include "QGenieRSPrint.h"
#include "QGenieRSRemote.h"
#endif
#ifdef USE_TRAFFICMETER
#include "QGenieTrafficMeterObject.h"
#endif
#include "QGenieDelayProcessLogic.h"
#include "QGenieTrafficMeterObject.h"
#include "QGenieTimelineLocker.h"
#include "QGenieFupdateObject.h"
#include "QGenieRCTopologyObject.h"

#ifdef USE_QOSFUNC
#include "QGenieQosObject.h"
#endif
#include "QGenieRebootObject.h"

extern quint64 sRCFlags;

GeniePlugin_RouterConfiguration::GeniePlugin_RouterConfiguration()
{ 
	if(qApp->arguments().contains("-dbg"))
    {
        sRCFlags |= RCF_DEBUG_GACC_COUNTDOUN;
    }
    mObj=new QGenieRCWirelessObject();
    mGaccObj=new QGenieGaccObject();
    mDelayProcessLogic=new QGenieDelayProcessLogic;
    mTMObj=new QGenieTrafficMeterObject();
    mFupObj=new QGenieFupdateObject();
    mTpObj=new QGenieRCTopologyObject();
	mQosObj = new QGenieQosObject();
    mRebootObj = new QGenieRebootObject();

}

GeniePlugin_RouterConfiguration::~GeniePlugin_RouterConfiguration()
{
    GENIE2_SAFEDELETE(mObj);
    GENIE2_SAFEDELETE(mGaccObj);
    GENIE2_SAFEDELETE(mDelayProcessLogic);
    GENIE2_SAFEDELETE(mTMObj);
    GENIE2_SAFEDELETE(mFupObj);
    GENIE2_SAFEDELETE(mTpObj);
	GENIE2_SAFEDELETE(mQosObj);
    GENIE2_SAFEDELETE(mRebootObj);
}

QVariant GeniePlugin_RouterConfiguration::command(int cmd,const QVariant &paras)
{
	Q_UNUSED(paras)
//    HANDLER_CMD(0,QString());
    if(cmd ==I2CMD_UUID)
    {
        return QString(PLUGIN_UUID);
    }
    else if(cmd==I2CMD_VERSIONNUMBER)
    {
        return QString(VERSION_STRING);
    }
    // else if(cmd==I2CMD_CREATE_MAINFRAME)
    // {
        // mUiObj->createMainFrame();

    // }
    else if(cmd==I2CMD_TIMETO_CREATEUI)
    {
#ifdef USE_WIRELESS_PARENT_UI
        QWidget *w=mObj->getWirelessUi()->parentWidget();
#else
        QWidget *w=mObj->getWirelessUi();
#endif
        GENIE2_REGISTER_UI(w,"1500:1500:Router_Configuration:cmd1");
        GENIE2_REGISTER_UI(w,"ReadySHARE:1700:ReadySHARE:readyshare");
        GENIE2_SUBSCRIB_MESSAGE(SCCMD_LANGUAGE);
        GENIE2_SUBSCRIB_MESSAGE(SCCMD_ROUTER_MODEL);
        GENIE2_SUBSCRIB_MESSAGE(SCCMD_CONNECTION_STATE);
        GENIE2_SUBSCRIB_MESSAGE(SCCMD_CURRENTSETTING);

        GENIE2_SUBSCRIB_MESSAGE(SCCMD_FIRMWARE);
        QGenieGaccDisableingLogic *gacc_dlogic=QGenieGaccDisableingLogic::GetSingletonPtr();
        Q_ASSERT(gacc_dlogic);
#ifdef DBG_GACC_DISABLING_PROCESS
        if(sRCFlags & RCF_DEBUG_GACC_COUNTDOUN)
        {
            GENIE2_REGISTER_DBG_UI(gacc_dlogic->mDbglabel);
        }
#endif

                //////
    }
    else if(cmd==I2CMD_NOTIFY_MESSAGE)
    {

        QVariantList varParas = paras.toList();
        //int nCount = varParas.count();
        int c=varParas.count();
        if(c>1)
        {
            QGenieRCWirelessObject *rcwo=QGenieRCWirelessObject::GetSingletonPtr();
            QGenieGaccObject *gacc=QGenieGaccObject::GetSingletonPtr();
//            QGenieDelayProcessLogic *delayprocess=QGenieDelayProcessLogic::GetSingletonPtr();
//            QGenieTimelineLocker *locker=QGenieTimelineLocker::GetSingletonPtr();
            QGenieFupdateObject *fupo=QGenieFupdateObject::GetSingletonPtr();
            QGenieRCTopoDetailViewManager *topodm=QGenieRCTopoDetailViewManager::GetSingletonPtr();
			QGenieRebootObject* rebootObj = QGenieRebootObject::GetSingletonPtr();
			QGenieQosObject* qosObj = QGenieQosObject::GetSingletonPtr();
            int notify_message=varParas[0].toInt();
            if(notify_message==SCCMD_LANGUAGE)
            {
                if(rcwo)
                {
                    rcwo->whenLanChaned();
                }
                if(gacc)
                {
                    gacc->translateGaccUi();
                }
                if(fupo)
                {
                    fupo->translateUi();
                }
                if(topodm)
                {
                    topodm->translateUi();
                }
				if(rebootObj)
				{
					rebootObj->retranslateUi();
				}
				if(qosObj)
				{
					qosObj->retranslateUi();
				}
#ifdef USE_READYSHARE
                QGenieRSManager *rsmanager=QGenieRSManager::GetSingletonPtr();
                if(rsmanager)
                {
                    rsmanager->retranslateUi();
                }
                QGenieRSBasic *rsbasic=QGenieRSBasic::GetSingletonPtr();
                if(rsbasic)
                {
                    rsbasic->retranslateUi();
                }
                QGenieRSRemote *rsremote=QGenieRSRemote::GetSingletonPtr();
                if(rsremote)
                {
                    rsremote->retranslateUi();
                }
                QGenieRSPrint *rsprint=QGenieRSPrint::GetSingletonPtr();
                if(rsprint)
                {
                    rsprint->retranslateUi();
                }

#endif
#ifdef USE_TRAFFICMETER
                QGenieTrafficMeterObject *tmobj=QGenieTrafficMeterObject::GetSingletonPtr();
                tmobj->translateUi();
#endif
            }
            else if(notify_message == SCCMD_ROUTER_MODEL)
            {
                QString model=varParas[1].toString();
                if(rcwo && rcwo->mModel!=model)
                {
                    if((sRCFlags & RCF_LOCAL_LOGEDIN)
                            && !(sRCFlags & RCF_IN_LOCAL_LOGIN_PROCESS)
                            )
                    {
                        if(!(sRCFlags&RCF_TIMELINELOCKER_WORKING))
                        {
                            rcwo->logOut(false);
                        }
                    }
                    if(fupo)
                    {
                        fupo->wantRefreshData();
                    }
                    if(!(sRCFlags&RCF_TIMELINELOCKER_WORKING))
                    {
                        rcwo->mModel=model;
                    }
                }

            }
            else if(notify_message == SCCMD_FIRMWARE)
            {
                QString firmware=varParas[1].toString();
                if(fupo && (rcwo->mFirmware != firmware))
                {
                    fupo->wantRefreshData();
                    rcwo->mFirmware=firmware;
                }
            }
            else if(notify_message ==SCCMD_CONNECTION_STATE)
            {
                static int sSavedNetState=-1;
                int state=varParas[1].toInt();
                if(state!=1 && sSavedNetState==1 )
                {
                    int cmd=QGenieIface2ShareObject::sSingleInstance->mNetgearInfoM
                            ->netgearCommand(INetgearInterface::IC_CURRENTSETTING
                                             ,QStringList(),NULL,NULL);
                    qDebug()<<"refresh currentsetting"<<__FILE__<<__LINE__<<cmd;
                }
                sSavedNetState=state;
            }
            else if(notify_message ==SCCMD_CURRENTSETTING)
            {
                QGenieRSManager *rsmanager=QGenieRSManager::GetSingletonPtr();
                if(rsmanager)
                {
                    QStringList lst=varParas[1].toStringList();
                    int flag=(lst.count()>5)?(lst[5].trimmed().toUInt()):0;
                    rsmanager->updateTitleButton(flag);
                }
            }
        }





    }
    else if(cmd==I2CMD_LAYZYLOAD)
    {
        return mObj->lazyLoad();
    }
    else if(cmd == I2CMD_TIMETO_START)
    {
	
    }
    else if(cmd ==I2CMD_UI_ACTION)
    {
        qDebug()<<paras;

        if(paras.toString()=="readyshare")
        {
            sRCFlags |=RCF_READYSHARE_MODE;
            if(!(sRCFlags &(RCF_IN_LOCAL_LOGIN_PROCESS|RCF_LOCAL_LOGEDIN))
                     && !mObj->mEdtPwd->text().isEmpty())
            {
                mObj->startLogin(false);
            }
//            Q_ASSERT(!IS_REMOTE());
//            mDelayProcessLogic->turnToReadyshare();
        }
        else
        {
            sRCFlags &= ~RCF_READYSHARE_MODE;
            if(sRCFlags & RCF_REMOTE)
            {
                if(!(sRCFlags &(RCF_IN_REMOTE_LOGIN_PROCESS|RCF_REMOTE_LOGEDIN))
                        &&(!mObj->mEdtSMPwd->text().isEmpty()))
                {
                    mObj->startLogin(true);
                }
            }
            else
            {
                if(!(sRCFlags &(RCF_IN_LOCAL_LOGIN_PROCESS|RCF_LOCAL_LOGEDIN))
                        &&!(mObj->mEdtPwd->text().isEmpty()))
                {
                    mObj->startLogin(false);
                }

            }
        }
        mObj->updateUi();
        mObj->tryUpdateData();
        mObj->tryUpdateFocus();
    }
    else if((cmd > EPCC_START) &&(cmd <EPCC_END))
    {
        mDelayProcessLogic->parseOuterCommand(cmd,paras);
        mObj->updateUi();
        mObj->tryUpdateData();
    }

    return QVariant();
}




