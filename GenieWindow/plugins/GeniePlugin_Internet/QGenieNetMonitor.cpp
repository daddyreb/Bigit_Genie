#include "QGenieNetMonitor.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#if defined(Q_OS_WIN32) && (defined(Q_CC_MSVC) || defined(Q_CC_GNU))
#define NM_CALL_CONV __stdcall
#else
#define NM_CALL_CONV
#endif

typedef enum
{
	NM_OK = 0,
	NM_ERR_UNKNOWN = -1,
	NM_ERR_NULL_PTR = -2,
	NM_ERR_NOT_ENOUGH_BUFFER = -3,
} nmStatus;

typedef struct nmInstance nmInstance;

typedef struct
{
	void (NM_CALL_CONV *Destroy)(nmInstance *pthis);
	void (NM_CALL_CONV *Start)(nmInstance *pthis);
	void (NM_CALL_CONV *Stop)(nmInstance *pthis);
	nmStatus (NM_CALL_CONV *Snapshot)(nmInstance *pthis, void *buffer, int *bufferSize);
	nmStatus (NM_CALL_CONV *GetAppIcon)(nmInstance *pthis, int pid, void **handle);
	nmStatus (NM_CALL_CONV *ReleaseAppIcon)(nmInstance *pthis, void *handle);
	nmStatus (NM_CALL_CONV *TestLoop)(nmInstance *pthis);
} nmFuncTable;

struct nmInstance
{
	nmFuncTable *vtbl;
};

nmStatus NM_CALL_CONV NetMonitorCreate(nmInstance **ppInstance);

#ifdef __cplusplus
};
#endif // __cplusplus

#if 1
#define __NET_MONITOR_USE_IPC__
#endif

#ifdef __NET_MONITOR_USE_IPC__

#include <genie_daemon_intf.h>

class NetMonitorImpl
{
public:
	NetMonitorImpl(NetMonitor *intf);
	~NetMonitorImpl();

	void start();
	void stop();

	void snapshot(std::vector<quint8>& buffer);
	QIcon getAppIcon(quint32 pid, const QString& path);
	bool testLoop();
};

NetMonitorImpl::NetMonitorImpl(NetMonitor *intf)
{
}

NetMonitorImpl::~NetMonitorImpl()
{
}

void NetMonitorImpl::start()
{
}

void NetMonitorImpl::stop()
{
}

void NetMonitorImpl::snapshot(std::vector<quint8>& buffer)
{
	GDF::queryNetTrafficData(buffer);
}

QIcon NetMonitorImpl::getAppIcon(quint32 pid, const QString& path)
{
	QIcon icon;

	GDF::AppIconInfo iconInfo;
	std::vector<quint8> iconData;
	if (GDF::Status_Ok == GDF::queryAppIconData(pid, iconInfo, iconData) && !iconData.empty()) {
		icon = QPixmap::fromImage(QImage(reinterpret_cast<const uchar*>(&iconData[0]), iconInfo.width, iconInfo.height, iconInfo.pitch, QImage::Format_ARGB32));
	}
	return icon;
}

bool NetMonitorImpl::testLoop()
{
	return true;
}

#else

class NetMonitorImpl
{
public:
	NetMonitorImpl(NetMonitor *intf);
	~NetMonitorImpl();

	void start();
	void stop();

	void snapshot(std::vector<quint8>& buffer);
	QIcon getAppIcon(quint32 pid, const QString& path);
	bool testLoop();

private:
	NetMonitor *m_intf;
	QLibrary m_nmlib;
	nmInstance *m_nm;
};

NetMonitorImpl::NetMonitorImpl(NetMonitor *intf)
: m_nmlib("NetMonitor"), m_nm(NULL)
{
	if (m_nmlib.load()) {
		typedef nmStatus (__stdcall *PFN_NetMonitorCreate)(nmInstance **ppInstance);
		PFN_NetMonitorCreate pfnNetMonitorCreate = reinterpret_cast<PFN_NetMonitorCreate>(m_nmlib.resolve("NetMonitorCreate"));
		if (pfnNetMonitorCreate) {
			(*pfnNetMonitorCreate)(&m_nm);
		}
	}
}

NetMonitorImpl::~NetMonitorImpl()
{
	if (m_nm) {
		m_nm->vtbl->Destroy(m_nm);
		m_nm = NULL;
	}
}

void NetMonitorImpl::start()
{
	if (m_nm) {
		m_nm->vtbl->Start(m_nm);
	}
}

void NetMonitorImpl::stop()
{
	if (m_nm) {
		m_nm->vtbl->Stop(m_nm);
	}
}

void NetMonitorImpl::snapshot(std::vector<quint8>& buffer)
{
	if (m_nm) {
		int bufferSize = 4096;
		buffer.resize(bufferSize);
		nmStatus status;
		while (NM_ERR_NOT_ENOUGH_BUFFER == (status = m_nm->vtbl->Snapshot(m_nm, &buffer[0], &bufferSize))) {
			bufferSize <<= 1;
			buffer.resize(bufferSize);
		}
		if (status == NM_OK) {
			buffer.resize(bufferSize);
		} else {
			buffer.clear();
		}
	} else {
		buffer.clear();
	}
}

QIcon NetMonitorImpl::getAppIcon(quint32 pid, const QString& path)
{
	QIcon icon;
#ifdef Q_OS_WIN32
	if (m_nm) {
		HICON hIcon;
		if (NM_OK == m_nm->vtbl->GetAppIcon(m_nm, pid, reinterpret_cast<void**>(&hIcon))) {
			icon = QPixmap::fromWinHICON(hIcon);
			m_nm->vtbl->ReleaseAppIcon(m_nm, reinterpret_cast<void*>(hIcon));
		}
	}
#endif // Q_OS_WIN32
	return icon;
}

bool NetMonitorImpl::testLoop()
{
	if (m_nm) {
		return m_nm->vtbl->TestLoop(m_nm) == NM_OK;
	}
	return true;
}

#endif // __NET_MONITOR_USE_IPC__

NetMonitor::NetMonitor(QObject *parent)
: QObject(parent)
{
	m_impl = new NetMonitorImpl(this);
}

NetMonitor::~NetMonitor()
{
	delete m_impl;
}

void NetMonitor::start()
{
	m_impl->start();
}

void NetMonitor::stop()
{
	m_impl->stop();
}

void NetMonitor::snapshot(std::vector<quint8>& buffer)
{
	m_impl->snapshot(buffer);
}

QIcon NetMonitor::getAppIcon(quint32 pid, const QString& path)
{
	return m_impl->getAppIcon(pid, path);
}

bool NetMonitor::testLoop()
{
	return m_impl->testLoop();
}
