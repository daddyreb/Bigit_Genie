#include "WiFiUtilities.h"
#include "WiFiApi.h"


#include <Rpc.h>
#include <wlanapi.h>

#pragma comment (lib,"Wlanapi")
#pragma comment (lib,"Rpcrt4")

//called success should FreeStrListBuffer
extern "C" DIAGNOSEDLL_API BOOL __stdcall GetWiFiInterfaces(StrList **sl)
{
	BOOL bRet = FALSE;
	if (NULL == sl)
	{
		return bRet;
	}

	std::list<TString> infList;
	if(GetWlanInterfaces(infList))
	{
		StrList *tmpsl = *sl = new StrList;
		tmpsl->strs = NULL;
		tmpsl->dwCount = infList.size();

		if (tmpsl->dwCount != 0)
		{
			int i = 0;
			tmpsl->strs = new LPTSTR[tmpsl->dwCount];
			for (std::list<TString>::const_iterator iter = infList.begin() ; iter != infList.end() ; ++iter,++i)
			{
				tmpsl->strs[i] = new TCHAR[iter->length() + 1];
#if defined UNICODE || defined _UNICODE
				wcscpy(tmpsl->strs[i],iter->c_str());
#else
				WideCharToMultiByte(CP_ACP,0,iter->c_str(),-1,
					tmpsl->strs[i],iter->length() + 1,NULL,NULL);
#endif
			}
		}

		bRet = TRUE;
	}

	return bRet;
}

//功能：	判断指定的无线网卡是否处于已连接状态；
//参数：	lpNicGuid为无线物理网卡的GUID（该GUID不带大括号）；
//返回值：	1 已连接，0 未连接，-1判断失败；
//说明：	该接口只对已经启用的网卡有效。

extern "C" DIAGNOSEDLL_API int __stdcall IsWlanNicConnected(LPCTSTR lpNicGuid)
{
	HANDLE						hClient		= NULL;
	PWLAN_INTERFACE_INFO_LIST	pIntfList	= NULL;
	DWORD						dwVersion	=  0;
	GUID						NicGuid;
	DWORD						i			=  0;
	RPC_STATUS					status;
	int							iResult		= -1;

	if (NULL==lpNicGuid || UuidFromString((RPC_TSTR)lpNicGuid,&NicGuid)!=RPC_S_OK)
	{
		return -1;
	}

	if (WlanOpenHandle(WLAN_API_VERSION,NULL,&dwVersion,&hClient) == ERROR_SUCCESS)
	{
		if (WlanEnumInterfaces(hClient,NULL,&pIntfList) == ERROR_SUCCESS)
		{
			for (i = 0;i < pIntfList->dwNumberOfItems ; ++i)
			{
				if (UuidEqual(&NicGuid,&(pIntfList->InterfaceInfo[i].InterfaceGuid),&status))
				{
					if (pIntfList->InterfaceInfo[i].isState==wlan_interface_state_connected)
					{
						iResult=1;
					}
					else
					{
						iResult=0;
					}
					break;
				}
			}//for

		}//if
	}//if

	if (pIntfList!=NULL)
	{
		WlanFreeMemory(pIntfList);
	}

	if (hClient!=NULL)
	{
		WlanCloseHandle(hClient,NULL);
	}

	return iResult;
}

//功能：	判断指定的无线网卡是否已经关闭；
//参数：	lpNicGuid 为无线物理网卡的GUID（该GUID不带大括号）；
//返回值：	1 硬件关闭，2软件关闭，0 未关闭，-1判断失败；
//说明：	如果网卡处于禁用状态，该接口不能正确的进行判断；并且该接口只有在Windows  vista
//			及以上版本的系统才支持。

//return: -1 error,0 switched on ,1 hard switched off,2 soft switched off
extern "C" DIAGNOSEDLL_API int __stdcall IsWlanNicSwitchedOff(LPCTSTR lpNicGuid)
{
	DWORD			dwVersion	= 0;
	HANDLE			hClient		= NULL;
	BOOL			bRet		= FALSE;
	int				iResult		= -1;
	BOOL			bSoftOff	= FALSE;
	BOOL			bHardOff	= FALSE;

	if (NULL==lpNicGuid || WlanOpenHandle(WLAN_API_VERSION,NULL,&dwVersion,&hClient) != ERROR_SUCCESS)
	{
		return iResult;
	}
	GUID  InfGuid;
	PVOID pData			= NULL;
	DWORD dwDataSize	= 0;
	WLAN_RADIO_STATE wlanRadioState;

	UuidFromString((RPC_TSTR)lpNicGuid,&InfGuid);
	bRet= (WlanQueryInterface(
		hClient,
		&InfGuid,
		wlan_intf_opcode_radio_state,
		NULL,                       // reserved
		&dwDataSize,
		&pData,
		NULL                        // not interested in the type of the opcode value
		) == ERROR_SUCCESS);

	if (bRet)
	{
		if (dwDataSize == sizeof(WLAN_RADIO_STATE))
		{
			wlanRadioState = *((PWLAN_RADIO_STATE)pData);
			DWORD i=0;
			for (i = 0; i < wlanRadioState.dwNumberOfPhys; ++i)
			{
				if (wlanRadioState.PhyRadioState[i].dot11HardwareRadioState==dot11_radio_state_off)
				{
					bHardOff=TRUE;
				}
				if (wlanRadioState.PhyRadioState[i].dot11SoftwareRadioState==dot11_radio_state_off)
				{
					bSoftOff=TRUE;
				}

			}//for

			if (bHardOff)
			{
				iResult=1;//hard switched off
			}
			else if (bSoftOff)
			{
				iResult=2;//soft switched off
			}
			else
			{
				iResult=0;//switched on
			}
		}

		WlanFreeMemory(pData);
		pData = NULL;
	}//if

	WlanCloseHandle(hClient,NULL);
	return iResult;
}

//功能：	将无线网卡的软件开关打开或关闭；
//参数：	lpNicGuid 为无线物理网卡的GUID（该GUID不带大括号），bSwitchOn 传递TRUE打开软件开关，传递FALSE关闭软件开关；
//返回值：	成功打开返回TRUE，否则返回FALSE；
//说明：	该接口只对启用的无线网卡有效并且该接口只有在Windows  vista及以上版本的系统才支持。

