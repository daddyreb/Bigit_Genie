
#include "topology.h"
#include <QtNetwork>


#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

//#define CHECK_RET() do{if(!mStarted){return;}}while(0)
#define DO_AND_CHECKEXIT(x) ((x) && (!mCoreExit))
#define FLAG_NO_FUNCTION(x) (!(x) || !(*(x)))
#define HOSTVERSION 255
#define HOSTNAMELEN 17
typedef struct  {
    u_char ID;
    u_char Flags;
    u_short Length;
    char calledname[34];
    char callingname[34];
}NBT_NAME_SESSION_REQUEST;

typedef struct {
   u_char f;
   u_char c;
   u_short id;
   union {
      u_long e;
      u_long w;
        };
   u_long l;
   u_long r;
}GETSTATUS;

typedef struct {
        u_short m;
        u_short v;
        u_short u;
        u_short i;
        u_short f;
        char	s[];
}GETSRVRINFO;

typedef struct  {
 ushort ID;
 ushort Flags;
 ushort Questions;
 ushort Answer;
 ushort Authority;
 ushort Additional;
}NBT_NAME_HEADER;
//NBT_NAME_ANSWER ½á¹¹

typedef struct  {
char   Name_[34];
ushort Type;
ushort Class;
uchar  TTL[4];
ushort DataLen;
uchar  Number;
//char   Name[16];
}NBT_NAME_ANSWER;

typedef struct {
    char   Name[15];
    uchar  suffix;
    ushort flag;
}NBT_NAME_FLAG;

int codename(char name[] , char *codedname)
{
    int i=0;
    unsigned char c;
    char * ptr = codedname;
    if(name != NULL && codedname != NULL)
    {
        memset(ptr, 'A', 34);
        *(ptr++) = ' ';
        for (i=0; i< HOSTNAMELEN-1; i++)
        {
            c = name[i];
            c = name[i] & 0xf0;
            c = (name[i] & 0xf0) >> 4;
            c = ((name[i] & 0xf0) >> 4) + 'A';
            *ptr++ = c;
            *ptr++	=	(name[i] & 0x0f) + 'A';
        }
        *(ptr++) = 0;
    }
    return 0;
}

bool sockWaitForRead(const QAbstractSocket *sock)
{
    int native_sock=sock->socketDescriptor();
    if(-1== native_sock)
    {
        return false;
    }
    fd_set fd;
    FD_ZERO(&fd);
    FD_SET(native_sock,&fd);
    struct timeval tv={2,500};
    int test=select(native_sock+1,&fd,0,0,&tv);
    return (test>0 && FD_ISSET(native_sock,&fd))   ;
}

QString strSecondPart(const QString &s)
{
    QStringList templst=s.split("=");
    if(templst.count()>=2)
    {
        return templst[1].replace("<br>","").trimmed();
    }
    return "";
}


#ifdef Q_OS_WIN32
#include "../../DiagnoseDll/inc/NetcardApi.h"
typedef int (*funGetIpMac)(const unsigned long &ip,const unsigned long &srcip, unsigned char  (&mac_str)[8]);
typedef int (*funcNetbiosGetDevInfo)(const unsigned long &ip,char (&hostname)[NETBIOS_PARA_LEN],char (&version)[NETBIOS_PARA_LEN]);
typedef int (*funcAllAdaptInfo)(struct AllAdapters * alladp,size_t _len);
typedef int (*funcLoopArpTable) (const unsigned long &iface_idx,char * retbuffer,int buffer_size);
//typedef bool (*funcIsPhysicalNic)(char *uuid);
typedef bool (_stdcall *funcEnumInterfaces)(NICLIST **Niclist);
typedef void (_stdcall *funcFreeNicListBuffer)(NICLIST *niclist);
class SVT_UtilWin32Helper
{
public:
    SVT_UtilWin32Helper()
        : mUtilLib(new QLibrary)
        ,mNetcardApiLib(new QLibrary)
    {
        Q_ASSERT(mUtilLib);
        Q_ASSERT(mNetcardApiLib);
        mUtilLib->setFileName("SVTUtils");
        _getIpMac=(funGetIpMac)mUtilLib->resolve("getipmac");
        _netbiosGetDevInfo=(funcNetbiosGetDevInfo)mUtilLib->resolve("get_device_info2");
        _allAdaptInfo=(funcAllAdaptInfo)mUtilLib->resolve("get_adapters_info");
        _loopArpTable=(funcLoopArpTable)mUtilLib->resolve("loop_arp_table");
     //   _isPhysicalNic=(funcIsPhysicalNic)mUtilLib->resolve("is_physical_nic");
        Q_ASSERT(_getIpMac);
        Q_ASSERT(_netbiosGetDevInfo);
        Q_ASSERT(_allAdaptInfo);
        Q_ASSERT(_loopArpTable);
       // Q_ASSERT(_isPhysicalNic);
        mNetcardApiLib->setFileName("NetcardApi.dll");
        _enumInterfaces=(funcEnumInterfaces)mNetcardApiLib->resolve("EnumInterfaces");
        _freeNicListBuffer=(funcFreeNicListBuffer)mNetcardApiLib->resolve("FreeNicListBuffer");
        Q_ASSERT(_enumInterfaces);
        Q_ASSERT(_freeNicListBuffer);
    }
    ~SVT_UtilWin32Helper()
    {
        mUtilLib->unload();
        mNetcardApiLib->unload();
        delete mUtilLib;
        delete mNetcardApiLib;
    }
    QLibrary *mUtilLib;
    QLibrary *mNetcardApiLib;
    funGetIpMac _getIpMac;
    funcNetbiosGetDevInfo _netbiosGetDevInfo;
    funcAllAdaptInfo _allAdaptInfo;
    funcLoopArpTable _loopArpTable;
    //funcIsPhysicalNic _isPhysicalNic;
    funcEnumInterfaces _enumInterfaces;
    funcFreeNicListBuffer _freeNicListBuffer;
};
#else
#include "wifi_lib_iface/INetApiInterface.h"
class SVT_UtilMacOSHelper
{
public:
    SVT_UtilMacOSHelper()
    {
        mInetIface=NULL;
//        qDebug()<<WIFI_PLUGIN_DIR;
        mLoader.setFileName(INET_PLUGIN_DIR+"/libINetApiPlugin.dylib");
        mLoader.load();
        QObject *temp = mLoader.instance();
        if(!temp)
        {
            mLoader.unload();
        }
        else
        {
            mInetIface=qobject_cast<INetApiInterface *>(temp);
        }

    };
    ~SVT_UtilMacOSHelper()
    {
        mLoader.unload();
        mInetIface=NULL;
    }
    bool isWireless(const QString &iname)
    {
        if(!mInetIface)
        {
            return false;
        }
        //Q_ASSERT(mWifiIface);
        bool btemp;
        if(0==mInetIface->IsNicWireless(iname,&btemp))
        {
            return btemp;
        }
        return false;
    }

