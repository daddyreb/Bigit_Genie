#include "CoreImp.h"
#include "NetcardApi.h"

#include <setupapi.h> 
#include <cfgmgr32.h>

#include <Iphlpapi.h>
#include <Rpc.h>

#include <winioctl.h>
#include <ntddndis.h>
#include <assert.h>

#pragma comment (lib,"setupapi")
#pragma comment (lib,"Rpcrt4")
#pragma comment(lib,"Ws2_32")
#pragma comment(lib,"Iphlpapi")


//功能：	枚举机器上的所有的物理网卡(不包括虚拟网卡)
//参数：	NIC结构体的列表，调用成功包含所有的物理网卡的相关系统
//返回值：	调用成功返回TRUE，否则返回FALSE
//调用成功后必须调用FreeNicListBuffer释放内存
extern "C" DIAGNOSEDLL_API BOOL __stdcall EnumInterfaces(NICLIST **Niclist)
{
	if (NULL==Niclist)
	{
		return FALSE;
	}
	else
	{
		*Niclist=NULL;
	}

	HDEVINFO hDevInfo=INVALID_HANDLE_VALUE;

	if (INVALID_HANDLE_VALUE == (hDevInfo =SetupDiGetClassDevs(&GUID_NDIS_LAN_CLASS/*NULL*/,NULL,NULL,	DIGCF_DEVICEINTERFACE/*DIGCF_PRESENT |DIGCF_ALLCLASSES*/))) 
	{ 
		return FALSE; 
	} 

	Garbage DevListGC(hDevInfo);

	DWORD	i		=0, 
		Status	=0,
		Problem	=0;
	NIC		CurNic;

	SP_DEVINFO_DATA DeviceInfoData	={sizeof(SP_DEVINFO_DATA)}; 

	TCHAR			Buffer[MY_BUFFER_SIZE]	={0};
	DWORD			Length			=sizeof(Buffer);

	std::list<NIC> nics_list;

	for (i=0;SetupDiEnumDeviceInfo(hDevInfo,i,&DeviceInfoData);i++) 
	{ 
		if (CM_Get_DevNode_Status(&Status, &Problem, DeviceInfoData.DevInst,0) == CR_SUCCESS)
		{
			if (((Status & DN_NO_SHOW_IN_DM)==0)&&(!IsClassHidden(&DeviceInfoData.ClassGuid)))
			{
				CurNic.m_dwId=i;

				if (SetupDiGetDeviceRegistryProperty(hDevInfo,   
					&DeviceInfoData, SPDRP_DEVICEDESC, NULL,(PBYTE)Buffer, Length, &Length))   
				{   
					Buffer[Length]=TEXT('\0');
					//CurNic.m_strDevDesc=Buffer;
					_tcscpy(CurNic.m_chDevDesc,Buffer);
					Length=sizeof(Buffer);
				}
				else
				{
					//CurNic.m_strDevDesc=TEXT("Nic without description");
					_tcscpy(CurNic.m_chDevDesc,TEXT("Nic without description"));
				}

				CurNic.m_bDisabled=(Status & DN_HAS_PROBLEM) && (CM_PROB_DISABLED == Problem);
				CurNic.m_dwNicType=0;
				//CurNic.m_strDevGuid.clear();
				CurNic.m_chDevGuid[0]=TEXT('\0');

				DWORD dwSizeRequired = 0;
				TCHAR szPnpInstanceId[MY_BUFFER_SIZE+1];

				if (SetupDiGetDeviceInstanceId(hDevInfo, &DeviceInfoData, 
					szPnpInstanceId, MY_BUFFER_SIZE, &dwSizeRequired))
				{
					//std::list<TString>	GuidList;
					TString				NicGuid;
					int					iMedia		=0;
					if (GetNicGuid(szPnpInstanceId,/*GuidList*/NicGuid)&&(!/*GuidList*/NicGuid.empty()))
					{
						//CurNic.m_strDevGuid=GuidList.front();
						_tcscpy(CurNic.m_chDevGuid,/*GuidList.front().c_str()*/NicGuid.c_str());

						if ((iMedia=GetNicMediaType(/*GuidList.front()*/NicGuid))>0)
						{
							CurNic.m_dwNicType=iMedia;
						}
					}

				}

				if (IsPhysicalNic(/*CurNic.m_strDevGuid.c_str()*/CurNic.m_chDevGuid))
				{
					//CurNic.m_strDevGuid=CurNic.m_strDevGuid.substr(1,36);//去掉GUID的大括号
					TString strDevGuid=CurNic.m_chDevGuid;
					_tcscpy(CurNic.m_chDevGuid,strDevGuid.substr(1,REG_GUID_STR_LEN).c_str());//去掉GUID的大括号
					nics_list.push_back(CurNic);
				}

			}//if
		}//if
	} //for


	if (nics_list.size()!=0)
	{
		*Niclist		= new NICLIST;
		NICLIST *lpTemp = *Niclist;
		lpTemp->dwCount	= nics_list.size();
		lpTemp->lpNics	= new NIC[lpTemp->dwCount];
		int k=0;
		for (std::list<NIC>::iterator i=nics_list.begin();i!=nics_list.end();++i,++k)
		{
			lpTemp->lpNics[k].m_bDisabled	=i->m_bDisabled;
			lpTemp->lpNics[k].m_dwId		=i->m_dwId;
			lpTemp->lpNics[k].m_dwNicType	=i->m_dwNicType;
			_tcscpy(lpTemp->lpNics[k].m_chDevDesc,i->m_chDevDesc);
			_tcscpy(lpTemp->lpNics[k].m_chDevGuid,i->m_chDevGuid);
		}
		return TRUE;
	}
	else
	{
		*Niclist=NULL;
		return FALSE;
	}
}

