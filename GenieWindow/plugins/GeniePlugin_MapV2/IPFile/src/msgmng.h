#ifndef MSGNETWORKTHREAD_H
#define MSGNETWORKTHREAD_H

#include <QThread>
#include <QString>
#include <WinSock2.h>
#include "IPFileMsg.h"
#include "IFMsg.h"

class MsgMng : public QThread
{
	Q_OBJECT

public:
	MsgMng(SOCKET u, int port);
	~MsgMng();
protected:
	void run();
private:
	bool m_bStop;
	int m_port;
	SOCKET m_Socket;
	qint64 packetNo;
signals:
	void ErrorOccurSignal(QString errString);
	void RecvMsgSignal(MsgBuf* msg);
	void SendMsgErrorSignal(int err);
public:
	void Stop();
public:
	unsigned long Send(HostSub *hostSub, unsigned long command, int val);
	unsigned long Send(HostSub *hostSub, unsigned long command, const char *message=NULL, const char *exMsg=NULL);
	unsigned long Send(unsigned long host_addr, int port_no, unsigned command, const char *message=NULL,
		const char *exMsg=NULL);
	bool ResolveMsg(RecvBuf *buf, MsgBuf *msg);
	qint64 MakePacketNo(void);
	unsigned long MakeMsg(char *udp_msg, qint64 packetNo, unsigned long command, const char *msg,
		const char *exMsg=NULL, int *packet_len=NULL);
	unsigned long MakeMsg(char *udp_msg, unsigned long command, const char *msg, const char *exMsg=NULL,
		int *packet_len=NULL);
	bool UdpSend(unsigned long host_addr, int port_no, const char *buf);
	bool UdpSend(unsigned long host_addr, int port_no, const char *buf, int len);
	bool Recv(MsgBuf *msg);
	bool UdpRecv(RecvBuf *buf);
private:
	HostSub host;
	long TimeNo(void);
};

#endif // MSGNETWORKTHREAD_H
