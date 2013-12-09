#include "ipc2_unix.h"
#include "logger.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string>

namespace GTF {

std::string buildUnixSocketName(const char *name)
{
	return std::string("/var/tmp/") + name;
}

int createUnixSocket(const char *path, bool bc, bool fPublic)
{
	int sock = socket(PF_UNIX, SOCK_STREAM, 0);
	if (sock != -1) {
		struct sockaddr_un addr;
		memset(&addr, 0, sizeof(addr));
		addr.sun_family = AF_UNIX;

		size_t nameLen = strlen(path);
		if (nameLen > sizeof(addr.sun_path)) {
			nameLen = sizeof(addr.sun_path);
		}
		memcpy(addr.sun_path, path, nameLen);

		if (bc) {
			// TODO: path
			unlink(path);
			if (bind(sock, reinterpret_cast<struct sockaddr*>(&addr), SUN_LEN(&addr)) == -1) {
				LOGGER_DEBUG(("bind() failed, errno = %d", errno));
				close(sock);
				sock = -1;
			}

			// TODO:
			if (fPublic) {
				chmod(path, S_IRWXU|S_IRWXG|S_IRWXO);
			}

		} else {
			if (connect(sock, reinterpret_cast<struct sockaddr*>(&addr), SUN_LEN(&addr)) == -1) {
				LOGGER_DEBUG(("connect() failed, errno = %d", errno));
				close(sock);
				sock = -1;
			}
		}
	} else {
		LOGGER_DEBUG(("socket() failed, errno = %d", errno));
	}
	return sock;
}

int readImpl(void *buffer, size_t bytesToRead, size_t *bytesActual, int timeout_ms, int fd)
{
	*bytesActual = 0;

	timeval timeout_tv;
	timeval *tv;
	if (timeout_ms < 0) {
		tv = NULL;
	} else {
		tv = &timeout_tv;
		div_t d = div(timeout_ms, 1000);
		tv->tv_sec = d.quot;
		tv->tv_usec = d.rem * 1000;
	}

	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(fd, &fds);
	int n = select(fd + 1, &fds, NULL, NULL, tv);
	if (n == 0) {
		return IOStatus_Timeout;
	}

	if (n < 0) {
		return IOStatus_UnknownError;
	}

	if (!FD_ISSET(fd, &fds)) {
		return IOStatus_UnknownError;
	}

	// TODO: SSIZE_MAX ??
	ssize_t cb = read(fd, buffer, bytesToRead);
	if (cb < 0) {
		return IOStatus_UnknownError;
	}

	*bytesActual = cb;
	return IOStatus_Ok;
}

int writeImpl(const void *buffer, size_t bytesToWrite, size_t *bytesActual, int timeout_ms, int fd)
{
	*bytesActual = 0;

	timeval timeout_tv;
	timeval *tv;
	if (timeout_ms < 0) {
		tv = NULL;
	} else {
		tv = &timeout_tv;
		div_t d = div(timeout_ms, 1000);
		tv->tv_sec = d.quot;
		tv->tv_usec = d.rem * 1000;
	}

	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(fd, &fds);
	int n = select(fd + 1, NULL, &fds, NULL, tv);
	if (n == 0) {
		return IOStatus_Timeout;
	}

	if (n < 0) {
		return IOStatus_UnknownError;
	}

	if (!FD_ISSET(fd, &fds)) {
		return IOStatus_UnknownError;
	}

	// TODO: SSIZE_MAX ??
	ssize_t cb = write(fd, buffer, bytesToWrite);
	if (cb < 0) {
		return IOStatus_UnknownError;
	}

	*bytesActual = cb;
	return IOStatus_Ok;
}

File::File(int fd)
: m_fd(fd)
{
}

File::~File()
{
	::close(m_fd);
}

int File::read(void *buffer, size_t bytesToRead, size_t *bytesActual, int timeout_ms)
{
	return readImpl(buffer, bytesToRead, bytesActual, timeout_ms, m_fd);
}

int File::write(const void *buffer, size_t bytesToWrite, size_t *bytesActual, int timeout_ms)
{
	return writeImpl(buffer, bytesToWrite, bytesActual, timeout_ms, m_fd);
}

void File::close()
{
	delete this;
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
: m_fd(-1)
{
}

LocalSocketImpl::~LocalSocketImpl()
{
	close();
}

bool LocalSocketImpl::connect(const char *serverName)
{
	if (m_fd != -1) {
		return false;
	}

	std::string serverPath = buildUnixSocketName(serverName);
	m_fd = createUnixSocket(serverPath.c_str(), false, false);
	if (m_fd == -1) {
		return false;
	}

	return true;
}

int LocalSocketImpl::read(void *buffer, size_t bytesToRead, size_t *bytesActual, int timeout_ms)
{
	return readImpl(buffer, bytesToRead, bytesActual, timeout_ms, m_fd);
}

int LocalSocketImpl::write(const void *buffer, size_t bytesToWrite, size_t *bytesActual, int timeout_ms)
{
	return writeImpl(buffer, bytesToWrite, bytesActual, timeout_ms, m_fd);
}

void LocalSocketImpl::close()
{
	if (m_fd != -1) {
		::close(m_fd);
		m_fd = -1;
	}
}

} // namespace GTF
