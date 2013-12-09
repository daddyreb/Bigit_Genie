#include "ipc_unix.h"
#include "logger.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

namespace GTF {

std::string buildUnixSocketName(const char *name);
int createUnixSocket(const char *path, bool bc, bool fPublic);
int readImpl(void *buffer, size_t bytesToRead, size_t *bytesActual, int timeout_ms, int fd);
int writeImpl(const void *buffer, size_t bytesToWrite, size_t *bytesActual, int timeout_ms, int fd);

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
: m_intf(intf), m_serverThread(NULL), m_svrSock(-1), m_cmdSock(-1)
{
	m_serverName = buildUnixSocketName(serverName);
	m_cmdSocketName = m_serverName + ".cmd";
}

LocalServerImpl::~LocalServerImpl()
{
	stop();
}

bool LocalServerImpl::start()
{
	if (m_serverThread) {
		return false;
	}

	m_svrSock = createUnixSocket(m_serverName.c_str(), true, true);
	if (m_svrSock == -1) {
		return false;
	}

	if (listen(m_svrSock, 5) == -1) {
		close(m_svrSock);
		m_svrSock = -1;
		return false;
	}

	m_cmdSock = createUnixSocket(m_cmdSocketName.c_str(), true, false);
	if (m_cmdSock == -1) {
		close(m_svrSock);
		m_svrSock = -1;
		return false;
	}

	if (listen(m_cmdSock, 5) == -1) {
		close(m_cmdSock);
		m_cmdSock = -1;
		close(m_svrSock);
		m_svrSock = -1;
		return false;
	}

	m_serverThread = Thread::create(&LocalServerImpl::__serverThreadProc, this);
	if (!m_serverThread) {
		close(m_cmdSock);
		m_cmdSock = -1;
		close(m_svrSock);
		m_svrSock = -1;
		return false;
	}

	return true;
}

void LocalServerImpl::stop()
{
	if (m_serverThread) {
		int cmdSock = createUnixSocket(m_cmdSocketName.c_str(), false, false);
		if (cmdSock != -1) {
			write(cmdSock, "q", 1);
			close(cmdSock);
		}

		m_serverThread->join();
		delete m_serverThread;
		m_serverThread = NULL;

		if (m_cmdSock != -1) {
			close(m_cmdSock);
			m_cmdSock = -1;
		}

		if (m_svrSock != -1) {
			close(m_svrSock);
			m_svrSock = -1;
		}
	}
}

void LocalServerImpl::__serverThreadProc(void *ctx)
{
	static_cast<LocalServerImpl*>(ctx)->serverThreadProc();
}

void LocalServerImpl::serverThreadProc()
{
	int nfds = m_cmdSock;
	if (nfds < m_svrSock) {
		nfds = m_svrSock;
	}

	++nfds;

	for (;;) {
		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(m_cmdSock, &fds);
		FD_SET(m_svrSock, &fds);

		timeval tv;
		tv.tv_sec = 1;
		tv.tv_usec = 0;

		int n = select(nfds, &fds, NULL, NULL, &tv);

		if (n > 0) {
			if (FD_ISSET(m_cmdSock, &fds)) {
				int fd = accept(m_cmdSock, NULL, 0);
				close(fd);
				break;
			}

			if (FD_ISSET(m_svrSock, &fds)) {
				int fd = accept(m_svrSock, NULL, 0);
				serve(fd);
				close(fd);
			}
		} else if (n < 0) {
			// TODO: report error
			LOGGER_DEBUG(("select returned %d, errno = %d", n, errno));
			//break;
		}

	}
}

void LocalServerImpl::serve(int fd)
{
	File *o = new File(fd);
	m_intf->serve(o);
}

} // namespace GTF
