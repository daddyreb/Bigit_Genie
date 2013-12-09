#ifndef __NetMonitor_h__
#define __NetMonitor_h__

#include <QLibrary>
#include <QIcon>
#include <vector>

typedef struct nmInstance nmInstance;

class NetMonitorImpl;

class NetMonitor
	: public QObject
{
	Q_OBJECT

public:
	NetMonitor(QObject *parent = NULL);
	virtual ~NetMonitor();

	void snapshot(std::vector<quint8>& buffer);
	QIcon getAppIcon(quint32 pid, const QString& path);
	bool testLoop();

public slots:
	void start();
	void stop();

private:
	friend class NetMonitorImpl;
	NetMonitorImpl *m_impl;
};

struct NM_CONNECTION_TCP
{
	quint32 localAddr;
	quint32 localPort;
	quint32 remoteAddr;
	quint32 remotePort;
	quint64 totalIn;
	quint64 totalOut;
	quint64 deltaIn;
	quint64 deltaOut;
};

struct NM_CONNECTION_UDP
{
	quint32 localAddr;
	quint32 localPort;
	quint32 remoteAddr;
	quint32 remotePort;
	quint64 totalIn;
	quint64 totalOut;
	quint64 deltaIn;
	quint64 deltaOut;
};

struct NM_APP_DETAIL
{
	quint32 pid;
	quint32 rsv0;
	quint32 rsv1;
	quint32 rsv2;
	quint32 tcpCount;
	quint32 tcpOffset;
	quint32 udpCount;
	quint32 udpOffset;
	quint32 moduleNameOffset;
	quint32 moduleNameLength;
	quint32 moduleFullPathOffset;
	quint32 moduleFullPathLength;
	quint64 totalIn;
	quint64 totalOut;
	quint64 deltaIn;
	quint64 deltaOut;
};

struct NM_SUMMARY
{
	quint32 version;
	quint32 deltaTime;
	quint32 appDetailCount;
	quint32 appDetailOffset;
	quint64 totalTcpIn;
	quint64 totalTcpOut;
	quint64 deltaTcpIn;
	quint64 deltaTcpOut;
	quint64 totalUdpIn;
	quint64 totalUdpOut;
	quint64 deltaUdpIn;
	quint64 deltaUdpOut;
};

#endif // __NetMonitor_h__
