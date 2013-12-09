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

	return bRet;
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




typedef BOOL (CALLBACK *PNotifyConfigChange)(
	LPWSTR lpwszServerName, // ���ػ���ΪNULL
	LPWSTR lpwszAdapterName, // ����������
	BOOL   bNewIpAddress, // TRUE��ʾ����IP
	DWORD  dwIpIndex, // ָ���ڼ���IP��ַ�����ֻ�иýӿ�ֻ��һ��IP��ַ��Ϊ0
	DWORD  dwIpAddress, // IP��ַ
	DWORD  dwSubNetMask, // ��������
	int    nDhcpAction ); // ��DHCP�Ĳ��� 0:���޸�, 1:���� DHCP��2:���� DHCP




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
