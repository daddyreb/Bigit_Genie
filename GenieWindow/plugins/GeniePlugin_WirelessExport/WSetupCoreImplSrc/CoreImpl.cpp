#include "CoreImpl.h"
#include "codec.h"

#include <Wincrypt.h>
#include <malloc.h>
#include <vector>


#pragma comment(lib,"wlanapi.lib")
#pragma comment(lib,"Rpcrt4.lib")
#pragma comment(lib,"Crypt32.lib")


bool get_profile(const wchar_t *nicguid,const wchar_t *profilename,wchar_t **profilexml)
{
	HANDLE	hclient				=NULL;
	DWORD	dwservice_version	=0;
	bool	bret				=false;
	UUID	guid;

	*profilexml=NULL;

	if (NULL==nicguid || UuidFromStringW((RPC_WSTR)nicguid,&guid)!=RPC_S_OK
		|| NULL==profilename || NULL==profilexml)
	{
		return bret;
	}
	
	if (WlanOpenHandle(WLAN_API_VERSION,NULL,&dwservice_version,&hclient)!=ERROR_SUCCESS)
	{
		return bret;
	}

	if (WlanGetProfile(hclient,&guid,profilename,NULL,profilexml,NULL,NULL)==ERROR_SUCCESS)
	{
		bret=true;
	}
	
	if (hclient!=NULL)
	{
		WlanCloseHandle(hclient,NULL);
	}

	return bret;
}

void free_profilexml_buffer(wchar_t *profile_buffer)
{
	if (profile_buffer!=NULL)
	{
		WlanFreeMemory(profile_buffer);
	}
}

