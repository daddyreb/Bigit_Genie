#include "QGenieSmartnetworkThread.h"
#include "SvtNetworkTool.h"
#include "QGenieNetgearInterface.h"
#include "INetgearInterface.h"
#ifdef GENIE2_MAINFRAME
#include "genie2_interface.h"
#include "genie2_settings.h"
#endif
#include <QDomDocument>
extern int getRetValue(const QDomNode &node,QString &desp);
QMap<QString,QString> sNullMap;

QGenieSmartnetworkThread::QGenieSmartnetworkThread(QGenieNetgearInterface *inf)
    :IThreadInterface(inf)
    #ifdef GENIE2_MAINFRAME
    ,mFcml(&inf->mExitFlag,GENIE2_GET_SETTING(SETTING_FCML_LINK).toString())
    #else
    ,mFcml(&inf->mExitFlag,"appgenie-staging.netgear.com")
    #endif
{
    Q_ASSERT(QThread::currentThread()==qApp->thread());

}

QGenieSmartnetworkThread::~QGenieSmartnetworkThread()
{
//    mWaitCondition.wakeAll();
    wait();
}

bool QGenieSmartnetworkThread::canProcess(int cmd)
{

    return (cmd >=INetgearInterface::IC_SM_LOGIN)
            &&(cmd <=INetgearInterface::IC_SM_END);
}

void QGenieSmartnetworkThread::getNetgearData(int cmd, const QStringList &reqlist, QStringList &retlist)
{
    retlist.clear();
    switch(cmd)
    {
    case INetgearInterface::IC_SM_LOGIN:
        doCmdLogin(reqlist,retlist);
        break;
    case INetgearInterface::IC_SM_WIRELESS:
        doCmdWireless(reqlist,retlist);
        break;
    case INetgearInterface::IC_SM_SET_WIRELESS:
        doCmdSetWireless(reqlist,retlist);
        break;
    case INetgearInterface::IC_SM_GACC:
        doCmdGacc(reqlist,retlist);
        break;
    case INetgearInterface::IC_SM_SET_GACC:
        doCmdSetGacc(reqlist,retlist);
        break;
    case INetgearInterface::IC_SM_GET_TM_OPTION:
        doCmdTMOption(reqlist,retlist);
        break;
    case INetgearInterface::IC_SM_GET_TM_STATISTICS:
        doCmdTMStatistics(reqlist,retlist);
        break;
    case INetgearInterface::IC_SM_SET_TM_OPTION:
        doCmdTMSetOption(reqlist,retlist);
        break;
    case INetgearInterface::IC_SM_SET_TM_ENABLE:
        doCmdTMSetEnable(reqlist,retlist);
        break;
    case INetgearInterface::IC_SM_GET_FIRMWARE:
        doCmdFirmware(reqlist,retlist);
        break;
    case INetgearInterface::IC_SM_LOGOUT:
        doCmdLogout(reqlist,retlist);
        break;
    case INetgearInterface::IC_SM_IS_5GOK:
        doCmdIs5GOk(reqlist,retlist);
        break;
    case INetgearInterface::IC_SM_GET_5GINFO:
        doCmdGet5GInfo(reqlist,retlist);
        break;
    case INetgearInterface::IC_SM_SET_5GINFO:
        doCmdSet5GInfo(reqlist,retlist);
        break;
    case INetgearInterface::IC_SM_GET_5GGACC:
        doCmdGet5GGaccInfo(reqlist,retlist);
        break;
    case INetgearInterface::IC_SM_SET_5GGACC:
        doCmdSet5GGaccInfo(reqlist,retlist);
        break;
    case INetgearInterface::IC_SM_ATTACHDEVICE:
        doCmdAttachDevice(reqlist,retlist);
        break;

    }
}


