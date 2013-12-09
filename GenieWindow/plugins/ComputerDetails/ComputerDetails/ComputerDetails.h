#ifndef COMPUTERDETAILS_H
#define COMPUTERDETAILS_H

#include "computerdetails_global.h"

#pragma pack(4)

typedef struct  
{
	DWORD			m_dwId;
	BOOL			m_bDisabled;
	DWORD			m_dwNicType;//1 有线网卡,2无线网卡
	TCHAR			m_chDevDesc[512];
	TCHAR			m_chDevGuid[128];
}NIC,*LPNIC;

typedef struct
{
	LPNIC	lpNics;
	DWORD	dwCount;
} NICLIST;


typedef struct  
{
	TCHAR m_Name[128];
	TCHAR m_Value[512];
}INFO, *LPINFO;

typedef struct
{
	LPINFO lpItem;
	int m_Count;
}INFOLIST;


typedef struct
{
	LPTSTR *lpKeyValue;
	int dwCount;
} ItemDataList;

extern "C" COMPUTERDETAILS_EXPORT BOOL  NetworkInterfaces(NICLIST **Niclist);
extern "C" COMPUTERDETAILS_EXPORT void  FreeNicListBuff(NICLIST *niclist);

extern "C" COMPUTERDETAILS_EXPORT BOOL  GetOperationDetails(INFOLIST **InfoList);
extern "C" COMPUTERDETAILS_EXPORT void  FreeOperListBuff(INFOLIST *FreeData);

void ReturnErrorInfo(ItemDataList **ErrorInfo, const std::wstring &sError);
std::list<std::string> splitTolist(std::string &mString, char &mSplit);

#endif // COMPUTERDETAILS_H
