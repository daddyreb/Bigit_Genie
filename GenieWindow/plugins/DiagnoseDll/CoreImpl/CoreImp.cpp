#include "CoreImp.h"
#include <Rpc.h>
#include <assert.h>

//#include <iostream>

#define NCF_PHYSICAL 0x4

extern const int *g_longoperation_interruptable_flag_addr;
extern HANDLE hflag_guard;

CSockLib theSockLib;
BOOL Ping(const char*,const char*,int);//defined in ping.cpp

BOOL IsClassHidden(const GUID * ClassGuid )
{
	#define  REGSTR_VAL_NODISPLAYCLASS TEXT("NoDisplayClass")

	BOOL bResult	=FALSE;
	HKEY hKey		=NULL;
	hKey=SetupDiOpenClassRegKey(ClassGuid,KEY_READ);
	if (hKey!=NULL)
	{
		bResult= RegQueryValueEx(hKey,REGSTR_VAL_NODISPLAYCLASS,NULL,NULL,NULL,NULL) ==ERROR_SUCCESS;
		RegCloseKey(hKey);
	}
	return bResult;
}


//���ܣ�	��ȡָ��������ý������
//������	strNicGuid (��������{})ָ��Ҫ��ȡý�����͵�����
//����ֵ��	ʧ�ܷ���-1,1����������2��������������ֵΪ�������͵�����
//
int GetNicMediaType(TString& strNicGuid)
{	
	HKEY	hKey	=NULL;
	TString	strPos	=KEY_NETWORK_CONNECTIONS;
	
	strPos+=TEXT("\\");
	strPos+=strNicGuid;
	strPos+=TEXT("\\Connection");

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, strPos.c_str(), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
		return -1;

	DWORD dwType		= REG_DWORD;
	DWORD dwMedia		= 0;
	DWORD dwBufferSize	= sizeof(DWORD);

	if (RegQueryValueEx(hKey, TEXT("MediaSubType"), NULL, &dwType, (LPBYTE)&dwMedia, &dwBufferSize) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		/*return -1;*/
		return 1;//Vista ϵͳ��������û��MediaSubType
	}

	RegCloseKey(hKey);

	return int(dwMedia);
}


//���ܣ�	����������PnpInstanceId��ȡ������Ӧ��GUID
//������	������PnpInstanceId,GuidList�������ص�GUID,���ص�GUID��������{}
//����ֵ��	���óɹ�����TRUE��ʧ�ܷ���FALSE

BOOL GetNicGuid(LPCTSTR pszPnpInstanceId, std::list<TString> &GuidList)
{
	//std::wcout << L"PnpInsId: " << pszPnpInstanceId <<std::endl;

	HKEY hKeyNetwork;
	BOOL res						= FALSE;
	TCHAR szGuid[MY_BUFFER_SIZE + 1];

	ZeroMemory(szGuid,sizeof(szGuid));

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, KEY_NETWORK_CONNECTIONS, 0, KEY_READ, &hKeyNetwork) != ERROR_SUCCESS)
		return FALSE;

	TCHAR szBuffer[MY_BUFFER_SIZE + 1];

	for (DWORD i=0 ; ; i++)
	{
		DWORD dwBufferSize = MY_BUFFER_SIZE;
		if (RegEnumKey(hKeyNetwork, i, szBuffer, dwBufferSize) != ERROR_SUCCESS)
		{
			RegCloseKey(hKeyNetwork);
			return res;
		}

		_tcscpy_s(szGuid,MY_BUFFER_SIZE+1 ,szBuffer);

		TCHAR szMyGuid[MY_BUFFER_SIZE+1];
		HKEY  hSubKey;
		DWORD dwType;

		wsprintf(szMyGuid, TEXT("%s\\Connection"), szGuid);
		if (RegOpenKeyEx(hKeyNetwork, szMyGuid, 0, KEY_READ, &hSubKey) != ERROR_SUCCESS)
			continue;

		dwType			= REG_SZ;
		dwBufferSize	= MY_BUFFER_SIZE;

		if (RegQueryValueEx(hSubKey, TEXT("PnpInstanceID"), NULL, &dwType, (LPBYTE)szBuffer, &dwBufferSize) != ERROR_SUCCESS)
		{
			RegCloseKey(hSubKey);
			continue;
		}

		if (_tcsicmp(szBuffer, pszPnpInstanceId) == 0)
		{
			res = TRUE;
			GuidList.push_back (szGuid);
			//std::wcout << L"Find a guid for this pnpid: " << szGuid << std::endl;
		}
		RegCloseKey(hSubKey);
	}

	RegCloseKey(hKeyNetwork);
	return res;
}

