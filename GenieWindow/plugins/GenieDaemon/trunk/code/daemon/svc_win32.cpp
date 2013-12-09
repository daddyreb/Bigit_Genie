#include "svc_win32.h"
#include "logger.h"
#include <shlobj.h>
#include <dbt.h>
#include <time.h>
#include <string>
#include <vector>

static int wrapper_console_main(int argc, WCHAR *argv[]);
int console_main(int argc, char **argv);
std::string convertToUtf8(const WCHAR *text, int length);
int ak_main(int argc, WCHAR **argv);

static FILE *g_gdl_fp = NULL;

void GDL_Init()
{
	if (g_gdl_fp) {
		return;
	}

	HRESULT hr;
	WCHAR szLoggerPath[MAX_PATH];
	hr = SHGetFolderPathW(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_DEFAULT, szLoggerPath);
	if (FAILED(hr)) {
		return;
	}

	std::wstring path(szLoggerPath);
	path += L"\\NETGEARGenieDaemon.log";

	//if (_wfopen_s(&g_gdl_fp, path.c_str(), L"ab") != 0) {
	if ((g_gdl_fp = _wfsopen(path.c_str(), L"ab", _SH_DENYNO)) == NULL) {
		return;
	}

	char tmbuf[256];
	tm tm1;
	time_t t1;
	time(&t1);
	localtime_s(&tm1, &t1);
	strftime(tmbuf, 255, "%Y-%m-%d %H:%M:%S", &tm1);
	fprintf(g_gdl_fp, "=================================\r\n GenieDaemon %s\r\n=================================\r\n", tmbuf);
	fflush(g_gdl_fp);
}

void GDL_Debug(const char *fmt, ...)
{
	if (!g_gdl_fp) {
		return;
	}

	char tmbuf[256];
	tm tm1;
	time_t t1;
	time(&t1);
	localtime_s(&tm1, &t1);
	strftime(tmbuf, 255, "%Y-%m-%d %H:%M:%S", &tm1);

	va_list args;
	va_start(args, fmt);

	char linebuf[2048];
	int len = vsprintf_s(linebuf, fmt, args);
	linebuf[len] = 0;

	DWORD tid = GetCurrentThreadId();
	DWORD pid = GetCurrentProcessId();

	fprintf(g_gdl_fp, "%s DBG[0x%04x][0x%04x] %s\r\n", tmbuf, pid, tid, linebuf);
	fflush(g_gdl_fp);

	va_end(args);

}

void GDL_Term()
{
	if (g_gdl_fp) {
		fprintf(g_gdl_fp, "\r\n\r\n\r\n");
		fclose(g_gdl_fp);
		g_gdl_fp = NULL;
	}
}

#ifdef _CONSOLE

int wmain(int argc, WCHAR *argv[])
{
	LOGGER_INIT();
	int retval = ak_main(argc, argv);
	LOGGER_DEBUG(("ak_main return: %d", retval));
	LOGGER_TERM();
	return retval;
}

#else

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	LOGGER_INIT();
	int argc;
	lpCmdLine = GetCommandLineW();
	LPWSTR *argv = CommandLineToArgvW(lpCmdLine, &argc);
	if (argv == NULL) {
		LOGGER_DEBUG(("CommandLineToArgvW failed, err = %d", GetLastError()));
		return -1;
	}
	int retval = ak_main(argc, argv);
	LocalFree(argv);
	LOGGER_DEBUG(("ak_main return: %d", retval));
	LOGGER_TERM();
	return retval;
}

#endif // _CONSOLE

#ifdef _WIN32