void QGenieSmartnetworkThread::doCmdLogin( const QStringList &reqlist, QStringList &retlist)
{
    retlist.clear();
    mUserName=mPassword=QString();
    if(reqlist.count()<2)
    {
        retlist<<QString::number(FE_LACK_PARAMETER)<<"not enough parameters";
        return;
    }

    QString serr;
    int ierr;
    bool b=mFcml.init(reqlist[0],reqlist[1],&ierr,&serr);
    if(!b)
    {
        Q_ASSERT(ierr!=0);
        retlist<<QString::number(ierr)<<serr;
        return;
    }
    QList<QMap<int,QString> > retlst;
    b=mFcml.getCPList(retlst);
    if(!b )
    {
        retlist<<QString::number(FE_GETCPLIST)<<"getCplist error";
    }
    else
    {
        mUserName=reqlist[0];
        mPassword=reqlist[1];
        retlist<<"000";
        const int cParas[]={SVT_Fcml::ECV_IDENTIFIER
                            ,SVT_Fcml::ECV_MODEL
                            ,SVT_Fcml::ECV_SERIAL
                            ,SVT_Fcml::ECV_NAME
                            ,SVT_Fcml::ECV_ONLINE
                           };
        int c=retlst.count();
        retlist<<QString::number(c)<<QString::number(sizeof(cParas)/sizeof(cParas[0]));
        QStringList offlinelist;
        for(int i=0;i<c;i++)
        {
            if(0==retlst[i].value(SVT_Fcml::ECV_ONLINE).compare("true",Qt::CaseInsensitive))
            {
                for(int j=0;j<sizeof(cParas)/sizeof(cParas[0]);j++)
                {
                    retlist<<retlst[i].value(cParas[j]);
                }
            }
            else
            {
                for(int j=0;j<sizeof(cParas)/sizeof(cParas[0]);j++)
                {
                    offlinelist<<retlst[i].value(cParas[j]);
                }
            }
        }
        retlist<<offlinelist;
    }
#ifdef TEST_NETGEAR
    mFcml.setAuthenData("cp171","admin","lenovot61p");
    mFcml.setAuthenData("cp40","admin","password");
#endif

}

void QGenieSmartnetworkThread::doCmdWireless( const QStringList &reqlist, QStringList &retlist)
{
    QDomElement elem;
    retlist.clear();
    int c=reqlist.count();
    if(c<1)
    {
        retlist<<QString::number(FE_LACK_PARAMETER)<<"not enough parameters";
        return;
    }
    int itemp;
    QString stemp;
    const QDomNode &_node=doFunc(reqlist[0],"WLANConfiguration","GetInfo"
                                 ,&itemp,&stemp);
    if(itemp!=0 )
    {
        retlist<<QString::number(itemp)<<stemp;
        return;
    }
    elem=_node.firstChild().toElement();

    const char *cParas[]={
        "NewSSID","NewRegion","NewChannel","NewWirelessMode","NewWLANMACAddress"
            };
    for(int i=0;i<sizeof(cParas)/sizeof(cParas[0]);i++)
    {
        retlist<<elem.attribute(cParas[i]);
    }


    //////////////////////////////////////////////////
    //////////////////////////////////////////////////

    QString secure_mode=elem.attribute("NewBasicEncryptionModes");
    QString secure_wep=elem.attribute("NewWEPAuthType");
    if((0==secure_mode.compare("wep",Qt::CaseInsensitive))
            ||(0==secure_mode.compare("wep64",Qt::CaseInsensitive))
          ||  (0==secure_mode.compare("wep128",Qt::CaseInsensitive))
         // ||  (0==secure_mode.compare("WPA2-PSK[AES]",Qt::CaseInsensitive))
            )
    {
        const QDomNode &_node0=doFunc(reqlist[0],"WLANConfiguration","GetWEPSecurityKeys"
                                      ,&itemp,&stemp);

        if(itemp!=0)
        {
            retlist.clear();
            retlist<<QString::number(itemp)<<stemp;
            return;
        }
        elem=_node0.firstChild().toElement();
        retlist.insert(0,elem.attribute("_responseCode"));
        retlist<<secure_mode<<elem.attribute("NewWEPKey");
    }
    else if(
                        (0==secure_mode.compare("wpa2-psk",Qt::CaseInsensitive))
        ||  (0==secure_mode.compare("wpa-psk/wpa2-psk",Qt::CaseInsensitive))
        ||  (0==secure_mode.compare("wpa-psk",Qt::CaseInsensitive))
        ||  (0==secure_mode.compare("mixed wpa",Qt::CaseInsensitive))
            )
    {
        const QDomNode &_node1=doFunc(reqlist[0],"WLANConfiguration","GetWPASecurityKeys"
                                      ,&itemp,&stemp);
        if(itemp!=0)
        {
            retlist.clear();
            retlist<<QString::number(itemp)<<stemp;
            return;
        }


        elem=_node1.firstChild().toElement();
        retlist.insert(0,elem.attribute("_responseCode"));
        retlist<<secure_mode<<secure_wep<<elem.attribute("NewWPAPassphrase");

    }
    else
    {
        retlist.insert(0,"000");
        retlist<<"None"<<""<<"";
    }
}


