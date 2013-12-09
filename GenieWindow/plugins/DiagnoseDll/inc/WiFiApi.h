#ifndef WIFIAPI_H
#define WIFIAPI_H

#include <Windows.h>
#include <tchar.h>

#include "Common.h"
#include "CommonApi.h"

#ifndef DIAGNOSEDLL_API
#ifdef DIAGNOSEDLL_EXPORTS
#define DIAGNOSEDLL_API __declspec(dllexport)
#else
#define DIAGNOSEDLL_API __declspec(dllimport)
#endif
#endif


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

extern "C" DIAGNOSEDLL_API BOOL __stdcall GetWiFiInterfaces(StrList **sl);
extern "C" DIAGNOSEDLL_API int __stdcall IsWlanNicConnected(LPCTSTR lpNicGuid);
extern "C" DIAGNOSEDLL_API int __stdcall IsWlanNicSwitchedOff(LPCTSTR lpNicGuid);
extern "C" DIAGNOSEDLL_API BOOL __stdcall SwitchWlanNic(LPCTSTR lpNicGuid,BOOL bSwitchOn);
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
extern "C" DIAGNOSEDLL_API int  __stdcall IsWlanAutoConfigEnabled(LPCTSTR lpNicGuid);
extern "C" DIAGNOSEDLL_API BOOL __stdcall EableWlanAutoConfig(LPCTSTR lpNicGuid);
extern "C" DIAGNOSEDLL_API BOOL __stdcall MakeWlanProfileForInterfaces(LPCTSTR lpszSsid,LPCTSTR lpszAuth,LPCTSTR lpszCipher,LPCTSTR lpszPwd);
extern "C" DIAGNOSEDLL_API BOOL __stdcall TryConnect2WlanWithProfile(LPCTSTR lpszProfile);
extern "C" DIAGNOSEDLL_API BOOL __stdcall IsSsidConnected(LPCTSTR lpszSsid);
extern "C" DIAGNOSEDLL_API int  __stdcall NicConnectedNetworkSignal(LPCTSTR lpNicGuid);
extern "C" DIAGNOSEDLL_API BOOL __stdcall GetWlanNicCurConnectedProfile(LPCTSTR lpNicGuid,LPTSTR lpszProfile,DWORD buflen);
extern "C" DIAGNOSEDLL_API BOOL __stdcall GetWlanNicCurConnectedSsid(LPCTSTR lpNicGuid,LPTSTR lpszSsid,DWORD buflen);
extern "C" DIAGNOSEDLL_API BOOL __stdcall TryConnect2WlanWithProfile2(LPCTSTR lpNicGuid,LPCTSTR lpszProfile);


/************************************************************************/
/* wireless profile apis                                            */
/************************************************************************/

extern "C" DIAGNOSEDLL_API BOOL __stdcall GetWlanProfileList(LPCTSTR lpNicGuid, StrList **sl);
extern "C" DIAGNOSEDLL_API BOOL __stdcall DeleteWlanProfile(LPCTSTR lpNicGuid, LPCTSTR lpProfileName);
extern "C" DIAGNOSEDLL_API BOOL __stdcall SetWlanProfilePosition(LPCTSTR lpNicGuid, LPCTSTR lpProfileName, unsigned long pos);
extern "C" DIAGNOSEDLL_API BOOL __stdcall SetWlanProfileList(LPCTSTR lpNicGuid, StrList *sl);

extern "C" DIAGNOSEDLL_API BOOL __stdcall SetInterfaceProfile(LPCTSTR lpNicGuid,LPCTSTR lpszSsid,LPCTSTR lpszAuth,LPCTSTR lpszCipher,LPCTSTR lpszPwd);
extern "C" DIAGNOSEDLL_API BOOL __stdcall SetWlanInterfaceProfile(LPCTSTR lpNicGuid,LPCTSTR lpszSsid,LPCTSTR lpszAuth,LPCTSTR lpszCipher,LPCTSTR lpszPwd,BOOL bNoneBroadCast);
extern "C" DIAGNOSEDLL_API BOOL __stdcall GetWlanInterfaceProfile(LPCTSTR lpNicGuid,LPCTSTR lpProfileName,StrList **sl);

#endif