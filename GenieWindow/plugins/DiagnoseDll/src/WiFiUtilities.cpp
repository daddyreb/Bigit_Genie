#include "WiFiUtilities.h"
#include <tchar.h>

// get auth algorithm string
LPCTSTR 
GetAuthAlgoString(
				  __in DOT11_AUTH_ALGORITHM dot11AuthAlgo
				  )
{
	LPCTSTR strRetCode = TEXT("Unknown algorithm");

	switch(dot11AuthAlgo)
	{
	case DOT11_AUTH_ALGO_80211_OPEN:
		strRetCode = TEXT("open");
		break;
	case DOT11_AUTH_ALGO_80211_SHARED_KEY:
		strRetCode = TEXT("shared");
		break;
	case DOT11_AUTH_ALGO_WPA:
		strRetCode = TEXT("WPA");
		break;
	case DOT11_AUTH_ALGO_WPA_PSK:
		strRetCode = TEXT("WPAPSK");
		break;
	case DOT11_AUTH_ALGO_WPA_NONE:
		strRetCode = TEXT("WPA");
		break;
	case DOT11_AUTH_ALGO_RSNA:
		strRetCode = TEXT("WPA2");
		break;
	case DOT11_AUTH_ALGO_RSNA_PSK:
		strRetCode = TEXT("WPA2PSK");
		break;
	default:
		if (dot11AuthAlgo & DOT11_AUTH_ALGO_IHV_START)
		{
			strRetCode = TEXT("Vendor-specific algorithm");
		}
	}

	return strRetCode;
}

// get cipher algorithm string
LPCTSTR
GetCipherAlgoString(
					__in DOT11_CIPHER_ALGORITHM dot11CipherAlgo
					)
{
	LPCTSTR strRetCode = TEXT("Unknown algorithm");

	switch(dot11CipherAlgo)
	{
	case DOT11_CIPHER_ALGO_NONE:
		strRetCode = TEXT("none");
		break;
	case DOT11_CIPHER_ALGO_WEP40:
		strRetCode = TEXT("WEP40");
		break;
	case DOT11_CIPHER_ALGO_TKIP:
		strRetCode = TEXT("TKIP");
		break;
	case DOT11_CIPHER_ALGO_CCMP:
		strRetCode = TEXT("AES");
		break;
	case DOT11_CIPHER_ALGO_WEP104:
		strRetCode = TEXT("WEP104");
		break;
	case DOT11_CIPHER_ALGO_WPA_USE_GROUP:
		strRetCode = TEXT("USE-GROUP");
		break;
	case DOT11_CIPHER_ALGO_WEP:
		strRetCode = TEXT("WEP");
		break;
	default:
		if (dot11CipherAlgo & DOT11_CIPHER_ALGO_IHV_START)
		{
			strRetCode = TEXT("Vendor-specific algorithm");
		}
	}

	return strRetCode;
}

std::wstring String2Wstring( const std::string &strConvert ) {
	int cchWideChar	=0;
	int sLength		= (int)strConvert.length()+1;
	cchWideChar		= MultiByteToWideChar(CP_ACP,0,strConvert.c_str(),sLength,0,0);
	wchar_t* buffer = new wchar_t[ cchWideChar ];
	MultiByteToWideChar(CP_ACP,0,strConvert.c_str(),sLength,buffer,cchWideChar);
	std::wstring wstrConvert(buffer);
	delete[] buffer;
	return wstrConvert;
}

// copy SSID to a null-terminated WCHAR string
// count is the number of WCHAR in the buffer.
LPWSTR
SsidToStringW(
			  __out_ecount(count) LPWSTR   buf,
			  __in ULONG   count,
			  __in PDOT11_SSID pSsid
			  )
{
	//ULONG   bytes, i;

	//bytes = min( count-1, pSsid->uSSIDLength);
	//for( i=0; i<bytes; i++)
	//	mbtowc( &buf[i], (const char *)&pSsid->ucSSID[i], MB_CUR_MAX);
	//buf[bytes] = '\0';

	int wchars = MultiByteToWideChar(CP_ACP,0,(LPCSTR)pSsid->ucSSID,pSsid->uSSIDLength,buf,count-1);
	buf[wchars] = L'\0';
	return buf;
}

BOOL StringToSsid(LPCTSTR lpSsid,PDOT11_SSID pdot11_ssid)
{
	BOOL bRet=FALSE;
	if (NULL==lpSsid || _tcslen(lpSsid)==0)
	{
		pdot11_ssid->uSSIDLength=0;
		bRet=TRUE;
	}
	else
	{
#if defined _UNICODE || defined UNICODE
		int bytes=WideCharToMultiByte(CP_ACP,0,lpSsid,-1,NULL,0,NULL,NULL);
		if (bytes!=0)
		{
			char *lpchSsid=new char[bytes+1];
			bytes=WideCharToMultiByte(CP_ACP,0,lpSsid,-1,lpchSsid,bytes,NULL,NULL);
			if (bytes!=0)
			{
				pdot11_ssid->uSSIDLength=strlen(lpchSsid);
				memcpy(pdot11_ssid->ucSSID,lpchSsid,pdot11_ssid->uSSIDLength);
				bRet=TRUE;
			}
			delete [] lpchSsid;
		}

#else
		pdot11_ssid->uSSIDLength=strlen(lpSsid);
		memcpy(pdot11_ssid->ucSSID,lpSsid,pdot11_ssid->uSSIDLength);
		bRet=TRUE;
#endif
	}
	return bRet;
}

BOOL StringToDot11Bsstype(LPCTSTR lpBssType,PDOT11_BSS_TYPE pdot11_bss_type)
{
	BOOL bRet=FALSE;
	if (NULL==lpBssType || _tcslen(lpBssType)==0)
	{
		return bRet;
	}

	bRet=TRUE;
	if (!_tcsicmp(lpBssType,TEXT("infrastructure")))
	{
		*pdot11_bss_type= dot11_BSS_type_infrastructure;
	}
	else if (!_tcsicmp(lpBssType,TEXT("independent")))
	{
		*pdot11_bss_type= dot11_BSS_type_independent;
	}
	else if (!_tcsicmp(lpBssType,TEXT("any")))
	{
		*pdot11_bss_type= dot11_BSS_type_any;
	}
	else
	{
		bRet=FALSE;
	}

	return bRet;
}

LPCTSTR Dot11BsstypeToString(DOT11_BSS_TYPE dot11_bss_type)
{
	LPCTSTR strRet=TEXT("unknow_type");
	switch(dot11_bss_type)
	{
	case dot11_BSS_type_infrastructure:
		strRet=TEXT("infrastructure");
		break;
	case dot11_BSS_type_independent:
		strRet=TEXT("independent");
		break;
	case dot11_BSS_type_any:
		strRet=TEXT("any");
		break;
	}
	return strRet;
}
//功能：	获取当前可见的网络列表，集中式网络优先，按信号由强到弱排列，
//参数：	lpNicGuid指定要获取可见网络列表的无线网卡，
//			lppVList输出参数包含各个可见网络的信息,
//			pSortedIndex输出参数按集中式网络优先，信号由强到弱原则对可见网络列表排序后得到的索引列表,已经连接的排第一位
//			dwNetworkNum输出参数返回可见网络的数目
//返回值：	获取成功返回TRUE，否则返回FASE。
//注意：	暂时未考虑不同路由器的相同ssid，当前算法只保留了其中的一个ssid

