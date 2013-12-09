#include "stdafx.h"
#include "wpinst.h"

#if 0

#define LOGGER_INIT(x)  LoggerInit(x)
#define LOGGER_DEBUG(x) LoggerDebug x
#define LOGGER_INFO(x)  LoggerInfo x
#define LOGGER_ERROR(x) LoggerError x
#define LOGGER_TERM()   LoggerTerm()

FILE *g_LoggerFP = NULL;

void LoggerInit(const std::wstring& path)
{
	g_LoggerFP = _wfopen(path.c_str(), L"w");
}

void __LoggerOutput(FILE *fp, const WCHAR *lbl, const WCHAR *fmt, va_list args)
{
	WCHAR tmbuf[256];
	tm tm1;
	time_t t1;
	time(&t1);
	localtime_s(&tm1, &t1);
	wcsftime(tmbuf, 255, L"%Y-%m-%d %H:%M:%S", &tm1);

	WCHAR buff[1024];
	int len = vswprintf_s(buff, fmt, args);
	buff[len] = 0;
	fwprintf(fp, L"[%s][%s] %s\n", lbl, tmbuf, buff);
	fflush(fp);
}

void LoggerDebug(const WCHAR *fmt, ...)
{
	if (!g_LoggerFP) {
		return;
	}

	va_list args;
	va_start(args, fmt);
	__LoggerOutput(g_LoggerFP, L"DBG", fmt, args);
}

void LoggerInfo(const WCHAR *fmt, ...)
{
	if (!g_LoggerFP) {
		return;
	}

	va_list args;
	va_start(args, fmt);
	__LoggerOutput(g_LoggerFP, L"INF", fmt, args);
}

void LoggerError(const WCHAR *fmt, ...)
{
	if (!g_LoggerFP) {
		return;
	}

	va_list args;
	va_start(args, fmt);
	__LoggerOutput(g_LoggerFP, L"ERR", fmt, args);
}

void LoggerTerm()
{
	if (g_LoggerFP) {
		fclose(g_LoggerFP);
		g_LoggerFP = NULL;
	}
}

#else

#define LOGGER_INIT(x)  __noop
#define LOGGER_DEBUG(x) __noop
#define LOGGER_INFO(x)  __noop
#define LOGGER_ERROR(x) __noop
#define LOGGER_TERM()   __noop

#endif

#ifdef _WIN64

struct BLOCK_HEADER
{
	DWORD magic0;
	DWORD magic1;
	DWORD magic2;
	DWORD magic3;
	WCHAR path[512];
};

BLOCK_HEADER g_header = {
	0xBAD2C0DE, 0xDEC0D2BA, 0xBABEFACE, 0xCEFABEBA,
	{ 0 }
};

std::vector<unsigned char> g_data0;

const WCHAR *g_files[] = {
	L"syswow64\\packet.dll",
	L"syswow64\\wpcap.dll",
	L"system32\\drivers\\npf.sys",
	L"system32\\packet.dll",
	L"system32\\wpcap.dll",
};

const int g_FileBase = 2;
const int g_DriverIndex = 2;

int main64();

#else

#include "data_bin.h"

const WCHAR *g_files[] = {
	L"system32\\drivers\\npf.sys",
	L"system32\\packet.dll",
	L"system32\\wpcap.dll",
};

const int g_FileBase = 1;
const int g_DriverIndex = 0;

std::wstring g_TempPath;
WCHAR g_ImagePath[500] = { 0 };
DWORD g_ImagePathLength = 0;

#ifdef MAKE_UNINST
int main32_uninst();
#else
int main32(int argc, WCHAR *argv[]);
#endif // MAKE_UNINST

#endif // _WIN64

unsigned long g_data_len = 0;
const unsigned char *g_data = NULL;
std::wstring g_windir;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	std::wstring sTempPath(500, 0);
	DWORD len = GetTempPath(static_cast<DWORD>(sTempPath.length() - 1), &sTempPath[0]);
	if (len == 0) {
		return 10;
	}
	sTempPath.resize(len);

#ifdef _WIN64
	LOGGER_INIT(sTempPath + L"wpinst64.log");
