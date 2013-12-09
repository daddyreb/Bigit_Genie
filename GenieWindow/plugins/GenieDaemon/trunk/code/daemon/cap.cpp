#include "cap.h"
#include "logger.h"

#if defined(_WIN32) || defined(WIN32)
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

static const char g_icmp_data[] = {
	0x08, 0x00, 0x4d, 0x51, 0x00, 0x01, 0x00, 0x0a,
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
	'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
	'q', 'r', 's', 't', 'u', 'v', 'w', 'a',
	'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i'
};

void scanRange(uint32_t ip, int mask_bits)
{
	uint32_t mask = ~((1 << (32 - mask_bits)) - 1);
	uint32_t base = ip & mask;
	int count = 1 << (32 - mask_bits);

	struct sockaddr_in destAddr;
	memset(&destAddr, 0, sizeof(destAddr));
	destAddr.sin_family = AF_INET;

#ifdef WIN32
	SOCKET s = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
#else
	int s = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
#endif

	if (s != -1) {
		uint32_t addr = base;
		for (int i = 1; i < count; i++, addr++) {
			destAddr.sin_addr.s_addr = ntohl(addr);
			sendto(s, g_icmp_data, sizeof(g_icmp_data), 0, reinterpret_cast<struct sockaddr*>(&destAddr), sizeof(destAddr));
		}

#ifdef WIN32
		closesocket(s);
#else
		close(s);
#endif
	}
}

InterfaceProc::InterfaceProc(PacketCore *owner, pcap_t *fd, const std::set<uint32_t>& selfSet)
: m_owner(owner), m_fd(fd), m_selfSet(selfSet), m_workerThread(NULL)
{
}

InterfaceProc::~InterfaceProc()
{
	stop();
	if (m_fd) {
		pcap_close(m_fd);
		m_fd = NULL;
	}
}

bool InterfaceProc::start()
{
	if (m_workerThread) {
		return false;
	}

	m_workerThread = GTF::Thread::create(&InterfaceProc::__WorkerThreadProc, this);
	if (!m_workerThread) {
		return false;
	}

	return true;
}

void InterfaceProc::stop()
{
	if (m_workerThread) {
		if (m_fd) {
			pcap_breakloop(m_fd);
		}
		m_workerThread->join();
		delete m_workerThread;
		m_workerThread = NULL;
	}
}

void InterfaceProc::WorkerThreadProc()
{
	int retval = pcap_loop(m_fd, -1, &InterfaceProc::__packetHandler, reinterpret_cast<u_char*>(this));
	if (retval == -1) {
		m_owner->reportError();
	}
}

void InterfaceProc::__WorkerThreadProc(void *ctx)
{
	static_cast<InterfaceProc*>(ctx)->WorkerThreadProc();
}

void InterfaceProc::__packetHandler(u_char *user, const struct pcap_pkthdr *pkt_header, const u_char *pkt_data)
{
	reinterpret_cast<InterfaceProc*>(user)->packetHandler(pkt_header, pkt_data);
}

void InterfaceProc::packetHandler(const struct pcap_pkthdr *pkt_header, const u_char *pkt_data)
{
	if (pkt_header->caplen > 34 && ntohs(*reinterpret_cast<const u_short*>(pkt_data + 12)) == 0x0800) {
		// IPv4
		handle_ipv4(pkt_data + 14, pkt_header->caplen - 14, pkt_header->len);
	} else if (pkt_header->caplen > 34 /* TODO */ && ntohs(*reinterpret_cast<const u_short*>(pkt_data + 12)) == 0x0806) {
		// ARP
		handle_arp(pkt_data + 14, pkt_header->caplen - 14, pkt_header->len);
	}
}

