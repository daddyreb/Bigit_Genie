#ifndef TOPOLOGY_DEF_H__
#define TOPOLOGY_DEF_H__


#define USE_NETBIOS_METHOD 1
#define USE_AFP_METHOD 1
#define USE_NETGEARSPECIFIC_METHOD 1
#define USE_UPNP_METHOD 1



#define SEED_SUBSCRIB_ONETIME 3
#define NETSCAN_BASIC_RUNNABLE_NUM 2
#define NETSCAN_HELPER_RUNNABLE_NUM 34
#define NETSCAN_SUBSCRIB_RUNNABLE_NUM 2
#define NETSCAN_HELPER_LOOPTIME 4
#ifdef Q_OS_WIN32
#define SEED_ALLNET_ONETIME 4
#define NETBIOS_BASIC_RUNNABLE_NUM 5
#define NETBIOS_OSVERSION_RETRYTIMES 1
#else
#define SEED_ALLNET_ONETIME 18
#define NETBIOS_BASIC_RUNNABLE_NUM 5
#define NETBIOS_OSVERSION_RETRYTIMES 3
#endif
#define SEED_ALLRESULT_ONTIME 20
#define NETBIOS_SUBSCRIB_RUNNABLE_NUM 2
#define AFP_BASIC_RUNNABLE_NUM 5
#define AFP_SUBSCRIB_RUNNABLE_NUM 2

#define SOAP_RETRY_COUNTER 300 //*100 MS

#define UPNP_LOOP_INTERVAL 60
#define CHECK_GATEWAY_INTERVAL 5000 //MS
#define UNIX_SCRIPTS_DIR (qApp->applicationDirPath()+"/Tools/scripts")
//#define UNIX_SCRIPTS_DIR (QDir::currentPath()+"/../../Tools/scripts")
//#define UNIX_SCRIPTS_DIR (QDir::currentPath()+"/../../../../../Tools/scripts")

#define PROCESS_WAIT_TIME 30000
#define UNIX_CHECKIP_ONETIME 18
#define NETSCAN_OFFLINE_RETRYTIMES 5
#define NOTIFY_NEWDEVICE_INTERVAL 30


enum MAP_FLAGS
{
    MF_ONSTACK_TOP=(1<<0)
    ,MF_USE_ARP_SCAN=(1<<1)
    ,MF_USE_NETBIOS_SCAN=(1<<2)
    ,MF_USE_RDP_SCAN=(1<<3)
    ,MF_USE_PUTTY_SCAN=(1<<4)
    ,MF_USE_AFP_SCAN=(1<<5)
    ,MF_USE_UPNP_SCAN=(1<<6)
    ,MF_USE_NETGEARSPECIFIC_SCAN=(1<<7)
    ,MF_INTERNET_OFFLINE=(1<<8)
    ,MF_IN_DBG_MOD=(1<<9)

};
#define ALL_SCAN_METHOD (MF_USE_ARP_SCAN|MF_USE_NETBIOS_SCAN|MF_USE_AFP_SCAN\
    |MF_USE_UPNP_SCAN|MF_USE_NETGEARSPECIFIC_SCAN/*|MF_USE_RDP_SCAN|MF_USE_PUTTY_SCAN*/)

