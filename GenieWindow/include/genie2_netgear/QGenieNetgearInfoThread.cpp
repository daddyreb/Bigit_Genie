#include "QGenieNetgearInfoThread.h"
#include "SvtNetworkTool.h"
#include "genie2_netgear.h"
#include <QApplication>

//#define ROUTER_HOST "routerlogin.net"///kai.yan remove for Manual input the IP address of the router
QString ROUTER_HOST = "routerlogin.net";///kai.yan add 
#ifdef Q_OS_WIN32
#define SLEEP(x) Sleep(x);
#else
#define SLEEP(x) usleep((x)*1000);
#endif

static QString strSecondPart(const QString &s)
{
    QStringList templst=s.split("=");
    if(templst.count()>=2)
    {
        return templst[1].replace("<br>","").trimmed();
    }
    return "";
}

enum CURRENTSETTING_INFO
{
    CI_FIRMWARE
    ,CI_MODEL
    ,CI_REGION_TAG
    ,CI_REGION
    ,CI_RSLEVEL
    ,CI_SMSUPPORT
    ,CI_SOAP_VERSION
};

QGenieNetgearInfoThread::QGenieNetgearInfoThread(QGenieNetgearInterface *inf)
    :IThreadInterface(inf)
    ,mSoap(&inf->mExitFlag)
{
    mSoap.setAutoChangePort(true);
}

QGenieNetgearInfoThread::~QGenieNetgearInfoThread()
{
    wait();
}

bool QGenieNetgearInfoThread::canProcess(int cmd)
{
    return (cmd >=QGenieNetgearInterface::IC_CURRENTSETTING)
            &&(cmd <=QGenieNetgearInterface::IC_RAWSOAP);
}


void QGenieNetgearInfoThread::getNetgearData(int cmd,const QStringList &reqlist,QStringList &retlist)
{
    retlist.clear();
    switch(cmd)
    {
    case QGenieNetgearInterface::IC_CURRENTSETTING:
        doCmdCurrentSetting(reqlist,retlist);
        break;
    case QGenieNetgearInterface::IC_LOGIN:
        doCmdLogin(reqlist,retlist);
        break;
    case QGenieNetgearInterface::IC_GET_WIRELESS:
        doCmdWirelessInfo(reqlist,retlist);
        break;
    case QGenieNetgearInterface::IC_SET_WIRELESS:
        doCmdSetWireless(reqlist,retlist);
        break;
    case QGenieNetgearInterface::IC_GET_GACC:
        doCmdGaccInfo(reqlist,retlist);
        break;
    case QGenieNetgearInterface::IC_SET_GACC:
        doCmdSetGacc(reqlist,retlist);
        break;
    case QGenieNetgearInterface::IC_GET_MACADDR:
        doCmdGetMacAddr(reqlist,retlist);
        break;
    case QGenieNetgearInterface::IC_GET_GACCSSID_ALIVE:
        doCmdGetGaccSsidAlive(reqlist,retlist);
        break;
    case QGenieNetgearInterface::IC_GET_CHANNEL:
        doCmdGetChannel(reqlist,retlist);
        break;
    case QGenieNetgearInterface::IC_GET_TM_OPION:
        doCmdTMOption(reqlist,retlist);
        break;
    case QGenieNetgearInterface::IC_GET_TM_STATISTICS:
        doCmdTMStatistics(reqlist,retlist);
        break;
    case QGenieNetgearInterface::IC_SET_TM_ENABLE:
        doCmdTMEnable(reqlist,retlist);
        break;
    case QGenieNetgearInterface::IC_SET_TM_OPTION:
        doCmdSetTM(reqlist,retlist);
        break;
    case QGenieNetgearInterface::IC_REBOOT:
        doCmdReBoot(reqlist,retlist);
        break;
    case QGenieNetgearInterface::IC_SEND_SINGLE_FINISH:
        doCmdSendSingleFinish(reqlist,retlist);
        break;
    case QGenieNetgearInterface::IC_CHECK_NEWFIRMWARE:
        doCmdCheckNewFirmware(reqlist,retlist);
        break;
    case QGenieNetgearInterface::IC_CHECK_NEWFIRMWARE2:
        doCmdCheckNewFirmware2(reqlist,retlist);
        break;
    case QGenieNetgearInterface::IC_UPDATE_NEWFIRMWARE:
        doCmdUpdateNewFirmware(reqlist,retlist);
        break;
    case QGenieNetgearInterface::IC_BLOCKINTERFACE:
        doCmdBlockInterface(reqlist,retlist);
		break;
    case QGenieNetgearInterface::IC_IS_5GOK:
        doCmdIs5GOk(reqlist,retlist);
        break;
    case QGenieNetgearInterface::IC_GET_5GINFO:
        doCmd5GInfo(reqlist,retlist);
        break;
    case QGenieNetgearInterface::IC_SET_5GINFO:
        doCmdSet5GInfo(reqlist,retlist);
        break;
    case QGenieNetgearInterface::IC_GET_5G_GACC:
        doCmd5GGaccInfo(reqlist,retlist);
        break;
    case QGenieNetgearInterface::IC_SET_5G_GACC:
        doCmdSet5GGAccInfo(reqlist,retlist);
        break;
    case QGenieNetgearInterface::IC_ATTACHDEVICE:
        doCmdAttachDevice(reqlist,retlist);
        break;
    case QGenieNetgearInterface::IC_RAWSOAP:
        doCmdRawSoap(reqlist,retlist);
        break;
	case QGenieNetgearInterface::IC_GET_QOSENABLESTATUS:
		doCmdGetQoSEnableStatus(reqlist,retlist);
		break;
	case  QGenieNetgearInterface::IC_SET_QOSENABLESTATUS:
		doCmdSetQoSEnableStatus(reqlist,retlist);
		break;
	case QGenieNetgearInterface::IC_GET_QOSRULES:
		doCmdGetQoSRules(reqlist,retlist);
		break;
	case  QGenieNetgearInterface::IC_ADD_QOSRULE:
		doCmdAddQoSRule(reqlist,retlist);
		break;
	case  QGenieNetgearInterface::IC_ADD_QOSRULEBYETHERNETPORT:
		doCmdAddQoSRuleByEhernetPort(reqlist,retlist);
		break;
	case QGenieNetgearInterface::IC_ADD_QOSRULEBYMAC:
		doCmdAddQoSRuleByMac(reqlist,retlist);
		break;
	case QGenieNetgearInterface::IC_DELETE_QOSRULE:
		doCmdDeleteQoSRule(reqlist,retlist);
		break;
	case QGenieNetgearInterface::IC_UPDATE_QOSPRIORITY:
		docmdUpdateQoSPrority(reqlist,retlist);
		break;
    }

}