void QGenieSmartnetworkThread::doCmdSetWireless( const QStringList &reqlist, QStringList &retlist)
{
    retlist.clear();
    if(reqlist.count()<5)
    {
        retlist<<QString::number(FE_LACK_PARAMETER)<<"lack parameter";
        return;
    }

    QMap<QString,QString> paras;
    paras.insert("NewSSID",reqlist[1]);
    paras.insert("NewRegion",reqlist[2]);
    paras.insert("NewChannel",reqlist[3]);
    paras.insert("NewWirelessMode",reqlist[4]);




    bool bSecure=false;
    if(reqlist.count()>6 && reqlist[6]!="" && (0!=reqlist[5].compare("none",Qt::CaseInsensitive)))
    {
        bSecure=true;
        Q_ASSERT(
                (0==reqlist[5].compare("WPA-PSK/WPA2-PSK",Qt::CaseInsensitive))
                ||(0==reqlist[5].compare("WPA2-PSK",Qt::CaseInsensitive))
                );

        paras.insert("NewWPAEncryptionModes",reqlist[5]);
        paras.insert("NewWPAPassphrase",reqlist[6]);
    }

    int iret;
    QString sret;
    /*const QDomNode &node=*/doFunc(reqlist[0]
                                ,"WLANConfiguration"
                                ,bSecure?"SetWLANWPAPSKByPassphrase":"SetWLANNoSecurity"
                                ,&iret,&sret
                                ,paras);
    retlist<<QString::number(iret)<<sret;

}

void QGenieSmartnetworkThread::doCmdGacc( const QStringList &reqlist, QStringList &retlist)
{
    retlist.clear();
    if(reqlist.count()<1)
    {
        retlist<<QString::number(FE_LACK_PARAMETER)<<"lack parameter";
        return;
    }
//    QMap<QString,QString> paras;

    int iret;
    QString sret;
    const QDomNode node=doFunc(reqlist[0],"WLANConfiguration","GetGuestAccessEnabled"
                                ,&iret,&sret);
    QDomElement elem=node.firstChild().toElement();
    if(iret !=0)
    {
        retlist<<QString::number(iret)<<sret;
        return;
    }
    QString senabled=elem.attribute("NewGuestAccessEnabled");
    const QDomNode &node1=doFunc(reqlist[0],"WLANConfiguration","GetGuestAccessNetworkInfo"
                                 ,&iret,&sret);
    if(iret !=0)
    {
        retlist<<QString::number(iret)<<sret;
        return;
    }
    elem=node1.firstChild().toElement();
    retlist<<"000"<<senabled;
    const char *cParas[]={
        "NewSSID"
        ,"NewSecurityMode"
        ,"NewKey"
    };
    for(int i=0;i<sizeof(cParas)/sizeof(cParas[0]);i++)
    {
        retlist<<elem.attribute(cParas[i]);
    }
}

