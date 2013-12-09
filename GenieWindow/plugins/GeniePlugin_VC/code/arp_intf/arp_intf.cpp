#include "arp_intf.h"
#include <genie_daemon_intf.h>

int main(int argc, char *argv[])
{
	std::list<GDF::ArpEntry> table;
	int err = GDF::queryArpTable(table);
	if (err != GDF::Status_Ok) {
		printf("genie_query_arp_table failed!\n");
		return -1;
	}

	std::list<GDF::ArpEntry>::const_iterator it = table.begin();
	std::list<GDF::ArpEntry>::const_iterator ie = table.end();
	for (; it != ie; ++it) {
		const GDF::ArpEntry& ae = *it;
		printf("%x [%02x:%02x:%02x:%02x:%02x:%02x]\n", ae.ip, ae.mac[0], ae.mac[1], ae.mac[2], ae.mac[3], ae.mac[4], ae.mac[5]);
	}
	return 0;
}

