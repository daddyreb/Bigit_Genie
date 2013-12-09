// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>

const int *	g_longoperation_interruptable_flag_addr = NULL;
HANDLE		hflag_guard								= NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		hflag_guard = CreateMutex(NULL,FALSE,NULL);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		CloseHandle(hflag_guard);
		hflag_guard = NULL;
		break;
	}
	return TRUE;
}