enum NODE_INFO_ENUM
{
    NIE_IP,
    NIE_MAC,
    NIE_LOCALNAME,
    NIE_LOCALOSVERSION,
    NIE_LOCAL_NICIDX,
    NIE_LOCALPOOLNUMBER,
    NIE_ONLINE,
    NIE_ISHOST,
    NIE_ISWIRED,
    NIE_ISGATEWAY,
    NIE_ISDEFAULT_GATEWAY,
    NIE_ISVIRTUALMACHINE,
    NIE_NETBIOS_VERSION,
    NIE_NETBIOS_HOSTNAME,
    NIE_NETBIOS_MACVERSION,
    NIE_NETBIOS_MACHOSTNAME,
    NIE_UPNP_DEVICETYPE,
    NIE_UPNP_FRIENDLYNAME,
    NIE_UPNP_MANUFACTURER,
    NIE_UPNP_MANUFACTURER_URL,
    NIE_UPNP_MODELDISCRIPTION,
    NIE_UPNP_MODELNAME,
    NIE_CURRENTSETTING_FIRMWARE,
    NIE_CURRENTSETTING_REGIONTAG,
    NIE_CURRENTSETTING_REGION,
    NIE_CURRENTSETTING_MODEL,
    NIE_CURRENTSETTING_OK,
    NIE_CURRENTSETTING_ROUTELOGINDOTNETMAC,
    NIE_CURRENTSETTING_ROUTELOGINDOTNETIP,
    NIE_CURRENTSETTING_RSLEVEL,
    NIE_CURRENTSETTING_MESSAGETYPE,
    NIE_ATTACHDEVICE_NAME,
    NIE_ATTACHDEVICE_ISWIRED,
    NIE_ATTACHDEVICE_LINKSPEED,
    NIE_ATTACHDEVICE_SIGNALSTRENGTH,
    NIE_ATTACHDEVICE_BLOCKED,
	//add by caiyujun
	NIE_ISEXTENDER_ATTATCHDEVICE,
	//add end
    NIE_UPDATE1_VERSION,
    NIE_UPDATE1_NEEDUPDATE,
    NIE_UPDATE1_ADDR,
    NIE_UPDATE1_MD5,
    NIE_UPDATE1_SIZE,
#if USE_RDP
    NIE_RDP_ENABLE,
#endif
#ifdef USE_PUTTY
    NIE_PUTTY_ENABLE,
#endif

    NIE_NUM
};

enum INTERFACE_INFO_ENUM
{
    IIE_IDX,
    IIE_ISDEFAULT,
    IIE_NAME,
    IIE_ISON,
    IIE_ISVISUAL,
    IIE_WIRED,
    IIE_IP,
    IIE_MAC,
    IIE_GATEWAY,
    IIE_NUM
};

enum TOPO_PROTOCAL_TYPE
{
    TPT_NETSCAN,
    TPT_NETBIOS,
#ifdef USE_RDP
    TPT_RDP,
#endif
#ifdef USE_PUTTY
    TPT_PUTTY,
#endif
    TPT_AFP,
    TPT_UPNP,
    TPT_NETGEARSPECIFIC,
    TPT_LOCAL,
    TPT_NUM
};

//add by caiyujun
enum EXTENDER_DEVICE_STATUS
{
	EXTENDER_NORMAL,
	EXTENDER_START,
	EXTENDER_STARTEND,
	EXTENDER_MID,
	EXTENDER_END
};
//add end

#define CORE_LOCK() SVT_Core::GetSingleton().mCoreMutex.lock()
#define CORE_UNLOCK()  SVT_Core::GetSingleton().mCoreMutex.unlock()
#define GET_PROGUARD() (SVT_Core::GetSingleton().mProguard)
#define GET_CORE() (SVT_Core::GetSingleton())
#define CORE_UTIL() (SVT_Core::GetSingleton().mCoreUtils)
#define CORE_STARTRUNNABLE(x) GET_CORE().mThreadMother->mThreadPool.start(x)
#ifdef Q_OS_WIN32
#define SLEEP(x) Sleep(x);
#else
#include <time.h>
#include <errno.h>
#define SLEEP(x) do{\
        struct timespec interval, remainder;\
        interval.tv_sec = 0;\
        interval.tv_nsec = x*1000000;\
        if (nanosleep(&interval, &remainder) == -1) {\
           if (errno == EINTR) {\
               (void)printf("nanosleep interrupted\n");\
               (void)printf("Remaining secs: %d\n", remainder.tv_sec);\
               (void)printf("Remaining nsecs: %d\n", remainder.tv_nsec);\
           }\
           else perror("nanosleep");\
        }\
        }while(0)

#endif
#define MAP_UUID QUuid("{5325cdd0-d419-4e98-88d8-66d865932824}")
#ifdef Q_OS_WIN32
#else
#define INET_PLUGIN_DIR (qApp->applicationDirPath()+"/../Plugins")
#endif
#endif
