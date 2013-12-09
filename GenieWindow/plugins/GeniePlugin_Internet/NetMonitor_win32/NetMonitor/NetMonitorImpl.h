#ifndef __NetMonitorImpl_h__
#define __NetMonitorImpl_h__

#include "NetMonitor.h"

class CCritSec
	: public CRITICAL_SECTION
{
public:
	CCritSec();
	~CCritSec();

	void Enter();
	void Leave();
};

class CAutoLock
{
public:
	CAutoLock(CRITICAL_SECTION *pcs);
	~CAutoLock();

private:
	CRITICAL_SECTION *m_pcs;
};

class CTrafficReport;

class CNetMonitorWinPcap
{
public:
	CNetMonitorWinPcap();
	~CNetMonitorWinPcap();

	void Start();
	void Stop();
	nmStatus Snapshot(void *buffer, int *bufferSize);
	nmStatus GetAppIcon(int pid, void **handle);
	nmStatus ReleaseAppIcon(void *handle);
	nmStatus TestLoop();

private:
	bool IsWorkerThreadRunning() const;
	bool StartWorkerThread();
	void StopWorkerThread();
	void WorkerThreadProc();
	static unsigned __stdcall __WorkerThreadProc(void *ctx);

private:
	HANDLE m_hWorkerThread;
	bool m_bWorkerThreadQuit;
	nmStatus m_loopStatus;

	CCritSec m_lock;
	std::vector<BYTE> m_snapshot;
	std::auto_ptr<CTrafficReport> m_trafficReport;
};

struct CONN_INFO
{
	DWORD localAddr;
	DWORD remoteAddr;
	WORD localPort;
	WORD remotePort;
};

struct PART_CONN_INFO
{
	DWORD addr;
	DWORD port;
};

struct CONN_DETAIL
{
	ULONGLONG totalIn;
	ULONGLONG totalOut;
};

struct LessOp_ConnInfo
{
	bool operator()(const CONN_INFO& l, const CONN_INFO& r) const
	{
		return memcmp(&l, &r, sizeof(CONN_INFO)) < 0;
	}
};

struct LessOp_PartConnInfo
{
	bool operator()(const PART_CONN_INFO& l, const PART_CONN_INFO& r) const
	{
		return memcmp(&l, &r, sizeof(PART_CONN_INFO)) < 0;
	}
};

typedef std::map<CONN_INFO, CONN_DETAIL, LessOp_ConnInfo> ConnDetailMap;

class CMonitorProc;

class CInterfaceProc
{
public:
	CInterfaceProc(CMonitorProc *owner, pcap_t *fd);
	~CInterfaceProc();

	bool Start();
	void Stop();

private:
	static unsigned __stdcall __WorkerThreadProc(void *ctx);
	static void __packetHandler(u_char *user, const struct pcap_pkthdr *pkt_header, const u_char *pkt_data);

	void WorkerThreadProc();
	void packetHandler(const struct pcap_pkthdr *pkt_header, const u_char *pkt_data);
	void handle_ipv4(const BYTE *data, UINT caplen, UINT len);

private:
	CMonitorProc *m_owner;
	pcap_t *m_fd;
	HANDLE m_hWorkerThread;
};

class CTrafficCollector
	: public CCritSec
{
public:
	CTrafficCollector();
	~CTrafficCollector();

	void Incoming(DWORD localAddr, WORD localPort, DWORD remoteAddr, WORD remotePort, UINT totalLength);
	void Outgoing(DWORD localAddr, WORD localPort, DWORD remoteAddr, WORD remotePort, UINT totalLength);
	void Take(ConnDetailMap& data);

private:
	ConnDetailMap m_data;
};

struct PRIV_TCP_DETAIL
{
	ULONGLONG totalIn;
	ULONGLONG totalOut;
	ULONGLONG deltaIn;
	ULONGLONG deltaOut;
	bool DeleteFlag;
};

struct PRIV_UDP_DETAIL
{
	ULONGLONG totalIn;
	ULONGLONG totalOut;
	ULONGLONG deltaIn;
	ULONGLONG deltaOut;
	bool DeleteFlag;
};

typedef std::map<CONN_INFO, PRIV_TCP_DETAIL, LessOp_ConnInfo> PrivTcpDetailMap;
typedef std::map<CONN_INFO, PRIV_UDP_DETAIL, LessOp_ConnInfo> PrivUdpDetailMap;

struct PRIV_APP_DETAIL
{
	ULONGLONG totalTcpIn;
	ULONGLONG totalTcpOut;
	ULONGLONG deltaTcpIn;
	ULONGLONG deltaTcpOut;
	ULONGLONG totalUdpIn;
	ULONGLONG totalUdpOut;
	ULONGLONG deltaUdpIn;
	ULONGLONG deltaUdpOut;
	bool DeleteFlag;