    QPluginLoader mLoader;
    INetApiInterface *mInetIface;
};

#endif



SVT_QtUtils::SVT_QtUtils(SVT_Core *core)
    :mCoreExit(core->mCoreExit)
{
#ifdef Q_OS_WIN32
    mWin32Helper=new SVT_UtilWin32Helper;
#else
    mMacOSHelper=new SVT_UtilMacOSHelper;
#endif
}

SVT_QtUtils::~SVT_QtUtils()
{
#ifdef Q_OS_WIN32
    delete mWin32Helper;
#else
    delete mMacOSHelper;
#endif
}


QString SVT_QtUtils::getAdpName(quint32 idx)
{
    Q_ASSERT(mAdpIdxHashName.contains(idx));
    return mAdpIdxHashName.value(idx);
}

quint32 SVT_QtUtils::getAdpIdx(const QString &name)
{

    if(mAdpIdxHashName.values().contains(name))
    {
        return mAdpIdxHashName.key(name);
    }
    for(int i=1;i<20;i++)
    {
        if(!mAdpIdxHashName.keys().contains(i))
        {
            mAdpIdxHashName.insert(i,name);
            return i;
        }
    }
    Q_ASSERT(0);
    return 0;

}
#ifdef Q_OS_MACX
bool SVT_QtUtils::netscanCheckIps(const QList<quint32> &src,quint32 adp_idx
                                  ,QList<quint32> & ret_exist,QList<quint32> & ret_offline
                                  ,QList<SVT_MacAddress> &ret_macs)
{
    ret_exist.clear();
    ret_offline.clear();
    QString cmd=QString("./netscan.sh ")+getAdpName(adp_idx);
    foreach(quint32 tempip,src)
    {
        cmd+=QString(" ")+SVT_IpAddress(tempip).toString();
    }
    SVT_UnixProcessHelper process;
    QStringList retlst;
    bool bOk=process.doCmd(cmd,retlst);
    if(bOk)
    {
        foreach(QString line,retlst)
        {
            if(line.isEmpty())
            {
                continue;
            }
            QStringList templst=line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
            if(templst.count() ==3)
            {
                SVT_IpAddress ip;
                ip.Set(templst[1]);
                if(templst[0]=="del")
                {
                    ret_offline.append(ip.asLong());
                }
                else if(templst[0]=="add")
                {
                    ret_exist.append(ip.asLong());
                    SVT_MacAddress mac;
                    mac.SetAddress(templst[2]);
                    ret_macs.append(mac);
                }
            }
        }
    }
    Q_ASSERT(ret_exist.count()==ret_macs.count());
    return bOk;
}

#endif

bool SVT_QtUtils::isPhysicalNic(quint32 adp_idx)
{
#ifdef Q_OS_WIN32
//    static QMap<quint32,bool> sCache;
//    if(sCache.contains(adp_idx))
//    {
//        return sCache.value(adp_idx);
//    }
//    bool ret= _isPhysicalNic(getAdpName(adp_idx).toLocal8Bit().data());
//    sCache.insert(adp_idx,ret);
//    return ret;
    return true;
#else
    return true;
#endif
}

//void SVT_QtUtils::start()
//{
//    mStarted=true;
//}

//void SVT_QtUtils::stop()
//{
//    mStarted=false;
//}

