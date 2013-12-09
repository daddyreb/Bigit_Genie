
#include "WlanApiInvoker.h"

CWlanApiInvoker::CWlanApiInvoker()
{
	m_hWlanLib= LoadLibrary(TEXT("Wlanapi.dll"));
}

CWlanApiInvoker::~CWlanApiInvoker()
{
	if (m_hWlanLib!=NULL)
	{
		FreeLibrary(m_hWlanLib);
		m_hWlanLib=NULL;
	}
}

//-----------invoker---------------------
BOOL CWlanApiInvoker::WlanOpenHandle(
									 __in        DWORD dwClientVersion,
									 __reserved  PVOID pReserved,
									 __out       PDWORD pdwNegotiatedVersion,
									 __out       PHANDLE phClientHandle
									)
{
	if (!IsLibLoaded())
	{
		return FALSE;
	}

	PWlanOpenHandle pWlanOpenHandle=(PWlanOpenHandle)GetProcAddress(m_hWlanLib,"WlanOpenHandle");
	if (NULL==pWlanOpenHandle)
	{
		return FALSE;
	}
	return pWlanOpenHandle(dwClientVersion,pReserved,pdwNegotiatedVersion,phClientHandle)==ERROR_SUCCESS;
}



BOOL CWlanApiInvoker::WlanCloseHandle(  
									  __in        HANDLE hClientHandle,
									  __reserved  PVOID pReserved
									  )
{
	if (!IsLibLoaded())
	{
		return FALSE;
	}

	PWlanCloseHandle pWlanCloseHandle=(PWlanCloseHandle)GetProcAddress(m_hWlanLib,"WlanCloseHandle");

	if (NULL==pWlanCloseHandle)
	{
		return FALSE;
	}
	return pWlanCloseHandle(hClientHandle,pReserved)==ERROR_SUCCESS;
}


BOOL CWlanApiInvoker::WlanEnumInterfaces(__in        HANDLE hClientHandle,
										 __reserved  PVOID pReserved,
										 __out       PWLAN_INTERFACE_INFO_LIST* ppInterfaceList
										 )
{
	if (!IsLibLoaded())
	{
		return FALSE;
	}

	PWlanEnumInterfaces pWlanEnumInterfaces =(PWlanEnumInterfaces)GetProcAddress(m_hWlanLib,"WlanEnumInterfaces");

	if (NULL==pWlanEnumInterfaces)
	{
		return FALSE;
	}
	return pWlanEnumInterfaces(hClientHandle,pReserved,ppInterfaceList)==ERROR_SUCCESS;
}


BOOL CWlanApiInvoker::WlanQueryInterface(__in        HANDLE hClientHandle,
										 __in        const GUID* pInterfaceGuid,
										 __in        WLAN_INTF_OPCODE OpCode,
										 __reserved  PVOID pReserved,
										 __out       PDWORD pdwDataSize,
										 __out       PVOID* ppData,
										 __out_opt   PWLAN_OPCODE_VALUE_TYPE pWlanOpcodeValueType
										 )
{
	if (!IsLibLoaded())
	{
		return FALSE;
	}

	PWlanQueryInterface pWlanQueryInterface=(PWlanQueryInterface)GetProcAddress(m_hWlanLib,"WlanQueryInterface");

	if (NULL==pWlanQueryInterface)
	{
		return FALSE;
	}
	
	return pWlanQueryInterface(hClientHandle,pInterfaceGuid,OpCode,pReserved,pdwDataSize,ppData,pWlanOpcodeValueType)==ERROR_SUCCESS;
}

VOID CWlanApiInvoker::WlanFreeMemory(
						   __in  PVOID pMemory
						   )
{
	if (!IsLibLoaded())
	{
		return;
	}

	PWlanFreeMemory pWlanFreeMemory=(PWlanFreeMemory)GetProcAddress(m_hWlanLib,"WlanFreeMemory");

	if (NULL==pWlanFreeMemory)
	{
		return;
	}

	pWlanFreeMemory(pMemory);
}