void QGenieSmartnetworkThread::doCmdSetGacc( const QStringList &reqlist, QStringList &retlist)
{
    retlist.clear();
    const int c=reqlist.count();
    if(c<2)
    {
        retlist<<QString::number(FE_LACK_PARAMETER)<<"lack parameter";
        return;
    }
    int iret;
    QString sret;
    QMap<QString,QString> paras;
    const QDomNode &node=doFunc(reqlist[0],"WLANConfiguration","GetGuestAccessEnabled"
                                ,&iret,&sret);
    QDomElement elem=node.firstChild().toElement();
    bool btemp;
    int ienabled=elem.attribute("NewGuestAccessEnabled").toInt(&btemp);
    if(iret!=0 ||!btemp ||(ienabled!=0 && ienabled!=1))
    {
        retlist<<((iret!=0)?QString::number(ienabled):"-30")<<sret;
        return;
    }
    if(reqlist[1]=="0")
    {
        if(ienabled !=0)
        {
            int itemp;
            QString stemp;
            paras.clear();
            paras.insert("NewGuestAccessEnabled","0");
            /*const QDomNode &node1=*/doFunc(reqlist[0],"WLANConfiguration","SetGuestAccessEnabled"
                                         ,&itemp,&stemp,paras);
            retlist<<QString::number(itemp)<<stemp;
            return;
        }
        else
        {
            retlist<<"000"<<"already disabled";
        }
    }
    else
    {
        if(c<5)
        {
            retlist<<QString::number(FE_LACK_PARAMETER)<<"lack parameters 2";
            return;
        }
        int itemp;
        QString stemp;
        paras.clear();
        paras.clear();
        paras.insert("NewSSID",reqlist[2]);
        paras.insert("NewSecurityMode",reqlist[3]);
        paras.insert("NewKey1",reqlist[4]);
        paras.insert("NewKey2","0");
        paras.insert("NewKey3","0");
        paras.insert("NewGuestAccessEnabled",reqlist[1]);
       /* const QDomNode &node2=*/doFunc(reqlist[0],"WLANConfiguration"
                                     ,"SetGuestAccessEnabled2"
                                     ,&itemp,&stemp,paras);

        retlist<<QString::number(itemp);
        return;

    }


}

void QGenieSmartnetworkThread::doCmdTMOption( const QStringList &reqlist, QStringList &retlist)
{

    retlist.clear();
    if(reqlist.count()<1)
    {
        retlist<<QString::number(FE_LACK_PARAMETER)<<"lack parameters";
        return;
    }
    int iret;
    QString sret;
    bool btemp;
    const QDomNode &node=doFunc(reqlist[0],"DeviceConfig","GetTrafficMeterEnabled"
                          ,&iret,&sret);
    if(iret !=0)
    {
        retlist<<QString::number(iret)<<sret<<"when get enable state";
        return;
    }
    QDomElement elem=node.firstChild().toElement();
    if((elem.attribute("NewTrafficMeterEnable").toInt(&btemp)==1)
        &&btemp)
        {
        const QDomNode node1=doFunc(reqlist[0],"DeviceConfig","GetTrafficMeterOptions"
                                    ,&iret,&sret);
        elem=node1.firstChild().toElement();
        if(iret !=0)
        {
            retlist<<QString::number(iret)<<sret;
            return;
        }
        const char *cTMParas[]=
        {
            "NewControlOption"
            ,"NewMonthlyLimit"
            ,"RestartHour"
            ,"RestartMinute"
            ,"RestartDay"

        };
        retlist<<"000"<<"1";
        for(int i=0;i<sizeof(cTMParas)/sizeof(cTMParas[0]);i++)
        {
            retlist<<elem.attribute(cTMParas[i]);
        }
    }
    else
    {
        retlist<<"000"<<"0";
    }

}

void QGenieSmartnetworkThread::doCmdTMStatistics( const QStringList &reqlist, QStringList &retlist)
{
    retlist.clear();
    if(reqlist.count()<1)
    {
        retlist<<QString::number(FE_LACK_PARAMETER)<<"lack parameters";
        return;
    }
    int iret;
    QString sret;
    const QDomNode &node=doFunc(reqlist[0],"DeviceConfig","GetTrafficMeterStatistics"
                                ,&iret,&sret);
    if(iret!=0)
    {
        retlist<<QString::number(iret)<<sret;
        return;
    }
    QDomElement elem=node.firstChild().toElement();
    retlist<<"000";
    const char *cTMParas[]=
    {
        "NewTodayConnectionTime","NewTodayUpload","NewTodayDownload",
            "NewYesterdayConnectionTime","NewYesterdayUpload","NewYesterdayDownload",
            "NewWeekConnectionTime","NewWeekUpload","NewWeekDownload",
            "NewMonthConnectionTime","NewMonthUpload","NewMonthDownload",
            "NewLastMonthConnectionTime","NewLastMonthUpload","NewLastMonthDownload"
    };
    for(int i=0;i<sizeof(cTMParas)/sizeof(cTMParas[0]);i++)
    {
        retlist<< elem.attribute(cTMParas[i]) ;
    }
}