SVT_MacAddress SVT_QtUtils::netscanGetMacForIp(const SVT_IpAddress &ip,const SVT_IpAddress &srcip,bool *ok)const
{
#ifdef Q_OS_WIN32
    unsigned char temp[8]={0};
    SVT_MacAddress macaddr;

    mWin32Helper->_getIpMac(ip.asLong(),srcip.asLong(),temp);
    macaddr.SetAddress(/*SVT_MacAddress::TYPE_UNKNOWN,*/(unsigned char *)temp,6);
    if(ok)
    {
        *ok=*((long *)(&(temp[0])))!=0 ||*((long *)(&(temp[4])))!=0  ;

    }


    return macaddr;
#else
    Q_ASSERT(0);
    return SVT_MacAddress();
#endif
}

SVT_MacAddress SVT_QtUtils::netscanGetMacForRouter(const SVT_IpAddress &ip, const SVT_IpAddress &srcip, bool *ok) const
{
#ifdef Q_OS_WIN32
    return netscanGetMacForIp(ip,srcip,ok);
#else
    SVT_UnixProcessHelper process(&(GET_CORE().mCoreExit));
    QStringList templst;
    process.doCmd(QString("./gatewaymac.sh %1").arg(ip.toString()),templst);
    SVT_MacAddress mac;
    if(!templst.isEmpty())
    {
        mac.SetAddress(templst[0]);
    }
    if(ok)
    {
        *ok=mac.isValid();
    }
    return mac;
#endif
}

void SVT_QtUtils::afpGetDevInfo(const SVT_IpAddress &ip,QMap<int,QString> &retmap,bool *poolExit) const
{
    retmap.clear();
    const static char request[] =
            "\x16\x03\x00\x00\x53\x01\x00\x00\x4f\x03\x00\x3f\x47\xd7\xf7\xba"
            "\x2c\xee\xea\xb2\x60\x7e\xf3\x00\xfd\x82\x7b\xb9\xd5\x96\xc8\x77"
            "\x9b\xe6\xc4\xdb\x3c\x3d\xdb\x6f\xef\x10\x6e\x00\x00\x28\x00\x16"
            "\x00\x13\x00\x0a\x00\x66\x00\x05\x00\x04\x00\x65\x00\x64\x00\x63"
            "\x00\x62\x00\x61\x00\x60\x00\x15\x00\x12\x00\x09\x00\x14\x00\x11"
            "\x00\x08\x00\x06\x00\x03\x01";
    char revc_buff[500]={0};

    QString qstemp;
    qint32 readed=0;


    //step1 gethostname

    SVT_TcpSocket tcp_sock(poolExit?poolExit:&mCoreExit);

    if(
           // DO_AND_CHECKEXIT(!retmap.contains(NIE_NETBIOS_VERSION))
            DO_AND_CHECKEXIT(tcp_sock.create())&&FLAG_NO_FUNCTION(poolExit)
            &&DO_AND_CHECKEXIT(tcp_sock.connect(ip,548))&&FLAG_NO_FUNCTION(poolExit)
            &&DO_AND_CHECKEXIT(tcp_sock.waitForWriteReady(1000))&&FLAG_NO_FUNCTION(poolExit)
            &&DO_AND_CHECKEXIT( sizeof(request)==tcp_sock.sendData((char *)&request,sizeof(request)))&&FLAG_NO_FUNCTION(poolExit)
            &&DO_AND_CHECKEXIT(tcp_sock.waitForReadReady(1000))&&FLAG_NO_FUNCTION(poolExit)
            &&DO_AND_CHECKEXIT((readed= tcp_sock.recvData(revc_buff,sizeof(revc_buff)) )>=0)&&FLAG_NO_FUNCTION(poolExit)
        )
    {

        GETSTATUS *getstatus = (GETSTATUS *)revc_buff;
        if(( sizeof(GETSTATUS) < readed )&& (getstatus->c==3 )
                &&( (ntohl(getstatus->l)+sizeof(GETSTATUS)==readed) )
                )
        {
            GETSRVRINFO *getsrvrinfo = (GETSRVRINFO *)(revc_buff + sizeof(GETSTATUS));

            size_t size ;
            u_short offset = ntohs(*((u_short *)(getsrvrinfo->s + 1 + *(getsrvrinfo->s) + 7)));

            if(sizeof(GETSTATUS)+ offset < readed)
            {
                size = (size_t)ntohs(*((u_short *)(revc_buff+sizeof(GETSTATUS)+ offset )));
                if(sizeof(GETSTATUS)+ offset +size <= readed)
                {
                    char stemp[HOSTVERSION]={0};
                    strncpy(stemp, (revc_buff+sizeof(GETSTATUS) + offset + 2),size<HOSTVERSION?size:HOSTVERSION);
                    qstemp=QString::fromUtf8((char *)stemp);
                    if(!qstemp.isEmpty())
                    {
                        retmap.insert(NIE_NETBIOS_MACHOSTNAME,qstemp);
                    }

                }
            }

            if(sizeof(GETSTATUS)+ntohs(getsrvrinfo->m) < readed)
            {
                size = *(revc_buff+sizeof(GETSTATUS)+ntohs(getsrvrinfo->m));
                if(sizeof(GETSTATUS)+ntohs(getsrvrinfo->m) + size <= readed)
                {
                    char stemp[HOSTVERSION]={0};
                    strncpy(stemp, (revc_buff+sizeof(GETSTATUS)+ntohs(getsrvrinfo->m)+1),size<HOSTVERSION?size:HOSTVERSION);
                    qstemp=(char *)stemp;
                    if(!qstemp.isEmpty())
                    {
                        retmap.insert(NIE_NETBIOS_MACVERSION,qstemp);
                    }
                }
            }
        }


    }
    tcp_sock.close();
}

