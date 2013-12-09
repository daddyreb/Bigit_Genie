#ifndef COMMONAPI_H
#define COMMONAPI_H

#ifndef DIAGNOSEDLL_API
#ifdef DIAGNOSEDLL_EXPORTS
#define DIAGNOSEDLL_API __declspec(dllexport)
#else
#define DIAGNOSEDLL_API __declspec(dllimport)
#endif
#endif
#include <Windows.h>

typedef struct
{
	LPTSTR	*strs;
	DWORD	dwCount;
} StrList;

//long time operation interrupt
extern "C" DIAGNOSEDLL_API void __stdcall enable_longtime_operation_interruptable(const int *flag_addr);
extern "C" DIAGNOSEDLL_API void __stdcall disble_longtime_operation_interruptable();
//end

extern "C" DIAGNOSEDLL_API void __stdcall FreeStrListBuffer(StrList *strlist);

#endif