void InterfaceProc::handle_ipv4(const uint8_t *data, uint32_t caplen, uint32_t len)
{
	int version = (data[0] & 0xF0) >> 4;
	int ihl = data[0] & 0x0F;

	if (version != 4 || ihl < 5) {
		return;
	}

	uint16_t totalLength = ntohs(*reinterpret_cast<const u_short*>(data + 2));
	uint8_t ttl = data[8];
	uint8_t protocol = data[9];
	uint16_t checksum = ntohs(*reinterpret_cast<const u_short*>(data + 10));
	uint32_t src_addr = ntohl(*reinterpret_cast<const u_long*>(data + 12));
	uint32_t dst_addr = ntohl(*reinterpret_cast<const u_long*>(data + 16));

	// TCP or UDP
	if (protocol == 6 || protocol == 17) {
		uint16_t src_port = ntohs(*reinterpret_cast<const u_short*>(data + ihl * 4));
		uint16_t dst_port = ntohs(*reinterpret_cast<const u_short*>(data + ihl * 4 + 2));

		bool bSrcSelf = m_selfSet.find(src_addr) != m_selfSet.end();
		bool bDstSelf = m_selfSet.find(dst_addr) != m_selfSet.end();
/*
		if (bSrcSelf) {
			if (bDstSelf) {
				// skip
			} else {
				// out
			}
		} else {
			if (bDstSelf) {
				// in
			} else {
				// in
			}
		}
*/
		if (bSrcSelf && bDstSelf) {
			return;
		}

		if (protocol == 6) {
			if (bSrcSelf) {
				m_owner->reportTcpOut(src_addr, src_port, dst_addr, dst_port, totalLength);
			} else {
				m_owner->reportTcpIn(dst_addr, dst_port, src_addr, src_port, totalLength);
			}
		} else {
			if (bSrcSelf) {
				m_owner->reportUdpOut(src_addr, src_port, dst_addr, dst_port, totalLength);
			} else {
				m_owner->reportUdpIn(dst_addr, dst_port, src_addr, src_port, totalLength);
			}
		}
	}
}

void InterfaceProc::handle_arp(const uint8_t *data, uint32_t caplen, uint32_t len)
{
	// TODO:
	//printf("ARP: %d %d\n", caplen, len);
}

PacketCore::PacketCore()
: m_statThread(NULL), m_errorFlag(false)
{
}

PacketCore::~PacketCore()
{
	stop();
}

bool PacketCore::start()
{
	if (m_statThread) {
		return false;
	}

	m_scanSet.clear();

	m_errorFlag = false;
	m_quit = false;
	m_statThread = GTF::Thread::create(&PacketCore::__statThreadProc, this);
	if (!m_statThread) {
		return false;
	}

	//const char *expr = "tcp or udp or arp";
	const char *expr = "tcp or udp";

	pcap_if_t *alldevs;
	char errbuf[PCAP_ERRBUF_SIZE];

	if (pcap_findalldevs(&alldevs, errbuf) == 0) {
		pcap_if_t *dev = alldevs;
		while (dev) {
			//printf("[%s]\n", dev->name);
			pcap_addr *dev_addr = dev->addresses;

			std::set<uint32_t> selfSet;

			while (dev_addr) {
				if (dev_addr->addr->sa_family == AF_INET) {
					struct sockaddr_in *inet_addr = reinterpret_cast<struct sockaddr_in*>(dev_addr->addr);
					selfSet.insert(ntohl(inet_addr->sin_addr.s_addr));
					//printf("\t[%s]\n", inet_ntoa(inet_addr->sin_addr));
				}
				dev_addr = dev_addr->next;
			}

			if (!selfSet.empty()) {
				pcap_t *fd = pcap_open_live(dev->name, 64, true, 10, errbuf);
				if (fd != NULL) {
					if (pcap_datalink(fd) == DLT_EN10MB) {
						bpf_program filter;
						if (pcap_compile(fd, &filter, expr, true, 0xFFFFFFFF) == 0) {
							pcap_setfilter(fd, &filter);
							pcap_freecode(&filter);
							m_allProc.push_back(new InterfaceProc(this, fd, selfSet));
							m_scanSet.insert(selfSet.begin(), selfSet.end());
						}
					} else {
						pcap_close(fd);
					}
				}
			}

			dev = dev->next;
		}
		pcap_freealldevs(alldevs);
	}
/*
	if (m_allProc.empty()) {
		m_quit = true;
		m_statThread->join();
		delete m_statThread;
		m_statThread = NULL;
		return false;
	}
*/
	m_scanThread = GTF::Thread::create(&PacketCore::__scanThreadProc, this);

	std::list<InterfaceProc*>::const_iterator it = m_allProc.begin();
	std::list<InterfaceProc*>::const_iterator ie = m_allProc.end();
	for (; it != ie; ++it) {
		InterfaceProc *proc = *it;
		proc->start();
	}

	snapshotIf(m_ifs);

	return true;
}