void QGenieNetgearInfoThread::getCurrentSetting(QMap<int, QString> &retvalues)
{
#ifdef WHEN_NO_NETGEARROUTER
    if(mInterface->mInterfaceFlags & NIF_IN_DEBUG_MODE)
    {
        retvalues.insert(CI_MODEL,"WNDR3800");
        retvalues.insert(CI_FIRMWARE,"88.88.88.88");
//        retvalues.insert()
        return;
    }
#endif	
    SVT_Http http(&mExitFlag);
     QString url=QString("http://%1/currentsetting.htm").arg(ROUTER_HOST);
	
    QByteArray &temparray=http.getHttpResult(url);

    QTextStream stream(&temparray);
    QRegExp reFirmware("^Firmware=.*",Qt::CaseInsensitive);
    QRegExp reRegionTag("^RegionTag=.*",Qt::CaseInsensitive);
    QRegExp reRegion("^Region=.*",Qt::CaseInsensitive);
    QRegExp reModel("^Model=.*",Qt::CaseInsensitive);
    QRegExp reRSLevel("^ReadyShareSupportedLevel=.*",Qt::CaseInsensitive);
    QRegExp reSMSupport("^SmartNetworkSupported=.*",Qt::CaseInsensitive);
    QRegExp reSoapversion("^SOAPVersion=.*",Qt::CaseInsensitive);
    QRegExp vInfoMatch("(\\D+\\d+(\\.\\d+)*).*",Qt::CaseInsensitive);

    while ( !stream.atEnd() ){
        QString _line=stream.readLine();

        QStringList split_with_slash_r=_line.split("\r");
        foreach(QString line,split_with_slash_r)
        {
            if(reFirmware.exactMatch(line))
            {
                if(vInfoMatch.indexIn(strSecondPart(line)) >-1 )
                {
                    retvalues.insert(CI_FIRMWARE,vInfoMatch.cap(1));;
                }
            }
            else if(reRegionTag.exactMatch(line))
            {
                retvalues.insert(CI_REGION_TAG,strSecondPart(line));
            }
            else if(reRegion.exactMatch(line))
            {
                retvalues.insert(CI_REGION,strSecondPart(line));
            }
            else if(reModel.exactMatch(line))
            {
                retvalues.insert(CI_MODEL,strSecondPart(line));

            }
            else if(reRSLevel.exactMatch(line))
            {
                retvalues.insert(CI_RSLEVEL,strSecondPart(line));
            }
            else if(reSMSupport.exactMatch(line))
            {
                retvalues.insert(CI_SMSUPPORT,strSecondPart(line));
            }
            else if(reSoapversion.exactMatch(line))
            {
                retvalues.insert(CI_SOAP_VERSION,strSecondPart(line));
            }
        }
    }
    mSoap.setRouteType(retvalues.value(CI_MODEL));
}

//paras:
//ret: retcode,model,firmware,region,region_tag,rs_level
void QGenieNetgearInfoThread::doCmdCurrentSetting( const QStringList &reqlist, QStringList &retlist)
{
	Q_UNUSED(reqlist)
    retlist.clear();

    QMap<int,QString> currentsetting;
    getCurrentSetting(currentsetting);
    if(currentsetting.contains(CI_MODEL)&&currentsetting.contains(CI_FIRMWARE))
    {
        retlist<<"000"<<currentsetting.value(CI_MODEL)<<currentsetting.value(CI_FIRMWARE)
                <<currentsetting.value(CI_REGION)<<currentsetting.value(CI_REGION_TAG)
                <<currentsetting.value(CI_RSLEVEL);

		//////////////////////////////////kai.yan add////////////////////////////////////////
		if(currentsetting.contains(CI_SMSUPPORT))
		{
			retlist<<currentsetting.value(CI_SMSUPPORT);
		}
		//////////////////////////////////////////////////////////////////////////
    }
    else
    {
        retlist<<"-2"<<"currentsetting error";
    }



}

//para: username,password,router ip
//ret: retcode,model,firmware,region,region_tag,rs_level
void QGenieNetgearInfoThread::doCmdLogin(const QStringList &reqlist, QStringList &retlist)
{
    retlist.clear();
#ifdef WHEN_NO_NETGEARROUTER
    if(mInterface->mInterfaceFlags & NIF_IN_DEBUG_MODE)
    {
        if((reqlist.count()<2)||(reqlist[0]!="admin")||(reqlist[1]!="password"))
        {
            retlist<<"-1"<<"debug error";
        }
        else
        {
            retlist<<"000"<<"WNDR3800"<<"V1.0.0.25"<<"us"<<"WNDR3800_NA"
                    <<"7";
        }

        SLEEP(2200);

        return;
    }
#endif

    if(reqlist.count()<3)
    {
        retlist<<"-1"<<"laker parameter";
        return;
    }
	ROUTER_HOST = reqlist[2];
    QMap<int,QString> currentsetting;
    getCurrentSetting(currentsetting);
    if(currentsetting.value(CI_MODEL,"")=="")
    {
        retlist<<"-2"<<"current setting error";
        return;
    }
	
	//////////////kai.yan add///////////////////////
	QString soapVersion = currentsetting.value(CI_SOAP_VERSION,"");
	bool isDGN1000;
	if(0 == currentsetting.value(CI_MODEL).compare("DGN1000",Qt::CaseInsensitive))
	{
		isDGN1000 = true;
	}else
	{
		isDGN1000 = false;
	}
	if(!isDGN1000 && (soapVersion == ""))
	{
		retlist<<"-6"<<"old netgear router";
		return;
	}
	QStringList soapv_lst=soapVersion.split(".");
	int c=soapv_lst.count();
	//        bool btemp1;
	int i0=(c>0)?(soapv_lst[0].toInt()):0;
	int i1=(c>1)?(soapv_lst[1].toInt()):0;
	if(!isDGN1000 && (i0*10000+i1)<10011)
	{
		retlist<<"-6"<<"old netgear router";
		return;
	}
	///////////////////end //////////////////////

    QMap<QString,QString> soapparas;
    QMap<QString,QString> soapret;
    soapparas.insert("NewUsername",reqlist[0]);
    soapparas.insert("NewPassword",reqlist[1]);
    bool btemp=mSoap.doFunc(ROUTER_HOST,"ParentalControl","Authenticate",&soapparas,&soapret);
    QString _5GState="0";
	QString _QoSState = "0";///kai.yan add
    if(btemp)
    {
//        //step1 check if soap version bigger than
//        QStringList soapv_lst=currentsetting.value(CI_SOAP_VERSION).split(".");
//        int c=soapv_lst.count();
////        bool btemp1;
//        int i0=(c>0)?(soapv_lst[0].toInt()):0;
//        int i1=(c>1)?(soapv_lst[1].toInt()):0;
        bool soapV_bigger_than_114=((i0*10000+i1)>10014);
       // soapV_bigger_than_114 = true; ///kai.yan add for test 5G
        //step2 check soap interface
        if(soapV_bigger_than_114)
        {
            QMap<QString,QString> soapparas1;
            QMap<QString,QString> soapret1;

            if(mSoap.doFunc(ROUTER_HOST,"WLANConfiguration","Is5GSupported",&soapparas1,&soapret1))
            {
                bool btemp1;
                int is5GSupported=soapret1.value("New5GSupported").toInt(&btemp1);
                if(btemp1 && (is5GSupported==1))
                {
                    const quint32 wireless5G=(1<<0);
                    const quint32 gacc5G=(1<<1);
                    _5GState=QString::number(wireless5G|gacc5G);
                }
            }
        }
		////////////////////////////////kai.yan add //////////////////////////////
		///isSupport QoS function
		QMap<QString,QString> soapparas2;
		QMap<QString,QString> soapret2;
		
		if(mSoap.doFunc(ROUTER_HOST,"DeviceConfig","GetQoSEnableStatus",&soapparas2,&soapret2))
		{
			bool btemp2;
			int isQoSSupported = soapret2.value("ResponseCode").toInt(&btemp2);
			if (btemp2 && (isQoSSupported >1))
			{
				_QoSState = "0";
			}else
			{
				_QoSState = "1";
			}
		}
		////////////////////////////////////end/////////////////////////////////
    }

    retlist<<soapret.value("ResponseCode","-4")<<currentsetting.value(CI_MODEL)<<currentsetting.value(CI_FIRMWARE)
            <<currentsetting.value(CI_REGION)<<currentsetting.value(CI_REGION_TAG)
           <<currentsetting.value(CI_RSLEVEL)<<currentsetting.value(CI_SMSUPPORT,"0")<<_5GState << _QoSState;



}