bool fork64(const WCHAR *arg1)
{
	LOGGER_DEBUG(("checking WOW64..."));

	HMODULE hKernel = GetModuleHandleW(L"kernel32");
	if (!hKernel) {
		LOGGER_DEBUG(("kernel32 not loaded, UNEXPECTED"));
		return false;
	}

	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
	LPFN_ISWOW64PROCESS pfnIsWow64Process = reinterpret_cast<LPFN_ISWOW64PROCESS>(GetProcAddress(hKernel, "IsWow64Process"));
	if (!pfnIsWow64Process) {
		LOGGER_DEBUG(("IsWow64Process symbol not found, assuming 32-bit Windows"));
		return false;
	}

	BOOL bIsWow64 = FALSE;

	if (!pfnIsWow64Process(GetCurrentProcess(), &bIsWow64)) {
		LOGGER_DEBUG(("IsWow64Process failed, err = %d", GetLastError()));
		return false;
	}

	if (!bIsWow64) {
		LOGGER_DEBUG(("not WOW64"));
		return false;
	}

	WCHAR szPath[MAX_PATH * 2 + 1];
	if (!GetModuleFileNameW(NULL, szPath, _countof(szPath) - 1)) {
		LOGGER_DEBUG(("GetModuleFileNameW failed, err = %d", GetLastError()));
		return false;
	}

	std::wstring sPath(szPath);
	size_t sep = sPath.find_last_of(L'.');
	if (sep != std::wstring::npos) {
		sPath = sPath.substr(0, sep);
	}

	sPath += L"64.exe ";
	sPath += arg1;

	DWORD dwExitCode = 35;
	STARTUPINFOW startupInfo;
	memset(&startupInfo, 0, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);
	PROCESS_INFORMATION processInfo;
	if (CreateProcessW(NULL, &sPath[0], NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo)) {
		WaitForSingleObject(processInfo.hProcess, INFINITE);
		GetExitCodeProcess(processInfo.hProcess, &dwExitCode);
		CloseHandle(processInfo.hProcess);
		CloseHandle(processInfo.hThread);
	} else {
		LOGGER_DEBUG(("CreateProcessW failed, err=%d", GetLastError()));
	}

	return true;
}

#endif // _WIN32

int ak_main(int argc, WCHAR *argv[])
{
	// TODO:
	if (argc == 2 && wcscmp(argv[1], L"install") == 0) {
#if defined(_WIN32) && !defined(_CONSOLE)
		if (fork64(L"install")) {
			return 0;
		}
#endif
		LOGGER_DEBUG(("installing service..."));
		int retval = GenieDaemonService::install();
		if (retval != 0) {
			LOGGER_DEBUG(("installing service failed, retval = %d", retval));
		}
		return retval;
	}

	if (argc == 2 && wcscmp(argv[1], L"uninstall") == 0) {
#if defined(_WIN32) && !defined(_CONSOLE)
		if (fork64(L"uninstall")) {
			return 0;
		}
#endif
		LOGGER_DEBUG(("uninstalling service..."));
		int retval = GenieDaemonService::uninstall();
		if (retval != 0) {
			LOGGER_DEBUG(("uninstalling service failed, retval = %d", retval));
		}
		return retval;
	}

	if (argc == 2 && wcscmp(argv[1], L"reinstall") == 0) {
#if defined(_WIN32) && !defined(_CONSOLE)
		if (fork64(L"reinstall")) {
			return 0;
		}
#endif
		LOGGER_DEBUG(("reinstall service..."));
		int retval = GenieDaemonService::reinstall();
		if (retval != 0) {
			LOGGER_DEBUG(("reinstalling service failed, retval = %d", retval));
		}
		return retval;
	}

	LOGGER_DEBUG(("trying to run as service"));
	int err = GenieDaemonService::run();
	if (err == 0) {
		LOGGER_DEBUG(("service stopped"));
	} else if (err == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT) {
		LOGGER_DEBUG(("No service controller, trying to run standalone"));
		return wrapper_console_main(argc, argv);
	} else {
		LOGGER_DEBUG(("StartServiceCtrlDispatcherW failed, err=%d", err));
		return -1;
	}

	return 0;
}

int wrapper_console_main(int argc, WCHAR *argv[])
{
	std::vector<std::string> vec0;
	std::vector<char*> va;
	for (int i = 0; i < argc; i++) {
		vec0.push_back(convertToUtf8(argv[i], -1));
		va.push_back(&vec0.back()[0]);
	}
	return console_main(argc, &va[0]);
}