extern "C" DIAGNOSEDLL_API BOOL __stdcall SwitchWlanNic(LPCTSTR lpNicGuid,BOOL bSwitchOn)
{
	DWORD						dwVersion			= 0;
	HANDLE						hClient				= NULL;
	BOOL						bRet				= FALSE;
	GUID						NicGuid;
	PWLAN_INTERFACE_CAPABILITY	pInterfaceCapability= NULL;
	WLAN_PHY_RADIO_STATE		RadioState;
	DWORD						i					= 0;

	RadioState.dot11SoftwareRadioState = bSwitchOn?dot11_radio_state_on:dot11_radio_state_off;
	//RadioState.dot11HardwareRadioState = bSwitchOn?dot11_radio_state_on:dot11_radio_state_off;

	if (NULL==lpNicGuid || UuidFromString((RPC_TSTR)lpNicGuid,&NicGuid)!=RPC_S_OK)
	{
		return FALSE;
	}

	if (WlanOpenHandle(WLAN_API_VERSION,NULL,&dwVersion,&hClient) == ERROR_SUCCESS)
	{
		if (WlanGetInterfaceCapability(hClient,&NicGuid,NULL,&pInterfaceCapability) == ERROR_SUCCESS)
		{
			for (i=0;i<pInterfaceCapability->dwNumberOfSupportedPhys;++i)
			{
				RadioState.dwPhyIndex=i;
				if (WlanSetInterface(hClient,&NicGuid,wlan_intf_opcode_radio_state,sizeof(WLAN_PHY_RADIO_STATE),(PVOID)&RadioState,NULL) != ERROR_SUCCESS)
				{
					break;
				}
			}//for
			if (i>=pInterfaceCapability->dwNumberOfSupportedPhys)
			{
				bRet=TRUE;
			}
		}

	}//if
	if (pInterfaceCapability != NULL)
	{
		WlanFreeMemory(pInterfaceCapability);
	}
	if (hClient != NULL)
	{
		WlanCloseHandle(hClient,NULL);
	}
	return bRet;
}
//功能：	使用该无线网卡当前存在的profile执行连接操作
//参数：	lpNicGuid指定要进行重连的无线网卡
//返回值：	重连成功无线网卡状态为已经连接则返回TRUE，否则返回FASLE

extern "C" DIAGNOSEDLL_API BOOL __stdcall ReConnectWlan(LPCTSTR lpNicGuid,int interrupt_flag)
{
	HANDLE						 hClient		= NULL;
	GUID						 NicGuid;
	WLAN_CONNECTION_PARAMETERS	 wlanConnPara;
	DWORD						 dwVersion		= 0;
	DWORD						 i				= 0;
	BOOL						 bRet			= FALSE;
	PWLAN_AVAILABLE_NETWORK_LIST pVList			= NULL;
	int*						 pIndex			= NULL;
	DWORD						 dwNetworkNum	= 0;

	wlanConnPara.wlanConnectionMode = wlan_connection_mode_profile;
	wlanConnPara.dot11BssType		= dot11_BSS_type_infrastructure;
	wlanConnPara.pDot11Ssid			= NULL;
	wlanConnPara.pDesiredBssidList	= NULL;
	wlanConnPara.dwFlags			= 0;
	wlanConnPara.strProfile			= NULL;


	if (NULL==lpNicGuid || 0==_tcslen(lpNicGuid) ||
		UuidFromString((RPC_TSTR)lpNicGuid,&NicGuid)!=RPC_S_OK || (!Rescan_Wlan(lpNicGuid,interrupt_flag)))
	{
		return FALSE;
	}

	if(GetSortedVisibleNetworkList(lpNicGuid,&pVList,&pIndex,dwNetworkNum))
	{
		WCHAR wchSsid[DOT11_SSID_MAX_LENGTH+1];

		//-----尝试连接存在配置文件的可用网络(按信号强度，集中式网络优先)-----

		if (WlanOpenHandle(WLAN_API_VERSION,NULL,&dwVersion,&hClient) != ERROR_SUCCESS)
		{
			FreeVisibleNetworkList(pVList,pIndex);
			return FALSE;
		}

		for (i=0;i<dwNetworkNum;++i)
		{
			if (pVList->Network[pIndex[i]].dwFlags&WLAN_AVAILABLE_NETWORK_HAS_PROFILE)
			{
				wlanConnPara.strProfile=pVList->Network[pIndex[i]].strProfileName;
				wlanConnPara.dot11BssType=pVList->Network[pIndex[i]].dot11BssType;

				SsidToStringW(wchSsid, sizeof(wchSsid)/sizeof(WCHAR), &(pVList->Network[pIndex[i]].dot11Ssid));

				if (WlanConnect(hClient,&NicGuid,&wlanConnPara,NULL) == ERROR_SUCCESS)
				{
					int times = 60;
					while (times--)
					{
						if (IsWlanNicConnected(lpNicGuid)==1)
						{
							bRet=TRUE;
							break;
						}

						if (interrupt_flag)
						{
							if (sleep_interruptable(1000))
							{
								break;
							}
						}
						else
						{
							Sleep(1000);
						}

					}

				}//if
			}
		}//for
		//------------------------end--------------------------------------------------
		if (hClient != NULL)
		{
			WlanCloseHandle(hClient,NULL);
		}

		FreeVisibleNetworkList(pVList,pIndex);
	}//if

	return bRet;
}

extern "C" DIAGNOSEDLL_API BOOL __stdcall UiEditProfile(LPCTSTR lpNicGuid,LPCTSTR lpProfileName)
{
	/*HANDLE						 hClient		= NULL;
	GUID						 NicGuid;
	DWORD						 dwVersion		= 0;
	BOOL						 bRet			= FALSE;

	if (	NULL==lpNicGuid||0==_tcslen(lpNicGuid)
		||	UuidFromString((RPC_TSTR)lpNicGuid,&NicGuid)!=RPC_S_OK
		||	NULL==lpProfileName)
	{
		return bRet;
	}

	if (WlanOpenHandle(WLAN_API_VERSION,NULL,&dwVersion,&hClient) != ERROR_SUCCESS)
	{
		return bRet;
	}

	WLAN_REASON_CODE ReasonCode = 0;
	WCHAR chProfileName[512] = {0};
#if defined UNICODE || defined _UNICODE
	wcscpy_s(chProfileName,512,lpProfileName);
#else
	MultiByteToWideChar(CP_ACP,0,lpProfileName,-1,chProfileName,512);
#endif

	if (WlanUIEditProfile(WLAN_UI_API_VERSION,chProfileName,&NicGuid,NULL,WLConnectionPage,NULL,&ReasonCode) == ERROR_SUCCESS)
	{
		bRet=TRUE;
	}

	if (hClient!=NULL)
	{
		WlanCloseHandle(hClient,NULL);
	}

	return bRet;*/

	return FALSE;
}

