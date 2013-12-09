#include "ipc2_win32.h"
#include <string>

namespace GTF {

std::wstring buildPipeName(const char *name)
{
	std::wstring pipeName(L"\\\\.\\pipe\\");
	size_t nameLen = strlen(name);
	for (size_t i = 0; i < nameLen; i++) {
		pipeName.push_back(name[i]);
	}
	return pipeName;
}

int readImpl(void *buffer, size_t bytesToRead, size_t *bytesActual, int timeout_ms, HANDLE hFile, HANDLE hEvent)
{
	*bytesActual = 0;

	DWORD dwTimeout = timeout_ms < 0 ? INFINITE : timeout_ms;
	DWORD cbActual = 0;
	OVERLAPPED ol;
	memset(&ol, 0, sizeof(ol));
	ol.hEvent = hEvent;

	if (ReadFile(hFile, buffer, bytesToRead, &cbActual, &ol)) {
		*bytesActual = cbActual;
		return IOStatus_Ok;
	}

	DWORD err = GetLastError();
	if (err != ERROR_IO_PENDING) {
		return IOStatus_UnknownError;
	}

	DWORD dwWait = WaitForSingleObject(ol.hEvent, dwTimeout);
	if (dwWait == WAIT_TIMEOUT) {
		return IOStatus_Timeout;
	}

	if (dwWait != WAIT_OBJECT_0) {
		return IOStatus_UnknownError;
	}

	if (!GetOverlappedResult(hFile, &ol, &cbActual, TRUE)) {
		return IOStatus_UnknownError;
	}

	*bytesActual = cbActual;
	return IOStatus_Ok;
}

int writeImpl(const void *buffer, size_t bytesToWrite, size_t *bytesActual, int timeout_ms, HANDLE hFile, HANDLE hEvent)
{
	*bytesActual = 0;

	DWORD dwTimeout = timeout_ms < 0 ? INFINITE : timeout_ms;
	DWORD cbActual = 0;
	OVERLAPPED ol;
	memset(&ol, 0, sizeof(ol));
	ol.hEvent = hEvent;

	if (WriteFile(hFile, buffer, bytesToWrite, &cbActual, &ol)) {
		*bytesActual = cbActual;
		return IOStatus_Ok;
	}

	DWORD err = GetLastError();
	if (err != ERROR_IO_PENDING) {
		return IOStatus_UnknownError;
	}

	DWORD dwWait = WaitForSingleObject(ol.hEvent, dwTimeout);
	if (dwWait == WAIT_TIMEOUT) {
		return IOStatus_Timeout;
	}

	if (dwWait != WAIT_OBJECT_0) {
		return IOStatus_UnknownError;
	}

	if (!GetOverlappedResult(hFile, &ol, &cbActual, TRUE)) {
		return IOStatus_UnknownError;
	}

	*bytesActual = cbActual;
	return IOStatus_Ok;
}

LocalSocket::LocalSocket()
{
	m_impl = new LocalSocketImpl();
}

LocalSocket::~LocalSocket()
{
	delete m_impl;
}

bool LocalSocket::connect(const char *serverName)
{
	return m_impl->connect(serverName);
}

int LocalSocket::read(void *buffer, size_t bytesToRead, size_t *bytesActual, int timeout_ms)
{
	return m_impl->read(buffer, bytesToRead, bytesActual, timeout_ms);
}

int LocalSocket::write(const void *buffer, size_t bytesToWrite, size_t *bytesActual, int timeout_ms)
{
	return m_impl->write(buffer, bytesToWrite, bytesActual, timeout_ms);
}

void LocalSocket::close()
{
	m_impl->close();
}

LocalSocketImpl::LocalSocketImpl()
: m_pipe(INVALID_HANDLE_VALUE), m_olEvent(NULL)
{
}

LocalSocketImpl::~LocalSocketImpl()
{
	close();
}

bool LocalSocketImpl::connect(const char *serverName)
{
	if (m_pipe != INVALID_HANDLE_VALUE) {
		return false;
	}

	if (!m_olEvent) {
		m_olEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	}

	std::wstring pipeName = buildPipeName(serverName);

	// TODO: timeout
	DWORD openTimeout = 2000;

	for (;;) {
		m_pipe = CreateFileW(
			pipeName.c_str(),
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_OVERLAPPED,
			NULL);

		if (m_pipe != INVALID_HANDLE_VALUE) {
			break;
		}

		DWORD err = GetLastError();
		if (err != ERROR_PIPE_BUSY) {
			break;
		}

		if (!WaitNamedPipeW(pipeName.c_str(), openTimeout)) {
			break;
		}
	}

	if (m_pipe == INVALID_HANDLE_VALUE) {
		return false;
	}

	return true;
}

int LocalSocketImpl::read(void *buffer, size_t bytesToRead, size_t *bytesActual, int timeout_ms)
{
	return readImpl(buffer, bytesToRead, bytesActual, timeout_ms, m_pipe, m_olEvent);
}

int LocalSocketImpl::write(const void *buffer, size_t bytesToWrite, size_t *bytesActual, int timeout_ms)
{
	return writeImpl(buffer, bytesToWrite, bytesActual, timeout_ms, m_pipe, m_olEvent);
}

void LocalSocketImpl::close()
{
	if (m_pipe != INVALID_HANDLE_VALUE) {
		CloseHandle(m_pipe);
		m_pipe = INVALID_HANDLE_VALUE;
	}

	if (m_olEvent) {
		CloseHandle(m_olEvent);
		m_olEvent = NULL;
	}
}

} // namespace GTF