#else
	LOGGER_INIT(sTempPath + L"wpinst.log");
	g_TempPath = sTempPath;

	if ((g_ImagePathLength = GetModuleFileName(NULL, g_ImagePath, _countof(g_ImagePath) - 1)) == 0) {
		LOGGER_ERROR((L"GetModuleFileName() failed, err=%d", GetLastError()));
		return 11;
	}
	g_ImagePath[g_ImagePathLength] = 0;
	LOGGER_INFO((L"Image Path: [%s]", g_ImagePath));

#endif

	g_windir.resize(500);
	len = GetWindowsDirectory(&g_windir[0], static_cast<UINT>(g_windir.length() - 1));
	if (len == 0) {
		LOGGER_ERROR((L"GetWindowsDirectory() failed, err=%d", GetLastError()));
		return 12;
	}

	g_windir.resize(len);
	g_windir.push_back(L'\\');
	LOGGER_INFO((L"Windows Directory: [%s]", g_windir.c_str()));

	int retval;
#ifdef _WIN64
	retval = main64();
#else
#ifdef MAKE_UNINST
	retval = main32_uninst();
#else
	int argc;
	LPWSTR *argv = CommandLineToArgvW(GetCommandLine(), &argc);
	retval = main32(argc, argv);
#endif
#endif
	LOGGER_DEBUG((L"main() retval=%d", retval));

	LOGGER_TERM();
	return retval;
}

#ifdef _WIN64

int main64()
{
#ifdef MAKE_UNINST
	return DoUninstall();
#else
	LOGGER_DEBUG((L"magic %08x %08x %08x %08x [%s]", g_header.magic0, g_header.magic1, g_header.magic2, g_header.magic3, g_header.path));
	if (g_header.magic0 == 0xBAD2C0DE || g_header.magic1 == 0xDEC0D2BA || g_header.path[0] == 0) {
		LOGGER_ERROR((L"Bad data header"));
		return 60;
	}

	bool fUninstall = g_header.magic2 != 0;

	if (!fUninstall) {
		std::ifstream fIn(g_header.path, std::ios_base::in|std::ios_base::binary);
		if (!fIn) {
			LOGGER_ERROR((L"Reading data failed"));
			return 61;
		}

		g_data0.resize(g_header.magic1);
		fIn.seekg(g_header.magic0);
		fIn.read(reinterpret_cast<char*>(&g_data0[0]), g_data0.size());
		g_data = &g_data0[0];
		g_data_len = static_cast<unsigned long>(g_data0.size());
	}

	return Process(fUninstall);
#endif // MAKE_UNINST
}

#else

bool IsWow64()
{
	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

	LPFN_ISWOW64PROCESS pfnIsWow64Process = reinterpret_cast<LPFN_ISWOW64PROCESS>(GetProcAddress(GetModuleHandle(_TEXT("kernel32")), "IsWow64Process"));
	BOOL bIsWow64 = FALSE;
	if (pfnIsWow64Process) {
		pfnIsWow64Process(GetCurrentProcess(), &bIsWow64);
	}
	return bIsWow64 ? true : false;
}

