
#include "stable.h"



QGenieInterfaceInfoManager::QGenieInterfaceInfoManager()
    :mCanGetInfoFromPlugin(true)
    ,mInfoMutex_netstate(QMutex::Recursive)
    ,mInfoMutex_plugininfo(QMutex::Recursive)
    ,mInfoMutex_fromui(QMutex::Recursive)
    ,mGatewayCache("")
{
}

QIcon QGenieInterfaceInfoManager::getIcon()
{
    return QIcon();
}

QString  QGenieInterfaceInfoManager::getInfo(int what,int para)
{
//    GENIE_LOG(QString("QGenieInterfaceInfoManager::getInfo")+what);
    if(what==INFO_CMD_NETSTATUSE)
    {
        mInfoMutex_netstate.lock();
        QString ret= getNetState();
        mInfoMutex_netstate.unlock();
        return ret;
    }
    else if(what ==INFO_CMD_NETGEAR)
    {

        QGenieInterfaceUiManager *uim=QGenieInterfaceUiManager::GetSingletonPtr();
        ASSERT(uim);
        return (uim && uim->mRouteType!="")?"y":"n";

    }
    else if(what== INFO_CMD_ROUTEMAC)
    {
        return mSavedMac;
    }
    else if(
            what ==INFO_CMD_NETGEAR_REFRESH
            ||what ==INFO_CMD_FIRMWARE
            ||what ==INFO_CMD_ROUTE_TYPE
            || what==INFO_CMD_FUPDATE_PARA1
            || what==INFO_CMD_FUPDATE_PARA2
            || what==INFO_CMD_FUPDATE_PARA3
            || what ==INFO_CMD_NETGEAR_CURRENTSET
            || what == INFO_CMD_RATROSHARELEVEL
            )
    {
        qDebug()<<"cur tid in getinfo "<<QThread::currentThreadId();
        mInfoMutex_plugininfo.lock();
        QString ret= getInfoFromPlugin(what);
        mInfoMutex_plugininfo.unlock();
        return ret;
    }
    else if(what == INFO_CMD_SSID_IS_THIS)
    {
        return ssidIsThis();
    }
    else if(what ==INFO_CMD_MAIN_GEOMETRY)
    {
        mInfoMutex_fromui.lock();
        QString ret= getMainGeometry();
        mInfoMutex_fromui.unlock();
        return ret;
    }
    else if(what ==INFO_CMD_ROUTE_TYPE_IMMEDIATE)
    {
        QGenieInterfaceUiManager *uim=QGenieInterfaceUiManager::GetSingletonPtr();
        ASSERT(uim);
        return uim->routeType();
//        return mRouteType;
    }
    else if(what == INFO_CMD_ROUTE_TYPE_IMMEDIATE_SHORT)
    {
        mInfoMutex_fromui.lock();
        QGenieInterfaceUiManager *uim=QGenieInterfaceUiManager::GetSingletonPtr();
        ASSERT(uim);
        QRegExp regx("v(\\d+)",Qt::CaseInsensitive);
        QString ret=  uim->mRouteType;
        ret=ret.replace(regx,"");
        mInfoMutex_fromui.unlock();
        return ret;
    }
    else if(what == INFO_CMD_FIRMWARE_IMMEDIATE)
    {
        mInfoMutex_fromui.lock();
        QGenieFrameTitle *title=QGenieFrameTitle::GetSingletonPtr();
        ASSERT(title);
        QString ret= title->mUi.text_firmware->text();
        mInfoMutex_fromui.unlock();
        return ret;

    }
    else if(what ==INFO_CMD_FIRMWARE_IMMEDIATE_SHORT)
    {
        mInfoMutex_fromui.lock();
        QGenieFrameTitle *title=QGenieFrameTitle::GetSingletonPtr();
        ASSERT(title);
        QString ret= title->mRouteFirmware;
        mInfoMutex_fromui.unlock();
        return ret;
    }
    else if(what ==INFO_CMD_ROUTE_NAME_B1 ||
            what == INFO_CMD_ROUTE_NAME_B2 ||
            what == INFO_CMD_ROUTE_NAME_B3 ||
            what == INFO_CMD_ROUTE_NAME_NORMAL ||
            what == INFO_CMD_ROUTE_NAME_SELECTED ||
            what == INFO_CMD_ROUTE_NAME_OFFLINE)
    {
        mInfoMutex_fromui.lock();
         QString ret=  getRouteImageName(what,para);
         mInfoMutex_fromui.unlock();
         return ret;
    }
    else if(what ==INFO_CMD_GATEWAY)
    {
        return mGatewayCache;
    }
    else if(what==INFO_CMD_CHANNEL ||
            what==INFO_CMD_REGION)
    {
        if(what==INFO_CMD_CHANNEL)
        {
            return mSaveChannel;
        }
        else
        {
            return mSaveRegion;
        }
    }
    else if(what ==INFO_CMD_ROUTE_WAITTIME)
    {
        return QString::number(constRouteWaitTime(para));
    }
    else if (what ==INFO_CMD_WIRELESS_OK)
    {
        return getIfWirelessOk();
    }
    else if(what ==INFO_CMD_GENIE_INFO)
    {
        return VERSION_STRING;
    }
    else if(what == INFO_CMD_OS_INFO)
    {
        return getOSInfo();
    }

    return QString("no message please contact yidong.tao@dragonflow.com");
}