const WCHAR *GenieDaemonService::s_serviceName = L"NETGEARGenieDaemon";
const WCHAR *GenieDaemonService::s_serviceDesc = L"NETGEARGenieDaemon";

GenieDaemonService::GenieDaemonService()
: m_hStoppedEvent(NULL), m_daemon(NULL)
{
}

GenieDaemonService::~GenieDaemonService()
{
	if (m_hStoppedEvent) {
		CloseHandle(m_hStoppedEvent);
		m_hStoppedEvent = NULL;
	}
}

int GenieDaemonService::install()
{
	WCHAR szPath[MAX_PATH + 1];
	if (!GetModuleFileNameW(NULL, szPath, MAX_PATH)) {
		LOGGER_DEBUG(("GetModuleFileNameW failed, err = %d", GetLastError()));
		return 1;
	}

	SC_HANDLE scm = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (scm == NULL) {
		LOGGER_DEBUG(("OpenSCManagerW failed, err = %d", GetLastError()));
		return 2;
	}

	SC_HANDLE svc;
	svc = CreateServiceW(scm, s_serviceName, s_serviceDesc, SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
		szPath, NULL, NULL, NULL, NULL, NULL);

	if (svc == NULL) {
		LOGGER_DEBUG(("CreateServiceW failed, err = %d", GetLastError()));
		CloseServiceHandle(scm);
		return 6;
	}

	CloseServiceHandle(svc);
	CloseServiceHandle(scm);
	return 0;
}

int GenieDaemonService::uninstall()
{
	DWORD err;

	SC_HANDLE scm = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (scm == NULL) {
		LOGGER_DEBUG(("OpenSCManagerW failed, err = %d", GetLastError()));
		return 2;
	}

	SC_HANDLE svc = OpenServiceW(scm, s_serviceName, SERVICE_ALL_ACCESS);
	if (svc != NULL) {
		SERVICE_STATUS status;
		if (ControlService(svc, SERVICE_CONTROL_STOP, &status)) {
			while (status.dwCurrentState != SERVICE_STOPPED) {
				LOGGER_DEBUG(("waiting service to stop, %dms", status.dwWaitHint));
				Sleep(status.dwWaitHint);

				if (!QueryServiceStatus(svc, &status)) {
					LOGGER_DEBUG(("QueryServiceStatus failed, err = %d", GetLastError()));
					CloseServiceHandle(svc);
					CloseServiceHandle(scm);
					return 7;
				}
			}
		} else {
			err = GetLastError();
			if (err != ERROR_SERVICE_NOT_ACTIVE) {
				LOGGER_DEBUG(("ControlService failed, err = %d", err));
				CloseServiceHandle(svc);
				CloseServiceHandle(scm);
				return 5;
			}
		}

		BOOL fDel = DeleteService(svc);
		CloseServiceHandle(svc);
		if (!fDel) {
			err = GetLastError();
			if (err != ERROR_SERVICE_MARKED_FOR_DELETE) {
				LOGGER_DEBUG(("DeleteService failed, err = %d", err));
				CloseServiceHandle(scm);
				return 4;
			}
		}
		svc = NULL;
	} else {
		err = GetLastError();
		if (err != ERROR_SERVICE_DOES_NOT_EXIST) {
			LOGGER_DEBUG(("OpenServiceW failed, err = %d", err));
			CloseServiceHandle(scm);
			return 3;
		}
	}

	CloseServiceHandle(scm);
	return 0;
}

