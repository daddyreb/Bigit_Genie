#include "stdafx.h"
#include "NetMonitorImpl.h"

std::string convertToUtf8(const WCHAR *text, int length)
{
	if (length < 0) {
		length = wcslen(text);
	}

	std::string result;
	result.resize(length * 6);

	int len = WideCharToMultiByte(CP_UTF8, 0, text, length, &result[0], result.length(), NULL, NULL);
	if (len > 0) {
		result.resize(len);
	} else {
		result.clear();
	}

	return result;
}

CCritSec::CCritSec()
{
	InitializeCriticalSection(this);
}

CCritSec::~CCritSec()
{
	DeleteCriticalSection(this);
}

void CCritSec::Enter()
{
	EnterCriticalSection(this);
}

void CCritSec::Leave()
{
	LeaveCriticalSection(this);
}

CAutoLock::CAutoLock(CRITICAL_SECTION *pcs)
: m_pcs(pcs)
{
	EnterCriticalSection(m_pcs);
}

CAutoLock::~CAutoLock()
{
	LeaveCriticalSection(m_pcs);
}

CNetMonitorWinPcap::CNetMonitorWinPcap()
: m_hWorkerThread(NULL), m_trafficReport(new CTrafficReport())
{
}

CNetMonitorWinPcap::~CNetMonitorWinPcap()
{
	Stop();
}

void CNetMonitorWinPcap::Start()
{
	if (!IsWorkerThreadRunning()) {
		StartWorkerThread();
	}
}

void CNetMonitorWinPcap::Stop()
{
	if (IsWorkerThreadRunning()) {
		StopWorkerThread();
	}
}

nmStatus CNetMonitorWinPcap::Snapshot(void *buffer, int *bufferSize)
{
	if (!buffer || !bufferSize) {
		return NM_ERR_NULL_PTR;
	}
	CAutoLock snapshotLock(&m_lock);
	int cb = m_snapshot.size();
	if (*bufferSize < cb) {
		*bufferSize = cb;
		return NM_ERR_NOT_ENOUGH_BUFFER;
	}

	*bufferSize = cb;
	if (cb > 0) {
		memcpy(buffer, &m_snapshot[0], cb);
	}
	return NM_OK;
}

nmStatus CNetMonitorWinPcap::GetAppIcon(int pid, void **handle)
{
	if (!handle) {
		return NM_ERR_NULL_PTR;
	}

	HICON hIcon = NULL;

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, pid);
	if (hProcess) {
		WCHAR szBuffer[MAX_PATH * 2 + 1];
		DWORD len = GetModuleFileNameEx(hProcess, NULL, szBuffer, _countof(szBuffer));
		if (len > 0) {
			hIcon = ExtractIcon(0, szBuffer, 0);
		}
		CloseHandle(hProcess);
	}

	if (!hIcon) {
		hIcon = LoadIcon(NULL, IDI_APPLICATION);
	}

	if (!hIcon) {
		return NM_ERR_UNKNOWN;
	}

	*reinterpret_cast<HICON*>(handle) = hIcon;

	return NM_OK;
}

nmStatus CNetMonitorWinPcap::ReleaseAppIcon(void *handle)
{
	DestroyIcon(reinterpret_cast<HICON>(handle));
	return NM_OK;
}

nmStatus CNetMonitorWinPcap::TestLoop()
{
	return m_loopStatus;
}

bool CNetMonitorWinPcap::IsWorkerThreadRunning() const
{
	return m_hWorkerThread != NULL;
}

bool CNetMonitorWinPcap::StartWorkerThread()
{
	if (m_hWorkerThread) {
		return false;
	}

	m_bWorkerThreadQuit = false;
	m_loopStatus = NM_OK;

	m_hWorkerThread = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, &CNetMonitorWinPcap::__WorkerThreadProc, this, 0, NULL));
	if (!m_hWorkerThread) {
		return false;
	}

	return true;
}

void CNetMonitorWinPcap::StopWorkerThread()
{
	if (m_hWorkerThread) {
		m_bWorkerThreadQuit = true;
		WaitForSingleObject(m_hWorkerThread, INFINITE);
		CloseHandle(m_hWorkerThread);
		m_hWorkerThread = NULL;
	}
}

unsigned CNetMonitorWinPcap::__WorkerThreadProc(void *ctx)
{
	static_cast<CNetMonitorWinPcap*>(ctx)->WorkerThreadProc();
	return 0;
}

void CNetMonitorWinPcap::WorkerThreadProc()
{
	CMonitorProc proc;
	if (!proc.Init()) {
		m_loopStatus = NM_ERR_UNKNOWN;
		return;
	}

	DWORD dwTickCount = GetTickCount();

	int ipcnt = 0;

	while (!m_bWorkerThreadQuit) {
		if (proc.ErrorFlag()) {
			m_loopStatus = NM_ERR_UNKNOWN;
			break;
		}

		Sleep(1000);
		DWORD dwTickCount2 = GetTickCount();
		std::vector<BYTE> buffer;
		proc.Process(buffer, dwTickCount2 - dwTickCount, ipcnt == 0, m_trafficReport.get());
		if (++ipcnt == 20) {
			ipcnt = 0;
		}
		CAutoLock snapshotLock(&m_lock);
		m_snapshot.swap(buffer);
		dwTickCount = dwTickCount2;
	}
}