void SVT_QtUtils::netbiosGetDevInfo(const SVT_IpAddress &ip,QMap<int,QString> &retmap,bool *poolExit) const
{
    retmap.clear();
    QString qstemp;
#ifdef NETBIOS_STRATEGY1
    static const char bs[]={0x0,0x00,0x0,0x10,0x0,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x20,0x43,0x4b,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x0,0x0,0x21,0x0,0x1};
#else
    static const char bs[]={0x20,0xBB,0x0,0x10,0x0,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x20,0x43,0x4b,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x0,0x0,0x21,0x0,0x1};
#endif
    const static char negotiate[] =
            "\x00\x00\x00\x2f\xff\x53\x4d\x42\x72\x00\x00\x00\x00\x00\x00\x00"
            "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x5c\x02"
            "\x00\x00\x00\x00\x00\x0c\x00\x02\x4e\x54\x20\x4c\x4d\x20\x30\x2e"
            "\x31\x32";
    const static char setup_account[] =
            "\x00\x00\x00\x48\xff\x53\x4d\x42\x73\x00\x00\x00\x00\x00\x00\x00"
            "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x5c\x02"
            "\x00\x00\x00\x00\x0d\xff\x00\x00\x00\xff\xff\x02\x00\x5c\x02\x00"
            "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x0b"
            "\x00\x00\x00\x6e\x74\00\x70\x79\x73\x6d\x62";


    char revc_buff[500]={0};
    char local_host[128];
    char dest_host[20];

    //unsigned long _ip=(192<<24)|(168<<16)|(9<<8)|(12);
    //QHostAddress addr(_ip);
    qint32 readed=0;
    bool netbiosnameGeted=false;

    //step1 gethostname

    SVT_UdpSocket udp_sock;
    SVT_TcpSocket tcp_sock(poolExit?poolExit:&mCoreExit);
    if(
            DO_AND_CHECKEXIT(udp_sock.create())&&FLAG_NO_FUNCTION(poolExit)
            &&DO_AND_CHECKEXIT(sizeof(bs)==udp_sock.sendTo(ip,137,bs,sizeof(bs)) )&&FLAG_NO_FUNCTION(poolExit)
            &&DO_AND_CHECKEXIT(udp_sock.waitForReadReady(1000))&&FLAG_NO_FUNCTION(poolExit)
            &&DO_AND_CHECKEXIT((readed=udp_sock.recvFrom(revc_buff,sizeof(revc_buff))) >57 )&&FLAG_NO_FUNCTION(poolExit)
            )
    {
        netbiosnameGeted=true;
#ifdef NETBIOS_STRATEGY1
        int loopend=qMin(qint32(57+20),readed);
        int idx=0;
        for(int i=57;i<loopend;i++)
        {
            if(revc_buff[i]==0x20 || revc_buff[i]==0xcc)
            {
                break;
            }
            if(revc_buff[i]>=' ')
            {
                dest_host[idx++]=revc_buff[i];
            }
        }
        dest_host[(idx<20)?idx:(20-1)]='\0';
        qstemp=(const char *)dest_host;
        if(!qstemp.isEmpty())
        {
            retmap.insert(NIE_NETBIOS_HOSTNAME,qstemp);
        }
#else
        NBT_NAME_HEADER    *nbtnameheader;
        NBT_NAME_ANSWER    *nbtnameanswer;
        NBT_NAME_FLAG      *nbtnameflag;
        if(readed >= sizeof(NBT_NAME_HEADER) + sizeof(NBT_NAME_ANSWER))
        {
            nbtnameheader = (NBT_NAME_HEADER *)revc_buff;
            nbtnameanswer = (NBT_NAME_ANSWER *)(revc_buff + sizeof(NBT_NAME_HEADER));
            for(int i=0; i< nbtnameanswer->Number; i++)
            {
                nbtnameflag = (NBT_NAME_FLAG *)(revc_buff + sizeof(NBT_NAME_HEADER) + sizeof(NBT_NAME_ANSWER) + sizeof(NBT_NAME_FLAG)*i-1);
                if(!(nbtnameflag->flag & 0x80) && nbtnameflag->suffix == 0 )
                {
                    memset(dest_host, 0, sizeof(dest_host));
                    qstrncpy(dest_host, nbtnameflag->Name, sizeof(dest_host));
                    retmap.insert(NIE_NETBIOS_HOSTNAME,QString(dest_host).trimmed());
                    break;
                }
            }
        }
#endif
    }
    udp_sock.close();


    //step2 get os version
    NBT_NAME_SESSION_REQUEST session_request;

    memset(&session_request, 0, sizeof(NBT_NAME_SESSION_REQUEST));
    session_request.ID = 0x81;
    session_request.Flags = 0;
    session_request.Length = 0x4400;

    char callingname[HOSTNAMELEN]={0};
    char calledname[HOSTNAMELEN]={0};

#ifdef Q_OS_WIN32
    if(gethostname((char *)local_host,sizeof(local_host))==0
            &&netbiosnameGeted)
#else
    const char *cTempName="MACMINI-032D7D";
    strncpy((char *)local_host,cTempName,sizeof(local_host));
    if(netbiosnameGeted)
#endif
    {
        memset(callingname, 0, HOSTNAMELEN);
        memset(calledname, 0, HOSTNAMELEN);
        memset(callingname, ' ', HOSTNAMELEN-1);
        memset(calledname, ' ', HOSTNAMELEN-1);
        strncpy(callingname,local_host,strlen(local_host));
        strncpy(calledname,dest_host,strlen(dest_host));
        codename(callingname, session_request.callingname);
        codename(calledname, session_request.calledname);




        for(int i=0;i<NETBIOS_OSVERSION_RETRYTIMES;i++)
        {
          //  qDebug()<<"netbios retry "<<i<<QString(calledname);
        if(
                DO_AND_CHECKEXIT(tcp_sock.create())&&FLAG_NO_FUNCTION(poolExit)
                &&DO_AND_CHECKEXIT(tcp_sock.connect(ip,139))&&FLAG_NO_FUNCTION(poolExit)
                 &&DO_AND_CHECKEXIT(tcp_sock.waitForWriteReady(1000))&&FLAG_NO_FUNCTION(poolExit)
                &&DO_AND_CHECKEXIT( sizeof(session_request)==tcp_sock.sendData((char *)&session_request,sizeof(session_request)))
                    &&FLAG_NO_FUNCTION(poolExit)
                &&DO_AND_CHECKEXIT(tcp_sock.waitForReadReady(1000))&&FLAG_NO_FUNCTION(poolExit)
                &&DO_AND_CHECKEXIT((readed= tcp_sock.recvData(revc_buff,sizeof(revc_buff)) )>=0)&&FLAG_NO_FUNCTION(poolExit)
                &&DO_AND_CHECKEXIT(tcp_sock.waitForWriteReady(1000))&&FLAG_NO_FUNCTION(poolExit)
                &&DO_AND_CHECKEXIT(sizeof(negotiate)==tcp_sock.sendData(negotiate,sizeof(negotiate)))&&FLAG_NO_FUNCTION(poolExit)
                &&DO_AND_CHECKEXIT(tcp_sock.waitForReadReady(1000))&&FLAG_NO_FUNCTION(poolExit)
                &&DO_AND_CHECKEXIT((readed= tcp_sock.recvData(revc_buff,sizeof(revc_buff)) )>=0)&&FLAG_NO_FUNCTION(poolExit)
                &&DO_AND_CHECKEXIT(tcp_sock.waitForWriteReady(1000))&&FLAG_NO_FUNCTION(poolExit)
                &&DO_AND_CHECKEXIT(sizeof(setup_account)==tcp_sock.sendData(setup_account,sizeof(setup_account)) )&&FLAG_NO_FUNCTION(poolExit)
                &&DO_AND_CHECKEXIT(tcp_sock.waitForReadReady(1000))&&FLAG_NO_FUNCTION(poolExit)
                &&DO_AND_CHECKEXIT((readed= tcp_sock.recvData(revc_buff,sizeof(revc_buff)))>0)&&FLAG_NO_FUNCTION(poolExit)
                    )
            {
                quint32 itemp=0;
                while((--readed >0) && (itemp<4))
                {
                    if((0x00==revc_buff[readed] )&& (3==itemp++))
                    {
                        char cVersion[64]={0};
                        strncpy(cVersion,&revc_buff[readed+1],sizeof(cVersion));
                        qstemp=QString((char *)cVersion);
                        if(!qstemp.isEmpty())
                        {
                            retmap.insert(NIE_NETBIOS_VERSION,qstemp);
                        }
                    }
                }
                tcp_sock.close();
                break;

            }
            tcp_sock.close();
            SLEEP(100);
        }//for
    }






}

