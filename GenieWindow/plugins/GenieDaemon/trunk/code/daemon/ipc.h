#ifndef __ipc_h__
#define __ipc_h__

#include <stddef.h>

namespace GTF {

enum IOStatus
{
	IOStatus_Ok,
	IOStatus_Timeout,
	IOStatus_UnknownError,
};

class IODeviceIntf
{
public:
	virtual int read(void *buffer, size_t bytesToRead, size_t *bytesActual, int timeout_ms) = 0;
	virtual int write(const void *buffer, size_t bytesToWrite, size_t *bytesActual, int timeout_ms) = 0;

protected:
	IODeviceIntf() {}
	virtual ~IODeviceIntf() {}

private:
	IODeviceIntf(const IODeviceIntf&);
	IODeviceIntf& operator=(const IODeviceIntf&);
};

class SocketIntf
	: public IODeviceIntf
{
public:
	virtual void close() = 0;

protected:
	SocketIntf() {}
	virtual ~SocketIntf() {}

private:
	SocketIntf(const SocketIntf&);
	SocketIntf& operator=(const SocketIntf&);
};

class LocalServerImpl;

class LocalServer
{
	friend class LocalServerImpl;

public:
	LocalServer(const char *serverName);
	virtual ~LocalServer();

	bool start();
	void stop();

protected:
	virtual void serve(SocketIntf *sock) = 0;

private:
	LocalServerImpl *m_impl;
};

class LocalSocketImpl;

class LocalSocket
	: public IODeviceIntf
{
	friend class LocalSocketImpl;

public:
	LocalSocket();
	virtual ~LocalSocket();

	bool connect(const char *serverName);
	virtual int read(void *buffer, size_t bytesToRead, size_t *bytesActual, int timeout_ms);
	virtual int write(const void *buffer, size_t bytesToWrite, size_t *bytesActual, int timeout_ms);
	void close();

private:
	LocalSocketImpl *m_impl;
};

} // namespace GTF

#endif // __ipc_h__
