#ifndef __ps_darwin_h__
#define __ps_darwin_h__

#include "ps.h"

class NetStatEngineImpl
{
public:
	NetStatEngineImpl();
	~NetStatEngineImpl();
	void snapshot(std::list<ArpEntry>& arpTable, TrafficReport *trafficReport, const ConnDetailMap& detailTcp, const ConnDetailMap& detailUdp);
};

struct PART_CONN_INFO
{
	uint32_t addr;
	uint32_t port;
};

struct LessOp_PartConnInfo
{
	bool operator()(const PART_CONN_INFO& l, const PART_CONN_INFO& r) const
	{
		return memcmp(&l, &r, sizeof(PART_CONN_INFO)) < 0;
	}
};

typedef std::map<PART_CONN_INFO, uint32_t, LessOp_PartConnInfo> UdpProcessMap;
typedef std::map<CONN_INFO, uint32_t, LessOp_ConnInfo> TcpProcessMap;

class lsof_parser
{
public:
	lsof_parser();
	~lsof_parser();

	void parse(char *output, size_t length);
	UdpProcessMap& udpMap() { return m_udpMap; }
	TcpProcessMap& tcpMap() { return m_tcpMap; }
	ProcessMap& processMap() { return m_processMap; }

private:
	void parseLine(char *line_start, char *line_end);
	void parseTcp(char *line_start, char *line_end);
	void parseUdp(char *line_start, char *line_end);
	bool parseIpv4(char *line_start, char *line_end, uint32_t& ip, uint16_t& port);
	void collect();

private:
	int m_pid;
	bool m_tcp;
	ProcessInfo m_pinfo;

	UdpProcessMap m_udpMap;
	TcpProcessMap m_tcpMap;
	ProcessMap m_processMap;
};

class socketinfo_collector
{
public:
	socketinfo_collector();
	~socketinfo_collector();

	void collect();
	UdpProcessMap& udpMap() { return m_udpMap; }
	TcpProcessMap& tcpMap() { return m_tcpMap; }
	ProcessMap& processMap() { return m_processMap; }

private:
	void collectSocketInfo(int pid);

private:
	UdpProcessMap m_udpMap;
	TcpProcessMap m_tcpMap;
	ProcessMap m_processMap;
};

#endif // __ps_darwin_h__
