#ifndef __ipc_win32_h__
#define __ipc_win32_h__

#include "ipc.h"
#include "thread.h"
#include <windows.h>
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
	void serve(HANDLE pipe);

	LocalServer *m_intf;
	Thread *m_serverThread;
	std::wstring m_serverPipeName;
	HANDLE m_olEvent;
	HANDLE m_sem;
//	PACL m_acl;
//	PSID m_sidEveryone;
	SECURITY_DESCRIPTOR *m_sd;
	bool m_quitFlag;
};

class Pipe
	: public SocketIntf
{
public:
	Pipe(HANDLE handle);
	virtual ~Pipe();

	virtual int read(void *buffer, size_t bytesToRead, size_t *bytesActual, int timeout_ms);
	virtual int write(const void *buffer, size_t bytesToWrite, size_t *bytesActual, int timeout_ms);
	virtual void close();

private:
	HANDLE m_hFile;
	HANDLE m_hEvent;
};

} // namespace GTF

#endif // __ipc_win32_h__