CMonitorProc::CMonitorProc()
: m_errorFlag(false)
{
}

CMonitorProc::~CMonitorProc()
{
	Term();
}

bool CMonitorProc::Init()
{
	pcap_if_t *alldevs;
	char errbuf[PCAP_ERRBUF_SIZE];

	if (pcap_findalldevs(&alldevs, errbuf) == 0) {
		pcap_if_t *dev = alldevs;
		while (dev) {
			pcap_t *fd = pcap_open_live(dev->name, 64, TRUE, 10, errbuf);
			if (fd != NULL) {
				if (pcap_datalink(fd) == DLT_EN10MB) {
					bpf_program filter;
					if (pcap_compile(fd, &filter, "tcp or udp", TRUE, 0xFFFFFFFF) == 0) {
						pcap_setfilter(fd, &filter);
						pcap_freecode(&filter);
						m_allProc.push_back(new CInterfaceProc(this, fd));
					}
				} else {
					pcap_close(fd);
				}
			}
			dev = dev->next;
		}
		pcap_freealldevs(alldevs);
	}

	if (m_allProc.empty()) {
		return false;
	}

	std::list<CInterfaceProc*>::const_iterator it = m_allProc.begin();
	std::list<CInterfaceProc*>::const_iterator ie = m_allProc.end();
	for (; it != ie; ++it) {
		CInterfaceProc *proc = *it;
		proc->Start();
	}

	return true;
}

void CMonitorProc::Term()
{
	std::list<CInterfaceProc*>::const_iterator it = m_allProc.begin();
	std::list<CInterfaceProc*>::const_iterator ie = m_allProc.end();
	for (; it != ie; ++it) {
		CInterfaceProc *proc = *it;
		delete proc;
	}
	m_allProc.clear();
}

void CMonitorProc::Process(std::vector<BYTE>& buffer, DWORD dt, bool updateIp, CTrafficReport *trafficReport)
{
	ConnDetailMap detailUdp;
	m_collUdp.Take(detailUdp);

	ConnDetailMap detailTcp;
	m_collTcp.Take(detailTcp);

	if (updateIp) {
		m_selfSet.clear();

		BYTE buff[2048];
		MIB_IPADDRTABLE *pIpTable = reinterpret_cast<MIB_IPADDRTABLE*>(buff);
		ULONG buffSize = sizeof(buff);
		if (NO_ERROR == GetIpAddrTable(pIpTable, &buffSize, FALSE)) {
			for (DWORD i = 0; i < pIpTable->dwNumEntries; i++) {
				m_selfSet.insert(ntohl(pIpTable->table[i].dwAddr));
			}
		}
	}

	trafficReport->BeginUpdate();

	std::vector<BYTE> data(512 * 1024);

	typedef std::map<PART_CONN_INFO, DWORD, LessOp_PartConnInfo> PartConnMap;
	PartConnMap udpMap;

	MIB_UDPTABLE_OWNER_PID *pUdpTable = reinterpret_cast<MIB_UDPTABLE_OWNER_PID*>(&data[0]);
	DWORD dwSize = data.size();
	if (NOERROR == GetExtendedUdpTable(pUdpTable, &dwSize, TRUE, AF_INET, UDP_TABLE_OWNER_PID, 0)) {
		for (DWORD i = 0; i < pUdpTable->dwNumEntries; i++) {
			const MIB_UDPROW_OWNER_PID& row = pUdpTable->table[i];
			PART_CONN_INFO pci;
			pci.addr = ntohl(row.dwLocalAddr);
			pci.port = ntohs(static_cast<u_short>(row.dwLocalPort));
			udpMap.insert(PartConnMap::value_type(pci, row.dwOwningPid));
		}
	}

	{
		ConnDetailMap::const_iterator it = detailUdp.begin();
		ConnDetailMap::const_iterator ie = detailUdp.end();
		for (; it != ie; ++it) {
			const CONN_INFO& conn = it->first;
			const CONN_DETAIL& detail = it->second;
			PART_CONN_INFO pci;
			pci.addr = conn.localAddr;
			pci.port = conn.localPort;
			PartConnMap::const_iterator itM = udpMap.find(pci);
			if (itM != udpMap.end()) {
				trafficReport->UpdateUdp(conn, detail, itM->second);
			} else {
				pci.addr = 0;
				itM = udpMap.find(pci);
				if (itM != udpMap.end()) {
					trafficReport->UpdateUdpAny(conn, detail, itM->second);
				} else {
					trafficReport->UpdateUdpMiss(conn, detail);
				}
			}
		}
	}

	typedef std::map<CONN_INFO, MIB_TCPROW_OWNER_PID, LessOp_ConnInfo> TcpConnMap;
	TcpConnMap tcpMap;

	MIB_TCPTABLE_OWNER_PID *pTcpTable = reinterpret_cast<MIB_TCPTABLE_OWNER_PID*>(&data[0]);
	dwSize = data.size();
	if (NOERROR == GetExtendedTcpTable(pTcpTable, &dwSize, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0)) {
		for (DWORD i = 0; i < pTcpTable->dwNumEntries; i++) {
			const MIB_TCPROW_OWNER_PID& row = pTcpTable->table[i];
			CONN_INFO conn;
			conn.localAddr = ntohl(row.dwLocalAddr);
			conn.localPort = ntohs(static_cast<u_short>(row.dwLocalPort));
			conn.remoteAddr = ntohl(row.dwRemoteAddr);
			conn.remotePort = ntohs(static_cast<u_short>(row.dwRemotePort));
			tcpMap.insert(TcpConnMap::value_type(conn, row));
		}
	}

	{
		ConnDetailMap::const_iterator it = detailTcp.begin();
		ConnDetailMap::const_iterator ie = detailTcp.end();
		for (; it != ie; ++it) {
			const CONN_INFO& conn = it->first;
			const CONN_DETAIL& detail = it->second;
			TcpConnMap::iterator itM = tcpMap.find(conn);
			if (itM != tcpMap.end()) {
				trafficReport->UpdateTcp(conn, detail, itM->second.dwOwningPid, itM->second.dwState);
				tcpMap.erase(itM);
			} else {
				trafficReport->UpdateTcpMiss(conn, detail);
			}
		}

		CONN_DETAIL zeroDetail;
		zeroDetail.totalIn = 0;
		zeroDetail.totalOut = 0;
		TcpConnMap::const_iterator itM = tcpMap.begin();
		TcpConnMap::const_iterator ieM = tcpMap.end();
		for (; itM != ieM; ++itM) {
			if (itM->second.dwState == MIB_TCP_STATE_ESTAB) {
				trafficReport->UpdateTcp(itM->first, zeroDetail, itM->second.dwOwningPid, itM->second.dwState);
			}
		}
	}

	trafficReport->EndUpdate();

	trafficReport->Serialize(buffer, dt);
}

