// SVTUtils.cpp : 定义 DLL 应用程序的入口点。
//


#include "SVTUtils.h"


#include <iostream>
#include <string>
#include <winsock2.h>

#include <list>
#include <lm.h>
#include <iphlpapi.h>
#include <winioctl.h> 
#include "Iphlpapi.h"
#include "ntddndis.h"
#include <Windows.h>
#include <lm.h>
#include <NtDDNdis.h>
#include <tchar.h>
#include <Rpc.h>
#include <assert.h>
//#include "../DiagnoseDll/DiagnoseDll.h"
#define MY_BUFFER_SIZE 1024
#define REG_GUID_STR_LEN	36
#define NETWORK_CONNECTIONS_KEY		("SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}\\")
#define KEY_NETWORK_CONNECTIONS TEXT("SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}")
#define KEY_NETWORK_CLASS TEXT("SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002BE10318}")
#define KEY_NETWORK_TCPIP_ADPTERS TEXT("SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Adapters")
#if defined _UNICODE || defined UNICODE
#define TString		std::wstring
#define RPC_TSTR	RPC_WSTR
#else
#define TString		std::string
#define RPC_TSTR	RPC_CSTR
#endif
#define NCF_PHYSICAL 0x4

#ifdef _MANAGED
#pragma managed(push, off)
#endif

//static size_t local_mem_size=0;
//static char *local_mem=NULL;
//const size_t cMemInitialSize=2048;

using namespace std;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	static int process_count=0;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		process_count++;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		//if((--process_count)==0 && local_mem)
		//{
		//	free(local_mem);

		//}
		break;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

// 这是导出变量的一个示例
//SVTUTILS_API int nSVTUtils=0;

// 这是导出函数的一个示例。
//SVTUTILS_API int fnSVTUtils(void)
//{
//	return 42;
//}
SVTUTILS_API bool get_adapters_info (struct AllAdapters * alladp,size_t _len);



SVTUTILS_API int getipmac(const unsigned long &ip, const unsigned long &srcip,unsigned char  (&mac_str)[8])
{
	ULONG   mac[2] ={0}, len = 0;
	memset (mac, 0xff, sizeof (mac));
	memset(mac_str,0,sizeof(mac_str));
	len = 6;

	if (NO_ERROR == SendARP (htonl(ip), htonl(srcip), mac, &len)) 
	{
		unsigned int i = 0, j = 0;
		PBYTE hex_mac = (PBYTE)mac;
		memset (mac_str, 0, 8);
		memcpy(mac_str,(char *)hex_mac,min(len,8));
	} 
	return 1;
}


bool query_ndis_media_state (const string adapter_name, NDIS_MEDIA_STATE &state)
{
	bool res = false;
	char name[1024];
	HANDLE h = INVALID_HANDLE_VALUE;

	memset (name, 0, sizeof (name));
	sprintf(name, "\\\\.\\%s", adapter_name.c_str ());

	h = CreateFile(name, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
		OPEN_EXISTING, 0, NULL);
	if (INVALID_HANDLE_VALUE == h) {
		//Log::report (str (boost::format ("NetScan::get_connection_type, CreateFile error: %s, %d") %name %GetLastError ()));
		return false;
	}

	DWORD oid = OID_GEN_MEDIA_CONNECT_STATUS, len = 0;
	NDIS_MEDIA_STATE media_state;

	if (DeviceIoControl (h, IOCTL_NDIS_QUERY_GLOBAL_STATS,
		&oid, sizeof (oid), &media_state, sizeof (media_state),
		&len, NULL)) {
			res = true;
			state = media_state;
	} else {
		res = false;
	}

	CloseHandle (h);

	return res;
}
//BOOL IsPhysicalNic(LPCTSTR lpNicGuid);
//SVTUTILS_API bool is_physical_nic (char * uuid)
//{
//	return IsPhysicalNic(uuid);
	//PIP_ADAPTER_INFO original_adapter = NULL, adapter = NULL;
	//ULONG len = 0;
	//char adpt_name[128]={0};
	//GetAdaptersInfo (adapter, &len);
	//original_adapter = adapter = (PIP_ADAPTER_INFO)GlobalAlloc (GPTR, len);
	//if (ERROR_SUCCESS != GetAdaptersInfo (adapter, &len)) {
	//	//Log::report ("NetScan::get_adapters_info, GetAdaptersInfo error");
	//	return false;
	//}

	//while (adapter) {
	//	if(adapter->Index==adp_idx)
	//	{
	//		strncpy(adpt_name,adapter->AdapterName,sizeof(adpt_name));
	//		break;
	//	}
	//	adapter = adapter->Next;

	//}
	//GlobalFree(original_adapter);
	//return strlen(adpt_name)?IsPhysicalNic(adpt_name):false;