void QGenieSmartnetworkThread::doCmdTMSetOption( const QStringList &reqlist, QStringList &retlist)
{
    retlist.clear();
    if(reqlist.count()<6)
    {
        retlist<<QString::number(FE_LACK_PARAMETER)<<"lack parameters";
        return;
    }
    int iret;
    QString sret;
    QMap<QString,QString> paras;

    paras.insert("NewControlOption", reqlist[1]);
    paras.insert("NewMonthlyLimit", reqlist[2]);
    paras.insert("RestartHour", reqlist[3]);
    paras.insert("RestartMinute", reqlist[4]);
    paras.insert("RestartDay", reqlist[5]);

    doFunc(reqlist[0],"DeviceConfig","SetTrafficMeterOptions"
           ,&iret,&sret,paras);
    retlist<<QString::number(iret)<<sret;
}

void QGenieSmartnetworkThread::doCmdTMSetEnable( const QStringList &reqlist, QStringList &retlist)
{
    retlist.clear();
    if(reqlist.count()<2)
    {
        retlist<<QString::number(FE_LACK_PARAMETER)<<"lack parameters";
        return;
    }
    int iret;
    QString sret;
    QMap<QString,QString> paras;
    paras.insert("NewTrafficMeterEnable",reqlist[1]);
    doFunc(reqlist[0],"DeviceConfig","EnableTrafficMeter"
           ,&iret,&sret,paras);
    retlist<<QString::number(iret)<<sret;
}

void QGenieSmartnetworkThread::doCmdFirmware(const QStringList &reqlist, QStringList &retlist)
{
    retlist.clear();
    if(reqlist.count()<1)
    {
        retlist<<QString::number(FE_LACK_PARAMETER)<<"lack parameters";
        return;
    }
    int iret;
    QString sret;
    const QDomNode &node=doFunc(reqlist[0],"DeviceInfo","GetInfo"
                                ,&iret,&sret);
    if(iret!=0)
    {
        retlist<<QString::number(iret)<<sret;
        return;
    }
    QDomElement elem=node.firstChild().toElement();
    retlist<<"000";
    retlist<< elem.attribute("Firmwareversion") ;

    const QDomNode node1=doFunc(reqlist[0],"WLANConfiguration","Is5GSupported"
                           ,&iret,&sret);
    if(!iret)
    {
        retlist.clear();
        retlist<<QString::number(iret)<<sret;
        return;
    }
    elem=node1.firstChild().toElement();
    QString _5GState="0";


    bool btemp1;
	int is5GSupported=elem.attribute("New5GSupported").toInt(&btemp1);
    if(btemp1 && (is5GSupported==1))
    {
        const quint32 wireless5G=(1<<0);
        const quint32 gacc5G=(1<<1);
        _5GState=QString::number(wireless5G|gacc5G);
    }

    retlist<<_5GState ;

}

void QGenieSmartnetworkThread::doCmdLogout(const QStringList &reqlist, QStringList &retlist)
{
	Q_UNUSED(reqlist);
	Q_UNUSED(retlist);
    mFcml.endAllSessions();
}

//void QGenieSmartnetworkThread::blockForAuthData()
//{
//    Q_ASSERT(QThread::currentThread()!=qApp->thread());
//    if(mExit)
//    {
//        return;
//    }
//    emit signal_waitAuthenticate();
//    mWaitCondition.wait(&mCondisionMutex);
//}

