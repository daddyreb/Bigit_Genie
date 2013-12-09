#ifdef Q_OS_WIN
#include <windows.h>
#include <Lmcons.h>
#include <stdio.h>
#include <stdlib.h>

#define XP (1)
#define WIN7 (2)
#define WIN8 (3)

static int GetOSVer()
{
    OSVERSIONINFOEX osviex;

    ZeroMemory(&osviex, sizeof(OSVERSIONINFO));
    osviex.dwOSVersionInfoSize   =   sizeof(OSVERSIONINFOEX);

    GetVersionEx((OSVERSIONINFO*)&osviex);

    if(osviex.dwPlatformId == VER_PLATFORM_WIN32_NT)
    {
        if(osviex.dwMajorVersion == 5 && osviex.dwMinorVersion == 1)
        {
            //qDebug() << "The Windows System is XP!";
            return(XP);
        }
        if(osviex.dwMajorVersion ==  6 && osviex.dwMinorVersion == 1 && osviex.wProductType == VER_NT_WORKSTATION)
        {
            //qDebug() << "The Windows System is WIN7!";
            return(WIN7);
        }
        if(osviex.dwMajorVersion ==  6 && osviex.dwMinorVersion == 2 && osviex.wProductType == VER_NT_WORKSTATION)
        {
            //qDebug() << "The Windows System is WIN8!";
            return(WIN8);
        }
    }

    qDebug() << "The Windows System is OTHERS!";
    return 0;
}
#endif
