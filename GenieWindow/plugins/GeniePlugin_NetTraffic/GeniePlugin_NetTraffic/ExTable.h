#ifndef EXTABLE_H
#define EXTABLE_H

#include <stdio.h>
#include <windows.h>
#include <Iphlpapi.h>
#include <tlhelp32.h>



// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DIAGNOSEDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DIAGNOSEDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef  EXTABLE_EXPORTS
#define EXTABLE_API __declspec(dllexport)
#else
#define EXTABLE_API __declspec(dllimport)
#endif


typedef struct 
{
	DWORD   dwState;        	// 连接状态
	DWORD   dwLocalAddr;    	// 本地地址
	DWORD   dwLocalPort;    	// 本地端口
	DWORD   dwRemoteAddr;   	// 远程地址
	DWORD   dwRemotePort;   	// 远程端口
	DWORD	dwProcessId;		// 进程ID号
} MIB_TCPEXROW, *PMIB_TCPEXROW;

typedef struct 
{
	DWORD			dwNumEntries;
	MIB_TCPEXROW	table[ANY_SIZE];
} MIB_TCPEXTABLE, *PMIB_TCPEXTABLE;

typedef struct 
{
	DWORD   dwLocalAddr;    	// 本地地址
	DWORD   dwLocalPort;    	// 本地端口
	DWORD	  dwProcessId;		// 进程ID号
} MIB_UDPEXROW, *PMIB_UDPEXROW;

typedef struct 
{
	DWORD			dwNumEntries;
	MIB_UDPEXROW	table[ANY_SIZE];
} MIB_UDPEXTABLE, *PMIB_UDPEXTABLE;




// 扩展函数原型
typedef DWORD (WINAPI *PFNAllocateAndGetTcpExTableFromStack)(
	//PMIB_TCPEXTABLE *pTcpTable, 
	//BOOL bOrder,             
	//HANDLE heap,
	//DWORD zero,
	//DWORD flags

	PVOID pTcpTable,
	PDWORD pdwSize,
	BOOL bOrder,
	ULONG ulAf,
	TCP_TABLE_CLASS TableClass,
	ULONG Reserved
	);
/*
typedef DWORD (WINAPI *PFNAllocateAndGetUdpExTableFromStack)(
	//PMIB_UDPEXTABLE *pUdpTable,  
	//BOOL bOrder,              
	//HANDLE heap,
	//DWORD zero,
	//DWORD flags

	PVOID pUdpTable,
	PDWORD pdwSize,
	BOOL bOrder,
	ULONG ulAf,
	UDP_TABLE_CLASS TableClass,
	ULONG Reserved
	);
*/
extern "C" EXTABLE_API BOOL __stdcall getTcpExTable(PMIB_TCPEXTABLE &pTcpExTable);

extern "C" EXTABLE_API BOOL __stdcall getUdpExTable(PMIB_UDPEXTABLE &pUdpExTable);

extern "C" EXTABLE_API PWCHAR __stdcall ProcessPidToName(/*HANDLE hProcessSnap,*/ DWORD ProcessId, PWCHAR ProcessName);

#endif
