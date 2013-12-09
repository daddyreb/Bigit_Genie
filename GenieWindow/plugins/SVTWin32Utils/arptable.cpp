#include "arptable.h"


//////////////////////////////////////////////////////////////////////
// IPArp.cpp�ļ�


#include <stdio.h>
#include <windows.h>
#include <Iphlpapi.h>

#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "WS2_32.lib")


PMIB_IPNETTABLE MyGetIpNetTable(BOOL bOrder);
void MyFreeIpNetTable(PMIB_IPNETTABLE pIpNetTable);
PMIB_IPADDRTABLE MyGetIpAddrTable(BOOL bOrder);
void MyFreeIpAddrTable(PMIB_IPADDRTABLE pIpAddrTable);
BOOL InterfaceIdxToInterfaceIp(PMIB_IPADDRTABLE pIpAddrTable, DWORD dwIndex, char str[]);


// ����IP��ַ�����ӿ�����ת��ΪIP��ַ
// pIpAddrTable��IP��ַ��
// dwIndex�ǽӿ�����
// ����ִ�гɹ�֮��str�������ӿڵ�IP��ַ
BOOL InterfaceIdxToInterfaceIp(PMIB_IPADDRTABLE pIpAddrTable, DWORD dwIndex, char str[])
{
    char* szIpAddr;

    if(pIpAddrTable == NULL ||  str == NULL)
        return FALSE;
    str[0] = '\0';
    // ����IP��ַ����������dwIndex��Ӧ��IP��ַ
    for(DWORD dwIdx = 0; dwIdx < pIpAddrTable->dwNumEntries; dwIdx++)
    {
        if(dwIndex == pIpAddrTable->table[dwIdx].dwIndex)
        {
            // ���ַ�������ʽ���ز�ѯ���
            szIpAddr = inet_ntoa(*((in_addr*)&pIpAddrTable->table[dwIdx].dwAddr));
            if(szIpAddr)
            {
                strcpy(str, szIpAddr);
                return TRUE;
            }
            else
                return FALSE;
        }
    }
    return FALSE;
}


// ��ȡIP��ַ����������ӳ���ϵ����ARP��

PMIB_IPNETTABLE MyGetIpNetTable(BOOL bOrder)
{
    PMIB_IPNETTABLE pIpNetTable = NULL;
    DWORD dwActualSize = 0;

    // ��ѯ���軺�����Ĵ�С
    if(::GetIpNetTable(pIpNetTable,
                       &dwActualSize, bOrder) == ERROR_INSUFFICIENT_BUFFER)
    {
        // ΪMIB_IPNETTABLE�ṹ�����ڴ�
        pIpNetTable = (PMIB_IPNETTABLE)::GlobalAlloc(GPTR, dwActualSize);
        // ��ȡARP��
        if(::GetIpNetTable(pIpNetTable,
                           &dwActualSize, bOrder) == NO_ERROR)
        {
            return pIpNetTable;
        }
        ::GlobalFree(pIpNetTable);
    }
    return NULL;
}

void MyFreeIpNetTable(PMIB_IPNETTABLE pIpNetTable)
{
    if(pIpNetTable != NULL)
        ::GlobalFree(pIpNetTable);
}


PMIB_IPADDRTABLE MyGetIpAddrTable(BOOL bOrder)
{
    PMIB_IPADDRTABLE pIpAddrTable = NULL;
    DWORD dwActualSize = 0;

    // ��ѯ���軺�����Ĵ�С
    if(::GetIpAddrTable(pIpAddrTable,
                        &dwActualSize, bOrder) == ERROR_INSUFFICIENT_BUFFER)
    {
        // ΪMIB_IPADDRTABLE�ṹ�����ڴ�
        pIpAddrTable = (PMIB_IPADDRTABLE)::GlobalAlloc(GPTR, dwActualSize);
        // ��ȡIP��ַ��
        if(::GetIpAddrTable(pIpAddrTable,
                            &dwActualSize, bOrder) == NO_ERROR)
            return pIpAddrTable;
        ::GlobalFree(pIpAddrTable);
    }
    return NULL;
}

void MyFreeIpAddrTable(PMIB_IPADDRTABLE pIpAddrTable)
{
    if(pIpAddrTable != NULL)
        ::GlobalFree(pIpAddrTable);
}



















