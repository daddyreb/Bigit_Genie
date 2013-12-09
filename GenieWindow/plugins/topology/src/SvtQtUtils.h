#ifndef SVTWIN32QTUTILS_H
#define SVTWIN32QTUTILS_H

class SVT_MacAddress;
class SVT_IpAddress;
class SVT_NetworkInterface;
class SVT_Core;
class QLibrary;
class QString;
class SVT_UtilWin32Helper;
class SVT_UtilMacOSHelper;



class SVT_QtUtils
{
public:
    SVT_QtUtils(SVT_Core *core);
    ~SVT_QtUtils();
    virtual SVT_MacAddress netscanGetMacForIp(const SVT_IpAddress &ip,const SVT_IpAddress &srcip,bool *ok=NULL) const;
    virtual SVT_MacAddress netscanGetMacForRouter(const SVT_IpAddress &ip,const SVT_IpAddress &srcip,bool *ok=NULL) const;

    virtual void afpGetDevInfo(const SVT_IpAddress &ip, QMap<int,QString> &retmap, bool *poolExit=NULL) const;
    virtual void netbiosGetDevInfo(const SVT_IpAddress &ip, QMap<int,QString> &retmap, bool *poolExit=NULL) const;
    virtual void netscanGetAllAdpInfo(QList<SVT_NetworkInterface> &resultlst);
    virtual void netscanLoopArpTable(quint32 iface,QList<SVT_IpAddress> &retlist);
    virtual void netgearGetCurrentSetting(const SVT_IpAddress &ip,QMap<int,QString> &retmap,bool *poolExit=NULL);
    virtual bool getRdpOk(const SVT_IpAddress &ip);
    virtual bool getPuttyOk(const SVT_IpAddress &ip);
    virtual void netgearGetFirmwareupdateInfo(const QString &para,QMap<int,QString> &retmap);
    virtual QString getOSVersion(int *ivalue=NULL);
//    void start();
//    void stop();

//    bool mStarted;

    QHash<quint32,QString> mAdpIdxHashName;
    QString getAdpName(quint32 idx);
    quint32 getAdpIdx(const QString &name);
#ifdef Q_OS_WIN32
    SVT_UtilWin32Helper *mWin32Helper;
#else
    SVT_UtilMacOSHelper *mMacOSHelper;
#endif
#ifdef Q_OS_MACX

    bool netscanCheckIps(const QList<quint32> &,quint32 adp_idx
                         ,QList<quint32> & ret_exist,QList<quint32> & ret_offline
                         ,QList<SVT_MacAddress> &ret_macs);
#endif
    bool isPhysicalNic(quint32 adp_idx);
    bool &mCoreExit;
protected:
    bool testPort(const SVT_IpAddress &ip,int port);
};

#endif // SVTWIN32QTUTILS_H