//功能：建立配置文件连接到指定的无线网络
//参数：lpNicGuid指定要进行连接的无线网卡
//		lpSsid要连接的无线网络的Ssid
//		lpPwd连接到网络所使用的密钥
//返回值：成功连接到网络,无线网卡状态为已连接则返回TRUE，否则返回FALSE
extern "C" DIAGNOSEDLL_API BOOL __stdcall ConnectToWlan(LPCTSTR lpNicGuid,LPCTSTR lpSsid,LPCTSTR lpPwd,int interrupt_flag)
{
	HANDLE							hClient		= NULL;
	GUID							NicGuid;
	WLAN_CONNECTION_PARAMETERS		wlanConnPara;
	DWORD							dwVersion		= 0;
	DWORD							i				= 0;
	BOOL							bRet			= FALSE;
	PWLAN_AVAILABLE_NETWORK_LIST	pVList			= NULL;
	WCHAR							strSsid[DOT11_SSID_MAX_LENGTH+1]={0};
	CHAR							chSsid[DOT11_SSID_MAX_LENGTH+1]={0};

	wlanConnPara.wlanConnectionMode = wlan_connection_mode_profile;
	wlanConnPara.dot11BssType		= dot11_BSS_type_infrastructure;
	wlanConnPara.pDot11Ssid			= NULL;
	wlanConnPara.pDesiredBssidList	= NULL;
	wlanConnPara.dwFlags			= 0;
	wlanConnPara.strProfile			= NULL;


	if (NULL==lpNicGuid||0==_tcslen(lpNicGuid)||
		UuidFromString((RPC_TSTR)lpNicGuid,&NicGuid)!=RPC_S_OK
		||NULL==lpPwd)
	{
		return bRet;
	}

	if (WlanOpenHandle(WLAN_API_VERSION,NULL,&dwVersion,&hClient) != ERROR_SUCCESS)
	{
		return bRet;
	}

	if (WlanGetAvailableNetworkList(hClient,&NicGuid,0,NULL,&pVList) != ERROR_SUCCESS)
	{
		WlanCloseHandle(hClient,NULL);
		return bRet;
	}

	for (i=0;i<pVList->dwNumberOfItems;++i)
	{
		SsidToStringW(strSsid,sizeof(strSsid)/sizeof(WCHAR),&(pVList->Network[i].dot11Ssid));
		WideCharToMultiByte(CP_ACP,0,strSsid,-1,chSsid,DOT11_SSID_MAX_LENGTH+1,NULL,FALSE);
#if defined _UNICODE || defined UNICODE
		if (!wcscmp(strSsid,lpSsid))
		{
			break;
		}
#else
		if (!strcmp(chSsid,lpSsid))
		{
			break;
		}
#endif
	}

	BOOL bProfileOk=FALSE;
	if (pVList->Network	[i].dot11DefaultCipherAlgorithm!=DOT11_CIPHER_ALGO_NONE)
	{
		bProfileOk=SetInterfaceProfile(lpNicGuid,
#if defined _UNICODE || defined UNICODE
			strSsid
#else
			chSsid
#endif
			,GetAuthAlgoString(pVList->Network[i].dot11DefaultAuthAlgorithm),
			GetCipherAlgoString(pVList->Network[i].dot11DefaultCipherAlgorithm),lpPwd);
	}
	else
	{
		bProfileOk=SetInterfaceProfile(lpNicGuid,
#if defined _UNICODE || defined UNICODE
			strSsid
#else
			chSsid
#endif
			,GetAuthAlgoString(pVList->Network[i].dot11DefaultAuthAlgorithm),
			GetCipherAlgoString(pVList->Network[i].dot11DefaultCipherAlgorithm),TEXT(""));
	}

	if (bProfileOk)
	{
		wlanConnPara.strProfile=strSsid;
		wlanConnPara.dot11BssType=pVList->Network[i].dot11BssType;

		if (WlanConnect(hClient,&NicGuid,&wlanConnPara,NULL) == ERROR_SUCCESS)
		{
			int times = 60;
			while (times--)
			{
				if (IsWlanNicConnected(lpNicGuid)==1)
				{
					bRet=TRUE;
					break;
				}

				if (interrupt_flag)
				{
					if (sleep_interruptable(1000))
					{
						break;
					}
				}
				else
				{
					Sleep(1000);
				}

			}
		}//if
	}

	WlanFreeMemory(pVList);
	pVList=NULL;

	WlanCloseHandle(hClient,NULL);

	return bRet;
}
//功能：修改密钥,重新连接到当前网络
//参数：lpNicGuid指定要进行连接的无线网卡
//返回值：成功连接到当前网络,无线网卡状态为已连接则返回TRUE，否则返回FALSE