extern "C" DIAGNOSEDLL_API void __stdcall FreeNicListBuffer(NICLIST *niclist)
{
	if (0 == niclist)
	{
		return;
	}

	delete [] niclist->lpNics;
	delete	  niclist;
}

//---------------------------------------------------------------------------   
//功能：	启用与禁用网卡  
//参数：	lpNicGuid是要进行状态改变的网卡的GUID未带大括号
//			Enabled     TRUE = 启用     FALSE = 禁用   
//返回值：	成功启用或禁用网卡返回TRUE，否则返回FALSE

//---------------------------------------------------------------------------   
extern "C" DIAGNOSEDLL_API BOOL __stdcall NetCardStateChange(LPCTSTR lpNicGuid, BOOL Enabled)   
{  
	if(NULL==lpNicGuid)
	{
		return FALSE;
	}
	//-----------------------------win 2003 /vista 实现------------------------
	DWORD dwOSMinorVersion=0;
	DWORD dwOsMajorVersion=GetOsVersion(&dwOSMinorVersion);

	if(((dwOsMajorVersion==5)&&(dwOSMinorVersion>=2))||dwOsMajorVersion==6)
	{
		TString strFriName;
		if (GetNicFriendlyName(lpNicGuid,strFriName))   
		{   
			if(NetCardStateChangeNetSh(strFriName.c_str(),Enabled))
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}
	//---------------------------------------end--------------------------------------------

	DWORD			DeviceId		= 0;   
	HDEVINFO		hDevInfo		= 0;  
	SP_DEVINFO_DATA DeviceInfoData	= {sizeof(SP_DEVINFO_DATA)};   
	DWORD			Status			= 0, 
		Problem			= 0,
		dwSizeRequired	= 0;
	BOOL			bGetNic			= FALSE;

	TCHAR			szPnpInstanceId[MY_BUFFER_SIZE]	={0};
	TCHAR			Buffer[MY_BUFFER_SIZE]			={0};
	DWORD			Length							=sizeof(Buffer);

	GUID			NicGuid,TempGuid;
	RPC_STATUS		status;

	if (UuidFromString((RPC_TSTR)lpNicGuid,&NicGuid)!=RPC_S_OK)
	{
		return FALSE;
	}

	if (INVALID_HANDLE_VALUE == (hDevInfo =	SetupDiGetClassDevs(&GUID_NDIS_LAN_CLASS/*NULL*/,NULL,NULL,DIGCF_DEVICEINTERFACE/*	DIGCF_PRESENT |DIGCF_ALLCLASSES*/)))   
		return FALSE;

	Garbage DevListGC(hDevInfo);

	for (DeviceId=0;SetupDiEnumDeviceInfo(hDevInfo,DeviceId,&DeviceInfoData);++DeviceId)
	{
		if (SetupDiGetDeviceInstanceId(hDevInfo, &DeviceInfoData, 
			szPnpInstanceId, MY_BUFFER_SIZE, &dwSizeRequired))
		{
			std::list<TString>	GuidList;
			if (GetNicGuid(szPnpInstanceId,GuidList)&&(!GuidList.empty()))
			{
				if (UuidFromString((RPC_TSTR)GuidList.front().substr(1,36).c_str(),&TempGuid)==RPC_S_OK)
				{
					if (UuidEqual(&NicGuid,&TempGuid,&status))
					{
						bGetNic=TRUE;
						break;
					}
				}
			}//if
		}//if
	}//for

	if (!bGetNic)
	{
		return FALSE;
	}

	if (CM_Get_DevNode_Status(&Status, &Problem,DeviceInfoData.DevInst,0) != CR_SUCCESS)   
		return FALSE;   

	SP_PROPCHANGE_PARAMS PropChangeParams;
	PropChangeParams.ClassInstallHeader.cbSize			= sizeof(SP_CLASSINSTALL_HEADER);
	PropChangeParams.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;   
	PropChangeParams.Scope								= DICS_FLAG_CONFIGSPECIFIC/*DICS_FLAG_GLOBAL*/;
	PropChangeParams.HwProfile							= 0;

	if (Enabled)   
	{   
		if (!((Status & DN_HAS_PROBLEM) && (CM_PROB_DISABLED == Problem)))   
		{    
			return FALSE;   
		}   
		PropChangeParams.StateChange = DICS_ENABLE;   
	}   
	else   
	{   
		if ((Status & DN_HAS_PROBLEM) && (CM_PROB_DISABLED == Problem))   
		{     
			return FALSE;   
		}   
		if (!((Status & DN_DISABLEABLE) && (CM_PROB_HARDWARE_DISABLED != Problem)))   
			return FALSE;   
		PropChangeParams.StateChange = DICS_DISABLE;   
	}   

	if (!SetupDiSetClassInstallParams(hDevInfo, &DeviceInfoData,   
		(SP_CLASSINSTALL_HEADER *)&PropChangeParams, sizeof(PropChangeParams)))   
		return FALSE;   
	if (!SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, hDevInfo, &DeviceInfoData))   
		return FALSE;   

	return TRUE;   
}