//}

BOOL IsPhysicalNic2(LPCTSTR lpNicGuid);
//BOOL IsPhysicalNic(LPCTSTR lpNicGuid)
//{
//	BOOL		 bRet		 =FALSE;
//	DWORD		 dwDataLen	 =MAX_PATH;
//	DWORD		 dwType		 =REG_SZ;
//	HKEY		 hNetKey	 =NULL;
//	HKEY		 hLocalNet	 =NULL;
//	DWORD	     dwMedia	 =0;
//	TCHAR		 szDataBuf[MY_BUFFER_SIZE+1];
//
//	if (NULL == lpNicGuid || _tcslen(lpNicGuid) < (REG_GUID_STR_LEN + 2))
//	{
//		return bRet;
//	}
//
//	if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, KEY_NETWORK_CONNECTIONS, 0, KEY_READ,&hNetKey))
//		return bRet;
//
//	wsprintf(szDataBuf, TEXT("%s\\Connection"),lpNicGuid);
//	if(ERROR_SUCCESS != RegOpenKeyEx(hNetKey ,szDataBuf ,0 ,KEY_READ, &hLocalNet))
//	{
//		RegCloseKey(hNetKey);
//		return bRet;
//	}
//
//	dwDataLen = MY_BUFFER_SIZE;
//	if (ERROR_SUCCESS != RegQueryValueEx(hLocalNet, TEXT("PnpInstanceID"), 0, &dwType, (BYTE *)szDataBuf, &dwDataLen))
//	{
//		goto Exit;
//	}
//
//	if ((NULL==_tcsstr(szDataBuf, TEXT("PCI")))&&(NULL==_tcsstr(szDataBuf,TEXT("USB")))&&(NULL==_tcsstr(szDataBuf,TEXT("1394"))))
//		goto Exit;
//
//	dwType		=REG_DWORD;
//	dwDataLen	=sizeof(DWORD);
//	if (ERROR_SUCCESS != RegQueryValueEx(hLocalNet, TEXT("MediaSubType"), 0, &dwType, (BYTE *)&dwMedia, &dwDataLen))
//	{
//		/*goto Exit;*/
//		dwMedia=1;//Vista 系统下有线网卡没有MediaSubType键值
//	}
//
//	if (dwMedia!=0x01&&dwMedia!=2&&dwMedia!=5)
//	{
//		goto Exit;
//	}
//
//	bRet = TRUE;
//
//Exit:
//	RegCloseKey(hLocalNet);
//	RegCloseKey(hNetKey);
//
//	return (bRet?bRet:IsPhysicalNic2(lpNicGuid));
//}

//BOOL IsPhysicalNic2(LPCTSTR lpNicGuid)
//{
//	HKEY		hKeyNetClass;
//	UUID		nic_guid,tmp_guid;
//	RPC_STATUS	rpc_status;
//	BOOL		res						= FALSE;
//	DWORD		dwCharactics			= 0;
//	TCHAR		szBuffer[MY_BUFFER_SIZE + 1];
//
//	if (NULL == lpNicGuid || _tcslen(lpNicGuid) < (REG_GUID_STR_LEN + 2))
//	{
//		return res;
//	}
//
//	TString	nicguid_str = lpNicGuid;
//	if(UuidFromString((RPC_TSTR)nicguid_str.substr(1,REG_GUID_STR_LEN).c_str(),&nic_guid) != RPC_S_OK)
//	{
//		return res;
//	}
//
//	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, KEY_NETWORK_CLASS, 0, KEY_READ, &hKeyNetClass) != ERROR_SUCCESS)
//		return res;
//
//	for (DWORD i=0 ; ; i++)
//	{
//		DWORD dwBufferSize = MY_BUFFER_SIZE;
//		if (RegEnumKey(hKeyNetClass, i, szBuffer, dwBufferSize) != ERROR_SUCCESS)
//		{
//			RegCloseKey(hKeyNetClass);
//			return res;
//		}
//
//		HKEY  hSubKey;
//		DWORD dwType;
//
//		if (RegOpenKeyEx(hKeyNetClass, szBuffer, 0, KEY_READ, &hSubKey) != ERROR_SUCCESS)
//			continue;
//
//		dwType			= REG_SZ;
//		dwBufferSize	= MY_BUFFER_SIZE;
//
//		if (RegQueryValueEx(hSubKey, TEXT("NetCfgInstanceId"), NULL, &dwType, (LPBYTE)szBuffer, &dwBufferSize) != ERROR_SUCCESS)
//		{
//			RegCloseKey(hSubKey);
//			continue;
//		}
//
//		TString	tmpguid_str(szBuffer);
//		if ( UuidFromString((RPC_TSTR)tmpguid_str.substr(1,REG_GUID_STR_LEN).c_str(),&tmp_guid) != RPC_S_OK )
//		{
//			RegCloseKey(hSubKey);
//			continue;
//		}
//
//		if (UuidEqual(&nic_guid,&tmp_guid,&rpc_status))
//		{
//			dwType			= REG_DWORD;
//			dwBufferSize	= sizeof(DWORD);
//
//			if (RegQueryValueEx(hSubKey, TEXT("Characteristics"), NULL, &dwType, (LPBYTE)&dwCharactics, &dwBufferSize) == ERROR_SUCCESS)
//			{
//				res = ((dwCharactics & NCF_PHYSICAL) == NCF_PHYSICAL);
//			}
//
//			RegCloseKey(hSubKey);
//			break;
//		}
//
//		RegCloseKey(hSubKey);
//	}
//
//	RegCloseKey(hKeyNetClass);
//
//	return res;
//}


