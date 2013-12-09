#include "netbios.h"


#define TCP_BUFFER 1460
/*
const static char session_request[] =   
	"\x81\x00\x00\x44\x20\x43\x4b\x46\x44\x45\x4e\x45\x43\x46\x44\x45"   
	"\x46\x46\x43\x46\x47\x45\x46\x46\x43\x43\x41\x43\x41\x43\x41\x43"   
	"\x41\x43\x41\x43\x41\x00\x20\x45\x4b\x45\x44\x46\x45\x45\x49\x45"   
	"\x44\x43\x41\x43\x41\x43\x41\x43\x41\x43\x41\x43\x41\x43\x41\x43"   
	"\x41\x43\x41\x43\x41\x41\x41\x00";  
	*/

/*
const static char session_request[] =   
	"\x81\x00\x00\x44\x20\x46\x48\x45
	\x46\x45\x4f\x45\x43\x45\x50\x45"
	"\x47\x45\x42\x45\x4f\x43\x4e\x46
	\x41\x45\x44\x43\x41\x43\x41\x43"
	"\x41\x43\x41\x41\x41\x00\x20\x45
	\x4d\x46\x46\x45\x50\x43\x4e\x44
	\x47\x45\x44\x45\x46\x44\x44\x45
	\x42\x44\x49\x44\x47\x45\x45\x45
	\x45\x44\x49\x45\x45\x43\x41\x00";  
*/

typedef struct  {
 u_char ID;
 u_char Flags;
 u_short Length;
 char calledname[34];
 char callingname[34];
}NBT_NAME_SESSION_REQUEST;

typedef struct {
   u_char f;
   u_char c;
   u_short id;
   union {
      u_long e;
      u_long w;
	};
   u_long l;
   u_long r;
}GETSTATUS;

typedef struct {
	u_short m;
	u_short v;
	u_short u;
	u_short i;
	u_short f;
	char	s[];
}GETSRVRINFO;

/*
const static char session_request[] = {
0x81, 0x00, 0x00, 0x44, 0x20, 0x46, 0x48, 0x45, 
0x46, 0x45, 0x4f, 0x45, 0x43, 0x45, 0x50, 0x45, 
0x47, 0x45, 0x42, 0x45, 0x4f, 0x43, 0x4e, 0x46, 
0x41, 0x45, 0x44, 0x43, 0x41, 0x43, 0x41, 0x43, 
0x41, 0x43, 0x41, 0x43, 0x41, 0x00, 0x20, 0x45, 
0x4d, 0x46, 0x46, 0x45, 0x50, 0x43, 0x4e, 0x44, 
0x47, 0x45, 0x44, 0x45, 0x46, 0x44, 0x44, 0x45, 
0x42, 0x44, 0x49, 0x44, 0x47, 0x45, 0x45, 0x45, 
0x45, 0x44, 0x49, 0x45, 0x45, 0x41, 0x41, 0x00};
*/

const static char negotiate[] =   
	"\x00\x00\x00\x2f\xff\x53\x4d\x42\x72\x00\x00\x00\x00\x00\x00\x00"   
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x5c\x02"   
	"\x00\x00\x00\x00\x00\x0c\x00\x02\x4e\x54\x20\x4c\x4d\x20\x30\x2e"   
	"\x31\x32\x00";   

const static char setup_account[] =   
	"\x00\x00\x00\x48\xff\x53\x4d\x42\x73\x00\x00\x00\x00\x00\x00\x00"   
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x5c\x02"   
	"\x00\x00\x00\x00\x0d\xff\x00\x00\x00\xff\xff\x02\x00\x5c\x02\x00"   
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x0b"   
	"\x00\x00\x00\x6e\x74\00\x70\x79\x73\x6d\x62\x00";

