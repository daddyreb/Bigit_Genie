#ifndef QGENIEDATACONTAINNER_H
#define QGENIEDATACONTAINNER_H
#include <QtCore>
#include "Singleton.h"
//#include <QHttp>
///
#include <QLibrary>
#include "QGenieHttpRequestManager.h"
#define MAX_DATE_COUNT 7
#define USE_DLL_GETINFO 0
//class ISettings;
//#ifdef Q_OS_WIN32
//#define NETGEARGENIE_DIR (QDir::home().absolutePath()+QString("/NETGEARGenie/"))
//#else
//#define NETGEARGENIE_DIR (QDir::home().absolutePath()+QString("/.NETGEARGenie/"))
//#endif
#include <QDesktopServices>
//#define NETGEARGENIE_DIR (QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/NETGEARGenie/")
#define NETGEARGENIE_DIR (QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/")
#define FILE_OF_SEARCHMAP (NETGEARGENIE_DIR+"searchmap.dat")
#define RECORD_NO_NETGEARROUTER 1

#ifdef Q_OS_WIN32
//#include <WinSock2.h>
#include <IPTypes.h>


typedef void (*GetAdapterInfo) (struct AdapterInfo *adapter);

struct AdapterInfo
{
    unsigned long m_index;
    char	  m_local_mac[18];
    char	  m_adapter_name[MAX_ADAPTER_NAME_LENGTH + 4];
    char    	  m_gateway[16];
    char 	  m_local_ip[16];
    char       	  m_mask[16];
};
#endif
enum ADD_TYPE{
    AT_ADD,
    AT_REGMAX
};

struct tRouteInfo{
    QString routemac;
    QString localmac;
    QString modeln;
    QString fversion;
};


class QGenieHttpRequestManager;

class QGenieDataContainner:public QObject,public Singleton<QGenieDataContainner>
{
    Q_OBJECT
public:
    QGenieDataContainner(QObject *parent=NULL);
    ~QGenieDataContainner();
    void addClick(const QString &para,ADD_TYPE tp=AT_ADD,int c=1);
    void addSearchStr(const QString &str);
    inline const QMap<QDate,QMap<QString,int> *> &getClickMap()
    {
        return mClickCountMap;
    }
    inline const QMap<QString,QString> &getLocalInfoMap()
    {
        return mLocalInfo;
    }

    void serialize(bool load);
    QString getOSInfo();

protected:
    void serializeClickCount(bool load);
    void serializeRouteType(bool load);
    void initHash();
    void cleanClickCount();
    QHash<QString,QString> mShortHashDetail;
    QMap<QDate,QMap<QString,int> *> mClickCountMap;
    friend class QGenieStaticInterface;
    QTimer mTimer;
    QMap<int,QDate> mRequestIdMapDate;
    QMap<int,QString> mRequestIdMapRoutetype;

    QMap<QString,int> mRequestSearchMap;

    ///
    QLibrary *MyLib;
    QGenieHttpRequestManager *mHttpRequestManager;
    QMap<QString,tRouteInfo *> mRoutTypeMap;

    QDateTime mLastSendTime;
#if USE_DLL_GETINFO
    GetAdapterInfo m_getadapterinfo;
    QLibrary *m_myLib;
#endif
    int mTickerCounter;
    QMap<QString,QString> mLocalInfo;
    //    QUuid mSoftwareUuid;//compare with the installed uuid, if not the same
    //then send it to server
    int mRequestInstallInfoId;
//    int mRequestRouterInfoId;
    int mRequestSearchId;
    int mSearchMapCount;
    int mFirstCommitTimes;
    //    bool mInstallInfoCommited;
    void commitClickCount();
    void commitInstallInfo();
    void recordRouteInfo();
    void commitRouterInfo();
    void commitSearchData();
    void getLocalInfo();
    void genDbgData();
    int sendRequest(const QString &path);
protected slots:
//    void slot_RequestFinished ( int id, bool error ) ;
    void slot_Timeout();
//    void slot_ReadyRead(const QHttpResponseHeader & resp);
    void slot_Done(int id, bool error,QString str);

    void slot_curSetting(int, QStringList);
    void slot_macAddr(int, QStringList);


};

#endif // QGENIEDATACONTAINNER_H
