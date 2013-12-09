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


//���ܣ�	ö�ٻ����ϵ����е���������(��������������)
//������	NIC�ṹ����б����óɹ��������е��������������ϵͳ
//����ֵ��	���óɹ�����TRUE�����򷵻�FALSE
//���óɹ���������FreeNicListBuffer�ͷ��ڴ�
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
					//CurNic.m_strDevGuid=CurNic.m_strDevGuid.substr(1,36);//ȥ��GUID�Ĵ�����
					TString strDevGuid=CurNic.m_chDevGuid;
					_tcscpy(CurNic.m_chDevGuid,strDevGuid.substr(1,REG_GUID_STR_LEN).c_str());//ȥ��GUID�Ĵ�����
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
//���ܣ�	�������������  
//������	lpNicGuid��Ҫ����״̬�ı��������GUIDδ��������
//			Enabled     TRUE = ����     FALSE = ����   
//����ֵ��	�ɹ����û������������TRUE�����򷵻�FALSE

//---------------------------------------------------------------------------   
extern "C" DIAGNOSEDLL_API BOOL __stdcall NetCardStateChange(LPCTSTR lpNicGuid, BOOL Enabled)   
{  
	if(NULL==lpNicGuid)
	{
		return FALSE;
	}
	//-----------------------------win 2003 /vista ʵ��------------------------
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

//���ܣ��ж�ָ���������Ƿ񱻽���
//������lpNicGuidָ��Ҫ�����жϵ�������GUID����������
//����ֵ��-1�ж�ʧ�ܣ�0δ����,1�ѽ���

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




//���ܣ�	����ָ�����������õ����أ�
//������	lpNicGuidΪ����������GUID����GUID���������ţ���sl���óɹ������������Ѿ����õ��������أ�
//����ֵ��	��ѯ�ɹ�����TRUE��ʧ�ܷ���FALSE��
//˵����	�ýӿڲ���Ӧ�����Ѿ����õĽӿڡ�
//���óɹ���������FreeStrListBuffer�ͷ��ڴ�
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

//���ܣ�	����ָ�����������õ�DNS��
//������	lpNicGuidΪ����������GUID����GUID���������ţ���sl���óɹ������������Ѿ����õ�����DNS��
//����ֵ��	��ѯ�ɹ�����TRUE��ʧ�ܷ���FALSE��
//˵����	�ýӿڲ���Ӧ�����Ѿ����õĽӿڡ�
//���óɹ���������FreeStrListBuffer�ͷ��ڴ�
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


//���ܣ�	�ж�ָ�����������Ƿ���������Ч�����أ�
//������	lpNicGuid Ϊ����������GUID����GUID���������ţ���
//����ֵ��	�����ϴ�����Ч�����ط���1�������ڷ���0,�ж�ʧ�ܷ���-1��
//˵����	�ýӿ�ֻ���ж����õ��������������ã����Ҫ���ѱ����õ����������жϣ�
//			�����ȵ����������ý��ýӿڽ����������ã�������Ҫ�ų�ϵͳ�����������������ĸ��ţ�
//			�����ж�֮ǰ�Ƚ�������������

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

//���ܣ�	�ж�ָ�����������Ƿ���������Ч��DNS��������
//������	lpNicGuid Ϊ����������GUID����GUID���������ţ���
//����ֵ��	�����ϴ�����Ч��DNS����1�������ڷ���0,�ж�ʧ�ܷ���-1��
//˵����	�ýӿ�ֻ���ж����õ�������DNS���ã����Ҫ���ѱ����õ����������жϣ�
//			�����ȵ����������ý��ýӿڽ����������ã����������������������ýӿڲ�����ȷ���ж�DNS����Ч�ԣ�
//			������Ҫ�ų�ϵͳ�����������������ĸ��ţ������ж�֮ǰ�Ƚ�������������

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

//���ܣ�	���ָ��������DHCP�Ƿ����ã�
//������	lpNicGuidΪ����������GUID����GUID���������ţ���
//����ֵ��	����DHCP����1��δ����DHCP����0���ж�ʧ�ܷ���-1��
//˵����	�ýӿڲ����ж��ѱ����õ�������DHCP���á�

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
//���ܣ�	��ָ��������������DHCP��
//������	lpNicGuidΪ����������GUID����GUID���������ţ���
//����ֵ��	�ɹ�����DHCP����TRUE�����򷵻�FALSE��
//˵����	�ýӿڲ������ѱ����õ�����������DHCP��

extern "C" DIAGNOSEDLL_API BOOL __stdcall EnableDhcp(LPCTSTR lpNicGuid,int interrupt_flag)
{
	if (NULL==lpNicGuid||_tcslen(lpNicGuid)==0)
	{
		return FALSE;
	}
	//--------------------vista ʵ��--------------------------------
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

//���ܣ�	��ָ��������������DNS����ԴΪDHCP
//������	lpNicGuidΪ����������GUID����GUID���������ţ���
//����ֵ��	�ɹ�����TRUE�����򷵻�FALSE��

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
//���ܣ�	��ָ��������������ָ����DNS��������
//������	lpNicGuidΪ����������GUID����GUID���������ţ���dwDnsc Dns�������ĸ�����szDnses Dns�������б�
//����ֵ��	�ɹ�����TRUE��ʧ�ܷ���FALSE��

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


//���ܣ�	���ָ�������������Ƿ��Ѿ���ã�
//������	lpNicGuidָ��Ҫ������������������GUID����GUID���������ţ���
//����ֵ��	���߲��Ϸ���TRUE�����߰γ�����FALSE��
//˵����	�ýӿ�ֻ�����õ�������Ч��

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
//���ܣ�	�ж�ָ�������ϵ����ػ�DNS�Ƿ���Ч
//������	lpNicGuidָ��Ҫ�����жϵ�������GUID
//			bGatewayorDnsΪTRUE�ж����أ�ΪFALSE�ж�DNS
//����ֵ��	-1�ж�ʧ�ܣ�0��Ч��1��Ч��2���ؿ�ͨ����ͨ����

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
	//����������������
	for (DWORD j=0;j<nl->dwCount;++j)
	{
		UuidFromString((RPC_TSTR)nl->lpNics[j].m_chDevGuid,&guid2);
		if (!UuidEqual(&guid1,&guid2,&status) && (!(nl->lpNics[j].m_bDisabled)))
		{
			NetCardStateChange(nl->lpNics[j].m_chDevGuid,FALSE);
		}
	}

	iRet=bGatewayorDns?IsNicGatewaysOk(lpNicGuid,interrupt_flag):IsNicDnsServersOk(lpNicGuid);

	//�ָ�������������
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