void CMonitorProc::ReportTcpIn(DWORD localAddr, WORD localPort, DWORD remoteAddr, WORD remotePort, UINT totalLength)
{
	m_collTcp.Incoming(localAddr, localPort, remoteAddr, remotePort, totalLength);
}

void CMonitorProc::ReportTcpOut(DWORD localAddr, WORD localPort, DWORD remoteAddr, WORD remotePort, UINT totalLength)
{
	m_collTcp.Outgoing(localAddr, localPort, remoteAddr, remotePort, totalLength);
}

void CMonitorProc::ReportUdpIn(DWORD localAddr, WORD localPort, DWORD remoteAddr, WORD remotePort, UINT totalLength)
{
	m_collUdp.Incoming(localAddr, localPort, remoteAddr, remotePort, totalLength);
}

void CMonitorProc::ReportUdpOut(DWORD localAddr, WORD localPort, DWORD remoteAddr, WORD remotePort, UINT totalLength)
{
	m_collUdp.Outgoing(localAddr, localPort, remoteAddr, remotePort, totalLength);
}

bool CMonitorProc::IsSelf(DWORD addr) const
{
	return m_selfSet.find(addr) != m_selfSet.end();
}

void CMonitorProc::ReportError()
{
	m_errorFlag = true;
}

bool CMonitorProc::ErrorFlag() const
{
	return m_errorFlag;
}

CInterfaceProc::CInterfaceProc(CMonitorProc *owner, pcap_t *fd)
: m_owner(owner), m_fd(fd), m_hWorkerThread(NULL)
{
}

CInterfaceProc::~CInterfaceProc()
{
	Stop();
	if (m_fd) {
		pcap_close(m_fd);
		m_fd = NULL;
	}
}

bool CInterfaceProc::Start()
{
	if (m_hWorkerThread) {
		return false;
	}

	m_hWorkerThread = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, &CInterfaceProc::__WorkerThreadProc, this, 0, NULL));
	if (!m_hWorkerThread) {
		return false;
	}

	return true;
}

void CInterfaceProc::Stop()
{
	if (m_hWorkerThread) {
		if (m_fd) {
			pcap_breakloop(m_fd);
		}
		WaitForSingleObject(m_hWorkerThread, INFINITE);
		CloseHandle(m_hWorkerThread);
		m_hWorkerThread = NULL;
	}
}

void CInterfaceProc::WorkerThreadProc()
{
	int retval = pcap_loop(m_fd, -1, &CInterfaceProc::__packetHandler, reinterpret_cast<u_char*>(this));
	if (retval == -1) {
		m_owner->ReportError();
	}
}

unsigned CInterfaceProc::__WorkerThreadProc(void *ctx)
{
	static_cast<CInterfaceProc*>(ctx)->WorkerThreadProc();
	return 0;
}

void CInterfaceProc::__packetHandler(u_char *user, const struct pcap_pkthdr *pkt_header, const u_char *pkt_data)
{
	reinterpret_cast<CInterfaceProc*>(user)->packetHandler(pkt_header, pkt_data);
}

