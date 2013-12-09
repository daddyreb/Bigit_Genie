#include "Common.h"
#include <assert.h>

extern const int *g_longoperation_interruptable_flag_addr;
extern HANDLE hflag_guard;

BOOL is_longoperation_canceled()
{
	assert(hflag_guard != NULL);
	WaitForSingleObject(hflag_guard,INFINITE);
	BOOL bresult = (g_longoperation_interruptable_flag_addr && *g_longoperation_interruptable_flag_addr);
	ReleaseMutex(hflag_guard);
	return bresult;
}

DWORD sleep_interruptable(DWORD dwMillionseconds)
{
	assert ( dwMillionseconds > 0 );

	DWORD times		= dwMillionseconds / 100;
	DWORD remainder = dwMillionseconds % 100;


	while(times-- > 0)
	{
		if (is_longoperation_canceled())
		{
			//canceled
			return (times * 100 + remainder);
		}
		Sleep(100);
	}


	if(remainder > 0)
	{
		if (is_longoperation_canceled())
		{
			//canceled
			return remainder;
		}

		Sleep(remainder);
	}

	return 0;
}

//功能：	获取系统的主版本号和副版本号
//参数：	pdwMinorVersion输出参数，不为NULL时返回副版本号
//返回值：	系统的主版本号

DWORD GetOsVersion(DWORD* pdwMinorVersion)
{
	OSVERSIONINFOEX osvi;
	DWORD			dwMajorVersion=0;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if (!GetVersionEx((OSVERSIONINFO *)&osvi))
	{
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (!GetVersionEx((OSVERSIONINFO *)&osvi))
		{
			if (pdwMinorVersion!=NULL)
			{
				*pdwMinorVersion=0;
			}
			return 0;
		}
	}

	if (osvi.dwPlatformId != VER_PLATFORM_WIN32_NT)
	{
		if (pdwMinorVersion!=NULL)
		{
			*pdwMinorVersion=0;
		}
		return 0;
	}

	if (pdwMinorVersion!=NULL)
	{
		*pdwMinorVersion=osvi.dwMinorVersion;
	}

	return osvi.dwMajorVersion;
}