QString QGenieInterfaceInfoManager::ssidIsThis()
{
    QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
    ASSERT(proxymanager);

    const plug_inf *p=proxymanager->getPluginInf(QUuid(PLUGIN_UUID_WIRELESS));
    if(p && p->pi && mCanGetInfoFromPlugin)
    {
        return p->pi->getInfo(INFO_CMD_SSID_IS_THIS);
    }
    return "";
}

void *QGenieInterfaceInfoManager::getSharedMemory(int para)
{
//    GENIE_LOG("QGenieInterfaceInfoManager::getSharedMemory");
    QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
    ASSERT(proxymanager);
    if(para==PLUGIN_INF_SM_TRAFFIC_STATS
       ||para==PLUGIN_INF_SM_TRAFFIC_INCOMING
       ||para==PLUGIN_INF_SM_TRAFFIC_OUTGOING)
    {
        static void *pstate=NULL;
        static void *pincom=NULL;
        static void *poutgo=NULL;
        if(!mCanGetInfoFromPlugin)
        {
            return NULL;
        }

        void ** pcurtype=NULL;
        if(para ==PLUGIN_INF_SM_TRAFFIC_STATS) {pcurtype=&pstate;}
        else if(para ==PLUGIN_INF_SM_TRAFFIC_INCOMING){pcurtype=&pincom;}
        else if(para == PLUGIN_INF_SM_TRAFFIC_OUTGOING){pcurtype=&poutgo;}

        ASSERT(pcurtype);

        if(*pcurtype)
        {
            return *pcurtype;
        }
        else
        {
            const plug_inf *p=proxymanager->getPluginInf(QUuid(PLUGIN_UUID_NETTRAFFIC));
            if(p && p->pi && mCanGetInfoFromPlugin)
            {
             //   GENIE_LOG("get pointer from QGeniePlugin_NetTraffic "+para);
                *pcurtype=p->pi->getPoint(para);
                return *pcurtype;
            }
            else
            {
              //  GENIE_LOG("getSharedMemory but plugin not exist (nettraffic)");
            }
        }
    }
    else if(para==PLUGIN_INF_TEXT_UPLOAD ||
            para==PLUGIN_INF_TEXT_DOWNLOAD )
    { 
        //nettraffic want these pointer from internet
        static void *ptext_upload=NULL;
        static void *ptext_download=NULL;
        if(!mCanGetInfoFromPlugin)
        {
            return NULL;
        }
        void **pcurtext=NULL;

        if(para ==PLUGIN_INF_TEXT_UPLOAD) {pcurtext=&ptext_upload;}
        else if(para == PLUGIN_INF_TEXT_DOWNLOAD){pcurtext=&ptext_download;}

        ASSERT(pcurtext);

        if(*pcurtext)
        {
            return *pcurtext;
        }
        else
        {
            const plug_inf *p=proxymanager->getPluginInf(QUuid(PLUGIN_UUID_INTERNET));
            if(p && p->pi && mCanGetInfoFromPlugin )
            {
            //    GENIE_LOG("get pointer from QGeniePlugin_Internet "+para);
                *pcurtext=p->pi->getPoint(para);
                return *pcurtext;
            }
            else
            {
             //  GENIE_LOG("getSharedMemory but plugin not exist(internet)");
            }
        }

    }
    else if(para == INFO_CMD_SHEET_PARENT)
    {
        QGenieFrameTitle *title=QGenieFrameTitle::GetSingletonPtr();
        return title;
    }

    return NULL;
}