//paras:
//ret: retcode [ssid channel mode mac [secure wep_secure password] ]
void QGenieNetgearInfoThread::doCmdWirelessInfo(const QStringList &reqlist, QStringList &retlist)
{
	Q_UNUSED(reqlist)
//#ifdef DBG_MODE
//    if(QApplication::arguments().contains("-dbg_mode"))
//    {

//        retlist<<"000"<<"wireless001"<<"United States"<<"Auto"<<"300Mbps"
//                <<"204E7F359BF3"<<"WPA2-PSK"<<"netgear";

//        SLEEP(2200);

//        return;
//    }

//#endif
    QString retStr="0";
    QMap<QString,QString> soapparas;
    QMap<QString,QString> soapret;
    bool btemp;
    if(!mSoap.doFunc(ROUTER_HOST,"WLANConfiguration","GetInfo",&soapparas,&soapret)
        ||(soapret.value("ResponseCode").toInt(&btemp)!=0)
        ||!btemp)
    {
        retlist<<soapret.value("ResponseCode","-4");
        return;
    }
    QString region=soapret.value("NewRegion");
#if 0
    if(0==region.compare("UNITED STATES", Qt::CaseInsensitive)
            ||0==region.compare("USA", Qt::CaseInsensitive))
    {
        region="US";
    }
#endif

    retlist<<soapret.value("NewSSID")<<region<<soapret.value("NewChannel")
        <<soapret.value("NewWirelessMode")<<soapret.value("NewWLANMACAddress")
        ;
    QString secure_mode=soapret.value("NewBasicEncryptionModes");
    QString secure_wep=soapret.value("NewWEPAuthType");
    soapparas.clear();
    soapret.clear();


    if((0==secure_mode.compare("wep",Qt::CaseInsensitive))
            ||(0==secure_mode.compare("wep64",Qt::CaseInsensitive))
          ||  (0==secure_mode.compare("wep128",Qt::CaseInsensitive))
         // ||  (0==secure_mode.compare("WPA2-PSK[AES]",Qt::CaseInsensitive))
            )
    {
        if(!mSoap.doFunc(ROUTER_HOST,"WLANConfiguration","GetWEPSecurityKeys",&soapparas,&soapret)
            ||(soapret.value("ResponseCode").toInt(&btemp)!=0)
            ||!btemp)
        {
            retlist.clear();
            retlist<<soapret.value("ResponseCode","-4");
            return;
        }
        retlist<<secure_mode<<secure_wep<<soapret.value("NewWEPKey");

    }
    else if(
			(0==secure_mode.compare("wpa2-psk",Qt::CaseInsensitive))
        ||  (0==secure_mode.compare("wpa-psk/wpa2-psk",Qt::CaseInsensitive))
        ||  (0==secure_mode.compare("wpa-psk",Qt::CaseInsensitive))
        ||  (0==secure_mode.compare("mixed wpa",Qt::CaseInsensitive))
            )
    {
        if(!mSoap.doFunc(ROUTER_HOST,"WLANConfiguration","GetWPASecurityKeys",&soapparas,&soapret)
            ||(soapret.value("ResponseCode").toInt(&btemp)!=0)
            ||!btemp)
        {
            retlist.clear();
            retlist<<soapret.value("ResponseCode","-4");
            return;
        }
        retlist<<secure_mode<<secure_wep<<soapret.value("NewWPAPassphrase");
    }
    else
    {
        retlist<<"None"<<""<<"";
    }
    retlist.insert(0,"000");



}

