#ifndef __ps_h__
#define __ps_h__

#include "platform.h"
#include "util.h"
#include <list>
#include <vector>

struct ArpEntry
{
	uint32_t ip;
	uint8_t mac[6];
};

class NetStatEngineImpl;

class NetStatEngine
{
	friend class NetStatEngineImpl;

public:
	NetStatEngine();
	~NetStatEngine();

	void snapshot(std::list<ArpEntry>& arpTable, TrafficReport *trafficReport, const ConnDetailMap& detailTcp, const ConnDetailMap& detailUdp);

private:
	NetStatEngineImpl *m_impl;
};

#endif // __ps_h__