void CInterfaceProc::packetHandler(const struct pcap_pkthdr *pkt_header, const u_char *pkt_data)
{
	if (pkt_header->caplen > 34 && ntohs(*reinterpret_cast<const u_short*>(pkt_data + 12)) == 0x0800) {
		// IPv4
		handle_ipv4(pkt_data + 14, pkt_header->caplen - 14, pkt_header->len);
	}
}

bool isPublicIPv4Address(DWORD ip)
{
	// 0.0.0.0
	if (ip == 0) {
		return false;
	}

	// 255.255.255.255 - broadcast
	if (ip == 0xFFFFFFFF) {
		return false;
	}

	// 127.0.0.0/8 - local
	if (ip >= 0x7F000000 && ip <= 0x7FFFFFFF) {
		return false;
	}
/*
	// RFC1918 - Address Allocation for Private Internets
	// 10.0.0.0        -   10.255.255.255  (10/8 prefix)
	// 172.16.0.0      -   172.31.255.255  (172.16/12 prefix)
	// 192.168.0.0     -   192.168.255.255 (192.168/16 prefix)
	if ((ip & 0xFF000000) == 0x0A000000 || (ip & 0xFFF00000) == 0xAC100000 || (ip & 0xFFFF0000) == 0xC0A80000) {
		return false;
	}

	// RFC5771 - IANA Guidelines for IPv4 Multicast Address Assignments
	// Address Range                 Size       Designation
	// -------------                 ----       -----------
	// 224.0.0.0 - 224.0.0.255 (/24) Local Network Control Block
	// 224.0.1.0 - 224.0.1.255 (/24) Internetwork Control Block
	// 224.0.2.0 - 224.0.255.255 (65024) AD-HOC Block I
	// 224.1.0.0 - 224.1.255.255 (/16) RESERVED
	// 224.2.0.0 - 224.2.255.255 (/16) SDP/SAP Block
	// 224.3.0.0 - 224.4.255.255 (2 /16s) AD-HOC Block II
	// 224.5.0.0 - 224.255.255.255 (251 /16s) RESERVED
	// 225.0.0.0 - 231.255.255.255 (7 /8s) RESERVED
	// 232.0.0.0 - 232.255.255.255 (/8) Source-Specific Multicast Block
	// 233.0.0.0 - 233.251.255.255 (16515072) GLOP Block
	// 233.252.0.0 - 233.255.255.255 (/14) AD-HOC Block III
	// 234.0.0.0 - 238.255.255.255 (5 /8s) RESERVED
	// 239.0.0.0 - 239.255.255.255 (/8) Administratively Scoped Block
	if (ip >= 0xE0000000 && ip <= 0xEFFFFFFF) {
		return false;
	}
*/
	return true;
}

void CInterfaceProc::handle_ipv4(const BYTE *data, UINT caplen, UINT len)
{
	int version = (data[0] & 0xF0) >> 4;
	int ihl = data[0] & 0x0F;

	if (version != 4 || ihl < 5) {
		return;
	}

	WORD totalLength = ntohs(*reinterpret_cast<const u_short*>(data + 2));
	BYTE ttl = data[8];
	BYTE protocol = data[9];
	WORD checksum = ntohs(*reinterpret_cast<const u_short*>(data + 10));
	DWORD src_addr = ntohl(*reinterpret_cast<const u_long*>(data + 12));
	DWORD dst_addr = ntohl(*reinterpret_cast<const u_long*>(data + 16));

	// TCP or UDP
	if (protocol == 6 || protocol == 17) {
		WORD src_port = ntohs(*reinterpret_cast<const u_short*>(data + ihl * 4));
		WORD dst_port = ntohs(*reinterpret_cast<const u_short*>(data + ihl * 4 + 2));

		bool bSrcPublic = isPublicIPv4Address(src_addr);
		bool bDstPublic = isPublicIPv4Address(dst_addr);

		//WCHAR buff[400];
		if (bSrcPublic || bDstPublic) {
			if (protocol == 6) {
				//swprintf_s(buff, L"IPv4[TCP] from %x:%d to %x:%d, total_length=%d, ttl=%d, protocol=%d, checksum=0x%x\n", src_addr, src_port, dst_addr, dst_port, totalLength, ttl, protocol, checksum);
				if (!m_owner->IsSelf(src_addr)) {
					m_owner->ReportTcpIn(dst_addr, dst_port, src_addr, src_port, totalLength);
				} else {
					m_owner->ReportTcpOut(src_addr, src_port, dst_addr, dst_port, totalLength);
				}
			} else {
				//swprintf_s(buff, L"IPv4[UDP] from %x:%d to %x:%d, total_length=%d, ttl=%d, protocol=%d, checksum=0x%x\n", src_addr, src_port, dst_addr, dst_port, totalLength, ttl, protocol, checksum);
				if (!m_owner->IsSelf(src_addr)) {
					m_owner->ReportUdpIn(dst_addr, dst_port, src_addr, src_port, totalLength);
				} else {
					m_owner->ReportUdpOut(src_addr, src_port, dst_addr, dst_port, totalLength);
				}
			}
			//OutputDebugStringW(buff);
		}
	}
}