BOOL GetNicGuid(LPCTSTR pszPnpInstanceId, TString &guid)
{
	BOOL				bres							= FALSE;
	std::list<TString>	GuidList;
	TCHAR				szBuffer[MY_BUFFER_SIZE + 1];
	TCHAR				*pbuf							= szBuffer + 1;
	DWORD				dwBufferSize					= MY_BUFFER_SIZE;
	HKEY				hTcpipApt						= NULL;
	GUID				guidl,guidr;
	RPC_STATUS			status;

	if (!GetNicGuid(pszPnpInstanceId,GuidList))
	{
		return bres;
	}

	
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, KEY_NETWORK_TCPIP_ADPTERS, 0, KEY_READ, &hTcpipApt) != ERROR_SUCCESS)
		return FALSE;


	for (std::list<TString>::const_iterator it = GuidList.begin();it != GuidList.end();++it)
	{
		if (UuidFromString((RPC_TSTR)it->substr(1,REG_GUID_STR_LEN).c_str(),&guidl) != RPC_S_OK)
		{
			RegCloseKey(hTcpipApt);
			return bres;
		}

		for (DWORD i=0 ; ; i++)
		{
			if (RegEnumKey(hTcpipApt, i, szBuffer, dwBufferSize) != ERROR_SUCCESS)
			{
				break;
			}

			pbuf[REG_GUID_STR_LEN] = 0;

			if (UuidFromString((RPC_TSTR)pbuf,&guidr) != RPC_S_OK)
			{
				continue;
			}

			if (UuidEqual(&guidl,&guidr,&status))
			{
				guid = *it;
				bres = TRUE;
				RegCloseKey(hTcpipApt);
				//std::wcout << L"Guid valid for this pnpInsid:" << guid << std::endl;
				return bres;
			}
		}
	}
	//std::wcout << L"Guid valid for this pnpid:" << guid << std::endl;
	RegCloseKey(hTcpipApt);
	return bres;
}

//���ܣ�	�ж�ָ���������ǲ��������������������������֣�
//������	lpNicGuidҪ�����жϵ�������GUID,��������{}
//����ֵ��	����Ϊ������������TRUE�����򷵻�FALSE

BOOL IsPhysicalNic(LPCTSTR lpNicGuid)
{
	BOOL		 bRet		 =FALSE;
	DWORD		 dwDataLen	 =MAX_PATH;
	DWORD		 dwType		 =REG_SZ;
	HKEY		 hNetKey	 =NULL;
	HKEY		 hLocalNet	 =NULL;
	DWORD	     dwMedia	 =0;
	TCHAR		 szDataBuf[MY_BUFFER_SIZE+1];

	if (NULL == lpNicGuid || _tcslen(lpNicGuid) < (REG_GUID_STR_LEN + 2))
	{
		return bRet;
	}

	if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, KEY_NETWORK_CONNECTIONS, 0, KEY_READ,&hNetKey))
		return bRet;

	wsprintf(szDataBuf, TEXT("%s\\Connection"),lpNicGuid);
	if(ERROR_SUCCESS != RegOpenKeyEx(hNetKey ,szDataBuf ,0 ,KEY_READ, &hLocalNet))
	{
		RegCloseKey(hNetKey);
		return bRet;
	}

	dwDataLen = MY_BUFFER_SIZE;
	if (ERROR_SUCCESS != RegQueryValueEx(hLocalNet, TEXT("PnpInstanceID"), 0, &dwType, (BYTE *)szDataBuf, &dwDataLen))
	{
		goto Exit;
	}

	if ((NULL==_tcsstr(szDataBuf, TEXT("PCI")))&&(NULL==_tcsstr(szDataBuf,TEXT("USB")))&&(NULL==_tcsstr(szDataBuf,TEXT("1394"))))
		goto Exit;

	dwType		=REG_DWORD;
	dwDataLen	=sizeof(DWORD);
	if (ERROR_SUCCESS != RegQueryValueEx(hLocalNet, TEXT("MediaSubType"), 0, &dwType, (BYTE *)&dwMedia, &dwDataLen))
	{
		/*goto Exit;*/
		dwMedia=1;//Vista ϵͳ����������û��MediaSubType��ֵ
	}

	if (dwMedia!=0x01&&dwMedia!=2&&dwMedia!=5)
	{
		goto Exit;
	}

	bRet = TRUE;