const QDomNode&QGenieSmartnetworkThread::doFunc(const QString &cp
    , const QString &model, const QString &func
    , int *error, QString *rets
    , const QMap<QString, QString> &paras)
{
    const QDomNode &node=mFcml.fcmlReqWithRefreshSession(cp,model,func,paras,error);

    if(error || rets/*&&(0==*error)*/)
    {
        QString s;
        int itemp;
        if(!rets)
        {
            rets=&s;
        }
        itemp=getRetValue(node,*rets);
        if((itemp==FE_NOT_AUTHENTICATIED)&&!mUserName.isEmpty())
        {
            qDebug()<<"smartnetwork re authenticate!!";
            QStringList loginRet;
            bool btemp;
            doCmdLogin(QStringList()<<mUserName<<mPassword,loginRet);
            if((loginRet.count())>0
                    && (loginRet[0].toInt(&btemp)==0)
                    && btemp)
            {
                const QDomNode &node1=mFcml.fcmlReqWithRefreshSession(cp,model,func,paras,error);
                itemp=getRetValue(node1,*rets);
                if(error && (0==*error))
                {
                    *error=itemp;
                }
                return node1;

            }
        }
        if(error && (0==*error))
        {
            *error=itemp;
        }
    }
    return  node;
}

void QGenieSmartnetworkThread::doCmdIs5GOk(const QStringList &reqlist, QStringList &retlist)
{
    retlist.clear();

    int iret;
    QString sret;
    const QDomNode &node=doFunc(reqlist[0],"WLANConfiguration","Is5GSupported"
                                ,&iret,&sret);
    if(iret!=0)
    {
        retlist<<QString::number(iret)<<sret;
        return;
    }
    QDomElement elem=node.firstChild().toElement();
    retlist<<"000";
    retlist<< elem.attribute("New5GSupported") ;
}

void QGenieSmartnetworkThread::doCmdGet5GInfo(const QStringList &reqlist, QStringList &retlist)
{
    QDomElement elem;
    retlist.clear();
    int c=reqlist.count();
    if(c<1)
    {
        retlist<<QString::number(FE_LACK_PARAMETER)<<"not enough parameters";
        return;
    }
    int itemp;
    QString stemp;
    const QDomNode &_node=doFunc(reqlist[0],"WLANConfiguration","Get5GInfo"
                                 ,&itemp,&stemp);
    if(itemp!=0 )
    {
        retlist<<QString::number(itemp)<<stemp;
        return;
    }
    elem=_node.firstChild().toElement();

    const char *cParas[]={
        "NewSSID","NewRegion","NewChannel","NewWirelessMode","NewWLANMACAddress"
            };
    for(int i=0;i<sizeof(cParas)/sizeof(cParas[0]);i++)
    {
        retlist<<elem.attribute(cParas[i]);
    }


    //////////////////////////////////////////////////
    //////////////////////////////////////////////////

    QString secure_mode=elem.attribute("NewBasicEncryptionModes");
    QString secure_wep=elem.attribute("NewWEPAuthType");
    if((0==secure_mode.compare("wep",Qt::CaseInsensitive))
            ||(0==secure_mode.compare("wep64",Qt::CaseInsensitive))
          ||  (0==secure_mode.compare("wep128",Qt::CaseInsensitive))
         // ||  (0==secure_mode.compare("WPA2-PSK[AES]",Qt::CaseInsensitive))
            )
    {
        const QDomNode &_node0=doFunc(reqlist[0],"WLANConfiguration","Get5GWEPSecurityKeys"
                                      ,&itemp,&stemp);

        if(itemp!=0)
        {
            retlist.clear();
            retlist<<QString::number(itemp)<<stemp;
            return;
        }
        elem=_node0.firstChild().toElement();
        retlist.insert(0,elem.attribute("_responseCode"));
        retlist<<secure_mode<<elem.attribute("NewWEPKey");
    }
    else if(
                        (0==secure_mode.compare("wpa2-psk",Qt::CaseInsensitive))
        ||  (0==secure_mode.compare("wpa-psk/wpa2-psk",Qt::CaseInsensitive))
        ||  (0==secure_mode.compare("wpa-psk",Qt::CaseInsensitive))
        ||  (0==secure_mode.compare("mixed wpa",Qt::CaseInsensitive))
            )
    {
        const QDomNode &_node1=doFunc(reqlist[0],"WLANConfiguration","Get5GWPASecurityKeys"
                                      ,&itemp,&stemp);
        if(itemp!=0)
        {
            retlist.clear();
            retlist<<QString::number(itemp)<<stemp;
            return;
        }


        elem=_node1.firstChild().toElement();
        retlist.insert(0,elem.attribute("_responseCode"));
        retlist<<secure_mode<<secure_wep<<elem.attribute("NewWPAPassphrase");

    }
    else
    {
        retlist.insert(0,"000");
        retlist<<"None"<<""<<"";
    }
}