//功能：判断指定的网卡是否被禁用
//参数：lpNicGuid指定要进行判断的网卡的GUID不带大括号
//返回值：-1判断失败，0未禁用,1已禁用

extern "C" DIAGNOSEDLL_API int __stdcall IsNicDisabled(LPCTSTR lpNicGuid)
{
	NICLIST*		nl=NULL;
	GUID			NicGuid,TempGuid;
	RPC_STATUS		status;
	int				iRet=-1;

	if (NULL==lpNicGuid || UuidFromString((RPC_TSTR)lpNicGuid,&NicGuid)!=RPC_S_OK)
	{
		return iRet;
	}

	if (!EnumInterfaces(&nl))
	{
		return iRet;
	}
	for (DWORD i=0;i<nl->dwCount;++i)
	{
		if (UuidFromString((RPC_TSTR)nl->lpNics[i].m_chDevGuid,&TempGuid)==RPC_S_OK)
		{
			if (UuidEqual(&NicGuid,&TempGuid,&status))
			{
				iRet=nl->lpNics[i].m_bDisabled?1:0;
				break;
			}
		}
	}//for

	FreeNicListBuffer(nl);
	return iRet;
}




//功能：	返回指定网卡上配置的网关；
//参数：	lpNicGuid为物理网卡的GUID（该GUID不带大括号），sl调用成功包含网卡上已经设置的所有网关；
//返回值：	查询成功返回TRUE，失败返回FALSE；
//说明：	该接口不能应用于已经禁用的接口。
//调用成功后必须调用FreeStrListBuffer释放内存
extern "C" DIAGNOSEDLL_API BOOL __stdcall GetInterfaceGateway(LPCTSTR lpNicGuid,StrList** sl)
{

	PIP_ADAPTER_INFO pAdapterInfo			  = NULL;
	PIP_ADAPTER_INFO pAdapter				  = NULL;
	DWORD			 dwRetVal				  = 0;
	GUID			 NicGuid,TempGuid;
	BOOL			 bIsok					  = FALSE;
	RPC_STATUS		 status;
	ULONG			 ulOutBufLen			  = 0;
	char			 chTemp[256]			  = {0};

	if (NULL==lpNicGuid||UuidFromString((RPC_TSTR)lpNicGuid,&NicGuid)!=RPC_S_OK
		||NULL==sl)
	{
		return bIsok;
	}

	*sl=NULL;

	std::list<TString> strs_list;

	pAdapterInfo = (IP_ADAPTER_INFO *) malloc( sizeof(IP_ADAPTER_INFO));
	ulOutBufLen  = sizeof(IP_ADAPTER_INFO);

	if (GetAdaptersInfo( pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *) malloc (ulOutBufLen); 
	}

	if ((dwRetVal = GetAdaptersInfo( pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
		pAdapter = pAdapterInfo;
		while (pAdapter) {
#if defined _UNICODE||defined UNICODE
			WCHAR			wchTemp[128]={0};
			MultiByteToWideChar(CP_ACP,0,pAdapter->AdapterName,-1,wchTemp,128);
			std::wstring	strTemp(wchTemp);
#else
			std::string		strTemp(pAdapter->AdapterName);
#endif


			if (UuidFromString((RPC_TSTR)strTemp.substr(1,36).c_str(),&TempGuid)==RPC_S_OK)
			{
				if (UuidEqual(&NicGuid,&TempGuid,&status))
				{
					for (PIP_ADDR_STRING p=&(pAdapter->GatewayList);p!=NULL;p=p->Next)
					{
						if (strlen(p->IpAddress.String)!=0)
						{
#if defined _UNICODE||defined UNICODE
							WCHAR	wchTemp[256]={0};
							MultiByteToWideChar(CP_ACP,0,p->IpAddress.String,-1,wchTemp,256);
							strs_list.push_back(wchTemp);
#else
							strs_list.push_back(p->IpAddress.String);
#endif
						}	
					}
					bIsok=TRUE;
					break;
				}
			}
			pAdapter = pAdapter->Next;
		}//while
	}//if

	free(pAdapterInfo);

	if (bIsok)
	{
		*sl		=new StrList;
		StrList *lpTemp	=*sl;
		lpTemp->dwCount	=strs_list.size();
		lpTemp->strs	=NULL;
		if (strs_list.size()!=0)
		{
			lpTemp->strs	=new LPTSTR[lpTemp->dwCount];
			int k=0;
			for (std::list<TString>::iterator i=strs_list.begin();i!=strs_list.end();++i,++k)
			{
				TCHAR *temp=new TCHAR[i->length()+1];
				_tcscpy(temp,i->c_str());
				lpTemp->strs[k]=temp;
			}
		}
	}

	return bIsok;
}

//功能：	返回指定网卡上配置的DNS；
//参数：	lpNicGuid为物理网卡的GUID（该GUID不带大括号），sl调用成功包含网卡上已经设置的所有DNS；
//返回值：	查询成功返回TRUE，失败返回FALSE；
//说明：	该接口不能应用于已经禁用的接口。
//调用成功后必须调用FreeStrListBuffer释放内存
extern "C" DIAGNOSEDLL_API BOOL __stdcall GetInterfaceDnsserver(LPCTSTR lpNicGuid,StrList** sl)
{
	PIP_ADAPTER_INFO	 pAdapterInfo					= NULL;
	PIP_ADAPTER_INFO	 pAdapter						= NULL;
	DWORD				 dwRetVal						= 0;
	GUID				 NicGuid,TempGuid;
	BOOL				 bIsok							= FALSE;
	RPC_STATUS			 status;
	PIP_PER_ADAPTER_INFO pPerAdapterInfo				= NULL;
	ULONG				 ulOutBufLen					= 0;


	if (NULL==lpNicGuid||UuidFromString((RPC_TSTR)lpNicGuid,&NicGuid)!=RPC_S_OK
		|| NULL==sl)
	{
		return bIsok;
	}

	*sl=NULL;

	std::list<TString> strs_list;

	pAdapterInfo = (IP_ADAPTER_INFO *) malloc( sizeof(IP_ADAPTER_INFO));
	ulOutBufLen  = sizeof(IP_ADAPTER_INFO);

	if (GetAdaptersInfo( pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *) malloc (ulOutBufLen); 
	}

	if ((dwRetVal = GetAdaptersInfo( pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
		pAdapter = pAdapterInfo;
		while (pAdapter) {
#if defined _UNICODE||defined UNICODE
			WCHAR			wchTemp[128]={0};
			MultiByteToWideChar(CP_ACP,0,pAdapter->AdapterName,-1,wchTemp,128);
			std::wstring	strTemp(wchTemp);
#else
			std::string		strTemp(pAdapter->AdapterName);
#endif
			if (UuidFromString((RPC_TSTR)strTemp.substr(1,36).c_str(),&TempGuid)==RPC_S_OK)
			{
				if (UuidEqual(&NicGuid,&TempGuid,&status))
				{
					ulOutBufLen=0;
					if (GetPerAdapterInfo(pAdapter->Index,pPerAdapterInfo,&ulOutBufLen)==ERROR_BUFFER_OVERFLOW)
					{
						pPerAdapterInfo = (IP_PER_ADAPTER_INFO *) malloc (ulOutBufLen);
						if (pPerAdapterInfo!=NULL)
						{
							if (GetPerAdapterInfo(pAdapter->Index,pPerAdapterInfo,&ulOutBufLen)==ERROR_SUCCESS)
							{
								IP_ADDR_STRING* pNext=&(pPerAdapterInfo->DnsServerList);
								while(pNext)
								{
									if (strlen(pNext->IpAddress.String)!=0)
									{
#if defined _UNICODE||defined UNICODE
										WCHAR	wchTemp[256]={0};
										MultiByteToWideChar(CP_ACP,0,pNext->IpAddress.String,-1,wchTemp,256);
										strs_list.push_back(wchTemp);
#else
										strs_list.push_back(pNext->IpAddress.String);
#endif
									}
									pNext=pNext->Next;
								}
								bIsok=TRUE;
							}
							free(pPerAdapterInfo);
						}
					}//if
					break;
				}//if uuidequal
			}//if uuidfromstring
			pAdapter = pAdapter->Next;
		}//while
	}//if

	free(pAdapterInfo);

	if (bIsok)
	{
		*sl		=new StrList;
		StrList *lpTemp	=*sl;
		lpTemp->dwCount	=strs_list.size();
		lpTemp->strs	=NULL;
		if (strs_list.size()!=0)
		{
			lpTemp->strs=new LPTSTR[lpTemp->dwCount];
			int k=0;
			for (std::list<TString>::iterator i=strs_list.begin();i!=strs_list.end();++i,++k)
			{
				TCHAR *temp=new TCHAR[i->length()+1];
				_tcscpy(temp,i->c_str());
				lpTemp->strs[k]=temp;
			}
		}
	}

	return bIsok;
}


//功能：	判断指定的网卡上是否设置了有效的网关；
//参数：	lpNicGuid 为物理网卡的GUID（该GUID不带大括号）；
//返回值：	网卡上存在有效的网关返回1，不存在返回0,判断失败返回-1；
//说明：	该接口只能判断启用的网卡的网关设置，如果要对已被禁用的网卡进行判断，
//			必须先调用网卡启用禁用接口将网卡先启用，并且需要排除系统中其它网卡所带来的干扰，
//			建议判断之前先禁用其它网卡。

extern "C" DIAGNOSEDLL_API int __stdcall IsNicGatewaysOk(LPCTSTR lpNicGuid,int interrupt_flag)
{
	StrList* sl		=NULL;
	int		 iRet	=-1;
	if (!GetInterfaceGateway(lpNicGuid,&sl) || 0==sl->dwCount)
	{
		return iRet;
	}

	iRet=IsGatewayValid(sl->strs[0],interrupt_flag);
	FreeStrListBuffer(sl);
	return iRet;
}

//功能：	判断指定的网卡上是否设置了有效的DNS服务器；
//参数：	lpNicGuid 为物理网卡的GUID（该GUID不带大括号）；
//返回值：	网卡上存在有效的DNS返回1，不存在返回0,判断失败返回-1；
//说明：	该接口只能判断启用的网卡的DNS设置，如果要对已被禁用的网卡进行判断，
//			必须先调用网卡启用禁用接口将网卡先启用，对于网关设置有误的情况该接口不能正确的判断DNS的有效性，
//			并且需要排除系统中其它网卡所带来的干扰，建议判断之前先禁用其它网卡。

extern "C" DIAGNOSEDLL_API int __stdcall IsNicDnsServersOk(LPCTSTR lpNicGuid)
{
	static const char* Name2Test[]=
	{
		/*"www.siteview.com",
		"www.cisco.com",
		"www.microsoft.com",
		"www.baidu.com"*/
		//"www.yahoo.com",
		//"www.google.com",
		"www.netgear.com"
	};

	int iret = 0;

	hostent* ph=NULL;

	for (int i=0;i<sizeof(Name2Test)/sizeof(Name2Test[0]);++i)
	{
		ph=gethostbyname(Name2Test[i]);
		if (ph!=NULL)
		{
			iret = 1;
			/*cout<<inet_ntoa(*((in_addr*)(ph->h_addr_list[0])))<<endl;*/
			StrList *gateways = NULL;
			if (GetInterfaceGateway(lpNicGuid,&gateways))
			{
				for (DWORD k=0;k < gateways->dwCount;++k)
				{
					char temp[256];
#if defined UNICODE || defined UNICODE
					WideCharToMultiByte(CP_ACP,0,gateways->strs[k],-1,temp,256,NULL,NULL);
#else
					strcpy(temp,gateways->strs[k]);
#endif
					if (inet_addr(temp) == ((in_addr*)(ph->h_addr_list[0]))->S_un.S_addr)
					{
						iret = 0;
						break;
					}
				}
				FreeStrListBuffer(gateways);
			}
			break;
		}
	}

	return iret;
}

//功能：	检查指定网卡的DHCP是否启用；
//参数：	lpNicGuid为物理网卡的GUID（该GUID不带大括号）；
//返回值：	启用DHCP返回1，未启用DHCP返回0，判断失败返回-1；
//说明：	该接口不能判断已被禁用的网卡的DHCP设置。

extern "C" DIAGNOSEDLL_API int __stdcall IsDhcpEnabled(LPCTSTR lpNicGuid)
{
	PIP_ADAPTER_INFO pAdapterInfo	= NULL;
	PIP_ADAPTER_INFO pAdapter		= NULL;
	ULONG			 ulOutBufLen	=  0;
	int				 iEnabled		= -1;

	GUID			 NicGuid,TempGuid;
	RPC_STATUS		 status;

	if (NULL==lpNicGuid || UuidFromString((RPC_TSTR)lpNicGuid,&NicGuid)!=RPC_S_OK)
	{
		return iEnabled;
	}
	if (GetAdaptersInfo( pAdapterInfo, &ulOutBufLen) != ERROR_BUFFER_OVERFLOW) {
		return iEnabled;
	}

	if (!(pAdapterInfo = (IP_ADAPTER_INFO *) malloc (ulOutBufLen)))
	{
		return iEnabled;
	}
	if (GetAdaptersInfo( pAdapterInfo, &ulOutBufLen) == NO_ERROR) {
		pAdapter = pAdapterInfo;
		while (pAdapter) 
		{
#if defined _UNICODE||defined UNICODE
			WCHAR			wchTemp[128]={0};
			MultiByteToWideChar(CP_ACP,0,pAdapter->AdapterName,-1,wchTemp,128);
			std::wstring	strTemp(wchTemp);
#else
			std::string		strTemp(pAdapter->AdapterName);
#endif

			if (UuidFromString((RPC_TSTR)strTemp.substr(1,36).c_str(),&TempGuid)==RPC_S_OK)
			{
				if (UuidEqual(&NicGuid,&TempGuid,&status))
				{					
					iEnabled= pAdapter->DhcpEnabled?1:0;
					break;

				}
			}
			pAdapter=pAdapter->Next;
		}//while
	}//if

	free(pAdapterInfo);
	return iEnabled;
}


BOOL VerfyEnableDhcpOk(LPCTSTR lpNicGuid,int interrupt_flag)
{
	BOOL	bresult = FALSE;
	int		times	= 20;

	while (times--)
	{
		if((bresult = (IsDhcpEnabled(lpNicGuid) == 1)))
		{
			break;
		}

		if(times <= 0)
		{
			break;
		}

		if (interrupt_flag)
		{
			if(sleep_interruptable(1000))
				break;
		}
		else
		{
			Sleep(1000);
		}
	}

	return bresult;
}
//功能：	在指定的网卡上启用DHCP；
//参数：	lpNicGuid为物理网卡的GUID（该GUID不带大括号）；
//返回值：	成功启用DHCP返回TRUE，否则返回FALSE；
//说明：	该接口不能在已被禁用的网卡上启用DHCP。

extern "C" DIAGNOSEDLL_API BOOL __stdcall EnableDhcp(LPCTSTR lpNicGuid,int interrupt_flag)
{
	if (NULL==lpNicGuid||_tcslen(lpNicGuid)==0)
	{
		return FALSE;
	}
	//--------------------vista 实现--------------------------------
	if(GetOsVersion()>=6)
	{
		TString		strFriName;
		TCHAR		chCmd[512]={0};
		if(GetNicFriendlyName(lpNicGuid,strFriName))
		{
			wsprintf(chCmd,TEXT("netsh interface ip set address name=\"%s\" source=dhcp"),strFriName.c_str());
			if(Execute_Command_Privilege(chCmd))
			{
				return VerfyEnableDhcpOk(lpNicGuid,interrupt_flag);
			}
		}

		return FALSE;
	}
	//-----------------------end------------------------------------
	PIP_ADAPTER_INFO pAdapterInfo= NULL;
	PIP_ADAPTER_INFO pAdapter    = NULL;
	ULONG			 ulOutBufLen = 0;
	BOOL			 bIsOk		 = FALSE;
	GUID			 NicGuid,TempGuid;
	RPC_STATUS		 status;

	if (UuidFromString((RPC_TSTR)lpNicGuid,&NicGuid)!=RPC_S_OK)
	{
		return FALSE;
	}
	if (GetAdaptersInfo( pAdapterInfo, &ulOutBufLen) != ERROR_BUFFER_OVERFLOW) {
		return FALSE;
	}

	if (!(pAdapterInfo = (IP_ADAPTER_INFO *) malloc (ulOutBufLen)))
	{
		return FALSE;
	}
	if (GetAdaptersInfo( pAdapterInfo, &ulOutBufLen) == NO_ERROR) {
		pAdapter = pAdapterInfo;
		while (pAdapter) 
		{
#if defined _UNICODE||defined UNICODE
			WCHAR			wchTemp[128]={0};
			MultiByteToWideChar(CP_ACP,0,pAdapter->AdapterName,-1,wchTemp,128);
			std::wstring	strTemp(wchTemp);
#else
			std::string		strTemp(pAdapter->AdapterName);
#endif
			if (UuidFromString((RPC_TSTR)strTemp.substr(1,36).c_str(),&TempGuid)==RPC_S_OK)
			{
				if (UuidEqual(&NicGuid,&TempGuid,&status))
				{				
					if (RegEnableDhcp(strTemp.c_str()))
					{
						NotifyDhcpEnable(strTemp.c_str());
						bIsOk = VerfyEnableDhcpOk(lpNicGuid,interrupt_flag);
					}
					break;
				}
			}
			pAdapter=pAdapter->Next;
		}//while
	}//if

	free(pAdapterInfo);
	return bIsOk;
}

//功能：	在指定的网卡上设置DNS的来源为DHCP
//参数：	lpNicGuid为物理网卡的GUID（该GUID不带大括号）；
//返回值：	成功返回TRUE，否则返回FALSE。

extern "C" DIAGNOSEDLL_API BOOL __stdcall EnableDnsFromDhcp(LPCTSTR lpNicGuid,int interrupt_flag)
{
	if (NULL==lpNicGuid||_tcslen(lpNicGuid)==0)
	{
		return FALSE;
	}

	TString		 strFriName;
	TCHAR		 chCmd[512]={0};
	if(GetNicFriendlyName(lpNicGuid,strFriName))
	{
		if (IsDhcpEnabled(lpNicGuid) != 1)
		{
			if (!EnableDhcp(lpNicGuid,interrupt_flag))
			{
				return FALSE;
			}
		}

		wsprintf(chCmd,TEXT("netsh interface ip set dns name=\"%s\" source=dhcp"),strFriName.c_str());
		if(Execute_Command_Privilege(chCmd))
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	return FALSE;
}
//----------------------------------------------dhcp API end-------------------------------
//功能：	在指定的网卡上设置指定的DNS服务器；
//参数：	lpNicGuid为物理网卡的GUID（该GUID不带大括号），dwDnsc Dns服务器的个数，szDnses Dns服务器列表；
//返回值：	成功返回TRUE，失败返回FALSE。

extern "C" DIAGNOSEDLL_API BOOL __stdcall SetInterfaceDnses(LPCTSTR lpNicGuid,DWORD dwDnsc,LPTSTR szDnses[],int interrupt_flag)
{
	BOOL bRet=FALSE;
	if (  NULL					==lpNicGuid
		||_tcslen(lpNicGuid)	==0
		||NULL					==szDnses
		||0						==dwDnsc
		)
	{
		return bRet;
	}

	TString		 strFriName;
	TCHAR		 chCmd[512]	={0};
	char		 chTemp[512]={0};
	if(!GetNicFriendlyName(lpNicGuid,strFriName))
	{
		return bRet;
	}

	wsprintf(chCmd,TEXT("netsh interface ip set dns name=\"%s\" static %s"),strFriName.c_str(),szDnses[0]);

	//#if defined _UNICODE||defined UNICODE
	//	WideCharToMultiByte(CP_ACP,0,chCmd,-1,chTemp,256,NULL,FALSE);
	//#else
	//	strcpy(chTemp,chCmd);
	//#endif
	//	if(!(WinExec(chTemp,SW_HIDE)>31))
	//	{
	//		return bRet;
	//	}

	if(!Execute_Command_Privilege(chCmd))
	{
		return bRet;
	}

	for (DWORD i=1;i<dwDnsc;++i)
	{
		wsprintf(chCmd,TEXT("netsh interface ip add dns name=\"%s\"  %s"),strFriName.c_str(),szDnses[i]);
		//Sleep(5000);
		if (interrupt_flag)
		{
			if (sleep_interruptable(5000))
			{
				break;
			}
		}
		else
		{
			Sleep(5000);
		}
		//#if defined _UNICODE||defined UNICODE
		//		WideCharToMultiByte(CP_ACP,0,chCmd,-1,chTemp,256,NULL,FALSE);
		//#else
		//		strcpy(chTemp,chCmd);
		//#endif
		//		if(!(WinExec(chTemp,SW_HIDE)>31))
		//		{
		//			return bRet;
		//		}
		if(!Execute_Command_Privilege(chCmd))
		{
			return bRet;
		}
	}

	//Sleep(15000);
	if (interrupt_flag)
	{
		sleep_interruptable(15000);
	}
	else
	{
		Sleep(15000);
	}

	StrList*		sl	=NULL;
	DWORD			i	=0;
	if (GetInterfaceDnsserver(lpNicGuid,&sl))
	{
		for (DWORD k=0;k<dwDnsc;++k)
		{
			for (i=0;i<sl->dwCount;++i)
			{
				if (!_tcscmp(sl->strs[i],szDnses[k]))
				{
					bRet=TRUE;
					break;
				}
			}
		}
		FreeStrListBuffer(sl);
	}

	return bRet;
}


//功能：	检测指定网卡的网线是否已经插好；
//参数：	lpNicGuid指定要检测的有线物理网卡的GUID（该GUID不带大括号）；
//返回值：	网线插上返回TRUE，网线拔出返回FALSE；
//说明：	该接口只对启用的网卡有效。

extern "C" DIAGNOSEDLL_API BOOL __stdcall IsNicCablePluggedin(LPCTSTR lpNicGuid)
{
	PIP_ADAPTER_INFO pAdapterInfo		= NULL;
	PIP_ADAPTER_INFO pAdapter			= NULL;
	DWORD			 dwRetVal			= 0;
	GUID			 NicGuid,TempGuid;
	BOOL			 bIsok				= FALSE;
	RPC_STATUS		 status;
	ULONG			 ulOutBufLen		= 0;
	DWORD			 dwIndex			= 0;


	if (NULL==lpNicGuid||_tcslen(lpNicGuid)==0)
	{
		return FALSE;
	}

	if (NULL==lpNicGuid||UuidFromString((RPC_TSTR)lpNicGuid,&NicGuid)!=RPC_S_OK)
	{
		return FALSE;
	}

	pAdapterInfo = (IP_ADAPTER_INFO *) malloc( sizeof(IP_ADAPTER_INFO));
	ulOutBufLen  = sizeof(IP_ADAPTER_INFO);

	if (GetAdaptersInfo( pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *) malloc (ulOutBufLen); 
	}

	if (GetAdaptersInfo( pAdapterInfo, &ulOutBufLen)== NO_ERROR) {
		pAdapter = pAdapterInfo;
		while (pAdapter) {
#if defined _UNICODE||defined UNICODE
			WCHAR			wchTemp[128]={0};
			MultiByteToWideChar(CP_ACP,0,pAdapter->AdapterName,-1,wchTemp,128);
			std::wstring	strTemp(wchTemp);
#else
			std::string		strTemp(pAdapter->AdapterName);
#endif
			if (UuidFromString((RPC_TSTR)strTemp.substr(1,36).c_str(),&TempGuid)==RPC_S_OK)
			{
				if (UuidEqual(&NicGuid,&TempGuid,&status))
				{
					dwIndex=pAdapter->Index;
					bIsok=TRUE;
					break;
				}
			}
			pAdapter = pAdapter->Next;
		}//while
	}//if

	free(pAdapterInfo);

	if (!bIsok)
	{
		return FALSE;
	}
	MIB_IFROW ifRow;
	ifRow.dwIndex=dwIndex;
	if(GetIfEntry(&ifRow)==NO_ERROR)
	{
		if (  MIB_IF_OPER_STATUS_NON_OPERATIONAL==ifRow.dwOperStatus
			||MIB_IF_OPER_STATUS_DISCONNECTED	==ifRow.dwOperStatus
			||MIB_IF_OPER_STATUS_UNREACHABLE	==ifRow.dwOperStatus)
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
	else
	{
		return FALSE;
	}
}


//-------------------------------------------------------------------------------------------------
//功能：	判断指定网卡上的网关或DNS是否有效
//参数：	lpNicGuid指定要进行判断的网卡的GUID
//			bGatewayorDns为TRUE判断网关，为FALSE判断DNS
//返回值：	-1判断失败，0无效，1有效，2网关可通但不通外网

extern "C" DIAGNOSEDLL_API int __stdcall RepairGatewayorDnsOk(LPCTSTR lpNicGuid,BOOL bGatewayorDns,int interrupt_flag)
{
	NICLIST			*nl=NULL;
	GUID			guid1,guid2;
	RPC_STATUS		status;
	int				iRet=-1;

	UuidFromString((RPC_TSTR)lpNicGuid,&guid1);
	if (!EnumInterfaces(&nl))
	{
		return iRet;
	}
	//禁用其它干扰网卡
	for (DWORD j=0;j<nl->dwCount;++j)
	{
		UuidFromString((RPC_TSTR)nl->lpNics[j].m_chDevGuid,&guid2);
		if (!UuidEqual(&guid1,&guid2,&status) && (!(nl->lpNics[j].m_bDisabled)))
		{
			NetCardStateChange(nl->lpNics[j].m_chDevGuid,FALSE);
		}
	}

	iRet=bGatewayorDns?IsNicGatewaysOk(lpNicGuid,interrupt_flag):IsNicDnsServersOk(lpNicGuid);

	//恢复其它干扰网卡
	for (DWORD j=0;j<nl->dwCount;++j)
	{
		if (!(nl->lpNics[j].m_bDisabled))
		{
			NetCardStateChange(nl->lpNics[j].m_chDevGuid,TRUE);
		}
	}

	FreeNicListBuffer(nl);
	return iRet;
}

extern "C" DIAGNOSEDLL_API BOOL __stdcall ConfigDNSManually(LPCTSTR lpNicGuid,int interrupt_flag)
{
	LPTSTR DNSes[]={TEXT("208.67.222.222"),TEXT("208.67.220.220")};

	if (SetInterfaceDnses(lpNicGuid,sizeof(DNSes)/sizeof(DNSes[0]),DNSes,interrupt_flag))
	{
		if (1==RepairGatewayorDnsOk(lpNicGuid,FALSE,interrupt_flag))
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	return FALSE;
}



extern "C" DIAGNOSEDLL_API BOOL __stdcall IpRenewInterface(LPCTSTR lpNicGuid)
{
	GUID				NicGuid,TempGuid;
	BOOL				bIsok						= FALSE;
	RPC_STATUS			status;
	ULONG				ulOutBufLen					= 0;
	PIP_INTERFACE_INFO	pInfo;
	char				chTemp[256]					= {0};

	if (NULL==lpNicGuid||UuidFromString((RPC_TSTR)lpNicGuid,&NicGuid)!=RPC_S_OK)
	{
		return bIsok;
	}

	pInfo = (IP_INTERFACE_INFO *) malloc(sizeof(IP_INTERFACE_INFO));
	if (GetInterfaceInfo(pInfo, &ulOutBufLen) == ERROR_INSUFFICIENT_BUFFER) {
		free(pInfo);
		pInfo = (IP_INTERFACE_INFO *) malloc (ulOutBufLen);
	}

	if (GetInterfaceInfo(pInfo, &ulOutBufLen) == NO_ERROR ) 
	{
		for(LONG i=0;i<pInfo->NumAdapters;++i)
		{
			TCHAR tchTemp[128] = {0};
#if defined _UNICODE||defined UNICODE
			wcscpy(tchTemp,pInfo->Adapter[0].Name);
#else
			WideCharToMultiByte(CP_ACP,0,pInfo->Adapter[0].Name,-1,tchTemp,128,NULL,NULL);	
#endif
			TCHAR * pbegin=_tcsstr(tchTemp,TEXT("{"));
			TCHAR * pend  =_tcsstr(tchTemp,TEXT("}"));
			if((pbegin!=NULL) && (pend!=NULL))
			{
				++pbegin;
				*pend = TEXT('\0');
				if (UuidFromString((RPC_TSTR)pbegin,&TempGuid)==RPC_S_OK)
				{
					if (UuidEqual(&NicGuid,&TempGuid,&status))
					{
						if ((IpReleaseAddress(&pInfo->Adapter[i]) == NO_ERROR) 
							&& (IpRenewAddress(&pInfo->Adapter[i]) == NO_ERROR)) 
						{
							bIsok = TRUE;
						}
						break;
					}
				}	
			}
		}
	}

	if(pInfo!=NULL)
		free(pInfo);
	return bIsok;
}

BOOL ping2(const char* szDestIp,const std::list<LPTSTR> &GatewayIps_list,unsigned long timeoutms);

extern "C" DIAGNOSEDLL_API BOOL __stdcall IsHostReachble(const char* szDestIp,unsigned long timeoutms)
{
	NICLIST *nic_list		= NULL;
	BOOL	bresult			= FALSE;

	if (!EnumInterfaces(&nic_list) || NULL == nic_list || 0 == nic_list->dwCount)
	{
		if (nic_list)
		{
			FreeNicListBuffer(nic_list);
		}
		return bresult;
	}

	StrList					*str_list			= NULL;
	std::list<StrList *>	garbage_str_list;
	std::list<LPTSTR>		gateways_list;

	for (DWORD k = 0;k<nic_list->dwCount;++k)
	{
		if (GetInterfaceGateway(nic_list->lpNics[k].m_chDevGuid,&str_list))
		{
			garbage_str_list.push_back(str_list);

			for (DWORD j = 0;j<str_list->dwCount;++j)
			{
				gateways_list.push_back(str_list->strs[j]);
			}
			str_list = NULL;
		}
	}

	bresult = ping2(szDestIp,gateways_list,timeoutms);

	for (std::list<StrList *>::const_iterator iter = garbage_str_list.begin() ;
		iter != garbage_str_list.end() ; ++iter)
	{
		FreeStrListBuffer(*iter);
	}

	FreeNicListBuffer(nic_list);

	return bresult;
}

extern "C" DIAGNOSEDLL_API BOOL __stdcall IsThisPhysicalNic(LPCTSTR lpNicGuid)
{
	return IsPhysicalNic(lpNicGuid);
}

