#include "proto.h"
#include "genie_daemon_intf.h"
#include <vector>
#include <memory.h>
#include <stdio.h>

SocketIO::SocketIO(GTF::SocketIntf *sock, int timeout)
: m_sock(sock), m_sock2(sock), m_timeout(timeout)
{
}

SocketIO::SocketIO(GTF::IODeviceIntf *sock, int timeout)
: m_sock(sock), m_sock2(NULL), m_timeout(timeout)
{
}

SocketIO::~SocketIO()
{
	if (m_sock2) {
		m_sock2->close();
	}
}

int SocketIO::timeout() const
{
	return m_timeout;
}

void SocketIO::setTimeout(int timeout)
{
	m_timeout = timeout;
}

int SocketIO::read(void *buffer, size_t bytesToRead, size_t *bytesActual)
{
	return m_sock->read(buffer, bytesToRead, bytesActual, m_timeout);
}

int SocketIO::write(const void *buffer, size_t bytesToWrite, size_t *bytesActual)
{
	return m_sock->write(buffer, bytesToWrite, bytesActual, m_timeout);
}

int SocketIO::readExact(void *buffer, size_t count)
{
	//printf("readExact %p %d\n", buffer, count);

	uint8_t *pos = static_cast<uint8_t*>(buffer);
	size_t remain = count;
	size_t cb;

	while (remain != 0) {
		//printf("readExact %p %d [%p %d] in\n", buffer, count, pos, remain);
		int err = read(pos, remain, &cb);
		//printf("readExact %p %d [%p %d] out: [%d] err=%d\n", buffer, count, pos, remain, cb, err);
		if (err != GTF::IOStatus_Ok) {
			return err;
		}

		if (cb == 0) {
			return GTF::IOStatus_UnknownError;
		}

		pos += cb;
		remain -= cb;
	}

	return GTF::IOStatus_Ok;
}

int SocketIO::writeExact(const void *buffer, size_t count)
{
	//printf("writeExact %p %d\n", buffer, count);

	const uint8_t *pos = static_cast<const uint8_t*>(buffer);
	size_t remain = count;
	size_t cb;

	while (remain != 0) {
		//printf("writeExact %p %d [%p %d] in\n", buffer, count, pos, remain);
		int err = write(pos, remain, &cb);
		//printf("writeExact %p %d [%p %d] out: [%d] err=%d\n", buffer, count, pos, remain, cb, err);
		if (err != GTF::IOStatus_Ok) {
			return err;
		}

		pos += cb;
		remain -= cb;
	}

	return GTF::IOStatus_Ok;
}

const char *g_genie_server_name = "geniedaemon_fd";