void SVT_QtUtils::netscanGetAllAdpInfo(QList<SVT_NetworkInterface> &resultlst)
{
    // qDebug()<<"SVT_QtUtils::netscanGetAllAdpInfo";
    resultlst.clear();
#ifdef Q_OS_WIN32
    NICLIST *niclist=NULL;
    bool btemp=mWin32Helper->_enumInterfaces(&niclist);
    if(!btemp ||!niclist)
    {
        return;
    }
    for(int i=0;i<niclist->dwCount;i++)
    {
        LPNIC lpnic=&(niclist->lpNics[i]);
        if(1!=lpnic->m_dwNicType && 2!=lpnic->m_dwNicType )
        {
            //skip 1394
            continue;
        }
        SVT_NetworkInterface iface;
        iface.m_name= QString::fromUtf16((ushort *)(lpnic->m_chDevDesc));
        iface.m_flags |= (lpnic->m_dwNicType==2)?SVT_NetworkInterface::NF_WIRELESS:0;
        iface.m_flags |=(!lpnic->m_bDisabled)?SVT_NetworkInterface::NF_ON:0;
        QString uuid=QString::fromUtf16((ushort *)(lpnic->m_chDevGuid));
        iface.m_idx=getAdpIdx(uuid);
        resultlst.append(iface);

    }
    mWin32Helper->_freeNicListBuffer(niclist);

#if 1
    char stemp[2048*2];
    struct AllAdapters *pAll=(struct AllAdapters*)stemp;
    if(mWin32Helper->_allAdaptInfo(pAll,sizeof(stemp)))
    {
        for(int i=0;i<pAll->m_count;i++)
        {
            Q_ASSERT(pAll->m_adpinfo);
            quint32 idx;
            //            QString name=QString::fromLocal8Bit((pAll->m_adpinfo)[i].m_adapter_name);
            idx=getAdpIdx(QString(pAll->m_adpinfo[i].m_adapter_uuid));//(pAll->m_adpinfo)[i].m_index;
            SVT_NetworkInterface *pIface=NULL;
            for(int j=0;j<resultlst.count();j++)
            {
                if(resultlst[j].m_idx == idx)
                {
                    pIface=&(resultlst[j]);
                }
            }
            if(pIface)
            {
                SVT_IpAddress ip;
                ip.Set((pAll->m_adpinfo)[i].m_ip);
                pIface->m_ip=ip;

                SVT_IpAddress mask;
                mask.Set((pAll->m_adpinfo)[i].m_mask);
                pIface->m_mask=mask;

                SVT_IpAddress gateway;
                gateway.Set((pAll->m_adpinfo)[i].m_gateway);
                pIface->m_gateway=gateway;

                SVT_MacAddress mac;
                mac.SetAddress((unsigned char *)(pAll->m_adpinfo[i].m_mac),6);
                pIface->m_mac=mac;

                if((pAll->m_adpinfo)[i].m_default)
                {
                    pIface->m_flags |=SVT_NetworkInterface::NF_DEFALT;
                }

            }
        }
    }
#endif
#else
    QStringList datalst;
    SVT_UnixProcessHelper process;
    process.doCmd("./ifaceinfo.sh",datalst);
    bool nameok,ipok,macok,gatewayok,maskok,stateok,default_gatewayok;
    nameok=ipok=macok=gatewayok=maskok=stateok=default_gatewayok=false;
    SVT_IpAddress ip,mask,gateway,default_gateway;
    SVT_MacAddress mac;
    bool active=false;
    QString name;
    if(!datalst.isEmpty())
    {
        QStringList::const_iterator i;
        for (i = datalst.constBegin(); i != datalst.constEnd(); ++i)
        {
            if((*i).contains("--------"))
            {
#ifdef FIX_FIX_MACOS_10_5_BUG
                int version_temp;
                static bool is10p5=getOSVersion(&version_temp).contains("10.5")
                                   ||version_temp<1067;

                if(!stateok && is10p5)
                {
                    active=true;
                    stateok=true;
                }
#endif
                if(nameok /*&& ipok */&& macok /*&& gatewayok && maskok*/ && stateok)
                {

                    SVT_NetworkInterface iface;
                    quint32 flag=0;
                    if(active)
                    {
                        flag |= SVT_NetworkInterface::NF_ON;
                    }
                    if(gateway==default_gateway && gatewayok)
                    {
                        flag |= SVT_NetworkInterface::NF_DEFALT;
                    }
                    if(mMacOSHelper->isWireless(name))
                    {
                        flag |= SVT_NetworkInterface::NF_WIRELESS;
                    }
                    iface.set(getAdpIdx(name),name,ip,mask,gateway,mac,flag);
                    resultlst.append(iface);

                }
                nameok=ipok=macok=gatewayok=maskok=stateok=false;
                ip.Set((unsigned long)0);
                unsigned char ctemp[8]={0};
                mac.SetAddress(ctemp,sizeof(ctemp));
            }
            QStringList templst=(*i).split(" ",QString::SkipEmptyParts);
            if(2==templst.size())
            {
                if(templst[0]=="name")
                {
                    name=templst[1];
                    nameok=true;
                }
                else if(templst[0]=="mac")
                {
                    mac.SetAddress(templst[1]);
                    macok=true;
                }
                else if(templst[0]=="ip")
                {
                    ip.Set(templst[1]);
                    ipok=true;
                }
                else if(templst[0]=="mask")
                {
                    mask.Set(templst[1].toUInt(0,16));
                    maskok=true;
                }
                else if(templst[0]=="state")
                {
                    active=(templst[1]=="active");
                    stateok=true;
                }
                else if(templst[0]=="gateway")
                {
                    gateway.Set(templst[1]);
                    gatewayok=true;
                }
                else if(templst[0]=="defaultgateway")
                {
                    default_gateway.Set(templst[1]);
                    default_gatewayok=true;
                }
            }

        }
//            cout << *i << endl;
    }
#endif

}