CTrafficCollector::CTrafficCollector()
{
}

CTrafficCollector::~CTrafficCollector()
{
}

void CTrafficCollector::Incoming(DWORD localAddr, WORD localPort, DWORD remoteAddr, WORD remotePort, UINT totalLength)
{
	CONN_INFO conn;
	conn.localAddr = localAddr;
	conn.localPort = localPort;
	conn.remoteAddr = remoteAddr;
	conn.remotePort = remotePort;

	CAutoLock cObjectLock(this);
	ConnDetailMap::iterator it = m_data.find(conn);
	if (it != m_data.end()) {
		it->second.totalIn += totalLength;
	} else {
		CONN_DETAIL detail;
		detail.totalIn = totalLength;
		detail.totalOut = 0;
		m_data.insert(ConnDetailMap::value_type(conn, detail));
	}
}

void CTrafficCollector::Outgoing(DWORD localAddr, WORD localPort, DWORD remoteAddr, WORD remotePort, UINT totalLength)
{
	CONN_INFO conn;
	conn.localAddr = localAddr;
	conn.localPort = localPort;
	conn.remoteAddr = remoteAddr;
	conn.remotePort = remotePort;

	CAutoLock cObjectLock(this);
	ConnDetailMap::iterator it = m_data.find(conn);
	if (it != m_data.end()) {
		it->second.totalOut += totalLength;
	} else {
		CONN_DETAIL detail;
		detail.totalIn = 0;
		detail.totalOut = totalLength;
		m_data.insert(ConnDetailMap::value_type(conn, detail));
	}
}

void CTrafficCollector::Take(ConnDetailMap& data)
{
	CAutoLock cObjectLock(this);
	m_data.swap(data);
}

CTrafficReport::CTrafficReport()
{
}

CTrafficReport::~CTrafficReport()
{
}

void CTrafficReport::BeginUpdate()
{
	//OutputDebugStringW(L"BeginUpdate\n");

	m_processMap.clear();

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot != INVALID_HANDLE_VALUE) {
		PROCESSENTRY32 pe;
		pe.dwSize = sizeof(pe);
		if (Process32First(hSnapshot, &pe)) {
			do {
				m_processMap.insert(ProcessEntryMap::value_type(pe.th32ProcessID, pe));
			} while (Process32Next(hSnapshot, &pe));
		}
		CloseHandle(hSnapshot);
	}

	AppDetailMap::iterator it = m_appMap.begin();
	AppDetailMap::iterator ie = m_appMap.end();
	for (; it != ie; ++it) {
		it->second.deltaTcpIn = 0;
		it->second.deltaTcpOut = 0;
		it->second.deltaUdpIn = 0;
		it->second.deltaUdpOut = 0;

		if (m_processMap.find(it->first) == m_processMap.end()) {
			//if (it->second.totalTcpIn + it->second.totalTcpOut + it->second.totalUdpIn + it->second.totalUdpOut == 0) {
				it->second.DeleteFlag = true;
			//}
		}

		PrivTcpDetailMap::iterator itTM = it->second.tcpMap.begin();
		PrivTcpDetailMap::iterator ieTM = it->second.tcpMap.end();
		for (; itTM != ieTM; ++itTM) {
			itTM->second.DeleteFlag = true;
		}

		PrivUdpDetailMap::iterator itUM = it->second.udpMap.begin();
		PrivUdpDetailMap::iterator ieUM = it->second.udpMap.end();
		for (; itUM != ieUM; ++itUM) {
			itUM->second.DeleteFlag = true;
		}
	}
}

void CTrafficReport::EndUpdate()
{
	AppDetailMap::iterator it = m_appMap.begin();
	AppDetailMap::iterator ie = m_appMap.end();
	while (it != ie) {
		if (it->second.DeleteFlag) {
			it = m_appMap.erase(it);
		} else {
			PrivTcpDetailMap::iterator itTM = it->second.tcpMap.begin();
			PrivTcpDetailMap::iterator ieTM = it->second.tcpMap.end();
			while (itTM != ieTM) {
				if (itTM->second.DeleteFlag) {
					itTM = it->second.tcpMap.erase(itTM);
				} else {
					itTM++;
				}
			}

			PrivUdpDetailMap::iterator itUM = it->second.udpMap.begin();
			PrivUdpDetailMap::iterator ieUM = it->second.udpMap.end();
			while (itUM != ieUM) {
				if (itUM->second.DeleteFlag) {
					itUM = it->second.udpMap.erase(itUM);
				} else {
					itUM++;
				}
			}

			it++;
		}
	}

	//OutputDebugStringW(L"EndUpdate\n");
}

