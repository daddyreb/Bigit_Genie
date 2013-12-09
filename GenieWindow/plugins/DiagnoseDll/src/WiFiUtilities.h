#ifndef WIFIUTILITIES_H
#define WIFIUTILITIES_H

#include <Windows.h>
#include <wlanapi.h>
#include <list>
#include "Common.h"

LPCTSTR GetAuthAlgoString(__in DOT11_AUTH_ALGORITHM dot11AuthAlgo);
LPCTSTR GetCipherAlgoString(__in DOT11_CIPHER_ALGORITHM dot11CipherAlgo);
std::wstring String2Wstring( const std::string &strConvert );
LPWSTR SsidToStringW( __out_ecount(count) LPWSTR buf, __in ULONG   count, __in PDOT11_SSID pSsid);
BOOL StringToSsid(LPCTSTR lpSsid,PDOT11_SSID pdot11_ssid);
BOOL StringToDot11Bsstype(LPCTSTR lpBssType,PDOT11_BSS_TYPE pdot11_bss_type);
LPCTSTR Dot11BsstypeToString(DOT11_BSS_TYPE dot11_bss_type);
BOOL GetSortedVisibleNetworkList(LPCTSTR lpNicGuid,PWLAN_AVAILABLE_NETWORK_LIST* lppVList,int** pSortedIndex,DWORD& dwNetworkNum);
BOOL FreeVisibleNetworkList(PWLAN_AVAILABLE_NETWORK_LIST pVList,int* pSortedIndex);
int GetChannelVistaAndNew(LPCTSTR lpNicGuid,const  PDOT11_SSID pDot11Ssid,DOT11_BSS_TYPE dot11BssType,BOOL bSecurityEnabled);
int GetWlanChannelXP(LPCTSTR lpNicGuid,LPCTSTR lpSsid);

BOOL Rescan_Wlan(LPCTSTR lpNicGuid,int interrupt_flag);
int is_service_running(const wchar_t *service_name);
bool start_service(const wchar_t *service_name);


BOOL GetWlanInterfaces(std::list<TString> &infList);

DWORD SetProfile(int argc, const TCHAR *argv[]);

#endif
