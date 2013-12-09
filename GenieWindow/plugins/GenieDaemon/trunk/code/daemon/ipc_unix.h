#ifndef __ipc_unix_h__
#define __ipc_unix_h__

#include "ipc.h"
#include "thread.h"
#include <string>

namespace GTF {

class LocalServerImpl
{
public:
	LocalServerImpl(LocalServer *intf, const char *serverName);
	~LocalServerImpl();

	bool start();
	void stop();

	static void __serverThreadProc(void *ctx);
	void serverThreadProc();
	void serve(int fd);

	LocalServer *m_intf;
	Thread *m_serverThread;
	int m_svrSock;
	int m_cmdSock;
	std::string m_serverName;
	std::string m_cmdSocketName;
};

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

#endif // __ipc_unix_h__
