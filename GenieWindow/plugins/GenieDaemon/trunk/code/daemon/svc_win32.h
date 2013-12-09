#ifndef __svc_win32_h__
#define __svc_win32_h__

#include <windows.h>
#include "daemon.h"

class GenieDaemonService
{
public:
	static int install();
	static int uninstall();
	static int reinstall();
	static int run();

private:
	GenieDaemonService();
	~GenieDaemonService();

	bool init(DWORD dwArgc, LPWSTR *lpszArgv);
	void exec();

	static VOID WINAPI ServiceMain(DWORD dwArgc, LPWSTR *lpszArgv);
	static DWORD WINAPI ServiceCtrlHandler(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext);
	DWORD Handler(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData);
	void reportStatus(DWORD dwCurrentState);
	void reportStatus(DWORD dwCurrentState, DWORD dwControlsAccepted);
	void reportStatus(DWORD dwCurrentState, DWORD dwControlsAccepted, DWORD dwCheckPoint);
	void reportStatus(DWORD dwCurrentState, DWORD dwControlsAccepted, DWORD dwCheckPoint, DWORD dwWaitHint);

private:
	static const WCHAR *s_serviceName;
	static const WCHAR *s_serviceDesc;

	SERVICE_STATUS_HANDLE m_svcHandle;
	HANDLE m_hStoppedEvent;
	SERVICE_STATUS m_status;
	HDEVNOTIFY m_hDevNotify;
	GenieDaemon *m_daemon;
};

#endif // __svc_win32_h__