void SVT_QtUtils::netscanLoopArpTable(quint32 iface,QList<SVT_IpAddress> &retlist)
{
#ifdef Q_OS_WIN32
	char stemp[2048]={0};
    int n=mWin32Helper->_loopArpTable(iface,(char *)stemp,sizeof(stemp));
	for(int i=0;i<n;i++)
	{
		SVT_IpAddress ip;
		ip.Set(*((unsigned long *)( ((char *)stemp) +sizeof(unsigned long)*i ) ) );
		retlist.append(ip);
		qDebug()<<ip.toString();

	}
#endif
}

void SVT_QtUtils::netgearGetCurrentSetting(const SVT_IpAddress &ip, QMap<int,QString> &retmap, bool *poolExit)
{
    SVT_Http http(poolExit?poolExit:&mCoreExit);
    QString url=QString("http://%1/currentsetting.htm").arg(ip.toString());
    QByteArray &temparray=http.getHttpResult(url);

    QTextStream stream(&temparray);
    QRegExp reFirmware("^Firmware=.*");
    QRegExp reRegionTag("^RegionTag=.*");
    QRegExp reRegion("^Region=.*");
    QRegExp reModel("^Model=.*");
    QRegExp reRSLevel("^ReadyShareSupportedLevel=.*");
    QRegExp vInfoMatch("(\\D+\\d+(\\.\\d+)*).*");

    qDebug () << "netgearGetCurrentSetting :temparray =" << temparray;

    while ( !stream.atEnd() ){
        QString _line=stream.readLine();
        //if(reMatch.exactMatch(line))
        //{
        QStringList split_with_slash_r=_line.split("\r");
        bool bHaveData=false;
        foreach(QString line,split_with_slash_r)
        {
            if(reFirmware.exactMatch(line))
            {

                if(vInfoMatch.indexIn(strSecondPart(line)) >-1 )
                {
                    retmap.insert(NIE_CURRENTSETTING_FIRMWARE,vInfoMatch.cap(1));;
                    qDebug () << "netgearGetCurrentSetting insert NIE_CURRENTSETTING_FIRMWARE";
                    bHaveData=true;
                }
            }
            else if(reRegionTag.exactMatch(line))
            {
                retmap.insert(NIE_CURRENTSETTING_REGIONTAG,strSecondPart(line));
                qDebug () << "netgearGetCurrentSetting insert NIE_CURRENTSETTING_REGIONTAG";
                bHaveData=true;

            }
            else if(reRegion.exactMatch(line))
            {
                retmap.insert(NIE_CURRENTSETTING_REGION,strSecondPart(line));
                qDebug () << "netgearGetCurrentSetting insert NIE_CURRENTSETTING_REGION";
                bHaveData=true;

            }
            else if(reModel.exactMatch(line))
            {
                 retmap.insert(NIE_CURRENTSETTING_MODEL,strSecondPart(line));
                qDebug () << "netgearGetCurrentSetting insert NIE_CURRENTSETTING_MODEL";
                bHaveData=true;

            }
            else if(reRSLevel.exactMatch(line))
            {
                retmap.insert(NIE_CURRENTSETTING_RSLEVEL,strSecondPart(line));
                qDebug () << "netgearGetCurrentSetting insert NIE_CURRENTSETTING_RSLEVEL";
                bHaveData=true;

            }
        }
        if(bHaveData)
        {
            retmap.insert(NIE_CURRENTSETTING_OK,"ok");
        }
    }


}

