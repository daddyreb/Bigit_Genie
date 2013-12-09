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
#include "date.h"
#include "version.h"
#ifndef ASSERT

#ifndef NO_ASSERT
#define ASSERT assert
#else
#define ASSERT(x)
#endif

#endif
#define APP_NAME "NETGEAR Genie"

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

#define SIZE_HINT_W 815
#define SIZE_HINT_H 600
#define WINDOW_TITLE "NETGEAR genie"


#define VERSION_STR_SUFFIX "" //" Alpha "


//#define VERSION_STRING_IN_ABOUT "Genie "
#define COPYRIGHT_STRING "Copyright 2010 NETGEAR INC."

#define NETSTATE_PARA "Internet"

#define FRAME_UUID "{7b399884-b36d-4de8-ac04-19e7452023d8}"
#define PLUGIN_UUID_INFO "{f9b01961-e3b2-474c-8521-42371d2cab61}"
#define PLUGIN_UUID_STATISTIC "{6b864d58-7d53-489d-a558-1684e2fb2f60}"
#define PLUGIN_UUID_NETTRAFFIC "{bfc53cc5-97a4-4060-aff9-30a4b52b7e99}"
#define PLUGIN_UUID_NETCONFIG "{6c1c9b2f-5f5e-45f8-8dad-1633ff446377}"
#define PLUGIN_UUID_FIRMWARE_UPDATE "{c02fb5db-6c62-42e7-bf4a-f704247ed7b8}"
#define PLUGIN_UUID_INTERNET "{6c7b332f-a107-40ab-84f3-2163ac13ad0a}"
#define PLUGIN_UUID_WIRELESS "{e722b972-b05e-4d71-8cd9-676ebd1535b9}"
#ifdef SHEET_STYLE_FORMAC
#define DIALOG_FLAG (Qt::Sheet| Qt::FramelessWindowHint)
#else
#define DIALOG_FLAG (Qt::Tool| Qt::FramelessWindowHint)
#endif

//#define LOG_CLICK(s) \
//            do{\
//            QGenieInterfaceClickManager *_clickmanager_uiikjghlldj=QGenieInterfaceClickManager::GetSingletonPtr();\
//            ASSERT(_clickmanager_uiikjghlldj);\
//            _clickmanager_uiikjghlldj->click(s);\
//        }while(0)
#ifdef USE_LOG
#define GENIE_LOG(s) do{\
    QGenieInterfaceLogManager *logmanager_ykdtgc=QGenieInterfaceLogManager::GetSingletonPtr();\
    ASSERT(logmanager_ykdtgc);\
    logmanager_ykdtgc->writeLog(s,QUuid(FRAME_UUID));\
    }while(0)

#else
#define GENIE_LOG(s) do{}while(0)
#endif

#ifdef USE_APP_PATH
#define GENIE_DIR (QDir(qApp->applicationDirPath()))
#else
#define GENIE_DIR (QDir::current())
#endif
#define GENIE_PATH (GENIE_DIR.absolutePath()+"/")

#endif /* DEFINES_H_ */
