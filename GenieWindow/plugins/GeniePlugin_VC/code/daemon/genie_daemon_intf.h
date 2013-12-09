#ifndef __genie_daemon_intf_h__
#define __genie_daemon_intf_h__

#include <inttypes.h>
#include <list>
#include <vector>

namespace GDF {

enum Status
{
	Status_Ok,
	Status_UnknownError,
	Status_Timeout,
	Status_ServiceUnavailable,
};

struct ArpEntry
{
	uint32_t ip;
	uint8_t mac[6];
};

struct AppIconInfo
{
	uint32_t width;
	uint32_t height;
	uint32_t pitch;
};

int queryArpTable(std::list<ArpEntry>& table, int timeout = -1);
int queryNetTrafficData(std::vector<uint8_t>& data, int timeout = -1);
int queryAppIconData(uint32_t pid, AppIconInfo& info, std::vector<uint8_t>& data, int timeout = -1);
int killProcess(uint32_t pid, int timeout = -1);

} // namespace GDF

#endif // __genie_daemon_intf_h__