BOOL GetSortedVisibleNetworkList(LPCTSTR lpNicGuid,PWLAN_AVAILABLE_NETWORK_LIST* lppVList,int** pSortedIndex,DWORD& dwNetworkNum)
{
	HANDLE  hClient		= NULL;
	DWORD	dwVersion	= 0;
	DWORD	i			= 0;
	GUID	NicGuid;

	*lppVList		=NULL;
	*pSortedIndex	=NULL;
	dwNetworkNum	=0;

	if (NULL==lpNicGuid||0==_tcslen(lpNicGuid)||
		UuidFromString((RPC_TSTR)lpNicGuid,&NicGuid)!=RPC_S_OK)
	{
		return FALSE;
	}

	if (WlanOpenHandle(WLAN_API_VERSION,NULL,&dwVersion,&hClient) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	if (WlanGetAvailableNetworkList(hClient,&NicGuid,0,NULL,lppVList) != ERROR_SUCCESS)
	{
		WlanCloseHandle(hClient,NULL);
		return FALSE;
	}
	if ((*lppVList)->dwNumberOfItems!=0)
	{
		dwNetworkNum=(*lppVList)->dwNumberOfItems;

		int *pIndex=new int[dwNetworkNum];

		//建立索引数组
		for (i=0;i<dwNetworkNum;++i)
		{
			pIndex[i] = i;
		}

		//---------------去空SSID和重复SSID-------------------------------
		WCHAR wstrSsid[DOT11_SSID_MAX_LENGTH+1];
		WCHAR wstrSsidTemp[DOT11_SSID_MAX_LENGTH+1];

		//去空
		for (i=0;i<dwNetworkNum;++i)
		{
			SsidToStringW(wstrSsid, sizeof(wstrSsid)/sizeof(WCHAR), &((*lppVList)->Network[pIndex[i]].dot11Ssid));
			if (wcslen(wstrSsid)==0)
			{
				for (DWORD j=i+1;j<dwNetworkNum;++j)
				{
					pIndex[j-1]=pIndex[j];
				}
				--i;//再次检查新移过来的项是否为空
				--dwNetworkNum;
			}

		}

		//去重复
		for(i=0;i<dwNetworkNum;++i)
		{
			SsidToStringW(wstrSsid, sizeof(wstrSsid)/sizeof(WCHAR), &((*lppVList)->Network[pIndex[i]].dot11Ssid));
			for (DWORD k=i+1;k<dwNetworkNum;++k)
			{
				SsidToStringW(wstrSsidTemp, sizeof(wstrSsidTemp)/sizeof(WCHAR), &((*lppVList)->Network[pIndex[k]].dot11Ssid));
				if (!wcscmp(wstrSsid,wstrSsidTemp))
				{
					//有重复时始终保留当前已经连接上或存在配置文件的那一项
					if(((*lppVList)->Network[pIndex[k]].dwFlags&WLAN_AVAILABLE_NETWORK_CONNECTED) || 
						(((*lppVList)->Network[pIndex[k]].dwFlags&WLAN_AVAILABLE_NETWORK_HAS_PROFILE) && 
						(!((*lppVList)->Network[pIndex[i]].dwFlags&WLAN_AVAILABLE_NETWORK_HAS_PROFILE))))
					{
						pIndex[i] ^= pIndex[k];
						pIndex[k] ^= pIndex[i];
						pIndex[i] ^= pIndex[k];
					}

					//修正信号强度

					if((*lppVList)->Network[pIndex[k]].wlanSignalQuality 
			> (*lppVList)->Network[pIndex[i]].wlanSignalQuality)
					{
						(*lppVList)->Network[pIndex[i]].wlanSignalQuality = (*lppVList)->Network[pIndex[k]].wlanSignalQuality;
					}

					for (DWORD l=k+1;l<dwNetworkNum;++l)
					{
						pIndex[l-1]=pIndex[l];
					}

					--k;//再次检查新移过来的项是否和当前项重复
					--dwNetworkNum;
				}
			}
		}
		//-----------------------end-------------------------------

		if(0==dwNetworkNum)
		{
			delete [] pIndex;
			pIndex = 0;
			goto lb_exit;
		}

		//-----------可用网络排序--------------
		int *pIndex2=new int[dwNetworkNum];
		int  iFront=-1,iEnd=(int)(dwNetworkNum);
		BOOL bExchanged=FALSE;

		for(i=0;i<dwNetworkNum;++i)
		{
			if ((*lppVList)->Network[pIndex[i]].dot11BssType==dot11_BSS_type_infrastructure)
			{
				pIndex2[++iFront] = pIndex[i];
			}
			else
			{
				pIndex2[--iEnd] = pIndex[i];
			}
		}

		delete [] pIndex;
		pIndex = 0;

		//已经连接的网络提到第一位
		for(i=0;i<dwNetworkNum;++i)
		{
			if ((*lppVList)->Network[pIndex2[i]].dwFlags&WLAN_AVAILABLE_NETWORK_CONNECTED)
			{
				if(i!=0)
				{
					if((*lppVList)->Network[pIndex2[i]].dot11BssType==dot11_BSS_type_infrastructure)
					{
						pIndex2[0] ^= pIndex2[i];
						pIndex2[i] ^= pIndex2[0];
						pIndex2[0] ^= pIndex2[i];
					}
					else
					{
						int index = pIndex2[i];

						for(int j=i;j>0;--j)
						{
							pIndex2[j] = pIndex2[j-1];
						}

						pIndex[0] = index;
					}
				}

				break;
			}
		}

		//sort infrastructure
		//排除已经连接的网络
		i = (((*lppVList)->Network[pIndex2[0]].dwFlags&WLAN_AVAILABLE_NETWORK_CONNECTED)?2:1);

		for (/*i=1*/;(int)i<=iFront;++i)
		{
			bExchanged=FALSE;
			for (int k=iFront;k>=(int)i;--k)
			{
				if ((*lppVList)->Network[pIndex2[k]].wlanSignalQuality>((*lppVList)->Network[pIndex2[k-1]].wlanSignalQuality))
				{
					pIndex2[k-1] ^= pIndex2[k];
					pIndex2[k]   ^= pIndex2[k-1];
					pIndex2[k-1] ^= pIndex2[k];
					bExchanged=TRUE;
				}
			}
			if (!bExchanged)
			{
				break;
			}
		}

		//sort other type
		i=iEnd+1;
		//排除已经连接的网络
		if((1==i) && ((*lppVList)->Network[pIndex2[0]].dwFlags&WLAN_AVAILABLE_NETWORK_CONNECTED))
		{
			i = 2;
		}

		for (/*i=iEnd+1*/;i<dwNetworkNum;++i)
		{
			bExchanged=FALSE;
			for (int k=(int)(dwNetworkNum-1);k>=(int)i;--k)
			{
				if ((*lppVList)->Network[pIndex2[k]].wlanSignalQuality>((*lppVList)->Network[pIndex2[k-1]].wlanSignalQuality))
				{
					pIndex2[k-1] ^= pIndex2[k];
					pIndex2[k]   ^= pIndex2[k-1];
					pIndex2[k-1] ^= pIndex2[k];
					bExchanged=TRUE;
				}
			}
			if (!bExchanged)
			{
				break;
			}
		}

lb_exit:
		if (0 == dwNetworkNum)
		{
			WlanFreeMemory(*lppVList);
			*lppVList=NULL;
		}
		else
		{
			*pSortedIndex=pIndex2;
		}

		//--------------------------可用网络排序 end--------------------------
	}

	WlanCloseHandle(hClient,NULL);

	return dwNetworkNum!=0;

}

//功能：释放可见网络列表和网络列表有序索引所占用的缓冲区
//参数：pVList可见网络列表，pSortedIndex有序索引列表
//返回值：释放成功返回TRUE，否则返回FASLE。

BOOL FreeVisibleNetworkList(PWLAN_AVAILABLE_NETWORK_LIST pVList,int* pSortedIndex)
{
	DWORD  dwVersion=0;
	HANDLE hClient=NULL;

	if (WlanOpenHandle(WLAN_API_VERSION,NULL,&dwVersion,&hClient) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	WlanFreeMemory(pVList);
	delete [] pSortedIndex;

	WlanCloseHandle(hClient,NULL);

	return TRUE;
}


//功能：获取指定网络的信道
//参数：lpNicGuid指定无线网卡
//		pDot11Ssid指定无线网络的SSID
//		dot11BssType指定网络类型
//		bSecurityEnabled指定是否启用安全
//返回值：返回信道
//该接口只能工作在vista及以后的系统上
int GetChannelVistaAndNew(LPCTSTR lpNicGuid,const  PDOT11_SSID pDot11Ssid,DOT11_BSS_TYPE dot11BssType,BOOL bSecurityEnabled)
{
	int iRet=0;
	GUID NicGuid;
	if (NULL==lpNicGuid||UuidFromString((RPC_TSTR)lpNicGuid,&NicGuid)!=RPC_S_OK)
	{
		return iRet;
	}

	HANDLE			hClient		=NULL;
	DWORD			dwVersion	=0;
	PWLAN_BSS_LIST	pWlanBssList=NULL;


	if (WlanOpenHandle(WLAN_API_VERSION,NULL,&dwVersion,&hClient) == ERROR_SUCCESS)
	{
		if (WlanGetNetworkBssList(hClient,&NicGuid,pDot11Ssid,dot11BssType,bSecurityEnabled,NULL,&pWlanBssList) == ERROR_SUCCESS)
		{
			ULONG frequency=pWlanBssList->wlanBssEntries[0].ulChCenterFrequency;

			if ((frequency > 2400000) && (frequency < 2484000))
			{
				iRet=(frequency - 2407000) / 5000;
			}
			else if ((frequency >= 2484000) && (frequency <= 2495000))
			{
				iRet= 14;
			}
			// 5 GHz
			else if ((frequency > 5000000) && (frequency < 5900000))
			{
				iRet= (frequency - 5000000) / 5000;
			}
			// Unknown
			else
			{
				iRet=0;
			}
		}
	}


	if (pWlanBssList != NULL)
	{
		WlanFreeMemory(pWlanBssList);
		pWlanBssList=NULL;
	}

	if (hClient !=NULL)
	{
		WlanCloseHandle(hClient,NULL);
	}

	return iRet;
}


//功能：获取指定网络的信道
//参数：lpNicGuid指定无线网卡
//		lpSsid指定无线网络的SSID
//返回值：返回信道
//该接口适用于winXP系统
int GetWlanChannelXP(LPCTSTR lpNicGuid,LPCTSTR lpSsid)
{
	int iChannel=0;
	if (NULL==lpNicGuid||NULL==lpSsid||_tcslen(lpNicGuid)<1)
	{
		return iChannel;
	}

	TCHAR* lpDevFileName=NULL;

	if (_tcsncmp(lpNicGuid,TEXT("{"),1))
	{
		lpDevFileName=new TCHAR[_tcslen(lpNicGuid)+_tcslen(TEXT("\\\\.\\{}"))+1] ;
		wsprintf(lpDevFileName,TEXT("\\\\.\\{%s}"),lpNicGuid);

	}
	else
	{
		lpDevFileName=new TCHAR[_tcslen(lpNicGuid)+_tcslen(TEXT("\\\\.\\"))+1];
		wsprintf(lpDevFileName,TEXT("\\\\.\\%s"),lpNicGuid);
	}

	if (NULL==lpDevFileName)
	{
		return iChannel;
	}

	HANDLE hDevNic=NULL;
	//open device
	hDevNic=CreateFile(lpDevFileName,
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0,
		NULL) ;

	delete [] lpDevFileName;
	lpDevFileName=NULL;

	if(hDevNic==INVALID_HANDLE_VALUE)
	{
		return iChannel;
	}


	//query info
	ULONG oidcode ;
	ULONG bytesreturned ;
#define NUMBEROF_BSSIDS 1000

	NDIS_802_11_BSSID_LIST* pBSSIDList = (NDIS_802_11_BSSID_LIST*)VirtualAlloc(
		NULL,sizeof(NDIS_802_11_BSSID_LIST)*NUMBEROF_BSSIDS,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);

	if( pBSSIDList == NULL)
	{
		CloseHandle(hDevNic);
		return iChannel;//error
	}

	memset( pBSSIDList, 0, sizeof( NDIS_802_11_BSSID_LIST) * NUMBEROF_BSSIDS) ;
	oidcode = OID_802_11_BSSID_LIST_SCAN ;

	DeviceIoControl(hDevNic,
		IOCTL_NDIS_QUERY_GLOBAL_STATS,
		&oidcode,
		sizeof( oidcode),
		( ULONG *) NULL,
		0,
		&bytesreturned,
		NULL) ;

	//Sleep( 2000) ;
	memset( pBSSIDList, 0, sizeof( NDIS_802_11_BSSID_LIST) * NUMBEROF_BSSIDS) ;
	oidcode = OID_802_11_BSSID_LIST ;

	BOOL bRet=DeviceIoControl(hDevNic,
		IOCTL_NDIS_QUERY_GLOBAL_STATS,
		&oidcode,
		sizeof( oidcode),
		( ULONG *)pBSSIDList,
		sizeof( NDIS_802_11_BSSID_LIST) * NUMBEROF_BSSIDS,
		&bytesreturned,
		NULL);

	CloseHandle(hDevNic);
	hDevNic=NULL;

	if(bRet)
	{
		TCHAR chSsid[128]={0};
		PNDIS_WLAN_BSSID cpSsid=pBSSIDList->Bssid;
		for(ULONG i=0 ;i<pBSSIDList->NumberOfItems;i++)
		{
#if defined UNICODE||defined UNICODE
			MultiByteToWideChar(CP_ACP,0,((LPCSTR)cpSsid->Ssid.Ssid),-1,chSsid,sizeof(chSsid)/sizeof(chSsid[0]));
#else
			_tcscpy(chSsid,(const char*)(cpSsid->Ssid.Ssid));
#endif
			if(!_tcscmp(chSsid,lpSsid))
			{
				int frequency= cpSsid->Configuration.DSConfig;
				if ((frequency > 2400000) && (frequency < 2484000))
				{
					iChannel=(frequency - 2407000) / 5000;
				}
				else if ((frequency >= 2484000) && (frequency <= 2495000))
				{
					iChannel= 14;
				}
				// 5 GHz
				else if ((frequency > 5000000) && (frequency < 5900000))
				{
					iChannel= (frequency - 5000000) / 5000;
				}
				// Unknown
				else
				{
					iChannel=0;
				}
			}
			//next ...
			cpSsid=(PNDIS_WLAN_BSSID)((char*)cpSsid+(cpSsid->Length));
		}
	}

	if(pBSSIDList!=NULL){
		VirtualFree(pBSSIDList,0,MEM_RELEASE);
		pBSSIDList =NULL;
	}

	return iChannel;
}


struct NotifyContext
{
	volatile BOOL bOptOk;
	GUID infGuid;
};

VOID __stdcall WlanNotificationCallBack(
										PWLAN_NOTIFICATION_DATA pData,
										PVOID pContext
										)
{
	if(NULL==pData || NULL==pContext)
		return;

	NotifyContext *pNotifyContext = static_cast<NotifyContext *>(pContext);

	if(NULL==pNotifyContext)
	{
		return;
	}

	RPC_STATUS rpc_status;
	if( UuidEqual(&(pNotifyContext->infGuid),&(pData->InterfaceGuid),&rpc_status)
		&& (WLAN_NOTIFICATION_SOURCE_ACM==pData->NotificationSource) 
		&& (wlan_notification_acm_scan_complete==pData->NotificationCode))
	{
		pNotifyContext->bOptOk = TRUE;
	}
}

BOOL Rescan_Wlan(LPCTSTR lpNicGuid,int interrupt_flag)
{
	GUID			nic_guid;
	DWORD			dwVersion	= 0;
	HANDLE			hClient		= NULL;

	if(NULL==lpNicGuid || UuidFromString((RPC_TSTR)lpNicGuid,&nic_guid)!=RPC_S_OK)
	{
		return FALSE;
	}


	if (WlanOpenHandle(WLAN_API_VERSION,NULL,&dwVersion,&hClient)!=ERROR_SUCCESS)
	{
		return FALSE;
	}

	if(GetOsVersion() >= 6)
	{
		NotifyContext notify_context;
		notify_context.bOptOk	= FALSE;
		notify_context.infGuid	= nic_guid;
		DWORD dwPrevNotifSource = 0;
		if(WlanRegisterNotification(hClient,WLAN_NOTIFICATION_SOURCE_ACM,FALSE
			,WlanNotificationCallBack,&notify_context,NULL,&dwPrevNotifSource) != ERROR_SUCCESS)
		{
			WlanCloseHandle(hClient,NULL);
			return FALSE;
		}

		if(WlanScan(hClient,&nic_guid,NULL,NULL,NULL)!=ERROR_SUCCESS)
		{
			WlanRegisterNotification(hClient,WLAN_NOTIFICATION_SOURCE_NONE
				,FALSE,NULL,NULL,NULL,&dwPrevNotifSource);
			WlanCloseHandle(hClient,NULL);
			return FALSE;
		}

		int times = 30;
		while((!notify_context.bOptOk) && (times--))
		{
			if (interrupt_flag)
			{
				if (sleep_interruptable(1000))
				{
					//interrupted
					break;
				}
			}
			else
			{
				Sleep(1000);
			}
		}

		WlanRegisterNotification(hClient,WLAN_NOTIFICATION_SOURCE_NONE
			,FALSE,NULL,NULL,NULL,&dwPrevNotifSource);
	}
	else
	{
		if(WlanScan(hClient,&nic_guid,NULL,NULL,NULL)!=ERROR_SUCCESS)
		{
			WlanCloseHandle(hClient,NULL);
			return FALSE;
		}
		//Sleep(5000);
	}

	WlanCloseHandle(hClient,NULL);

	return TRUE;
}

int is_service_running(const wchar_t *service_name)
{
	if (NULL==service_name || wcslen(service_name)==0)
	{
		return -1;
	}

	SC_HANDLE scm_handle		= NULL;
	SC_HANDLE service_handle	= NULL;
	SERVICE_STATUS_PROCESS service_status;
	DWORD	  bytes_needed		= 0;

	scm_handle = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if (NULL==scm_handle)
	{
		return -1;
	}

#if defined UNICODE || _UNICODE
	service_handle = OpenService(scm_handle,service_name,SERVICE_ALL_ACCESS);
#else
	char *chserviceName = new char[wcslen(service_name) + 1];
	WideCharToMultiByte(CP_ACP,0,service_name,-1,chserviceName,wcslen(service_name) + 1,NULL,NULL);
	service_handle = OpenService(scm_handle,chserviceName,SERVICE_ALL_ACCESS);
	delete [] chserviceName;
	chserviceName = NULL;
#endif

	if (NULL==service_handle)
	{
		CloseServiceHandle(scm_handle);
		return -1;
	}

	if (!QueryServiceStatusEx(service_handle,SC_STATUS_PROCESS_INFO ,(LPBYTE)&service_status,sizeof(SERVICE_STATUS_PROCESS),&bytes_needed))
	{
		CloseServiceHandle(service_handle);
		CloseServiceHandle(scm_handle);
		return -1;
	}

	CloseServiceHandle(service_handle);
	CloseServiceHandle(scm_handle);

	return (SERVICE_RUNNING==service_status.dwCurrentState?1:0);
}

bool start_service(const wchar_t *service_name)
{
	if (NULL==service_name || wcslen(service_name)==0)
	{
		return false;
	}

	SC_HANDLE				scm_handle			= NULL;
	SC_HANDLE				service_handle		= NULL;
	SERVICE_STATUS_PROCESS	service_status;
	DWORD					bytes_needed		= 0;
	DWORD					buffer_bytes		= 0;
	LPQUERY_SERVICE_CONFIG  pqservice_cfg		= NULL;

	scm_handle = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if (NULL==scm_handle)
	{
		return false;
	}

#if defined UNICODE || _UNICODE
	service_handle = OpenService(scm_handle,service_name,SERVICE_ALL_ACCESS);
#else
	char *chserviceName = new char[wcslen(service_name) + 1];
	WideCharToMultiByte(CP_ACP,0,service_name,-1,chserviceName,wcslen(service_name) + 1,NULL,NULL);
	service_handle = OpenService(scm_handle,chserviceName,SERVICE_ALL_ACCESS);
	delete [] chserviceName;
	chserviceName = NULL;
#endif

	if (NULL==service_handle)
	{
		CloseServiceHandle(scm_handle);
		return false;
	}

	QueryServiceConfig(service_handle,NULL,0,&bytes_needed);

	if(ERROR_INSUFFICIENT_BUFFER==GetLastError())
	{
		buffer_bytes  = bytes_needed;
		pqservice_cfg = (LPQUERY_SERVICE_CONFIG) LocalAlloc(LMEM_FIXED, buffer_bytes);
	}
	else
	{
		CloseServiceHandle(service_handle);
		CloseServiceHandle(scm_handle);
		return false;
	}

	if (!QueryServiceConfig(service_handle,pqservice_cfg,buffer_bytes,&bytes_needed))
	{
		LocalFree(pqservice_cfg);
		CloseServiceHandle(service_handle);
		CloseServiceHandle(scm_handle);
		return false;
	}

	if (SERVICE_DISABLED==pqservice_cfg->dwStartType)
	{
		if (!ChangeServiceConfig(service_handle,SERVICE_NO_CHANGE,SERVICE_AUTO_START,SERVICE_NO_CHANGE,
			NULL,NULL,NULL,NULL,NULL,NULL,NULL))
		{
			LocalFree(pqservice_cfg);
			CloseServiceHandle(service_handle);
			CloseServiceHandle(scm_handle);
			return false;
		}
	}

	LocalFree(pqservice_cfg);
	pqservice_cfg = NULL;

	if (!StartService(service_handle,0,NULL))
	{
		CloseServiceHandle(service_handle);
		CloseServiceHandle(scm_handle);
		return false;
	}

	if (!QueryServiceStatusEx(service_handle,SC_STATUS_PROCESS_INFO ,(LPBYTE)&service_status,sizeof(SERVICE_STATUS_PROCESS),&bytes_needed))
	{
		CloseServiceHandle(service_handle);
		CloseServiceHandle(scm_handle);
		return false;
	}

	// Save the tick count and initial checkpoint.

	DWORD dwStartTickCount = GetTickCount();
	DWORD dwOldCheckPoint = service_status.dwCheckPoint;

	while (service_status.dwCurrentState == SERVICE_START_PENDING) 
	{ 
		// Do not wait longer than the wait hint. A good interval is 
		// one-tenth the wait hint, but no less than 1 second and no 
		// more than 10 seconds. 

		DWORD dwWaitTime = service_status.dwWaitHint / 10;

		if( dwWaitTime < 1000 )
			dwWaitTime = 1000;
		else if ( dwWaitTime > 10000 )
			dwWaitTime = 10000;

		Sleep( dwWaitTime );

		// Check the status again. 

		if (!QueryServiceStatusEx( 
			service_handle,              
			SC_STATUS_PROCESS_INFO, 
			(LPBYTE) &service_status,          
			sizeof(SERVICE_STATUS_PROCESS),
			&bytes_needed))
			break; 

		if ( service_status.dwCheckPoint > dwOldCheckPoint )
		{
			// The service is making progress.
			dwStartTickCount = GetTickCount();
			dwOldCheckPoint = service_status.dwCheckPoint;
		}
		else
		{
			if(GetTickCount()-dwStartTickCount > service_status.dwWaitHint)
			{
				// No progress made within the wait hint.
				break;
			}
		}
	} 

	CloseServiceHandle(service_handle);
	CloseServiceHandle(scm_handle);
	// Determine whether the service is running
	if (service_status.dwCurrentState == SERVICE_RUNNING) 
	{
		return true; 
	}
	else 
	{ 
		return false; 
	} 

}

BOOL GetWlanInterfaces(std::list<TString> &infList)
{
	PWLAN_INTERFACE_INFO_LIST	pIntfList	= NULL;
	DWORD						dwVersion	= 0;
	HANDLE						hClient		= NULL;
	RPC_TSTR					strUuid		= NULL;
	BOOL						bRet		= TRUE;

	infList.clear();

	if (WlanOpenHandle(WLAN_API_VERSION,NULL,&dwVersion,&hClient) == ERROR_SUCCESS)
	{
		if (WlanEnumInterfaces(hClient,NULL,&pIntfList) == ERROR_SUCCESS)
		{
			for (DWORD i = 0 ; i < pIntfList->dwNumberOfItems ; ++i)
			{
				if (UuidToString(&pIntfList->InterfaceInfo[i].InterfaceGuid,&strUuid) == RPC_S_OK)
				{
					infList.push_back(TString((TCHAR*)strUuid));
					RpcStringFree(&strUuid);
				}
				else
				{
					bRet = FALSE;
				}
			}

			WlanFreeMemory(pIntfList);
		}

		WlanCloseHandle(hClient,NULL);
	}

	return bRet;
}


//----------------------wifi profile -------------------------------------------

#include <stdio.h>
#include <Rpc.h>
#include <string>
#include <TCHAR.h>
#include <wlanapi.h>


#define NICGUID_LEN	  36
#define PARAMETER_LEN 10
#define MAX_NETWORKKEY_LEN 64
#define MAX_PREFERRED_NETWORKS 32
#define MAX_TRUSTED_ROOT_CA_LEN 256
#define MAX_CERT_NUM 10
#define HASH_LEN 20

#define EAP_TYPE_GTC 6

#define WLSAMPLE_REASON_STRING_LEN 256   // the max lenght of the reason string in characters

#pragma comment(lib,"crypt32.lib")
#pragma comment(lib,"Rpcrt4.lib")


//
// Retrieve the thumbprint of the certificate
//
DWORD GetCertificate(IN WCHAR* pwcTrustRootCA, 
					 IN PBYTE  pbCertHash,
					 IN DWORD* pcbCertHash)
{
	HCERTSTORE		hCertStore;
	WCHAR			*pwcSubjectName;
	DWORD			cwcSubjectName; 
	PBYTE			pbSHA1;
	DWORD			cbSHA1;
	PCCERT_CONTEXT	pCertContext = NULL;
	DWORD			dwRet;

	dwRet = NO_ERROR;

	if( ( hCertStore = CertOpenStore( CERT_STORE_PROV_SYSTEM, 
		X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
		( HCRYPTPROV ) NULL,
		CERT_SYSTEM_STORE_LOCAL_MACHINE,
		L"ROOT" ) ) )
	{
		BOOL	bFoundCert = FALSE;

		while( ( pCertContext = CertEnumCertificatesInStore( hCertStore,
			pCertContext ) ) &&
			bFoundCert == FALSE )
		{
			if( ( cwcSubjectName  = CertGetNameString( pCertContext,
				CERT_NAME_SIMPLE_DISPLAY_TYPE,
				0, 
				NULL,
				NULL,
				0 ) ) > 0 )
			{
				if( ( pwcSubjectName = ( WCHAR* ) malloc( cwcSubjectName * sizeof( WCHAR ) ) ) )
				{
					if( CertGetNameStringW( pCertContext,
						CERT_NAME_SIMPLE_DISPLAY_TYPE,
						0, 
						NULL,
						pwcSubjectName,
						cwcSubjectName ) > 0 )
					{
						//printf("Found the certificate [%ws] in the system store\n", pwcSubjectName);

						if ( wcscmp(pwcTrustRootCA, pwcSubjectName) == 0)
						{
							bFoundCert = TRUE;

							// Retrieve information on the property by first getting the property size.
							if(CertGetCertificateContextProperty( pCertContext, 
								CERT_SHA1_HASH_PROP_ID , // work fine on XP 
								NULL, 
								&cbSHA1))
							{
								// Use the size to allocate the memory for the property
								if ( pbSHA1 = (BYTE *) malloc(cbSHA1 * sizeof(BYTE) ) )
								{
									// Retrieve HASH of the certificate
									if(CertGetCertificateContextProperty( pCertContext, 
										CERT_SHA1_HASH_PROP_ID ,  
										pbSHA1, 
										&cbSHA1))
									{
										memcpy(pbCertHash, pbSHA1, cbSHA1);
										*pcbCertHash = cbSHA1;
									}
									else
									{
										printf("->GetCertificate :: Error retrieving certificate HASH.\n");
										dwRet = ERROR_CANTOPEN;
									}

									free(pbSHA1);

								}
								else
								{
									printf("->GetCertificate :: Error allocating memory.\n");
									dwRet = ERROR_NOT_ENOUGH_MEMORY;
								}
							}
							else
							{
								printf("->GetCertificate :: Error getting certificate property.\n");
								dwRet = ERROR_CANTOPEN;
							}

						} 
					}
					else
					{
						printf("->GetCertificate :: Error getting certificate name string.\n");
						dwRet = ERROR_CANTOPEN;
					}

					free( pwcSubjectName );
					cwcSubjectName = 0;
				}
				else
				{
					printf("->GetCertificate :: Error allocating memory.\n");
					dwRet = ERROR_NOT_ENOUGH_MEMORY;
				}
			}
			else
				dwRet = ERROR_CANTOPEN;
		}

		if( !bFoundCert )
		{
			printf("->GetCertificate :: Error looking for the certificate in the system store.\n");
			dwRet = ERROR_CANTOPEN;
		}

		if( dwRet != NO_ERROR )
		{
			if( pCertContext )
				CertFreeCertificateContext( pCertContext );

		}

		CertCloseStore( hCertStore, CERT_CLOSE_STORE_CHECK_FLAG );
	}
	else
	{
		printf("->GetCertificate :: Error opening system store.\n");
		dwRet = ERROR_CANTOPEN;
	}


	return dwRet;

}

// Helper function that converts a BYTE string to HEX WCHAR string
DWORD ByteToHexW_s( BYTE *xBytes, int xLength, WCHAR** xOutBuffer )
{
	WCHAR *xPtr;
	BYTE *xbPtr;
	int i, xByte;

	if( ( *xOutBuffer = ( WCHAR* ) malloc( sizeof( WCHAR ) * ( xLength + xLength + 1  ) ) ) )
	{
		xPtr = *xOutBuffer;
		xbPtr = xBytes;

		for (i = 0; i < xLength; i++)
		{
			xByte = ( *xbPtr & 0xf0 ) >> 4;
			*xPtr++ = ( xByte <= 9) ? xByte + '0' : ( xByte - 10 ) + 'A';
			xByte = (*xbPtr & 0x0f );
			*xPtr++ = (xByte <= 9 ) ? xByte + '0' : ( xByte - 10 ) + 'A';
			xbPtr++;
		}

		*xPtr++ = 0;
	}
	else
		return ERROR_NOT_ENOUGH_MEMORY;

	return NO_ERROR;
}


DWORD SetProfile(int argc, const TCHAR *argv[])
{ 
	DWORD                            dwRet = NO_ERROR; 
	DWORD                              i;
	HANDLE                           hClientHandle = NULL;
	WCHAR                            strReason[WLSAMPLE_REASON_STRING_LEN] = L"";
	DWORD                            dwReason;
	DWORD							 dwServiceVersion = 0;
	WCHAR					         *pwcXMLTemplate = NULL;
	DWORD					         ccXMLTemplate = 0; 	

	// input parameters
	GUID  NICGuid;
	TCHAR pcGUID[NICGUID_LEN+1] = TEXT("");
	TCHAR pcSSID[ DOT11_SSID_MAX_LENGTH + 1 ] = TEXT("");
	WCHAR pwcSSID[ DOT11_SSID_MAX_LENGTH+ 1 ]= L"";
	WCHAR pwcConnectionType[PARAMETER_LEN] = L"ESS";        // IBSS, ESS
	WCHAR pwcConnectionMode[PARAMETER_LEN] = L"auto";       // auto, manual
	WCHAR pwcAuthenticationMode[PARAMETER_LEN] = L"";       // open, shared, WPA, WPAPSK, WPA2, WPA2PSK
	WCHAR pwcEncryptionType[PARAMETER_LEN] = L"";           // none, WEP, TKIP, AES
	WCHAR pwcNetworkKey[MAX_NETWORKKEY_LEN] = L"";          // NULL, WEP keys are of 5 or 13 characters (ASCII) or of 10 or 26 characters (hexadecimal)
	// WPAPSK and WPA2PSK keys are 8-63 characters (ASCII) or 64 characters (hexadecimal)
	DWORD ulKeyIndex = 1;                                   // NULL, 1, 2, 3, 4
	WCHAR pwcEAPMethod[PARAMETER_LEN] = L"";                // EAP-TLS, PEAP, SecureW2
	DWORD dwEAPMethod = 25;                                 // 13, 25 
	WCHAR pwcInnerAuthenticationMethod[PARAMETER_LEN] = L"";  // GTC, MSCHAPv2
	BOOL  bUseWindowsLogonInfo = FALSE;
	BOOL  bEnableFastReconnect = FALSE;
	BOOL  bNoneBroadCast	   = FALSE;
	WCHAR pwcUseOneX[PARAMETER_LEN] = L"false";                   // true, false
	BOOL  bValidateServerCert = FALSE;
	WCHAR pwcTrustRootCA[MAX_CERT_NUM][MAX_TRUSTED_ROOT_CA_LEN];  // store names of certificates 
	BYTE  pbCertHash[MAX_CERT_NUM][HASH_LEN];                     // store HASH of certificates
	DWORD cbCertHash;
	DWORD dwCertNum = 0;   

	std::wstring wstrNetworkKey = L"";
	std::wstring strSrc= L"&";
	std::wstring strDest = L"&amp;";
	std::wstring::size_type pos = 0;
	//
	// EAP patterns
	//

	// EAP-TLS

	BYTE	pbPattern_TLS[] = 	{ 
		0x02,0x00,0x00,0x00, 0x2A,0x00,0x00,0x00, 
		0x17,0x00,0x00,0x00                        // pbPattern[8] : 0x15 -> Enable "Validate server certificate"; 0x17 -> Disable "Validate server certificate"
	};


	BYTE    pbPattern_TLS_Cert_Num[] = { 
		0x00,0x00,0x00,0x00, 0x00,0x00           // pbPattern[2]: the number of certificates
	};

	// PEAP-GTC


	BYTE	pbPattern_PEAP_GTC[] = 	{ 
		0x01,0x00,0x00,0x00, 0x36,0x00,0x00,0x00,
		0x01,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, // pbPattern[12] : 0x01 -> Enable Fast Reconnect; 0x00 -> Disable Fast Reconnect
		0x01,0x00,0x00,0x00, 0x15,0x00,0x00,0x00,
		0x17,0x00,0x00,0x00, 0x00,0x00,0x00,0x00  // pbPattern[24] : 0x15 -> Enable "Validate server certificate"; 0x17 -> Disable "Validate server certificate"
	};

	BYTE	pbPattern_GTC[] = 	{  
		0x00,0x00,0x01,0x00, 0x00,0x00,0x0F,0x00,
		0x00,0x00,0x06,0x00, 0x00,0x00,0x00,0x00, // 0x06 -> GTC
		0x00,0x00,0x00,0x00, 0x00,0x00
	};

	// PEAP-MSCHAPv2


	BYTE	pbPattern_PEAP_MSCHAPv2[] = { 
		0x01,0x00,0x00,0x00, 0x3E,0x00,0x00,0x00,
		0x01,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, // pbPattern[12] : 0x01 -> Enable Fast Reconnect; 0x00 -> Disable Fast Reconnect
		0x01,0x00,0x00,0x00, 0x15,0x00,0x00,0x00,
		0x17,0x00,0x00,0x00, 0x00,0x00,0x00,0x00  // pbPattern[24] : 0x15 -> Enable "Validate server certificate"; 0x17 -> Disable "Validate server certificate"
	};


	BYTE	pbPattern_MSCHAPv2[] = 	{
		0x00,0x00,0x01,0x00, 0x00,0x00,0x17,0x00,
		0x00,0x00,0x1A,0x00, 0x00,0x00,0x01,0x00, // pbPattern[10] : 0x1A -> MSCHAPv2
		0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, // pbPattern[18] : 0x02 -> UseWindowsLogonInfo; 0x00 -> !UseWindowsLogonInfo
		0x00,0x00,0x00,0x00, 0x00,0x00
	}; 


	// Certificate pattern

	BYTE	pbPattern_Cert[] = 	{
		0x14,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00
	}; 

	BYTE	pbPattern_Cert_Prefix[] = 	{
		0x14,0x00,0x00,0x00
	}; 



	//----------------------------------------------------------------------------------------------------

	// Initialize
	ZeroMemory(pwcTrustRootCA, sizeof(pwcTrustRootCA));

	// Scan the input parameters
	for (i=0; i<(DWORD)argc; i++)
	{
		const TCHAR *pStr;
		if (_tcsncmp(argv[i], TEXT("/NICGUID:"), _tcslen(TEXT("/NICGUID:"))) == 0 )
		{
			pStr = _tcschr(argv[i], TEXT(':'));
			pStr++;

			if (_tcslen(pStr)!=36)
			{
				printf("->SetProfile :: [NICGUID] : invalid input\n");
				dwRet = ERROR_INVALID_DATA;
				goto LDone;
			}
			else
			{ 
				_tcsncpy_s(pcGUID,NICGUID_LEN+1, pStr, _tcslen(pStr));
			}
		}
		else if ( _tcsncmp(argv[i], TEXT("/SSID:"), _tcslen(TEXT("/SSID:"))) == 0 )
		{
			pStr = _tcschr(argv[i], TEXT(':'));
			pStr++;

			if ( ( _tcslen(pStr) < 1 ) || ( _tcslen(pStr) > DOT11_SSID_MAX_LENGTH ) )
			{
				printf("->SetProfile :: [SSID] : invalid input\n");
				dwRet = ERROR_INVALID_DATA;

				goto LDone;
			}
			else
			{ 
				_tcsncpy_s(pcSSID,DOT11_SSID_MAX_LENGTH + 1, pStr, _tcslen(pStr));
			}
		}	
		else if ( _tcsncmp(argv[i], TEXT("/Authentication:"), _tcslen(TEXT("/Authentication:"))) == 0 )
		{ 
			pStr = _tcschr(argv[i], TEXT(':'));
			pStr++;

			if ( ( _tcscmp(pStr, TEXT("open"))==0 ) ||
				( _tcscmp(pStr, TEXT("shared"))==0 ) ||
				( _tcscmp(pStr, TEXT("WPA"))==0 ) ||
				( _tcscmp(pStr, TEXT("WPAPSK"))==0 ) ||
				( _tcscmp(pStr, TEXT("WPA2"))==0 ) ||
				( _tcscmp(pStr, TEXT("WPA2PSK"))==0 ) )
			{
#if defined _UNICODE||defined UNICODE
				wcscpy(pwcAuthenticationMode,pStr);
#else
				sscanf(pStr, "%ws", pwcAuthenticationMode);
#endif

				if ( _tcscmp(pStr, TEXT("WPA"))==0 || _tcscmp(pStr, TEXT("WPA2"))==0 )
					wcscpy_s(pwcUseOneX,PARAMETER_LEN, L"true");
			}
			else
			{
				printf("->SetProfile :: [AuthenticationMode] : invalid input\n");
				dwRet = ERROR_INVALID_DATA;

				goto LDone;
			}
		}
		else if ( _tcsncmp(argv[i], TEXT("/Encryption:"), _tcslen(TEXT("/Encryption:"))) == 0 )
		{ 
			pStr = _tcschr(argv[i], TEXT(':'));
			pStr++;

			if ( (_tcscmp(pStr, TEXT("WEP"))==0) || 
				(_tcscmp(pStr, TEXT("none"))==0) ||
				(_tcscmp(pStr, TEXT("AES"))== 0) || 
				(_tcscmp(pStr, TEXT("TKIP"))==0) )  
			{
#if defined _UNICODE||defined UNICODE
				wcscpy(pwcEncryptionType,pStr);
#else
				sscanf(pStr, "%ws", pwcEncryptionType);
#endif
			}
			else
			{
				printf("->SetProfile :: [Encryptiontype] : invalid input\n");
				dwRet = ERROR_INVALID_DATA;

				goto LDone;
			}
		}
		else if ( _tcsncmp(argv[i], TEXT("/NetworkKey:"), _tcslen(TEXT("/NetworkKey:"))) == 0 )
		{ 
			pStr = _tcschr(argv[i], TEXT(':'));
			pStr++;

			if ( _tcslen(pStr) <= 64 )

			{
#if defined _UNICODE||defined UNICODE
				wcscpy(pwcNetworkKey,pStr);
#else
				sscanf(pStr, "%ws", pwcNetworkKey);
#endif	 
			} 
			else
			{
				printf("->SetProfile :: [NetworkKey] : invalid input\n");
				dwRet = ERROR_INVALID_DATA;

				goto LDone;
			}
		}
		else if ( _tcsncmp(argv[i], TEXT("/KeyIndex:"), _tcslen(TEXT("/KeyIndex:"))) == 0 )
		{ 
			pStr = _tcschr(argv[i], TEXT(':'));
			pStr++;

			if ( (_tcscmp(pStr, TEXT("1"))== 0) ||
				(_tcscmp(pStr, TEXT("2"))== 0)  ||
				(_tcscmp(pStr, TEXT("3"))==0)   ||
				(_tcscmp(pStr, TEXT("4"))==0) )
			{
				_stscanf( pStr, TEXT("%d"), &ulKeyIndex);
			} 
			else
			{
				printf("->SetProfile :: [KeyIndex] : invalid input\n");
				dwRet = ERROR_INVALID_DATA;

				goto LDone;
			}
		}
		else if ( _tcsncmp(argv[i], TEXT("/EAPType:"), _tcslen(TEXT("/EAPType:"))) == 0 )
		{ 
			pStr = _tcschr(argv[i], TEXT(':'));
			pStr++;

			if ( (_tcscmp(pStr, TEXT("PEAP"))==0) || (_tcscmp(pStr, TEXT("EAP-TLS"))==0) )
			{
#if defined _UNICODE||defined UNICODE
				wcscpy(pwcEAPMethod,pStr);
#else
				sscanf( pStr, "%ws", pwcEAPMethod);
#endif	

				if ( _tcscmp(pStr, TEXT("PEAP"))==0 )
					dwEAPMethod = 25;
				else if ( _tcscmp(pStr, TEXT("EAP-TLS"))==0 )
					dwEAPMethod = 13;
			} 
			else
			{
				printf("->SetProfile :: [EAPType] : invalid input\n");
				dwRet = ERROR_INVALID_DATA;

				goto LDone;
			}
		} 
		else if ( _tcsncmp(argv[i], TEXT("/InnerAuthentication:"), _tcslen(TEXT("/InnerAuthentication:"))) == 0 )
		{ 
			pStr = _tcschr(argv[i], TEXT(':'));
			pStr++;

			if ( (_tcscmp(pStr, TEXT("GTC"))== 0) || (_tcscmp(pStr, TEXT("MSCHAPv2"))== 0) )
			{
				_stscanf( pStr,TEXT("%ws"), pwcInnerAuthenticationMethod); 
			} 
			else
			{
				printf("->SetProfile :: [InnerAuthenticationMethod] : invalid input\n");
				dwRet = ERROR_INVALID_DATA;

				goto LDone;
			}
		}
		else if ( _tcsncmp(argv[i], TEXT("/UseWindowsLogonInfo:"), _tcslen(TEXT("/UseWindowsLogonInfo:"))) == 0 )
		{ 
			pStr = _tcschr(argv[i], TEXT(':'));
			pStr++;

			if (_tcscmp(pStr, TEXT("TRUE")) == 0)
			{
				bUseWindowsLogonInfo = TRUE;
			}
			else if (_tcscmp(pStr, TEXT("FALSE")) == 0)
			{
				bUseWindowsLogonInfo = FALSE;
			}
			else
			{
				printf("->SetProfile :: [UseWindowsLogonInfo] : invalid input\n");
				dwRet = ERROR_INVALID_DATA;

				goto LDone;
			}
		}
		else if ( _tcsncmp(argv[i], TEXT("/NoneBroadCast:"), _tcslen(TEXT("/NoneBroadCast:"))) == 0 )
		{ 
			pStr = _tcschr(argv[i], TEXT(':'));
			pStr++;

			if (_tcscmp(pStr, TEXT("TRUE")) == 0)
			{
				bNoneBroadCast = TRUE;
			}
			else if (_tcscmp(pStr, TEXT("FALSE")) == 0)
			{
				bNoneBroadCast = FALSE;
			}
			else
			{
				printf("->SetProfile :: [NoneBroadCast] : invalid input\n");
				dwRet = ERROR_INVALID_DATA;

				goto LDone;
			}
		}
		else if ( _tcsncmp(argv[i], TEXT("/EnableFastReconnect:"), _tcslen(TEXT("/EnableFastReconnect:"))) == 0 )
		{ 
			pStr = _tcschr(argv[i], TEXT(':'));
			pStr++;

			if (_tcscmp(pStr, TEXT("TRUE")) == 0)
			{
				bEnableFastReconnect = TRUE;
			}
			else if (_tcscmp(pStr, TEXT("FALSE")) == 0)
			{
				bEnableFastReconnect = FALSE;
			}
			else
			{
				printf("->SetProfile :: [EnableFastReconnect] : invalid input\n");
				dwRet = ERROR_INVALID_DATA;

				goto LDone;
			}
		}
		else if ( _tcsncmp(argv[i], TEXT("/ValidateServerCert:"), _tcslen(TEXT("/ValidateServerCert:"))) == 0 )
		{
			pStr = _tcschr(argv[i], TEXT(':'));
			pStr++;

			if (_tcscmp(pStr, TEXT("TRUE")) == 0)
			{
				bValidateServerCert = TRUE;
			}
			else if (_tcscmp(pStr, TEXT("FALSE")) == 0)
			{
				bValidateServerCert = FALSE;
			}
			else
			{
				printf("->SetProfile :: [ValidateServerCert] : invalid input\n");
				dwRet = ERROR_INVALID_DATA;

				goto LDone;
			}
		}
		else if ( _tcsncmp(argv[i], TEXT("/TrustRootCA:"), _tcslen(TEXT("/TrustRootCA:"))) == 0 )
		{ 
			pStr = _tcschr(argv[i], TEXT(':'));
			pStr++;

			if ( ( _tcslen(pStr) < 1 ) || ( _tcslen(pStr) > MAX_TRUSTED_ROOT_CA_LEN ) )
			{
				printf("->SetProfile :: [TrustRootCA] : invalid input\n");
				dwRet = ERROR_INVALID_DATA;

				goto LDone;
			}
			else
			{
				if ( dwCertNum < MAX_CERT_NUM )
				{
					size_t i;

					_stscanf(pStr, TEXT("%ws"), pwcTrustRootCA[dwCertNum]);

					// Change the format of the certificate from "xxxx_xxxx_xxxx" to "xxxx xxxx xxxx" 
					for ( i = 0; i < wcslen(pwcTrustRootCA[dwCertNum]); i++ )
					{
						if (pwcTrustRootCA[dwCertNum][i] == L'_')
							pwcTrustRootCA[dwCertNum][i] = L' ';
					} 

					dwCertNum++;
				}
				else
				{
					printf("->SetProfile :: [TrustRootCA] : the max number of certificates is %d.\n", MAX_CERT_NUM);
					dwRet = ERROR_INVALID_DATA;

					goto LDone;
				}
			}

		}
		else
		{
			printf("->SetProfile :: invalid input\n");
			dwRet = ERROR_INVALID_DATA;

			goto LDone;
		}
	}//for

	//--------------------------------------------------------------------------------------------------

	// Check the validity of input parameters

	//NICGUID
	if ( _tcscmp(pcGUID, TEXT(""))==0 )
	{
		printf("->SetProfile :: [NICGUID] must be set\n");
		dwRet = ERROR_INVALID_DATA;
		goto LDone;
	}
	else if (UuidFromString(
#if defined _UNDERFLOW || defined UNICODE
		(RPC_WSTR)pcGUID
#else
		(RPC_CSTR)pcGUID
#endif
		,&NICGuid) != RPC_S_OK)
	{
		printf("->SetProfile :: [NICGUID] is incorrect\n");
		dwRet = ERROR_INVALID_DATA;
		goto LDone;
	}
	// SSID
	if ( _tcscmp(pcSSID, TEXT("")) == 0 )
	{
		printf("->SetProfile :: [SSID] must be set\n");
		dwRet = ERROR_INVALID_DATA;
		goto LDone;
	}

	// Authentication mode
	{ 
		if ( (wcscmp(pwcAuthenticationMode, L"WPA") == 0) || (wcscmp(pwcAuthenticationMode, L"WPA2") == 0) )
		{
			// A networkKey must not be present
			if ( wcscmp(pwcNetworkKey, L"") != 0)
			{
				printf("->SetProfile :: [NetworkKey] should not be set\n");
				dwRet = ERROR_INVALID_DATA;
				goto LDone;
			} 
		}

		if ( (wcscmp(pwcAuthenticationMode, L"WPAPSK") == 0) || (wcscmp(pwcAuthenticationMode, L"WPA2PSK") == 0) )
		{ 
			// A networkKey must be present
			if ( wcscmp(pwcNetworkKey, L"") == 0)
			{
				printf("->SetProfile :: [NetworkKey] must be set\n");
				dwRet = ERROR_INVALID_DATA;
				goto LDone;
			}
		}
	}

	// Encryption type
	if ( ( (wcscmp(pwcEncryptionType, L"WEP")==0) || (wcscmp(pwcEncryptionType, L"none")==0) ) && 
		( (wcscmp(pwcAuthenticationMode, L"open")==0) || (wcscmp(pwcAuthenticationMode, L"shared")==0) )
		||
		( (wcscmp(pwcEncryptionType, L"AES")== 0) || (wcscmp(pwcEncryptionType, L"TKIP")==0) ) &&
		( (wcscmp(pwcAuthenticationMode, L"WPA")==0) || (wcscmp(pwcAuthenticationMode, L"WPAPSK")==0) || (wcscmp(pwcAuthenticationMode, L"WPA2")==0) || (wcscmp(pwcAuthenticationMode, L"WPA2PSK")==0) ) )
	{ 

		if ( (wcscmp(pwcAuthenticationMode, L"open")==0) && (wcscmp(pwcEncryptionType, L"none")==0) )
		{
			if ( wcscmp(pwcNetworkKey, L"") != 0)
			{
				printf("->SetProfile :: [NetworkKey] should not be set\n");
				dwRet = ERROR_INVALID_DATA;
				goto LDone;
			}
		}
	}
	else
	{
		printf("->SetProfile :: [Encryptiontype] : invalid input\n");
		dwRet = ERROR_INVALID_DATA;

		goto LDone;
	}

	// Network key
	// WEP keys are of 5 or 13 characters (ASCII) or of 10 or 26 characters (hexadecimal)
	// WPAPSK and WPA2PSK keys are 8-63 characters (ASCII) or 64 characters (hexadecimal)
	if ( ( ((wcscmp(pwcEncryptionType, L"WEP")==0) || ((wcscmp(pwcAuthenticationMode, L"shared")==0) && (wcscmp(pwcEncryptionType, L"none")==0))) && ( ( wcslen(pwcNetworkKey)!=5 ) && ( wcslen(pwcNetworkKey)!=13 ) && ( wcslen(pwcNetworkKey)!=10 ) && ( wcslen(pwcNetworkKey)!=26 ) ) )
		||
		( ( (wcscmp(pwcAuthenticationMode, L"WPAPSK")==0) || (wcscmp(pwcAuthenticationMode, L"WPA2PSK")==0)) && ( ( ( wcslen(pwcNetworkKey) < 8 ) || ( wcslen(pwcNetworkKey) > 64 ) ) ) ) )

	{
		printf("->SetProfile :: [NetworkKey] : invalid input\n");
		dwRet = ERROR_INVALID_DATA;
		goto LDone; 
	}


	// EAP Method
	if (  (wcscmp(pwcEAPMethod, L"PEAP") == 0) || (wcscmp(pwcEAPMethod, L"EAP-TLS") == 0) )
	{ 
		if ( wcscmp(pwcEAPMethod, L"PEAP") == 0 )
		{
			if ( wcscmp(pwcInnerAuthenticationMethod, L"") == 0 )
			{
				printf("->SetProfile :: the element [InnerAuthenticationMethod] should be set\n");
				dwRet = ERROR_INVALID_DATA;
				goto LDone; 
			}
		}

		if ( wcscmp(pwcEAPMethod, L"EAP-TLS") == 0 )
		{ 
			if ( wcscmp(pwcInnerAuthenticationMethod, L"") != 0 )
			{
				printf("->SetProfile :: the element [InnerAuthenticationMethod] should not be set\n");
				dwRet = ERROR_INVALID_DATA;
				goto LDone; 
			}

			if ( bUseWindowsLogonInfo != FALSE )
			{
				printf("->SetProfile :: the element [UseWindowsLogonInfo] should not be set\n");
				dwRet = ERROR_INVALID_DATA;
				goto LDone; 
			}

			if ( bEnableFastReconnect != FALSE )
			{
				printf("->SetProfile :: the element [EnableFastReconnect] should not be set\n");
				dwRet = ERROR_INVALID_DATA;
				goto LDone; 
			}
		}
	}  


	// Inner authentication method
	if ( (wcscmp(pwcInnerAuthenticationMethod, L"GTC") == 0) || (wcscmp(pwcInnerAuthenticationMethod, L"MSCHAPv2") == 0) )
	{ 
		if ( wcscmp(pwcEAPMethod, L"PEAP") != 0 )
		{
			printf("->SetProfile :: the element [EAPType] should be set to PEAP\n");
			dwRet = ERROR_INVALID_DATA;

			goto LDone;
		}
	}  

	// Use Windows logon info
	if ( bUseWindowsLogonInfo == TRUE )
	{
		if ( wcscmp(pwcInnerAuthenticationMethod, L"MSCHAPv2") != 0) 
		{
			printf("->SetProfile :: the element [InnerAuthenticationMethod] should be set to MSCHAPv2\n");
			dwRet = ERROR_INVALID_DATA;

			goto LDone;
		}
	}

	// Enable Fast Reconnect
	if ( bEnableFastReconnect == TRUE )
	{ 
		if ( wcscmp(pwcEAPMethod, L"PEAP") != 0 )
		{
			printf("->SetProfile :: the element [EAPType] should be set to PEAP\n");
			dwRet = ERROR_INVALID_DATA;

			goto LDone;
		}

		if ( (wcscmp(pwcInnerAuthenticationMethod, L"GTC") != 0) && (wcscmp(pwcInnerAuthenticationMethod, L"MSCHAPv2") != 0) )
		{
			printf("->SetProfile :: the element [InnerAuthenticationMethod] should be set to GTC or MSCHAPv2\n");
			dwRet = ERROR_INVALID_DATA;

			goto LDone;
		}
	}

	// retrieve HASH of the certificates
	if ( (bValidateServerCert == TRUE) && (dwCertNum != 0) )
	{
		// initialize
		memset( pbCertHash, 0, sizeof(pbCertHash) );
		cbCertHash = HASH_LEN;

		for(i = 0; i < dwCertNum; i++)
		{ 
			if ( NO_ERROR != GetCertificate(pwcTrustRootCA[i], pbCertHash[i], &cbCertHash))
			{
				printf("->SetProfile :: Error looking for the certificate [%ws] in the sytem store.\n", pwcTrustRootCA[i]);
				dwRet = ERROR_INVALID_DATA;

				goto LDone;
			}
		}
	}
	else if ( (bValidateServerCert == FALSE) && (dwCertNum > 0) )
	{
		printf("->SetProfile :: [ValidateServerCert] is not set.\n");
		dwRet = ERROR_INVALID_DATA;

		goto LDone;
	}

	//
	// print the input parameters
	//
	printf("->SetProfile :: input parameter: SSID : %ws\n", pcSSID); 
	printf("->SetProfile :: input parameter: AuthenticationMode : %ws\n", pwcAuthenticationMode); 
	printf("->SetProfile :: input parameter: EncryptionType : %ws\n", pwcEncryptionType); 
	printf("->SetProfile :: input parameter: NetworkKey : %ws\n", pwcNetworkKey); 
	printf("->SetProfile :: input parameter: KeyIndex : %d\n", ulKeyIndex);  
	printf("->SetProfile :: input parameter: EAPType : %ws\n", pwcEAPMethod);  
	printf("->SetProfile :: input parameter: InnerAuthenticaionMode : %ws\n", pwcInnerAuthenticationMethod); 
	printf("->SetProfile :: input parameter: UseWindowsLogonInfo : %d\n", bUseWindowsLogonInfo); 
	printf("->SetProfile :: input parameter: bNoneBroadCast : %d\n", bNoneBroadCast); 
	printf("->SetProfile :: input parameter: EnableFastReconnect : %d\n", bEnableFastReconnect);
	printf("->SetProfile :: input parameter: ValidateServerCert : %d\n", bValidateServerCert);

	for ( i = 0; i < dwCertNum; i++)
		printf("->SetProfile :: input parameter: TrustRootCA[%d] : %ws\n", i, pwcTrustRootCA[i]);

	wstrNetworkKey = pwcNetworkKey;
	while( (pos = wstrNetworkKey.find(strSrc, pos)) != std::wstring::npos)
	{
		wstrNetworkKey.replace(pos, strSrc.length(), strDest);
		pos += strDest.length();
	}

	//---------------------------------------------------------------------------------------------------------------

	// open handle	
	if(WlanOpenHandle(WLAN_API_VERSION,NULL,&dwServiceVersion,&hClientHandle) == ERROR_SUCCESS)
	{
#if defined _UNDERFLOW||defined UNICODE
		wcscpy(pwcSSID,pcSSID);
#else
		if ( (MultiByteToWideChar(CP_ACP, 0, pcSSID, -1, pwcSSID, sizeof(pwcSSID))) == 0)
		{
			dwRet = GetLastError();
			goto LDone;
		}
#endif

		// 
		// Create XML Template
		//
		if ( (wcscmp(pwcAuthenticationMode, L"WPAPSK")==0) || (wcscmp(pwcAuthenticationMode, L"WPA2PSK")==0) )
		{					 
			// Cases: WPAPSK/WPA2PSK --> AES/TKIP --> key
			// <useOneX> must be set to "false"
			// <keyType> must be set to "passPhrase"

			// calculate the size of the XML WirelessProfile Schema
			ccXMLTemplate = ( ( DWORD ) wcslen( L"<?xml version=\"1.0\"?><WLANProfile xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v1\"><name></name><SSIDConfig><SSID><name></name></SSID></SSIDConfig><connectionType></connectionType><connectionMode></connectionMode><MSM><security><authEncryption><authentication></authentication><encryption></encryption><useOneX></useOneX></authEncryption><sharedKey><keyType>passPhrase</keyType><protected>false</protected><keyMaterial></keyMaterial></sharedKey></security></MSM></WLANProfile>" )
				+ wcslen(pwcSSID)
				+ wcslen(pwcSSID)
				+ wcslen(pwcConnectionType)
				+ wcslen(pwcConnectionMode)
				+ wcslen(pwcAuthenticationMode)
				+ wcslen(pwcEncryptionType)
				+ wcslen(pwcUseOneX)
				//+ wcslen(pwcNetworkKey) ) * sizeof( WCHAR );
				+ wstrNetworkKey.length() ) * sizeof( WCHAR );

			// allocate memory for the XML template
			if( ( pwcXMLTemplate = (WCHAR*)malloc((ccXMLTemplate+1)*sizeof(WCHAR))))  
			{			
				memset( pwcXMLTemplate, 0, ccXMLTemplate );

				swprintf( pwcXMLTemplate, ccXMLTemplate, L"<?xml version=\"1.0\"?><WLANProfile xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v1\"><name>%s</name><SSIDConfig><SSID><name>%s</name></SSID></SSIDConfig><connectionType>%s</connectionType><connectionMode>%s</connectionMode><MSM><security><authEncryption><authentication>%s</authentication><encryption>%s</encryption><useOneX>%s</useOneX></authEncryption><sharedKey><keyType>passPhrase</keyType><protected>false</protected><keyMaterial>%s</keyMaterial></sharedKey></security></MSM></WLANProfile>",
					pwcSSID,
					pwcSSID, 
					pwcConnectionType,
					pwcConnectionMode,
					pwcAuthenticationMode, 
					pwcEncryptionType,
					pwcUseOneX,
					//pwcNetworkKey
					wstrNetworkKey.c_str()
					);
			}
			else
			{
				dwRet = ERROR_NOT_ENOUGH_MEMORY;
				goto LDone;
			}

		}
		else if ( (wcscmp(pwcAuthenticationMode, L"open")==0) || (wcscmp(pwcAuthenticationMode, L"shared")==0) ) 
		{
			if ( (wcscmp(pwcAuthenticationMode, L"open")==0) && wcscmp(pwcEncryptionType, L"none")==0 ) 
			{
				// Cases: open --> none

				// calculate the size of the XML WirelessProfile Schema
				ccXMLTemplate = ( ( DWORD ) wcslen( L"<?xml version=\"1.0\"?><WLANProfile xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v1\"><name></name><SSIDConfig><SSID><name></name></SSID></SSIDConfig><connectionType></connectionType><connectionMode></connectionMode><MSM><security><authEncryption><authentication></authentication><encryption></encryption><useOneX></useOneX></authEncryption></security></MSM></WLANProfile>" )
					+ wcslen(pwcSSID)
					+ wcslen(pwcSSID)
					+ wcslen(pwcConnectionType)
					+ wcslen(pwcConnectionMode)
					+ wcslen(pwcAuthenticationMode)
					+ wcslen(pwcEncryptionType)
					+ wcslen(pwcUseOneX)  ) * sizeof( WCHAR );

				// allocate memory for the XML template
				if( ( pwcXMLTemplate = (WCHAR*)malloc((ccXMLTemplate+1)*sizeof(WCHAR))))  
				{					
					memset( pwcXMLTemplate, 0, ccXMLTemplate );

					swprintf( pwcXMLTemplate, ccXMLTemplate, L"<?xml version=\"1.0\"?><WLANProfile xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v1\"><name>%s</name><SSIDConfig><SSID><name>%s</name></SSID></SSIDConfig><connectionType>%s</connectionType><connectionMode>%s</connectionMode><MSM><security><authEncryption><authentication>%s</authentication><encryption>%s</encryption><useOneX>%s</useOneX></authEncryption></security></MSM></WLANProfile>",
						pwcSSID,
						pwcSSID, 
						pwcConnectionType,
						pwcConnectionMode,
						pwcAuthenticationMode, 
						pwcEncryptionType,
						pwcUseOneX
						);
				}
				else
				{
					dwRet = ERROR_NOT_ENOUGH_MEMORY;
					goto LDone;
				}
			}
			else /*if ( wcscmp(pwcEncryptionType, L"WEP")==0 )*/
			{
				// Cases: open --> WEP ;shared --> WEP/none

				// calculate the size of the XML WirelessProfile Schema
				ccXMLTemplate = ( ( DWORD ) wcslen( L"<?xml version=\"1.0\"?><WLANProfile xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v1\"><name></name><SSIDConfig><SSID><name></name></SSID></SSIDConfig><connectionType></connectionType><connectionMode></connectionMode><MSM><security><authEncryption><authentication></authentication><encryption></encryption><useOneX></useOneX></authEncryption><sharedKey><keyType>networkKey</keyType><protected>false</protected><keyMaterial></keyMaterial></sharedKey><keyIndex></keyIndex></security></MSM></WLANProfile>" )
					+ wcslen(pwcSSID)
					+ wcslen(pwcSSID)
					+ wcslen(pwcConnectionType)
					+ wcslen(pwcConnectionMode)
					+ wcslen(pwcAuthenticationMode)
					+ wcslen(pwcEncryptionType)
					+ wcslen(pwcUseOneX)
					//+ wcslen(pwcNetworkKey) ) * sizeof( WCHAR ) + sizeof(DWORD);
					+ wstrNetworkKey.length() ) * sizeof( WCHAR ) + sizeof(DWORD);

				// allocate memory for the XML template
				if( ( pwcXMLTemplate = ( WCHAR* ) malloc((ccXMLTemplate+1)*sizeof(WCHAR))))  
				{					
					memset( pwcXMLTemplate, 0, ccXMLTemplate );

					swprintf( pwcXMLTemplate, ccXMLTemplate, L"<?xml version=\"1.0\"?><WLANProfile xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v1\"><name>%s</name><SSIDConfig><SSID><name>%s</name></SSID></SSIDConfig><connectionType>%s</connectionType><connectionMode>%s</connectionMode><MSM><security><authEncryption><authentication>%s</authentication><encryption>%s</encryption><useOneX>%s</useOneX></authEncryption><sharedKey><keyType>networkKey</keyType><protected>false</protected><keyMaterial>%s</keyMaterial></sharedKey><keyIndex>%d</keyIndex></security></MSM></WLANProfile>",
						pwcSSID,
						pwcSSID, 
						pwcConnectionType,
						pwcConnectionMode,
						pwcAuthenticationMode, 
						pwcEncryptionType,
						pwcUseOneX,
						//pwcNetworkKey,
						wstrNetworkKey.c_str(),
						(ulKeyIndex-1));	
				}
				else
				{
					dwRet = ERROR_NOT_ENOUGH_MEMORY;
					goto LDone;
				}
			}
		}
		else if ( wcscmp(pwcUseOneX, L"true") == 0 )
		{
			// Cases: WPA/WPA2 --> AES/TKIP --> PEAP/TLS --> EAP options
			// <useOneX> must be set to "true"

			// construct the config blob
			PBYTE pbConfig = NULL, pbConfigPtr = NULL;
			DWORD cbConfig = 0;

			if ( (wcscmp(pwcEAPMethod, L"PEAP")==0) && (wcscmp(pwcInnerAuthenticationMethod, L"MSCHAPv2")==0) )
			{
				// PEAP-MSCHAPv2
				// calculate the size of the memory
				// size = Patterns + Certificates
				cbConfig = sizeof( pbPattern_PEAP_MSCHAPv2 )+ sizeof( pbPattern_MSCHAPv2 );

				if ( dwCertNum != 0)
					cbConfig += dwCertNum * ( sizeof(pbPattern_Cert_Prefix) + HASH_LEN ); 

				// allocate and initialize memory
				pbConfig = (PBYTE) malloc (cbConfig);
				if ( pbConfig == NULL )
				{
					dwRet = ERROR_NOT_ENOUGH_MEMORY;
					goto LDone;
				}

				memset( pbConfig, 0, cbConfig );
				pbConfigPtr = pbConfig;

				// set the [EnableFastReconnect] option
				// pbPattern_PEAP_MSCHAPv2[12] : 0x01 -> Enable Fast Reconnect; 0x00 -> Disable Fast Reconnect
				// Default: 0x00
				if ( bEnableFastReconnect == TRUE )
				{
					pbPattern_PEAP_MSCHAPv2[12] = 0x01;
				}

				// set the [ValidateServerCert] option
				// pbPattern[24] : 0x15 -> Enable "Validate server certificate"; 0x17 -> Disable "Validate server certificate".
				// Default: 0x17    
				if ( bValidateServerCert == TRUE)
					pbPattern_PEAP_MSCHAPv2[24] = 0x15;

				// set the certificate-related elements
				if ( dwCertNum != 0 )
				{
					// pbPattern_PEAP_MSCHAPv2[28] represents the number of certificates
					// pbPattern_PEAP_MSCHAPv2[4] and pbPattern_PEAP_MSCHAPv2[20] increase (0x18) for each cert
					pbPattern_PEAP_MSCHAPv2[4] = (BYTE)(pbPattern_PEAP_MSCHAPv2[4] + dwCertNum * (0x18));
					pbPattern_PEAP_MSCHAPv2[20] = (BYTE)(pbPattern_PEAP_MSCHAPv2[20] + dwCertNum * (0x18));
					pbPattern_PEAP_MSCHAPv2[28] = (BYTE)(pbPattern_PEAP_MSCHAPv2[28] + dwCertNum);
				}

				// set the [UseWindowsLogonInfo] option
				// pbPattern_MSCHAPv2[18] : 0x02 -> UseWindowsLogonInfo; 0x00 -> !UseWindowsLogonInfo
				// Default: 0x00
				if ( bUseWindowsLogonInfo == TRUE )
				{  
					pbPattern_MSCHAPv2[18] = 0x02;
				}

				// construct the content of the config blob
				memcpy( pbConfigPtr, pbPattern_PEAP_MSCHAPv2, sizeof( pbPattern_PEAP_MSCHAPv2 ) );
				pbConfigPtr += sizeof( pbPattern_PEAP_MSCHAPv2 );

				if ( dwCertNum != 0)
				{ 
					for ( i = 0 ; i < dwCertNum ; i++)
					{
						memcpy( pbConfigPtr, pbPattern_Cert_Prefix, sizeof( pbPattern_Cert_Prefix ));
						pbConfigPtr += sizeof( pbPattern_Cert_Prefix );
						memcpy( pbConfigPtr, pbCertHash[i], HASH_LEN );
						pbConfigPtr += HASH_LEN;
					}

					memcpy( pbConfigPtr, pbPattern_MSCHAPv2, sizeof( pbPattern_MSCHAPv2 ) );
				}
				else
					memcpy( pbConfigPtr, pbPattern_MSCHAPv2, sizeof( pbPattern_MSCHAPv2 ) );

			} 
			else if ( (wcscmp(pwcEAPMethod, L"PEAP")==0) && (wcscmp(pwcInnerAuthenticationMethod, L"GTC")==0) )
			{
				// PEAP-GTC
				// calculate the size of the memory
				// size = Patterns + Certificates

				cbConfig = sizeof( pbPattern_PEAP_GTC )+ sizeof( pbPattern_GTC );

				if ( dwCertNum != 0)
					cbConfig += dwCertNum * ( sizeof(pbPattern_Cert_Prefix) + HASH_LEN );

				// allocate and initialize memory
				pbConfig = (PBYTE) malloc (cbConfig);
				if ( pbConfig == NULL )
				{
					dwRet = ERROR_NOT_ENOUGH_MEMORY;
					goto LDone;
				}

				memset( pbConfig, 0, cbConfig );
				pbConfigPtr = pbConfig;

				// set the [EnableFastReconnect] option
				// pbPattern_PEAP_GTC[12] : 0x01 -> Enable Fast Reconnect; 0x00 -> Disable Fast Reconnect
				// Default: 0x00
				if ( bEnableFastReconnect == TRUE )
				{
					pbPattern_PEAP_GTC[12] = 0x01;
				}

				// set the [ValidateServerCert] option
				// pbPattern[24] : 0x15 -> Enable "Validate server certificate"; 0x17 -> Disable "Validate server certificate".
				// Default: 0x17    
				if ( bValidateServerCert == TRUE)
					pbPattern_PEAP_GTC[24] = 0x15;

				// set the certificate-related elements
				if ( dwCertNum != 0 )
				{
					// pbPattern_PEAP_GTC[28] represents the number of certificates
					// pbPattern_PEAP_GTC[4] and pbPattern_PEAP_GTC[20] increase (0x18) for each cert
					pbPattern_PEAP_GTC[4] = (BYTE)(pbPattern_PEAP_GTC[4] + dwCertNum * (0x18));
					pbPattern_PEAP_GTC[20] = (BYTE)(pbPattern_PEAP_GTC[20] + dwCertNum * (0x18));
					pbPattern_PEAP_GTC[28] =(BYTE)(pbPattern_PEAP_GTC[28] + dwCertNum) ;
				}

				// construct the content of the config blob
				memcpy( pbConfigPtr, pbPattern_PEAP_GTC, sizeof( pbPattern_PEAP_GTC ) );
				pbConfigPtr += sizeof( pbPattern_PEAP_GTC );

				if ( dwCertNum != 0)
				{ 
					for ( i = 0 ; i < dwCertNum ; i++)
					{
						memcpy( pbConfigPtr, pbPattern_Cert_Prefix, sizeof( pbPattern_Cert_Prefix ));
						pbConfigPtr += sizeof( pbPattern_Cert_Prefix );
						memcpy( pbConfigPtr, pbCertHash[i], HASH_LEN );
						pbConfigPtr += HASH_LEN;
					}

					memcpy( pbConfigPtr, pbPattern_GTC, sizeof( pbPattern_GTC ) );
				}
				else
					memcpy( pbConfigPtr, pbPattern_GTC, sizeof( pbPattern_GTC ) );
			}
			else if ( wcscmp(pwcEAPMethod, L"EAP-TLS")==0 )
			{
				// EAP-TLS 

				// size = Patterns + Certificates
				if (dwCertNum <= 1)
				{
					cbConfig = sizeof( pbPattern_TLS ) + sizeof( pbPattern_Cert_Prefix ) + HASH_LEN 
						+ sizeof( pbPattern_TLS_Cert_Num );
				}
				else
				{
					cbConfig = sizeof( pbPattern_TLS ) + dwCertNum * ( sizeof( pbPattern_Cert_Prefix ) + HASH_LEN ) 
						+ sizeof( pbPattern_TLS_Cert_Num );
				}

				// allocate and initialize memory
				if ( NULL == ( pbConfig = (PBYTE) malloc (cbConfig) ) )
				{
					dwRet = ERROR_NOT_ENOUGH_MEMORY;
					goto LDone;
				}

				memset( pbConfig, 0, cbConfig );
				pbConfigPtr = pbConfig;


				// set the [ValidateServerCert] option
				// pbPattern[8] : 0x15 -> Enable "Validate server certificate"; 0x17 -> Disable "Validate server certificate".
				// Default: 0x17    
				if ( bValidateServerCert == TRUE)
					pbPattern_TLS[8] = 0x15;

				// set the certificate-related elements
				if ( dwCertNum >=2 )
				{ 
					// pbPattern_TLS[4] increase (0x18) for each cert if there is more than one cert
					pbPattern_TLS[4] = (BYTE)(pbPattern_TLS[4] + (dwCertNum-1) * (0x18)); 
				}


				// set the number of certificates
				pbPattern_TLS_Cert_Num[2] = (BYTE)dwCertNum;

				// construct the blob content
				memcpy( pbConfigPtr, pbPattern_TLS, sizeof( pbPattern_TLS ) );
				pbConfigPtr += sizeof( pbPattern_TLS );

				if ( dwCertNum == 0)
				{
					pbConfigPtr += ( sizeof(pbPattern_Cert_Prefix) + HASH_LEN );
					memcpy( pbConfigPtr, pbPattern_TLS_Cert_Num, sizeof(pbPattern_TLS_Cert_Num) );						 
				}
				else
				{
					memcpy( pbConfigPtr, pbPattern_Cert_Prefix, sizeof( pbPattern_Cert_Prefix ));
					pbConfigPtr += sizeof( pbPattern_Cert_Prefix );
					memcpy( pbConfigPtr, pbCertHash[0], HASH_LEN );
					pbConfigPtr += HASH_LEN;
					memcpy( pbConfigPtr, pbPattern_TLS_Cert_Num, sizeof(pbPattern_TLS_Cert_Num) );
					pbConfigPtr += sizeof( pbPattern_TLS_Cert_Num );

					if ( dwCertNum >= 2 )
					{
						for ( i = 1 ; i < dwCertNum ; i++)
						{
							memcpy( pbConfigPtr, pbPattern_Cert_Prefix, sizeof( pbPattern_Cert_Prefix ));
							pbConfigPtr += sizeof( pbPattern_Cert_Prefix );
							memcpy( pbConfigPtr, pbCertHash[i], HASH_LEN );
							pbConfigPtr += HASH_LEN;
						}
					} 
				}
			}

			if (NULL == pbConfig || cbConfig == 0)
			{
				dwRet = ERROR_INVALID_DATA;
				goto LDone;
			}

			WCHAR *pwcConfigData = NULL;
			dwRet = ByteToHexW_s( pbConfig, cbConfig, &pwcConfigData );
			free(pbConfig);
			pbConfig = NULL;
			cbConfig = 0;

			if (dwRet != NO_ERROR)
			{
				goto LDone;
			}


			// calculate the size of the XML WirelessProfile Schema
			ccXMLTemplate = ( ( DWORD ) wcslen( L"<?xml version=\"1.0\"?><WLANProfile xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v1\"><name></name><SSIDConfig><SSID><name></name></SSID></SSIDConfig><connectionType></connectionType><connectionMode></connectionMode><MSM><security><authEncryption><authentication></authentication><encryption></encryption><useOneX></useOneX></authEncryption><OneX xmlns=\"http://www.microsoft.com/networking/OneX/v1\"><EAPConfig><EapHostConfig xmlns=\"http://www.microsoft.com/provisioning/EapHostConfig\"><EapMethod><Type xmlns=\"http://www.microsoft.com/provisioning/EapCommon\"></Type><VendorId xmlns=\"http://www.microsoft.com/provisioning/EapCommon\">0</VendorId><VendorType xmlns=\"http://www.microsoft.com/provisioning/EapCommon\">0</VendorType><AuthorId xmlns=\"http://www.microsoft.com/provisioning/EapCommon\">0</AuthorId></EapMethod><ConfigBlob></ConfigBlob></EapHostConfig></EAPConfig></OneX></security></MSM></WLANProfile>" )
				+ wcslen(pwcSSID)
				+ wcslen(pwcSSID)
				+ wcslen(pwcConnectionType)
				+ wcslen(pwcConnectionMode)
				+ wcslen(pwcAuthenticationMode)
				+ wcslen(pwcEncryptionType)
				+ wcslen(pwcUseOneX)
				+ wcslen(pwcConfigData) ) * sizeof( WCHAR ) + sizeof(DWORD)	;


			// allocate memory for the XML template
			if( ( pwcXMLTemplate = (WCHAR*)malloc((ccXMLTemplate+1)*sizeof(WCHAR))))  
			{				
				memset( pwcXMLTemplate, 0, ccXMLTemplate );

				swprintf( pwcXMLTemplate, ccXMLTemplate, L"<?xml version=\"1.0\"?><WLANProfile xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v1\"><name>%s</name><SSIDConfig><SSID><name>%s</name></SSID></SSIDConfig><connectionType>%s</connectionType><connectionMode>%s</connectionMode><MSM><security><authEncryption><authentication>%s</authentication><encryption>%s</encryption><useOneX>%s</useOneX></authEncryption><OneX xmlns=\"http://www.microsoft.com/networking/OneX/v1\"><EAPConfig><EapHostConfig xmlns=\"http://www.microsoft.com/provisioning/EapHostConfig\"><EapMethod><Type xmlns=\"http://www.microsoft.com/provisioning/EapCommon\">%d</Type><VendorId xmlns=\"http://www.microsoft.com/provisioning/EapCommon\">0</VendorId><VendorType xmlns=\"http://www.microsoft.com/provisioning/EapCommon\">0</VendorType><AuthorId xmlns=\"http://www.microsoft.com/provisioning/EapCommon\">0</AuthorId></EapMethod><ConfigBlob>%s</ConfigBlob></EapHostConfig></EAPConfig></OneX></security></MSM></WLANProfile>",
					pwcSSID,
					pwcSSID, 
					pwcConnectionType,
					pwcConnectionMode,
					pwcAuthenticationMode, 
					pwcEncryptionType,
					pwcUseOneX,
					dwEAPMethod,
					pwcConfigData
					);

			}
			else
			{
				dwRet = ERROR_NOT_ENOUGH_MEMORY;	
			}

			free(pwcConfigData);
			pwcConfigData = NULL;

			if (dwRet != NO_ERROR)
			{
				goto LDone;
			}
		}
		else
		{ 
			dwRet = ERROR_NOT_SUPPORTED;
			goto LDone;
		}

		if (bNoneBroadCast)
		{
			WCHAR *pNode = L"<nonBroadcast>true</nonBroadcast>";
			WCHAR *pTemp = wcsstr(pwcXMLTemplate,L"</SSID>");
			WCHAR *pTempXML = (WCHAR*) malloc((wcslen(pwcXMLTemplate) + wcslen(pNode) + 1) * sizeof(WCHAR));

			if ((NULL == pTemp) || (NULL == pTempXML))
			{
				dwRet = ((NULL == pTemp) ?ERROR_INVALID_DATA : ERROR_NOT_ENOUGH_MEMORY);
				goto LDone;
			}

			pTemp += wcslen(L"</SSID>");

			wcsncpy(pTempXML,pwcXMLTemplate,pTemp - pwcXMLTemplate);
			pTempXML[pTemp - pwcXMLTemplate] = L'\0';

			wcscat(pTempXML,pNode);
			wcscat(pTempXML,pTemp);

			free(pwcXMLTemplate);

			pwcXMLTemplate = pTempXML;

		}

		//
		// Add to interface which GUID is in NICGuid
		//

		//// set profile remove old first
		//if (WlanDeleteProfile(hClientHandle,&NICGuid,pwcSSID,NULL) == ERROR_SUCCESS)
		//{
		//	printf("->SetProfile :: Delete old profile succeeded\n");
		//}

		if (WlanSetProfile(
			hClientHandle, 
			&NICGuid,      // GUID of the interface 
			0,                                               // flags to set on the profile. an all-user profile.
			pwcXMLTemplate,                                  // the XML representation of the profile
			NULL,                                            // use the default ACL
			TRUE,		                                     // overwrite a profile if it already exists
			NULL,                                            // reserved
			&dwReason
			) == ERROR_SUCCESS)
		{
			printf("->SetProfile :: WlanSetProfile succeeded\n");
		}
		else
		{
			printf("->SetProfile :: WlanSetProfile ERROR!");
			dwRet=ERROR_NOT_SUPPORTED;//
		}
	}
	else
	{
		dwRet=ERROR_NOT_SUPPORTED;//
	}

	//------------------------------------------------------------- 

LDone: 

	// clean up
	if (pwcXMLTemplate != NULL)
		free(pwcXMLTemplate);

	if (hClientHandle != NULL)
	{
		WlanCloseHandle(hClientHandle, NULL);
	} 

	return dwRet;
}

