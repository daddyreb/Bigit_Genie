#include "CommonApi.h"
#include <Windows.h>
#include <assert.h>

extern const int *g_longoperation_interruptable_flag_addr;
extern HANDLE hflag_guard;

extern "C" DIAGNOSEDLL_API void __stdcall enable_longtime_operation_interruptable(const int *flag_addr)
{
	assert(hflag_guard != NULL);
	WaitForSingleObject(hflag_guard,INFINITE);
	g_longoperation_interruptable_flag_addr = flag_addr;
	ReleaseMutex(hflag_guard);
}

extern "C" DIAGNOSEDLL_API void __stdcall disble_longtime_operation_interruptable()
{
	assert(hflag_guard != NULL);
	WaitForSingleObject(hflag_guard,INFINITE);
	g_longoperation_interruptable_flag_addr = NULL;
	ReleaseMutex(hflag_guard);
}

extern "C" DIAGNOSEDLL_API void __stdcall FreeStrListBuffer(StrList *strlist)
{
	if (NULL==strlist)
	{
		return;
	}

	for (DWORD i=0;i<strlist->dwCount;++i)
	{
		delete [] strlist->strs[i];
	}

	delete [] strlist->strs;

	delete strlist;
}