#ifndef __util_h__
#define __util_h__

#include "platform.h"
#include "thread.h"
#include <map>
#include <string>
#include <vector>

struct CONN_INFO
{
	uint32_t localAddr;
	uint32_t remoteAddr;
	uint16_t localPort;
	uint16_t remotePort;
};

struct CONN_DETAIL
{
	uint64_t totalIn;
	uint64_t totalOut;
};

struct LessOp_ConnInfo
{
	bool operator()(const CONN_INFO& l, const CONN_INFO& r) const
	{
		return memcmp(&l, &r, sizeof(CONN_INFO)) < 0;
	}
};

typedef std::map<CONN_INFO, CONN_DETAIL, LessOp_ConnInfo> ConnDetailMap;

class TrafficCollector
	: public GTF::Mutex
{
public:
	TrafficCollector();
	~TrafficCollector();

	void Incoming(uint32_t localAddr, uint16_t localPort, uint32_t remoteAddr, uint16_t remotePort, uint32_t totalLength);
	void Outgoing(uint32_t localAddr, uint16_t localPort, uint32_t remoteAddr, uint16_t remotePort, uint32_t totalLength);
	void Take(ConnDetailMap& data);

private:
	ConnDetailMap m_data;
};

struct PRIV_TCP_DETAIL
{
	uint64_t totalIn;
	uint64_t totalOut;
	uint64_t deltaIn;
	uint64_t deltaOut;
	bool DeleteFlag;
};

struct PRIV_UDP_DETAIL
{
	uint64_t totalIn;
	uint64_t totalOut;
	uint64_t deltaIn;
	uint64_t deltaOut;
	bool DeleteFlag;
};

typedef std::map<CONN_INFO, PRIV_TCP_DETAIL, LessOp_ConnInfo> PrivTcpDetailMap;
typedef std::map<CONN_INFO, PRIV_UDP_DETAIL, LessOp_ConnInfo> PrivUdpDetailMap;

struct PRIV_APP_DETAIL
{
	uint64_t totalTcpIn;
	uint64_t totalTcpOut;
	uint64_t deltaTcpIn;
	uint64_t deltaTcpOut;
	uint64_t totalUdpIn;
	uint64_t totalUdpOut;
	uint64_t deltaUdpIn;
	uint64_t deltaUdpOut;
	bool DeleteFlag;

	std::string moduleFullPath;
	std::string moduleName;
	uint32_t appIconWidth;
	uint32_t appIconHeight;
	uint32_t appIconPitch;
	std::vector<uint8_t> appIconData;

	PrivTcpDetailMap tcpMap;
	PrivUdpDetailMap udpMap;
};

struct NM_CONNECTION_TCP
{
	uint32_t localAddr;
	uint32_t localPort;
	uint32_t remoteAddr;
	uint32_t remotePort;
	uint64_t totalIn;
	uint64_t totalOut;
	uint64_t deltaIn;
	uint64_t deltaOut;
};

struct NM_CONNECTION_UDP
{
	uint32_t localAddr;
	uint32_t localPort;
	uint32_t remoteAddr;
	uint32_t remotePort;
	uint64_t totalIn;
	uint64_t totalOut;
	uint64_t deltaIn;
	uint64_t deltaOut;
};

struct NM_APP_DETAIL
{
	uint32_t pid;
	uint32_t rsv0;
	uint32_t rsv1;
	uint32_t rsv2;
	uint32_t tcpCount;
	uint32_t tcpOffset;
	uint32_t udpCount;
	uint32_t udpOffset;
	uint32_t moduleNameOffset;
	uint32_t moduleNameLength;
	uint32_t moduleFullPathOffset;
	uint32_t moduleFullPathLength;
	uint64_t totalIn;
	uint64_t totalOut;
	uint64_t deltaIn;
	uint64_t deltaOut;
};

struct NM_SUMMARY
{
	uint32_t version;
	uint32_t deltaTime;
	uint32_t appDetailCount;
	uint32_t appDetailOffset;
	uint64_t totalTcpIn;
	uint64_t totalTcpOut;
	uint64_t deltaTcpIn;
	uint64_t deltaTcpOut;
	uint64_t totalUdpIn;
	uint64_t totalUdpOut;
	uint64_t deltaUdpIn;
	uint64_t deltaUdpOut;
};

struct ProcessInfo
{
	std::string moduleName;
	std::string modulePath;
};

typedef std::map<uint32_t, ProcessInfo> ProcessMap;

struct AppInfo
{
	uint32_t appIconWidth;
	uint32_t appIconHeight;
	uint32_t appIconPitch;
	std::vector<uint8_t> appIconData;
};

typedef std::map<uint32_t, AppInfo> AppInfoMap;

class TrafficReport
{
public:
	TrafficReport();
	~TrafficReport();

	void BeginUpdate(ProcessMap *processMap);
	void EndUpdate();

	void UpdateUdp(const CONN_INFO& conn, const CONN_DETAIL& detail, uint32_t pid);
	void UpdateUdpAny(const CONN_INFO& conn, const CONN_DETAIL& detail, uint32_t pid);
	void UpdateUdpMiss(const CONN_INFO& conn, const CONN_DETAIL& detail);
	void UpdateTcp(const CONN_INFO& conn, const CONN_DETAIL& detail, uint32_t pid, uint32_t state);
	void UpdateTcpMiss(const CONN_INFO& conn, const CONN_DETAIL& detail);

	void Serialize(std::vector<uint8_t>& outputBuffer, double dt);
	void CopyAppInfoMap(AppInfoMap& appInfoMap);

private:
	typedef std::map<uint32_t, PRIV_APP_DETAIL> AppDetailMap;
	ProcessMap *m_processMap;

	PRIV_APP_DETAIL& EnsureAppDetail(uint32_t pid);
	void UpdateUdpDetail(PRIV_APP_DETAIL& appDetail, const CONN_INFO& conn, const CONN_DETAIL& detail);
	void UpdateTcpDetail(PRIV_APP_DETAIL& appDetail, const CONN_INFO& conn, const CONN_DETAIL& detail);

	AppDetailMap m_appMap;
};

bool PS_ExtractAppIcon(uint32_t pid, const std::string& path, std::vector<uint8_t>& data, uint32_t& width, uint32_t& height, uint32_t& pitch);
bool PS_KillProcess(uint32_t pid);

#endif // __util_h__
