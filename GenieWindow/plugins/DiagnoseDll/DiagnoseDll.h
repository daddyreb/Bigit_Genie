#ifndef DIAGNOSEDLL_H
#define DIAGNOSEDLL_H

#include <Windows.h>
#include <tchar.h>
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DIAGNOSEDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DIAGNOSEDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef DIAGNOSEDLL_EXPORTS
#define DIAGNOSEDLL_API __declspec(dllexport)
#else
#define DIAGNOSEDLL_API __declspec(dllimport)
#endif

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

typedef struct
{
	LPTSTR	*strs;
	DWORD	dwCount;
} StrList;

typedef struct  
{
	TCHAR	strProfileName[256];
	TCHAR	strSsid[64];
	TCHAR	strBssType[32];
	TCHAR	strdot11DefaultAuthAlgorithm[64];
	TCHAR	strdot11DefaultCipherAlgorithm[64];
	UINT	wlanSignalQuality;
	BOOL	bSecurityEnabled;
	BOOL	bcurConnected;
	BOOL	bhasProfile;
	BOOL	bNetworkConnectable;
}Visible_Network;

//long time operation interrupt
extern "C" DIAGNOSEDLL_API void __stdcall enable_longtime_operation_interruptable(const int *flag_addr);
extern "C" DIAGNOSEDLL_API void __stdcall disble_longtime_operation_interruptable();
//end

extern "C" DIAGNOSEDLL_API BOOL __stdcall EnumInterfaces(NICLIST **Niclist);

extern "C" DIAGNOSEDLL_API BOOL __stdcall GetInterfaceDnsserver(LPCTSTR lpNicGuid,StrList **sl);
extern "C" DIAGNOSEDLL_API BOOL __stdcall GetInterfaceGateway(LPCTSTR lpNicGuid,StrList **sl);

extern "C" DIAGNOSEDLL_API void __stdcall FreeNicListBuffer(NICLIST *niclist);
extern "C" DIAGNOSEDLL_API void __stdcall FreeStrListBuffer(StrList *strlist);

extern "C" DIAGNOSEDLL_API int __stdcall IsNicGatewaysOk(LPCTSTR lpNicGuid,int interrupt_flag);
extern "C" DIAGNOSEDLL_API int __stdcall IsNicDnsServersOk(LPCTSTR lpNicGuid);


extern "C" DIAGNOSEDLL_API BOOL __stdcall IsNicCablePluggedin(LPCTSTR lpNicGuid);
extern "C" DIAGNOSEDLL_API int __stdcall IsNicDisabled(LPCTSTR lpNicGuid);
extern "C" DIAGNOSEDLL_API int __stdcall IsWlanNicConnected(LPCTSTR lpNicGuid);
extern "C" DIAGNOSEDLL_API int __stdcall IsWlanNicSwitchedOff(LPCTSTR lpNicGuid);


extern "C" DIAGNOSEDLL_API BOOL __stdcall NetCardStateChange(LPCTSTR lpNicGuid, BOOL Enabled);

extern "C" DIAGNOSEDLL_API BOOL __stdcall SetInterfaceProfile(LPCTSTR lpNicGuid,LPCTSTR lpszSsid,LPCTSTR lpszAuth,LPCTSTR lpszCipher,LPCTSTR lpszPwd);

extern "C" DIAGNOSEDLL_API int __stdcall IsDhcpEnabled(LPCTSTR lpNicGuid);
extern "C" DIAGNOSEDLL_API BOOL __stdcall EnableDhcp(LPCTSTR lpNicGuid,int interrupt_flag);

extern "C" DIAGNOSEDLL_API BOOL __stdcall EnableDnsFromDhcp(LPCTSTR lpNicGuid,int interrupt_flag);
extern "C" DIAGNOSEDLL_API BOOL __stdcall SetInterfaceDnses(LPCTSTR lpNicGuid,DWORD dwDnsc,LPTSTR szDnses[],int interrupt_flag);

extern "C" DIAGNOSEDLL_API BOOL __stdcall SwitchWlanNic(LPCTSTR lpNicGuid,BOOL bSwitchOn);

