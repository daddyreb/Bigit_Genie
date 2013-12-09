#include <WinSock2.h>
#include <Windows.h>
#include <Ws2tcpip.h>
#include <stdio.h>
#include <iphlpapi.h>
#include <icmpapi.h>


#include "CoreImp.h"

#pragma comment(lib,"iphlpapi.lib")

typedef struct icmp_hdr
{
    unsigned char   icmp_type;		// ��Ϣ����
    unsigned char   icmp_code;		// ����
    unsigned short  icmp_checksum;	// У���
	// �����ǻ���ͷ
    unsigned short  icmp_id;		// ����Ωһ��ʶ�������ID�ţ�ͨ������Ϊ����ID
    unsigned short  icmp_sequence;	// ���к�
    unsigned long   icmp_timestamp; // ʱ���
} ICMP_HDR, *PICMP_HDR;


USHORT checksum(USHORT* buff, int size)
{
	unsigned long cksum = 0;
	while(size>1)
	{
		cksum += *buff++;
		size -= sizeof(USHORT);
	}
	// ������
	if(size)
	{
		cksum += *(UCHAR*)buff;
	}
	// ��32λ��chsum��16λ�͵�16λ��ӣ�Ȼ��ȡ��
	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >> 16);			
	return (USHORT)(~cksum);
}

BOOL SetTimeout(SOCKET s, int nTime, BOOL bRecv)
{
	int ret = ::setsockopt(s, SOL_SOCKET, 
		bRecv ? SO_RCVTIMEO : SO_SNDTIMEO, (char*)&nTime, sizeof(nTime));
	return ret != SOCKET_ERROR;
}

BOOL sleep_return(DWORD dwMillionseconds,int interrupt_flag)
{
	if (interrupt_flag)
	{
		if (sleep_interruptable(dwMillionseconds))
		{
			return TRUE;
		}
	}
	else
	{
		Sleep(dwMillionseconds);
	}

	return FALSE;
}

BOOL Ping(const char* szDestIp,const char* szGatewayIp,int interrupt_flag)
{
	// ����ԭʼ�׽���
	SOCKET sRaw = ::socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if(INVALID_SOCKET==sRaw)
	{
		//int err = WSAGetLastError();
		return FALSE;
	}
	// ���ý��ճ�ʱ
	SetTimeout(sRaw, 4000, TRUE);

	// ����Ŀ�ĵ�ַ
	SOCKADDR_IN dest;
	dest.sin_family				= AF_INET;
	dest.sin_port				= htons(0);
	dest.sin_addr.S_un.S_addr	= inet_addr(szDestIp);

	//---------------NETGEAR Requirments-------------------
	//Gateway ip
	unsigned long GatewayIp		= inet_addr(szGatewayIp);
	unsigned long PseuoIp		= inet_addr("10.0.0.1");
	//---------------NETGEAR Requirments End---------------
	// ����ICMP���
	char		buff[sizeof(ICMP_HDR) + 32];
	ICMP_HDR*	pIcmp = (ICMP_HDR*)buff;
	// ��дICMP�������
	pIcmp->icmp_type		= 8;	// ����һ��ICMP����
	pIcmp->icmp_code		= 0;
	pIcmp->icmp_id			= (USHORT)::GetCurrentProcessId();
	pIcmp->icmp_checksum	= 0;
	pIcmp->icmp_sequence	= 0;
	// ������ݲ��֣�����Ϊ����
	memset(&buff[sizeof(ICMP_HDR)], 'E', 32);
	
	// ��ʼ���ͺͽ���ICMP���
	USHORT		nSeq	= 0;
	char		recvBuf[1024];
	SOCKADDR_IN from;
	int			nLen	= sizeof(from);
	int			nCount	= 0;
	int			nRet	= 0;
	BOOL		bHasOk	= FALSE;

	while(TRUE)
	{
		if(nCount++ == 4)
			break;
		pIcmp->icmp_checksum	= 0;
		pIcmp->icmp_timestamp	= ::GetTickCount();
		pIcmp->icmp_sequence	= nSeq++;
		pIcmp->icmp_checksum	= checksum((USHORT*)buff, sizeof(ICMP_HDR) + 32);

		nRet = ::sendto(sRaw, buff, sizeof(ICMP_HDR) + 32, 0, (SOCKADDR *)&dest, sizeof(dest));
		if(nRet == SOCKET_ERROR)
		{
			//int error = WSAGetLastError();
			if (sleep_return(1000,interrupt_flag))
			{
				break;
			}

			continue;
		}
		nRet = ::recvfrom(sRaw, recvBuf, 1024, 0, (sockaddr*)&from, &nLen);
		if(nRet == SOCKET_ERROR)
		{
			if (sleep_return(1000,interrupt_flag))
			{
				break;
			}
			continue;
		}

		if(((GatewayIp!=dest.sin_addr.S_un.S_addr) && 
			(from.sin_addr.S_un.S_addr==GatewayIp))
			|| ((PseuoIp!=dest.sin_addr.S_un.S_addr) 
				&& (from.sin_addr.S_un.S_addr==PseuoIp)))
		{
			if (sleep_return(1000,interrupt_flag))
			{
				break;
			}
			continue;
		}

		// �������յ���ICMP���
		int nTick = ::GetTickCount();
		// ���յ��������а���IPͷ��IPͷ��СΪ20���ֽڣ����Լ�20�õ�ICMPͷ
		ICMP_HDR* pRecvIcmp = (ICMP_HDR*)(recvBuf + 20); // (ICMP_HDR*)(recvBuf + sizeof(IPHeader));
		if(pRecvIcmp->icmp_type != 0)	// ����
		{
			if (sleep_return(1000,interrupt_flag))
			{
				break;
			}
			continue;
		}

		if(pRecvIcmp->icmp_id != ::GetCurrentProcessId())
		{
			if (sleep_return(1000,interrupt_flag))
			{
				break;
			}
			continue;
		}
		
		bHasOk=TRUE;
		break;
	}//while

	closesocket(sRaw);
	return bHasOk;
}

