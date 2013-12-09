#pragma once
#include <winsock2.h>
#include <tchar.h>
#include <list>
#include <setupapi.h>
#include <string>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

// See http://msdn2.microsoft.com/en-us/library/ms695279.aspx
#define _WIN32_DCOM
#define SECURITY_WIN32

// Standard Windows
#include <windows.h>
//#include <tchar.h>
//#include <list>
#include <map>
#include <algorithm>


// Security
#include <Security.h>   // GetUserNameEx
#pragma comment(lib, "Secur32")

// COM
#include <objbase.h>    // CoIntializeEx, CoUnintialize
#include <objidl.h>     // CoInitializeSecurity
#include <atlbase.h>    // CComPtr, CComBSTR

#include <comutil.h>
#pragma comment(lib, "comsuppw.lib")

// Runtime Library
#include <iostream>
#include <assert.h>
//#include <atlstr.h>     //“˝»ÎCString     

// WMI
#include <wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")

#define TString std::wstring

#ifdef UNICODE
#  define tcout wcout
#else
#  define tcout cout
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////////













#define MY_BUFFER_SIZE 1024

DEFINE_GUID(GUID_NDIS_LAN_CLASS, 0xad498944, 0x762f, 0x11d0, 0x8d, 0xcb, 0x00, 0xc0, 0x4f, 0xc3, 0x35, 0x8c);

#define KEY_NETWORK_CONNECTIONS TEXT("SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}")

#if defined _UNICODE || defined UNICODE
#define TString		std::wstring
#define RPC_TSTR	RPC_WSTR
#else
#define TString		std::string
#define RPC_TSTR	RPC_CSTR
#endif

//class CSockLib
//{
//	public:
//		CSockLib()
//		{
//			WSADATA wsaData;
//			WSAStartup(MAKEWORD(2,2),&wsaData);		
//		}
//		virtual ~CSockLib()
//		{
//			WSACleanup();
//		}
//};


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
BOOL IsPhysicalNic(LPCTSTR lpNicGuid);
DWORD GetOsVersion(DWORD* pdwMinorVersion=NULL);

std::wstring String2Wstring( const std::string &strConvert );






//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//std::wstring ANSIToUnicode(const std::string& str);
//void ReturnErrorInfo(ItemDataList **ErrorInfo, const std::wstring &sError);
//std::list<std::string> splitTolist(std::string &mString, char &mSplit);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////