void QGenieInterfaceInfoManager::notifyPluginMessage(int msg,QString para0,QString para1)
{
    qDebug()<<"QGenieInterfaceInfoManager::notifyPluginMessage";
    if(msg==INFO_NOTIFY_SSID_CHANGE ||
       msg == INFO_NOTIFY_SSID_CHANGE0 ||
            msg ==INFO_NOTIFY_NEED_RECONNECT)
    {
        QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
        ASSERT(proxymanager);
        if(proxymanager)
        {
            plug_inf *p=proxymanager->getPluginInf(PLUGIN_UUID_WIRELESS);
            if(p && p->pi)
            {
                qDebug("QGenieInterfaceInfoManager::notifyPluginMessage to wireless Plugin");
                p->pi->command(msg,para0,para1);
            }
        }
    }
    else if(msg == INFO_NOTIFY_MAC_CHANGE)
    {
        mSavedMac=formatMac(para0);
    }
    else if(msg==INFO_NOTIFY_GATEWAY_CHANGE)
    {
        QGenieInterfaceUiManager *uimanager=QGenieInterfaceUiManager::GetSingletonPtr();
        ASSERT(uimanager);
        uimanager->notifyGateway(para0);
    }
    else if(msg == INFO_NOTIFY_REGION_CHANGE)
    {
        mSaveRegion=para0;
    }
    else if(msg == INFO_NOTIFY_CHANNEL_CHANGE)
    {
        mSaveChannel=para0;
    }
}

QString QGenieInterfaceInfoManager::formatMac(const QString &mac)
{
    QRegExp reg0("^([0-9a-fA-F]{2})([0-9a-fA-F]{2})([0-9a-fA-F]{2})([0-9a-fA-F]{2})([0-9a-fA-F]{2})([0-9a-fA-F]{2})$");
    QRegExp reg1("^[0-9a-fA-F]([0-9a-fA-F][:-][0-9a-fA-F]){5}[0-9a-fA-F]$");

    QString ret=mac;

    if(reg1.exactMatch(ret))
    {
        ret=ret.replace("-","").replace(":","");
    }
    if(reg0.exactMatch(ret))
    {
		ret=reg0.cap(1)+":"+reg0.cap(2)+":"+reg0.cap(3)+":"
           +reg0.cap(4)+":"+reg0.cap(5)+":"+reg0.cap(6);
    }
    else
    {
        ret="";
    }
    return ret;

}



QString QGenieInterfaceInfoManager::getRouteImageName(int what,int para)
{
    qDebug()<<"QGenieInterfaceInfoManager::getRouteImageName(QString para)";

    QGenieInterfaceUiManager *uim=QGenieInterfaceUiManager::GetSingletonPtr();
    ASSERT(uim);
//    QGenieFrameTitle *title=QGenieFrameTitle::GetSingletonPtr();
//    ASSERT(title);
    QString route_rough=uim->mRouteType;
    qDebug()<<"route_rough "<<uim->mRouteType;
    if(route_rough=="" || route_rough=="N/A")
    {
        //set default png name
        route_rough="DEFAULT_NONETGEAR";
    }
    QString mode="";
    struct _{
        int name;
        char *pic;
        int total;
    } cModeMap[]={
    {INFO_CMD_ROUTE_NAME_B1,":/route/%1big_1_%2.png",1},
    {INFO_CMD_ROUTE_NAME_B2, ":/route/%1big_2_%2.png",1},
    {INFO_CMD_ROUTE_NAME_B3,":/route/%1big_3_%2.png",2},
    {INFO_CMD_ROUTE_NAME_NORMAL,":/route/%1Normal_%2.png",2},
    {INFO_CMD_ROUTE_NAME_SELECTED,":/route/%1Selected_%2.png",2},
    {INFO_CMD_ROUTE_NAME_OFFLINE,":/route/%1OffLine_%2.png",1}

    };
//    const char *cModeMap[][3]={
//        {INFO_CMD_ROUTE_NAME_B1,":/route/%1big_1_%2.png","not"},
//        {INFO_CMD_ROUTE_NAME_B2, ":/route/%1big_2_%2.png","not"},
//        {INFO_CMD_ROUTE_NAME_B3,":/route/%1big_3_%2.png","2"},
//        {INFO_CMD_ROUTE_NAME_NORMAL,":/route/%1Normal_%2.png","2"},
//        {INFO_CMD_ROUTE_NAME_SELECTED,":/route/%1Selected_%2.png","2"},
//        {INFO_CMD_ROUTE_NAME_OFFLINE,":/route/%1OffLine_%2.png","not"}
//    };

    for(int i=0;i<sizeof(cModeMap)/sizeof(cModeMap[0]);i++)
    {
        if(what == cModeMap[i].name)
        {
            bool btemp;
            int total=cModeMap[i].total;

            if(para > total || para <1)
            {
                para=1;
            }
            mode=cModeMap[i].pic;
            break;
        }
    }
    if(mode != "")
    {
        QString imgname=mode.arg(route_rough,QString::number(para));
        if(!QFile::exists(imgname))
        {
            imgname=mode.arg("DEFAULT_NETGEAR",QString::number(para));
        }
        return imgname;
    }
    return "";
}

