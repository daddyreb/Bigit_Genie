#ifndef _COMMON_H______
#define _COMMON_H______
#include <assert.h>

#ifndef NO_ASSERT
#define ASSERT assert
#else
#define ASSERT(x)
#endif


#define SAFE_DELETE(p) do \
        {\
         if(p){delete p;p=0;}\
        }while(0)
		
#define UUID_OF_THIS "{6c1c9b2f-5f5e-45f8-8dad-1633ff446377}"

#define DATE_FORMAT "yyyy:MM:dd-hh:mm:ss"

#define ISETTING_GACC_MAC "plugin/routeconf_gacc_mac"
#define ISETTING_GACC_TIME "plugin/routeconf_gacc_time"
#define ERROR_MODE "<center><font color='red'>%1</font></center>"
#define REFRESH_WAITING_MSECS 20000
#define WHEN_APPLY(x) MainWindow::GetSingleton().whenApply(x)
#define STATISTIC()     MainWindow::sHandler->getClickManager()->click("p")
//#define REFRESH_DELAY 60000
#define APPLY_DELAY 30000

#ifdef USE_APP_PATH
#define GENIE_DIR (QDir(qApp->applicationDirPath()))
#else
#define GENIE_DIR (QDir::current())
#endif
#define GENIE_PATH (GENIE_DIR.absolutePath()+"/")

#define ADD_CLICK(s) do{\
    if(MainWindow::sHandler){\
        MainWindow::sHandler->getClickManager()->click(s);\
    }\
}while(0)

#endif
