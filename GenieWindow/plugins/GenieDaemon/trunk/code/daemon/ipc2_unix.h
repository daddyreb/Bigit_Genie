#ifndef __ipc2_unix_h__
#define __ipc2_unix_h__

#include "ipc.h"

namespace GTF {

class LocalSocketImpl
{
public:
	LocalSocketImpl();
	~LocalSocketImpl();

	bool connect(const char *serverName);
	int read(void *buffer, size_t bytesToRead, size_t *bytesActual, int timeout_ms);
	int write(const void *buffer, size_t bytesToWrite, size_t *bytesActual, int timeout_ms);
	void close();

private:
	int m_fd;
};

class File
	: public SocketIntf
{
public:
	File(int fd);
	virtual ~File();

	virtual int read(void *buffer, size_t bytesToRead, size_t *bytesActual, int timeout_ms);
	virtual int write(const void *buffer, size_t bytesToWrite, size_t *bytesActual, int timeout_ms);
	virtual void close();

private:
	int m_fd;
};

} // namespace GTF

#endif // __ipc2_unix_h__