const QRect &QGenieInterfaceInfoManager::getGeometry(int what)
{
    QGenieFrameMain *main=QGenieFrameMain::GetSingletonPtr();
    ASSERT(main);
    if(what ==INFO_CMD_MAIN_GEOMETRY)
    {
        return main->geometry();
    }
    return QRect();
}

QString QGenieInterfaceInfoManager::getNetState()
{
//    const char * cText[]={
//        "good",
//        "warring",
//        "error",
//        "waiting",
//        "unknown"
//    };
//	ASSERT(0);
    QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
    ASSERT(proxymanager);

    const plug_inf *p=proxymanager->getPluginInf(QUuid(PLUGIN_UUID_INTERNET));
    if(p && p->pi && mCanGetInfoFromPlugin)
    {
        return p->pi->getInfo(INFO_CMD_NETSTATUSE);
    }
    return LACK_PLUGIN;

}

QString QGenieInterfaceInfoManager::getInfoFromPlugin(int para)
{
    QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
    ASSERT(proxymanager);

    const plug_inf *p=proxymanager->getPluginInf(QUuid(PLUGIN_UUID_INFO));
    if(p && p->pi && mCanGetInfoFromPlugin)
    {
      //  GENIE_LOG("get info from QGeniePlugin_Info "+para);
        return p->pi->getInfo(para);
    }
    return LACK_PLUGIN;

}

QString QGenieInterfaceInfoManager::getMainGeometry()
{
    char temps[128];
    QGenieFrameMain *main=QGenieFrameMain::GetSingletonPtr();
    ASSERT(main);
    QRect rect=main->geometry();
    sprintf(temps,"%d,%d,%d,%d",rect.x(),rect.y(),rect.width(),rect.height());
    return QString(temps);
}

int QGenieInterfaceInfoManager::constRouteWaitTime(int type)
{
    const int cDefault=100;
    static QMap<QString,int> sMap;
    static QMap<QString,int> sMapGacc;
    static bool sbInited=false;

    if(!sbInited)
    {
        static const struct {const char *name;int time;} cT[]={
        #include "wait_time_def.h"
        };
        const int cNum=sizeof(cT)/sizeof(cT[0]);
        for(int i=0;i<cNum;i++)
        {
            sMap.insert(cT[i].name,cT[i].time);
        };

        static const struct {const char *name;int time;} cTGacc[]={
        #include "wait_time_gacc_def.h"
        };
        const int cNumGacc=sizeof(cTGacc)/sizeof(cTGacc[0]);
        for(int i=0;i<cNumGacc;i++)
        {
            sMapGacc.insert(cTGacc[i].name,cTGacc[i].time);
        }
        sbInited=true;
    }

    QString route=getInfo(INFO_CMD_ROUTE_TYPE_IMMEDIATE_SHORT);
    if(type==0)
    {
        return sMap.value(route,cDefault);
    }
    return sMapGacc.value(route,cDefault);
}

QString QGenieInterfaceInfoManager::getIfWirelessOk()
{
//	ASSERT(0);
   QGeniePluginProxyManager *proxymanager=QGeniePluginProxyManager::GetSingletonPtr();
    ASSERT(proxymanager);

    const plug_inf *p=proxymanager->getPluginInf(QUuid(PLUGIN_UUID_WIRELESS));
    if(p && p->pi )
    {
        GENIE_LOG("get wireless state from QGeniePlugin_wireless ");
        QString para="";

        return p->pi->getInfo(INFO_CMD_WIRELESS_OK);
    }
    return LACK_PLUGIN;
}

QString QGenieInterfaceInfoManager::getOSInfo()
{
#ifdef PLATFORM_WIN32
    OSVERSIONINFO info ;
    info.dwOSVersionInfoSize = sizeof (info) ;
    GetVersionEx (&info) ;
    QString ret;
    ret.sprintf("%d.%d Build %d %s",
                        info.dwMajorVersion,info.dwMinorVersion,(int)(info.dwBuildNumber & 0xFFFF),info.szCSDVersion);

    return ret;
#elif defined(PLATFORM_MACOSX)
    return "macos";
#else
    return "unknow";
#endif
}

