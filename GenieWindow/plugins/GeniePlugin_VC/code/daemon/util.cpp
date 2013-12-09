#include "util.h"
#ifdef _WIN32
#else
#include <strings.h>
#define stricmp strcasecmp
#endif

TrafficCollector::TrafficCollector()
{
}

TrafficCollector::~TrafficCollector()
{
}

void TrafficCollector::Incoming(uint32_t localAddr, uint16_t localPort, uint32_t remoteAddr, uint16_t remotePort, uint32_t totalLength)
{
	CONN_INFO conn;
	conn.localAddr = localAddr;
	conn.localPort = localPort;
	conn.remoteAddr = remoteAddr;
	conn.remotePort = remotePort;

	GTF::Locker cObjectLock(this);
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

void TrafficCollector::Outgoing(uint32_t localAddr, uint16_t localPort, uint32_t remoteAddr, uint16_t remotePort, uint32_t totalLength)
{
	CONN_INFO conn;
	conn.localAddr = localAddr;
	conn.localPort = localPort;
	conn.remoteAddr = remoteAddr;
	conn.remotePort = remotePort;

	GTF::Locker cObjectLock(this);
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

void TrafficCollector::Take(ConnDetailMap& data)
{
	GTF::Locker cObjectLock(this);
	m_data.swap(data);
}

TrafficReport::TrafficReport()
{
}

TrafficReport::~TrafficReport()
{
}

void TrafficReport::BeginUpdate(ProcessMap *processMap)
{
	m_processMap = processMap;
	//OutputDebugStringW(L"BeginUpdate\n");

	AppDetailMap::iterator it = m_appMap.begin();
	AppDetailMap::iterator ie = m_appMap.end();
	for (; it != ie; ++it) {
		it->second.deltaTcpIn = 0;
		it->second.deltaTcpOut = 0;
		it->second.deltaUdpIn = 0;
		it->second.deltaUdpOut = 0;

		if (m_processMap->find(it->first) == m_processMap->end()) {
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

void TrafficReport::EndUpdate()
{
	AppDetailMap::iterator it = m_appMap.begin();
	AppDetailMap::iterator ie = m_appMap.end();
	while (it != ie) {
		if (it->second.DeleteFlag) {
			m_appMap.erase(it++);
		} else {
			PrivTcpDetailMap::iterator itTM = it->second.tcpMap.begin();
			PrivTcpDetailMap::iterator ieTM = it->second.tcpMap.end();
			while (itTM != ieTM) {
				if (itTM->second.DeleteFlag) {
					it->second.tcpMap.erase(itTM++);
				} else {
					itTM++;
				}
			}

			PrivUdpDetailMap::iterator itUM = it->second.udpMap.begin();
			PrivUdpDetailMap::iterator ieUM = it->second.udpMap.end();
			while (itUM != ieUM) {
				if (itUM->second.DeleteFlag) {
					it->second.udpMap.erase(itUM++);
				} else {
					itUM++;
				}
			}

			it++;
		}
	}

	//OutputDebugStringW(L"EndUpdate\n");
}

void TrafficReport::Serialize(std::vector<uint8_t>& outputBuffer, double dt)
{
	uint32_t bufferSize = sizeof(NM_SUMMARY);
	uint32_t numTcp = 0;
	uint32_t numUdp = 0;
	uint32_t offsetApp = bufferSize;
	uint32_t offsetTcp = 0;
	uint32_t offsetUdp = 0;
	uint32_t offsetStringTable = 0;

	uint64_t totalTcpIn = 0;
	uint64_t totalTcpOut = 0;
	uint64_t deltaTcpIn = 0;
	uint64_t deltaTcpOut = 0;
	uint64_t totalUdpIn = 0;
	uint64_t totalUdpOut = 0;
	uint64_t deltaUdpIn = 0;
	uint64_t deltaUdpOut = 0;

	{
		uint32_t stringLength = 0;
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
		bufferSize += stringLength; // sizeof(char) == 1
	}

	// safe guard
	bufferSize += 16;

	std::vector<uint8_t> buffer(bufferSize);
	NM_SUMMARY *pSummary = reinterpret_cast<NM_SUMMARY*>(&buffer[0]);
	NM_APP_DETAIL *pAppItem = reinterpret_cast<NM_APP_DETAIL*>(&buffer[offsetApp]);
	NM_CONNECTION_TCP *pTcpItem = reinterpret_cast<NM_CONNECTION_TCP*>(&buffer[offsetTcp]);
	NM_CONNECTION_UDP *pUdpItem = reinterpret_cast<NM_CONNECTION_UDP*>(&buffer[offsetUdp]);
	char *pStringTable = reinterpret_cast<char*>(&buffer[offsetStringTable]);

	pSummary->version = 0x10001;
	pSummary->deltaTime = static_cast<uint32_t>(dt * 1000);
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
		pAppItem->tcpOffset = reinterpret_cast<uint8_t*>(pTcpItem) - reinterpret_cast<uint8_t*>(pSummary);
		pAppItem->udpCount = appDetail.udpMap.size();
		pAppItem->udpOffset = reinterpret_cast<uint8_t*>(pUdpItem) - reinterpret_cast<uint8_t*>(pSummary);

		pAppItem->moduleFullPathOffset = reinterpret_cast<uint8_t*>(pStringTable) - reinterpret_cast<uint8_t*>(pSummary);
		pAppItem->moduleFullPathLength = appDetail.moduleFullPath.length();
		if (pAppItem->moduleFullPathLength > 0) {
			memcpy(pStringTable, appDetail.moduleFullPath.c_str(), pAppItem->moduleFullPathLength);
			pStringTable += pAppItem->moduleFullPathLength;
		}

		pAppItem->moduleNameOffset = reinterpret_cast<uint8_t*>(pStringTable) - reinterpret_cast<uint8_t*>(pSummary);
		pAppItem->moduleNameLength = appDetail.moduleName.length();
		if (pAppItem->moduleNameLength > 0) {
			memcpy(pStringTable, appDetail.moduleName.c_str(), pAppItem->moduleNameLength);
			pStringTable += pAppItem->moduleNameLength;
		}

		//swprintf_s(line, L"APP [%s] [%d] %I64d %I64d %I64d %I64d %I64d %I64d %I64d %I64d\n", it->second.moduleName.c_str(), it->first, appDetail.totalTcpIn, appDetail.totalTcpOut, appDetail.deltaTcpIn, appDetail.deltaTcpOut, appDetail.totalUdpIn, appDetail.totalUdpOut, appDetail.deltaUdpIn, appDetail.deltaUdpOut);
		//OutputDebugStringW(line);
/*#ifdef __MSC_VER__
		printf("APP [%s] [%d] %I64d %I64d %I64d %I64d %I64d %I64d %I64d %I64d\n", it->second.moduleName.c_str(), it->first, appDetail.totalTcpIn, appDetail.totalTcpOut, appDetail.deltaTcpIn, appDetail.deltaTcpOut, appDetail.totalUdpIn, appDetail.totalUdpOut, appDetail.deltaUdpIn, appDetail.deltaUdpOut);
#else
		printf("APP [%s] [%d] %lld %lld %lld %lld %lld %lld %lld %lld\n", it->second.moduleName.c_str(), it->first, appDetail.totalTcpIn, appDetail.totalTcpOut, appDetail.deltaTcpIn, appDetail.deltaTcpOut, appDetail.totalUdpIn, appDetail.totalUdpOut, appDetail.deltaUdpIn, appDetail.deltaUdpOut);
#endif*/

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

void TrafficReport::CopyAppInfoMap(AppInfoMap& appInfoMap)
{
	appInfoMap.clear();
	AppDetailMap::const_iterator it = m_appMap.begin();
	AppDetailMap::const_iterator ie = m_appMap.end();
	for (; it != ie; ++it) {
		AppInfo ai;
		ai.appIconData = it->second.appIconData;
		ai.appIconWidth = it->second.appIconWidth;
		ai.appIconHeight = it->second.appIconHeight;
		ai.appIconPitch = it->second.appIconPitch;
		appInfoMap.insert(AppInfoMap::value_type(it->first, ai));
	}
}

void TrafficReport::UpdateUdp(const CONN_INFO& conn, const CONN_DETAIL& detail, uint32_t pid)
{
	//WCHAR buff[400];
	//swprintf_s(buff, L"UpdateUdp [%d] %x:%d %x:%d %I64d %I64d\n", pid, conn.localAddr, conn.localPort, conn.remoteAddr, conn.remotePort, detail.totalIn, detail.totalOut);
	//OutputDebugStringW(buff);

	PRIV_APP_DETAIL& appDetail = EnsureAppDetail(pid);
	UpdateUdpDetail(appDetail, conn, detail);
}

void TrafficReport::UpdateUdpAny(const CONN_INFO& conn, const CONN_DETAIL& detail, uint32_t pid)
{
	//WCHAR buff[400];
	//swprintf_s(buff, L"UpdateUdpAny [%d] %x:%d %x:%d %I64d %I64d\n", pid, conn.localAddr, conn.localPort, conn.remoteAddr, conn.remotePort, detail.totalIn, detail.totalOut);
	//OutputDebugStringW(buff);

	PRIV_APP_DETAIL& appDetail = EnsureAppDetail(pid);
	UpdateUdpDetail(appDetail, conn, detail);
}

void TrafficReport::UpdateUdpMiss(const CONN_INFO& conn, const CONN_DETAIL& detail)
{
	//WCHAR buff[400];
	//swprintf_s(buff, L"UpdateUdpMiss [XXX] %x:%d %x:%d %I64d %I64d\n", conn.localAddr, conn.localPort, conn.remoteAddr, conn.remotePort, detail.totalIn, detail.totalOut);
	//OutputDebugStringW(buff);

	// ignore miss
	//PRIV_APP_DETAIL& appDetail = EnsureAppDetail(0);
	//UpdateUdpDetail(appDetail, conn, detail);
}

void TrafficReport::UpdateTcp(const CONN_INFO& conn, const CONN_DETAIL& detail, uint32_t pid, uint32_t state)
{
	//WCHAR buff[400];
	//swprintf_s(buff, L"UpdateTcp [%d] %x:%d %x:%d %I64d %I64d [%d]\n", pid, conn.localAddr, conn.localPort, conn.remoteAddr, conn.remotePort, detail.totalIn, detail.totalOut, state);
	//OutputDebugStringW(buff);

	PRIV_APP_DETAIL& appDetail = EnsureAppDetail(pid);
	UpdateTcpDetail(appDetail, conn, detail);
}

void TrafficReport::UpdateTcpMiss(const CONN_INFO& conn, const CONN_DETAIL& detail)
{
	//WCHAR buff[400];
	//swprintf_s(buff, L"UpdateTcpMiss [XXX] %x:%d %x:%d %I64d %I64d\n", conn.localAddr, conn.localPort, conn.remoteAddr, conn.remotePort, detail.totalIn, detail.totalOut);
	//OutputDebugStringW(buff);

	// ignore miss
	//PRIV_APP_DETAIL& appDetail = EnsureAppDetail(0);
	//UpdateTcpDetail(appDetail, conn, detail);
}

PRIV_APP_DETAIL& TrafficReport::EnsureAppDetail(uint32_t pid)
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

		ProcessMap::iterator itPM = m_processMap->find(pid);
		if (itPM != m_processMap->end()) {
			it->second.moduleName = itPM->second.moduleName;
			it->second.moduleFullPath = itPM->second.modulePath;
			if (!PS_ExtractAppIcon(pid, itPM->second.modulePath, it->second.appIconData, it->second.appIconWidth, it->second.appIconHeight, it->second.appIconPitch)) {
				it->second.appIconWidth = 0;
				it->second.appIconHeight = 0;
				it->second.appIconPitch = 0;
				it->second.appIconData.clear();
			}
		}
	}
	return it->second;
}

uint64_t hack(uint64_t v, uint64_t dd, uint64_t ll)
{
	if (v <= ll) {
		return v;
	}

	uint64_t a = v / dd;
	if (a < ll) {
		a = ll;
	}
	return a;
}

void TrafficReport::UpdateUdpDetail(PRIV_APP_DETAIL& appDetail, const CONN_INFO& conn, const CONN_DETAIL& detail2)
{
	CONN_DETAIL detail = detail2;
	if (stricmp(appDetail.moduleName.c_str(), "netgeargenie.exe") == 0) {
		detail.totalIn = hack(detail.totalIn, 100, 2);
		detail.totalOut = hack(detail.totalOut, 100, 2);
	}

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

void TrafficReport::UpdateTcpDetail(PRIV_APP_DETAIL& appDetail, const CONN_INFO& conn, const CONN_DETAIL& detail2)
{
	CONN_DETAIL detail = detail2;
	if (stricmp(appDetail.moduleName.c_str(), "netgeargenie.exe") == 0) {
		detail.totalIn = hack(detail.totalIn, 100, 2);
		detail.totalOut = hack(detail.totalOut, 100, 2);
	}

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