bool get_profile_fromstorage(const wchar_t *filename,std::wstring &profilexml)
{
	bool	bret		= false;
	HANDLE	hfile		= INVALID_HANDLE_VALUE;

	if (NULL==filename)
	{
		return bret;
	}

	hfile=CreateFile(filename,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	
	if (INVALID_HANDLE_VALUE==hfile)
	{
		return bret;
	}

	DWORD	dw_size		= GetFileSize(hfile,NULL);
	if (dw_size>10240)//file maximum size 10KB
	{
		CloseHandle(hfile);
		return bret;
	}

	DWORD	dw_bytes	= dw_size;
	DWORD	dw_readed	= 0;
	char	*buffer		= new char[dw_bytes+1];

	bret	= true;
	char *p = buffer;
	while(dw_size!=0)
	{
		if (ReadFile(hfile,p,dw_size,&dw_readed,NULL))
		{
			dw_size	-= dw_readed;
			p		+= dw_readed;
		}
		else
		{
			bret = false;
			break;
		}
	}

	if (hfile!=INVALID_HANDLE_VALUE)
	{
		CloseHandle(hfile);
	}

	if (bret)
	{
		bret = false;
		int len=MultiByteToWideChar(CP_ACP,0,buffer,dw_bytes,NULL,0);
		if (len!=0)
		{
			wchar_t *wchbuffer = new wchar_t[len];
			if (wchbuffer)
			{
				if (MultiByteToWideChar(CP_ACP,0,buffer,dw_bytes,wchbuffer,len)==len)
				{
					profilexml = wchbuffer;
					bret = true;
				}				
				delete [] wchbuffer;
			}	   		
		}
	}

	delete [] buffer;

	return bret;
	
}

bool get_password_fromprofile_new(const wchar_t *profilename,std::wstring &pwd,
								  bool &bnetworkkey_or_passphrase,bool *protect)
{
	bool bret = false;
	
	if (NULL==profilename || NULL==protect)
	{
		return bret;
	}
	std::wstring str_appdir;
	if (!get_app_dir(str_appdir))
	{
		return bret;
	}

	//copy temp file to root directory
	str_appdir = str_appdir.substr(0,3);

	std::wstring netshcmd = L"netsh wlan export profile name=\"";
	netshcmd+= profilename;
	netshcmd+= L"\"";
	netshcmd+= L" folder=";
	netshcmd+= str_appdir;
	netshcmd+= L" key=clear";

	wchar_t *netshcmdpath = new wchar_t[netshcmd.length()+1];
	wcscpy(netshcmdpath,netshcmd.c_str());

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si,sizeof(si));
	ZeroMemory(&pi,sizeof(pi));
	si.cb=sizeof(pi);

	si.dwFlags |=STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	
	if (!CreateProcess(NULL,netshcmdpath,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi))
	{
		delete [] netshcmdpath;
		return bret;
	}

	delete [] netshcmdpath;

	if (WAIT_OBJECT_0!=WaitForSingleObject(pi.hProcess,10000))
	{
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return bret;
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	//----------------------------ShellExecuteEx version------------------------------
	//std::wstring netshcmd = L"/c netsh wlan export profile name=\"";
	//netshcmd+= profilename;
	//netshcmd+= L"\"";
	//netshcmd+= L" folder=";
	//netshcmd+= str_appdir;
	//netshcmd+= L" key=clear";

	//wchar_t *netshcmdpath = new wchar_t[netshcmd.length()+1];
	//wcscpy(netshcmdpath,netshcmd.c_str());

	//SHELLEXECUTEINFO sei = {sizeof(SHELLEXECUTEINFO)};
	//// Ask for privileges elevation.
	//sei.lpVerb = L"runas";
	//sei.lpFile = L"cmd";
	//sei.lpParameters = netshcmdpath;
	//sei.fMask  |=SEE_MASK_NOCLOSEPROCESS;

	//if (!ShellExecuteEx(&sei))
	//{
	//   DWORD dwStatus = GetLastError();
	//   if (dwStatus == ERROR_CANCELLED)
	//   {
	//	  // The user refused to allow privileges elevation.
	//   }
	//   else if (dwStatus == ERROR_FILE_NOT_FOUND)
	//   {
	//	 // The file defined by lpFile was not found and
	//	 // an error message popped up.
	//   }

	//   delete [] netshcmdpath;
	//   return bret;
	//}

	//delete [] netshcmdpath;

	//if (WAIT_OBJECT_0!=WaitForSingleObject(sei.hProcess,10000))
	//{
	//	CloseHandle(sei.hProcess);
	//	return bret;
	//}
	//CloseHandle(sei.hProcess);
	//----------------------------end ShellExecuteEx version--------------------------

	WIN32_FIND_DATA fdata;
	std::wstring	filename_tofind=str_appdir;
	filename_tofind+=L"*-";
	filename_tofind+=profilename;
	filename_tofind+=L".xml";
	HANDLE hfine_find = FindFirstFile(filename_tofind.c_str(),&fdata);
	if (INVALID_HANDLE_VALUE==hfine_find)
	{
		return bret;
	}

	do
	{
		if (fdata.nFileSizeLow>10*1024)
		{
			continue;
		}

		std::wstring filepath		= str_appdir+fdata.cFileName;
		std::wstring xmlprofile;
		std::wstring temp_pwd;
		bool		 temp_protect	= false;
		if (get_profile_fromstorage(filepath.c_str(),xmlprofile))
		{
			if (get_password_fromprofile(xmlprofile.c_str(),temp_pwd,bnetworkkey_or_passphrase,&temp_protect))
			{
				pwd			= temp_pwd;
				*protect	= temp_protect;
				bnetworkkey_or_passphrase = true;//vista win7 netsh always get a plain text network key
				bret		= true;
				DeleteFile(filepath.c_str());
				break;
			}
		}
	}while(FindNextFile(hfine_find,&fdata));

	FindClose(hfine_find);

	return bret;
}


bool get_ssid_profilenames(const wchar_t *nicguid,const wchar_t *ssid,std::list<std::wstring> &profile_names)
{
	HANDLE	hclient							= NULL;
	DWORD	dwservice_version				= 0;
	bool	bret							= false;
	UUID	guid;
	wchar_t chssid[DOT11_SSID_MAX_LENGTH+1]	= {0};

	PWLAN_AVAILABLE_NETWORK_LIST pvlist		= NULL;

	if (NULL==nicguid || NULL==ssid || UuidFromStringW((RPC_WSTR)nicguid,&guid)!=RPC_S_OK)
	{
		return bret;
	}
	
	if (WlanOpenHandle(WLAN_API_VERSION,NULL,&dwservice_version,&hclient)!=ERROR_SUCCESS)
	{
		return bret;
	}

	if (WlanGetAvailableNetworkList(hclient,&guid,0,NULL,&pvlist)==ERROR_SUCCESS)
	{
		for (DWORD k=0;k<pvlist->dwNumberOfItems;++k)
		{
			ssid2stringw(chssid,DOT11_SSID_MAX_LENGTH+1,&pvlist->Network[k].dot11Ssid);
			if (!wcscmp(ssid,chssid))
			{
				if (pvlist->Network[k].dwFlags & WLAN_AVAILABLE_NETWORK_HAS_PROFILE)
				{
					profile_names.push_back(pvlist->Network[k].strProfileName);
					bret = true;
				}
			}
		}
	}

	WlanFreeMemory(pvlist);

	if (hclient!=NULL)
	{
		WlanCloseHandle(hclient,NULL);
	}

	return bret;
}

bool get_password_fromprofile(const wchar_t *xmlprofile,std::wstring &pwd
							  ,bool &bnetworkkey_or_passphrase,bool *protect)
{
	bool			bret			= false;
	const wchar_t	*pbegin			= NULL;
	const wchar_t	*pend			= NULL;
	const wchar_t	*pxmlend		= NULL;
	int				size			= 0;

	pwd.clear();
	*protect = false;

	if (NULL==xmlprofile || wcslen(xmlprofile)==0)
	{
		return bret;
	}

	if (wcsstr(xmlprofile,L"WLANProfile")==NULL || wcsstr(xmlprofile,L"</WLANProfile>")==NULL)
	{
		return bret;
	}

	pxmlend	= xmlprofile+wcslen(xmlprofile);

	//--------------get authentication method----------------------------------------
	pbegin  = wcsstr(xmlprofile,L"<authentication>");
	if (NULL==pbegin)
	{
		return bret;
	}
	pbegin +=wcslen(L"<authentication>");
	if (pbegin>pxmlend)
	{
		return false;
	}
	pend = wcsstr(pbegin,L"</authentication>");
	if (NULL==pend)
	{
		return false;
	}
	if ((!wcsnicmp(pbegin,L"WPAPSK",pend-pbegin))
		|| (!wcsnicmp(pbegin,L"WPA2PSK",pend-pbegin)))
	{
		bnetworkkey_or_passphrase = false;
	}
	else
	{
		bnetworkkey_or_passphrase = true;
	}

	//--------------get password and protect attribute--------------------------------
	pbegin	= wcsstr(xmlprofile,L"<sharedKey>");
	if (NULL==pbegin)//no password
	{
		bret = true;
		return bret;
	}
	
	pbegin = wcsstr(pbegin,L"<protected>");
	if (NULL==pbegin)
	{
		return bret;
	}

	pbegin += wcslen(L"<protected>");
	if (pbegin>pxmlend)
	{
		return bret;
	}

	pend	= wcsstr(pbegin,L"</protected>");
	if (NULL==pend)
	{
		return bret;
	}
	
	size = pend-pbegin;//wide char number
	if (size<=0)
	{
		return bret;
	}

	if (!wcsnicmp(pbegin,L"false",size))
	{
		*protect = false;
	}
	else if(!wcsnicmp(pbegin,L"true",size))
	{
		*protect = true;
	}
	else
	{
		return bret;
	}

	pbegin = wcsstr(pend,L"<keyMaterial>");
	if (NULL==pbegin)
	{
		return bret;
	}
	pbegin +=wcslen(L"<keyMaterial>");
	if (pbegin>pxmlend)
	{
		return bret;
	}
	pend = wcsstr(pbegin,L"</keyMaterial>");
	if (NULL==pend)
	{
		return bret;
	}

	size	= pend-pbegin;
	pwd		= pbegin;

	pwd.resize(size);
	bret	= true;

	return bret;
}

int hex2vlaue(wchar_t ch)
{
	int ret = -1;
	if (ch>=L'0' && ch <=L'9')
	{
		ret = ch-48;
	}
	else if (ch>=L'A' && ch<=L'F')
	{
		ret = ch-55;
	}
	else if (ch>=L'a' && ch<=L'f')
	{
		ret = ch-87;
	}
	
	return ret;
}

BOOL StringToBinary(LPCWSTR pszData, PBYTE pbData, PDWORD pcbData)
{
	_ASSERT(pszData && pbData && pcbData);

	int nLen = wcslen(pszData);

	if (*pcbData < (DWORD)(nLen/2) )
		return FALSE;

	LPCWSTR pszCur = pszData;
	int j;
	for (j=0; j<nLen/2; j++)
	{
		BYTE b = 0;
		for (int i=0; i<2; i++)
		{
			if ((*pszCur >= L'0') && (*pszCur <= L'9'))
				b = (b << 4) | (*pszCur - L'0');
			else if ((*pszCur >= L'A') && (*pszCur <= L'F'))
				b = (b << 4) | (*pszCur - L'A') + 10;
			else if ((*pszCur >= L'a') && (*pszCur <= L'f'))
				b = (b << 4) | (*pszCur - L'a') + 10;
			else
				return FALSE;

			pszCur++;
		}

		pbData[j] = b;
	}

	*pcbData = j;

	return TRUE;
}

bool decrypt_password(const wchar_t *pwd,std::wstring &out_pwd)
{
	bool			bret			= false;
	BYTE			*temp_bytes		= NULL;
	DWORD			hex_num			= 0;
	const wchar_t	*ptopwd			= NULL;
	LPWSTR			pDescrOut		= NULL;


	out_pwd.clear();

	DATA_BLOB pwd_in;
	DATA_BLOB pwd_out;

	if (NULL==pwd || wcslen(pwd)==0 || wcslen(pwd)%2!=0)
	{
		return bret;
	}
	
	hex_num		= wcslen(pwd)/2;
	temp_bytes	= (BYTE*)malloc(hex_num);
	/*ptopwd		= pwd;
	for (int i=0;i<hex_num;++i)
	{
		temp_bytes[i] = hex2vlaue(*ptopwd)*16+hex2vlaue(*(ptopwd+1));
		ptopwd+=2;
	}*/
	if (!StringToBinary(pwd,temp_bytes,&hex_num))
	{
		return bret;
	}

	pwd_in.pbData = temp_bytes;
	pwd_in.cbData = hex_num;

	if (CryptUnprotectData(&pwd_in,&pDescrOut,NULL,NULL,NULL,0,&pwd_out))
	{
		BYTE *ptemp = (BYTE*)malloc(pwd_out.cbData+sizeof(wchar_t));
		memcpy(ptemp,pwd_out.pbData,pwd_out.cbData);
		*((wchar_t*)(ptemp+pwd_out.cbData))=L'\0';
		LocalFree(pwd_out.pbData);
		LocalFree(pDescrOut);

		out_pwd = (wchar_t*)ptemp;
		free(ptemp);
		bret = true;
	}
	
	free(temp_bytes);

	return bret;
}

bool set_profile(const wchar_t *nicguid,const wchar_t *profilexml)
{
	bool	bret				=false;
	DWORD	dwservice_version	=0;
	DWORD	dwreseaon			=0;
	HANDLE	hclient				=NULL;
	UUID	guid;

	if (NULL==nicguid || UuidFromStringW((RPC_WSTR)nicguid,&guid)!=RPC_S_OK || NULL==profilexml)
	{
		return bret;
	}

	if (WlanOpenHandle(WLAN_API_VERSION,NULL,&dwservice_version,&hclient)!=ERROR_SUCCESS)
	{
		return bret;
	}

	if (WlanSetProfile(hclient,&guid,0,profilexml,NULL,TRUE,NULL,&dwreseaon)==ERROR_SUCCESS)
	{
		bret=true;
	}

	if (hclient!=NULL)
	{
		WlanCloseHandle(hclient,NULL);
	}

	return bret;	
}

bool enum_interfaces(std::list<std::wstring> &inf_list)
{
	bool						bret				=false;
	HANDLE						hclient				=NULL;
	DWORD						dwservice_version	=0;
	PWLAN_INTERFACE_INFO_LIST	pinf_list			=NULL;
	RPC_WSTR					str_infguid			=NULL;

	if (WlanOpenHandle(WLAN_API_VERSION,NULL,&dwservice_version,&hclient)!=ERROR_SUCCESS)
	{
		return bret;
	}

	if (WlanEnumInterfaces(hclient,NULL,&pinf_list)==ERROR_SUCCESS)
	{
		for (DWORD k=0;k<pinf_list->dwNumberOfItems;++k)
		{
			if (UuidToStringW(&pinf_list->InterfaceInfo[k].InterfaceGuid,&str_infguid)==RPC_S_OK)
			{
				inf_list.push_back(std::wstring((wchar_t*)str_infguid));
				RpcStringFreeW(&str_infguid);
			}
		}

		bret=true;

		if (pinf_list!=NULL)
		{
			WlanFreeMemory(pinf_list);
		}
	}

	if (hclient!=NULL)
	{
		WlanCloseHandle(hclient,NULL);
	}

	return bret;
}

bool get_connected_ssid_profilename(const wchar_t *nicguid,std::wstring &ssid,std::wstring &profilename)
{
	bool							bret								=false;
	HANDLE							hclient								=NULL;
	DWORD							dwservice_version					=0;
	UUID							infguid;
	PWLAN_AVAILABLE_NETWORK_LIST	pnetwork_list						=NULL;
	wchar_t							ssid_buf[DOT11_SSID_MAX_LENGTH+1]	={0};

	if (NULL==nicguid || UuidFromStringW((RPC_WSTR)nicguid,&infguid)!=RPC_S_OK)
	{
		return bret;
	}

	if (WlanOpenHandle(WLAN_API_VERSION,NULL,&dwservice_version,&hclient)!=ERROR_SUCCESS)
	{
		return bret;
	}
	
	if (WlanGetAvailableNetworkList(hclient,&infguid,0,NULL,&pnetwork_list)==ERROR_SUCCESS)
	{
		for (DWORD i=0;i<pnetwork_list->dwNumberOfItems;++i)
		{
			if (pnetwork_list->Network[i].dwFlags&WLAN_AVAILABLE_NETWORK_CONNECTED)
			{
				ssid2stringw(ssid_buf,DOT11_SSID_MAX_LENGTH+1,&pnetwork_list->Network[i].dot11Ssid);
				ssid		=ssid_buf;
				profilename	=pnetwork_list->Network[i].strProfileName;
				bret=true;
				break;
			}
		}
		WlanFreeMemory(pnetwork_list);
	}

	if (hclient!=NULL)
	{
		WlanCloseHandle(hclient,NULL);
	}

	return bret;
}

bool build_config_string(const wchar_t *ssid,const wchar_t *pwd
						 ,bool bnetworkKey_or_passPhrase,std::wstring &cfg_str)
{
	bool bret = false;

	if (NULL==ssid || NULL==pwd ||wcslen(ssid)==0)
	{
		return bret;
	}
	
	cfg_str =  ssid;
	cfg_str += L"\n";
	cfg_str += pwd;
	cfg_str += L"\n";
	cfg_str += (bnetworkKey_or_passPhrase?L"true":L"false");

	bret	= true;
	return bret;
}

bool parse_config_string(const wchar_t *cfg_str,std::wstring &ssid,std::wstring &pwd,bool &bnetworkKey_or_passPhrase)
{
	const wchar_t *ptemp	= NULL;
	const wchar_t *ptemp_s	= NULL;
	const wchar_t *pend		= NULL;

	if (NULL==cfg_str || wcslen(cfg_str)==0)
	{
		return false;
	}

	pend  = cfg_str+wcslen(cfg_str);//pend pointer to the terminated L'\0'
	ptemp = wcsstr(cfg_str,L"\n");
	if (NULL== ptemp)
	{
		return false;
	}

	ssid = cfg_str;
	ssid.resize(ptemp-cfg_str);

	++ptemp;//skip '\n'
	if (ptemp>pend)//out of boundary
	{
		return false;
	}

	ptemp_s = wcsstr(ptemp,L"\n");
	if (NULL==ptemp_s)
	{
		return false;
	}
	pwd = ptemp;
	pwd.resize(ptemp_s-ptemp);

	++ptemp_s;//skip '\n'
	if(ptemp_s > pend)
	{
		return false;
	}

	if (!wcsicmp(ptemp_s,L"true"))
	{
		bnetworkKey_or_passPhrase = true;
	}
	else if(!wcsicmp(ptemp_s,L"false"))
	{
		bnetworkKey_or_passPhrase = false;
	}
	else
	{
		return false;
	}

	return true;
}
//--------------------------------------导出文件中加入了认证和加密方式-----------------------------
bool build_config_string_v2(const wchar_t *ssid,const wchar_t *pwd,bool bnetworkKey_or_passPhrase,
							const wchar_t *auth,const wchar_t *cipher,std::wstring &cfg_str)
{
	bool bret = false;

	if (NULL==ssid || NULL==pwd || wcslen(ssid)==0 ||
		NULL==auth || NULL==cipher)
	{
		return bret;
	}
	
	cfg_str =  ssid;
	cfg_str += L"\n";
	cfg_str += pwd;
	cfg_str += L"\n";
	cfg_str += (bnetworkKey_or_passPhrase?L"true":L"false");

	cfg_str += L"\n";
	cfg_str += auth;
	cfg_str += L"\n";
	cfg_str += cipher;

	bret	= true;
	return bret;
}

bool parse_config_string_v2(const wchar_t *cfg_str,std::wstring &ssid,std::wstring &pwd,bool &bnetworkKey_or_passPhrase,
							std::wstring &auth,std::wstring &cipher)
{
	const wchar_t *ptemp	= NULL;
	const wchar_t *ptemp_s	= NULL;
	const wchar_t *pend		= NULL;

	std::vector<std::wstring> list_str;

	if (NULL==cfg_str || wcslen(cfg_str)==0)
	{
		return false;
	}

	pend  = cfg_str+wcslen(cfg_str);//pend pointer to the terminated L'\0'
	ptemp = cfg_str;

	while(ptemp && (ptemp < pend))
	{
		std::wstring   tempstr(ptemp);
		ptemp_s = wcsstr(ptemp,L"\n");
		if(ptemp_s != NULL)
		{
			tempstr.resize(ptemp_s - ptemp);
			++ptemp_s;//skip '\n'
		}
		list_str.push_back(tempstr);

		ptemp = ptemp_s;
	}

	if(list_str.size()!=5)
	{
		return false;
	}
	
	if (!wcsicmp(list_str[2].c_str(),L"true"))
	{
		bnetworkKey_or_passPhrase = true;
	}
	else if(!wcsicmp(list_str[2].c_str(),L"false"))
	{
		bnetworkKey_or_passPhrase = false;
	}
	else
	{
		return false;
	}

	ssid	= list_str[0];
	pwd		= list_str[1];
	auth	= list_str[3];
	cipher	= list_str[4];

	return true;
}


bool import_profile(const wchar_t *nicguid,const wchar_t *ssid,const wchar_t *pwd,bool bnetworkKey_or_passPhrase,
					const wchar_t *auth,const wchar_t *cipher)
{
	return SetInterfaceProfile(nicguid,ssid,auth,cipher,pwd,bnetworkKey_or_passPhrase);
}

bool tryconnect_afterimport(const wchar_t *nicguid,const wchar_t *ssid)
{
	HANDLE							hClient			= NULL;
	GUID							NicGuid;
	WLAN_CONNECTION_PARAMETERS		wlanConnPara;
	DWORD							dwVersion		= 0;
	bool							bRet			= false;


	wlanConnPara.wlanConnectionMode = wlan_connection_mode_profile;
	wlanConnPara.dot11BssType		= dot11_BSS_type_infrastructure;
	wlanConnPara.pDot11Ssid			= NULL;
	wlanConnPara.pDesiredBssidList	= NULL;
	wlanConnPara.dwFlags			= 0;
	wlanConnPara.strProfile			= NULL;


	if (NULL==nicguid||0==wcslen(nicguid)||
		UuidFromString((RPC_WSTR)nicguid,&NicGuid)!=RPC_S_OK 
		|| NULL==ssid)
	{
		return bRet;
	}

	if (WlanOpenHandle(WLAN_API_VERSION,NULL,&dwVersion,&hClient)!=ERROR_SUCCESS)
	{
		return bRet;
	}


	wlanConnPara.strProfile=ssid;

	if (WlanConnect(hClient,&NicGuid,&wlanConnPara,NULL)==ERROR_SUCCESS)
	{
		bRet=true;
	}//if
	

	WlanCloseHandle(hClient,NULL);

	return bRet;
}

//--------------------------------------------------结尾-------------------------------------------

bool save_to_file(const wchar_t *filename,const wchar_t *str_config)
{
	bool	bret	=false;
	HANDLE	hfile	=INVALID_HANDLE_VALUE;
	if (NULL==filename || NULL==str_config)
	{
		return bret;
	}
	
	hfile=CreateFile(filename,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if (INVALID_HANDLE_VALUE==hfile)
	{
		return bret;
	}
	
	const char* pdata		=base64_encode_bin((const char*)str_config,(wcslen(str_config)+1)*sizeof(wchar_t));
	DWORD		dw_bytes	=strlen(pdata)+1;
	DWORD		dw_writen	=0;
	const char  *p			=pdata;

	bret=true;
	while (dw_bytes!=0)
	{
		if (WriteFile(hfile,p,dw_bytes,&dw_writen,NULL))
		{
			p+=dw_writen;
			dw_bytes-=dw_writen;
		}
		else
		{	
			bret=false;
			break;
		}
	}

	CloseHandle(hfile);

	return bret;
}

bool load_from_file(const wchar_t *filename,std::wstring &str_config)
{
	bool	bret	=false;
	HANDLE	hfile	=INVALID_HANDLE_VALUE;
	
	str_config.clear();

	if (NULL==filename)
	{
		return bret;
	}

	hfile=CreateFile(filename,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (INVALID_HANDLE_VALUE==hfile)
	{
		return bret;
	}

	DWORD	dw_size		= GetFileSize(hfile,NULL);
	if (dw_size>10240)//file maximum size 10KB
	{
		CloseHandle(hfile);
		return bret;
	}
	DWORD	dw_readed	= 0;
	char	*buffer		= new char[dw_size];

	bret	= true;
	char *p = buffer;
	while(dw_size!=0)
	{
		if (ReadFile(hfile,p,dw_size,&dw_readed,NULL))
		{
			dw_size	-= dw_readed;
			p		+= dw_readed;
		}
		else
		{
			bret = false;
			break;
		}
	}

	if (hfile!=INVALID_HANDLE_VALUE)
	{
		CloseHandle(hfile);
	}

	if (bret)
	{
		str_config = (wchar_t*)base64_decode(buffer);
	}
	
	delete [] buffer;
	return bret;
}

wchar_t* ssid2stringw(wchar_t *buf, int count,PDOT11_SSID pSsid)
{
	int bytes, i;
	bytes = min( count-1, pSsid->uSSIDLength);
	for( i=0; i<bytes; i++)
		mbtowc(&buf[i],(const char *)&pSsid->ucSSID[i],1);
	buf[bytes] = '\0';
	return buf;
}

extern DWORD SetProfile(int argc, const TCHAR *argv[],bool networkKey_or_passPhrase);

bool SetInterfaceProfile(const wchar_t *nicguid,const wchar_t *ssid,
						 const wchar_t *auth,const wchar_t *cipher,const wchar_t *pwd,bool bnetworkKey_or_passPhrase)
{

	int argc=5;
	std::wstring strGuid=L"/NICGUID:";
	strGuid+=nicguid;

	std::wstring strSsid=L"/SSID:";
	strSsid+=ssid;

	std::wstring strAuth=L"/Authentication:";
	strAuth+=auth;

	std::wstring strCipher=L"/Encryption:";
	strCipher+=cipher;

	std::wstring strPwd=L"/NetworkKey:";
	strPwd+=pwd;

	const wchar_t* argv[5]={NULL};
	argv[0]=strGuid.c_str();
	argv[1]=strSsid.c_str();
	argv[2]=strAuth.c_str();
	argv[3]=strCipher.c_str();
	argv[4]=strPwd.c_str();

	return NO_ERROR==SetProfile(5,argv,bnetworkKey_or_passPhrase);
}


//bnetworkKey_or_passPhrase is true the input field of password is networkKey 
//otherwise is passPhrase
bool connect2wlan(const wchar_t *nicguid,const wchar_t *ssid,const wchar_t *pwd,bool bnetworkKey_or_passPhrase)
{
	HANDLE							hClient		= NULL;
	GUID							NicGuid;
	WLAN_CONNECTION_PARAMETERS		wlanConnPara;
	DWORD							dwVersion		= 0;
	DWORD							i				= 0;
	bool							bRet			= false;
	PWLAN_AVAILABLE_NETWORK_LIST	pVList			= NULL;
	WCHAR							strSsid[DOT11_SSID_MAX_LENGTH+1]={0};

	wlanConnPara.wlanConnectionMode = wlan_connection_mode_profile;
	wlanConnPara.dot11BssType		= dot11_BSS_type_infrastructure;
	wlanConnPara.pDot11Ssid			= NULL;
	wlanConnPara.pDesiredBssidList	= NULL;
	wlanConnPara.dwFlags			= 0;
	wlanConnPara.strProfile			= NULL;


	if (NULL==nicguid||0==wcslen(nicguid)||
		UuidFromString((RPC_WSTR)nicguid,&NicGuid)!=RPC_S_OK
		||NULL==pwd || NULL==ssid)
	{
		return bRet;
	}

	if (WlanOpenHandle(WLAN_API_VERSION,NULL,&dwVersion,&hClient)!=ERROR_SUCCESS)
	{
		return bRet;
	}

	if (WlanGetAvailableNetworkList(hClient,&NicGuid,0,NULL,&pVList)!=ERROR_SUCCESS)
	{
		WlanCloseHandle(hClient,NULL);
		return bRet;
	}

	for (i=0;i<pVList->dwNumberOfItems;++i)
	{
		ssid2stringw(strSsid,sizeof(strSsid)/sizeof(WCHAR),&(pVList->Network[i].dot11Ssid));
		if (!wcscmp(strSsid,ssid))
		{
			break;
		}
	}

	bool bProfileOk=SetInterfaceProfile(nicguid,
		strSsid
		,GetAuthAlgoString(pVList->Network[i].dot11DefaultAuthAlgorithm),
		GetCipherAlgoString(pVList->Network[i].dot11DefaultCipherAlgorithm),pwd,bnetworkKey_or_passPhrase);

	if (bProfileOk)
	{
		wlanConnPara.strProfile=strSsid;
		wlanConnPara.dot11BssType=pVList->Network[i].dot11BssType;

		if (WlanConnect(hClient,&NicGuid,&wlanConnPara,NULL)==ERROR_SUCCESS)
		{
			bRet=true;
		}//if
	}

	WlanFreeMemory(pVList);
	pVList=NULL;

	WlanCloseHandle(hClient,NULL);

	return bRet;
}

// get auth algorithm string
const wchar_t *GetAuthAlgoString(DOT11_AUTH_ALGORITHM dot11AuthAlgo)
{
	const wchar_t *strRetCode = L"Unknown algorithm";

	switch(dot11AuthAlgo)
	{
	case DOT11_AUTH_ALGO_80211_OPEN:
		strRetCode = L"open";
		break;
	case DOT11_AUTH_ALGO_80211_SHARED_KEY:
		strRetCode = L"shared";
		break;
	case DOT11_AUTH_ALGO_WPA:
		strRetCode = L"WPA";
		break;
	case DOT11_AUTH_ALGO_WPA_PSK:
		strRetCode = L"WPAPSK";
		break;
	case DOT11_AUTH_ALGO_WPA_NONE:
		strRetCode = L"WPA";
		break;
	case DOT11_AUTH_ALGO_RSNA:
		strRetCode = L"WPA2";
		break;
	case DOT11_AUTH_ALGO_RSNA_PSK:
		strRetCode = L"WPA2PSK";
		break;
	default:
		if (dot11AuthAlgo & DOT11_AUTH_ALGO_IHV_START)
		{
			strRetCode = L"Vendor-specific algorithm";
		}
	}

	return strRetCode;
}

// get cipher algorithm string
const wchar_t *GetCipherAlgoString(DOT11_CIPHER_ALGORITHM dot11CipherAlgo)
{
	const wchar_t *strRetCode = L"Unknown algorithm";

	switch(dot11CipherAlgo)
	{
	case DOT11_CIPHER_ALGO_NONE:
		strRetCode = L"none";
		break;
	case DOT11_CIPHER_ALGO_WEP40:
		strRetCode = L"WEP40";
		break;
	case DOT11_CIPHER_ALGO_TKIP:
		strRetCode = L"TKIP";
		break;
	case DOT11_CIPHER_ALGO_CCMP:
		strRetCode = L"AES";
		break;
	case DOT11_CIPHER_ALGO_WEP104:
		strRetCode = L"WEP104";
		break;
	case DOT11_CIPHER_ALGO_WPA_USE_GROUP:
		strRetCode = L"USE-GROUP";
		break;
	case DOT11_CIPHER_ALGO_WEP:
		strRetCode = L"WEP";
		break;
	default:
		if (dot11CipherAlgo & DOT11_CIPHER_ALGO_IHV_START)
		{
			strRetCode = L"Vendor-specific algorithm";
		}
	}

	return strRetCode;
}



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


bool get_app_dir(std::wstring &dir_path)
{
	bool	bret			= false;
	wchar_t path[MAX_PATH]	= {0};

	if (0==GetModuleFileName(NULL,path,MAX_PATH))
	{
		return bret;
	}

	size_t len = wcslen(path);
	for(size_t i=len-1;i>=0;--i)
	{
		if (path[i]==L'\\')
		{
			path[i+1]	= L'\0';
			dir_path	= path;
			bret		= true;
			break;
		}
	}

	return bret;
}

//----------------------------------------------
bool get_logicaldriver_letters(std::list<std::wstring> &letters)
{
	bool bret = false;

	DWORD size=GetLogicalDriveStrings(0,NULL);
	if (0==size)
	{
		return bret;
	}

	wchar_t *buffer = new wchar_t[size];
	size_t	len		= 0;
	if (GetLogicalDriveStrings(size,buffer))
	{
		wchar_t *p = buffer;
		len = wcslen(p);
		while (len!=0) 
		{
			letters.push_back(p);
			p +=len;
			++p;//skip '\0'
			len = wcslen(p);
		}
		bret = true;
	}
	delete [] buffer;

	return bret;
}

bool is_logicaldriver_removable(const wchar_t* ldriver)
{
	return (NULL==ldriver)?false:(GetDriveType(ldriver)==DRIVE_REMOVABLE);
}

bool is_logicaldriver_cdrom(const wchar_t* ldriver)
{
	return (NULL==ldriver)?false:(GetDriveType(ldriver)==DRIVE_CDROM);
}

bool is_logicaldriver_system(const wchar_t* ldriver)
{
	if (NULL==ldriver)
	{
		return false;
	}

	wchar_t path[MAX_PATH] = {0};
	GetWindowsDirectory(path,MAX_PATH);
	return 0==wcsnicmp(path,ldriver,3);
}

