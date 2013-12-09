#ifndef NETCARDAPI_H
#define NETCARDAPI_H

#include <Windows.h>
#include <tchar.h>
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DIAGNOSEDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DIAGNOSEDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifndef DIAGNOSEDLL_API
#ifdef DIAGNOSEDLL_EXPORTS
#define DIAGNOSEDLL_API __declspec(dllexport)
#else
#define DIAGNOSEDLL_API __declspec(dllimport)
#endif
#endif


#include "Common.h"
#include "CommonApi.h"

typedef struct  
{
	DWORD			m_dwId;
	BOOL			m_bDisabled;
	DWORD			m_dwNicType;//1 有线网卡,2无线网卡
	TCHAR			m_chDevDesc[512];
	TCHAR			m_chDevGuid[128];
}NIC,*LPNIC;

typedef struct
{
	LPNIC	lpNics;
	DWORD	dwCount;
} NICLIST;


extern "C" DIAGNOSEDLL_API BOOL __stdcall EnumInterfaces(NICLIST **Niclist);

extern "C" DIAGNOSEDLL_API BOOL __stdcall GetInterfaceDnsserver(LPCTSTR lpNicGuid,StrList **sl);
extern "C" DIAGNOSEDLL_API BOOL __stdcall GetInterfaceGateway(LPCTSTR lpNicGuid,StrList **sl);

extern "C" DIAGNOSEDLL_API void __stdcall FreeNicListBuffer(NICLIST *niclist);

extern "C" DIAGNOSEDLL_API int __stdcall IsNicGatewaysOk(LPCTSTR lpNicGuid,int interrupt_flag);
extern "C" DIAGNOSEDLL_API int __stdcall IsNicDnsServersOk(LPCTSTR lpNicGuid);


extern "C" DIAGNOSEDLL_API BOOL __stdcall IsNicCablePluggedin(LPCTSTR lpNicGuid);
extern "C" DIAGNOSEDLL_API int __stdcall IsNicDisabled(LPCTSTR lpNicGuid);



extern "C" DIAGNOSEDLL_API BOOL __stdcall NetCardStateChange(LPCTSTR lpNicGuid, BOOL Enabled);

extern "C" DIAGNOSEDLL_API int __stdcall IsDhcpEnabled(LPCTSTR lpNicGuid);
extern "C" DIAGNOSEDLL_API BOOL __stdcall EnableDhcp(LPCTSTR lpNicGuid,int interrupt_flag);

extern "C" DIAGNOSEDLL_API BOOL __stdcall EnableDnsFromDhcp(LPCTSTR lpNicGuid,int interrupt_flag);
extern "C" DIAGNOSEDLL_API BOOL __stdcall SetInterfaceDnses(LPCTSTR lpNicGuid,DWORD dwDnsc,LPTSTR szDnses[],int interrupt_flag);



extern "C" DIAGNOSEDLL_API int __stdcall RepairGatewayorDnsOk(LPCTSTR lpNicGuid,BOOL bGatewayorDns,int interrupt_flag);
extern "C" DIAGNOSEDLL_API BOOL __stdcall ConfigDNSManually(LPCTSTR lpNicGuid,int interrupt_flag);
extern "C" DIAGNOSEDLL_API BOOL __stdcall IpRenewInterface(LPCTSTR lpNicGuid);
extern "C" DIAGNOSEDLL_API BOOL __stdcall IsHostReachble(const char* szDestIp,unsigned long timeoutms);
extern "C" DIAGNOSEDLL_API BOOL __stdcall IsThisPhysicalNic(LPCTSTR lpNicGuid);

#endif