void CTrafficReport::Serialize(std::vector<BYTE>& outputBuffer, DWORD dt)
{
	UINT bufferSize = sizeof(NM_SUMMARY);
	UINT numTcp = 0;
	UINT numUdp = 0;
	UINT offsetApp = bufferSize;
	UINT offsetTcp = 0;
	UINT offsetUdp = 0;
	UINT offsetStringTable = 0;

	ULONGLONG totalTcpIn = 0;
	ULONGLONG totalTcpOut = 0;
	ULONGLONG deltaTcpIn = 0;
	ULONGLONG deltaTcpOut = 0;
	ULONGLONG totalUdpIn = 0;
	ULONGLONG totalUdpOut = 0;
	ULONGLONG deltaUdpIn = 0;
	ULONGLONG deltaUdpOut = 0;

	{
		UINT stringLength = 0;
		AppDetailMap::iterator it = m_appMap.begin();
		AppDetailMap::iterator ie = m_appMap.end();
		for (; it != ie; ++it) {
			const PRIV_APP_DETAIL& appDetail = it->second;
			numTcp += appDetail.tcpMap.size();
			numUdp += appDetail.udpMap.size();
			totalTcpIn += appDetail.totalTcpIn;
			totalTcpOut += appDetail.totalTcpOut;
			deltaTcpIn += appDetail.deltaTcpIn;
			deltaTcpOut += appDetail.deltaTcpOut;
			totalUdpIn += appDetail.totalUdpIn;
			totalUdpOut += appDetail.totalUdpOut;
			deltaUdpIn += appDetail.deltaUdpIn;
			deltaUdpOut += appDetail.deltaUdpOut;
			stringLength += appDetail.moduleName.length();
			stringLength += appDetail.moduleFullPath.length();
		}

		bufferSize += m_appMap.size() * sizeof(NM_APP_DETAIL);
		offsetTcp = bufferSize;
		bufferSize += numTcp * sizeof(NM_CONNECTION_TCP);
		offsetUdp = bufferSize;
		bufferSize += numUdp * sizeof(NM_CONNECTION_UDP);
		offsetStringTable = bufferSize;
		bufferSize += stringLength;
	}

	// safe guard
	bufferSize += 16;

	std::vector<BYTE> buffer(bufferSize);
	NM_SUMMARY *pSummary = reinterpret_cast<NM_SUMMARY*>(&buffer[0]);
	NM_APP_DETAIL *pAppItem = reinterpret_cast<NM_APP_DETAIL*>(&buffer[offsetApp]);
	NM_CONNECTION_TCP *pTcpItem = reinterpret_cast<NM_CONNECTION_TCP*>(&buffer[offsetTcp]);
	NM_CONNECTION_UDP *pUdpItem = reinterpret_cast<NM_CONNECTION_UDP*>(&buffer[offsetUdp]);
	char *pStringTable = reinterpret_cast<char*>(&buffer[offsetStringTable]);

	pSummary->version = 0x10001;
	pSummary->deltaTime = dt;
	pSummary->appDetailCount = m_appMap.size();
	pSummary->appDetailOffset = offsetApp;
	pSummary->totalTcpIn = totalTcpIn;
	pSummary->totalTcpOut = totalTcpOut;
	pSummary->deltaTcpIn = deltaTcpIn;
	pSummary->deltaTcpOut = deltaTcpOut;
	pSummary->totalUdpIn = totalUdpIn;
	pSummary->totalUdpOut = totalUdpOut;
	pSummary->deltaUdpIn = deltaUdpIn;
	pSummary->deltaUdpOut = deltaUdpOut;

	//WCHAR line[600];
	AppDetailMap::iterator it = m_appMap.begin();
	AppDetailMap::iterator ie = m_appMap.end();
	for (; it != ie; ++it, ++pAppItem) {
		const PRIV_APP_DETAIL& appDetail = it->second;

		pAppItem->pid = it->first;
		pAppItem->totalIn = appDetail.totalTcpIn + appDetail.totalUdpIn;
		pAppItem->totalOut = appDetail.totalTcpOut + appDetail.totalUdpOut;
		pAppItem->deltaIn = appDetail.deltaTcpIn + appDetail.deltaUdpIn;
		pAppItem->deltaOut = appDetail.deltaTcpOut + appDetail.deltaUdpOut;
		pAppItem->tcpCount = appDetail.tcpMap.size();
		pAppItem->tcpOffset = reinterpret_cast<BYTE*>(pTcpItem) - reinterpret_cast<BYTE*>(pSummary);
		pAppItem->udpCount = appDetail.udpMap.size();
		pAppItem->udpOffset = reinterpret_cast<BYTE*>(pUdpItem) - reinterpret_cast<BYTE*>(pSummary);

		pAppItem->moduleFullPathOffset = reinterpret_cast<BYTE*>(pStringTable) - reinterpret_cast<BYTE*>(pSummary);
		pAppItem->moduleFullPathLength = appDetail.moduleFullPath.length();
		if (pAppItem->moduleFullPathLength > 0) {
			memcpy(pStringTable, appDetail.moduleFullPath.c_str(), pAppItem->moduleFullPathLength);
			pStringTable += pAppItem->moduleFullPathLength;
		}

		pAppItem->moduleNameOffset = reinterpret_cast<BYTE*>(pStringTable) - reinterpret_cast<BYTE*>(pSummary);
		pAppItem->moduleNameLength = appDetail.moduleName.length();
		if (pAppItem->moduleNameLength > 0) {
			memcpy(pStringTable, appDetail.moduleName.c_str(), pAppItem->moduleNameLength);
			pStringTable += pAppItem->moduleNameLength;
		}

		//swprintf_s(line, L"APP [%s] [%d] %I64d %I64d %I64d %I64d %I64d %I64d %I64d %I64d\n", it->second.moduleName.c_str(), it->first, appDetail.totalTcpIn, appDetail.totalTcpOut, appDetail.deltaTcpIn, appDetail.deltaTcpOut, appDetail.totalUdpIn, appDetail.totalUdpOut, appDetail.deltaUdpIn, appDetail.deltaUdpOut);
		//OutputDebugStringW(line);

		{
			PrivTcpDetailMap::const_iterator itTM = appDetail.tcpMap.begin();
			PrivTcpDetailMap::const_iterator ieTM = appDetail.tcpMap.end();
			for (; itTM != ieTM; ++itTM, ++pTcpItem) {
				const CONN_INFO& conn = itTM->first;
				const PRIV_TCP_DETAIL& detail = itTM->second;

				pTcpItem->localAddr = conn.localAddr;
				pTcpItem->localPort = conn.localPort;
				pTcpItem->remoteAddr = conn.remoteAddr;
				pTcpItem->remotePort = conn.remotePort;
				pTcpItem->totalIn = detail.totalIn;
				pTcpItem->totalOut = detail.totalOut;
				pTcpItem->deltaIn = detail.deltaIn;
				pTcpItem->deltaOut = detail.deltaOut;

				//swprintf_s(line, L"\t[TCP] %x:%d %x:%d %I64d %I64d %I64d %I64d\n", conn.localAddr, conn.localPort, conn.remoteAddr, conn.remotePort, detail.totalIn, detail.totalOut, detail.deltaIn, detail.deltaOut);
				//OutputDebugStringW(line);
			}
		}

		{
			PrivUdpDetailMap::const_iterator itUM = appDetail.udpMap.begin();
			PrivUdpDetailMap::const_iterator ieUM = appDetail.udpMap.end();
			for (; itUM != ieUM; ++itUM, ++pUdpItem) {
				const CONN_INFO& conn = itUM->first;
				const PRIV_UDP_DETAIL& detail = itUM->second;

				pUdpItem->localAddr = conn.localAddr;
				pUdpItem->localPort = conn.localPort;
				pUdpItem->remoteAddr = conn.remoteAddr;
				pUdpItem->remotePort = conn.remotePort;
				pUdpItem->totalIn = detail.totalIn;
				pUdpItem->totalOut = detail.totalOut;
				pUdpItem->deltaIn = detail.deltaIn;
				pUdpItem->deltaOut = detail.deltaOut;

				//swprintf_s(line, L"\t[UDP] %x:%d %x:%d %I64d %I64d %I64d %I64d\n", conn.localAddr, conn.localPort, conn.remoteAddr, conn.remotePort, detail.totalIn, detail.totalOut, detail.deltaIn, detail.deltaOut);
				//OutputDebugStringW(line);
			}
		}
	}

	outputBuffer.swap(buffer);
}

