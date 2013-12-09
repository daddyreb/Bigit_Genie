#ifndef __ps_win32_h__
#define __ps_win32_h__

#include "ps.h"
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <iphlpapi.h>
#include <list>
#include <vector>

struct PART_CONN_INFO
{
	DWORD addr;
	DWORD port;
};

struct LessOp_PartConnInfo
{
	bool operator()(const PART_CONN_INFO& l, const PART_CONN_INFO& r) const
	{
		return memcmp(&l, &r, sizeof(PART_CONN_INFO)) < 0;
	}
};

void collectProcessInformation(ProcessMap& processMap);
std::string convertToUtf8(const WCHAR *text, int length);

class NetStatEngineImpl
{
public:
	NetStatEngineImpl();
	~NetStatEngineImpl();
	void snapshot(std::list<ArpEntry>& arpTable, TrafficReport *trafficReport, const ConnDetailMap& detailTcp, const ConnDetailMap& detailUdp);

private:
	std::list<ArpEntry> m_arpTable;
};

#endif // __ps_win32_h__
