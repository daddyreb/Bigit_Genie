#pragma once
#include <winsock2.h>
#include <tchar.h>
#include <list>
#include <setupapi.h>
#include <string>

#include "WlanApiInvoker.h"

#define MY_BUFFER_SIZE 1024

DEFINE_GUID(GUID_NDIS_LAN_CLASS, 0xad498944, 0x762f, 0x11d0, 0x8d, 0xcb, 0x00, 0xc0, 0x4f, 0xc3, 0x35, 0x8c);

#define KEY_NETWORK_CONNECTIONS TEXT("SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}")
#define KEY_NETWORK_CLASS TEXT("SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002BE10318}")
#define KEY_NETWORK_TCPIP_ADPTERS TEXT("SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Adapters")
#define REG_GUID_STR_LEN	36

#if defined _UNICODE || defined UNICODE
#define TString		std::wstring
#define RPC_TSTR	RPC_WSTR
#else
#define TString		std::string
#define RPC_TSTR	RPC_CSTR
#endif

class CSockLib
{
	public:
		CSockLib()
		{
			WSADATA wsaData;
			WSAStartup(MAKEWORD(2,2),&wsaData);		
		}
		virtual ~CSockLib()
		{
			WSACleanup();
		}
};


class Garbage
{
	protected:
		HDEVINFO m_hDeviInfo;
	public:
		Garbage(HDEVINFO hDeviInfo=INVALID_HANDLE_VALUE):m_hDeviInfo(hDeviInfo){}
		virtual ~Garbage()
		{
			if (m_hDeviInfo!=INVALID_HANDLE_VALUE)
			{
				SetupDiDestroyDeviceInfoList(m_hDeviInfo);
			}
		}
	private:
		Garbage(const Garbage&);
		Garbage& operator=(const Garbage&);
};

BOOL IsClassHidden(const GUID * ClassGuid );
int GetNicMediaType(TString& strNicGuid);
BOOL GetNicGuid(LPCTSTR pszPnpInstanceId, std::list<TString> &GuidList);
BOOL GetNicGuid(LPCTSTR pszPnpInstanceId, TString &guid);
BOOL IsPhysicalNic(LPCTSTR lpNicGuid);
BOOL IsPhysicalNic2(LPCTSTR lpNicGuid);
DWORD GetOsVersion(DWORD* pdwMinorVersion=NULL);
BOOL GetNicFriendlyName(LPCTSTR lpNicGuid,TString& strFriName);
BOOL NetCardStateChangeNetSh(LPCTSTR lpNicFriName,BOOL bEnable);
int IsGatewayValid(LPCTSTR szGatewayIp,int interrupt_flag);
BOOL RegEnableDhcp(LPCTSTR lpszAdapterName);
BOOL NotifyDhcpEnable(LPCTSTR lpszAdapterName);
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
BOOL Execute_Command_Privilege(LPCTSTR lpCmdLine);
BOOL Rescan_Wlan(LPCTSTR lpNicGuid,int interrupt_flag);
int is_service_running(const wchar_t *service_name);
bool start_service(const wchar_t *service_name);
DWORD sleep_interruptable(DWORD dwMillionseconds);