bool SVT_QtUtils::getRdpOk(const SVT_IpAddress &ip)
{
    return testPort(ip,3389);
}

bool SVT_QtUtils::getPuttyOk(const SVT_IpAddress &ip)
{
    return testPort(ip,22);
}

void SVT_QtUtils::netgearGetFirmwareupdateInfo(const QString &para,QMap<int,QString> &retmap)
{
//	QString ret="";
    QHostInfo hinfo = QHostInfo::fromName("updates1.netgear.com");
	const size_t cbuff_size=1024*8;
	char *revc_buff=new char[cbuff_size];
    if (revc_buff && hinfo.error() == QHostInfo::NoError && !hinfo.addresses().isEmpty())
    {
        quint32 ip=hinfo.addresses()[0].toIPv4Address();
        QString request="GET %1fileinfo.txt HTTP/1.1\r\n"
                "Host: updates1.netgear.com\r\n"
//                //"User-Agent: Mozilla/5.0 (Windows NT 6.1; rv:6.0.2) Gecko/20100101 Firefox/6.0.2\r\n"
   //             "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
//                "Accept-Language: en-us,en;q=0.5\r\n"
//                "Accept-Encoding:gzip, deflate\r\n"
//                "Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\n"
//                "Connection: keep-alive\r\n"
//                "If-Modified-Since: Wed, 09 Feb 2011 14:18:49 GMT\r\n"
//                "If-None-Match: \"49317310\"\r\n"
//                "Cache-Control: max-age=0\r\n\r\n"
                "\r\n";
        request=request.arg(para.toLower());
        const QByteArray reqarray=request.toLatin1();
        SVT_TcpSocket tcp_sock(&mCoreExit);

        int readed=0;
        if(
                // DO_AND_CHECKEXIT(!retmap.contains(NIE_NETBIOS_VERSION))
                DO_AND_CHECKEXIT(tcp_sock.create())
                &&DO_AND_CHECKEXIT(tcp_sock.connect(ip,80))
                 &&DO_AND_CHECKEXIT(tcp_sock.waitForWriteReady(1000))
                &&DO_AND_CHECKEXIT( reqarray.size()==tcp_sock.sendData(reqarray.data(),reqarray.size()) )
                &&DO_AND_CHECKEXIT(tcp_sock.waitForReadReady(1000))
                &&DO_AND_CHECKEXIT((readed= tcp_sock.recvData(revc_buff,4096) )>=0)
                )
        {
            for(int i=0;i<100 && !mCoreExit;i++)
            {
                int readedtemp=-1;
                if(tcp_sock.waitForReadReady(100))
                {
					const size_t cursize=MIN(cbuff_size-readed,4096);
                    readedtemp=tcp_sock.recvData(revc_buff+readed,cursize);
                }

                if(readedtemp >=0)
                {
                    readed+=readedtemp;
                    if(readed >= sizeof(revc_buff))
                    {
                        break;
                    }
                }
                else
                {
                    break;
                }

            }
            revc_buff[readed]='\0';
            revc_buff[cbuff_size-1]='\0';
            const char finder[]="\r\n\r\n";
            char *c=strstr(revc_buff,finder);

            if(c!=0)
            {
                // QString qstemp=QString((char *)(c+strlen(finder)));
                QByteArray temparray((char *)(c+strlen(finder)),cbuff_size-(c-revc_buff)-strlen(finder));
                QTextStream stream(&temparray);
                QRegExp vMatchVer("file=.*-(\\D+\\d+(\\.\\d+)*).*(chk|img)$",Qt::CaseInsensitive);
                QRegExp vMatchMd5("md5=.*",Qt::CaseInsensitive);
                QRegExp vMatchSize("size=.*",Qt::CaseInsensitive);
                const quint32 fileNotOk=(1<<0);
                const quint32 md5NotOk=(1<<1);
                const quint32 sizeNotOk=(1<<2);
                quint32 flag=fileNotOk|md5NotOk|sizeNotOk;
                while ( flag && !stream.atEnd() ){
                    QString _line = stream.readLine();
                    QStringList split_with_slash_r=_line.split("\r");
                    foreach(QString line,split_with_slash_r)
                    {
                        if ( vMatchVer.exactMatch(line) ){
                            retmap.insert(NIE_UPDATE1_VERSION,vMatchVer.cap(1));
                            QString addr=QString("http://updates1.netgear.com%1%2")
                                    .arg(para.toLower(),strSecondPart(line));
                            retmap.insert(NIE_UPDATE1_ADDR,addr);
                            flag &= ~fileNotOk;
                        }
                        if(vMatchMd5.exactMatch(line))
                        {
                            retmap.insert(NIE_UPDATE1_MD5,strSecondPart(line));
                            flag &=~md5NotOk;
                        }
                        if(vMatchSize.exactMatch(line))
                        {
                            retmap.insert(NIE_UPDATE1_SIZE,strSecondPart(line));
                            flag &=~sizeNotOk;
                        }
                        if(0==flag)
                        {
                            break;
                        }
                    }

                }
            }

        }
        tcp_sock.close();



    }
    delete []revc_buff;
}