BOOL CWlanApiInvoker::WlanGetInterfaceCapability(
	__in        HANDLE hClientHandle,
	__in        const GUID* pInterfaceGuid,
	__reserved  PVOID pReserved,
	__out       PWLAN_INTERFACE_CAPABILITY* ppCapability
	)
{
	if (!IsLibLoaded())
	{
		return FALSE;
	}

	PWlanGetInterfaceCapability pWlanGetInterfaceCapability=(PWlanGetInterfaceCapability)GetProcAddress(m_hWlanLib,"WlanGetInterfaceCapability");
	
	if (NULL==pWlanGetInterfaceCapability)
	{
		return FALSE;
	}

	return pWlanGetInterfaceCapability(hClientHandle,pInterfaceGuid,pReserved,ppCapability)==ERROR_SUCCESS;
}

BOOL CWlanApiInvoker::WlanSetInterface(
	__in        HANDLE hClientHandle,
	__in        const GUID* pInterfaceGuid,
	__in        WLAN_INTF_OPCODE OpCode,
	__in        DWORD dwDataSize,
	__in        const PVOID pData,
	__reserved  PVOID pReserved
	)
{
	if (!IsLibLoaded())
	{
		return FALSE;
	}

	PWlanSetInterface pWlanSetInterface=(PWlanSetInterface)GetProcAddress(m_hWlanLib,"WlanSetInterface");

	if (NULL==pWlanSetInterface)
	{
		return FALSE;
	}

	return pWlanSetInterface(hClientHandle,pInterfaceGuid,OpCode,dwDataSize,pData,pReserved)==ERROR_SUCCESS;
}

BOOL CWlanApiInvoker::WlanConnect(
	__in        HANDLE hClientHandle,
	__in        const GUID* pInterfaceGuid,
	__in        const PWLAN_CONNECTION_PARAMETERS pConnectionParameters,
	__reserved  PVOID pReserved
	)
{
	if (!IsLibLoaded())
	{
		return FALSE;
	}

	PWlanConnect pWlanConnect=(PWlanConnect)GetProcAddress(m_hWlanLib,"WlanConnect");

	if (NULL==pWlanConnect)
	{
		return FALSE;
	}
	
	return pWlanConnect(hClientHandle,pInterfaceGuid,pConnectionParameters,pReserved)==ERROR_SUCCESS;
}

BOOL CWlanApiInvoker::WlanDisconnect(
							__in          HANDLE hClientHandle,
							__in          const GUID* pInterfaceGuid,
										 PVOID pReserved
							)
{
	if (!IsLibLoaded())
	{
		return FALSE;
	}

	PWlanDisconnect pWlanDisconnect=(PWlanDisconnect)GetProcAddress(m_hWlanLib,"WlanDisconnect");

	if (NULL==pWlanDisconnect)
	{
		return FALSE;
	}

	return pWlanDisconnect(hClientHandle,pInterfaceGuid,pReserved)==ERROR_SUCCESS;
}


BOOL CWlanApiInvoker::WlanGetProfileList(
	__in        HANDLE hClientHandle,
	__in        const GUID* pInterfaceGuid,
	__reserved  PVOID pReserved,
	__out       PWLAN_PROFILE_INFO_LIST* ppProfileList
	)
{
	if (!IsLibLoaded())
	{
		return FALSE;
	}

	PWlanGetProfileList pWlanGetProfileList=(PWlanGetProfileList)GetProcAddress(m_hWlanLib,"WlanGetProfileList");

	if (NULL==pWlanGetProfileList)
	{
		return FALSE;
	}

	return pWlanGetProfileList(hClientHandle,pInterfaceGuid,pReserved,ppProfileList)==ERROR_SUCCESS;
}

BOOL CWlanApiInvoker::WlanGetAvailableNetworkList(
	__in        HANDLE hClientHandle,
	__in        const GUID* pInterfaceGuid,
	__in        DWORD dwFlags,
	__reserved  PVOID pReserved,
	__out       PWLAN_AVAILABLE_NETWORK_LIST* ppAvailableNetworkList
	)
{
	if (!IsLibLoaded())
	{
		return FALSE;
	}

	PWlanGetAvailableNetworkList pWlanGetAvailableNetworkList=(PWlanGetAvailableNetworkList)GetProcAddress(m_hWlanLib,"WlanGetAvailableNetworkList");

	if (NULL==pWlanGetAvailableNetworkList)
	{
		return FALSE;
	}

	return pWlanGetAvailableNetworkList(hClientHandle,pInterfaceGuid,dwFlags,pReserved,ppAvailableNetworkList)==ERROR_SUCCESS;
}

