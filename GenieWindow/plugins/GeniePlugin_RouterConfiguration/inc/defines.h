#ifndef DEFINES_H_
#define DEFINES_H_
#define SAFE_DELETE(p) do \
        {\
         if(p){delete p;p=0;}\
        }while(0)
		
#ifdef USE_APP_PATH
#define GENIE_DIR (QDir(qApp->applicationDirPath()))
#else
#define GENIE_DIR (QDir::current())
#endif
#define GENIE_PATH (GENIE_DIR.absolutePath()+"/")

#endif