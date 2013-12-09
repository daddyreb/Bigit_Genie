/*
 * defines.h
 *
 *  Created on: 2010-4-10
 *      Author: taoyidong
 */

#ifndef DEFINES_H_
#define DEFINES_H_


#include <QDebug>
#include <stdio.h>
#include <iostream>

#include <assert.h>
#ifndef ASSERT

#ifndef NO_ASSERT
#define ASSERT assert
#else
#define ASSERT(x)
#endif

#endif

#if USE_DBG
#define DBG_CHAR(para) \
	do{\
		/*QDebug dbg(QString());*/\
		std::cout<<(para)<<std::endl;\
	}while(0)
#define DBG_QSTRING(para) \
	do{\
		/*QDebug dbg(QString());*/\
		std::cout<<((para).toLatin1().data())<<endl;\
	}while(0)

#else
#define DBG(para)
#endif

#define SAFE_DELETE(p) do \
        {\
         if(p){delete p;p=0;}\
        }while(0)


//#define DB_TABLE_NAME "dd"
#define DATE_FORMAT "yyyyMMddhhmmss"

//#define HTTP_HOST "192.168.2.225"
//#define HTTP_DISP "/test/testupdate/genie_desp.txt"
//#define HTTP_DISP_FILE "http://192.168.2.225/test/testupdate/genie_desp.txt"
#define HTTP_DISP_FILE_NAME "genie_desp.ini"
#define HTTP_UPDATE_FILE_NAME "NETGEARGenie-install.exe"
#ifdef PLATFORM_WIN32
#define NETGEAR_DIR (QDir::home().absolutePath()+QString("/NETGEARGenie/"))
#else
#define NETGEAR_DIR (QDir::home().absolutePath()+QString("/.NETGEARGenie/"))
#endif
#define HTTP_LOCAL_DIR (NETGEAR_DIR+"update_temp")
#define HTTP_REMOTE_FILE_NAME "update.db"

#define DB_PREFIX "plugins_"
#define DB_SUFFIX ".db"
#define DB_IN_LOCAL "update.db"

#define TAG_IN_DESPFILE "NETGEAR genie {7b399884-b36d-4de8-ac04-19e7452023d8}"
#define TAG_IN_DESPFILE_URL "NETGEAR genie {7b399884-b36d-4de8-ac04-19e7452023d8} URL"
#ifdef _WIN32
#ifdef GENIE_MM
#define GENIE_DLL "GenieMM.dll"
#else
#define GENIE_DLL "Genie.dll"
#endif
#endif

#ifdef PLATFORM_MACOSX
#define GENIE_DLL "libGenie.dylib"
#endif

#ifdef PLATFORM_LINUX
#define GENIE_DLL "libGenie.so"
#endif

#define DIALOG_FLAG (Qt::Tool| Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint)

#define DESP_VERIFY_STR "[update]"

#ifdef USE_APP_PATH
#define GENIE_DIR (QDir(qApp->applicationDirPath()))
#else
#define GENIE_DIR (QDir::current())
#endif
#define GENIE_PATH (GENIE_DIR.absolutePath()+"/")

#endif /* DEFINES_H_ */