BOOL satisfy_netgear_haj(const std::list<LPTSTR> &GatewayIps_list,unsigned long DestAddr,unsigned long ReplyAddr)
{
	//---------------NETGEAR Requirments-------------------
	unsigned long PseuoIp		= inet_addr("10.0.0.1");
	//---------------NETGEAR Requirments End---------------

	if ((PseuoIp != DestAddr) && (ReplyAddr == PseuoIp))
	{
		return FALSE;
	}

	for (std::list<LPTSTR>::const_iterator iter = GatewayIps_list.begin() ;
		iter != GatewayIps_list.end() ; ++iter)
	{
		unsigned long tmpaddr = 0;
#if defined UNICODE || defined _UNICODE
		char tmpbuf[512];
		WideCharToMultiByte(CP_ACP,0,*iter,-1,tmpbuf,512,NULL,NULL);
		tmpaddr = inet_addr(tmpbuf);	
#else
		tmpaddr = inet_addr(*iter);
#endif
		if ((tmpaddr != DestAddr) && (ReplyAddr == tmpaddr))
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL ping2(const char* szDestIp,const std::list<LPTSTR> &GatewayIps_list,unsigned long timeoutms)
{

	BOOL bresult = FALSE;

	if (NULL == szDestIp || szDestIp[0] == '\0' || 0 == timeoutms)
	{
		return bresult;
	}

	char			SendData[]	= "icmp echo data";
	DWORD			ReplySize	= 0;
	LPVOID			ReplyBuffer	= NULL;
	DWORD			dwRetVal	= 0;
	unsigned long	ipaddr		= inet_addr(szDestIp);

	HANDLE hIcmpFile = IcmpCreateFile();
	if (INVALID_HANDLE_VALUE == hIcmpFile)
	{
		return bresult;
	}

	ReplySize	= sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
	ReplyBuffer = (VOID*) malloc(ReplySize);

	if (ReplyBuffer == NULL) {
		IcmpCloseHandle(hIcmpFile);
		return bresult;
	}    

	dwRetVal = IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData), 
		NULL, ReplyBuffer, ReplySize, timeoutms);

	if (dwRetVal != 0) {
		PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)ReplyBuffer;
		if((pEchoReply->Status == IP_SUCCESS) && satisfy_netgear_haj(GatewayIps_list,ipaddr,pEchoReply->Address))
		{
			bresult = TRUE;
		}
	}

	free(ReplyBuffer);
	IcmpCloseHandle(hIcmpFile);
	return bresult;
}

