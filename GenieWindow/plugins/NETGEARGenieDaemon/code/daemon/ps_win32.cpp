#include "ps_win32.h"
#include "logger.h"
#include <shellapi.h>

NetStatEngine::NetStatEngine()
{
	m_impl = new NetStatEngineImpl();
}

NetStatEngine::~NetStatEngine()
{
	delete m_impl;
}

void NetStatEngine::snapshot(std::list<ArpEntry>& arpTable, TrafficReport *trafficReport, const ConnDetailMap& detailTcp, const ConnDetailMap& detailUdp)
{
	m_impl->snapshot(arpTable, trafficReport, detailTcp, detailUdp);
}

NetStatEngineImpl::NetStatEngineImpl()
{
}

NetStatEngineImpl::~NetStatEngineImpl()
{
}
/*
#include <stdlib.h>
#include <string.h>

typedef std::map<PART_CONN_INFO, uint32_t, LessOp_PartConnInfo> UdpProcessMap;
typedef std::map<CONN_INFO, uint32_t, LessOp_ConnInfo> TcpProcessMap;

class lsof_parser
{
public:
	lsof_parser();
	~lsof_parser();

	void parse(char *output, size_t length);

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

lsof_parser::lsof_parser()
{
}

lsof_parser::~lsof_parser()
{
}

void lsof_parser::parse(char *output, size_t length)
{
	m_pid = -1;
	m_udpMap.clear();
	m_tcpMap.clear();
	m_processMap.clear();

	char *line_start = output;
	char *line_end = line_start + length;
	char *p = line_start;
	while (p != line_end) {
		if (*p == 0x0a) {
			*p = 0;
			parseLine(line_start, p);
			line_start = ++p;
		} else {
			p++;
		}
	}

	if (m_pid != -1) {
		collect();
	}

	printf("Process Map:\n");
	ProcessMap::iterator itPM = m_processMap.begin();
	ProcessMap::iterator iePM = m_processMap.end();
	for (; itPM != iePM; ++itPM) {
		printf("[%d] [%s][%s]\n", itPM->first, itPM->second.moduleName.c_str(), itPM->second.modulePath.c_str());
	}

	printf("TCP Map:\n");
	TcpProcessMap::iterator itTM = m_tcpMap.begin();
	TcpProcessMap::iterator ieTM = m_tcpMap.end();
	for (; itTM != ieTM; ++itTM) {
		printf("[%d] [%x:%d] [%x:%d]\n", itTM->second, itTM->first.localAddr, itTM->first.localPort, itTM->first.remoteAddr, itTM->first.remotePort);
	}

	printf("UDP Map:\n");
	UdpProcessMap::iterator itUM = m_udpMap.begin();
	UdpProcessMap::iterator ieUM = m_udpMap.end();
	for (; itUM != ieUM; ++itUM) {
		printf("[%d] [%x:%d]\n", itUM->second, itUM->first.addr, itUM->first.port);
	}
}

void lsof_parser::parseLine(char *line_start, char *line_end)
{
	switch (line_start[0]) {
	case 'p':
		if (m_pid != -1) {
			collect();
		}
		m_pid = atoi(line_start + 1);
		break;
	case 'c':
		m_pinfo.moduleName.assign(line_start + 1, line_end);
		break;
	case 'P':
		if (strcmp(line_start + 1, "TCP") == 0) {
			m_tcp = true;
		} else {
			m_tcp = false;
		}
		break;
	case 'n':
		if (m_tcp) {
			parseTcp(line_start + 1, line_end);
		} else {
			parseUdp(line_start + 1, line_end);
		}
		break;
	}
}

void lsof_parser::parseTcp(char *line_start, char *line_end)
{
	char *sep = strstr(line_start, "->");

	uint32_t localIp;
	uint16_t localPort;
	uint32_t remoteIp;
	uint16_t remotePort;

	if (sep) {
		*sep = 0;
		if (!parseIpv4(line_start, sep, localIp, localPort) || !parseIpv4(sep + 2, line_end, remoteIp, remotePort)) {
			return;
		}
	} else {
		if (!parseIpv4(line_start, line_end, localIp, localPort)) {
			return;
		} else {
			remoteIp = 0;
			remotePort = 0;
		}
	}

	CONN_INFO ci;
	ci.localAddr = localIp;
	ci.localPort = localPort;
	ci.remoteAddr = remoteIp;
	ci.remotePort = remotePort;
	m_tcpMap.insert(TcpProcessMap::value_type(ci, m_pid));
}

void lsof_parser::parseUdp(char *line_start, char *line_end)
{
	uint32_t localIp;
	uint16_t localPort;
	if (parseIpv4(line_start, line_end, localIp, localPort)) {
		PART_CONN_INFO pci;
		pci.addr = localIp;
		pci.port = localPort;
		m_udpMap.insert(UdpProcessMap::value_type(pci, m_pid));
	}
}

bool lsof_parser::parseIpv4(char *line_start, char *line_end, uint32_t& ip, uint16_t& port)
{
	char *sep = strchr(line_start, ':');
	if (!sep) {
		return false;
	}

	char *port_str = sep + 1;
	if (*port_str == '*') {
		return false;
	}

	*sep = 0;

	if (*line_start == '*') {
		ip = 0;
	} else {
		ip = ntohl(inet_addr(line_start));
	}
	port = atoi(port_str);

	return true;
}

void lsof_parser::collect()
{
	m_processMap.insert(ProcessMap::value_type(m_pid, m_pinfo));
}

#include <stdio.h>
*/
void NetStatEngineImpl::snapshot(std::list<ArpEntry>& arpTable, TrafficReport *trafficReport, const ConnDetailMap& detailTcp, const ConnDetailMap& detailUdp)
{
/*	{
		std::vector<char> lsof_output(8192);
		FILE *fp = fopen("c:\\users\\yy\\lsof_bin", "rb");
		int len = fread(&lsof_output[0], 1, lsof_output.size(), fp);
		fclose(fp);
		lsof_output.resize(len);

		lsof_parser parser;
		parser.parse(&lsof_output[0], lsof_output.size());
	}*/

	ProcessMap processMap;
	collectProcessInformation(processMap);

	std::vector<BYTE> data(512 * 1024);

	m_arpTable.clear();
	MIB_IPNETTABLE *pArpTable = reinterpret_cast<MIB_IPNETTABLE*>(&data[0]);
	ULONG ulSize = data.size();
	if (NO_ERROR == GetIpNetTable(pArpTable, &ulSize, TRUE)) {
		for (DWORD i = 0; i < pArpTable->dwNumEntries; i++) {
			const MIB_IPNETROW& arpRow = pArpTable->table[i];
			if (arpRow.dwType == 3 && arpRow.dwPhysAddrLen == 6) {
				ArpEntry ae;
				ae.ip = ntohl(arpRow.dwAddr);
				memcpy(ae.mac, arpRow.bPhysAddr, 6);
				//printf("%x %x:%x:%x:%x:%x:%x\n", ntohl(arpRow.dwAddr), arpRow.bPhysAddr[0], arpRow.bPhysAddr[1], arpRow.bPhysAddr[2], arpRow.bPhysAddr[3], arpRow.bPhysAddr[4], arpRow.bPhysAddr[5]);
				m_arpTable.push_back(ae);
			}
		}
	}

	trafficReport->BeginUpdate(&processMap);

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

	arpTable = m_arpTable;
}

