#include "CoreImp.h"
#include <Rpc.h>
#include <assert.h>

//#include <iostream>

#define NCF_PHYSICAL 0x4

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


//功能：	获取指定网卡的媒体类型
//参数：	strNicGuid (带大括号{})指定要获取媒体类型的网卡
//返回值：	失败返回-1,1有线网卡，2无线网卡，其它值为其它类型的网卡
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
		return 1;//Vista 系统有线网卡没有MediaSubType
	}

	RegCloseKey(hKey);

	return int(dwMedia);
}


//功能：	根据网卡的PnpInstanceId获取网卡对应的GUID
//参数：	网卡的PnpInstanceId,GuidList包含返回的GUID,返回的GUID带大括号{}
//返回值：	调用成功返回TRUE，失败返回FALSE

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

//功能：	判断指定的网卡是不是物理网卡（和虚拟网卡区分）
//参数：	lpNicGuid要进行判断的网卡的GUID,带大括号{}
//返回值：	网卡为物理网卡返回TRUE，否则返回FALSE

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
		dwMedia=1;//Vista 系统下有线网卡没有MediaSubType键值
	}

	if (dwMedia!=0x01&&dwMedia!=2&&dwMedia!=5)
	{
		goto Exit;
	}

	bRet = TRUE;

Exit:
	RegCloseKey(hLocalNet);
	RegCloseKey(hNetKey);

	return (bRet?IsPhysicalNic2(lpNicGuid):bRet);
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


//-------------------win 2003/ vista 下启用禁用网卡------------------------------------
//功能：	返回指定网卡的名称
//参数：	lpNicGuid指定要获取名称的网卡的GUID，
//			strFriName输出参数，返回网卡的名称
//返回值：	获取成功返回TRUE，否则返回FALSE

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

//功能：	启用和禁用网卡
//参数：	lpNicFriName为网卡的名称,可通过GetNicFriendlyName获得
//			bEnable为TRUE启用网卡，FALSE禁用网卡
//返回值：	操作成功返回TRUE，失败返回FALSE
//说明：	该接口内部调用 netsh实现

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


//return :-1:检测失败，0:网关无效,1:网关有效,2网关可通但向外出口无效
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
		//"64.94.70.219"//www.netgear.com
		"208.67.222.222" //kai.yan modify
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
//	设置注册表中的DHCP信息,lpszAdapterName带大括号{}
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
	LPWSTR lpwszServerName, // 本地机器为NULL
	LPWSTR lpwszAdapterName, // 适配器名称
	BOOL   bNewIpAddress, // TRUE表示更改IP
	DWORD  dwIpIndex, // 指明第几个IP地址，如果只有该接口只有一个IP地址则为0
	DWORD  dwIpAddress, // IP地址
	DWORD  dwSubNetMask, // 子网掩码
	int    nDhcpAction ); // 对DHCP的操作 0:不修改, 1:启用 DHCP，2:禁用 DHCP

//通知系统DHCP配置已经改变 ,lpszAdapterName带大括号{}
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