void QGenieSmartnetworkThread::doCmdSet5GInfo(const QStringList &reqlist, QStringList &retlist)
{
    retlist.clear();
    if(reqlist.count()<5)
    {
        retlist<<QString::number(FE_LACK_PARAMETER)<<"lack parameter";
        return;
    }

    QMap<QString,QString> paras;
    paras.insert("NewSSID",reqlist[1]);
    paras.insert("NewRegion",reqlist[2]);
    paras.insert("NewChannel",reqlist[3]);
    paras.insert("NewWirelessMode",reqlist[4]);




    bool bSecure=false;
    if(reqlist.count()>6 && reqlist[6]!="" && (0!=reqlist[5].compare("none",Qt::CaseInsensitive)))
    {
        bSecure=true;
        Q_ASSERT(
                (0==reqlist[5].compare("WPA-PSK/WPA2-PSK",Qt::CaseInsensitive))
                ||(0==reqlist[5].compare("WPA2-PSK",Qt::CaseInsensitive))
                );

        paras.insert("NewWPAEncryptionModes",reqlist[5]);
        paras.insert("NewWPAPassphrase",reqlist[6]);
    }

    int iret;
    QString sret;
    /*const QDomNode &node=*/doFunc(reqlist[0]
                                ,"WLANConfiguration"
                                ,bSecure?"Set5GWLANWPAPSKByPassphrase":"Set5GWLANNoSecurity"
                                ,&iret,&sret
                                ,paras);
    retlist<<QString::number(iret)<<sret;
}

void QGenieSmartnetworkThread::doCmdGet5GGaccInfo(const QStringList &reqlist, QStringList &retlist)
{
    retlist.clear();
    if(reqlist.count()<1)
    {
        retlist<<QString::number(FE_LACK_PARAMETER)<<"lack parameter";
        return;
    }
//    QMap<QString,QString> paras;

    int iret;
    QString sret;
    const QDomNode node=doFunc(reqlist[0],"WLANConfiguration","Get5GGuestAccessEnabled"
                                ,&iret,&sret);
    QDomElement elem=node.firstChild().toElement();
    if(iret !=0)
    {
        retlist<<QString::number(iret)<<sret;
        return;
    }
    QString senabled=elem.attribute("NewGuestAccessEnabled");
    const QDomNode &node1=doFunc(reqlist[0],"WLANConfiguration","Get5GGuestAccessNetworkInfo"
                                 ,&iret,&sret);
    if(iret !=0)
    {
        retlist<<QString::number(iret)<<sret;
        return;
    }
    elem=node1.firstChild().toElement();
    retlist<<"000"<<senabled;
    const char *cParas[]={
        "NewSSID"
        ,"NewSecurityMode"
        ,"NewKey"
    };
    for(int i=0;i<sizeof(cParas)/sizeof(cParas[0]);i++)
    {
        retlist<<elem.attribute(cParas[i]);
    }
}