Exit:
	RegCloseKey(hLocalNet);
	RegCloseKey(hNetKey);

	return (bRet?bRet:IsPhysicalNic2(lpNicGuid));
}

BOOL IsPhysicalNic2(LPCTSTR lpNicGuid)
{
	HKEY		hKeyNetClass;
	UUID		nic_guid,tmp_guid;
	RPC_STATUS	rpc_status;
	BOOL		res						= FALSE;
	DWORD		dwCharactics			= 0;
	TCHAR		szBuffer[MY_BUFFER_SIZE + 1];

	if (NULL == lpNicGuid || _tcslen(lpNicGuid) < (REG_GUID_STR_LEN + 2))
	{
		return res;
	}

	TString	nicguid_str = lpNicGuid;
	if(UuidFromString((RPC_TSTR)nicguid_str.substr(1,REG_GUID_STR_LEN).c_str(),&nic_guid) != RPC_S_OK)
	{
		return res;
	}

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, KEY_NETWORK_CLASS, 0, KEY_READ, &hKeyNetClass) != ERROR_SUCCESS)
		return res;

	for (DWORD i=0 ; ; i++)
	{
		DWORD dwBufferSize = MY_BUFFER_SIZE;
		if (RegEnumKey(hKeyNetClass, i, szBuffer, dwBufferSize) != ERROR_SUCCESS)
		{
			RegCloseKey(hKeyNetClass);
			return res;
		}

		HKEY  hSubKey;
		DWORD dwType;

		if (RegOpenKeyEx(hKeyNetClass, szBuffer, 0, KEY_READ, &hSubKey) != ERROR_SUCCESS)
			continue;

		dwType			= REG_SZ;
		dwBufferSize	= MY_BUFFER_SIZE;

		if (RegQueryValueEx(hSubKey, TEXT("NetCfgInstanceId"), NULL, &dwType, (LPBYTE)szBuffer, &dwBufferSize) != ERROR_SUCCESS)
		{
			RegCloseKey(hSubKey);
			continue;
		}

		TString	tmpguid_str(szBuffer);
		if ( UuidFromString((RPC_TSTR)tmpguid_str.substr(1,REG_GUID_STR_LEN).c_str(),&tmp_guid) != RPC_S_OK )
		{
			RegCloseKey(hSubKey);
			continue;
		}

		if (UuidEqual(&nic_guid,&tmp_guid,&rpc_status))
		{
			dwType			= REG_DWORD;
			dwBufferSize	= sizeof(DWORD);

			if (RegQueryValueEx(hSubKey, TEXT("Characteristics"), NULL, &dwType, (LPBYTE)&dwCharactics, &dwBufferSize) == ERROR_SUCCESS)
			{
				res = ((dwCharactics & NCF_PHYSICAL) == NCF_PHYSICAL);
			}

			RegCloseKey(hSubKey);
			break;
		}

		RegCloseKey(hSubKey);
	}

	RegCloseKey(hKeyNetClass);

	return res;
}

//���ܣ�	��ȡϵͳ�����汾�ź͸��汾��
//������	pdwMinorVersion�����������ΪNULLʱ���ظ��汾��
//����ֵ��	ϵͳ�����汾��

DWORD GetOsVersion(DWORD* pdwMinorVersion)
{
	OSVERSIONINFOEX osvi;
	DWORD			dwMajorVersion=0;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if (!GetVersionEx((OSVERSIONINFO *)&osvi))
	{
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (!GetVersionEx((OSVERSIONINFO *)&osvi))
		{
			if (pdwMinorVersion!=NULL)
			{
				*pdwMinorVersion=0;
			}
			return 0;
		}
	}

	if (osvi.dwPlatformId != VER_PLATFORM_WIN32_NT)
	{
		if (pdwMinorVersion!=NULL)
		{
			*pdwMinorVersion=0;
		}
		return 0;
	}

	if (pdwMinorVersion!=NULL)
	{
		*pdwMinorVersion=osvi.dwMinorVersion;
	}

	return osvi.dwMajorVersion;
}