extern "C" DIAGNOSEDLL_API int __stdcall RepairGatewayorDnsOk(LPCTSTR lpNicGuid,BOOL bGatewayorDns,int interrupt_flag);
extern "C" DIAGNOSEDLL_API BOOL __stdcall ConfigDNSManually(LPCTSTR lpNicGuid,int interrupt_flag);
extern "C" DIAGNOSEDLL_API BOOL __stdcall ReConnectWlan(LPCTSTR lpNicGuid,int interrupt_flag);
extern "C" DIAGNOSEDLL_API BOOL __stdcall UiEditProfile(LPCTSTR lpNicGuid,LPCTSTR lpProfileName);
extern "C" DIAGNOSEDLL_API BOOL __stdcall ConnectToWlan(LPCTSTR lpNicGuid,LPCTSTR lpSsid,LPCTSTR lpPwd,int interrupt_flag);
extern "C" DIAGNOSEDLL_API BOOL __stdcall ReConnectCurWlan(LPCTSTR lpNicGuid,LPCTSTR lpNewPwd,int interrupt_flag);
extern "C" DIAGNOSEDLL_API int __stdcall  GetWlanChannel(LPCTSTR lpNicGuid,const  LPCTSTR lpSsid,LPCTSTR lpdot11BssType,BOOL bSecurityEnabled);
extern "C" DIAGNOSEDLL_API BOOL __stdcall GetVisibleWlanNetworks(LPCTSTR lpNicGuid,Visible_Network **lpNetworks,DWORD *pdwNum,int interrupt_flag);
extern "C" DIAGNOSEDLL_API void __stdcall FreeVisibleWlanNetworks(Visible_Network *lpNetworks);
extern "C" DIAGNOSEDLL_API BOOL __stdcall IsWzcServiceRunning();
extern "C" DIAGNOSEDLL_API int	__stdcall IsWzcServiceRunning2();
extern "C" DIAGNOSEDLL_API BOOL __stdcall StartWzcService();
extern "C" DIAGNOSEDLL_API BOOL __stdcall Disconnect(LPCTSTR lpNicGuid);
extern "C" DIAGNOSEDLL_API BOOL __stdcall GetVisibleWlanNetworks2(LPCTSTR lpNicGuid,Visible_Network **lppNetworks,DWORD	*pdwNum,BOOL bRescan,int interrupt_flag);
extern "C" DIAGNOSEDLL_API BOOL __stdcall IpRenewInterface(LPCTSTR lpNicGuid);
extern "C" DIAGNOSEDLL_API int  __stdcall IsWlanAutoConfigEnabled(LPCTSTR lpNicGuid);
extern "C" DIAGNOSEDLL_API BOOL __stdcall EableWlanAutoConfig(LPCTSTR lpNicGuid);
extern "C" DIAGNOSEDLL_API BOOL __stdcall MakeWlanProfileForInterfaces(LPCTSTR lpszSsid,LPCTSTR lpszAuth,LPCTSTR lpszCipher,LPCTSTR lpszPwd);
extern "C" DIAGNOSEDLL_API BOOL __stdcall TryConnect2WlanWithProfile(LPCTSTR lpszProfile);
extern "C" DIAGNOSEDLL_API BOOL __stdcall IsSsidConnected(LPCTSTR lpszSsid);
extern "C" DIAGNOSEDLL_API int  __stdcall NicConnectedNetworkSignal(LPCTSTR lpNicGuid);
extern "C" DIAGNOSEDLL_API BOOL __stdcall GetWlanNicCurConnectedProfile(LPCTSTR lpNicGuid,LPTSTR lpszProfile,DWORD buflen);
extern "C" DIAGNOSEDLL_API BOOL __stdcall GetWlanNicCurConnectedSsid(LPCTSTR lpNicGuid,LPTSTR lpszSsid,DWORD buflen);
extern "C" DIAGNOSEDLL_API BOOL __stdcall TryConnect2WlanWithProfile2(LPCTSTR lpNicGuid,LPCTSTR lpszProfile);
extern "C" DIAGNOSEDLL_API BOOL __stdcall IsHostReachble(const char* szDestIp,unsigned long timeoutms);
extern "C" DIAGNOSEDLL_API BOOL __stdcall IsThisPhysicalNic(LPCTSTR lpNicGuid);
#endif