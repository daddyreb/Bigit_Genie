#include "QLaunchSemaphore.h"
//#include "defines.h"
//#include <QDebug>
#ifdef _WIN32
#else
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>
#endif

#define SEMAPHORE_NAME "SEMEPHORE_GENIE2"


QLaunchSemaphore::QLaunchSemaphore()
    :mSem(SEMAPHORE_NAME,0,QSystemSemaphore::Create)
{

}



QLaunchSemaphore::~QLaunchSemaphore()
{

}

bool QLaunchSemaphore::aquire(int n)
{
    for(int i=0;i<n;i++)
    {
        if(! mSem.acquire())
        {
            return false;
        }
    }
    return true;
}

void QLaunchSemaphore::create()
{

}

void QLaunchSemaphore::release(int n)
{
    mSem.release(n);
}