//-------------------win 2003/ vista �����ý�������------------------------------------
//���ܣ�	����ָ������������
//������	lpNicGuidָ��Ҫ��ȡ���Ƶ�������GUID��
//			strFriName�����������������������
//����ֵ��	��ȡ�ɹ�����TRUE�����򷵻�FALSE

BOOL GetNicFriendlyName(LPCTSTR lpNicGuid,TString& strFriName)
{

	TString strKey	=KEY_NETWORK_CONNECTIONS;
	TString strTemp	=lpNicGuid;

	strKey+=TEXT("\\");
	strTemp.insert(0,TEXT("{"));
	strTemp.append(TEXT("}"));
	strKey+=strTemp;
	strKey+=TEXT("\\Connection");

	HKEY	hKey				=NULL;
	TCHAR	szBuffer[MAX_PATH]	={0};
	DWORD	dwType				=REG_SZ;
	DWORD   dwBufferSize		=sizeof(szBuffer);

	strFriName.clear();

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, strKey.c_str(), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
		return FALSE;

	if (RegQueryValueEx(hKey, TEXT("Name"), NULL, &dwType, (LPBYTE)szBuffer, &dwBufferSize) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return FALSE;
	}

	strFriName=szBuffer;
	
	RegCloseKey(hKey);
	return TRUE;
}

//���ܣ�	���úͽ�������
//������	lpNicFriNameΪ����������,��ͨ��GetNicFriendlyName���
//			bEnableΪTRUE����������FALSE��������
//����ֵ��	�����ɹ�����TRUE��ʧ�ܷ���FALSE
//˵����	�ýӿ��ڲ����� netshʵ��

BOOL NetCardStateChangeNetSh(LPCTSTR lpNicFriName,BOOL bEnable)
{
	TCHAR chCmd[512]={0};
	wsprintf(chCmd,TEXT("netsh interface set interface \"%s\" %s"),lpNicFriName,(bEnable?TEXT("enabled"):TEXT("disabled")));
	/*#if defined _UNICODE||defined UNICODE
		char chTemp[256]={0};
		WideCharToMultiByte(CP_ACP,0,chCmd,-1,chTemp,256,NULL,FALSE);
		return WinExec(chTemp,SW_HIDE)>31;
	#else
		return WinExec(chCmd,SW_HIDE)>31;
	#endif*/
	return Execute_Command_Privilege(chCmd);
}
//-------------------------end-----------------------------------------------


//return :-1:���ʧ�ܣ�0:������Ч,1:������Ч,2���ؿ�ͨ�����������Ч
int IsGatewayValid(LPCTSTR szGatewayIp,int interrupt_flag)
{
	static const char* PublicIPsToPing[]=
	{
		//"222.240.176.222",//site view vpn server
		//"203.208.39.99" ,//www.google.cn
		//"119.75.216.30",//www.baidu.com
		//"59.175.132.63"//www.sina.com.cn
		//"66.249.89.104",//www.google.com
		//"98.137.149.56"//?
		"64.94.70.219"//www.netgear.com
	};

	int iRet=-1;
	char chTemp[256]={0};

	if (NULL==szGatewayIp||_tcslen(szGatewayIp)==0)
	{
		return iRet;
	}

	#if defined _UNICODE||defined UNICODE
		WideCharToMultiByte(CP_ACP,0,szGatewayIp,-1,chTemp,256,NULL,NULL);
		BOOL b1=Ping(chTemp,chTemp,interrupt_flag);
	#else
		BOOL b1=Ping(szGatewayIp,szGatewayIp,interrupt_flag);
	#endif

	BOOL b2=FALSE;
	if (b1)
	{
		iRet=2;
		for (int i=0;i<sizeof(PublicIPsToPing)/sizeof(PublicIPsToPing[0]);++i)
		{
#if defined _UNICODE || defined UNICODE
			b2=Ping(PublicIPsToPing[i],chTemp,interrupt_flag);
#else
			b2=Ping(PublicIPsToPing[i],szGatewayIp,interrupt_flag);
#endif
			if (b2)
			{
				iRet=1;
				break;
			}
		}
	}
	else
	{
		iRet=0;
	}

	return iRet;
}


