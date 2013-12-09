#include "QGenieDelayProcessLogic.h"
#include "QGenieRCWirelessObject.h"
#include "genie2_interface.h"
#include "QGenieGaccObject.h"
#include "QGenieFupdateObject.h"
#include "pcpform.h"
extern quint64 sRCFlags;



QGenieDelayProcessLogic::QGenieDelayProcessLogic()
{
}

void QGenieDelayProcessLogic::parseOuterCommand(int cmd,const QVariant &paras)
{
    Q_UNUSED(paras)
    QGenieRCWirelessObject *rcwo=QGenieRCWirelessObject::GetSingletonPtr();
    Q_ASSERT(rcwo);


    if(cmd==EPCC_WIRELESSEXPORT_RETURN)
    {
        rcwo->mStackWC->setCurrentIndex(0);
        return;
    }

    int cPara[][3]={
        {EPCC_TURN_TO_CHANNEL,RCF_TRUNTO_CHANEL,RCF_TRUNTO_CHANEL}
        ,{EPCC_TURN_TO_GACC,RCF_TURNTO_GACC,RCF_TURNTO_GACC}
        ,{EPCC_FIRMWARE_UPDATE,RCF_TURNTO_FUPDATE,RCF_TURNTO_FUPDATE}
        ,{EPCC_TURN_TO_WIRELESS_PWD,RCF_TURNTO_PWD,RCF_TURNTO_PWD}
    };

    quint32 lcmd=0;
    for(int i=0;i<sizeof(cPara)/sizeof(cPara[0]);i++)
    {
        if(cPara[i][0]==cmd)
        {
            quint32 clearF=((sRCFlags & RCF_REMOTE)?ALL_REMOTE_CMD:ALL_LOCAL_CMD);
            sRCFlags &=~clearF;
            lcmd=((sRCFlags & RCF_REMOTE)?(cPara[i][2]):(cPara[i][1]));
            sRCFlags |=lcmd;
        }
    }
//    tryProcessPendedCmd();
//    setCmd();


}

//void QGenieDelayProcessLogic::setCmd(quint32 cmd)
//{
//    const int cPrioritys[]={
//        0,//DC_NONE,
//        1,//DC_CHANEL,
//        1,//DC_READYSHARE,
//        1,//DC_GACC,
//        1,//DC_FUPDATE,
//        1,//DC_TURNTO_ORIGIN,
//        1,// DC_PWD,
//        2,//DC_LOGOUT
//    };
//    if(cPrioritys[mCmd]<=cPrioritys[cmd])
//    {
//        mCmd=cmd;
//    }
//    bool b_busy=isInBusyState();
//    if(/*b_logedin &&*/ ! b_busy)
//    {
//        tryProcessPendedCmd();
//    }
//}

//void QGenieDelayProcessLogic::turnToReadyshare()
//{
////
//    bool b_busy=isInBusyState();
//    mCmd=DC_READYSHARE;

//    if(/*b_logedin &&*/ ! b_busy)
//    {
//        tryProcessPendedCmd();
//    }
//}

//void QGenieDelayProcessLogic::trunToOrigin()
//{
//    bool b_logedin=isLogedIn();
//    bool b_busy=isInBusyState();
//    mCmd=DC_TURNTO_ORIGIN;

//    if(b_logedin && ! b_busy)
//    {
//        tryProcessPendedCmd();
//    }
//}

void QGenieDelayProcessLogic::whenLoginButton()
{
    if(IS_REMOTE_UI())
    {
        sRCFlags &=~ALL_REMOTE_CMD;
    }
    else
    {
        sRCFlags &= ~ALL_LOCAL_CMD;
    }
}

//void QGenieDelayProcessLogic::whenLoginEnd(bool ok,bool remote)
//{
//    if(remote)
//    {
//        sRCFlags &=~ALL_REMOTE_CMD;
//    }
//    else
//    {
//        sRCFlags &= ~ALL_LOCAL_CMD;
//    }
//}

//void QGenieDelayProcessLogic::whenGetWirelessInfoEnd(bool b)
//{
//    if(b)
//    {
//        tryProcessPendedCmd();
//    }
//}

//void QGenieDelayProcessLogic::whenGetGaccLocalInfoEnd(bool b)
//{
//    if(b)
//    {
//        tryProcessPendedCmd();
//    }
//}

void QGenieDelayProcessLogic::whenSettingProcessEnd()
{
    if(sRCFlags & RCF_REMOTE)
    {
        sRCFlags &=~ALL_REMOTE_CMD;
    }
    else
    {
        sRCFlags &= ~ALL_LOCAL_CMD;
    }
}

