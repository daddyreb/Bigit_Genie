#include "ipc_win32.h"
#include <aclapi.h>
#include <limits.h>

namespace GTF {

std::wstring buildPipeName(const char *name);
int readImpl(void *buffer, size_t bytesToRead, size_t *bytesActual, int timeout_ms, HANDLE hFile, HANDLE hEvent);
int writeImpl(const void *buffer, size_t bytesToWrite, size_t *bytesActual, int timeout_ms, HANDLE hFile, HANDLE hEvent);

Pipe::Pipe(HANDLE handle)
: m_hFile(handle)
{
	m_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

Pipe::~Pipe()
{
	CloseHandle(m_hFile);
	CloseHandle(m_hEvent);
}

int Pipe::read(void *buffer, size_t bytesToRead, size_t *bytesActual, int timeout_ms)
{
	return readImpl(buffer, bytesToRead, bytesActual, timeout_ms, m_hFile, m_hEvent);
}

int Pipe::write(const void *buffer, size_t bytesToWrite, size_t *bytesActual, int timeout_ms)
{
	return writeImpl(buffer, bytesToWrite, bytesActual, timeout_ms, m_hFile, m_hEvent);
}

void Pipe::close()
{
	delete this;
}

LocalServer::LocalServer(const char *serverName)
{
	m_impl = new LocalServerImpl(this, serverName);
}

LocalServer::~LocalServer()
{
	delete m_impl;
}

bool LocalServer::start()
{
	return m_impl->start();
}

void LocalServer::stop()
{
	m_impl->stop();
}

LocalServerImpl::LocalServerImpl(LocalServer *intf, const char *serverName)
: m_intf(intf), m_serverThread(NULL), m_olEvent(NULL), m_sem(NULL)
{
	m_serverPipeName = buildPipeName(serverName);

/*	SID_IDENTIFIER_AUTHORITY sidAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
	AllocateAndInitializeSid(&sidAuthWorld, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &m_sidEveryone);

	EXPLICIT_ACCESS ea;
	memset(&ea, 0, sizeof(ea));
	ea.grfAccessPermissions = KEY_ALL_ACCESS;
	ea.grfAccessMode = SET_ACCESS;
	ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
	ea.Trustee.ptstrName  = (LPTSTR) m_sidEveryone;

	SetEntriesInAcl(1, &ea, NULL, &m_acl);
*/
	m_sd = static_cast<SECURITY_DESCRIPTOR*>(malloc(SECURITY_DESCRIPTOR_MIN_LENGTH));
	InitializeSecurityDescriptor(m_sd, SECURITY_DESCRIPTOR_REVISION);

	SetSecurityDescriptorDacl(m_sd, TRUE, NULL, FALSE);
}

LocalServerImpl::~LocalServerImpl()
{
	stop();

	free(m_sd);
/*	LocalFree(m_acl);
	FreeSid(m_sidEveryone);*/
}

bool LocalServerImpl::start()
{
	if (m_serverThread) {
		return false;
	}

	m_quitFlag = false;

	m_olEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!m_olEvent) {
		return false;
	}

	m_sem = CreateSemaphore(NULL, 0, LONG_MAX, NULL);
	if (!m_sem) {
		CloseHandle(m_olEvent);
		m_olEvent = NULL;
		return false;
	}

	m_serverThread = Thread::create(&LocalServerImpl::__serverThreadProc, this);
	if (!m_serverThread) {
		CloseHandle(m_sem);
		m_sem = NULL;
		CloseHandle(m_olEvent);
		m_olEvent = NULL;
		return false;
	}

	return true;
}

void LocalServerImpl::stop()
{
	if (m_serverThread) {
		// quit now
		m_quitFlag = true;

		if (m_sem) {
			ReleaseSemaphore(m_sem, 1, NULL);
		}

		m_serverThread->join();
		delete m_serverThread;
		m_serverThread = NULL;

		if (m_sem) {
			CloseHandle(m_sem);
			m_sem = NULL;
		}

		if (m_olEvent) {
			CloseHandle(m_olEvent);
			m_olEvent = NULL;
		}
	}
}

void LocalServerImpl::__serverThreadProc(void *ctx)
{
	static_cast<LocalServerImpl*>(ctx)->serverThreadProc();
}

void LocalServerImpl::serverThreadProc()
{
	HANDLE pipe = NULL;
	HANDLE waitObj[] = { m_sem, m_olEvent };

	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = m_sd;
	sa.bInheritHandle = FALSE;

	pipe = CreateNamedPipeW(m_serverPipeName.c_str(),
		PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		4096,
		4096,
		0,
		&sa);

	for (;;) {
		if (pipe == INVALID_HANDLE_VALUE) {
			// TODO: report error
			break;
		}

		OVERLAPPED ol;
		memset(&ol, 0, sizeof(ol));
		ol.hEvent = m_olEvent;

		BOOL connected = ConnectNamedPipe(pipe, &ol);
		if (!connected) {
			DWORD err = GetLastError();
			if (err == ERROR_IO_PENDING) {
				// wait
			} else if (err == ERROR_PIPE_CONNECTED) {
				connected = TRUE;
			} else {
				// TODO: report error
				break;
			}
		}

		if (!connected) {
			DWORD wait = WaitForMultipleObjects(2, waitObj, FALSE, INFINITE);
			if (wait == WAIT_OBJECT_0 + 1) { // m_olEvent
				DWORD cb;
				connected = GetOverlappedResult(pipe, &ol, &cb, TRUE);
			}
		}

		if (m_quitFlag) {
			if (pipe != INVALID_HANDLE_VALUE) {
				CloseHandle(pipe);
			}
			break;
		}

		if (!connected) {
			// TODO: report error
			if (pipe != INVALID_HANDLE_VALUE) {
				CloseHandle(pipe);
			}
			break;
		}

		HANDLE servePipe = pipe;

		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = m_sd;
		sa.bInheritHandle = FALSE;

		pipe = CreateNamedPipeW(m_serverPipeName.c_str(),
			PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
			PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
			PIPE_UNLIMITED_INSTANCES,
			4096,
			4096,
			0,
			&sa);

		serve(servePipe);
	}
}

void LocalServerImpl::serve(HANDLE pipe)
{
	Pipe *o = new Pipe(pipe);
	m_intf->serve(o);
}

} // namespace GTF