BOOL CWlanApiInvoker::WlanUIEditProfile(
										__in   DWORD dwClientVersion,
										__in   LPCWSTR wstrProfileName,
										__in   GUID* pInterfaceGuid,
										__in   HWND hWnd,
										__in   WL_DISPLAY_PAGES wlStartPage,
										PVOID pReserved,
										__out  PWLAN_REASON_CODE* pWlanReasonCode
										)
{
	if (!IsLibLoaded())
	{
		return FALSE;
	}

	PWlanUIEditProfile pWlanUIEditProfile=(PWlanUIEditProfile)GetProcAddress(m_hWlanLib,"WlanUIEditProfile");

	if (NULL==pWlanUIEditProfile)
	{
		return FALSE;
	}

	return pWlanUIEditProfile(dwClientVersion,wstrProfileName,pInterfaceGuid,hWnd,wlStartPage,pReserved,pWlanReasonCode)==ERROR_SUCCESS;
}

BOOL CWlanApiInvoker::WlanSetProfile(
	__in        HANDLE hClientHandle,
	__in        const GUID* pInterfaceGuid,
	__in        DWORD dwFlags,
	__in        LPCWSTR strProfileXml,
	__in_opt    LPCWSTR strAllUserProfileSecurity,
	__in        BOOL bOverwrite,
	__reserved  PVOID pReserved,
	__out       DWORD* pdwReasonCode
	)
{
	if (!IsLibLoaded())
	{
		return FALSE;
	}

	PWlanSetProfile pWlanSetProfile=(PWlanSetProfile)GetProcAddress(m_hWlanLib,"WlanSetProfile");

	if (NULL==pWlanSetProfile)
	{
		return FALSE;
	}

	return pWlanSetProfile(hClientHandle,pInterfaceGuid,dwFlags,strProfileXml,
		strAllUserProfileSecurity,bOverwrite,pReserved,pdwReasonCode)==ERROR_SUCCESS;
}

BOOL CWlanApiInvoker::WlanDeleteProfile(
							   __in        HANDLE hClientHandle,
							   __in        const GUID* pInterfaceGuid,
							   __in        LPCWSTR strProfileName,
							   __reserved  PVOID pReserved
							   )
{
	if (!IsLibLoaded())
	{
		return FALSE;
	}

	PWlanDeleteProfile pWlanDeleteProfile=(PWlanDeleteProfile)GetProcAddress(m_hWlanLib,"WlanDeleteProfile");

	if (NULL==pWlanDeleteProfile)
	{
		return FALSE;
	}

	return pWlanDeleteProfile(hClientHandle,pInterfaceGuid,strProfileName,pReserved)==ERROR_SUCCESS;
}


BOOL CWlanApiInvoker::WlanGetNetworkBssList(
	__in          HANDLE hClientHandle,
	__in          const GUID* pInterfaceGuid,
	__in_opt      const  PDOT11_SSID pDot11Ssid,
	__in          DOT11_BSS_TYPE dot11BssType,
	__in          BOOL bSecurityEnabled,
				  PVOID	pReserved,
	__out         PWLAN_BSS_LIST* ppWlanBssList
	)
{
	if (!IsLibLoaded())
	{
		return FALSE;
	}

	PWlanGetNetworkBssList pWlanGetNetworkBssList=(PWlanGetNetworkBssList)GetProcAddress(m_hWlanLib,"WlanGetNetworkBssList");

	if (NULL==pWlanGetNetworkBssList)
	{
		return FALSE;
	}

	return pWlanGetNetworkBssList(hClientHandle,pInterfaceGuid,pDot11Ssid,
		dot11BssType,bSecurityEnabled,pReserved,ppWlanBssList)==ERROR_SUCCESS;
}

//-------------------invoker end--------------------------------------
