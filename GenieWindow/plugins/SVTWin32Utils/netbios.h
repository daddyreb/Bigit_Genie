#ifndef NETBIOS_H
#define NETBIOS_H
#include <winsock2.h>
#include <string>
#include <algorithm>
#include "SVTUtils.h"


//using namespace std;
//
#define HOSTNAMELEN 17
//#define IPADDRLEN 16
//#define HOSTTYPELEN 16

SVTUTILS_API int get_device_info (const unsigned long &ip,const char * callingname,const char *calledname,char (&result)[NETBIOS_PARA_LEN]);
SVTUTILS_API int get_device_info2 (const unsigned long &ip,char (&hostname)[NETBIOS_PARA_LEN],char (&version)[NETBIOS_PARA_LEN]);
//struct HostInfo
//{
//	char m_ip[IPADDRLEN];
//	char m_calledname[HOSTNAMELEN];
//	char m_callingname[HOSTNAMELEN];
//	char m_type[HOSTTYPELEN];
//	char m_version[HOSTVERSION];
//};
//
//struct HostInfo2
//{
//	char m_ip[IPADDRLEN];
//	char m_hostname[255];
//	char m_type[HOSTTYPELEN];
//	char m_version[HOSTVERSION];
//};
#endif //NETBIOS_H