const static char request[] = {
	0x16, 0x03, 0x00, 0x00, 0x53, 0x01, 0x00, 0x00, 
	0x4f, 0x03, 0x00, 0x3f, 0x47, 0xd7, 0xf7, 0xba, 
	0x2c, 0xee, 0xea, 0xb2, 0x60, 0x7e, 0xf3, 0x00, 
	0xfd, 0x82, 0x7b, 0xb9, 0xd5, 0x96, 0xc8, 0x77, 
	0x9b, 0xe6, 0xc4, 0xdb, 0x3c, 0x3d, 0xdb, 0x6f, 
	0xef, 0x10, 0x6e, 0x00, 0x00, 0x28, 0x00, 0x16, 
	0x00, 0x13, 0x00, 0x0a, 0x00, 0x66, 0x00, 0x05, 
	0x00, 0x04, 0x00, 0x65, 0x00, 0x64, 0x00, 0x63, 
	0x00, 0x62, 0x00, 0x61, 0x00, 0x60, 0x00, 0x15, 
	0x00, 0x12, 0x00, 0x09, 0x00, 0x14, 0x00, 0x11, 
	0x00, 0x08, 0x00, 0x06, 0x00, 0x03, 0x01, 0x00 
};


int codename(const char name[] , char *codedname)
{
	int i=0;
	unsigned char c;
	char * ptr = codedname;
	if(name != NULL && codedname != NULL)
	{
		memset(ptr, 'A', 34);
		*(ptr++) = ' ';
		for (i=0; i< HOSTNAMELEN-1; i++)
		{
			c = name[i];
			c = name[i] & 0xf0;
			c = (name[i] & 0xf0) >> 4;
			c = ((name[i] & 0xf0) >> 4) + 'A';
			*ptr++ = c;
			*ptr++	=	(name[i] & 0x0f) + 'A';
		}
		*(ptr++) = 0;
	}
	return 0;
}