void PacketCore::stop()
{
	if (m_statThread) {
		std::list<InterfaceProc*>::const_iterator it = m_allProc.begin();
		std::list<InterfaceProc*>::const_iterator ie = m_allProc.end();
		for (; it != ie; ++it) {
			InterfaceProc *proc = *it;
			proc->stop();
			delete proc;
		}
		m_allProc.clear();

		m_quit = true;
		m_statThreadCond.signal();
		m_scanThreadCond.signal();

		m_scanThread->join();
		delete m_scanThread;
		m_scanThread = NULL;

		m_statThread->join();
		delete m_statThread;
		m_statThread = NULL;
	}
}

bool PacketCore::changed()
{
	std::vector<InterfaceEntry> ifs;
	snapshotIf(ifs);
	if (ifs.size() != m_ifs.size()) {
		return true;
	}

	if (!ifs.empty() && memcmp(&ifs[0], &m_ifs[0], sizeof(InterfaceEntry) * ifs.size()) != 0) {
		return true;
	}

	return false;
}

void PacketCore::snapshotIf(std::vector<InterfaceEntry>& vec)
{
	vec.clear();
	InterfaceEntry entry;

	pcap_if_t *alldevs;
	char errbuf[PCAP_ERRBUF_SIZE];

	LOGGER_DEBUG(("enumerating interfaces begin"));

	if (pcap_findalldevs(&alldevs, errbuf) == 0) {
		pcap_if_t *dev = alldevs;
		while (dev) {
			pcap_addr *dev_addr = dev->addresses;

			LOGGER_DEBUG(("if [%s]", dev->name));

			memset(&entry, 0, sizeof(entry));

			while (dev_addr) {
				if (dev_addr->addr->sa_family == AF_INET) {
					struct sockaddr_in *inet_addr = reinterpret_cast<struct sockaddr_in*>(dev_addr->addr);
					LOGGER_DEBUG(("\t[%s]", inet_ntoa(inet_addr->sin_addr)));
					if (entry.ip_count < sizeof(entry.ip) / sizeof(entry.ip[0])) {
						entry.ip[entry.ip_count++] = inet_addr->sin_addr.s_addr;
					}
				}
				dev_addr = dev_addr->next;
			}

			if (entry.ip_count > 0) {
				size_t len = strlen(dev->name);
				if (len >= sizeof(entry.name)) {
					len = sizeof(entry.name) - 1;
				}
				memcpy(entry.name, dev->name, len);
				vec.push_back(entry);
			}

			dev = dev->next;
		}
		pcap_freealldevs(alldevs);
	}

	LOGGER_DEBUG(("enumerating interfaces end"));
}

void PacketCore::__scanThreadProc(void *ctx)
{
	static_cast<PacketCore*>(ctx)->scanThreadProc();
}

void PacketCore::scanThreadProc()
{
#ifdef WIN32
#else
	for (;;) {
		m_scanThreadCond.timedWait(180000);
		if (m_quit) {
			break;
		}

		std::set<uint32_t>::const_iterator it = m_scanSet.begin();
		std::set<uint32_t>::const_iterator ie = m_scanSet.end();
		for (; it != ie; ++it) {
			scanRange(*it, 24);
		}
	}
#endif // WIN32
}

void PacketCore::__statThreadProc(void *ctx)
{
	static_cast<PacketCore*>(ctx)->statThreadProc();
}

void PacketCore::statThreadProc()
{
	GTF::ElapsedTimer elapsedTimer;

	for (;;) {
		m_statThreadCond.timedWait(1000);
		if (m_quit) {
			break;
		}

		process(elapsedTimer.restart());
	}
}

