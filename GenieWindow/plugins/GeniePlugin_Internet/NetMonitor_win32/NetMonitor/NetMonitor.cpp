// NetMonitor.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "NetMonitorImpl.h"
#include "NetMonitor.h"

class CInstance
	: public nmInstance
{
public:
	CInstance();
	~CInstance();

private:
	static void NM_CALL_CONV __Destroy(nmInstance *pthis);
	static void NM_CALL_CONV __Start(nmInstance *pthis);
	static void NM_CALL_CONV __Stop(nmInstance *pthis);
	static nmStatus NM_CALL_CONV __Snapshot(nmInstance *pthis, void *buffer, int *bufferSize);
	static nmStatus NM_CALL_CONV __GetAppIcon(nmInstance *pthis, int pid, void **handle);
	static nmStatus NM_CALL_CONV __ReleaseAppIcon(nmInstance *pthis, void *handle);
	static nmStatus NM_CALL_CONV __TestLoop(nmInstance *pthis);

	static nmFuncTable s_vtbl;
	CNetMonitorWinPcap m_cap;
};

nmFuncTable CInstance::s_vtbl = {
	&CInstance::__Destroy,
	&CInstance::__Start,
	&CInstance::__Stop,
	&CInstance::__Snapshot,
	&CInstance::__GetAppIcon,
	&CInstance::__ReleaseAppIcon,
	&CInstance::__TestLoop
};

nmStatus __stdcall NetMonitorCreate(nmInstance **ppInstance)
{
	if (!ppInstance) {
		return NM_ERR_NULL_PTR;
	}

	*ppInstance = new CInstance();

	return NM_OK;
}

CInstance::CInstance()
{
	vtbl = &s_vtbl;
}

CInstance::~CInstance()
{
}

void CInstance::__Destroy(nmInstance *pthis)
{
	delete static_cast<CInstance*>(pthis);
}

void CInstance::__Start(nmInstance *pthis)
{
	static_cast<CInstance*>(pthis)->m_cap.Start();
}

void CInstance::__Stop(nmInstance *pthis)
{
	static_cast<CInstance*>(pthis)->m_cap.Stop();
}

nmStatus CInstance::__Snapshot(nmInstance *pthis, void *buffer, int *bufferSize)
{
	return static_cast<CInstance*>(pthis)->m_cap.Snapshot(buffer, bufferSize);
}

nmStatus CInstance::__GetAppIcon(nmInstance *pthis, int pid, void **handle)
{
	return static_cast<CInstance*>(pthis)->m_cap.GetAppIcon(pid, handle);
}

nmStatus CInstance::__ReleaseAppIcon(nmInstance *pthis, void *handle)
{
	return static_cast<CInstance*>(pthis)->m_cap.ReleaseAppIcon(handle);
}

nmStatus CInstance::__TestLoop(nmInstance *pthis)
{
	return static_cast<CInstance*>(pthis)->m_cap.TestLoop();
}