SVTUTILS_API bool get_adapters_info (struct AllAdapters * alladp,size_t _len)
{
#define ADP_INFO_BUFFER(c) ((AdapterInfo *)((char *)alladp+sizeof(struct AllAdapters)+(sizeof(AdapterInfo)*(c)) ) )

	//step1 gateway info
	DWORD size = 0;
	PMIB_IPFORWARDTABLE forward_tab = NULL;
	bool res = true;
	ULONG len = 0;
	PIP_ADAPTER_INFO original_adapter = NULL, adapter = NULL;
	string gateway;
//	NICLIST *niclist=NULL;

	assert(alladp);
	assert(_len >=sizeof(struct AllAdapters)+sizeof(struct AdapterInfo) );

	memset(alladp,0,_len);
	AllAdapters *pInfo=(AllAdapters *)alladp;
	pInfo->m_count=0;
	pInfo->m_adpinfo=NULL;


	if(ERROR_INSUFFICIENT_BUFFER == ::GetIpForwardTable (forward_tab, &size, TRUE)) {
		forward_tab = (PMIB_IPFORWARDTABLE)::GlobalAlloc (GPTR, size);
		if (NO_ERROR != GetIpForwardTable (forward_tab, &size, TRUE)) {
			//Log::report (str (boost::format ("NetScan::get_default_gateway, GetIpForwardTable error: %d") %GetLastError ()));
			res=false;
			goto get_adapters_info_end;
		}
	}



	for (DWORD i = 0; i < forward_tab->dwNumEntries; ++i) {
		if (0 == forward_tab->table[i].dwForwardDest) {
			in_addr in;
			in.s_addr = forward_tab->table[i].dwForwardNextHop;

			char *cGateway=inet_ntoa (in);
			if(!cGateway)
			{
				res=false;
				goto get_adapters_info_end;
			}
			gateway=cGateway;
			pInfo->m_gateway=ntohl(in.s_addr);
			break;
		}
	}



	//step2 列举所有物理网卡
	//NICLIST *niclist;
	//if(EnumInterfaces(&niclist))
	//{
	//	for(int i=0;i<niclist->dwCount;i++)
	//	{
	//		NIC &nic=niclist->lpNics[i];
	//		if(nic.m_dwNicType !=1 && nic.m_dwNicType !=2)
	//		{
	//			continue;
	//		}
	//		AdapterInfo *pCur=(AdapterInfo *)((char *)alladp+sizeof(struct AllAdapters)+(sizeof(AdapterInfo)*pInfo->m_count));
	//		if((char *)pCur+sizeof(AdapterInfo) >(char *)alladp+_len)
	//		{
	//			res=false;
	//			goto get_adapters_info_end;
	//		}
	//		strncpy(pCur->m_adapter_name,nic.m_chDevDesc,sizeof(pCur->m_adapter_name)-1);
	//		strncpy(pCur->m_adapter_uuid,nic.m_chDevGuid,sizeof(pCur->m_adapter_uuid)-1);
	//		pCur->m_default=false;
	//		pCur->m_gateway=0;
	//		pCur->m_index=999+i;
	//		pCur->m_ip=pCur->m_mask=0;
	//		pCur->m_mac[0]=pCur->m_mac[1]=0;
	//		pCur->m_on=false;
	//		pCur->m_type=(nic.m_dwNicType==1)?WIRED_TYPE:WIRELESS_TYPE;

	//		if(!pInfo->m_adpinfo)
	//		{
	//			pInfo->m_adpinfo=pCur;

	//		}

	//		pInfo->m_count++;
	//	}
	//	::FreeNicListBuffer(niclist);

	//}



	//step3 获取网卡的详细信息
	GetAdaptersInfo (adapter, &len);
	original_adapter = adapter = (PIP_ADAPTER_INFO)GlobalAlloc (GPTR, len);
	if (ERROR_SUCCESS != GetAdaptersInfo (adapter, &len)) {
		//Log::report ("NetScan::get_adapters_info, GetAdaptersInfo error");
		res=false;
		goto get_adapters_info_end;
	}



	while (adapter) {
		AdapterInfo *pCur=NULL;
		//for(int i=0;i<pInfo->m_count;i++)
		//{
		//	AdapterInfo *pTemp=(AdapterInfo *)((char *)alladp+sizeof(struct AllAdapters)+(sizeof(AdapterInfo)*i));
		//	if(0==strncmp(pTemp->m_adapter_uuid,((char *)adapter->AdapterName)+1,strlen((char *)adapter->AdapterName)-2))
		//	{
		//		pCur=pTemp;
		//		break;
		//	}
	

		//}
		pCur=ADP_INFO_BUFFER(pInfo->m_count);
		if(pCur && (char *)pCur+sizeof(AdapterInfo)< (char *)alladp+_len)
		{
			if (strlen (adapter->IpAddressList.IpAddress.String)) {
				int itemp[4]={0};
				sscanf(adapter->IpAddressList.IpAddress.String,"%d.%d.%d.%d",&itemp[0], &itemp[1],&itemp[2], &itemp[3] );
				pCur->m_ip=((0xff&itemp[0])<<24)|((0xff&itemp[1])<<16)|((0xff&itemp[2])<<8)|(0xff&itemp[3]);

				sscanf(adapter->IpAddressList.IpMask.String,"%d.%d.%d.%d",&itemp[0], &itemp[1],&itemp[2], &itemp[3] );
				pCur->m_mask=((0xff&itemp[0])<<24)|((0xff&itemp[1])<<16)|((0xff&itemp[2])<<8)|(0xff&itemp[3]);
			}
			memcpy(pCur->m_mac,adapter->Address,min(sizeof(pCur->m_mac),adapter->AddressLength));

		///	pCur->m_index = adapter->Index;


			//HKEY key;
			//DWORD len_temp = 0, type = 0, data = 0;

			//string stemp = NETWORK_CONNECTIONS_KEY;
			//stemp+=adapter->AdapterName;
			//stemp+="\\Connection";

		//	pCur->m_type=WIRED_TYPE;
			//pCur->m_on=true;
			//LONG res = RegOpenKeyEx (HKEY_LOCAL_MACHINE, stemp.c_str (), 0, KEY_READ, &key);
			//if (ERROR_SUCCESS == res) {
			//	len_temp = sizeof (data);
			//	res = RegQueryValueEx (key, "MediaSubType", NULL, &type, (unsigned char *)&data, &len);
			//	pCur->m_type= (ERROR_SUCCESS == res && REG_DWORD == type && (2==data))?WIRELESS_TYPE:WIRED_TYPE;
			//	RegCloseKey (key);
			//}


			//"{DB1738E5-BB9C-49CF-B65B-8E72801F3C06}"
			strncpy(pCur->m_adapter_uuid,(char *)adapter->AdapterName+1,strlen((char *)adapter->AdapterName)-2);

			NDIS_MEDIA_STATE state;
			bool btemp=query_ndis_media_state((char *)adapter->AdapterName,state);
			pCur->m_on=btemp?(state==NdisMediaStateConnected):false;

			PIP_ADDR_STRING pGatewayLoop=&adapter->GatewayList;
			int itemp[4]={0};
			sscanf(adapter->GatewayList.IpAddress.String,"%d.%d.%d.%d",&itemp[0], &itemp[1],&itemp[2], &itemp[3] );
			pCur->m_gateway=((0xff&itemp[0])<<24)|((0xff&itemp[1])<<16)|((0xff&itemp[2])<<8)|(0xff&itemp[3]);

			pCur->m_default=false;
			while (0!= pGatewayLoop && pGatewayLoop->IpAddress.String)
			{
				if(0==strncmp(pGatewayLoop->IpAddress.String,gateway.c_str(),16))
				{
					pCur->m_default=true;
					break;
				}
				pGatewayLoop=pGatewayLoop->Next;
			}




		if(!pInfo->m_adpinfo)
		{
			pInfo->m_adpinfo=pCur;

		}


			pInfo->m_count++;

		}
	
		adapter = adapter->Next;

	}


get_adapters_info_end:
	if (forward_tab) {
		::GlobalFree(forward_tab);
	}

	if(original_adapter)
	{
		GlobalFree (original_adapter);
	}



	//if(alladp)
	//{

	//	*alladp=res?((AllAdapters *)local_mem):NULL;
	//}
	return res;
#undef ADP_INFO_BUFFER
}



// 这是已导出类的构造函数。
// 有关类定义的信息，请参阅 SVTUtils.h
//CSVTUtils::CSVTUtils()
//{
//	return;
//}
