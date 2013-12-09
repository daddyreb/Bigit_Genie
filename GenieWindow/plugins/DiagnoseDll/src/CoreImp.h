#pragma once
#include <winsock2.h>
#include <tchar.h>
#include <list>
#include <setupapi.h>
#include <string>
#include "Common.h"

#define MY_BUFFER_SIZE 1024

DEFINE_GUID(GUID_NDIS_LAN_CLASS, 0xad498944, 0x762f, 0x11d0, 0x8d, 0xcb, 0x00, 0xc0, 0x4f, 0xc3, 0x35, 0x8c);

#define KEY_NETWORK_CONNECTIONS TEXT("SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}")
#define KEY_NETWORK_CLASS TEXT("SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002BE10318}")
#define KEY_NETWORK_TCPIP_ADPTERS TEXT("SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Adapters")
#define REG_GUID_STR_LEN	36



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

BOOL GetNicFriendlyName(LPCTSTR lpNicGuid,TString& strFriName);
BOOL NetCardStateChangeNetSh(LPCTSTR lpNicFriName,BOOL bEnable);
int IsGatewayValid(LPCTSTR szGatewayIp,int interrupt_flag);
BOOL RegEnableDhcp(LPCTSTR lpszAdapterName);
BOOL NotifyDhcpEnable(LPCTSTR lpszAdapterName);
BOOL Execute_Command_Privilege(LPCTSTR lpCmdLine);
