#ifndef __ipc2_win32_h__
#define __ipc2_win32_h__

#include "ipc.h"
#include <windows.h>

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
	HANDLE m_pipe;
	HANDLE m_olEvent;
};

} // namespace GTF

#endif // __ipc2_win32_h__