void collectProcessInformation(ProcessMap& processMap)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot != INVALID_HANDLE_VALUE) {
		PROCESSENTRY32W pe;
		pe.dwSize = sizeof(pe);
		if (Process32FirstW(hSnapshot, &pe)) {
			do {
				ProcessInfo info;
				info.moduleName = convertToUtf8(pe.szExeFile, -1);

				HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, pe.th32ProcessID);
				if (hProcess) {
					WCHAR szBuffer[MAX_PATH * 2 + 1];
					DWORD len = GetModuleFileNameExW(hProcess, NULL, szBuffer, _countof(szBuffer));
					if (len > 0) {
						info.modulePath = convertToUtf8(szBuffer, len);
						LOGGER_DEBUG(("walk process %x: [%s]", pe.th32ProcessID, info.modulePath.c_str()));
					} else {
						LOGGER_DEBUG(("walk process %x: GetModuleFileNameExW failed, err = %d", pe.th32ProcessID, GetLastError()));
					}
					CloseHandle(hProcess);
				} else {
					LOGGER_DEBUG(("walk process %x: OpenProcess failed, err = %d", pe.th32ProcessID, GetLastError()));
				}

				processMap.insert(ProcessMap::value_type(pe.th32ProcessID, info));
			} while (Process32NextW(hSnapshot, &pe));
		}
		CloseHandle(hSnapshot);
	}
}

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

std::wstring convertToUtf16(const char *text, int length)
{
	if (length < 0) {
		length = strlen(text);
	}

	std::wstring result;
	result.resize(length * 2);

	int len = MultiByteToWideChar(CP_UTF8, 0, text, length, &result[0], result.length());
	if (len > 0) {
		result.resize(len);
	} else {
		result.clear();
	}

	return result;
}

class CMemoryImage
{
public:
	CMemoryImage();
	~CMemoryImage();

	bool create(int width, int height);
	HDC hdc() const { return m_dc; }
	int width() const { return m_bm.bmWidth; }
	int height() const { return m_bm.bmHeight; }
	int pitch() const { return m_bm.bmWidthBytes; }
	void *bits() { return m_bits; }

private:
	HDC m_dc;
	HBITMAP m_bitmap;
	void *m_bits;
	HGDIOBJ m_oldBitmap;
	BITMAP m_bm;
};

CMemoryImage::CMemoryImage()
: m_dc(NULL), m_bitmap(NULL), m_bits(NULL), m_oldBitmap(NULL)
{
}