void PacketCore::process(double dt)
{
	ConnDetailMap detailUdp;
	m_collUdp.Take(detailUdp);

	ConnDetailMap detailTcp;
	m_collTcp.Take(detailTcp);

	std::list<ArpEntry> arpTable;

	m_nsengine.snapshot(arpTable, &m_report, detailTcp, detailUdp);

	std::list<ArpEntry>::const_iterator itA = arpTable.begin();
	std::list<ArpEntry>::const_iterator ieA = arpTable.end();
	for (; itA != ieA; ++itA) {
		const ArpEntry& ae = *itA;
		//printf("%x %x:%x:%x:%x:%x:%x\n", ae.ip, ae.mac[0], ae.mac[1], ae.mac[2], ae.mac[3], ae.mac[4], ae.mac[5]);
	}

	std::vector<uint8_t> bufferArp;
	serializeArpTable(arpTable, bufferArp);

	{
		GTF::WriterLocker locker(&m_bufferArpLock);
		m_bufferArp = bufferArp;
	}

	std::vector<uint8_t> buffer;
	m_report.Serialize(buffer, dt);

	{
		GTF::WriterLocker locker(&m_bufferNetLock);
		m_bufferNet = buffer;
	}

	AppInfoMap appInfoMap;
	m_report.CopyAppInfoMap(appInfoMap);

	{
		GTF::WriterLocker locker(&m_appInfoMapLock);
		m_appInfoMap.swap(appInfoMap);
	}
}

void PacketCore::serializeArpTable(const std::list<ArpEntry>& arpTable, std::vector<uint8_t>& buffer)
{
	size_t cb = 4 + arpTable.size() * 10;
	buffer.resize(cb);
	void *data0 = &buffer[0];

	*static_cast<uint32_t*>(data0) = arpTable.size();
	uint8_t *ptr = reinterpret_cast<uint8_t*>(static_cast<uint32_t*>(data0) + 1);

	std::list<ArpEntry>::const_iterator it = arpTable.begin();
	std::list<ArpEntry>::const_iterator ie = arpTable.end();
	for (; it != ie; ++it, ptr += 10) {
		const ArpEntry& ae = *it;
		*reinterpret_cast<uint32_t*>(ptr) = ae.ip;
		memcpy(reinterpret_cast<uint32_t*>(ptr) + 1, ae.mac, 6);
	}
}

void PacketCore::reportError()
{
	m_errorFlag = true;
}

bool PacketCore::errorFlag() const
{
	return m_errorFlag;
}

void PacketCore::copyArpBuffer(std::vector<uint8_t>& buffer)
{
	GTF::ReaderLocker locker(&m_bufferArpLock);
	buffer = m_bufferArp;
}

void PacketCore::copyNetBuffer(std::vector<uint8_t>& buffer)
{
	GTF::ReaderLocker locker(&m_bufferNetLock);
	buffer = m_bufferNet;
}

bool PacketCore::findAppInfo(uint32_t pid, AppInfo& appInfo)
{
	GTF::ReaderLocker locker(&m_appInfoMapLock);
	AppInfoMap::const_iterator it = m_appInfoMap.find(pid);
	if (it != m_appInfoMap.end()) {
		appInfo = it->second;
		return true;
	}
	return false;
}

void PacketCore::reportTcpIn(uint32_t localAddr, uint16_t localPort, uint32_t remoteAddr, uint16_t remotePort, uint32_t totalLength)
{
	LOGGER_DEBUG(("reportTcpIn %x:%d->%x:%d [%d]\n", remoteAddr, remotePort, localAddr, localPort, totalLength));
	m_collTcp.Incoming(localAddr, localPort, remoteAddr, remotePort, totalLength);
}

void PacketCore::reportTcpOut(uint32_t localAddr, uint16_t localPort, uint32_t remoteAddr, uint16_t remotePort, uint32_t totalLength)
{
	LOGGER_DEBUG(("reportTcpOut %x:%d->%x:%d [%d]\n", localAddr, localPort, remoteAddr, remotePort, totalLength));
	m_collTcp.Outgoing(localAddr, localPort, remoteAddr, remotePort, totalLength);
}

void PacketCore::reportUdpIn(uint32_t localAddr, uint16_t localPort, uint32_t remoteAddr, uint16_t remotePort, uint32_t totalLength)
{
	LOGGER_DEBUG(("reportUdpIn %x:%d->%x:%d [%d]\n", remoteAddr, remotePort, localAddr, localPort, totalLength));
	m_collUdp.Incoming(localAddr, localPort, remoteAddr, remotePort, totalLength);
}

void PacketCore::reportUdpOut(uint32_t localAddr, uint16_t localPort, uint32_t remoteAddr, uint16_t remotePort, uint32_t totalLength)
{
	LOGGER_DEBUG(("reportUdpOut %x:%d->%x:%d [%d]\n", localAddr, localPort, remoteAddr, remotePort, totalLength));
	m_collUdp.Outgoing(localAddr, localPort, remoteAddr, remotePort, totalLength);
}
