#ifndef __proto_h__
#define __proto_h__

#include "ipc.h"
#include <inttypes.h>

class SocketIO
{
public:
	SocketIO(GTF::SocketIntf *sock, int timeout = -1);
	SocketIO(GTF::IODeviceIntf *sock, int timeout = -1);
	~SocketIO();

	int timeout() const;
	void setTimeout(int timeout);

	int read(void *buffer, size_t bytesToRead, size_t *bytesActual);
	int write(const void *buffer, size_t bytesToWrite, size_t *bytesActual);

	int readExact(void *buffer, size_t count);
	int writeExact(const void *buffer, size_t count);

	template <class T>
	bool read(T& v)
	{
		size_t cb;
		return GTF::IOStatus_Ok == read(&v, sizeof(v), &cb)  && cb == sizeof(v);
	}

	template <class T>
	bool write(T v)
	{
		size_t cb;
		return GTF::IOStatus_Ok == write(&v, sizeof(v), &cb)  && cb == sizeof(v);
	}

private:
	GTF::SocketIntf *m_sock2;
	GTF::IODeviceIntf *m_sock;
	int m_timeout;
};

#define GENIE_DAEMON_MAGIC 0xc11d793a

struct GenieDaemonRequestHeader
{
	uint32_t magic;
	uint32_t command;
	uint32_t payloadSize;
	uint32_t rsv0;
};

struct GenieDaemonResponseHeader
{
	uint32_t magic;
	uint32_t command;
	uint32_t payloadSize;
	uint32_t retval;
};
/*
class GenieDaemonProtocol
{
public:
	static int queryArpTable(const char *server_name, std::list<ArpEntry>& table, int timeout = -1);
};
*/
#endif // __proto_h__