SVTUTILS_API int get_device_info (const unsigned long &ip,const char * callingname,const char *calledname,char (&version)[NETBIOS_PARA_LEN])
{
	int iResult;
	//string host_name;
	//string type;
	//string version;
	//char callingname[HOSTNAMELEN];
	//char calledname[HOSTNAMELEN];
	NBT_NAME_SESSION_REQUEST session_request;
	memset(&session_request, 0, sizeof(NBT_NAME_SESSION_REQUEST));
	session_request.ID = 0x81;
	session_request.Flags = 0;
	session_request.Length = 0x4400;

	//memset(callingname, 0, HOSTNAMELEN);
	//memset(calledname, 0, HOSTNAMELEN);
	//memset(callingname, ' ', HOSTNAMELEN-1);
	//memset(calledname, ' ', HOSTNAMELEN-1);
	//strncpy(callingname, hostinfo->m_callingname, strlen(hostinfo->m_callingname));
	//strncpy(calledname, hostinfo->m_calledname, strlen(hostinfo->m_calledname));
	codename(callingname, session_request.callingname);
	codename(calledname, session_request.calledname);

	//if(!hostinfo)
	//{
	//	return 0;
	//}
	//string ip = string(hostinfo->m_ip);   
	
	WSADATA data;
	int ret;
	if( (ret = WSAStartup (MAKEWORD (2, 2), &data)) != 0)
	{
	  printf("WSAStartup failed with error %d\n", ret);
      return 0;
	}

	SOCKET sock = INVALID_SOCKET;
	const short smb_port = 139;
	unsigned char buf[256];

	struct sockaddr_in addr;
	//struct hostent *ent = NULL;

	//ent = gethostbyname (ip.c_str());
	//if (!ent) {
	//	addr.sin_family = ent->h_addrtype;
	//	addr.sin_addr.s_addr = inet_addr (ip.c_str ());
	//} else {
	//	addr.sin_family = AF_INET;
	//	memcpy (&addr.sin_addr, ent->h_addr, ent->h_length);
	//}
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(ip);//inet_addr (ip.c_str ());
	addr.sin_port = htons (smb_port);

	sock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);  
	if (sock == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

	if (SOCKET_ERROR == connect (sock, (struct sockaddr *)&addr, sizeof (struct sockaddr_in))) {
		//Log::repor (str (boost::format ("NetBios::get_device_info, connect error: %s, %d") %ip %WSAGetLastError ()));
		//shutdown (sock, 1);
		closesocket (sock);
		WSACleanup();
		return 1;
	}
	
	iResult = send (sock, (const char *)&session_request, sizeof(NBT_NAME_SESSION_REQUEST), 0);      
	if (iResult == SOCKET_ERROR) {
        printf("send() failed with error: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }
 
	Sleep (100);   

	if (SOCKET_ERROR == recv (sock, (char*)buf, sizeof(buf), 0)) {
		shutdown (sock, 1);
		closesocket (sock);
        WSACleanup();
		//Log::repor (str (boost::format ("NetBios::get_device_info, recv error: %s, %d") %ip %WSAGetLastError ()));
		return 1;
	}
	memset (buf, 0, sizeof(buf));   

	iResult = send (sock, negotiate, sizeof (negotiate) - 1, 0);     
	if (iResult == SOCKET_ERROR) {
        printf("send() failed with error: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }
	Sleep (100);   

	if (SOCKET_ERROR == recv (sock, (char*)buf, sizeof(buf), 0)) {
		shutdown (sock, 1);
		closesocket (sock);
        WSACleanup();
		//Log::repor (str (boost::format ("NetBios::get_device_info, recv error: %s, %d") %ip %WSAGetLastError ()));
		return 1;
	}
	memset (buf, 0, sizeof (buf));   

	iResult = send (sock, setup_account, sizeof (setup_account) - 1, 0);       
	if (iResult == SOCKET_ERROR) {
        printf("send() failed with error: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }
	Sleep (100);   

	int read_bytes = recv (sock, (char*)buf, sizeof(buf), 0);
	if (SOCKET_ERROR == read_bytes) {
		shutdown (sock, 1);
		closesocket (sock);
        WSACleanup();
		//Log::repor (str (boost::format ("NetBios::get_device_info, recv error: %s, %d") %ip %WSAGetLastError ()));
		return 1;
	}

	if (0 == read_bytes) {
	} else {
		unsigned int zeroc = 0;
		unsigned char data[1024];
		memcpy (data, buf, sizeof (buf));

		while ((--read_bytes > 0) && (zeroc < 4)) {
			if (0x00 == data[read_bytes]) {
				if (3 == zeroc++) {
					//Log::repor (str (boost::format ("NetBios::get_device_info success: %s, %s") %ip %(char*)&(data[read_bytes + 1])));
					char *ver=(char*)&data[read_bytes + 1];
					size_t siz=min(NETBIOS_PARA_LEN,strlen(ver));
					strncpy((char *)version,ver,siz);

					//version = 
					//type = "windowspc";
					//string ver = (char*)&data[read_bytes + 1];
					//transform(ver.begin (), ver.end (), ver.begin (), tolower);
					//
					//string::size_type pos = ver.find ("windows server");
					//if (string::npos != pos) {
					//	type = "windowspc";
					//}

					//pos = ver.find ("windows server 2003");
					//if (string::npos != pos) {
					//	version = "Windows Server 2003";
					//	type = "windowspc";
					//}

					//pos = ver.find ("windows 5.1");
					//if (string::npos != pos) {
					//	version = "Windows XP";
					//	type = "windowspc";
					//}
					//	
					//pos = ver.find ("windows 5.0");
					//if (string::npos != pos) {
					//	version = "Windows 2000";
					//	type = "windowspc";
					//}

					//pos = ver.find ("windows 6.0");
					//if (string::npos != pos) {
					//	version = "Windows Vista";
					//	type = "windowspc";
					//}

					//pos = ver.find ("windows 6.1");
					//if (string::npos != pos) {
					//	version = "Windows Vista(sp1)";
					//	type = "windowspc";
					//}

					//pos = ver.find ("windows 7.0");
					//if (string::npos != pos) {
					//	version = "Windows 7";
					//	type = "windowspc";
					//}

					//pos = ver.find ("linux");
					//if (string::npos != pos) {
					//	version = "Linux";
					//	type = "linuxpc";
					//}

					//pos = ver.find ("unix");
					//if (string::npos != pos) {
					//	version = "Unix";
					//	type = "unixpc";
					//}
				}
			}
		}
	}

	shutdown (sock, 1);
	closesocket (sock);
	WSACleanup ();
	//strncpy(hostinfo->m_type, type.c_str(), HOSTTYPELEN);
	//strncpy(hostinfo->m_version, version.c_str(), HOSTVERSION);

	return 0;
}


SVTUTILS_API int get_device_info2 (const unsigned long &ip,char (&hostname)[NETBIOS_PARA_LEN],char (&version)[NETBIOS_PARA_LEN])
{
	//string hostname;
	//string type;
	//string version;
	int iResult;

	//if(!hostinfo)
	//{
	//	return 0;
	//}
	//string ip = string(hostinfo->m_ip);   
	
	WSADATA data;
	int ret;
	if( (ret = WSAStartup (MAKEWORD (2, 2), &data)) != 0)
	{
	  printf("WSAStartup failed with error %d\n", ret);
      return 0;
	}

	SOCKET sock = INVALID_SOCKET;
	const short smb_port = 548;
	char buf[TCP_BUFFER];

	struct sockaddr_in addr;
	
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(ip);
	addr.sin_port = htons (smb_port);

	sock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);     
	if (sock == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
 
	if (SOCKET_ERROR == connect (sock, (struct sockaddr *)&addr, sizeof (struct sockaddr_in))) {
		shutdown (sock, 1);
		closesocket (sock);
        WSACleanup();
		return 1;
	}
	
	iResult = send (sock, request, sizeof (request) - 1, 0);   
	if (iResult == SOCKET_ERROR) {
        printf("send() failed with error: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }

	Sleep (100);   

	memset(buf, 0 ,sizeof(buf));
	int read_bytes = recv (sock, (char*)buf, sizeof(buf), 0);
	if (SOCKET_ERROR == read_bytes) {
		shutdown (sock, 1);
		closesocket (sock);
        WSACleanup();
		return 1;
	}
	
	if (0 == read_bytes) {
	} else {
		GETSTATUS *getstatus = (GETSTATUS *)buf;
		if( sizeof(GETSTATUS) < read_bytes && getstatus->c==3 && (ntohl(getstatus->l)+sizeof(GETSTATUS)==read_bytes) )
		{
			GETSRVRINFO *getsrvrinfo = (GETSRVRINFO *)(buf + sizeof(GETSTATUS));

			rsize_t size ;
			u_short offset = ntohs(*((u_short *)(getsrvrinfo->s + 1 + *(getsrvrinfo->s) + 7)));

			if(sizeof(GETSTATUS)+ offset < read_bytes)
			{
				size = (rsize_t)ntohs(*((u_short *)(buf+sizeof(GETSTATUS)+ offset )));
				if(sizeof(GETSTATUS)+ offset +size <= read_bytes)
				{
					memset(hostname,0,NETBIOS_PARA_LEN);
					strncpy((char*)hostname, (buf+sizeof(GETSTATUS) + offset + 2),size<NETBIOS_PARA_LEN?size:NETBIOS_PARA_LEN);	
				}
			}

			if(sizeof(GETSTATUS)+ntohs(getsrvrinfo->m) < read_bytes)
			{
				size = *(buf+sizeof(GETSTATUS)+ntohs(getsrvrinfo->m));
				if(sizeof(GETSTATUS)+ntohs(getsrvrinfo->m) + size <= read_bytes)
				{
					const char stemp[]="macospc ";
					memset((char *)version,0,NETBIOS_PARA_LEN);
					strncpy(version, "macospc", sizeof(stemp));
					
					strncpy((char *)version + sizeof(stemp), (buf+sizeof(GETSTATUS)+ntohs(getsrvrinfo->m)+1),min(size,NETBIOS_PARA_LEN-sizeof(stemp)));
				}
			}
		}
	}

	shutdown (sock, 1);
	closesocket (sock);
	WSACleanup ();
	//strncpy(hostinfo->m_hostname, hostname.c_str(), 255);

	return 0;
}
