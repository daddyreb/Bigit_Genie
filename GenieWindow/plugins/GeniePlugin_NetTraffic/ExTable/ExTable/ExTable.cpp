// ExTable.cpp : Defines the exported functions for the DLL application.

#include "ExTable.h"
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "WS2_32.lib")
// 将进程ID号（PID）转化为进程名称
extern "C" EXTABLE_API PWCHAR __stdcall ProcessPidToName(/*HANDLE hProcessSnap,*/ DWORD ProcessId, PWCHAR ProcessName)
{

	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(hProcessSnap == INVALID_HANDLE_VALUE)
	{
		//printf(" Failed to take process snapshot. Process names will not be shown.\n\n");
		//return -1;
	}
	PROCESSENTRY32 processEntry;
	processEntry.dwSize = sizeof(processEntry);
	// 找不到的话，默认进程名为“???”
	wcscpy( ProcessName, L"???");
	if(!::Process32First(hProcessSnap, &processEntry)) 
		return ProcessName;
	do 
	{
		if(processEntry.th32ProcessID == ProcessId) // 就是这个进程
		{
			wcscpy(ProcessName, processEntry.szExeFile);
			break;
		}
	}
	while(::Process32Next(hProcessSnap, &processEntry));

	::CloseHandle(hProcessSnap);

	return ProcessName;
}

extern "C" EXTABLE_API BOOL __stdcall getTcpExTable(PMIB_TCPEXTABLE &pTcpExTable)
{
	PFNAllocateAndGetTcpExTableFromStack pAllocateAndGetTcpExTableFromStack;
	
	// 获取扩展函数的入口地址	
	HMODULE hModule = ::LoadLibrary(L"iphlpapi.dll");
	pAllocateAndGetTcpExTableFromStack = 
		(PFNAllocateAndGetTcpExTableFromStack)::GetProcAddress(hModule, 
		"AllocateAndGetTcpExTableFromStack");

	
	if(pAllocateAndGetTcpExTableFromStack == NULL )
	{
		// 说明你应该调用普通的IP帮助API去获取TCP连接表和UDP监听表
		return FALSE;
	}

	// 调用扩展函数，获取TCP扩展连接表和UDP扩展监听表

	//PMIB_TCPEXTABLE pTcpExTable;
	// pTcpExTable和pUdpExTable所指的缓冲区自动由扩展函数在进程堆中申请
	if(pAllocateAndGetTcpExTableFromStack(&pTcpExTable, TRUE, GetProcessHeap(), 2, 2) != 0)
	{
		//printf(" Failed to snapshot TCP endpoints.\n");
		::FreeLibrary(hModule);
		return FALSE;
	}



		// 打印出此入口的信息
		//printf("%-5s %s:%d\n      State:   %s\n", "[TCP]", 
		//	ProcessPidToName(/*hProcessSnap, */pTcpExTable->table[i].dwProcessId, szProcessName),
		//	pTcpExTable->table[i].dwProcessId,
		//	strState);

		//printf("      Local:   %s\n      Remote:  %s\n",
		//	szLocalAddr, szRemoteAddr);



	::FreeLibrary(hModule);
	return TRUE;
}

extern "C" EXTABLE_API BOOL __stdcall getUdpExTable(PMIB_UDPEXTABLE &pUdpExTable)
{

	// 定义扩展函数指针
	
	PFNAllocateAndGetUdpExTableFromStack pAllocateAndGetUdpExTableFromStack;

	// 获取扩展函数的入口地址	
	HMODULE hModule = ::LoadLibrary(L"iphlpapi.dll");
	
	pAllocateAndGetUdpExTableFromStack = 
		(PFNAllocateAndGetUdpExTableFromStack)::GetProcAddress(hModule, 
		"AllocateAndGetUdpExTableFromStack");

	if(pAllocateAndGetUdpExTableFromStack == NULL)
	{
		//printf(" Ex APIs are not present \n ");
		// 说明你应该调用普通的IP帮助API去获取TCP连接表和UDP监听表
		return FALSE;
	}

	// 调用扩展函数，获取TCP扩展连接表和UDP扩展监听表
	//PMIB_UDPEXTABLE pUdpExTable;
	if(pAllocateAndGetUdpExTableFromStack(&pUdpExTable, TRUE, GetProcessHeap(), 2, 2) != 0)
	{
		//printf(" Failed to snapshot UDP endpoints.\n");
		::FreeLibrary(hModule);
		return FALSE;
	}
	::FreeLibrary(hModule);
	return TRUE;
}