CMemoryImage::~CMemoryImage()
{
	if (m_dc) {
		SelectObject(m_dc, m_oldBitmap);
		DeleteObject(m_bitmap);
		DeleteDC(m_dc);
		m_bitmap = NULL;
		m_dc = NULL;
		m_bits = NULL;
		m_oldBitmap = NULL;
	}
}

bool CMemoryImage::create(int width, int height)
{
	m_dc = CreateCompatibleDC(NULL);
	if (!m_dc) {
		return false;
	}

	BITMAPINFOHEADER bm;
	memset(&bm, 0, sizeof(bm));
	bm.biSize = sizeof(bm);
	bm.biWidth = width;
	bm.biHeight = height;
	bm.biPlanes = 1;
	bm.biBitCount = 32;
	bm.biCompression = BI_RGB;

	m_bitmap = CreateDIBSection(NULL, reinterpret_cast<BITMAPINFO*>(&bm), DIB_RGB_COLORS, &m_bits, NULL, 0);
	if (!m_bitmap) {
		DeleteDC(m_dc);
		m_dc = NULL;
		return false;
	}

	if (GetObject(m_bitmap, sizeof(m_bm), &m_bm) != sizeof(m_bm)) {
		DeleteObject(m_bitmap);
		m_bitmap = NULL;
		DeleteDC(m_dc);
		m_dc = NULL;
		return false;
	}

	m_oldBitmap = SelectObject(m_dc, m_bitmap);
	return true;
}

bool convertIcon(HICON icon, std::vector<uint8_t>& data, uint32_t& width, uint32_t& height, uint32_t& pitch)
{
	ICONINFO iconInfo;
	if (!GetIconInfo(icon, &iconInfo)) {
		return false;
	}

	int w = iconInfo.xHotspot * 2;
	int h = iconInfo.yHotspot * 2;
	CMemoryImage content;
	if (!content.create(w, h)) {
		return false;
	}

	if (!DrawIconEx(content.hdc(), 0, 0, icon, w, h, 0, NULL, DI_NORMAL)) {
		return false;
	}

	GdiFlush();

	bool foundAlpha = false;

	BYTE *row = reinterpret_cast<BYTE*>(content.bits());
	for (int y = 0; y < h && !foundAlpha; y++, row += content.pitch()) {
		DWORD *scanLine = reinterpret_cast<DWORD*>(row);
		for (int x = 0; x < w; x++) {
			if ((scanLine[x] & 0xFF000000) != 0) {
				foundAlpha = true;
				break;
			}
		}
	}

	if (!foundAlpha) {
		CMemoryImage mask;
		if (!mask.create(w, h)) {
			return false;
		}

		if (!DrawIconEx(mask.hdc(), 0, 0, icon, w, h, 0, NULL, DI_MASK)) {
			return false;
		}

		GdiFlush();

		BYTE *contentRow = reinterpret_cast<BYTE*>(content.bits());
		const BYTE *maskRow = reinterpret_cast<const BYTE*>(mask.bits());
		for (int y = 0; y < h; y++) {
			DWORD *contentPixel = reinterpret_cast<DWORD*>(contentRow);
			const DWORD *maskPixel = reinterpret_cast<const DWORD*>(maskRow);
			for (int x = 0; x < w; x++, contentPixel++, maskPixel++) {
				if ((*maskPixel & 0xFFFFFFFF) != 0) {
					*contentPixel = 0;
				} else {
					*contentPixel |= 0xFF000000;
				}
			}
			contentRow += content.pitch();
			maskRow += mask.pitch();
		}
	}

	int srcPitch = content.pitch();
	int dstPitch = w * 4;
	data.resize(dstPitch * h);
	const BYTE *srcRow = reinterpret_cast<BYTE*>(content.bits());
	srcRow += (h - 1) * srcPitch;
	BYTE *dstRow = reinterpret_cast<BYTE*>(&data[0]);

	for (int y = 0; y < h; y++) {
		memcpy(dstRow, srcRow, dstPitch);
		dstRow += dstPitch;
		srcRow -= srcPitch;
	}

	width = w;
	height = h;
	pitch = dstPitch;
	return true;
}

bool PS_ExtractAppIcon(uint32_t pid, const std::string& path, std::vector<uint8_t>& data, uint32_t& width, uint32_t& height, uint32_t& pitch)
{
	if (path.empty()) {
		return false;
	}

	std::wstring wPath = convertToUtf16(path.c_str(), path.length());
	HICON hIcon = ExtractIconW(NULL, wPath.c_str(), 0);
	if (!hIcon) {
		hIcon = LoadIcon(NULL, IDI_APPLICATION);
	}

	if (!hIcon) {
		return false;
	}

	bool retval = convertIcon(hIcon, data, width, height, pitch);
	DestroyIcon(hIcon);
	return retval;
}

bool PS_KillProcess(uint32_t pid)
{
	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
	if (!hProcess) {
		return false;
	}

	if (!TerminateProcess(hProcess, 0xbad2dead)) {
		CloseHandle(hProcess);
		return false;
	}

	CloseHandle(hProcess);
	return true;
}