//-----------------------------------------------------------------
//	����ע����е�DHCP��Ϣ,lpszAdapterName��������{}
//-----------------------------------------------------------------

BOOL RegEnableDhcp(LPCTSTR lpszAdapterName)
{
	HKEY   hKey				= NULL;
	DWORD  dwEnable			= 1;
	TCHAR  DefaultGateway[2]={0,0};

	TString	 strKeyName		=TEXT("SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\");

	strKeyName += lpszAdapterName;

	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,strKeyName.c_str(),0,KEY_WRITE,&hKey) != ERROR_SUCCESS)
		return FALSE;

	if (RegSetValueEx(hKey, TEXT("EnableDHCP"), 0, REG_DWORD, (const BYTE*)&dwEnable,sizeof(dwEnable))!=ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return FALSE;
	}

	if (RegSetValueEx(hKey, TEXT("DefaultGateway"), 0, REG_MULTI_SZ, (const BYTE*)DefaultGateway,2)!=ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return FALSE;
	}

	RegCloseKey(hKey);
	return TRUE;
}

typedef BOOL (CALLBACK *PNotifyConfigChange)(
	LPWSTR lpwszServerName, // ���ػ���ΪNULL
	LPWSTR lpwszAdapterName, // ����������
	BOOL   bNewIpAddress, // TRUE��ʾ����IP
	DWORD  dwIpIndex, // ָ���ڼ���IP��ַ�����ֻ�иýӿ�ֻ��һ��IP��ַ��Ϊ0
	DWORD  dwIpAddress, // IP��ַ
	DWORD  dwSubNetMask, // ��������
	int    nDhcpAction ); // ��DHCP�Ĳ��� 0:���޸�, 1:���� DHCP��2:���� DHCP