int GenieDaemonService::reinstall()
{
	DWORD err;
	WCHAR szPath[MAX_PATH + 1];
	if (!GetModuleFileNameW(NULL, szPath, MAX_PATH)) {
		LOGGER_DEBUG(("GetModuleFileNameW failed, err = %d", GetLastError()));
		return 1;
	}

	SC_HANDLE scm = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (scm == NULL) {
		LOGGER_DEBUG(("OpenSCManagerW failed, err = %d", GetLastError()));
		return 2;
	}

	SC_HANDLE svc = OpenServiceW(scm, s_serviceName, SERVICE_ALL_ACCESS);
	if (svc != NULL) {
		SERVICE_STATUS status;
		if (ControlService(svc, SERVICE_CONTROL_STOP, &status)) {
			while (status.dwCurrentState != SERVICE_STOPPED) {
				LOGGER_DEBUG(("waiting service to stop, %dms", status.dwWaitHint));
				Sleep(status.dwWaitHint);

				if (!QueryServiceStatus(svc, &status)) {
					LOGGER_DEBUG(("QueryServiceStatus failed, err = %d", GetLastError()));
					CloseServiceHandle(svc);
					CloseServiceHandle(scm);
					return 7;
				}
			}
		} else {
			err = GetLastError();
			if (err != ERROR_SERVICE_NOT_ACTIVE) {
				LOGGER_DEBUG(("ControlService failed, err = %d", err));
				CloseServiceHandle(svc);
				CloseServiceHandle(scm);
				return 5;
			}
		}

		BOOL fDel = DeleteService(svc);
		CloseServiceHandle(svc);
		if (!fDel) {
			err = GetLastError();
			if (err != ERROR_SERVICE_MARKED_FOR_DELETE) {
				LOGGER_DEBUG(("DeleteService failed, err = %d", err));
				CloseServiceHandle(scm);
				return 4;
			}
		}
		svc = NULL;
	} else {
		err = GetLastError();
		if (err != ERROR_SERVICE_DOES_NOT_EXIST) {
			LOGGER_DEBUG(("OpenServiceW failed, err = %d", err));
			CloseServiceHandle(scm);
			return 3;
		}
	}

	svc = CreateServiceW(scm, s_serviceName, s_serviceDesc, SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
		szPath, NULL, NULL, NULL, NULL, NULL);

	if (svc == NULL) {
		LOGGER_DEBUG(("CreateServiceW failed, err = %d", GetLastError()));
		CloseServiceHandle(scm);
		return 6;
	}

	if (!StartServiceW(svc, 0, NULL)) {
		LOGGER_DEBUG(("StartServiceW failed, err = %d", GetLastError()));
		CloseServiceHandle(svc);
		CloseServiceHandle(scm);
		return 8;
	}

	CloseServiceHandle(svc);
	CloseServiceHandle(scm);
	return 0;
}

int GenieDaemonService::run()
{
	SERVICE_TABLE_ENTRYW dispatchTable[] = {
		{ L"", &GenieDaemonService::ServiceMain },
	};

	if (StartServiceCtrlDispatcherW(dispatchTable)) {
		return NOERROR;
	} else {
		return GetLastError();
	}
}

VOID GenieDaemonService::ServiceMain(DWORD dwArgc, LPWSTR *lpszArgv)
{
	GenieDaemonService svc;
	if (!svc.init(dwArgc, lpszArgv)) {
		return;
	}

	svc.exec();
}

DWORD GenieDaemonService::ServiceCtrlHandler(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext)
{
	return static_cast<GenieDaemonService*>(lpContext)->Handler(dwControl, dwEventType, lpEventData);
}

bool GenieDaemonService::init(DWORD dwArgc, LPWSTR *lpszArgv)
{
	m_hStoppedEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!m_hStoppedEvent) {
		LOGGER_DEBUG(("CreateEvent failed, err = %d", GetLastError()));
		return false;
	}

	m_svcHandle = RegisterServiceCtrlHandlerExW(s_serviceName, &GenieDaemonService::ServiceCtrlHandler, this);
	if (!m_svcHandle) {
		LOGGER_DEBUG(("RegisterServiceCtrlHandlerExW failed, err = %d", GetLastError()));
		return false;
	}

	static const GUID NetAdapterClassGuid = {
		0x4d36e972, 0xe325, 0x11ce, { 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18 }
	};
