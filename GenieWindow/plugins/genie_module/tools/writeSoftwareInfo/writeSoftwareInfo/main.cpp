/* uuidgen.c 
   2007-09-15 Last created by cheungmine.
   All rights reserved by cheungmine.
   C application
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "md5.h"
#include "uuid.h"
//#include <Windows.h>
#include "inifile_tool.h"

#define SECTION_NAME "local_info"
#define KEY_UUID "uuid"
#define KEY_INSTALL_TIME "installtime"
#define KEY_VERSION "version"
#define INI_FILE "localinfo.ini"
#include <time.h>
#include "../../../version.h"

extern bool write2Ini(const char *section,const char *key,const char *value,const char *fname);

int main(int argc,char **argv)
{
	if(!(argc==2 && strcmp(argv[1],"regist")) )
		return -1;
	time_t timep;
	struct tm *p;
	time(&timep);
	p=localtime(&timep); /*取得当地时间*/
	char stemp[256]={0};
	sprintf ((char *)stemp,"%d-%d-%d-%.2d:%.2d:%.2d", (1900+p->tm_year),( 1+p->tm_mon), p->tm_mday,p->tm_hour, p->tm_min, p->tm_sec);
	SetValueToIniFile(INI_FILE,SECTION_NAME,KEY_INSTALL_TIME,(char *)stemp);//向配置文件中写入一个键值


	uuid_t u;
	uuid_create(&u);
	char *uid = uuid_to_string(&u);

	printf("uuid U to string:{%s} \n", uid);

	SetValueToIniFile(INI_FILE,SECTION_NAME,KEY_UUID,uid);
	//SetValueToIniFile(INI_FILE,SECTION_NAME,"ddd",uid);
//	WritePrivateProfileStringA(SECTION_NAME,KEY_UUID,"ui",INI_FILE);
//	write2Ini(SECTION_NAME,KEY_UUID,"uid",INI_FILE);//向配置文件中写入一个键值

	SetValueToIniFile(INI_FILE,SECTION_NAME,KEY_VERSION,VERSION_STRING);

//	printf(“%s%d:%d:%d\n”, wday[p->tm_wday],p->tm_hour, p->tm_min, p->tm_sec);
	return 0;

}

	
