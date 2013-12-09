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

#define SIZE_HINT_W 815
#define SIZE_HINT_H 600
#define WINDOW_TITLE "NETGEAR genie"
//#define VERSION_STRING "2000.00.01"
#define VERSION_STRING_IN_ABOUT "Genie alpha"

#define NETSTATE_PARA "Internet"

#endif /* DEFINES_H_ */
