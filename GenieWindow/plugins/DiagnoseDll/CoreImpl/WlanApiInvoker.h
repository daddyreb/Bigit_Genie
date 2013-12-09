#pragma once

#include <Windows.h>
#include <wlanapi.h>

//////////////////////////////////////////////////////////////////////////
//-------wifi native api prototype definition--------------------

//for WlanOpenHandle
typedef DWORD (WINAPI *PWlanOpenHandle)(
										__in        DWORD,
										__reserved  PVOID,
										__out       PDWORD,
										__out       PHANDLE
										);

//for WlanCloseHandle
typedef DWORD (WINAPI *PWlanCloseHandle)(
	__in        HANDLE,
	__reserved  PVOID
	);
//for WlanEnumInterfaces
typedef DWORD (WINAPI *PWlanEnumInterfaces)(
	__in        HANDLE,
	__reserved  PVOID,
	__out       PWLAN_INTERFACE_INFO_LIST*
	);


//for WlanQueryInterface
typedef DWORD (WINAPI *PWlanQueryInterface)(
	__in        HANDLE,
	__in        const GUID*,
	__in        WLAN_INTF_OPCODE ,
	__reserved  PVOID,
	__out       PDWORD ,
	__out       PVOID* ,
	__out_opt   PWLAN_OPCODE_VALUE_TYPE
	);

//for WlanSetInterface
typedef DWORD (WINAPI *PWlanSetInterface)(
	__in        HANDLE,
	__in        const GUID*,
	__in        WLAN_INTF_OPCODE,
	__in        DWORD,
	__in        const PVOID,
	__reserved  PVOID
	);

typedef VOID (WINAPI *PWlanFreeMemory)(
								   __in  PVOID
								   );

typedef DWORD (WINAPI *PWlanGetInterfaceCapability)(
	__in        HANDLE,
	__in        const GUID*,
	__reserved  PVOID,
	__out       PWLAN_INTERFACE_CAPABILITY*
	);

typedef DWORD (WINAPI *PWlanSetInterface)(
									  __in        HANDLE,
									  __in        const GUID*,
									  __in        WLAN_INTF_OPCODE,
									  __in        DWORD ,
									  __in        const PVOID ,
									  __reserved  PVOID
									  );

typedef DWORD (WINAPI *PWlanConnect)(
								 __in        HANDLE,
								 __in        const GUID*,
								 __in        const PWLAN_CONNECTION_PARAMETERS,
								 __reserved  PVOID
								 );

typedef DWORD (WINAPI *PWlanDisconnect)(
									__in          HANDLE,
									__in          const GUID*,
									PVOID
									);


typedef DWORD (WINAPI *PWlanGetProfileList)(
										__in        HANDLE,
										__in        const GUID*,
										__reserved  PVOID ,
										__out       PWLAN_PROFILE_INFO_LIST*
										);


typedef DWORD (WINAPI *PWlanGetAvailableNetworkList)(
	__in        HANDLE,
	__in        const GUID*,
	__in        DWORD ,
	__reserved  PVOID ,
	__out       PWLAN_AVAILABLE_NETWORK_LIST*
	);

typedef DWORD (WINAPI *PWlanUIEditProfile)(
									   __in   DWORD,
									   __in   LPCWSTR,
									   __in   GUID*,
									   __in   HWND,
									   __in   WL_DISPLAY_PAGES,
									   PVOID ,
									   __out  PWLAN_REASON_CODE*
									   );

typedef DWORD (WINAPI *PWlanSetProfile)(
									__in        HANDLE,
									__in        const GUID*,
									__in        DWORD ,
									__in        LPCWSTR,
									__in_opt    LPCWSTR,
									__in        BOOL,
									__reserved  PVOID,
									__out       DWORD*
									);


typedef DWORD (WINAPI *PWlanDeleteProfile)(
									   __in        HANDLE,
									   __in        const GUID*,
									   __in        LPCWSTR,
									   __reserved  PVOID
									   );