//֪ͨϵͳDHCP�����Ѿ��ı� ,lpszAdapterName��������{}
BOOL NotifyDhcpEnable(LPCTSTR lpszAdapterName)
{
	BOOL					bResult				= FALSE;
	HINSTANCE				hDhcpDll			= NULL;
	PNotifyConfigChange		pDhcpNotifyProc		= NULL;
	WCHAR					wcAdapterName[256]	= {0};

	#if !defined _UNICODE && !defined UNICODE
		MultiByteToWideChar(CP_ACP, 0, lpszAdapterName, -1, wcAdapterName,256);
	#else
		wcscpy_s(wcAdapterName,lpszAdapterName);
	#endif

	if((hDhcpDll = LoadLibrary(TEXT("dhcpcsvc"))) == NULL)
		return FALSE;

	if((pDhcpNotifyProc = (PNotifyConfigChange)GetProcAddress(hDhcpDll, "DhcpNotifyConfigChange")) != NULL)
		if((pDhcpNotifyProc)(NULL, wcAdapterName, FALSE, 0,0,0,1) == ERROR_SUCCESS)
			bResult = TRUE;

	FreeLibrary(hDhcpDll);
	return bResult;
}


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
//���ܣ�	��ȡ��ǰ�ɼ��������б�����ʽ�������ȣ����ź���ǿ�������У�
//������	lpNicGuidָ��Ҫ��ȡ�ɼ������б������������
//			lppVList����������������ɼ��������Ϣ,
//			pSortedIndex�������������ʽ�������ȣ��ź���ǿ����ԭ��Կɼ������б������õ��������б�,�Ѿ����ӵ��ŵ�һλ
//			dwNetworkNum����������ؿɼ��������Ŀ
//����ֵ��	��ȡ�ɹ�����TRUE�����򷵻�FASE��
//ע�⣺	��ʱδ���ǲ�ͬ·��������ͬssid����ǰ�㷨ֻ���������е�һ��ssid

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

	CWlanApiInvoker invoker;
	if (!invoker.WlanOpenHandle(WLAN_API_VERSION,NULL,&dwVersion,&hClient))
	{
		return FALSE;
	}

	if (!invoker.WlanGetAvailableNetworkList(hClient,&NicGuid,0,NULL,lppVList))
	{
		invoker.WlanCloseHandle(hClient,NULL);
		return FALSE;
	}
	if ((*lppVList)->dwNumberOfItems!=0)
	{
		dwNetworkNum=(*lppVList)->dwNumberOfItems;

		int *pIndex=new int[dwNetworkNum];

		//������������
		for (i=0;i<dwNetworkNum;++i)
		{
			pIndex[i] = i;
		}

		//---------------ȥ��SSID���ظ�SSID-------------------------------
		WCHAR wstrSsid[DOT11_SSID_MAX_LENGTH+1];
		WCHAR wstrSsidTemp[DOT11_SSID_MAX_LENGTH+1];

		//ȥ��
		for (i=0;i<dwNetworkNum;++i)
		{
			SsidToStringW(wstrSsid, sizeof(wstrSsid)/sizeof(WCHAR), &((*lppVList)->Network[pIndex[i]].dot11Ssid));
			if (wcslen(wstrSsid)==0)
			{
				for (DWORD j=i+1;j<dwNetworkNum;++j)
				{
					pIndex[j-1]=pIndex[j];
				}
				--i;//�ٴμ�����ƹ��������Ƿ�Ϊ��
				--dwNetworkNum;
			}
			
		}

		//ȥ�ظ�
		for(i=0;i<dwNetworkNum;++i)
		{
			SsidToStringW(wstrSsid, sizeof(wstrSsid)/sizeof(WCHAR), &((*lppVList)->Network[pIndex[i]].dot11Ssid));
			for (DWORD k=i+1;k<dwNetworkNum;++k)
			{
				SsidToStringW(wstrSsidTemp, sizeof(wstrSsidTemp)/sizeof(WCHAR), &((*lppVList)->Network[pIndex[k]].dot11Ssid));
				if (!wcscmp(wstrSsid,wstrSsidTemp))
				{
					//���ظ�ʱʼ�ձ�����ǰ�Ѿ������ϻ���������ļ�����һ��
					if(((*lppVList)->Network[pIndex[k]].dwFlags&WLAN_AVAILABLE_NETWORK_CONNECTED) || 
						(((*lppVList)->Network[pIndex[k]].dwFlags&WLAN_AVAILABLE_NETWORK_HAS_PROFILE) && 
						(!((*lppVList)->Network[pIndex[i]].dwFlags&WLAN_AVAILABLE_NETWORK_HAS_PROFILE))))
					{
						pIndex[i] ^= pIndex[k];
						pIndex[k] ^= pIndex[i];
						pIndex[i] ^= pIndex[k];
					}

					//�����ź�ǿ��

					if((*lppVList)->Network[pIndex[k]].wlanSignalQuality 
						> (*lppVList)->Network[pIndex[i]].wlanSignalQuality)
					{
						(*lppVList)->Network[pIndex[i]].wlanSignalQuality = (*lppVList)->Network[pIndex[k]].wlanSignalQuality;
					}

					for (DWORD l=k+1;l<dwNetworkNum;++l)
					{
						pIndex[l-1]=pIndex[l];
					}

					--k;//�ٴμ�����ƹ��������Ƿ�͵�ǰ���ظ�
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

		//-----------������������--------------
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

		//�Ѿ����ӵ������ᵽ��һλ
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
		//�ų��Ѿ����ӵ�����
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
		//�ų��Ѿ����ӵ�����
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
		if (0==dwNetworkNum)
		{
			invoker.WlanFreeMemory(*lppVList);
			*lppVList=NULL;
		}
		else
		{
			*pSortedIndex=pIndex2;
		}

		//--------------------------������������ end--------------------------
	}

	invoker.WlanCloseHandle(hClient,NULL);

	return dwNetworkNum!=0;

}

//���ܣ��ͷſɼ������б�������б�����������ռ�õĻ�����
//������pVList�ɼ������б�pSortedIndex���������б�
//����ֵ���ͷųɹ�����TRUE�����򷵻�FASLE��

BOOL FreeVisibleNetworkList(PWLAN_AVAILABLE_NETWORK_LIST pVList,int* pSortedIndex)
{
	DWORD  dwVersion=0;
	HANDLE hClient=NULL;
	CWlanApiInvoker invoker;
	if (!invoker.WlanOpenHandle(WLAN_API_VERSION,NULL,&dwVersion,&hClient))
	{
		return FALSE;
	}
	invoker.WlanFreeMemory(pVList);
	delete [] pSortedIndex;

	invoker.WlanCloseHandle(hClient,NULL);

	return TRUE;
}


//���ܣ���ȡָ��������ŵ�
//������lpNicGuidָ����������
//		pDot11Ssidָ�����������SSID
//		dot11BssTypeָ����������
//		bSecurityEnabledָ���Ƿ����ð�ȫ
//����ֵ�������ŵ�
//�ýӿ�ֻ�ܹ�����vista���Ժ��ϵͳ��
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

	CWlanApiInvoker invoker;

	if (invoker.WlanOpenHandle(WLAN_API_VERSION,NULL,&dwVersion,&hClient))
	{
		if (invoker.WlanGetNetworkBssList(hClient,&NicGuid,pDot11Ssid,dot11BssType,bSecurityEnabled,NULL,&pWlanBssList))
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


	if (pWlanBssList!=NULL)
	{
		invoker.WlanFreeMemory(pWlanBssList);
		pWlanBssList=NULL;
	}

	if (hClient!=NULL)
	{
		invoker.WlanCloseHandle(hClient,NULL);
	}

	return iRet;
}


//���ܣ���ȡָ��������ŵ�
//������lpNicGuidָ����������
//		lpSsidָ�����������SSID
//����ֵ�������ŵ�
//�ýӿ�������winXPϵͳ
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
		VirtualFree(pBSSIDList,sizeof( NDIS_802_11_BSSID_LIST)*NUMBEROF_BSSIDS,0);
		pBSSIDList =NULL;
	}

	return iChannel;
}

