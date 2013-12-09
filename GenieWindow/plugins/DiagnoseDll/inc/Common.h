#ifndef DIAGNOSEDLL_COMMON_H
#define DIAGNOSEDLL_COMMON_H

#include <Windows.h>
#include <string>
#include <Rpc.h>

#if defined _UNICODE || defined UNICODE
#define TString		std::wstring
#define RPC_TSTR	RPC_WSTR
#else
#define TString		std::string
#define RPC_TSTR	RPC_CSTR
#endif

DWORD sleep_interruptable(DWORD dwMillionseconds);
DWORD GetOsVersion(DWORD* pdwMinorVersion=NULL);

#endif