typedef DWORD (WINAPI *PWlanGetNetworkBssList)(
										__in          HANDLE ,
										__in          const GUID*,
										__in_opt         const  PDOT11_SSID ,
										__in          DOT11_BSS_TYPE ,
										__in          BOOL ,
										PVOID ,
										__out         PWLAN_BSS_LIST* 
										);



//------------------------end--------------------------------------
//////////////////////////////////////////////////////////////////////////


class CWlanApiInvoker 
{
	public:
		CWlanApiInvoker();
		~CWlanApiInvoker();
	protected:
		HMODULE				m_hWlanLib;
private:
	BOOL IsLibLoaded()
	{
		if (NULL==m_hWlanLib)
		{
			m_hWlanLib=LoadLibrary(TEXT("Wlanapi.dll"));
			if (NULL==m_hWlanLib)
			{
				return FALSE;
			}
		}
		return TRUE;
	}
	public:
		BOOL WlanOpenHandle(										
			__in        DWORD,
			__reserved  PVOID,
			__out       PDWORD,
			__out       PHANDLE
			);
		BOOL WlanCloseHandle(
			__in        HANDLE,
			__reserved  PVOID
			);
		BOOL WlanEnumInterfaces(
			__in        HANDLE,
			__reserved  PVOID,
			__out       PWLAN_INTERFACE_INFO_LIST*
			);
		BOOL WlanQueryInterface(
			__in        HANDLE,
			__in        const GUID*,
			__in        WLAN_INTF_OPCODE ,
			__reserved  PVOID,
			__out       PDWORD ,
			__out       PVOID* ,
			__out_opt   PWLAN_OPCODE_VALUE_TYPE
			);
		BOOL WlanSetInterface(
			__in        HANDLE,
			__in        const GUID*,
			__in        WLAN_INTF_OPCODE,
			__in        DWORD,
			__in        const PVOID,
			__reserved  PVOID
			);

		VOID WlanFreeMemory(
			__in  PVOID
			);

		BOOL WlanGetInterfaceCapability(
			__in        HANDLE,
			__in        const GUID*,
			__reserved  PVOID,
			__out       PWLAN_INTERFACE_CAPABILITY*
			);
		BOOL WlanConnect(
			__in        HANDLE,
			__in        const GUID*,
			__in        const PWLAN_CONNECTION_PARAMETERS,
			__reserved  PVOID
			);

		BOOL WlanDisconnect(
			__in          HANDLE,
			__in          const GUID*,
			PVOID
			);


		BOOL WlanGetProfileList(
			__in        HANDLE,
			__in        const GUID*,
			__reserved  PVOID,
			__out       PWLAN_PROFILE_INFO_LIST*
			);

		BOOL WlanGetAvailableNetworkList(
			__in        HANDLE,
			__in        const GUID*,
			__in        DWORD,
			__reserved  PVOID,
			__out       PWLAN_AVAILABLE_NETWORK_LIST*
			);

		BOOL WlanUIEditProfile(
			__in   DWORD,
			__in   LPCWSTR,
			__in   GUID*,
			__in   HWND,
			__in   WL_DISPLAY_PAGES,
			PVOID ,
			__out  PWLAN_REASON_CODE*
			);

		BOOL WlanSetProfile(
			__in        HANDLE,
			__in        const GUID*,
			__in        DWORD ,
			__in        LPCWSTR,
			__in_opt    LPCWSTR,
			__in        BOOL,
			__reserved  PVOID,
			__out       DWORD*
			);

		BOOL WlanDeleteProfile(
			__in        HANDLE,
			__in        const GUID*,
			__in        LPCWSTR,
			__reserved  PVOID
			);

		BOOL WlanGetNetworkBssList(
			__in          HANDLE ,
			__in          const GUID*,
			__in_opt	  const  PDOT11_SSID ,
			__in          DOT11_BSS_TYPE ,
			__in          BOOL ,
			PVOID ,
			__out         PWLAN_BSS_LIST* 
			);
};