//paras: ssid region channel mode [secure password]
//ret:
//secure: {WPA-PSK/WPA2-PSK, WPA2-PSK}
void QGenieNetgearInfoThread::doCmdSetWireless(const QStringList &reqlist, QStringList &retlist)
{
    retlist.clear();
    if(reqlist.count()<4)
    {
        retlist<<"-1"<<"lack parameter";
        return;
    }

    QMap<QString,QString> soapparas;
    QMap<QString,QString> soapret;
    soapparas.insert("NewSSID",reqlist[0]);
    soapparas.insert("NewRegion",reqlist[1]);
    soapparas.insert("NewChannel",reqlist[2]);
    soapparas.insert("NewWirelessMode",reqlist[3]);
    bool bSecure=false;
    if(reqlist.count()>=6 && reqlist[5]!="" && (0!=reqlist[4].compare("none",Qt::CaseInsensitive)))
    {
        bSecure=true;
        Q_ASSERT(
                (0==reqlist[4].compare("WPA-PSK/WPA2-PSK",Qt::CaseInsensitive))
                ||(0==reqlist[4].compare("WPA2-PSK",Qt::CaseInsensitive))
                );

        soapparas.insert("NewWPAEncryptionModes",reqlist[4]);
        soapparas.insert("NewWPAPassphrase",reqlist[5]);
    }
    if(!mSoap.doFunc(ROUTER_HOST,"WLANConfiguration"
                 ,bSecure?"SetWLANWPAPSKByPassphrase":"SetWLANNoSecurity"
                 ,&soapparas,&soapret) )
    {
        retlist<<soapret.value("ResponseCode","-4");
        return;
    }
    retlist.insert(0,soapret.value("ResponseCode"));


}
//paras:
//ret: retcode 0|1 [ssid secure password]
void QGenieNetgearInfoThread::doCmdGaccInfo( const QStringList &reqlist, QStringList &retlist)
{
	Q_UNUSED(reqlist)
//    Q_ASSERT(reqlist.count()>0);
    QMap<QString,QString> soapparas;
    QMap<QString,QString> soapret;
    bool btemp;
    retlist.clear();
    if(!mSoap.doFunc(ROUTER_HOST,"WLANConfiguration","GetGuestAccessEnabled",&soapparas,&soapret)
        ||(soapret.value("ResponseCode").toInt(&btemp)!=0)
        ||!btemp)
        {
//        if(soapret.value("error").trimmed()=="401")
//        {
//            retlist<<"-4"<<"not supported";
//        }
//        else
//        {
//            retlist<<"-3"<<"soap GetGuestAccessEnabled error";
//        }
        retlist<<soapret.value("ResponseCode","-4");
        return;
    }
    QString enabled=soapret.value("NewGuestAccessEnabled");
    bool b_enabled=(enabled.toInt(&btemp)==1)&&btemp;

    if(!mSoap.doFunc(ROUTER_HOST,"WLANConfiguration","GetGuestAccessNetworkInfo",&soapparas,&soapret))
    {
        retlist.clear();
        retlist<<soapret.value("ResponseCode","-4") << "error in step2";
        return;
    }

    retlist<<soapret.value("ResponseCode")<<(b_enabled?"1":"0")<<soapret.value("NewSSID")<<soapret.value("NewSecurityMode")
                <<soapret.value("NewKey");

}
//paras: 0|1 [ssid secure password]
//ret: retcode
//secure: {"None","WPA2-PSK", "Mixed WPA"};
void QGenieNetgearInfoThread::doCmdSetGacc( const QStringList &reqlist, QStringList &retlist)
{
    QMap<QString,QString> soapparas;
    QMap<QString,QString> soapret;
    bool btemp;
    retlist.clear();

    if(reqlist.isEmpty())
    {
        retlist<<"-1"<<"lack parameter";
        return;
    }


    if(!mSoap.doFunc(ROUTER_HOST,"WLANConfiguration","GetGuestAccessEnabled",&soapparas,&soapret)
        ||(soapret.value("ResponseCode").toInt(&btemp)!=0)
        ||!btemp)
        {
        retlist<<soapret.value("ResponseCode","-4");
        return;
    }
    //1.enabled 0.disabled -1 unknown
    int enabled=((soapret.value("NewGuestAccessEnabled").toInt(&btemp)==1)&&btemp)?1
        :(btemp?0:-1);

    if(reqlist[0]=="0")
    {
        if((enabled==1)||(enabled==-1))
        {
            soapparas.clear();
            soapret.clear();
            soapparas.insert("NewGuestAccessEnabled","0");
            if(!mSoap.doFunc(ROUTER_HOST,"WLANConfiguration","SetGuestAccessEnabled",&soapparas))
            {
                retlist<<soapret.value("ResponseCode","-4");
                return;
            }
            retlist<<soapret.value("ResponseCode","-4");
        }
        else
        {
            retlist<<"000"<<"already disabled";
        }
        return;
    }

    //enabled
    if(reqlist.count()<4)
    {
        retlist<<"-1"<<"lack parameter 2";
        return;
    }


    soapparas.clear();
    soapret.clear();

    soapparas.insert("NewSSID",reqlist[1]);
    soapparas.insert("NewSecurityMode",reqlist[2]);
    soapparas.insert("NewKey1",reqlist[3]);
    soapparas.insert("NewKey2","0");
    soapparas.insert("NewKey3","0");





    soapparas.insert("NewKey4","0");

    if((enabled==0)||(enabled==-1) )
    {
        soapparas.insert("NewGuestAccessEnabled","1");
        if(!mSoap.doFunc(ROUTER_HOST,"WLANConfiguration","SetGuestAccessEnabled2",&soapparas,&soapret))
        {
            retlist<<"-3"<<"soap SetGuestAccessEnabled2 error";
            return;
        }
        retlist<<soapret.value("ResponseCode","-4");
        return;
    }
    else
    {
        if(!mSoap.doFunc(ROUTER_HOST,"WLANConfiguration","SetGuestAccessNetwork",&soapparas,&soapret))
        {
            retlist<<"-3"<<"SetGuestAccessNetwork";
            return;
        }
        retlist<<soapret.value("ResponseCode","-4");
        return;
    }
}
void QGenieNetgearInfoThread::doCmdGetGaccSsidAlive( const QStringList &reqlist, QStringList &retlist)
{
    QMap<QString,QString> soapparas;
    QMap<QString,QString> soapret;
    bool btemp;
    retlist.clear();
    bool _24Alive=false;
    bool _5Alive=false;

    if(reqlist.count()<2)
    {
        retlist<<"-1"<<"lack parameter";
        return;
    }

    if(!reqlist[0].isEmpty())
    {
        soapparas.clear();
        soapret.clear();
        if(mSoap.doFunc(ROUTER_HOST,"WLANConfiguration","GetGuestAccessEnabled",&soapparas,&soapret)
            &&(soapret.value("ResponseCode").toInt(&btemp)==0)
            &&btemp)
        {
            soapparas.clear();
            soapret.clear();
            if(mSoap.doFunc(ROUTER_HOST,"WLANConfiguration","GetGuestAccessNetworkInfo",&soapparas,&soapret)
                    &&(soapret.value("ResponseCode").toInt(&btemp)==0)
                    &&btemp)
            {
                _24Alive=soapret.value("NewSSID")==reqlist[0];
            }
        }
    }
    if(!reqlist[1].isEmpty())
    {
        soapparas.clear();
        soapret.clear();
        if(mSoap.doFunc(ROUTER_HOST,"WLANConfiguration","Get5GGuestAccessEnabled",&soapparas,&soapret)
            &&(soapret.value("ResponseCode").toInt(&btemp)==0)
            &&btemp)
        {
            soapparas.clear();
            soapret.clear();
            if(mSoap.doFunc(ROUTER_HOST,"WLANConfiguration","Get5GGuestAccessNetworkInfo",&soapparas,&soapret)
                    &&(soapret.value("ResponseCode").toInt(&btemp)==0)
                    &&btemp)
            {
                _5Alive=soapret.value("NewSSID")==reqlist[1];
            }
        }
    }
    retlist<<"000"<<(_24Alive?"1":"0")<<(_5Alive?"1":"0");

}

//paras:
//ret: retcode mac
void QGenieNetgearInfoThread::doCmdGetMacAddr( const QStringList &reqlist, QStringList &retlist)
{
	Q_UNUSED(reqlist)
    QMap<QString,QString> soapparas;
    QMap<QString,QString> soapret;
    bool btemp;
    retlist.clear();

    if(!mSoap.doFunc(ROUTER_HOST,"WLANConfiguration","GetInfo",&soapparas,&soapret)
        ||(soapret.value("ResponseCode").toInt(&btemp)!=0)
        ||!btemp)
    {
        retlist<<soapret.value("ResponseCode","-4");
        return;
    }
    retlist<<"000"<<soapret.value("NewWLANMACAddress");

}

