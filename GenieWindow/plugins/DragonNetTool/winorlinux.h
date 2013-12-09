#ifndef WINORLINUX_H
#define WINORLINUX_H


#include <QString>


#pragma pack(4)

#ifdef WIN32
#include <winsock2.h>
#include <Windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <stdio.h>
#endif


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

typedef struct
{
    char **lpKeyValue;
    int dwCount;
} ReturnDataType;

//#define SysInfo QMap<QString, QString>
#define SysInfo QStringList






/////// Ping
//#ifndef DEBUG_ALL
//#define DEBUG_ALL 0
//#endif
//
//#pragma pack(4)
//
//#ifndef WIN32
//
///* FreeBSD */
////#define _BSD_SOURCE
//
//#define BYTE int
//#define USHORT unsigned short
//#define ULONG unsigned long
//#define UCHAR unsigned char
//
//#define INVALID_SOCKET -1
//#define SOCKET_ERROR -1
//#define SOCKET int
//
//#include <unistd.h>
//#include <string.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <arpa/inet.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/tcp.h>
//#include <netinet/ip.h>
//#include <netinet/in.h>
//#include <netinet/ip_icmp.h>
//#include <netdb.h>
//#include <unistd.h>
//#include <sys/ioctl.h>
//
//
//#ifdef __GLIBC__
//#include <netinet/if_ether.h>
//#else
//#include <linux/if_ether.h>
//#endif
//
//
//#include <net/if_arp.h>
//#include <net/if.h>
//#include <signal.h>
//#include <fcntl.h>
//#include <sys/time.h>
//#else


///* Common */
//#include <stdio.h>
//#include <stdlib.h>
//#include <time.h>


//#define ICMP_ECHO 8
//#define ICMP_ECHOREPLY 0
//#define ICMP_MIN 8
//#define PING_TIMES 3
//#define DEF_PACKET_SIZE 32
//#define MAX_PACKET 1024



//
///* The IP header */
//typedef struct _iphdr {
//  unsigned int h_len:4;          // length of the header
//  unsigned int version:4;        // Version of IP
//  unsigned char tos;             // Type of service
//  unsigned short total_len;      // total length of the packet
//  unsigned short ident;          // unique identifier
//  unsigned short frag_and_flags; // flags
//  unsigned char  ttl;
//  unsigned char proto;           // protocol (TCP, UDP etc)
//  unsigned short checksum;       // IP checksum
//
//  unsigned int sourceIP;
//  unsigned int destIP;
//}IpHeader;
//
///* The ICMP header */
//typedef struct _ihdr {
//  BYTE i_type;
//  BYTE i_code;
//  USHORT i_cksum;
//  USHORT i_id;
//  USHORT i_seq;
//  ULONG timestamp;
//}IcmpHeader;
//
//
///* Linux version of Windows functions */
//#ifndef WIN32
//
//#define GetCurrentProcessId() getpid()
//
//unsigned long GetTickCount(){
//  struct timeval tv;
//  gettimeofday(&tv, NULL);
//  return ((tv.tv_sec*1000000)+(tv.tv_usec))/1000; // msec
//};
//#endif
//
//
//
//USHORT CheckSum(USHORT *buffer, int size)
//{
//    unsigned long cksum=0;
//
//    while(size >1) {
//      cksum+=*buffer++;
//      size -=sizeof(USHORT);
//    }
//
//    if(size ) {
//      cksum += *(UCHAR*)buffer;
//    }
//
//    cksum = (cksum >> 16) + (cksum & 0xffff);
//    cksum += (cksum >>16);
//    return (USHORT)(~cksum);
//}
//
//void FillIcmpData(char * i_data, int datasize)
//{
//
//    IcmpHeader *icmp_hdr;
//    char *datapart;
//
//    icmp_hdr = (IcmpHeader*)i_data;
//
//    icmp_hdr->i_type = ICMP_ECHO;
//    icmp_hdr->i_code = 0;
//    icmp_hdr->i_id = (USHORT)GetCurrentProcessId();
//    icmp_hdr->i_cksum = 0;
//    icmp_hdr->i_seq = 0;
//
//    datapart = i_data + sizeof(IcmpHeader);
//
//    memset(datapart, 'E', datasize - sizeof(IcmpHeader));
//    //strncpy(datapart, "Whee, i`m a fast packet.", datasize - sizeof(IcmpHeader));
//
//}











//typedef struct
//{
//        int			m_id;
//        bool		m_disabled;
//        int			m_nictype;//1 有线网卡,2无线网卡
//        QString		m_devdesc;
//        QString		m_devguid;
//}NCard;

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









#endif // WINORLINUX_H
