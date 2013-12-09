#include "msgmng.h"
#include <stdio.h>

MsgMng::MsgMng(SOCKET u, int port)
{
	m_Socket = u;
	m_port = port;
	m_bStop = false;

	char buf[260] = {0};
	unsigned long size = sizeof buf;
	if(!::GetComputerNameA(buf, &size)) {
		int err = WSAGetLastError();
		emit ErrorOccurSignal(QString::fromLocal8Bit("Can't get ComputerName, error code: %1").arg(err));
		return;
	}
	memcpy(host.hostName, buf, sizeof(host.hostName));

	size = 260;
	memset(buf, 0, sizeof buf);
	if(!::GetUserNameA(buf, &size)) {
		int err = WSAGetLastError();
		emit ErrorOccurSignal(QString::fromLocal8Bit("Can't get user name, error code: %1").arg(err));
		return ;
	}
	memcpy(host.userName, buf, sizeof(host.userName));

	sockaddr_in addr;

	m_Socket = socket(AF_INET, SOCK_DGRAM, 0);
	if(INVALID_SOCKET == m_Socket) {
		int err = WSAGetLastError();
		emit ErrorOccurSignal(QString("%1").arg(err));
		return;
	}

	bool flag = true;
	setsockopt(m_Socket, SOL_SOCKET, SO_BROADCAST, (char *)&flag, sizeof(flag));

	int	buf_size = 65536, buf_minsize = 65536 / 2;
	setsockopt(m_Socket, SOL_SOCKET, SO_SNDBUF, (char *)&buf_size, sizeof(int));
	setsockopt(m_Socket, SOL_SOCKET, SO_SNDBUF, (char *)&buf_minsize, sizeof(int));

	buf_size = 65536, buf_minsize = 65536 / 2;
	setsockopt(m_Socket, SOL_SOCKET, SO_RCVBUF, (char *)&buf_size, sizeof(int));
	setsockopt(m_Socket, SOL_SOCKET, SO_RCVBUF, (char *)&buf_minsize, sizeof(int));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(m_port);
	addr.sin_addr.s_addr = INADDR_ANY;
	memset(addr.sin_zero, 0, 8);
	if(bind(m_Socket, (const sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
		int err = WSAGetLastError();
		emit ErrorOccurSignal(QString("%1").arg(err));
		closesocket(m_Socket);
		return;
	}

}

MsgMng::~MsgMng()
{

}

void MsgMng::run()
{
	timeval tv;
	fd_set readfds;

	tv.tv_sec = 1;
	tv.tv_usec = 0;

	while(!m_bStop) {
		FD_ZERO(&readfds);
		FD_SET(m_Socket, &readfds);
		if(select(0, &readfds, NULL, NULL, &tv) > 0 && FD_ISSET(m_Socket, &readfds)) {
			//emit RecvMsgSignal();
			MsgBuf *msg = new MsgBuf();
			if(Recv(msg)) {
				emit RecvMsgSignal(msg);
			}
		}
	}
}

void MsgMng::Stop()
{
	m_bStop = true;
}

qint64 MsgMng::MakePacketNo()
{
	qint64 now = (qint64)TimeNo();

	if (now > packetNo) {
		packetNo = now;
	}

	return packetNo++;
}

long MsgMng::TimeNo(void)
{
	SYSTEMTIME	st;
	qint64	ft;

#define UNIXTIME_BASE	((qint64)0x019db1ded53e8000)

	::GetSystemTime(&st);
	::SystemTimeToFileTime(&st, (FILETIME *)&ft);
	return	(qint64)((ft - UNIXTIME_BASE) / 10000000);
}

unsigned long MsgMng::MakeMsg(char *udp_msg, unsigned long command, const char *msg, const char *exMsg, int *packet_len) 
{
	return	MakeMsg(udp_msg, MakePacketNo(), command, msg, exMsg, packet_len);
}

unsigned long MsgMng::MakeMsg(char *buf, qint64 _packetNo, unsigned long command, const char *msg, const char *exMsg, int *packet_len)
{
	int	cmd = GET_MODE(command);
	bool is_broadcast_cmd = (cmd==IPFILE_BR_ENTER || cmd==IPFILE_BR_EXIT) ? true : false;
	int pkt_len = sprintf(buf, "%d:%I64d:%s:%s:%u", IPFILE_VERSION, _packetNo,
		host.userName, host.hostName, command);
	
	if(!is_broadcast_cmd) {
		buf[pkt_len++] = 0;
		pkt_len += sprintf(buf+pkt_len, "%s", msg);

		buf[pkt_len++] = 0;
		pkt_len += sprintf(buf+pkt_len, "%s", exMsg);
	}
	
	*packet_len = pkt_len;
	return	_packetNo;
}

unsigned long MsgMng::Send(HostSub *hostSub, unsigned long command, int val)
{
	char	buf[MAX_NAMEBUF];
	sprintf(buf, "%d", val);
	return	Send(hostSub, command, buf);
}

unsigned long MsgMng::Send(HostSub *hostSub, unsigned long command, const char *message, const char *exMsg)
{
	return	Send(hostSub->addr, hostSub->portNo, command, message, exMsg);
}

unsigned long MsgMng::Send(unsigned long host_addr, int port_no, unsigned command, const char *message, const char *exMsg) 
{
	char	buf[MAX_UDPBUF];
	int		trans_len;

	MakeMsg(buf, command, message, exMsg, &trans_len);
	return	UdpSend(host_addr, port_no, buf, trans_len);
}

bool MsgMng::UdpSend(unsigned long host_addr, int port_no, const char *buf)
{
	return	UdpSend(host_addr, port_no, buf, strlen(buf) +1);
}

bool MsgMng::UdpSend(unsigned long host_addr, int port_no, const char *buf, int len)
{
	struct sockaddr_in	addr;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family			= AF_INET;
	addr.sin_port			= port_no;
	addr.sin_addr.s_addr	= host_addr;

	if (sendto(m_Socket, buf, len, 0, (LPSOCKADDR)&addr, sizeof(addr)) == SOCKET_ERROR) {
		int err = WSAGetLastError();
		emit SendMsgErrorSignal(err);
		return false;
	}

	return	true;
}

bool MsgMng::Recv(MsgBuf *msg)
{
	RecvBuf	buf;

	if (!UdpRecv(&buf) || buf.size == 0)
		return	false;

	return	ResolveMsg(&buf, msg);
}

bool MsgMng::UdpRecv(RecvBuf *buf)
{
	buf->addrSize = sizeof(buf->addr);

	if ((buf->size = recvfrom(m_Socket, buf->msgBuf, sizeof(buf->msgBuf) -1, 0, (struct sockaddr*)&buf->addr, &buf->addrSize)) == SOCKET_ERROR)
		return	false;
	buf->msgBuf[buf->size] = 0;

	return	true;
}


bool MsgMng::ResolveMsg(RecvBuf *buf, MsgBuf *msg)
{
	char	*exStr  = NULL, *tok, *p;
	char	*exStr2 = NULL;
	char	*userName, *hostName;
	int		len, exLen = 0;

	len = strlen(buf->msgBuf); // main message

	if (buf->size > len + 1) { // ex message (group name or attached file)
		exStr = buf->msgBuf + len + 1;
		exLen = strlen(exStr);
		if (buf->size > len + 1 + exLen + 1) { // ex2 message (utf8 entry)
			exStr2 = exStr + exLen + 1;
		}
	}

	msg->hostSub.addr	= buf->addr.sin_addr.s_addr;
	msg->hostSub.portNo	= buf->addr.sin_port;

	if ((tok = separate_token(buf->msgBuf, ':', &p)) == NULL)
		return	false;
	msg->version = atoi(tok);

	//if ((msg->version = atoi(tok)) != IPFILE_VERSION)
	//	return	false;

	if ((tok = separate_token(NULL, ':', &p)) == NULL)
		return	false;
	msg->packetNo = strtoul(tok, 0, 10);
	memcpy(msg->packetNoStr, tok, sizeof(msg->packetNoStr)); 

	if ((userName = separate_token(NULL, ':', &p)) == NULL)
		return	false;
	memcpy(msg->hostSub.userName, userName, sizeof(msg->hostSub.userName));

	if ((hostName = separate_token(NULL, ':', &p)) == NULL)
		return	false;
	memcpy(msg->hostSub.hostName, hostName, sizeof(msg->hostSub.hostName));

	if ((tok = separate_token(NULL, ':', &p)) == NULL)
		return	false;
	msg->command = atol(tok);

	bool  is_broadcast = (msg->command == IPFILE_BR_ENTER || msg->command == IPFILE_BR_EXIT) ? true : false;
	if(!is_broadcast) {
		if(exStr) {
			memcpy(msg->msgBuf, exStr, strlen(exStr));
		}

		if(exStr2) {
			memcpy(msg->exBuf, exStr2, strlen(exStr2));
		}
	}
	return	true;
}