//paras:
//ret: retcode channel
void QGenieNetgearInfoThread::doCmdGetChannel( const QStringList &reqlist, QStringList &retlist)
{
	Q_UNUSED(reqlist)
    QMap<QString,QString> soapparas;
    QMap<QString,QString> soapret;
    bool btemp;
    retlist.clear();

    if(!mSoap.doFunc(ROUTER_HOST,"WLANConfiguration","GetInfo",&soapparas,&soapret)
        ||(soapret.value("ResponseCode").toInt(&btemp)!=0)
        ||!btemp)
    {
        retlist<<soapret.value("ResponseCode","-4");
        return;
    }
    retlist<<"000"<<soapret.value("NewChannel");
}

//para
//ret: 0|1|2 "NewTodayConnectionTime,NewTodayUpload,NewTodayDownload,NewYesterdayConnectionTime,NewYesterdayUpload,NewYesterdayDownload,NewWeekConnectionTime,NewWeekUpload,NewWeekDownload,NewMonthConnectionTime,NewMonthUpload,NewMonthDownload,NewLastMonthConnectionTime,NewLastMonthUpload,NewLastMonthDownload";
void QGenieNetgearInfoThread::doCmdTMOption( const QStringList &reqlist, QStringList &retlist)
{
	Q_UNUSED(reqlist)
    QMap<QString,QString> soapparas;
    QMap<QString,QString> soapret;
    bool btemp;
    retlist.clear();

    if(!mSoap.doFunc(ROUTER_HOST,"DeviceConfig","GetTrafficMeterEnabled",&soapparas,&soapret)
        ||(soapret.value("ResponseCode").toInt(&btemp)!=0)
        ||!btemp)
    {
        retlist<<soapret.value("ResponseCode","-4");
        return;
    }
    if(soapret.value("NewTrafficMeterEnable")=="1")
    {
        if(!mSoap.doFunc(ROUTER_HOST,"DeviceConfig","GetTrafficMeterOptions",&soapparas,&soapret)
            ||(soapret.value("ResponseCode").toInt(&btemp)!=0)
            ||!btemp)
        {
            retlist<<soapret.value("ResponseCode","-4")<<"soap GetTrafficMeterStatistics error";
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
        retlist<<soapret.value("ResponseCode")<<"1";
        for(int i=0;i<sizeof(cTMParas)/sizeof(cTMParas[0]);i++)
        {
            retlist<<soapret.value(cTMParas[i]);
        }

    }
    else
    {
        retlist<<soapret.value("ResponseCode")<<"0"/*soapret.value("NewTrafficMeterEnable")*/;
    }


}


void QGenieNetgearInfoThread::doCmdTMStatistics( const QStringList &reqlist, QStringList &retlist)
{
	Q_UNUSED(reqlist)
    QMap<QString,QString> soapparas;
    QMap<QString,QString> soapret;
    bool btemp;
    retlist.clear();

    if(!mSoap.doFunc(ROUTER_HOST,"DeviceConfig","GetTrafficMeterStatistics",&soapparas,&soapret)
        ||(soapret.value("ResponseCode").toInt(&btemp)!=0)
        ||!btemp)
    {
        retlist<<soapret.value("ResponseCode","-4");
        return;
    }
    const char *cTMParas[]=
    {
        "NewTodayConnectionTime","NewTodayUpload","NewTodayDownload",
            "NewYesterdayConnectionTime","NewYesterdayUpload","NewYesterdayDownload",
            "NewWeekConnectionTime","NewWeekUpload","NewWeekDownload",
            "NewMonthConnectionTime","NewMonthUpload","NewMonthDownload",
            "NewLastMonthConnectionTime","NewLastMonthUpload","NewLastMonthDownload"
    };
    retlist<<soapret.value("ResponseCode");
    for(int i=0;i<sizeof(cTMParas)/sizeof(cTMParas[0]);i++)
    {
        retlist<<soapret.value(cTMParas[i]);
    }
}

//paras: 0|1
//ret:
void QGenieNetgearInfoThread::doCmdTMEnable( const QStringList &reqlist, QStringList &retlist)
{
    QMap<QString,QString> soapparas;
    QMap<QString,QString> soapret;
    retlist.clear();
    if(reqlist.isEmpty())
    {
        retlist.insert(0,"-1");
        return;
    }
    soapparas.insert("NewTrafficMeterEnable", reqlist[0]);
    if(!mSoap.doFunc(ROUTER_HOST,"DeviceConfig","EnableTrafficMeter",&soapparas,&soapret))
    {
        retlist<<soapret.value("ResponseCode","-4");
        return;
    }
    retlist<<soapret.value("ResponseCode");
}

//paras: option,monthly_limit,restart_hour,minute,day
//ret:
void QGenieNetgearInfoThread::doCmdSetTM( const QStringList &reqlist, QStringList &retlist)
{
    QMap<QString,QString> soapparas;
    QMap<QString,QString> soapret;
    retlist.clear();
    if(reqlist.count()<5)
    {
        retlist.insert(0,"-1");
        return;
    }

    soapparas.insert("NewControlOption", reqlist[0]);
    soapparas.insert("NewMonthlyLimit", reqlist[1]);
    soapparas.insert("RestartHour", reqlist[2]);
    soapparas.insert("RestartMinute", reqlist[3]);
    soapparas.insert("RestartDay", reqlist[4]);


    if(!mSoap.doFunc(ROUTER_HOST,"DeviceConfig","SetTrafficMeterOptions",&soapparas,&soapret))
    {
        retlist<<soapret.value("ResponseCode","-4");
        return;
    }
    retlist<<soapret.value("ResponseCode");
}

void QGenieNetgearInfoThread::doCmdReBoot( const QStringList &reqlist, QStringList &retlist)
{
	Q_UNUSED(reqlist)
    retlist.clear();
	    QMap<QString,QString> soapret;
    if(!mSoap.doFunc(ROUTER_HOST,"DeviceConfig","Reboot",NULL,&soapret))
        {
        retlist<<soapret.value("ResponseCode","-4");
    }
    else
    {
        retlist<<soapret.value("ResponseCode");
    }

}

void QGenieNetgearInfoThread::doCmdSendSingleFinish( const QStringList &reqlist, QStringList &retlist)
{
	Q_UNUSED(reqlist)
    retlist.clear();
    mSoap.sendFinish(ROUTER_HOST,80);
    mSoap.sendFinish(ROUTER_HOST,5000);
    retlist<<"000";
}

void QGenieNetgearInfoThread::doCmdCheckNewFirmware( const QStringList &reqlist, QStringList &retlist)
{
	Q_UNUSED(reqlist)
    retlist.clear();
    QMap<QString,QString> soapret;
    if(!mSoap.doFunc(ROUTER_HOST,"DeviceConfig","CheckNewFirmware",NULL,&soapret))
    {
        retlist<<soapret.value("ResponseCode","-1");
    }
    else
    {
        retlist<<soapret.value("ResponseCode")
                <<soapret.value("CurrentVersion")
                <<soapret.value("NewVersion")
                <<soapret.value("ReleaseNote");
    }
}

void QGenieNetgearInfoThread::doCmdCheckNewFirmware2( const QStringList &reqlist, QStringList &retlist)
{
	Q_UNUSED(reqlist)
    retlist.clear();

    QMap<int,QString> currentsetting;
    getCurrentSetting(currentsetting);
    if(currentsetting.value(CI_MODEL,"")=="")
    {
        retlist<<"-2"<<"current setting error";
        return;
    }
//    QString url="http://updates1.netgear.com/%1/%2/fileinfo.txt";
    QString url="http://updates1.netgear.com/%1/ww/fileinfo.txt";

    url=url.arg(currentsetting.value(CI_MODEL),currentsetting.value(CI_REGION,"ww")).toLower();
    SVT_Http http(&mExitFlag);
    const QByteArray &bytearray=http.getHttpResult(url);
    QTextStream s(bytearray);
//    QStringList slist = strInfo.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    QRegExp regFirst(".*Major.*");
    if(s.atEnd() ||!regFirst.exactMatch(s.readLine()))
    {
        retlist<<"-4"<<"first line not match";
        return;
    }
    QRegExp regEnd("^\\[.*language.*\\]$");
    QRegExp regData("^(.*file.*)=.*-(V\\d+.*\\d+).*\\.(chk|img).*$");
    while(!s.atEnd())
    {
        QString line=s.readLine();
        if(regEnd.exactMatch(line))
        {
            break;
        }
        if(regData.exactMatch(line))
        {
            QString curV=currentsetting.value(CI_FIRMWARE);
            retlist<<"000"<<curV<<regData.cap(2)<<"";
            return;

        }
    }
    retlist<<"-5"<<"can not find data in file";



}

void QGenieNetgearInfoThread::doCmdUpdateNewFirmware( const QStringList &reqlist, QStringList &retlist)
{
//    Q_ASSERT(reqlist.count()>0);
    QMap<QString,QString> soapparas;
    QMap<QString,QString> soapret;
    retlist.clear();
    soapparas.insert("YesOrNo","1");
    //soapparas.insert("YesOrNo",reqlist.value(0));  //changed by hu
    mSoap.doFunc(ROUTER_HOST,"DeviceConfig","UpdateNewFirmware",&soapparas,&soapret);
    retlist<<soapret.value("ResponseCode","-4");
}

void QGenieNetgearInfoThread::doCmdBlockInterface( const QStringList &reqlist, QStringList &retlist)
{
    retlist.clear();
    QDateTime unlock_time=QDateTime::currentDateTime().addYears(-1);
    if(reqlist.count()>0)
    {
        bool btemp;
        int itemp=reqlist[0].toInt(&btemp);
        unlock_time=QDateTime::currentDateTime().addSecs(btemp?itemp:0);
    }
    while(!mExitFlag && QDateTime::currentDateTime()< unlock_time)
    {
        QThread::msleep(200);
    }
    retlist<<"0";
}

void QGenieNetgearInfoThread::doCmdIs5GOk(const QStringList &reqlist, QStringList &retlist)
{
    QMap<QString,QString> soapparas;
    QMap<QString,QString> soapret;
    retlist.clear();
    mSoap.doFunc(ROUTER_HOST,"WLANConfiguration ","Is5GSupported ",&soapparas,&soapret);
    retlist<<soapret.value("ResponseCode","-4")<<soapret.value("New5GSupported","2");

}

void QGenieNetgearInfoThread::doCmd5GInfo(const QStringList &reqlist,QStringList &retlist)
{
    Q_UNUSED(reqlist)

    QMap<QString,QString> soapparas;
    QMap<QString,QString> soapret;
    bool btemp;
    if(!mSoap.doFunc(ROUTER_HOST,"WLANConfiguration","Get5GInfo",&soapparas,&soapret)
        ||(soapret.value("ResponseCode").toInt(&btemp)!=0)
        ||!btemp)
    {
        retlist<<soapret.value("ResponseCode","-4");
        return;
    }
    QString region=soapret.value("NewRegion");
#if 0
    if(0==region.compare("UNITED STATES", Qt::CaseInsensitive)
            ||0==region.compare("USA", Qt::CaseInsensitive))
    {
        region="US";
    }
#endif

    retlist<<soapret.value("NewSSID")<<region<<soapret.value("NewChannel")
        <<soapret.value("NewWirelessMode")<<soapret.value("NewWLANMACAddress")
        ;
    QString secure_mode=soapret.value("NewBasicEncryptionModes");
    QString secure_wep=soapret.value("NewWEPAuthType");
    soapparas.clear();
    soapret.clear();


    if((0==secure_mode.compare("wep",Qt::CaseInsensitive))
            ||(0==secure_mode.compare("wep64",Qt::CaseInsensitive))
          ||  (0==secure_mode.compare("wep128",Qt::CaseInsensitive))
         // ||  (0==secure_mode.compare("WPA2-PSK[AES]",Qt::CaseInsensitive))
            )
    {
        if(!mSoap.doFunc(ROUTER_HOST,"WLANConfiguration","Get5GWEPSecurityKeys",&soapparas,&soapret)
            ||(soapret.value("ResponseCode").toInt(&btemp)!=0)
            ||!btemp)
        {
            retlist.clear();
            retlist<<soapret.value("ResponseCode","-4");
            return;
        }
        retlist<<secure_mode<<secure_wep<<soapret.value("NewWEPKey");

    }
    else if(
            (0==secure_mode.compare("wpa2-psk",Qt::CaseInsensitive))
        ||  (0==secure_mode.compare("wpa-psk/wpa2-psk",Qt::CaseInsensitive))
        ||  (0==secure_mode.compare("wpa-psk",Qt::CaseInsensitive))
        ||  (0==secure_mode.compare("mixed wpa",Qt::CaseInsensitive))
            )
    {
        if(!mSoap.doFunc(ROUTER_HOST,"WLANConfiguration","Get5GWPASecurityKeys",&soapparas,&soapret)
            ||(soapret.value("ResponseCode").toInt(&btemp)!=0)
            ||!btemp)
        {
            retlist.clear();
            retlist<<soapret.value("ResponseCode","-4");
            return;
        }
        retlist<<secure_mode<<secure_wep<<soapret.value("NewWPAPassphrase");
    }
    else
    {
        retlist<<"None"<<""<<"";
    }
    retlist.insert(0,"000");

}
void QGenieNetgearInfoThread::doCmdSet5GInfo(const QStringList &reqlist, QStringList &retlist)
{
    retlist.clear();
    if(reqlist.count()<4)
    {
        retlist<<"-1"<<"lack parameter";
        return;
    }

    QMap<QString,QString> soapparas;
    QMap<QString,QString> soapret;
    soapparas.insert("NewSSID",reqlist[0]);
    soapparas.insert("NewRegion",reqlist[1]);
    soapparas.insert("NewChannel",reqlist[2]);
    soapparas.insert("NewWirelessMode",reqlist[3]);
    bool bSecure=false;
    if(reqlist.count()>=6 && reqlist[5]!="" && (0!=reqlist[4].compare("none",Qt::CaseInsensitive)))
    {
        bSecure=true;
        Q_ASSERT(
                (0==reqlist[4].compare("WPA-PSK/WPA2-PSK",Qt::CaseInsensitive))
                ||(0==reqlist[4].compare("WPA2-PSK",Qt::CaseInsensitive))
                );

        soapparas.insert("NewWPAEncryptionModes",reqlist[4]);
        soapparas.insert("NewWPAPassphrase",reqlist[5]);
    }
    if(!mSoap.doFunc(ROUTER_HOST,"WLANConfiguration"
                 ,bSecure?"Set5GWLANWPAPSKByPassphrase":"Set5GWLANNoSecurity"
                 ,&soapparas,&soapret) )
    {
        retlist<<soapret.value("ResponseCode","-4");
        return;
    }
    retlist.insert(0,soapret.value("ResponseCode"));
}
void QGenieNetgearInfoThread::doCmdAttachDevice(const QStringList &reqlist, QStringList &retlist)
{
    retlist.clear();
    QMap<QString,QString> soapparas;
    QMap<QString,QString> soapret;
    QString stemp;
#if 0
    if(mInterface->mInterfaceFlags & NIF_IN_DEBUG_MODE)
    {
        stemp="8"
                "@1;192.168.1.2;geines-Mac-mini;28:37:37:12:09:2F;wireless;54;86;Allow"
                "@2;192.168.1.6;xgywz-PC;00:1D:0F:33:08:1C;wireless;54;87;Allow"
                "@3;192.168.1.10;Android_359836041571187;00:0C:29:CB:2E:A4;wireless;54;79;Allow"
                "@4;192.168.1.11;Android_357710047293728;64:A7:69:3F:E6:E9;wireless;54;67;Allow"
                "@5;192.168.1.14;android_8eb092bb95e89cf;C8:AA:21:80:A8:F1;wireless;54;87;Blocked"
                "@6;192.168.1.15;--;28:37:37:12:C6:9D;wireless;54;86;Allow"
                "@7;192.168.1.29;PC2011110909BPO;14:E6:E4:06:F8:E8;wireless;54;92;Allow"
                "@8;192.168.1.30;taoyidongtest;14:E6:E4:06:F8:E9;wired;54;92;Allow"
                ;
        retlist<<"000";
        SLEEP(2000);
    }
    else
#endif
    {
        mSoap.doFunc(ROUTER_HOST,"DeviceInfo ","GetAttachDevice",&soapparas,&soapret);
        retlist<<soapret.value("ResponseCode","-4");
        stemp =soapret.value("NewAttachDevice","");
    }
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
            retlist<<slist[0];
            for(int i=1;i<c;i++)
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

void QGenieNetgearInfoThread::doCmd5GGaccInfo(const QStringList &reqlist, QStringList &retlist)
{
    Q_UNUSED(reqlist)
//    Q_ASSERT(reqlist.count()>0);
    QMap<QString,QString> soapparas;
    QMap<QString,QString> soapret;
    bool btemp;
    retlist.clear();
    if(!mSoap.doFunc(ROUTER_HOST,"WLANConfiguration","Get5GGuestAccessEnabled",&soapparas,&soapret)
        ||(soapret.value("ResponseCode").toInt(&btemp)!=0)
        ||!btemp)
        {
//        if(soapret.value("error").trimmed()=="401")
//        {
//            retlist<<"-4"<<"not supported";
//        }
//        else
//        {
//            retlist<<"-3"<<"soap GetGuestAccessEnabled error";
//        }
        retlist<<soapret.value("ResponseCode","-4");
        return;
    }
    QString enabled=soapret.value("NewGuestAccessEnabled");
    int i_enable=enabled.toInt(&btemp);
    Q_ASSERT(btemp);
    if(i_enable >1)
    {
        //not supported
        retlist.clear();
        retlist<<"000"<<"2";
        return;
    }
    if(!mSoap.doFunc(ROUTER_HOST,"WLANConfiguration","Get5GGuestAccessNetworkInfo",&soapparas,&soapret))
    {
        retlist.clear();
        retlist<<soapret.value("ResponseCode","-4") << "error in step2";
        return;
    }

    retlist<<soapret.value("ResponseCode")<<QString::number(i_enable)<<soapret.value("NewSSID")<<soapret.value("NewSecurityMode")
                <<soapret.value("NewKey");
}

void QGenieNetgearInfoThread::doCmdSet5GGAccInfo(const QStringList &reqlist, QStringList &retlist)
{
    QMap<QString,QString> soapparas;
    QMap<QString,QString> soapret;
    bool btemp;
    retlist.clear();

    if(reqlist.isEmpty())
    {
        retlist<<"-1"<<"lack parameter";
        return;
    }


    if(!mSoap.doFunc(ROUTER_HOST,"WLANConfiguration","Get5GGuestAccessEnabled",&soapparas,&soapret)
        ||(soapret.value("ResponseCode").toInt(&btemp)!=0)
        ||!btemp)
        {
        retlist<<soapret.value("ResponseCode","-4");
        return;
    }
    //1.enabled 0.disabled -1 unknown
    int enabled=((soapret.value("NewGuestAccessEnabled").toInt(&btemp)==1)&&btemp)?1
        :(btemp?0:-1);

    if(reqlist[0]=="0")
    {
        if((enabled==1)||(enabled==-1))
        {
            soapparas.clear();
            soapret.clear();
            soapparas.insert("NewGuestAccessEnabled","0");
            if(!mSoap.doFunc(ROUTER_HOST,"WLANConfiguration","Set5GGuestAccessEnabled",&soapparas))
            {
                retlist<<soapret.value("ResponseCode","-4");
                return;
            }
            retlist<<soapret.value("ResponseCode","-4");
        }
        else
        {
            retlist<<"000"<<"already disabled";
        }
        return;
    }

    //enabled
    if(reqlist.count()<4)
    {
        retlist<<"-1"<<"lack parameter 2";
        return;
    }


    soapparas.clear();
    soapret.clear();

    soapparas.insert("NewSSID",reqlist[1]);
    soapparas.insert("NewSecurityMode",reqlist[2]);
    soapparas.insert("NewKey1",reqlist[3]);
    soapparas.insert("NewKey2","0");
    soapparas.insert("NewKey3","0");





    soapparas.insert("NewKey4","0");

    if((enabled==0)||(enabled==-1) )
    {
        soapparas.insert("NewGuestAccessEnabled","1");
        if(!mSoap.doFunc(ROUTER_HOST,"WLANConfiguration","Set5GGuestAccessEnabled2",&soapparas,&soapret))
        {
            retlist<<"-3"<<"soap SetGuestAccessEnabled2 error";
            return;
        }
        retlist<<soapret.value("ResponseCode","-4");
        return;
    }
    else
    {
        if(!mSoap.doFunc(ROUTER_HOST,"WLANConfiguration","Set5GGuestAccessNetwork",&soapparas,&soapret))
        {
            retlist<<"-3"<<"SetGuestAccessNetwork";
            return;
        }
        retlist<<soapret.value("ResponseCode","-4");
        return;
    }

}


void QGenieNetgearInfoThread::doCmdRawSoap(const QStringList &reqlist, QStringList &retlist)
{
    int c=reqlist.count();
    Q_ASSERT(c>=2);
    QMap<QString,QString> soapparas;
    QMap<QString,QString> soapret;
    retlist.clear();
    for(int i=2;(i+1)<c;i++)
    {
        soapparas.insert(reqlist[i],reqlist[i+1]);
    }
    mSoap.doFunc(ROUTER_HOST,reqlist[0],reqlist[1],&soapparas,&soapret);
    retlist<<soapret.value("ResponseCode","-1");
    QMap<QString,QString>::const_iterator it=soapret.constBegin();
    while(it != soapret.constEnd())
    {
        if(it.key().compare("ResponseCode",Qt::CaseInsensitive)!=0)
        {
            retlist<<it.key()<<it.value();
        }
        it++;
    }

}

void QGenieNetgearInfoThread::doCmdGetQoSEnableStatus(const QStringList &reqlist,QStringList &retlist)
{
	Q_UNUSED(reqlist)
	QMap<QString,QString> soapparas;
	QMap<QString,QString> soapret;
	bool btemp;
	if(!mSoap.doFunc(ROUTER_HOST,"DeviceConfig","GetQoSEnableStatus",&soapparas,&soapret))
	{
		retlist<<soapret.value("ResponseCode","-4");
		return;
	}
	retlist<<soapret.value("ResponseCode","-4")<< soapret.value("NewQoSEnableStatus","2");
}

void QGenieNetgearInfoThread::doCmdSetQoSEnableStatus(const QStringList &reqlist,QStringList &retlist)
{
	QMap<QString,QString> soapparas;
	QMap<QString,QString> soapret;
	bool btemp;
	retlist.clear();

	if(reqlist.isEmpty())
	{
		retlist<<"-1"<<"lack parameter";
		return;
	}
	soapparas.insert("NewQoSEnable",reqlist[0]);
	if(!mSoap.doFunc(ROUTER_HOST,"DeviceConfig","SetQoSEnableStatus",&soapparas,&soapret))
	{
		retlist<<soapret.value("ResponseCode","-4");
		return;
	}
	retlist.insert(0,soapret.value("ResponseCode","-4"));
}

void QGenieNetgearInfoThread::doCmdGetQoSRules(const QStringList &reqlist, QStringList &retlist)
{
	Q_UNUSED(reqlist)
	QMap<QString,QString> soapparas;
	QMap<QString,QString> soapret;
	bool btemp;
	retlist.clear();
	if(!mSoap.doFunc(ROUTER_HOST,"DeviceConfig","GetQoSRules",&soapparas,&soapret)
		||(soapret.value("ResponseCode").toInt(&btemp)!=0)
		||!btemp)
	{
		retlist<<soapret.value("ResponseCode","-4");
		return;
	}
	retlist<<soapret.value("ResponseCode","-4") << soapret.value("NewQoSRules","");
}

void QGenieNetgearInfoThread::doCmdAddQoSRule(const QStringList &reqlist, QStringList &retlist)
{
	QMap<QString,QString> soapparas;
	QMap<QString,QString> soapret;
	bool btemp;
	retlist.clear();

	if(reqlist.isEmpty())
	{
		retlist<<"-1"<<"lack parameter";
		return;
	}
	soapparas.insert("NewQoSName",reqlist[0]);
	soapparas.insert("NewCategory",reqlist[1]);
	soapparas.insert("NewPriority",reqlist[2]);
	soapparas.insert("NewConnectionType",reqlist[3]);
	soapparas.insert("NewStartingPort",reqlist[4]);
	soapparas.insert("NewEndingPort",reqlist[5]);

	if(!mSoap.doFunc(ROUTER_HOST,"DeviceConfig","AddQoSRule",&soapparas,&soapret))
	{
		retlist<<soapret.value("ResponseCode","-4");
		return;
	}
	retlist.insert(0,soapret.value("ResponseCode","-4"));
}

void QGenieNetgearInfoThread::doCmdAddQoSRuleByEhernetPort(const QStringList &reqlist, QStringList & retlist)
{
	QMap<QString,QString> soapparas;
	QMap<QString,QString> soapret;
	bool btemp;
	retlist.clear();

	if(reqlist.isEmpty())
	{
		retlist<<"-1"<<"lack parameter";
		return;
	}
	soapparas.insert("NewQoSName",reqlist[0]);
	soapparas.insert("NewPriority",reqlist[1]);
	soapparas.insert("NewPortNumber",reqlist[2]);

	if(!mSoap.doFunc(ROUTER_HOST,"DeviceConfig","AddQoSRuleByEthernetPort",&soapparas,&soapret))
	{
		retlist<<soapret.value("ResponseCode","-4");
		return;
	}
	retlist.insert(0,soapret.value("ResponseCode","-4"));
}

void QGenieNetgearInfoThread::doCmdAddQoSRuleByMac(const QStringList &reqlist, QStringList &retlist)
{
	QMap<QString,QString> soapparas;
	QMap<QString,QString> soapret;
	bool btemp;
	retlist.clear();

	if(reqlist.isEmpty())
	{
		retlist<<"-1"<<"lack parameter";
		return;
	}
	soapparas.insert("NewQoSName",reqlist[0]);
	soapparas.insert("NewPriority",reqlist[1]);
	soapparas.insert("NewMACAddress",reqlist[2]);
	soapparas.insert("NewDeviceName",reqlist[3]);

	if(!mSoap.doFunc(ROUTER_HOST,"DeviceConfig","AddQoSRuleByMAC",&soapparas,&soapret))
	{
		retlist<<soapret.value("ResponseCode","-4");
		return;
	}
	retlist.insert(0,soapret.value("ResponseCode","-4"));
}

void QGenieNetgearInfoThread::doCmdDeleteQoSRule(const QStringList &reqlist, QStringList &retlist)
{
	QMap<QString,QString> soapparas;
	QMap<QString,QString> soapret;
	retlist.clear();

	if(reqlist.isEmpty())
	{
		retlist<<"-1"<<"lack parameter";
		return;
	}
	soapparas.insert("NewQoSName",reqlist[0]);
	if(!mSoap.doFunc(ROUTER_HOST,"DeviceConfig","DeleteQoSRule",&soapparas,&soapret))
	{
		retlist<<soapret.value("ResponseCode","-4");
		return;
	}
	retlist.insert(0,soapret.value("ResponseCode","-4"));
}

void QGenieNetgearInfoThread::docmdUpdateQoSPrority(const QStringList &reqlist, QStringList &retlist)
{
	QMap<QString,QString> soapparas;
	QMap<QString,QString> soapret;
	retlist.clear();

	if(reqlist.isEmpty())
	{
		retlist<<"-1"<<"lack parameter";
		return;
	}

	soapparas.insert("NewQoSName", reqlist[0]);
	soapparas.insert("NewPriority", reqlist[1]);
	mSoap.doFunc(ROUTER_HOST, "DeviceConfig", "UpdateQoSPriority", &soapparas, &soapret);

	retlist<<soapret.value("ResponseCode","-4");
}