	std::string moduleFullPath;
	std::string moduleName;

	PrivTcpDetailMap tcpMap;
	PrivUdpDetailMap udpMap;
};

class CTrafficReport
{
public:
	CTrafficReport();
	~CTrafficReport();

	void BeginUpdate();
	void EndUpdate();

	void UpdateUdp(const CONN_INFO& conn, const CONN_DETAIL& detail, DWORD pid);
	void UpdateUdpAny(const CONN_INFO& conn, const CONN_DETAIL& detail, DWORD pid);
	void UpdateUdpMiss(const CONN_INFO& conn, const CONN_DETAIL& detail);
	void UpdateTcp(const CONN_INFO& conn, const CONN_DETAIL& detail, DWORD pid, DWORD state);
	void UpdateTcpMiss(const CONN_INFO& conn, const CONN_DETAIL& detail);

	void Serialize(std::vector<BYTE>& outputBuffer, DWORD dt);

private:
	typedef std::map<DWORD, PRIV_APP_DETAIL> AppDetailMap;
	typedef std::map<DWORD, PROCESSENTRY32> ProcessEntryMap;

	PRIV_APP_DETAIL& EnsureAppDetail(DWORD pid);
	void UpdateUdpDetail(PRIV_APP_DETAIL& appDetail, const CONN_INFO& conn, const CONN_DETAIL& detail);
	void UpdateTcpDetail(PRIV_APP_DETAIL& appDetail, const CONN_INFO& conn, const CONN_DETAIL& detail);

	AppDetailMap m_appMap;
	ProcessEntryMap m_processMap;
};

class CMonitorProc
{
public:
	CMonitorProc();
	~CMonitorProc();

	bool Init();
	void Term();
	void Process(std::vector<BYTE>& buffer, DWORD dt, bool updateIp, CTrafficReport *trafficReport);

	void ReportTcpIn(DWORD localAddr, WORD localPort, DWORD remoteAddr, WORD remotePort, UINT totalLength);
	void ReportTcpOut(DWORD localAddr, WORD localPort, DWORD remoteAddr, WORD remotePort, UINT totalLength);
	void ReportUdpIn(DWORD localAddr, WORD localPort, DWORD remoteAddr, WORD remotePort, UINT totalLength);
	void ReportUdpOut(DWORD localAddr, WORD localPort, DWORD remoteAddr, WORD remotePort, UINT totalLength);

	bool IsSelf(DWORD addr) const;
	void ReportError();
	bool ErrorFlag() const;

private:
	void packetHandler(const struct pcap_pkthdr *pkt_header, const u_char *pkt_data);
	void handle_ipv4(const BYTE *data, UINT caplen, UINT len);

	std::list<CInterfaceProc*> m_allProc;
	CTrafficCollector m_collTcp;
	CTrafficCollector m_collUdp;
	std::set<DWORD> m_selfSet;
	bool m_errorFlag;
};

struct NM_CONNECTION_TCP
{
	DWORD localAddr;
	DWORD localPort;
	DWORD remoteAddr;
	DWORD remotePort;
	ULONGLONG totalIn;
	ULONGLONG totalOut;
	ULONGLONG deltaIn;
	ULONGLONG deltaOut;
};

struct NM_CONNECTION_UDP
{
	DWORD localAddr;
	DWORD localPort;
	DWORD remoteAddr;
	DWORD remotePort;
	ULONGLONG totalIn;
	ULONGLONG totalOut;
	ULONGLONG deltaIn;
	ULONGLONG deltaOut;
};

struct NM_APP_DETAIL
{
	DWORD pid;
	DWORD rsv0;
	DWORD rsv1;
	DWORD rsv2;
	DWORD tcpCount;
	DWORD tcpOffset;
	DWORD udpCount;
	DWORD udpOffset;
	DWORD moduleNameOffset;
	DWORD moduleNameLength;
	DWORD moduleFullPathOffset;
	DWORD moduleFullPathLength;
	ULONGLONG totalIn;
	ULONGLONG totalOut;
	ULONGLONG deltaIn;
	ULONGLONG deltaOut;
};

struct NM_SUMMARY
{
	DWORD version;
	DWORD deltaTime;
	DWORD appDetailCount;
	DWORD appDetailOffset;
	ULONGLONG totalTcpIn;
	ULONGLONG totalTcpOut;
	ULONGLONG deltaTcpIn;
	ULONGLONG deltaTcpOut;
	ULONGLONG totalUdpIn;
	ULONGLONG totalUdpOut;
	ULONGLONG deltaUdpIn;
	ULONGLONG deltaUdpOut;
};



#endif // __NetMonitorImpl_h__
