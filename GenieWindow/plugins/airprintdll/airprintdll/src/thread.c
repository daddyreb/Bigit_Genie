#include "common.h"
#include "thread-private.h"
#include <process.h>
void mutexInit(mutex_t *mutex)
{
  InitializeCriticalSection(&mutex->m_criticalSection);
  mutex->m_init = 1;
}

void mutexLock(mutex_t *mutex)
{
  EnterCriticalSection(&mutex->m_criticalSection);
}

void mutexUnlock(mutex_t *mutex)
{
  LeaveCriticalSection(&mutex->m_criticalSection);
}

void RWInit(rwlock_t *rwlock)
{
  mutexInit((mutex_t *)rwlock);
}

void RWLockRead(rwlock_t *rwlock)
{
  mutexLock((mutex_t *)rwlock);
}

void RWLockWrite(rwlock_t *rwlock)
{
  mutexLock((mutex_t *)rwlock);
}

void RWUnlock(rwlock_t *rwlock)
{
  mutexUnlock((mutex_t *)rwlock);
}

int threadCreate(thread_func_t func,void *arg)
{
  return (_beginthreadex(NULL, 0, (LPTHREAD_START_ROUTINE) func, arg, 0, NULL) != 0);
}