void QGenieSmartnetworkThread::doCmdSet5GGaccInfo(const QStringList &reqlist, QStringList &retlist)
{
    retlist.clear();
    const int c=reqlist.count();
    if(c<2)
    {
        retlist<<QString::number(FE_LACK_PARAMETER)<<"lack parameter";
        return;
    }
    int iret;
    QString sret;
    QMap<QString,QString> paras;
    const QDomNode &node=doFunc(reqlist[0],"WLANConfiguration","Get5GGuestAccessEnabled"
                                ,&iret,&sret);
    QDomElement elem=node.firstChild().toElement();
    bool btemp;
    int ienabled=elem.attribute("NewGuestAccessEnabled").toInt(&btemp);
    if(iret!=0 ||!btemp ||(ienabled!=0 && ienabled!=1))
    {
        retlist<<((iret!=0)?QString::number(ienabled):"-30")<<sret;
        return;
    }
    if(reqlist[1]=="0")
    {
        if(ienabled !=0)
        {
            int itemp;
            QString stemp;
            paras.clear();
            paras.insert("NewGuestAccessEnabled","0");
            /*const QDomNode &node1=*/doFunc(reqlist[0],"WLANConfiguration","Set5GGuestAccessEnabled"
                                         ,&itemp,&stemp,paras);
            retlist<<QString::number(itemp)<<stemp;
            return;
        }
        else
        {
            retlist<<"000"<<"already disabled";
        }
    }
    else
    {
        if(c<5)
        {
            retlist<<QString::number(FE_LACK_PARAMETER)<<"lack parameters 2";
            return;
        }
        int itemp;
        QString stemp;
        paras.clear();
        paras.clear();
        paras.insert("NewSSID",reqlist[2]);
        paras.insert("NewSecurityMode",reqlist[3]);
        paras.insert("NewKey1",reqlist[4]);
        paras.insert("NewKey2","0");
        paras.insert("NewKey3","0");
        paras.insert("NewGuestAccessEnabled",reqlist[1]);
       /* const QDomNode &node2=*/doFunc(reqlist[0],"WLANConfiguration"
                                     ,"Set5GGuestAccessEnabled2"
                                     ,&itemp,&stemp,paras);

        retlist<<QString::number(itemp);
        return;

    }
}

void QGenieSmartnetworkThread::doCmdAttachDevice(const QStringList &reqlist, QStringList &retlist)
{
    retlist.clear();
    if(reqlist.count()<1)
    {
        retlist<<"-1"<<"lack parameter";
        return;
    }
    QMap<QString,QString> paras;
    int iret;
    QString sret;

    const QDomNode &node=doFunc(reqlist[0]
                                ,"DeviceInfo"
                               ,"GetAttachDevice"
                                ,&iret,&sret
                                ,paras);
    if(iret!=0)
    {
        retlist<<QString::number(iret)<<sret;
        return;
    }

    QDomElement elem=node.firstChild().toElement();
    retlist<<elem.attribute("_responseCode");
    QString stemp=elem.attribute("NewAttachDevice");

    if(stemp.isEmpty())
    {
        retlist<<"0"<<"0";
    }
    else
    {
        //7@1;192.168.1.2;geines-Mac-mini;28:37:37:12:09:2F;wireless;54;86;Allow@2;192.168.1.6;xgywz-PC;00:1D:0F:33:08:1C;wireless;54;87;Allow@3;192.168.1.10;Android_359836041571187;A8:26:D9:CB:2E:A4;wireless;54;79;Allow@4;192.168.1.11;Android_357710047293728;64:A7:69:3F:E6:E9;wireless;54;67;Allow@5;192.168.1.14;android_8eb092bb95e89cf;C8:AA:21:80:A8:F1;wireless;54;87;Allow@6;192.168.1.15;--;28:37:37:12:C6:9D;wireless;54;86;Allow@7;192.168.1.29;PC2011110909BPO;14:E6:E4:06:F8:E8;wireless;54;92;Allow
        QRegExp reg("@\\d+;");
        QStringList slist=stemp.split(reg);
        qDebug()<<slist;

        int c=slist.count();
        if(c<1)
        {
            retlist<<"0"<<"0";
        }
        else
        {
            retlist<<"0";
            for(int i=0;i<c;i++)
            {
                QStringList ssublist=slist[i].split(";");
                int cc=ssublist.count();

                for(int j=0;j<cc;j++)
                {
                    retlist<<ssublist[j];
                }
                if(1==i)
                {
                    retlist.insert(2,QString::number(cc));
                }

            }
        }
    }

}

//void QGenieSmartnetworkThread::slot_waitAuthenticate()
//{
//    qDebug()<<"QGenieSmartnetworkThread::slot_waitAuthenticate()";
//    mWaitCondition.wakeAll();
//}