namespace GDF {

int queryArpTable(std::list<ArpEntry>& table, int timeout)
{
	int err;
	GTF::LocalSocket sock;
	if (!sock.connect(g_genie_server_name)) {
		return Status_ServiceUnavailable;
	}

	std::vector<uint8_t> overflowBufferVec(4096);
	uint8_t *overflowBuffer = &overflowBufferVec[0];

	const uint32_t SAFEGUARD = 0xbadface3;

	SocketIO io(&sock, timeout);
	GenieDaemonRequestHeader& requestHeader = *reinterpret_cast<GenieDaemonRequestHeader*>(overflowBuffer);
	*reinterpret_cast<uint32_t*>(overflowBuffer + sizeof(GenieDaemonRequestHeader)) = SAFEGUARD;
	requestHeader.magic = GENIE_DAEMON_MAGIC;
	requestHeader.command = 0x10001;
	requestHeader.payloadSize = 0;
	requestHeader.rsv0 = 0;
	err = io.writeExact(&requestHeader, sizeof(requestHeader));
	if (err == GTF::IOStatus_Timeout) {
		return Status_Timeout;
	} else if (err != GTF::IOStatus_Ok) {
		return Status_UnknownError;
	}

	if (*reinterpret_cast<uint32_t*>(overflowBuffer + sizeof(GenieDaemonRequestHeader)) != SAFEGUARD) {
		//printf("VERY BAD!!!! aaaa\n");
		return Status_UnknownError;
	}

	GenieDaemonResponseHeader& responseHeader = *reinterpret_cast<GenieDaemonResponseHeader*>(overflowBuffer);
	*reinterpret_cast<uint32_t*>(overflowBuffer + sizeof(GenieDaemonResponseHeader)) = SAFEGUARD;
	err = io.readExact(&responseHeader, sizeof(responseHeader));
	if (err == GTF::IOStatus_Timeout) {
		return Status_Timeout;
	} else if (err != GTF::IOStatus_Ok) {
		return Status_UnknownError;
	}

	if (*reinterpret_cast<uint32_t*>(overflowBuffer + sizeof(GenieDaemonResponseHeader)) != SAFEGUARD) {
		//printf("VERY BAD!!!! bbbb\n");
		return Status_UnknownError;
	}

	if (responseHeader.retval != 0 || responseHeader.payloadSize == 0) {
		return Status_UnknownError;
	}

	std::vector<uint8_t> data(responseHeader.payloadSize);
	err = io.readExact(&data[0], responseHeader.payloadSize);
	if (err == GTF::IOStatus_Timeout) {
		return Status_Timeout;
	} else if (err != GTF::IOStatus_Ok) {
		return Status_UnknownError;
	}

	const void *data0 = &data[0];
	uint32_t count = *static_cast<const uint32_t*>(data0);
	const uint8_t *ptr = reinterpret_cast<const uint8_t*>(static_cast<const uint32_t*>(data0) + 1);
	for (uint32_t i = 0; i < count; i++, ptr += 10) {
		GDF::ArpEntry ae;
		ae.ip = *reinterpret_cast<const uint32_t*>(ptr);
		memcpy(ae.mac, ptr + 4, 6);
		table.push_back(ae);
	}

	return Status_Ok;
}

int queryNetTrafficData(std::vector<uint8_t>& data, int timeout)
{
	int err;
	GTF::LocalSocket sock;
	if (!sock.connect(g_genie_server_name)) {
		return Status_ServiceUnavailable;
	}

	std::vector<uint8_t> overflowBufferVec(4096);
	uint8_t *overflowBuffer = &overflowBufferVec[0];

	const uint32_t SAFEGUARD = 0xbadface3;

	SocketIO io(&sock, timeout);
	GenieDaemonRequestHeader& requestHeader = *reinterpret_cast<GenieDaemonRequestHeader*>(overflowBuffer);
	*reinterpret_cast<uint32_t*>(overflowBuffer + sizeof(GenieDaemonRequestHeader)) = SAFEGUARD;
	requestHeader.magic = GENIE_DAEMON_MAGIC;
	requestHeader.command = 0x10002;
	requestHeader.payloadSize = 0;
	requestHeader.rsv0 = 0;
	err = io.writeExact(&requestHeader, sizeof(requestHeader));
	if (err == GTF::IOStatus_Timeout) {
		return Status_Timeout;
	} else if (err != GTF::IOStatus_Ok) {
		return Status_UnknownError;
	}

	if (*reinterpret_cast<uint32_t*>(overflowBuffer + sizeof(GenieDaemonRequestHeader)) != SAFEGUARD) {
		//printf("VERY BAD!!!! 111\n");
		return Status_UnknownError;
	}

	GenieDaemonResponseHeader& responseHeader = *reinterpret_cast<GenieDaemonResponseHeader*>(overflowBuffer);
	*reinterpret_cast<uint32_t*>(overflowBuffer + sizeof(GenieDaemonResponseHeader)) = SAFEGUARD;
	err = io.readExact(&responseHeader, sizeof(responseHeader));
	if (err == GTF::IOStatus_Timeout) {
		return Status_Timeout;
	} else if (err != GTF::IOStatus_Ok) {
		return Status_UnknownError;
	}

	if (*reinterpret_cast<uint32_t*>(overflowBuffer + sizeof(GenieDaemonResponseHeader)) != SAFEGUARD) {
		//printf("VERY BAD!!!! 2\n");
		return Status_UnknownError;
	}

	if (responseHeader.retval != 0 || responseHeader.payloadSize == 0) {
		return Status_UnknownError;
	}

	data.resize(responseHeader.payloadSize);
	err = io.readExact(&data[0], responseHeader.payloadSize);
	if (err == GTF::IOStatus_Timeout) {
		return Status_Timeout;
	} else if (err != GTF::IOStatus_Ok) {
		return Status_UnknownError;
	}

	return Status_Ok;
}

int queryAppIconData(uint32_t pid, AppIconInfo& info, std::vector<uint8_t>& data, int timeout)
{
	int err;
	GTF::LocalSocket sock;
	if (!sock.connect(g_genie_server_name)) {
		return Status_ServiceUnavailable;
	}

	std::vector<uint8_t> overflowBufferVec(4096);
	uint8_t *overflowBuffer = &overflowBufferVec[0];

	const uint32_t SAFEGUARD = 0xbadface3;

	SocketIO io(&sock, timeout);
	std::vector<uint8_t> requestData(sizeof(GenieDaemonRequestHeader) + 4);
	GenieDaemonRequestHeader *requestHeader = reinterpret_cast<GenieDaemonRequestHeader*>(&requestData[0]);
	requestHeader->magic = GENIE_DAEMON_MAGIC;
	requestHeader->command = 0x10003;
	requestHeader->payloadSize = 4;
	requestHeader->rsv0 = 0;
	*reinterpret_cast<uint32_t*>(requestHeader + 1) = pid;

	err = io.writeExact(&requestData[0], requestData.size());
	if (err == GTF::IOStatus_Timeout) {
		return Status_Timeout;
	} else if (err != GTF::IOStatus_Ok) {
		return Status_UnknownError;
	}

	GenieDaemonResponseHeader& responseHeader = *reinterpret_cast<GenieDaemonResponseHeader*>(overflowBuffer);
	*reinterpret_cast<uint32_t*>(overflowBuffer + sizeof(GenieDaemonResponseHeader)) = SAFEGUARD;
	err = io.readExact(&responseHeader, sizeof(responseHeader));
	if (err == GTF::IOStatus_Timeout) {
		return Status_Timeout;
	} else if (err != GTF::IOStatus_Ok) {
		return Status_UnknownError;
	}

	if (*reinterpret_cast<uint32_t*>(overflowBuffer + sizeof(GenieDaemonResponseHeader)) != SAFEGUARD) {
		//printf("VERY BAD!!!! 33334\n");
		return Status_UnknownError;
	}

	if (responseHeader.retval != 0 || responseHeader.payloadSize <= 16) {
		return Status_UnknownError;
	}

	std::vector<uint8_t> payload;
	payload.resize(responseHeader.payloadSize);
	err = io.readExact(&payload[0], responseHeader.payloadSize);
	if (err == GTF::IOStatus_Timeout) {
		return Status_Timeout;
	} else if (err != GTF::IOStatus_Ok) {
		return Status_UnknownError;
	}

	AppIconInfo *iconInfo = reinterpret_cast<AppIconInfo*>(&payload[0]);
	info = *iconInfo;

	uint32_t iconDataLen = *reinterpret_cast<uint32_t*>(iconInfo+1);
	data.assign(payload.begin() + sizeof(AppIconInfo), payload.end());
	return Status_Ok;
}

int killProcess(uint32_t pid, int timeout)
{
	int err;
	GTF::LocalSocket sock;
	if (!sock.connect(g_genie_server_name)) {
		return Status_ServiceUnavailable;
	}

	SocketIO io(&sock, timeout);
	std::vector<uint8_t> requestData(sizeof(GenieDaemonRequestHeader) + 4);
	GenieDaemonRequestHeader *requestHeader = reinterpret_cast<GenieDaemonRequestHeader*>(&requestData[0]);
	requestHeader->magic = GENIE_DAEMON_MAGIC;
	requestHeader->command = 0x10004;
	requestHeader->payloadSize = 4;
	requestHeader->rsv0 = 0;
	*reinterpret_cast<uint32_t*>(requestHeader + 1) = pid;

	err = io.writeExact(&requestData[0], requestData.size());
	if (err == GTF::IOStatus_Timeout) {
		return Status_Timeout;
	} else if (err != GTF::IOStatus_Ok) {
		return Status_UnknownError;
	}

	GenieDaemonResponseHeader responseHeader;
	err = io.readExact(&responseHeader, sizeof(responseHeader));
	if (err == GTF::IOStatus_Timeout) {
		return Status_Timeout;
	} else if (err != GTF::IOStatus_Ok) {
		return Status_UnknownError;
	}

	if (responseHeader.retval != 0 || responseHeader.payloadSize != 0) {
		return Status_UnknownError;
	}

	return Status_Ok;
}

} // namespace GDF