QString SVT_QtUtils::getOSVersion(int *ivalue)
{

    static QString sCache="null";
    static int iCache=0;
    if(ivalue)
    {
        *ivalue=iCache;
    }
    if(sCache!="null")
    {
        return sCache;
    }
#ifdef Q_OS_WIN32
    OSVERSIONINFO info ;
    info.dwOSVersionInfoSize = sizeof (info) ;
    GetVersionEx (&info) ;
    QString ret;
    ret.sprintf("%d.%d Build %d %s",
                        info.dwMajorVersion,info.dwMinorVersion,(int)(info.dwBuildNumber & 0xFFFF),info.szCSDVersion);
    sCache=ret;
    return ret;
#else
    SVT_UnixProcessHelper process;
    QStringList retlst;
    if(process.doCmd("sw_vers",retlst) && retlst.count() >=3)
    {
        QStringList templst=retlst[1].split(QRegExp("\\s+"),QString::SkipEmptyParts);
        if(templst.count() >=2)
        {
            QString stemp=templst[1];
            stemp=stemp.replace(".","").trimmed();
            iCache=stemp.toInt();
            if(ivalue)
            {
                *ivalue=iCache;
            }
            sCache= QString("Mac OS X ")+templst[1];
            return sCache;
        }
    }

    return "macos";
#endif
}

bool SVT_QtUtils::testPort(const SVT_IpAddress &ip, int port)
{
    bool ret=false;
    SVT_TcpSocket tcp_sock(&mCoreExit);
    if(
            // DO_AND_CHECKEXIT(!retmap.contains(NIE_NETBIOS_VERSION))
            DO_AND_CHECKEXIT(tcp_sock.create())
            &&DO_AND_CHECKEXIT(tcp_sock.connect(ip,port))
             &&(DO_AND_CHECKEXIT(tcp_sock.waitForWriteReady(1000))
                ||DO_AND_CHECKEXIT(tcp_sock.waitForWriteReady(1000)))
            )
    {
        ret=true;
    }
    tcp_sock.close();
    return ret;
}