extern "C" DIAGNOSEDLL_API BOOL __stdcall ReConnectCurWlan(LPCTSTR lpNicGuid,LPCTSTR lpNewPwd,int interrupt_flag)
{
	HANDLE						 hClient		= NULL;
	GUID						 NicGuid;
	WLAN_CONNECTION_PARAMETERS	 wlanConnPara;
	DWORD						 dwVersion		= 0;
	DWORD						 i				= 0;
	BOOL						 bRet			= FALSE;
	PWLAN_AVAILABLE_NETWORK_LIST pVList			= NULL;
	WCHAR							strSsid[DOT11_SSID_MAX_LENGTH+1]={0};
	CHAR							chSsid[DOT11_SSID_MAX_LENGTH+1]={0};

	wlanConnPara.wlanConnectionMode = wlan_connection_mode_profile;
	wlanConnPara.dot11BssType		= dot11_BSS_type_infrastructure;
	wlanConnPara.pDot11Ssid			= NULL;
	wlanConnPara.pDesiredBssidList	= NULL;
	wlanConnPara.dwFlags			= 0;
	wlanConnPara.strProfile			= NULL;


	if (NULL==lpNicGuid || 0==_tcslen(lpNicGuid) ||
		UuidFromString((RPC_TSTR)lpNicGuid,&NicGuid)!=RPC_S_OK
		|| NULL==lpNewPwd || (!Rescan_Wlan(lpNicGuid,interrupt_flag)))
	{
		return FALSE;
	}

	if (WlanOpenHandle(WLAN_API_VERSION,NULL,&dwVersion,&hClient) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	if (WlanGetAvailableNetworkList(hClient,&NicGuid,0,NULL,&pVList) != ERROR_SUCCESS)
	{
		WlanCloseHandle(hClient,NULL);
		return FALSE;
	}

	for (i=0;i<pVList->dwNumberOfItems;++i)
	{
		if (pVList->Network[i].dwFlags&WLAN_AVAILABLE_NETWORK_CONNECTED)
		{
			break;
		}
	}

	SsidToStringW(strSsid,sizeof(strSsid)/sizeof(WCHAR),&(pVList->Network[i].dot11Ssid));
#if !defined _UNICODE && !defined UNICODE
	WideCharToMultiByte(CP_ACP,0,strSsid,-1,chSsid,DOT11_SSID_MAX_LENGTH+1,NULL,FALSE);
#endif
	BOOL bProfileOk=FALSE;
	if (pVList->Network	[i].dot11DefaultCipherAlgorithm!=DOT11_CIPHER_ALGO_NONE)
	{
		bProfileOk=SetInterfaceProfile(lpNicGuid,
#if defined _UNICODE || defined UNICODE
			strSsid
#else
			chSsid
#endif
			,GetAuthAlgoString(pVList->Network[i].dot11DefaultAuthAlgorithm),
			GetCipherAlgoString(pVList->Network[i].dot11DefaultCipherAlgorithm),lpNewPwd);
	}
	else
	{
		bProfileOk=SetInterfaceProfile(lpNicGuid,
#if defined _UNICODE || defined UNICODE
			strSsid
#else
			chSsid
#endif
			,GetAuthAlgoString(pVList->Network[i].dot11DefaultAuthAlgorithm),
			GetCipherAlgoString(pVList->Network[i].dot11DefaultCipherAlgorithm),TEXT(""));
	}

	if (bProfileOk)
	{
		wlanConnPara.strProfile=strSsid;
		wlanConnPara.dot11BssType=pVList->Network[i].dot11BssType;

		if (WlanConnect(hClient,&NicGuid,&wlanConnPara,NULL) == ERROR_SUCCESS)
		{
			int times = 60;
			while (times--)
			{
				if (IsWlanNicConnected(lpNicGuid)==1)
				{
					bRet=TRUE;
					break;
				}

				if (interrupt_flag)
				{
					if (sleep_interruptable(1000))
					{
						break;
					}
				}
				else
				{
					Sleep(1000);
				}			
			}
		}//if
	}

	WlanFreeMemory(pVList);
	pVList=NULL;

	WlanCloseHandle(hClient,NULL);

	return bRet;
}

//功能：获取指定网络的信道
//参数：lpNicGuid指定无线网卡
//		pDot11Ssid指定无线网络的SSID
//		dot11BssType指定网络类型
//		bSecurityEnabled指定是否启用安全
//返回值：返回信道
extern "C" DIAGNOSEDLL_API int __stdcall GetWlanChannel(LPCTSTR lpNicGuid,const  LPCTSTR lpSsid,LPCTSTR lpdot11BssType,BOOL bSecurityEnabled)
{
	int iRet=-1;
	DWORD dwOsVersion=GetOsVersion(NULL);
	if (dwOsVersion>=6)
	{
		if (NULL==lpNicGuid || NULL==lpSsid || NULL==lpdot11BssType)
		{
			return iRet;
		}

		DOT11_SSID Ssid;
		DOT11_BSS_TYPE Bss_type;
		if (StringToSsid(lpSsid,&Ssid) && StringToDot11Bsstype(lpdot11BssType,&Bss_type))
		{
			return GetChannelVistaAndNew(lpNicGuid,&Ssid,Bss_type,bSecurityEnabled);
		}
	}
	else
	{
		if (NULL==lpNicGuid || NULL==lpSsid)
		{
			return iRet;
		}

		return GetWlanChannelXP(lpNicGuid,lpSsid);
	}

	return iRet;
}

//功能：	获取当前可见的网络列表，集中式网络优先，按信号由强到弱排列，
//参数：	lppNicGuid指定要获取可见网络列表的无线网卡，
//			lpNetworks输出参数包含各个可见网络的信息,
//			dwNetworkNum输出参数返回可见网络的数目
//返回值：	获取成功返回TRUE，否则返回FASE。
extern "C" DIAGNOSEDLL_API BOOL __stdcall GetVisibleWlanNetworks(LPCTSTR lpNicGuid,Visible_Network **lppNetworks,DWORD	*pdwNum,int interrupt_flag)
{
	return GetVisibleWlanNetworks2(lpNicGuid,lppNetworks,pdwNum,TRUE,interrupt_flag);
}

extern "C" DIAGNOSEDLL_API void __stdcall FreeVisibleWlanNetworks(Visible_Network *lpNetworks)
{
	delete [] lpNetworks;
}

extern "C" DIAGNOSEDLL_API BOOL __stdcall IsWzcServiceRunning()
{
	DWORD			dwVersion	= 0;
	DWORD			dwError		= 0;
	HANDLE			hClient		= NULL;

	dwError=WlanOpenHandle(WLAN_API_VERSION,NULL,&dwVersion,&hClient);
	if (dwError==ERROR_SUCCESS)
	{
		WlanCloseHandle(hClient,NULL);
	}
	return dwError!=1062;//service is not running error code
}

//return -1 failed,0 not running ,1 running
extern "C" DIAGNOSEDLL_API int	__stdcall IsWzcServiceRunning2()
{
	return is_service_running(L"WZCSVC");
}

extern "C" DIAGNOSEDLL_API BOOL __stdcall StartWzcService()
{
	return start_service(L"WZCSVC")!=false;
}

extern "C" DIAGNOSEDLL_API BOOL __stdcall Disconnect(LPCTSTR lpNicGuid)
{
	DWORD			dwVersion	= 0;
	HANDLE			hClient		= NULL;
	BOOL			bRet		= FALSE;
	GUID			nic_guid;

	if (NULL==lpNicGuid || UuidFromString((RPC_TSTR)lpNicGuid,&nic_guid)!=RPC_S_OK)
	{
		return bRet;
	}

	if (WlanOpenHandle(WLAN_API_VERSION,NULL,&dwVersion,&hClient)==ERROR_SUCCESS)
	{
		if (WlanDisconnect(hClient,&nic_guid,NULL)==ERROR_SUCCESS)
		{
			bRet = TRUE;
		}
	}

	WlanCloseHandle(hClient,NULL);

	return bRet;
}



//功能：	获取当前可见的网络列表，集中式网络优先，按信号由强到弱排列，
//参数：	lppNicGuid指定要获取可见网络列表的无线网卡，
//			lpNetworks输出参数包含各个可见网络的信息,
//			dwNetworkNum输出参数返回可见网络的数目
//			bRescan 为TRUE时首先进行重新扫描操作，反之不执行重新扫描
//返回值：	获取成功返回TRUE，否则返回FASE。
extern "C" DIAGNOSEDLL_API BOOL __stdcall GetVisibleWlanNetworks2(LPCTSTR lpNicGuid,Visible_Network **lppNetworks,DWORD	*pdwNum,BOOL bRescan,int interrupt_flag)
{
	BOOL			bRet		= FALSE;
	GUID			nic_guid;
	DWORD			dwVersion	= 0;
	HANDLE			hClient		= NULL;


	*lppNetworks = NULL;
	*pdwNum		 = 0;

	if(NULL == lpNicGuid || NULL == lppNetworks ||UuidFromString((RPC_TSTR)lpNicGuid,&nic_guid) != RPC_S_OK)
	{
		return bRet;
	}

	if(bRescan && (!Rescan_Wlan(lpNicGuid,interrupt_flag)))
	{
		return bRet;
	}


	PWLAN_AVAILABLE_NETWORK_LIST	pVList;
	int								*pIndex;
	WCHAR							wTemp[256];

	if (GetSortedVisibleNetworkList(lpNicGuid,&pVList,&pIndex,*pdwNum))
	{
		if (*pdwNum!=0)
		{
			*lppNetworks=new Visible_Network[*pdwNum];
			Visible_Network *temp_VN=*lppNetworks;

			for (DWORD i=0;i<*pdwNum;++i)
			{
#if defined _UNICODE || defined UNICODE
				wcscpy(temp_VN[i].strProfileName,pVList->Network[pIndex[i]].strProfileName);
				wcscpy(temp_VN[i].strSsid,SsidToStringW(wTemp,sizeof(wTemp)/sizeof(WCHAR),&(pVList->Network[pIndex[i]].dot11Ssid)));
#else
				CHAR cTemp[256];
				WideCharToMultiByte(CP_ACP,0,pVList->Network[pIndex[i]].strProfileName,-1,cTemp,256,NULL,NULL);
				strcpy(temp_VN[i].strProfileName,cTemp);
				SsidToStringW(wTemp,sizeof(wTemp)/sizeof(WCHAR),&(pVList->Network[pIndex[i]].dot11Ssid));
				WideCharToMultiByte(CP_ACP,0,wTemp,-1,cTemp,256,NULL,NULL);
				strcpy(temp_VN[i].strSsid,cTemp);
#endif
				_tcscpy(temp_VN[i].strBssType,Dot11BsstypeToString(pVList->Network[pIndex[i]].dot11BssType));

				_tcscpy(temp_VN[i].strdot11DefaultAuthAlgorithm,
					GetAuthAlgoString(pVList->Network[pIndex[i]].dot11DefaultAuthAlgorithm));
				_tcscpy(temp_VN[i].strdot11DefaultCipherAlgorithm,
					GetCipherAlgoString(pVList->Network[pIndex[i]].dot11DefaultCipherAlgorithm));
				temp_VN[i].wlanSignalQuality=pVList->Network[pIndex[i]].wlanSignalQuality;
				temp_VN[i].bhasProfile=pVList->Network[pIndex[i]].dwFlags&WLAN_AVAILABLE_NETWORK_HAS_PROFILE;
				temp_VN[i].bcurConnected=pVList->Network[pIndex[i]].dwFlags&WLAN_AVAILABLE_NETWORK_CONNECTED;
				temp_VN[i].bNetworkConnectable=pVList->Network[pIndex[i]].bNetworkConnectable;
				temp_VN[i].bSecurityEnabled=pVList->Network[pIndex[i]].bSecurityEnabled;
			}//for

			bRet=TRUE;
		}
		FreeVisibleNetworkList(pVList,pIndex);
	}

	return bRet;
}

extern "C" DIAGNOSEDLL_API int  __stdcall IsWlanAutoConfigEnabled(LPCTSTR lpNicGuid)
{
	DWORD			dwVersion	= 0;
	DWORD			dwDataSize  = 0;
	PVOID			pData		= NULL;
	HANDLE			hClient		= NULL;
	int				iRet		= -1;
	GUID			nic_guid;

	if (NULL==lpNicGuid || UuidFromString((RPC_TSTR)lpNicGuid,&nic_guid)!=RPC_S_OK)
	{
		return iRet;
	}

	if (WlanOpenHandle(WLAN_API_VERSION,NULL,&dwVersion,&hClient)==ERROR_SUCCESS)
	{
		if ((WlanQueryInterface(
			hClient,
			&nic_guid,
			wlan_intf_opcode_autoconf_enabled,
			NULL,           
			&dwDataSize,
			&pData,
			NULL                        
			)==ERROR_SUCCESS) && (dwDataSize == sizeof(BOOL)))
		{
			iRet = (*((BOOL*)pData)?1:0);
		}
		if(pData)
		{
			WlanFreeMemory(pData);
			pData = NULL;
		}
	}

	WlanCloseHandle(hClient,NULL);

	return iRet;
}

extern "C" DIAGNOSEDLL_API BOOL __stdcall EableWlanAutoConfig(LPCTSTR lpNicGuid)
{
	DWORD			dwVersion		= 0;
	HANDLE			hClient			= NULL;
	BOOL			bRet			= FALSE;
	GUID			nic_guid;
	BOOL			enable_autoconf = TRUE;
	DWORD			dwDataSize		= sizeof(enable_autoconf);

	if (NULL==lpNicGuid || UuidFromString((RPC_TSTR)lpNicGuid,&nic_guid)!=RPC_S_OK)
	{
		return bRet;
	}

	if (WlanOpenHandle(WLAN_API_VERSION,NULL,&dwVersion,&hClient)==ERROR_SUCCESS)
	{
		if (WlanSetInterface(
			hClient,
			&nic_guid,
			wlan_intf_opcode_autoconf_enabled,
			dwDataSize,(const PVOID)&enable_autoconf,NULL)==ERROR_SUCCESS)
		{
			bRet = TRUE;
		}
	}

	WlanCloseHandle(hClient,NULL);

	return bRet;
}

extern "C" DIAGNOSEDLL_API BOOL __stdcall MakeWlanProfileForInterfaces(LPCTSTR lpszSsid,LPCTSTR lpszAuth,LPCTSTR lpszCipher,LPCTSTR lpszPwd)
{
	BOOL bRet = TRUE;

	if(NULL==lpszSsid || NULL==lpszAuth || 
		NULL==lpszCipher || NULL==lpszPwd)
	{
		return FALSE;
	}

	/*NICLIST	*nic_list = NULL;

	if(!EnumInterfaces(&nic_list))
	{
		return FALSE;
	}

	for(DWORD i=0;i<nic_list->dwCount;++i)
	{
		if(2==nic_list->lpNics[i].m_dwNicType)
		{
			if(!SetInterfaceProfile(nic_list->lpNics[i].m_chDevGuid,lpszSsid,lpszAuth,lpszCipher,lpszPwd))
			{
				bRet = FALSE;
			}
		}
	}

	FreeNicListBuffer(nic_list);*/

	std::list<TString> infList;
	if(GetWlanInterfaces(infList))
	{
		for (std::list<TString>::const_iterator i = infList.begin() ; i != infList.end() ; ++i)
		{
			if(!SetInterfaceProfile(i->c_str(),lpszSsid,lpszAuth,lpszCipher,lpszPwd))
			{
				bRet = FALSE;
			}
		}
	}

	return bRet;
}

extern "C" DIAGNOSEDLL_API BOOL __stdcall TryConnect2WlanWithProfile(LPCTSTR lpszProfile)
{
	HANDLE						 hClient		= NULL;
	GUID						 NicGuid;
	WLAN_CONNECTION_PARAMETERS	 wlanConnPara;
	DWORD						 dwVersion		= 0;
	BOOL						 bRet			= FALSE;

	if (NULL == lpszProfile || _tcslen(lpszProfile) == 0)
	{
		return bRet;
	}

	wlanConnPara.wlanConnectionMode = wlan_connection_mode_profile;
	wlanConnPara.dot11BssType		= dot11_BSS_type_infrastructure;
#if defined _UNICODE||defined UNICODE
	wlanConnPara.strProfile			= lpszProfile;
#else
	WCHAR wchSsid[DOT11_SSID_MAX_LENGTH+1];
	MultiByteToWideChar(CP_ACP,0,lpszProfile,-1,wchSsid,DOT11_SSID_MAX_LENGTH+1);	
	wlanConnPara.strProfile			= wchSsid;
#endif
	wlanConnPara.pDot11Ssid			= NULL;
	wlanConnPara.pDesiredBssidList	= NULL;
	wlanConnPara.dwFlags			= 0;


	std::list<TString> infList;
	if (!GetWlanInterfaces(infList))
	{
		return bRet;
	}

	if (WlanOpenHandle(WLAN_API_VERSION,NULL,&dwVersion,&hClient) != ERROR_SUCCESS)
	{
		return bRet;
	}

	for(std::list<TString>::const_iterator i = infList.begin() ; i != infList.end() ; ++i)
	{
		if(UuidFromString((RPC_TSTR)(i->c_str()),&NicGuid) == RPC_S_OK)
		{
			if(WlanConnect(hClient,&NicGuid,&wlanConnPara,NULL) == ERROR_SUCCESS)
			{
				bRet = TRUE;
			}
		}
	}


	if (hClient!=NULL)
	{
		WlanCloseHandle(hClient,NULL);
	}

	return bRet;
}

extern "C" DIAGNOSEDLL_API BOOL __stdcall IsSsidConnected(LPCTSTR lpszSsid)
{
	HANDLE						 hClient		= NULL;
	GUID						 NicGuid;
	PVOID					     pData			= NULL;
	DWORD						 dwDataSize     = 0;
	DWORD						 dwVersion		= 0;
	BOOL						 bRet			= FALSE;

	if (NULL == lpszSsid || 0 == _tcslen(lpszSsid))
	{
		return bRet;
	}

	std::list<TString> infList;
	if (!GetWlanInterfaces(infList))
	{
		return bRet;
	}

	if (WlanOpenHandle(WLAN_API_VERSION,NULL,&dwVersion,&hClient) != ERROR_SUCCESS)
	{
		return bRet;
	}

	for(std::list<TString>::const_iterator i = infList.begin() ; i != infList.end() ; ++i)
	{
		if(UuidFromString((RPC_TSTR)(i->c_str()),&NicGuid) == RPC_S_OK)
		{
			if(WlanQueryInterface(hClient,&NicGuid,wlan_intf_opcode_current_connection 
				,NULL,&dwDataSize,&pData,NULL) == ERROR_SUCCESS)
			{
				if(sizeof(WLAN_CONNECTION_ATTRIBUTES) == dwDataSize)
				{
					WLAN_CONNECTION_ATTRIBUTES *p = (WLAN_CONNECTION_ATTRIBUTES*)pData;
					if (p)
					{
#if defined _UNICODE || defined UNICODE
						bRet = (0 == wcscmp(lpszSsid,p->strProfileName));
#else
						WCHAR temp_s[256];
						MultiByteToWideChar(CP_ACP,0,lpszSsid,-1,temp_s,256);
						bRet = (0==wcscmp(temp_s,p->strProfileName));
#endif
						if (bRet)
						{
							WlanFreeMemory(pData);
							pData = NULL;
							break;
						}
					}
				}
			}

			if(pData)
			{
				WlanFreeMemory(pData);
				pData = NULL;
			}
		}
		
	}

	if (hClient != NULL)
	{
		WlanCloseHandle(hClient,NULL);
	}

	return bRet;

}

extern "C" DIAGNOSEDLL_API int  __stdcall NicConnectedNetworkSignal(LPCTSTR lpNicGuid)
{
	int				iret		= -1;
	Visible_Network *vn_list	= NULL;
	DWORD			dwNum		= 0;

	if (NULL == lpNicGuid)
	{
		return iret;
	}

	if (!GetVisibleWlanNetworks2(lpNicGuid,&vn_list,&dwNum,FALSE,0))
	{
		return iret;
	}

	for (DWORD i = 0;i < dwNum;++i)
	{
		if (vn_list[i].bcurConnected)
		{
			iret = (int)vn_list[i].wlanSignalQuality;
			break;
		}
	}

	FreeVisibleWlanNetworks(vn_list);
	vn_list = NULL;

	return iret;
}

extern "C" DIAGNOSEDLL_API BOOL __stdcall GetWlanNicCurConnectedSsidOrProfile(LPCTSTR lpNicGuid,LPTSTR lpszBuffer,DWORD buflen,BOOL bssid)
{
	HANDLE						 hClient		= NULL;
	GUID						 NicGuid1,NicGuid2;
	PVOID					     pData			= NULL;
	DWORD						 dwDataSize     = 0;
	DWORD						 dwVersion		= 0;
	BOOL						 bRet			= FALSE;
	RPC_STATUS					 rpcStatus;

	if (NULL == lpNicGuid || UuidFromString((RPC_TSTR)lpNicGuid,&NicGuid1) != RPC_S_OK || NULL == lpszBuffer || 0 == buflen)
	{
		return bRet;
	}

	lpszBuffer[0] = 0;

	std::list<TString> infList;
	if (!GetWlanInterfaces(infList))
	{
		return bRet;
	}

	if (WlanOpenHandle(WLAN_API_VERSION,NULL,&dwVersion,&hClient) != ERROR_SUCCESS)
	{
		return bRet;
	}

	for(std::list<TString>::const_iterator i = infList.begin() ; i != infList.end() ; ++i)
	{

		if( UuidFromString((RPC_TSTR)(i->c_str()),&NicGuid2) == RPC_S_OK &&
			UuidEqual(&NicGuid1,&NicGuid2,&rpcStatus)			   
			)
		{
			if(WlanQueryInterface(hClient,&NicGuid1,wlan_intf_opcode_current_connection 
				,NULL,&dwDataSize,&pData,NULL) == ERROR_SUCCESS)
			{
				if(sizeof(WLAN_CONNECTION_ATTRIBUTES) == dwDataSize)
				{
					WLAN_CONNECTION_ATTRIBUTES *p = (WLAN_CONNECTION_ATTRIBUTES*)pData;
					if (p)
					{
						bRet = TRUE;

						if (bssid)
						{
#if defined _UNICODE || defined UNICODE
							SsidToStringW(lpszBuffer,buflen/sizeof(TCHAR),&p->wlanAssociationAttributes.dot11Ssid);
							//wcscpy_s(lpszBuffer,buflen/sizeof(TCHAR),p->strProfileName);
#else
							WCHAR wbuffer[1024];
							SsidToStringW(wbuffer,sizeof(wbuffer)/sizeof(wbuffer[0]),&p->wlanAssociationAttributes.dot11Ssid);
							WideCharToMultiByte(CP_ACP,0,wbuffer,-1,lpszBuffer,buflen,NULL,NULL);
#endif
						}
						else
						{
#if defined _UNICODE || defined UNICODE
							wcscpy_s(lpszBuffer,buflen/sizeof(TCHAR),p->strProfileName);
#else
							WideCharToMultiByte(CP_ACP,0,p->strProfileName,-1,lpszBuffer,buflen,NULL,NULL);
#endif
						}

						if (bRet)
						{
							WlanFreeMemory(pData);
							pData = NULL;
							break;
						}
					}
				}
			}

			if(pData)
			{
				WlanFreeMemory(pData);
				pData = NULL;
			}
		}

	}

	if (hClient != NULL)
	{
		WlanCloseHandle(hClient,NULL);
	}

	return bRet;
}

extern "C" DIAGNOSEDLL_API BOOL __stdcall GetWlanNicCurConnectedProfile(LPCTSTR lpNicGuid,LPTSTR lpszProfile,DWORD buflen)
{
	return GetWlanNicCurConnectedSsidOrProfile(lpNicGuid,lpszProfile,buflen,FALSE);
}

extern "C" DIAGNOSEDLL_API BOOL __stdcall GetWlanNicCurConnectedSsid(LPCTSTR lpNicGuid,LPTSTR lpszSsid,DWORD buflen)
{
	return GetWlanNicCurConnectedSsidOrProfile(lpNicGuid,lpszSsid,buflen,TRUE);
}

extern "C" DIAGNOSEDLL_API BOOL __stdcall TryConnect2WlanWithProfile2(LPCTSTR lpNicGuid,LPCTSTR lpszProfile)
{
	HANDLE						 hClient		= NULL;
	GUID						 NicGuid1,NicGuid2;
	WLAN_CONNECTION_PARAMETERS	 wlanConnPara;
	DWORD						 dwVersion		= 0;
	BOOL						 bRet			= FALSE;
	RPC_STATUS					 rpcStatus;

	if (NULL == lpNicGuid || UuidFromString((RPC_TSTR)lpNicGuid,&NicGuid1) != RPC_S_OK || NULL == lpszProfile || _tcslen(lpszProfile) == 0)
	{
		return bRet;
	}

	wlanConnPara.wlanConnectionMode = wlan_connection_mode_profile;
	wlanConnPara.dot11BssType		= dot11_BSS_type_infrastructure;
#if defined _UNICODE||defined UNICODE
	wlanConnPara.strProfile			= lpszProfile;
#else
	WCHAR wchSsid[DOT11_SSID_MAX_LENGTH+1];
	MultiByteToWideChar(CP_ACP,0,lpszProfile,-1,wchSsid,DOT11_SSID_MAX_LENGTH+1);	
	wlanConnPara.strProfile			= wchSsid;
#endif
	wlanConnPara.pDot11Ssid			= NULL;
	wlanConnPara.pDesiredBssidList	= NULL;
	wlanConnPara.dwFlags			= 0;


	std::list<TString> infList;
	if (!GetWlanInterfaces(infList))
	{
		return bRet;
	}

	if (WlanOpenHandle(WLAN_API_VERSION,NULL,&dwVersion,&hClient) != ERROR_SUCCESS)
	{
		return bRet;
	}

	for(std::list<TString>::const_iterator i = infList.begin() ; i != infList.end() ; ++i)
	{
		if( UuidFromString((RPC_TSTR)(i->c_str()),&NicGuid2) == RPC_S_OK &&
			UuidEqual(&NicGuid1,&NicGuid2,&rpcStatus))
		{
			if(WlanConnect(hClient,&NicGuid1,&wlanConnPara,NULL) == ERROR_SUCCESS)
			{
				bRet = TRUE;
			}
			break;
		}	
	}

	if (hClient != NULL)
	{
		WlanCloseHandle(hClient,NULL);
	}

	return bRet;
}

//called success should FreeStrListBuffer
extern "C" DIAGNOSEDLL_API BOOL __stdcall GetWlanProfileList(LPCTSTR lpNicGuid, StrList **sl)
{
	BOOL	bRet		= FALSE;
	HANDLE	hClient		= NULL;
	DWORD	dwVersion	= 0;

	PWLAN_PROFILE_INFO_LIST	pprofileList = NULL;
	GUID					nicGuid;

	if ((NULL == lpNicGuid) || (UuidFromString((RPC_TSTR)lpNicGuid,&nicGuid) != RPC_S_OK) || (NULL == sl))
	{
		return bRet;
	}

	if (WlanOpenHandle(WLAN_API_VERSION,NULL,&dwVersion,&hClient) != ERROR_SUCCESS)
	{
		return bRet;
	}

	if (WlanGetProfileList(hClient,&nicGuid,NULL,&pprofileList) == ERROR_SUCCESS)
	{
		bRet = TRUE;

		StrList *tmpsl = *sl = new StrList;
		tmpsl->strs = NULL;
		tmpsl->dwCount = pprofileList->dwNumberOfItems;

		if (tmpsl->dwCount != 0)
		{
			tmpsl->strs = new LPTSTR[tmpsl->dwCount];
			for (DWORD i = 0;i < tmpsl->dwCount;++i)
			{
				tmpsl->strs[i] = new TCHAR[wcslen(pprofileList->ProfileInfo[i].strProfileName) + 1];
#if defined UNICODE || defined _UNICODE
				wcscpy(tmpsl->strs[i],pprofileList->ProfileInfo[i].strProfileName);
#else
				WideCharToMultiByte(CP_ACP,0,pprofileList->ProfileInfo[i].strProfileName,-1,
					tmpsl->strs[i],wcslen(pprofileList->ProfileInfo[i].strProfileName) + 1,NULL,NULL);
#endif
			}
		}

		WlanFreeMemory(pprofileList);
	}

	WlanCloseHandle(hClient,NULL);

	return bRet;
}

extern "C" DIAGNOSEDLL_API BOOL __stdcall DeleteWlanProfile(LPCTSTR lpNicGuid, LPCTSTR lpProfileName)
{
	BOOL	bRet		= FALSE;
	HANDLE	hClient		= NULL;
	DWORD	dwVersion	= 0;

	GUID	nicGuid;
	WCHAR	profileName[512];

	if ((NULL == lpNicGuid) || (UuidFromString((RPC_TSTR)lpNicGuid,&nicGuid) != RPC_S_OK) ||
		(NULL == lpProfileName))
	{
		return bRet;
	}

	if (WlanOpenHandle(WLAN_API_VERSION,NULL,&dwVersion,&hClient) != ERROR_SUCCESS)
	{
		return bRet;
	}

#if defined UNICODE || defined _UNICODE
	wcscpy(profileName,lpProfileName);
#else
	MultiByteToWideChar(CP_ACP,0,lpProfileName,-1,profileName,512);
#endif

	bRet = (WlanDeleteProfile(hClient,&nicGuid,profileName,NULL) == ERROR_SUCCESS);

	WlanCloseHandle(hClient,NULL);

	return bRet;
}

extern "C" DIAGNOSEDLL_API BOOL __stdcall SetWlanProfilePosition(LPCTSTR lpNicGuid, LPCTSTR lpProfileName, unsigned long pos)
{
	BOOL	bRet		= FALSE;
	HANDLE	hClient		= NULL;
	DWORD	dwVersion	= 0;

	GUID	nicGuid;
	WCHAR	profileName[512];

	if ((NULL == lpNicGuid) || (UuidFromString((RPC_TSTR)lpNicGuid,&nicGuid) != RPC_S_OK) ||
		(NULL == lpProfileName))
	{
		return bRet;
	}

	if (WlanOpenHandle(WLAN_API_VERSION,NULL,&dwVersion,&hClient) != ERROR_SUCCESS)
	{
		return bRet;
	}

#if defined UNICODE || defined _UNICODE
	wcscpy(profileName,lpProfileName);
#else
	MultiByteToWideChar(CP_ACP,0,lpProfileName,-1,profileName,512);
#endif

	bRet = (WlanSetProfilePosition(hClient,&nicGuid,profileName,pos,NULL) == ERROR_SUCCESS);

	WlanCloseHandle(hClient,NULL);

	return bRet;
}

extern "C" DIAGNOSEDLL_API BOOL __stdcall SetWlanProfileList(LPCTSTR lpNicGuid, StrList *sl)
{
	BOOL	bRet		= FALSE;
	HANDLE	hClient		= NULL;
	DWORD	dwVersion	= 0;

	GUID	nicGuid;

	if ((NULL == lpNicGuid) || (UuidFromString((RPC_TSTR)lpNicGuid,&nicGuid) != RPC_S_OK) ||
		(NULL == sl) || (sl->dwCount == 0))
	{
		return bRet;
	}

	if (WlanOpenHandle(WLAN_API_VERSION,NULL,&dwVersion,&hClient) != ERROR_SUCCESS)
	{
		return bRet;
	}

#if defined UNICODE || defined _UNICODE
	bRet = (WlanSetProfileList(hClient,&nicGuid,sl->dwCount,(LPCWSTR *)sl->strs,NULL) == ERROR_SUCCESS);
#else
	LPWSTR* chProfileNames = new LPWSTR[sl->dwCount];
	for (DWORD idx = 0 ; idx < sl->dwCount ; ++idx)
	{
		chProfileNames[idx] = new WCHAR[_tcslen(sl->strs[idx]) + 1];
		MultiByteToWideChar(CP_ACP,0,sl->strs[idx],-1,chProfileNames[idx],_tcslen(sl->strs[idx]) + 1);
	}
	bRet = (WlanSetProfileList(hClient,&nicGuid,sl->dwCount,(LPCWSTR *)chProfileNames,NULL) == ERROR_SUCCESS);

	for (DWORD i = 0 ; i < sl->dwCount ; ++i)
	{
		delete [] chProfileNames[i];
	}
	delete [] chProfileNames;

#endif



	WlanCloseHandle(hClient,NULL);

	return bRet;
}

extern "C" DIAGNOSEDLL_API BOOL __stdcall GetWlanInterfaceProfile(LPCTSTR lpNicGuid,LPCTSTR lpProfileName,StrList **sl)
{
	BOOL	bRet			= FALSE;
	HANDLE	hClient			= NULL;
	DWORD	dwVersion		= 0;
	GUID	nicGuid;
	DWORD	dwFlags			= 0;
	DWORD   dwGrantAccess	= 0;
	LPWSTR  lpwProfileName	= NULL;
	LPWSTR  lpwProfileXml	= NULL;

	if ((NULL == lpNicGuid) || 
		(UuidFromString((RPC_TSTR)lpNicGuid,&nicGuid) != RPC_S_OK) ||
		(NULL == lpProfileName) ||
		(NULL == sl))
	{
		return bRet;
	}

	*sl = NULL;

	if (WlanOpenHandle(WLAN_API_VERSION,NULL,&dwVersion,&hClient) != ERROR_SUCCESS)
	{
		return bRet;
	}

#if defined UNICODE || defined _UNICODE
	lpwProfileName = (LPWSTR)lpProfileName;
#else
	int bufferSize = MultiByteToWideChar(CP_ACP,0,lpProfileName,-1,NULL,0);
	if(bufferSize <= 0 || ((lpwProfileName = new WCHAR[bufferSize]) == NULL))
	{
		return bRet;
	}
	MultiByteToWideChar(CP_ACP,0,lpProfileName,-1,lpwProfileName,bufferSize);
#endif

	bRet = (WlanGetProfile(hClient,&nicGuid,lpwProfileName,NULL,&lpwProfileXml,&dwFlags,&dwGrantAccess) == ERROR_SUCCESS);

#if !(defined UNICODE || defined _UNICODE)
	delete [] lpwProfileName;
#endif

	WlanCloseHandle(hClient,NULL);

	if (bRet && lpwProfileXml)
	{
		*sl	= new StrList;
		StrList *lpTemp	= *sl;
		lpTemp->dwCount	= 1;	
		lpTemp->strs	= new LPTSTR[lpTemp->dwCount];
		lpTemp->strs[0] = NULL;

#if defined UNICODE || defined _UNICODE
		lpTemp->strs[0] = new WCHAR[wcslen(lpwProfileXml) + 1];
		if (lpTemp->strs[0] != NULL)
		{
			wcscpy(lpTemp->strs[0],lpwProfileXml);
		}
		else
		{
			bRet = FALSE;
		}

#else
		int size = WideCharToMultiByte(CP_ACP,0,lpwProfileXml,-1,NULL,0,NULL,NULL);
		if (size <= 0 || (lpTemp->strs[0] = new TCHAR[size]) == NULL)
		{
			bRet = FALSE;
		}
		else
		{
			bRet = (WideCharToMultiByte(CP_ACP,0,lpwProfileXml,-1,lpTemp->strs[0],size,NULL,NULL) != 0);
		}		
#endif
		if (!bRet)
		{
			*sl = NULL;
			delete [] lpTemp->strs[0];
			delete [] lpTemp->strs;
			delete lpTemp;
		}

		WlanFreeMemory(lpwProfileXml);
	}

	return bRet;
}

//功能：	在指定的无线网卡上为指定的无线网络增加profile
//参数：	lpNicGuid为要增加profile的无线网卡
//			lpszSsid为profile对应的无线网络的SSID
//			lpszAuth指定认证方式
//			lpszCipher指定加密方式
//			lpszPwd指定密钥
//			bNoneBroadCast为true时连接到非广播网络
//返回值:	profile增加成功返回TRUE，否则返回FALSE
//说明：	该接口只对启用的无线网卡有效

extern "C" DIAGNOSEDLL_API BOOL __stdcall SetWlanInterfaceProfile(LPCTSTR lpNicGuid,LPCTSTR lpszSsid,LPCTSTR lpszAuth,LPCTSTR lpszCipher,LPCTSTR lpszPwd,BOOL bNoneBroadCast)
{
#if defined _UNICODE || defined UNICODE
#define stringt std::wstring
#else
#define stringt std::string
#endif
	stringt strGuid = TEXT("/NICGUID:");
	strGuid += lpNicGuid;

	stringt strSsid = TEXT("/SSID:");
	strSsid += lpszSsid;

	stringt strAuth=TEXT("/Authentication:");
	strAuth += lpszAuth;

	stringt strCipher=TEXT("/Encryption:");
	strCipher += lpszCipher;

	stringt strPwd = TEXT("/NetworkKey:");
	strPwd += lpszPwd;

	stringt strEAPType = TEXT("/EAPType:");
	strEAPType += TEXT("EAP-TLS");

	stringt strNoneBroadCast = TEXT("/NoneBroadCast:");
	strNoneBroadCast += TEXT("TRUE");

	const TCHAR* argv[7] = {NULL};
	argv[0] = strGuid.c_str();
	argv[1] = strSsid.c_str();
	argv[2] = strAuth.c_str();
	argv[3] = strCipher.c_str();
	argv[4] = strPwd.c_str();


	int argc = 5;

	if ((_tcscmp(lpszAuth,TEXT("WPA")) == 0) || (_tcscmp(lpszAuth,TEXT("WPA2")) == 0))
	{
		argv[argc] = strEAPType.c_str();
		++argc;
	}

	if (bNoneBroadCast)
	{
		argv[argc] = strNoneBroadCast.c_str();
		++argc;
	}

	return NO_ERROR == SetProfile(argc,argv);
}

extern "C" DIAGNOSEDLL_API BOOL __stdcall SetInterfaceProfile(LPCTSTR lpNicGuid,LPCTSTR lpszSsid,LPCTSTR lpszAuth,LPCTSTR lpszCipher,LPCTSTR lpszPwd)
{
	return SetWlanInterfaceProfile(lpNicGuid,lpszSsid,lpszAuth,lpszCipher,lpszPwd,FALSE);
}