void QGenieDelayProcessLogic::tryProcessPendedCmd()
{
    QGenieRCWirelessObject *rcwo=QGenieRCWirelessObject::GetSingletonPtr();
    Q_ASSERT(rcwo);
    QGenieFupdateObject *fupo=QGenieFupdateObject::GetSingletonPtr();
    int itemp;
    //process fupdate button,this will turn remote mode to local,when remote not loged in
    const quint32 outerCmd=(RCF_TURNTO_FUPDATE|RCF_TURNTO_PWD|RCF_TRUNTO_CHANEL|RCF_TURNTO_GACC);
    if((sRCFlags & outerCmd)&&(sRCFlags & RCF_REMOTE))
    {
//        rcwo->cob_loginmode->setCurrentIndex(0);
//        rcwo->stack_login->setCurrentIndex(0);

        Q_ASSERT(!(sRCFlags&(RCF_REMOTE_LOGEDIN|RCF_IN_REMOTE_LOGIN_PROCESS)));

        rcwo->changeRemoteState(false);
    }

    //process local pendings
    if((sRCFlags & ALL_LOCAL_CMD) &&!(sRCFlags &RCF_REMOTE))
    {
        Q_ASSERT(!(sRCFlags & RCF_REMOTE_LOGEDIN));
        if(!(sRCFlags & (RCF_IN_LOCAL_LOGIN_PROCESS|RCF_LOCAL_LOGEDIN)))
        {
            if(rcwo->mEdtPwd->text().isEmpty())
            {
                sRCFlags &=~ALL_LOCAL_CMD;
            }
            else
            {
                rcwo->startLogin(false);
            }
            return;
        }
        if(sRCFlags & (RCF_IN_LOCAL_LOGIN_PROCESS|RCF_IN_LOCAL_REFRESHING))
        {
            return;
        }
        Q_ASSERT(sRCFlags & RCF_LOCAL_LOGEDIN);
        quint32 cmd=ALL_LOCAL_CMD&sRCFlags;
        switch(cmd)
        {
        case RCF_TURNTO_PWD:
        case RCF_TRUNTO_CHANEL:
            rcwo->mStackWC->setCurrentIndex(0);
//            rcwo->mTab->setCurrentIndex(0);
            itemp=rcwo->mTab->indexOf(rcwo->mPage1);
            if(itemp >=0)
            {
                rcwo->mTab->setCurrentIndex(itemp);
            }
            rcwo->mSavedTabW=rcwo->mPage1;
            break;
        case RCF_TURNTO_GACC:
            rcwo->mTab->setCurrentWidget(rcwo->mGaccW);
            break;
        case RCF_TURNTO_FUPDATE:
//            Q_ASSERT((sRCFlags & RCF_HAVE_FIRMWARE_UPDATE)
//                     &&!(sRCFlags&(RCF_REMOTE_LOGEDIN|RCF_IN_REMOTE_LOGIN_PROCESS)));
            rcwo->mTab->setCurrentWidget(rcwo->mSubpluginUpdateW);
            if((sRCFlags &RCF_HAVE_FIRMWARE_UPDATE)
                &&(sRCFlags &RCF_FUPDATE_DATA_OK)
                    )
            {
                fupo->changePage("p_data");
            }
            else
            {
                fupo->changePage("p_wait");
                fupo->tryRefreshData();
            }

            break;
        default:
            Q_ASSERT(0);//不允许多重命令
        }
        sRCFlags &=~ALL_LOCAL_CMD;
    }

    //process remote pendings
    if((sRCFlags & ALL_REMOTE_CMD)&&(sRCFlags &RCF_REMOTE))
    {
        Q_ASSERT(0);//放弃这种模式
       // Q_ASSERT(!(sRCFlags & RCF_LOCAL_LOGEDIN));

        if(!(sRCFlags & (RCF_IN_REMOTE_LOGIN_PROCESS|RCF_REMOTE_LOGEDIN)))
        {
            rcwo->startLogin(true);
            return;
        }
        if(sRCFlags & (RCF_IN_REMOTE_LOGIN_PROCESS|RCF_IN_REMOTE_REFRESHING))
        {
            return;
        }
        Q_ASSERT(sRCFlags & RCF_REMOTE_LOGEDIN);

        quint32 cmd=ALL_REMOTE_CMD&sRCFlags;
        switch(cmd)
        {
        case RCF_TURNTO_PWD_REMOTE:
        case RCF_TRUNTO_CHANEL_REMOTE:
//            rcwo->mStackWC->setCurrentIndex(0);
            rcwo->mSavedTabW=rcwo->mPage1;
            rcwo->mTab->setCurrentIndex(0);
            break;
        case RCF_TURNTO_GACC_REMOTE:
            rcwo->mTab->setCurrentWidget(rcwo->mGaccW);
            break;
        default:
            Q_ASSERT(0);//不允许多重命令
        }
        sRCFlags &=~ALL_REMOTE_CMD;
    }
}


void QGenieDelayProcessLogic::whenLogout(bool remote)
{
    if(remote)
    {
        sRCFlags &=~ALL_REMOTE_CMD;
    }
    else
    {
        sRCFlags &= ~ALL_LOCAL_CMD;
    }
}


