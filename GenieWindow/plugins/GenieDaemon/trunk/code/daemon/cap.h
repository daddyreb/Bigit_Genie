#ifndef __cap_h__
#define __cap_h__

#include "thread.h"
#include "ipc.h"
#include "ps.h"
#include "util.h"
#include <pcap.h>
#include <list>
#include <map>
#include <vector>
#include <set>
#include <string>

class PacketCore;

class InterfaceProc
{
public:
	InterfaceProc(PacketCore *owner, pcap_t *fd, const std::set<uint32_t>& selfSet);
	~InterfaceProc();

	bool start();
	void stop();

private:
	static void __WorkerThreadProc(void *ctx);
	void WorkerThreadProc();

	static void __packetHandler(u_char *user, const struct pcap_pkthdr *pkt_header, const u_char *pkt_data);
	void packetHandler(const struct pcap_pkthdr *pkt_header, const u_char *pkt_data);
	void handle_ipv4(const uint8_t *data, uint32_t caplen, uint32_t len);
	void handle_arp(const uint8_t *data, uint32_t caplen, uint32_t len);

private:
	PacketCore *m_owner;
	pcap_t *m_fd;
	std::set<uint32_t> m_selfSet;
	GTF::Thread *m_workerThread;
};

struct InterfaceEntry
{
	char name[128];
	uint32_t ip_count;
	uint32_t ip[16];
};

class PacketCore
{
public:
	PacketCore();
	~PacketCore();

	bool start();
	void stop();
	bool errorFlag() const;
	bool changed();

	// internal
	void reportError();
	void reportTcpIn(uint32_t localAddr, uint16_t localPort, uint32_t remoteAddr, uint16_t remotePort, uint32_t totalLength);
	void reportTcpOut(uint32_t localAddr, uint16_t localPort, uint32_t remoteAddr, uint16_t remotePort, uint32_t totalLength);
	void reportUdpIn(uint32_t localAddr, uint16_t localPort, uint32_t remoteAddr, uint16_t remotePort, uint32_t totalLength);
	void reportUdpOut(uint32_t localAddr, uint16_t localPort, uint32_t remoteAddr, uint16_t remotePort, uint32_t totalLength);

	void copyArpBuffer(std::vector<uint8_t>& buffer);
	void copyNetBuffer(std::vector<uint8_t>& buffer);
	bool findAppInfo(uint32_t pid, AppInfo& appInfo);

private:
	static void __statThreadProc(void *ctx);
	void statThreadProc();
	void process(double dt);

	static void __scanThreadProc(void *ctx);
	void scanThreadProc();

	void serializeArpTable(const std::list<ArpEntry>& arpTable, std::vector<uint8_t>& buffer);

	void snapshotIf(std::vector<InterfaceEntry>& vec);

private:
	std::list<InterfaceProc*> m_allProc;
	GTF::Thread *m_statThread;
	GTF::CondVar m_statThreadCond;
	GTF::Thread *m_scanThread;
	GTF::CondVar m_scanThreadCond;
	std::set<uint32_t> m_scanSet;
	bool m_errorFlag;
	bool m_quit;
	TrafficCollector m_collTcp;
	TrafficCollector m_collUdp;
	TrafficReport m_report;
	NetStatEngine m_nsengine;
	GTF::RWLock m_bufferArpLock;
	std::vector<uint8_t> m_bufferArp;
	GTF::RWLock m_bufferNetLock;
	std::vector<uint8_t> m_bufferNet;
	GTF::RWLock m_appInfoMapLock;
	AppInfoMap m_appInfoMap;
	std::vector<InterfaceEntry> m_ifs;
};

#endif // __cap_h__
