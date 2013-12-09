#ifndef DRAGONNETTOOL_H
#define DRAGONNETTOOL_H

//#include "getthread.h"

#include <QtCore/qglobal.h>

#ifdef DRAGONNETTOOL_LIB
# define DRAGONNETTOOL_EXPORT Q_DECL_EXPORT
#else
# define DRAGONNETTOOL_EXPORT Q_DECL_IMPORT
#endif

#include <QObject>
#include <QHostInfo>

#include <QProcess>

#include <QLibrary>
#include <QMap>
#include <QNetworkInterface>

//class GetThread;

#include "../getsystemdetails.h"

class DRAGONNETTOOL_EXPORT DragonNetTool : public QObject
{
    Q_OBJECT

public:
    DragonNetTool();
    ~DragonNetTool();

    //
    void DnsLookup(const QString &HostName, QStringList* *OutData);
    void Ping(const QString &HostName, QStringList* *OutData);
    int isPing(const QString &HostName);
    void TraceRoute(const QString &HostName, QStringList* *OutData);
    //void GetSysInfo(QMap<QString, QStringList>* *OutData);
	void GetSysInfoA( int &i)
	{
		i = 100;
	};
    void GetSysInfoNew(QMap<QString, QMap<QString, QStringList> > &OutData);
//	{
////		if(m_getsysinfo)
//			m_getsysinfo.SystemDetailsNew(OutData);
//		emit GetSysInfoFinish();
//	};

public slots:
    //about TraceRoute

private slots:
    //about DNSLookup
    void GetLookupData(const QHostInfo &Host);

    //about Ping
    void Resolved(const QHostInfo &Host);
    void NeedNotPing();
    void ExecutePing();
    void ReadyPingData();
    void ReadyPingLineData();
    void PingDataOver(int, QProcess::ExitStatus);

    //about TraceRoute
    void ReadyTraceLineData();
    void TraceDataOver(int, QProcess::ExitStatus);
    void NeedNotTrace();
    void ExecuteTrace();


signals:
    //about DNSLookup
    void DnsLookupFinish();

    //about Ping
    void PingFinish();
    void ReadPingData();
    void ResolvedSuccess();
    void ResolvedFailed();

    //about TraceToute
    void TraceRouteFinish();
    void ReadTraceRouteData();
    void ReadySuccess();
    void ReadyFailed();

    //about GetSysInfo
    void GetSysInfoFinish();


private:
    void ResolvedHost(const QString &DestHost);
    QString AddSpaceChar(QString ldata);
    std::list<std::string> splitTolist(std::string mString, char &mSplit);

    QString DefaultHost;
    QString EnterHost;
    QStringList *GetDataList;
    QString ToolType;

    int intReturn;
    QString TempData;


    QString CmdName;
    QString mCommand;

    //about Ping
    int times;
    unsigned int nTimeout;
    char *Desthost;
    QProcess *PingCmd;
    unsigned int mPingNum;
    QString strRowData;
    bool dFlag;

    //about TraceRoute
    QProcess *TraceCmd;
    QString compagesData;
    QString preData;


    //about GetSysInfo
    QLibrary *GetInformation;
    QString DllFilePath;
    std::list<std::string> QueryKeys;

    QMap<QString, QStringList> cDetailsMap;
    QStringList mDetails;



    QString DllNetwork;
    QLibrary *GetInterface;
    QMap<QString, QMap<QString, QStringList> > *InterfaceMap;
    QStringList netDetails;

    QNetworkInterface otherInfo;

  //  GetThread *mGetThread;
	GetSystemDetails m_getsysinfo;

};

#endif // DRAGONNETTOOL_H
