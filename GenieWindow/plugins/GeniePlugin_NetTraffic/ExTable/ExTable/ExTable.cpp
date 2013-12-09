// ExTable.cpp : Defines the exported functions for the DLL application.

#include "ExTable.h"
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "WS2_32.lib")
// ������ID�ţ�PID��ת��Ϊ��������
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
	// �Ҳ����Ļ���Ĭ�Ͻ�����Ϊ��???��
	wcscpy( ProcessName, L"???");
	if(!::Process32First(hProcessSnap, &processEntry)) 
		return ProcessName;
	do 
	{
		if(processEntry.th32ProcessID == ProcessId) // �����������
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
	
	// ��ȡ��չ��������ڵ�ַ	
	HMODULE hModule = ::LoadLibrary(L"iphlpapi.dll");
	pAllocateAndGetTcpExTableFromStack = 
		(PFNAllocateAndGetTcpExTableFromStack)::GetProcAddress(hModule, 
		"AllocateAndGetTcpExTableFromStack");

	
	if(pAllocateAndGetTcpExTableFromStack == NULL )
	{
		// ˵����Ӧ�õ�����ͨ��IP����APIȥ��ȡTCP���ӱ��UDP������
		return FALSE;
	}

	// ������չ��������ȡTCP��չ���ӱ��UDP��չ������

	//PMIB_TCPEXTABLE pTcpExTable;
	// pTcpExTable��pUdpExTable��ָ�Ļ������Զ�����չ�����ڽ��̶�������
	if(pAllocateAndGetTcpExTableFromStack(&pTcpExTable, TRUE, GetProcessHeap(), 2, 2) != 0)
	{
		//printf(" Failed to snapshot TCP endpoints.\n");
		::FreeLibrary(hModule);
		return FALSE;
	}



		// ��ӡ������ڵ���Ϣ
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

	// ������չ����ָ��
	
	PFNAllocateAndGetUdpExTableFromStack pAllocateAndGetUdpExTableFromStack;

	// ��ȡ��չ��������ڵ�ַ	
	HMODULE hModule = ::LoadLibrary(L"iphlpapi.dll");
	
	pAllocateAndGetUdpExTableFromStack = 
		(PFNAllocateAndGetUdpExTableFromStack)::GetProcAddress(hModule, 
		"AllocateAndGetUdpExTableFromStack");

	if(pAllocateAndGetUdpExTableFromStack == NULL)
	{
		//printf(" Ex APIs are not present \n ");
		// ˵����Ӧ�õ�����ͨ��IP����APIȥ��ȡTCP���ӱ��UDP������
		return FALSE;
	}

	// ������չ��������ȡTCP��չ���ӱ��UDP��չ������
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