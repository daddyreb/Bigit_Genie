#include "daemon.h"
#include "genie_daemon_intf.h"
#include "logger.h"
#include "cap.h"
#include "proto.h"
#include <stddef.h>
#include <iostream>
#include <sstream>
#include <string>

extern const char *g_genie_server_name;

void display_arp(const std::list<GDF::ArpEntry>& ls)
{
	std::list<GDF::ArpEntry>::const_iterator it = ls.begin();
	std::list<GDF::ArpEntry>::const_iterator ie = ls.end();
	for (; it != ie; ++it) {
		const GDF::ArpEntry& ae = *it;
		printf("%x [%02x:%02x:%02x:%02x:%02x:%02x]\n", ae.ip, ae.mac[0], ae.mac[1], ae.mac[2], ae.mac[3], ae.mac[4], ae.mac[5]);
	}
}

void display_netstat(const std::vector<uint8_t>& data)
{
	printf("net table\n");
	const void *data0 = &data[0];
	const NM_SUMMARY *summary = static_cast<const NM_SUMMARY*>(data0);
	printf("%x\n", summary->version);
}

TestCon2::TestCon2()
{
	m_daemon = GenieDaemon::create();
}

TestCon2::~TestCon2()
{
	delete m_daemon;
}

int TestCon2::run()
{
	if (!m_daemon->start()) {
		return -1;
	}

	for (;;) {
		std::string cmd;
		std::cin >> cmd;

		if (!std::cin) {
			break;
		}

		if (cmd == "quit") {
			break;
		} else if (cmd == "restart") {
		} else if (cmd == "stop") {
		} else if (cmd == "start") {
		} else if (cmd == "arp") {
			std::list<GDF::ArpEntry> ls;
			int err = GDF::queryArpTable(ls);
			if (err == GDF::Status_Ok) {
				display_arp(ls);
			}
		} else if (cmd == "netstat") {
		}
	}

	m_daemon->stop();
	return 0;
}

int console_main(int argc, char **argv)
{
	TestCon2 con;
	return con.run();
}

class GenieDaemonImpl
	: public GenieDaemon
{
public:
	GenieDaemonImpl();
	virtual ~GenieDaemonImpl();

	virtual bool start();
	virtual void check();
	virtual void stop();

protected:
	void serve(GTF::SocketIntf *sock);

private:
	void serveSync(GTF::SocketIntf *sock, int timeout);
	bool processCommand(uint32_t command, const std::vector<uint8_t>& in, std::vector<uint8_t>& out, uint32_t& retval);

private:
	class ServerImpl
		: public GTF::LocalServer
	{
	public:
		ServerImpl(GenieDaemonImpl *owner, const char *serverName)
		: LocalServer(serverName), m_owner(owner)
		{
		}

	protected:
		virtual void serve(GTF::SocketIntf *sock)
		{
			m_owner->serve(sock);
			//reinterpret_cast<GenieDaemonImpl*>(reinterpret_cast<uint8_t*>(this) - reinterpret_cast<uint8_t*>(&reinterpret_cast<GenieDaemonImpl*>(0)->m_server))->serve(sock);
			//reinterpret_cast<GenieDaemonImpl*>(reinterpret_cast<uint8_t*>(this) - offsetof(GenieDaemonImpl, m_server))->serve(sock);
		}

	private:
		GenieDaemonImpl *m_owner;
	};

	ServerImpl m_server;
	PacketCore m_core;
	bool m_running;
	bool m_pendingError;
};

GenieDaemon *GenieDaemon::create()
{
	return new GenieDaemonImpl();
}

GenieDaemonImpl::GenieDaemonImpl()
: m_server(this, g_genie_server_name), m_running(false)
, m_pendingError(false)
{
}

GenieDaemonImpl::~GenieDaemonImpl()
{
	stop();
}

bool GenieDaemonImpl::start()
{
	if (m_running) {
		return false;
	}
/*
	if (!m_core.start()) {
		return false;
	}
*/
	if (!m_server.start()) {
		//m_core.stop();
		return false;
	}

	m_running = true;

	return true;
}

void GenieDaemonImpl::stop()
{
	if (m_running) {
		m_server.stop();
		m_core.stop();
		m_running = false;
		m_pendingError = false;
	}
}

