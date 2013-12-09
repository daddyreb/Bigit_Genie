#ifndef QLAUNCHSEMAPHORE_H
#define QLAUNCHSEMAPHORE_H

#define SEMAPHORE_MAX 9
#ifdef _WIN32
#include <stdio.h>
#include <Windows.h>
#else
#include <semaphore.h>
#endif

#define SEMAPHORE_MAX 9
#define USE_QTS_CLASS
#include <QSystemSemaphore>

class QLaunchSemaphore
{
public:
    QLaunchSemaphore();
    ~QLaunchSemaphore();

    void create();
    void release(int n=1);
    bool aquire(int n=1);
protected:
#ifdef USE_QTS_CLASS
    QSystemSemaphore mSem;
#else
    #ifdef _WIN32
        HANDLE mhSemaphore;
    #else
        sem_t *mSem;
    #endif
    bool mCreated;
#endif


};


#endif // QLAUNCHSEMAPHORE_H