//execute command line with privilege under vista and win7
BOOL Execute_Command_Privilege(LPCTSTR lpCmdLine)
{
	if(NULL==lpCmdLine || _tcslen(lpCmdLine)==0)
	{
		return FALSE;
	}

	TCHAR chCmd[512] = {0};
	wsprintf(chCmd,TEXT("/c %s"),lpCmdLine);
	SHELLEXECUTEINFO sei = {sizeof(SHELLEXECUTEINFO)};
	if(GetOsVersion()>=6)
	{
		// Ask for privileges elevation.
		sei.lpVerb = TEXT("runas");
	}
	else
	{
		sei.lpVerb = TEXT("open");
	}
	sei.lpFile = TEXT("cmd");
	sei.lpParameters = chCmd;
	sei.fMask  |=SEE_MASK_NOCLOSEPROCESS;

	if (!ShellExecuteEx(&sei))
	{
	   DWORD dwStatus = GetLastError();
	   if (dwStatus == ERROR_CANCELLED)
	   {
		  // The user refused to allow privileges elevation.
	   }
	   else if (dwStatus == ERROR_FILE_NOT_FOUND)
	   {
		 // The file defined by lpFile was not found and
		 // an error message popped up.
	   }
	   return FALSE;
	}

	if (WAIT_OBJECT_0!=WaitForSingleObject(sei.hProcess,20000))
	{
		CloseHandle(sei.hProcess);
		return FALSE;
	}

	CloseHandle(sei.hProcess);
	return TRUE;		
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

	service_handle = OpenService(scm_handle,service_name,SERVICE_ALL_ACCESS);
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

	service_handle = OpenService(scm_handle,service_name,SERVICE_ALL_ACCESS);
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

BOOL is_longoperation_canceled()
{
	assert(hflag_guard != NULL);
	WaitForSingleObject(hflag_guard,INFINITE);
	BOOL bresult = (g_longoperation_interruptable_flag_addr && *g_longoperation_interruptable_flag_addr);
	ReleaseMutex(hflag_guard);
	return bresult;
}

DWORD sleep_interruptable(DWORD dwMillionseconds)
{
	assert ( dwMillionseconds > 0 );

	DWORD times		= dwMillionseconds / 100;
	DWORD remainder = dwMillionseconds % 100;


	while(times-- > 0)
	{
		if (is_longoperation_canceled())
		{
			//canceled
			return (times * 100 + remainder);
		}
		Sleep(100);
	}


	if(remainder > 0)
	{
		if (is_longoperation_canceled())
		{
			//canceled
			return remainder;
		}

		Sleep(remainder);
	}

	return 0;
}