int Fork64(bool fUninstall)
{
	std::vector<unsigned char> bin;
	if (!ExtractData(0, bin)) {
		LOGGER_ERROR((L"Failed to extract 64bit binary"));
		return 31;
	}

#if !defined(MAKE_UNINST)
	const DWORD *pMagic = reinterpret_cast<const DWORD*>(g_data);
	*reinterpret_cast<DWORD*>(&bin[0] + pMagic[0]) = pMagic[1];
	*reinterpret_cast<DWORD*>(&bin[0] + pMagic[0] + 4) = g_data_len;
	*reinterpret_cast<DWORD*>(&bin[0] + pMagic[0] + 8) = fUninstall ? 1 : 0;
	memcpy(&bin[0] + pMagic[0] + 16, g_ImagePath, g_ImagePathLength * sizeof(WCHAR));
#endif // !MAKE_UNINST

	std::wstring sExePath(g_TempPath);
	sExePath += L"wpinst64.exe";

	{
		std::ofstream fOut(sExePath.c_str(), std::ios_base::out|std::ios_base::trunc|std::ios_base::binary);
		if (!fOut) {
			LOGGER_ERROR((L"Failed to write 64bit binary"));
			return 32;
		}

		fOut.write(reinterpret_cast<char*>(&bin[0]), bin.size());
	}

	DWORD dwExitCode = 35;
	STARTUPINFO startupInfo;
	memset(&startupInfo, 0, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);
	PROCESS_INFORMATION  processInfo;
	if (CreateProcess(sExePath.c_str(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo)) {
		WaitForSingleObject(processInfo.hProcess, INFINITE);
		GetExitCodeProcess(processInfo.hProcess, &dwExitCode);
		CloseHandle(processInfo.hProcess);
		CloseHandle(processInfo.hThread);
	} else {
		LOGGER_ERROR((L"CreateProcess() failed, err=%d", GetLastError()));
	}
	DeleteFile(sExePath.c_str());
	return dwExitCode;
}

#ifdef MAKE_UNINST

int main32_uninst()
{
	g_data = get_data_bin(&g_data_len);
	const DWORD *magic = reinterpret_cast<const DWORD*>(g_data);

	LOGGER_DEBUG((L"magic %08x %08x %08x %08x", magic[0], magic[1], magic[2], magic[3]));

	int retval;

	if (IsWow64()) {
		LOGGER_INFO((L"WOW64 forking"));
		retval = Fork64(true);
		LOGGER_INFO((L"WOW64 retval=%d", retval));
	} else {
		retval = DoUninstall();
	}

	return retval;
}

#else

int main32(int argc, WCHAR *argv[])
{
	g_data = get_data_bin(&g_data_len);
	const DWORD *magic = reinterpret_cast<const DWORD*>(g_data);

	LOGGER_DEBUG((L"magic %08x %08x %08x %08x", magic[0], magic[1], magic[2], magic[3]));

	if (magic[0] == 0xBAD2C0DE || magic[1] == 0xDEC0D2BA) {
		LOGGER_ERROR((L"Bad data header"));
		return 30;
	}

	bool fUninstall = false;
	if (argc >= 2 && (argv[1][0] == L'/' || argv[1][0] == L'-') && _wcsicmp(argv[1]+1, L"uninstall") == 0) {
		fUninstall = true;
	}

	int retval;

	if (IsWow64()) {
		LOGGER_INFO((L"WOW64 forking"));
		retval = Fork64(fUninstall);
		LOGGER_INFO((L"WOW64 retval=%d", retval));
	} else {
		retval = Process(fUninstall);
	}

	return retval;
}

#endif // MAKE_UNINST

#endif // _WIN64

#ifdef MAKE_UNINST
#else

bool ExtractFile(const WCHAR *path, int index)
{
	std::vector<unsigned char> data;
	if (!ExtractData(index, data)) {
		return false;
	}

	std::ofstream fOut(path, std::ios_base::out|std::ios_base::trunc|std::ios_base::binary);
	if (!fOut) {
		return false;
	}

	fOut.write(reinterpret_cast<char*>(&data[0]), data.size());
	return true;
}

int Process(bool fUninstall)
{
	if (fUninstall) {
		return DoUninstall();
	} else {
		DoUninstall();
		return DoInstall();
	}
}

int DoInstall()
{
	LOGGER_INFO((L"Installing..."));

	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (!hSCM) {
		LOGGER_ERROR((L"OpenSCManager() failed, err=%d", GetLastError()));
		return 51;
	}

	SC_HANDLE hSvc = OpenService(hSCM, L"NPF", SERVICE_ALL_ACCESS);
	if (hSvc) {
		LOGGER_INFO((L"NPF service already installed, cancel installation"));
		CloseServiceHandle(hSvc);
		CloseServiceHandle(hSCM);
		return 0;
	}

	for (size_t i = 0; i < _countof(g_files); i++) {
		std::wstring sFilePath(g_windir + g_files[i]);
		LOGGER_INFO((L"Extracting [%s]", sFilePath.c_str()));
		if (!ExtractFile(sFilePath.c_str(), static_cast<int>(g_FileBase + i))) {
			LOGGER_ERROR((L"Failed to extract [%s]", sFilePath.c_str()));
			CloseServiceHandle(hSCM);
			return 50;
		}
	}

	std::wstring serviceBin = g_windir + g_files[g_DriverIndex];
	LOGGER_DEBUG((L"Creating NPF service [%s] ...", serviceBin.c_str()));
	hSvc = CreateService(hSCM, L"NPF", L"NetGroup Packet Filter Driver", SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, serviceBin.c_str(), NULL, NULL, NULL, NULL, NULL);
	if (!hSvc) {
		LOGGER_ERROR((L"CreateService() failed, err=%d", GetLastError()));
		CloseServiceHandle(hSCM);
		return 52;
	}

	LOGGER_DEBUG((L"Starting NPF service..."));
	if (!StartService(hSvc, 0, NULL)) {
		LOGGER_ERROR((L"StartService() failed, err=%d", GetLastError()));
		CloseServiceHandle(hSvc);
		CloseServiceHandle(hSCM);
		return 53;
	}

	CloseServiceHandle(hSCM);

	return 0;
}

#endif // MAKE_UNINST

#if !defined(_WIN64) || !defined(MAKE_UNINST)

bool ExtractData(int index, std::vector<unsigned char>& data)
{
	const DWORD *pOffset = reinterpret_cast<const DWORD*>(g_data) + 4 + index * 3;
	DWORD length = *(pOffset + 1);
	DWORD flen = *(pOffset + 2);
	if (*pOffset + length > g_data_len) {
		return false;
	}
	data.resize(flen);
	z_stream strm;
	memset(&strm, 0, sizeof(strm));
	if (Z_OK != inflateInit(&strm)) {
		return false;
	}

	strm.next_in = const_cast<unsigned char*>(g_data) + *pOffset;
	strm.avail_in = length;
	strm.next_out = &data[0];
	strm.avail_out = static_cast<uInt>(data.size());
	int retval = inflate(&strm, Z_FINISH);
	inflateEnd(&strm);
	if (retval != Z_STREAM_END) {
		return false;
	}

	return true;
}

#endif

int DoUninstall()
{
	LOGGER_INFO((L"Uninstalling..."));
	
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, DELETE);
	if (!hSCM) {
		LOGGER_ERROR((L"OpenSCManager() failed, err=%d", GetLastError()));
		return 40;
	}

	SC_HANDLE hSvc = OpenService(hSCM, L"NPF", SERVICE_ALL_ACCESS);
	if (hSvc) {
		SERVICE_STATUS status;
		LOGGER_INFO((L"Stopping NPF service..."));
		if (ControlService(hSvc, SERVICE_CONTROL_STOP, &status)) {
			LOGGER_INFO((L"NPF service stopped OK"));
		} else {
			DWORD err = GetLastError();
			if (err == ERROR_SERVICE_NOT_ACTIVE) {
				LOGGER_INFO((L"NPF service not running"));
			} else {
				LOGGER_ERROR((L"ControlService() failed, err=%d", err));
				CloseServiceHandle(hSvc);
				CloseServiceHandle(hSCM);
				return 42;
			}
		}

		if (DeleteService(hSvc)) {
			LOGGER_INFO((L"NPF service deleted"));
		} else {
			LOGGER_ERROR((L"DeleteService() failed, err=%d", GetLastError()));
			CloseServiceHandle(hSvc);
			CloseServiceHandle(hSCM);
			return 43;
		}

		CloseServiceHandle(hSvc);

	} else {
		DWORD err = GetLastError();
		if (err == ERROR_SERVICE_DOES_NOT_EXIST) {
			LOGGER_INFO((L"NPF service not installed"));
		} else {
			LOGGER_ERROR((L"OpenService() failed, err=%d", err));
			CloseServiceHandle(hSCM);
			return 41;
		}
	}

	CloseServiceHandle(hSCM);

	for (size_t i = 0; i < _countof(g_files); i++) {
		std::wstring sFilePath(g_windir + g_files[i]);
		LOGGER_DEBUG((L"Deleting [%s]", sFilePath.c_str()));
		if (!DeleteFile(sFilePath.c_str())) {
			DWORD err = GetLastError();
			if (err != ERROR_FILE_NOT_FOUND) {
				LOGGER_ERROR((L"DeleteFile() failed, err=%d", err));
				return 44;
			}
		}
	}

	return 0;
}
