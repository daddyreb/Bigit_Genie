#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include "DiagnoseDll.h"
#include "WiFiApi.h"

//#include <Rpc.h>
//#include<wlanapi.h>
//#include <list>
//#pragma comment(lib,"wlanapi")
//#pragma comment (lib,"Rpcrt4")
//#if defined _UNICODE || defined UNICODE
//#define TString		std::wstring
//#define RPC_TSTR	RPC_WSTR
//#else
//#define TString		std::string
//#define RPC_TSTR	RPC_CSTR
//#endif

using namespace std;

//BOOL GetWlanInterfaces(std::list<TString> &infList)
//{
//	PWLAN_INTERFACE_INFO_LIST	pIntfList	= NULL;
//	DWORD						dwVersion	= 0;
//	HANDLE						hClient		= NULL;
//	RPC_TSTR					strUuid		= NULL;
//	BOOL						bRet		= TRUE;
//
//	infList.clear();
//
//	if (WlanOpenHandle(WLAN_API_VERSION,NULL,&dwVersion,&hClient) == ERROR_SUCCESS)
//	{
//		if (WlanEnumInterfaces(hClient,NULL,&pIntfList) == ERROR_SUCCESS)
//		{
//			for (DWORD i = 0 ; i < pIntfList->dwNumberOfItems ; ++i)
//			{
//				if (UuidToString(&pIntfList->InterfaceInfo[i].InterfaceGuid,&strUuid) == RPC_S_OK)
//				{
//					infList.push_back(TString((TCHAR*)strUuid));
//					RpcStringFree(&strUuid);
//				}
//				else
//				{
//					bRet = FALSE;
//				}
//			}
//
//			WlanFreeMemory(pIntfList);
//		}
//
//		WlanCloseHandle(hClient,NULL);
//	}
//
//	return bRet;
//}

int main()
{
	//NICLIST *nic_list = NULL;
	//StrList *sl		  = NULL;

	//int g_flag = 0;

	//if (!EnumInterfaces(&nic_list) || NULL == nic_list || 0 == nic_list->dwCount)
	//{
	//	if (nic_list)
	//	{
	//		FreeNicListBuffer(nic_list);
	//		nic_list = NULL;
	//	}

	//	return 0;
	//}

	//for (DWORD k = 0;k<nic_list->dwCount;++k)
	//{
	//	if (nic_list->lpNics[k].m_dwNicType == 2)
	//	{
	//		//if (GetWlanProfileList(nic_list->lpNics[k].m_chDevGuid,&sl))
	//		//{
	//		//	cout << "get wlan profile list ok" << endl;

	//		//	for (DWORD x = 0;x < sl->dwCount;++x)
	//		//	{
	//		//		wcout << sl->strs[x] << endl;
	//		//		/*if (DeleteWlanProfile(nic_list->lpNics[k].m_chDevGuid,sl->strs[x]))
	//		//		{
	//		//			wcout << L"delete profile " << sl->strs[x] << L" ok" << endl;
	//		//		}*/
	//		//		if (1 == x)
	//		//		{
	//		//			if (SetWlanProfilePosition(nic_list->lpNics[k].m_chDevGuid,sl->strs[x],0))
	//		//			{
	//		//				wcout << L"move profile " << sl->strs[x] << " first ok" << endl;
	//		//			}
	//		//		}
	//		//	}
	//		//	FreeStrListBuffer(sl);
	//		//	sl = NULL;
	//		//}
	//		//SetWlanInterfaceProfile(nic_list->lpNics[k].m_chDevGuid,L"WNDR-3400",L"WPA2PSK",L"AES",L"12345678",TRUE);
	//		
	//		StrList *sl = NULL;

	//		wcout << GetWlanInterfaceProfile(nic_list->lpNics[k].m_chDevGuid,L"NETGEAR_WNDR3700_Online",&sl);

	//		wcout << sl->strs[0];

	//		FreeStrListBuffer(sl);

	//		break;
	//	}
	//}
	

	//std::list<TString> infList;
	//GetWlanInterfaces(infList);

	//HINSTANCE	hDhcpDll = NULL;
	//if((hDhcpDll = LoadLibrary(TEXT("dhcpcsvc"))) == NULL)
	//	return FALSE;
	return 0;
}