void CTrafficReport::UpdateUdp(const CONN_INFO& conn, const CONN_DETAIL& detail, DWORD pid)
{
	//WCHAR buff[400];
	//swprintf_s(buff, L"UpdateUdp [%d] %x:%d %x:%d %I64d %I64d\n", pid, conn.localAddr, conn.localPort, conn.remoteAddr, conn.remotePort, detail.totalIn, detail.totalOut);
	//OutputDebugStringW(buff);

	PRIV_APP_DETAIL& appDetail = EnsureAppDetail(pid);
	UpdateUdpDetail(appDetail, conn, detail);
}

void CTrafficReport::UpdateUdpAny(const CONN_INFO& conn, const CONN_DETAIL& detail, DWORD pid)
{
	//WCHAR buff[400];
	//swprintf_s(buff, L"UpdateUdpAny [%d] %x:%d %x:%d %I64d %I64d\n", pid, conn.localAddr, conn.localPort, conn.remoteAddr, conn.remotePort, detail.totalIn, detail.totalOut);
	//OutputDebugStringW(buff);

	PRIV_APP_DETAIL& appDetail = EnsureAppDetail(pid);
	UpdateUdpDetail(appDetail, conn, detail);
}

void CTrafficReport::UpdateUdpMiss(const CONN_INFO& conn, const CONN_DETAIL& detail)
{
	//WCHAR buff[400];
	//swprintf_s(buff, L"UpdateUdpMiss [XXX] %x:%d %x:%d %I64d %I64d\n", conn.localAddr, conn.localPort, conn.remoteAddr, conn.remotePort, detail.totalIn, detail.totalOut);
	//OutputDebugStringW(buff);

	PRIV_APP_DETAIL& appDetail = EnsureAppDetail(0);
	UpdateUdpDetail(appDetail, conn, detail);
}

