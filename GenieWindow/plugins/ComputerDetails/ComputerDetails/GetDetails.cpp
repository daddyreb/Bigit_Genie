//#include "ComputerInfo.h"
#include "GetDetails.h"

//CSockLib theSockLib;



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
		}
		RegCloseKey(hSubKey);
	}

	RegCloseKey(hKeyNetwork);
	return res;
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

	if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, KEY_NETWORK_CONNECTIONS, 0, KEY_READ,&hNetKey))
		return FALSE;

	wsprintf(szDataBuf, TEXT("%s\\Connection"),lpNicGuid);
	if(ERROR_SUCCESS != RegOpenKeyEx(hNetKey ,szDataBuf ,0 ,KEY_READ, &hLocalNet))
	{
		RegCloseKey(hNetKey);
		return FALSE;
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

	return bRet;
}





//功能：	获取系统的主版本号和副版本号
//参数：	pdwMinorVersion输出参数，不为NULL时返回副版本号
//返回值：	系统的主版本号

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




typedef BOOL (CALLBACK *PNotifyConfigChange)(
	LPWSTR lpwszServerName, // 本地机器为NULL
	LPWSTR lpwszAdapterName, // 适配器名称
	BOOL   bNewIpAddress, // TRUE表示更改IP
	DWORD  dwIpIndex, // 指明第几个IP地址，如果只有该接口只有一个IP地址则为0
	DWORD  dwIpAddress, // IP地址
	DWORD  dwSubNetMask, // 子网掩码
	int    nDhcpAction ); // 对DHCP的操作 0:不修改, 1:启用 DHCP，2:禁用 DHCP




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
