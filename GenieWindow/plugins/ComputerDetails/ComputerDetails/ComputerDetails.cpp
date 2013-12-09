
// DiagnoseDll.cpp : Defines the exported functions for the DLL application.
//

#include "GetDetails.h"
#include "ComputerDetails.h"



#include <setupapi.h> 
#include <cfgmgr32.h>

//#include <Iphlpapi.h>
#include <Rpc.h>

#include <winioctl.h>
#include <ntddndis.h>

#pragma comment (lib,"setupapi")
#pragma comment (lib,"Rpcrt4")
#pragma comment(lib,"Ws2_32")
//#pragma comment(lib,"Iphlpapi")
#pragma comment (lib,"Wlanapi")

//功能：	枚举机器上的所有的物理网卡(不包括虚拟网卡)
//参数：	NIC结构体的列表，调用成功包含所有的物理网卡的相关系统
//返回值：	调用成功返回TRUE，否则返回FALSE
//调用成功后必须调用FreeNicListBuffer释放内存
 BOOL  NetworkInterfaces(NICLIST **Niclist)
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

	//if (INVALID_HANDLE_VALUE == (hDevInfo =SetupDiGetClassDevs(&GUID_NDIS_LAN_CLASS/*NULL*/,NULL,NULL,	DIGCF_DEVICEINTERFACE/*DIGCF_PRESENT |DIGCF_ALLCLASSES*/))) 
	if (INVALID_HANDLE_VALUE == (hDevInfo =SetupDiGetClassDevs(&GUID_NDIS_LAN_CLASS, NULL, NULL, DIGCF_DEVICEINTERFACE))) 
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
			if ( ((Status & DN_NO_SHOW_IN_DM)==0)&&(!IsClassHidden(&DeviceInfoData.ClassGuid)) )
			//if ( ((Status & DN_NO_SHOW_IN_DM)==0) )
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
					std::list<TString>	GuidList;
					int					iMedia		=0;
					if (GetNicGuid(szPnpInstanceId,GuidList)&&(!GuidList.empty()))
					{
						//CurNic.m_strDevGuid=GuidList.front();
						_tcscpy(CurNic.m_chDevGuid,GuidList.front().c_str());

						if ((iMedia=GetNicMediaType(GuidList.front()))>0)
						{
							CurNic.m_dwNicType=iMedia;
						}
					}

				}

				if (IsPhysicalNic(/*CurNic.m_strDevGuid.c_str()*/CurNic.m_chDevGuid))
				{
					//TString strDevGuid=CurNic.m_chDevGuid;
					//_tcscpy(CurNic.m_chDevGuid,strDevGuid.substr(1,36).c_str());//去掉GUID的大括号

					nics_list.push_back(CurNic);
				}

			}//if
		}//if
	} //for


	if (nics_list.size()!=0)
	{
		*Niclist=new NICLIST;
		NICLIST *lpTemp=*Niclist;
		lpTemp->dwCount	=nics_list.size();
		lpTemp->lpNics		=new NIC[lpTemp->dwCount];
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


 void  FreeNicListBuff(NICLIST *niclist)
{
	if (0==niclist)
	{
		return;
	}

	delete [] niclist->lpNics;
	delete	  niclist;
}


  BOOL GetOperationDetails(INFOLIST **InfoList)
{
	if ( NULL==InfoList )
		return FALSE;
	else{
		*InfoList = NULL;
		*InfoList = new INFOLIST;
	}
	INFOLIST *TempList;
	TempList = *InfoList;


	std::wstring wItemString;
	std::wstring wNameString;
	INFO m_Item;
	std::list<INFO> Info_List;




	//OSVERSIONINFO osvi;
	OSVERSIONINFOEX osvi;
	//BOOL bIsWindowsXPorLater;

	//ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	//osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	//GetVersionEx(&osvi);
	if ( !GetVersionEx((LPOSVERSIONINFO)&osvi) ){
	//if ( GetVersionEx(&osvi) == 0 ){

		TempList->m_Count = 1;
		TempList->lpItem = new INFO[TempList->m_Count];
		_tcscpy(TempList->lpItem[0].m_Name, TEXT("Failed"));
		_tcscpy(TempList->lpItem[0].m_Value, TEXT("Call Function"));

		return false;
	}


	wNameString = TEXT("Caption");
	_tcscpy(m_Item.m_Name, wNameString.c_str());
	if ( osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS ){
		if ( osvi.dwMajorVersion==4 ){
			if ( osvi.dwMinorVersion==0 ){
				wItemString = TEXT("Windows 95");
			}else if ( osvi.dwMinorVersion==10 ){
				wItemString = TEXT("Windows 98");
			}else if ( osvi.dwMinorVersion==90 ){
				wItemString = TEXT("Windows Me");
			}else {
				wItemString = TEXT("No Name");
			}
		}else {
			wItemString = TEXT("No Name");
		}
	}else if ( osvi.dwPlatformId == VER_PLATFORM_WIN32_NT ){
		//wItemString = TEXT("Windows XP");
		if( osvi.dwMajorVersion==5 ){
			if ( osvi.dwMinorVersion==0 ){
				wItemString = TEXT("Windows 2000");
			}else if ( osvi.dwMinorVersion==1 ){
				wItemString = TEXT("Windows XP");
			}else if ( osvi.dwMinorVersion==2 ){
				//if ( GetSystemMetrics(SM_SERVERR2) == 0 )
				wItemString = TEXT("Windows Home Server");
				if ( osvi.wProductType == VER_NT_WORKSTATION )
					wItemString = TEXT("Windows XP Professional");
				if ( GetSystemMetrics(SM_SERVERR2) == 0 )
					wItemString = TEXT("Windows Server 2003");
				//SYSTEM_INFO.w == VER_SUITE_WH_SERVER

			}else {
				wItemString = TEXT("No Name");
			}

		}else if ( osvi.dwMajorVersion==6 ){
			if ( osvi.dwMinorVersion==0 ){
				if ( osvi.wProductType == VER_NT_WORKSTATION )
					wItemString = TEXT("Windows Vista");
				else
					wItemString = TEXT("Windows Server 2008");
			}else if ( osvi.dwMinorVersion==1 ){
				if ( osvi.wProductType == VER_NT_WORKSTATION )
					wItemString = TEXT("Windows 7");
				else
					wItemString = TEXT("Windows Server 2008 R2");
			}else if( osvi.dwMinorVersion==2 )
			{
				if(osvi.wProductType ==VER_NT_WORKSTATION)
				{
					wItemString = TEXT("Windows 8");
				}else
				{}
			}
			else {
				wItemString = TEXT("No Name");
			}
		}else {
			wItemString = TEXT("No Name");
		}


	}else if ( osvi.dwPlatformId == VER_PLATFORM_WIN32s ){
		wItemString = TEXT("Windows 3.1");
	}else {
		wItemString = TEXT("No Name");
	}
	_tcscpy(m_Item.m_Value, wItemString.c_str());
	Info_List.push_back(m_Item);

	wNameString = TEXT("CSDVersion");
	_tcscpy(m_Item.m_Name, wNameString.c_str());
	wItemString = osvi.szCSDVersion;
	if ( wItemString.empty() )
		wItemString = TEXT("No Service Pack Installed");
	_tcscpy(m_Item.m_Value, wItemString.c_str());
	//m_Item.m_Name = TEXT("Opertion");
	//m_Item.m_Value = osvi.szCSDVersion;
	Info_List.push_back(m_Item);

	wNameString = TEXT("WindowsDirectory");
	_tcscpy(m_Item.m_Name, wNameString.c_str());
	UINT wPathsize = GetWindowsDirectory(NULL,0);
	wchar_t *wPath = new wchar_t[wPathsize];
	if( GetWindowsDirectory(wPath, wPathsize)!=0 )
		wItemString = wPath;
	else
		wItemString = TEXT("Not find Windows Directory");
	delete [] wPath;
	_tcscpy(m_Item.m_Value, wItemString.c_str());
	Info_List.push_back(m_Item);




	if (Info_List.size()!=0){
		//*InfoList = new INFOLIST;
		//INFOLIST *TempList = *InfoList;

		TempList->m_Count = Info_List.size();
		TempList->lpItem = new INFO[TempList->m_Count];
		int k=0;
		for (std::list<INFO>::iterator i=Info_List.begin(); i!=Info_List.end(); ++i,++k)
		{
			_tcscpy(TempList->lpItem[k].m_Name, i->m_Name);
			_tcscpy(TempList->lpItem[k].m_Value, i->m_Value);
		}

		return TRUE;
	}else {
		TempList->m_Count = 1;
		TempList->lpItem = new INFO[TempList->m_Count];
		_tcscpy(TempList->lpItem[0].m_Name, TEXT("Error"));
		_tcscpy(TempList->lpItem[0].m_Value, TEXT("Not Find"));
		return FALSE;
	}

}





void  FreeOperListBuff(INFOLIST *FreeData)
{

	if ( 0 == FreeData )
		return;

	////for (int i=0; i<FreeData->m_Count; i++){
	////	delete [] FreeData->lpItem[i].m_Name;
	////	delete [] FreeData->lpItem[i].m_Value;
	////}

	delete [] FreeData->lpItem;

	delete FreeData;



	//INFOLIST *TempList;
	//TempList = *FreeData;

	//delete [] TempList->lpItem;
	//delete FreeData;


}


void ReturnErrorInfo(ItemDataList **ErrorInfo, const std::wstring &sError)
{
	if ( *ErrorInfo == NULL )
		*ErrorInfo = new ItemDataList;

	//std::wstring printString = _T("Failed");
	std::wstring printString = sError;

	ItemDataList *lpErrData = *ErrorInfo;
	lpErrData->dwCount = 1;
	lpErrData->lpKeyValue = NULL;

	lpErrData->lpKeyValue = new LPTSTR[lpErrData->dwCount];
	TCHAR *etemp = new TCHAR[printString.length()+1];
	_tcscpy(etemp, printString.c_str());
	lpErrData->lpKeyValue[0] = etemp;

}


std::list<std::string> splitTolist(std::string &mString, char &mSplit)
{
	std::list<std::string> relist;

	std::string::size_type pos1, pos2;
	pos2 = mString.find(mSplit);
	pos1 = 0;        
	while (std::string::npos != pos2)
	{
		relist.push_back(mString.substr(pos1, pos2 - pos1));

		pos1 = pos2 + 1;
		pos2 = mString.find(mSplit, pos1);
	}
	relist.push_back(mString.substr(pos1));

	return relist;
}




