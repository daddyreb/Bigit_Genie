#include "arptable.h"


//////////////////////////////////////////////////////////////////////
// IPArp.cpp文件


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


// 根据IP地址表，将接口索引转化为IP地址
// pIpAddrTable是IP地址表
// dwIndex是接口索引
// 函数执行成功之后，str将包含接口的IP地址
BOOL InterfaceIdxToInterfaceIp(PMIB_IPADDRTABLE pIpAddrTable, DWORD dwIndex, char str[])
{
    char* szIpAddr;

    if(pIpAddrTable == NULL ||  str == NULL)
        return FALSE;
    str[0] = '\0';
    // 遍历IP地址表，查找索引dwIndex对应的IP地址
    for(DWORD dwIdx = 0; dwIdx < pIpAddrTable->dwNumEntries; dwIdx++)
    {
        if(dwIndex == pIpAddrTable->table[dwIdx].dwIndex)
        {
            // 以字符串的形式返回查询结果
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


// 获取IP地址到适配器的映射关系，即ARP表

PMIB_IPNETTABLE MyGetIpNetTable(BOOL bOrder)
{
    PMIB_IPNETTABLE pIpNetTable = NULL;
    DWORD dwActualSize = 0;

    // 查询所需缓冲区的大小
    if(::GetIpNetTable(pIpNetTable,
                       &dwActualSize, bOrder) == ERROR_INSUFFICIENT_BUFFER)
    {
        // 为MIB_IPNETTABLE结构申请内存
        pIpNetTable = (PMIB_IPNETTABLE)::GlobalAlloc(GPTR, dwActualSize);
        // 获取ARP表
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

    // 查询所需缓冲区的大小
    if(::GetIpAddrTable(pIpAddrTable,
                        &dwActualSize, bOrder) == ERROR_INSUFFICIENT_BUFFER)
    {
        // 为MIB_IPADDRTABLE结构申请内存
        pIpAddrTable = (PMIB_IPADDRTABLE)::GlobalAlloc(GPTR, dwActualSize);
        // 获取IP地址表
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

    // 首先获取ARP表
    PMIB_IPNETTABLE pIpNetTable = MyGetIpNetTable(TRUE);
	PMIB_IPADDRTABLE pIpAddrTable = MyGetIpAddrTable(TRUE);
	unsigned long mask;
	unsigned long ip;
    if (pIpNetTable == NULL)
    {
        printf( "pIpNetTable == NULL in line %d\n", __LINE__);
        goto ret_flag;
    }

    // 获取IP地址表，以便根据它将ARP表项中的接口索引转化为IP地址


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



    // 打印出索引为dwCurrIndex的适配器上的ARP表项
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