void GenieDaemonImpl::check()
{
	if (m_running) {
		if (m_core.errorFlag()) {
			LOGGER_DEBUG(("GenieDaemonImpl::check() got errorFlag, restarting core..."));
			m_core.stop();
			if (m_core.start()) {
				LOGGER_DEBUG(("core started"));
			} else {
				LOGGER_DEBUG(("core failed, will retry next time"));
				m_pendingError = true;
			}
		} else if (m_pendingError) {
			LOGGER_DEBUG(("GenieDaemonImpl::check() pending error, restarting core..."));
			if (m_core.start()) {
				LOGGER_DEBUG(("pending start succeeded!"));
				m_pendingError = false;
			} else {
				LOGGER_DEBUG(("pending start failed!"));
			}
		} else if (m_core.changed()) {
			LOGGER_DEBUG(("GenieDaemonImpl::check() adapters changed, restarting core..."));
			m_core.stop();
			if (m_core.start()) {
				LOGGER_DEBUG(("core started"));
			} else {
				LOGGER_DEBUG(("core failed, will retry next time"));
				m_pendingError = true;
			}
		}
	}
/*	} else {
		LOGGER_DEBUG(("GenieDaemonImpl::check() trying to start core..."));
		if (m_core.start()) {
			LOGGER_DEBUG(("core started"));
		} else {
			LOGGER_DEBUG(("core failed"));
		}
	}*/
}

void GenieDaemonImpl::serve(GTF::SocketIntf *sock)
{
	serveSync(sock, 1000);
}

void GenieDaemonImpl::serveSync(GTF::SocketIntf *sock, int timeout)
{
	SocketIO io(sock, timeout);
	int err;

	GenieDaemonRequestHeader header;
	err = io.readExact(&header, sizeof(header));
	if (err != GTF::IOStatus_Ok) {
		return;
	}

	if (header.magic != GENIE_DAEMON_MAGIC || header.rsv0 != 0 || header.payloadSize > 0x00010000) {
		return;
	}

	std::vector<uint8_t> payload(header.payloadSize);
	if (header.payloadSize > 0) {
		err = io.readExact(&payload[0], header.payloadSize);
		if (err != GTF::IOStatus_Ok) {
			return;
		}
	}

	std::vector<uint8_t> outputPacket(sizeof(GenieDaemonResponseHeader));
	GenieDaemonResponseHeader *responseHeader = reinterpret_cast<GenieDaemonResponseHeader*>(&outputPacket[0]);

	std::vector<uint8_t> resultData;
	responseHeader->magic = GENIE_DAEMON_MAGIC;
	responseHeader->command = header.command;
	responseHeader->retval = 0;
	if (!processCommand(header.command, payload, resultData, responseHeader->retval)) {
		return;
	}

	responseHeader->payloadSize = resultData.size();

	outputPacket.insert(outputPacket.end(), resultData.begin(), resultData.end());
	err = io.writeExact(&outputPacket[0], outputPacket.size());
	if (err != GTF::IOStatus_Ok) {
		return;
	}

	// TODO: check err
	timeout = 0;
}

bool GenieDaemonImpl::processCommand(uint32_t command, const std::vector<uint8_t>& in, std::vector<uint8_t>& out, uint32_t& retval)
{
	if (command == 0x10001) {
		m_core.copyArpBuffer(out);
		LOGGER_DEBUG(("processCommand: ARP table [len=%d]", out.size()));
	} else if (command == 0x10002) {
		m_core.copyNetBuffer(out);
		LOGGER_DEBUG(("processCommand: NET table [len=%d]", out.size()));
	} else if (command == 0x10003) {
		if (in.size() != 4) {
			LOGGER_DEBUG(("processCommand: APP INFO failed, invalid args!"));
			return false;
		}
		AppInfo appInfo;
		if (!m_core.findAppInfo(*reinterpret_cast<const uint32_t*>(&in[0]), appInfo)) {
			LOGGER_DEBUG(("processCommand: APP INFO failed for pid %d", *reinterpret_cast<const uint32_t*>(&in[0])));
			return false;
		}

		out.resize(sizeof(GDF::AppIconInfo) + appInfo.appIconData.size());
		GDF::AppIconInfo *info = reinterpret_cast<GDF::AppIconInfo*>(&out[0]);
		info->width = appInfo.appIconWidth;
		info->height = appInfo.appIconHeight;
		info->pitch = appInfo.appIconPitch;
		if (!appInfo.appIconData.empty()) {
			memcpy(&out[sizeof(GDF::AppIconInfo)], &appInfo.appIconData[0], appInfo.appIconData.size());
		}
		LOGGER_DEBUG(("processCommand: APP INFO [pid=%d,w=%d,h=%d,p=%d]", *reinterpret_cast<const uint32_t*>(&in[0]), appInfo.appIconWidth, appInfo.appIconHeight, appInfo.appIconPitch));
	} else if (command == 0x10004) {
		if (in.size() != 4) {
			LOGGER_DEBUG(("processCommand: APP KILL failed, invalid args!"));
			return false;
		}
		retval = PS_KillProcess(*reinterpret_cast<const uint32_t*>(&in[0])) ? 0 : 2;
		LOGGER_DEBUG(("processCommand: APP KILL [pid=%d,ret=%d]", *reinterpret_cast<const uint32_t*>(&in[0]), retval));
	} else {
		return false;
	}
	return true;
}