/*
	DEV_BROADCAST_DEVICEINTERFACE notifyFilter;
	memset(&notifyFilter, 0, sizeof(notifyFilter));
	notifyFilter.dbcc_size = sizeof(notifyFilter);
	notifyFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	notifyFilter.dbcc_classguid = NetAdapterClassGuid;
	m_hDevNotify = RegisterDeviceNotification(m_svcHandle, &notifyFilter, DEVICE_NOTIFY_SERVICE_HANDLE);
	if (!m_hDevNotify) {
		LOGGER_DEBUG(("RegisterDeviceNotification failed, err = %d", GetLastError()));
	}
*/
	m_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	m_status.dwWin32ExitCode = 0;
	reportStatus(SERVICE_START_PENDING, SERVICE_ACCEPT_STOP, 0, 1000);
	return true;
}

DWORD GenieDaemonService::Handler(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData)
{
	LOGGER_DEBUG(("Handler %x %d %p", dwControl, dwEventType, lpEventData));
	if (dwControl == SERVICE_CONTROL_STOP) {
		reportStatus(SERVICE_STOP_PENDING, 0);
		SetEvent(m_hStoppedEvent);
	}

	return NO_ERROR;
}

void GenieDaemonService::exec()
{
	m_daemon = GenieDaemon::create();
	if (!m_daemon || !m_daemon->start()) {
		delete m_daemon;
		m_daemon = NULL;
		m_status.dwWin32ExitCode = ERROR_DEV_NOT_EXIST;
		reportStatus(SERVICE_STOPPED, 0);
		return;
	}

	reportStatus(SERVICE_RUNNING);

	for (;;) {
		DWORD dwWait = WaitForSingleObject(m_hStoppedEvent, 5000);
		if (dwWait == WAIT_OBJECT_0) {
			break;
		} else if (dwWait == WAIT_TIMEOUT) {
			m_daemon->check();
		} else {
			LOGGER_DEBUG(("WaitForSingleObject retval = %d, not good! [err=%d]", dwWait, GetLastError()));
			Sleep(2000);
		}
	}

	m_daemon->stop();
	delete m_daemon;
	m_daemon = NULL;

	if (m_hDevNotify) {
		UnregisterDeviceNotification(m_hDevNotify);
		m_hDevNotify = NULL;
	}

	reportStatus(SERVICE_STOPPED, 0);
}

void GenieDaemonService::reportStatus(DWORD dwCurrentState)
{
	m_status.dwCurrentState = dwCurrentState;
	m_status.dwWin32ExitCode = 0;
	SetServiceStatus(m_svcHandle, &m_status);
}

void GenieDaemonService::reportStatus(DWORD dwCurrentState, DWORD dwControlsAccepted)
{
	m_status.dwCurrentState = dwCurrentState;
	m_status.dwControlsAccepted = dwControlsAccepted;
	m_status.dwWin32ExitCode = 0;
	SetServiceStatus(m_svcHandle, &m_status);
}

void GenieDaemonService::reportStatus(DWORD dwCurrentState, DWORD dwControlsAccepted, DWORD dwCheckPoint)
{
	m_status.dwCurrentState = dwCurrentState;
	m_status.dwControlsAccepted = dwControlsAccepted;
	m_status.dwCheckPoint = dwCheckPoint;
	m_status.dwWin32ExitCode = 0;
	SetServiceStatus(m_svcHandle, &m_status);
}

void GenieDaemonService::reportStatus(DWORD dwCurrentState, DWORD dwControlsAccepted, DWORD dwCheckPoint, DWORD dwWaitHint)
{
	m_status.dwCurrentState = dwCurrentState;
	m_status.dwControlsAccepted = dwControlsAccepted;
	m_status.dwCheckPoint = dwCheckPoint;
	m_status.dwWaitHint = dwWaitHint;
	m_status.dwWin32ExitCode = 0;
	SetServiceStatus(m_svcHandle, &m_status);
}