void PrintIpAddrTable()
{
    DWORD i;
    struct in_addr inadTmp1;
    struct in_addr inadTmp2;
    char szAddr[128];
    char szMask[128];

    PMIB_IPADDRTABLE pIpAddrTable = MyGetIpAddrTable(TRUE);

    if (pIpAddrTable == NULL)
    {
        printf( "pIpAddrTable == NULL in line %d\n", __LINE__);
        return;
    }
    printf("ipAdEntAddr\t ifAdEntIfIndex\t ipAdEntNetMask\t ipAdEntBcastAddr\t ipAdEntReasmMaxSize\n");
    for (i = 0; i < pIpAddrTable->dwNumEntries; ++i)
    {
	inadTmp1.s_addr = pIpAddrTable->table[i].dwAddr;
	strcpy(szAddr, inet_ntoa(inadTmp1));
	inadTmp2.s_addr = pIpAddrTable->table[i].dwMask;
	strcpy(szMask, inet_ntoa(inadTmp2));
	printf("  %s\t 0x%X\t %s\t %s\t %u\n",
               szAddr,
               pIpAddrTable->table[i].dwIndex,
               szMask,
               (pIpAddrTable->table[i].dwBCastAddr ? "255.255.255.255" : "0.0.0.0"),
               pIpAddrTable->table[i].dwReasmSize);

    }

    MyFreeIpAddrTable(pIpAddrTable);
}















SVTUTILS_API int loop_arp_table (const unsigned long &iface_idx,char * retbuffer,int buffer_size)
{
    int retnum=0;
    //  PrintIpAddrTable();

    // ���Ȼ�ȡARP��
    PMIB_IPNETTABLE pIpNetTable = MyGetIpNetTable(TRUE);
	PMIB_IPADDRTABLE pIpAddrTable = MyGetIpAddrTable(TRUE);
	unsigned long mask;
	unsigned long ip;
    if (pIpNetTable == NULL)
    {
        printf( "pIpNetTable == NULL in line %d\n", __LINE__);
        goto ret_flag;
    }

    // ��ȡIP��ַ���Ա��������ARP�����еĽӿ�����ת��ΪIP��ַ


	bool iface_finded=false;
	for(DWORD dwIdx = 0; dwIdx < pIpAddrTable->dwNumEntries; dwIdx++)
	{
		if(iface_idx == pIpAddrTable->table[dwIdx].dwIndex)
		{
			mask=pIpAddrTable->table[dwIdx].dwMask;
			ip=pIpAddrTable->table[dwIdx].dwAddr;
			
			iface_finded=true;
		}
	}

	if(!iface_finded)
	{

		goto ret_flag;
	}



    // ��ӡ������ΪdwCurrIndex���������ϵ�ARP����
    for(int i = 0; i < pIpNetTable->dwNumEntries; ++i)
    {
        if(sizeof(unsigned long)*(retnum+1)>buffer_size)
        {
            break;
        }
		if(pIpNetTable->table[i].dwIndex==iface_idx 
			&& (
				pIpNetTable->table[i].bPhysAddr[0]!=0
				||pIpNetTable->table[i].bPhysAddr[1]!=0
				||pIpNetTable->table[i].bPhysAddr[2]!=0
				||pIpNetTable->table[i].bPhysAddr[3]!=0
				||pIpNetTable->table[i].bPhysAddr[4]!=0
				||pIpNetTable->table[i].bPhysAddr[5]!=0
				||pIpNetTable->table[i].bPhysAddr[6]!=0
				)	
			)
		{
			unsigned long &testip=pIpNetTable->table[i].dwAddr;
			if((testip &mask)==(ip &mask) && (testip &~mask)!=0 && (testip &~mask)!=~mask)
			{
				*((unsigned long *)(retbuffer+sizeof(unsigned long)*retnum))=ntohl(testip);
				retnum++;
			}

		}

		//struct in_addr inadTmp;
		//inadTmp.s_addr = pIpNetTable->table[i].dwAddr;
		//printf("  %-16s      %-17s     %-11s\n", inet_ntoa(inadTmp), "", "");



    }
ret_flag:
	MyFreeIpAddrTable(pIpAddrTable);
    MyFreeIpNetTable(pIpNetTable);
    return retnum;
}