void CTrafficReport::UpdateTcp(const CONN_INFO& conn, const CONN_DETAIL& detail, DWORD pid, DWORD state)
{
	//WCHAR buff[400];
	//swprintf_s(buff, L"UpdateTcp [%d] %x:%d %x:%d %I64d %I64d [%d]\n", pid, conn.localAddr, conn.localPort, conn.remoteAddr, conn.remotePort, detail.totalIn, detail.totalOut, state);
	//OutputDebugStringW(buff);

	PRIV_APP_DETAIL& appDetail = EnsureAppDetail(pid);
	UpdateTcpDetail(appDetail, conn, detail);
}

void CTrafficReport::UpdateTcpMiss(const CONN_INFO& conn, const CONN_DETAIL& detail)
{
	//WCHAR buff[400];
	//swprintf_s(buff, L"UpdateTcpMiss [XXX] %x:%d %x:%d %I64d %I64d\n", conn.localAddr, conn.localPort, conn.remoteAddr, conn.remotePort, detail.totalIn, detail.totalOut);
	//OutputDebugStringW(buff);

	PRIV_APP_DETAIL& appDetail = EnsureAppDetail(0);
	UpdateTcpDetail(appDetail, conn, detail);
}

PRIV_APP_DETAIL& CTrafficReport::EnsureAppDetail(DWORD pid)
{
	AppDetailMap::iterator it = m_appMap.find(pid);
	if (it != m_appMap.end()) {
		it->second.DeleteFlag = false;
	} else {
		PRIV_APP_DETAIL appDetail;
		appDetail.deltaTcpIn = 0;
		appDetail.deltaTcpOut = 0;
		appDetail.totalTcpIn = 0;
		appDetail.totalTcpOut = 0;
		appDetail.deltaUdpIn = 0;
		appDetail.deltaUdpOut = 0;
		appDetail.totalUdpIn = 0;
		appDetail.totalUdpOut = 0;
		appDetail.DeleteFlag = false;

		it = m_appMap.insert(AppDetailMap::value_type(pid, appDetail)).first;

		ProcessEntryMap::iterator itPM = m_processMap.find(pid);
		if (itPM != m_processMap.end()) {
			it->second.moduleName = convertToUtf8(itPM->second.szExeFile, -1);
		}

		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, pid);
		if (hProcess) {
			WCHAR szBuffer[MAX_PATH * 2 + 1];
			DWORD len = GetModuleFileNameEx(hProcess, NULL, szBuffer, _countof(szBuffer));
			if (len > 0) {
				it->second.moduleFullPath = convertToUtf8(szBuffer, len);
			}
			CloseHandle(hProcess);
		}
	}
	return it->second;
}

void CTrafficReport::UpdateUdpDetail(PRIV_APP_DETAIL& appDetail, const CONN_INFO& conn, const CONN_DETAIL& detail)
{
	PrivUdpDetailMap::iterator it = appDetail.udpMap.find(conn);
	if (it != appDetail.udpMap.end()) {
		PRIV_UDP_DETAIL& udpDetail = it->second;
		udpDetail.deltaIn = detail.totalIn;
		udpDetail.deltaOut = detail.totalOut;
		udpDetail.totalIn += detail.totalIn;
		udpDetail.totalOut += detail.totalOut;
		it->second.DeleteFlag = false;
	} else {
		PRIV_UDP_DETAIL udpDetail;
		udpDetail.totalIn = udpDetail.deltaIn = detail.totalIn;
		udpDetail.totalOut = udpDetail.deltaOut = detail.totalOut;
		udpDetail.DeleteFlag = false;
		it = appDetail.udpMap.insert(PrivUdpDetailMap::value_type(conn, udpDetail)).first;
	}

	appDetail.deltaUdpIn += detail.totalIn;
	appDetail.deltaUdpOut += detail.totalOut;
	appDetail.totalUdpIn += detail.totalIn;
	appDetail.totalUdpOut += detail.totalOut;
}

void CTrafficReport::UpdateTcpDetail(PRIV_APP_DETAIL& appDetail, const CONN_INFO& conn, const CONN_DETAIL& detail)
{
	PrivTcpDetailMap::iterator it = appDetail.tcpMap.find(conn);
	if (it != appDetail.tcpMap.end()) {
		PRIV_TCP_DETAIL& tcpDetail = it->second;
		tcpDetail.deltaIn = detail.totalIn;
		tcpDetail.deltaOut = detail.totalOut;
		tcpDetail.totalIn += detail.totalIn;
		tcpDetail.totalOut += detail.totalOut;
		it->second.DeleteFlag = false;
	} else {
		PRIV_TCP_DETAIL tcpDetail;
		tcpDetail.totalIn = tcpDetail.deltaIn = detail.totalIn;
		tcpDetail.totalOut = tcpDetail.deltaOut = detail.totalOut;
		tcpDetail.DeleteFlag = false;
		it = appDetail.tcpMap.insert(PrivTcpDetailMap::value_type(conn, tcpDetail)).first;
	}

	appDetail.deltaTcpIn += detail.totalIn;
	appDetail.deltaTcpOut += detail.totalOut;
	appDetail.totalTcpIn += detail.totalIn;
	appDetail.totalTcpOut += detail.totalOut;
}
