#ifndef _THREAD_PRIVATE_H_
#define _THREAD_PRIVATE_H_
#include <windows.h>

#define sleep(X)	Sleep(1000 * (X))
#define usleep(X)	Sleep((X)/1000)

typedef void *(__stdcall *thread_func_t)(void *arg);
typedef struct _mutex_s
{
  int m_init;	
  CRITICAL_SECTION	m_criticalSection;
}mutex_t;
typedef mutex_t rwlock_t;

extern void	mutexInit(mutex_t *mutex);
extern void	mutexLock(mutex_t *mutex);
extern void	mutexUnlock(mutex_t *mutex);
extern void	RWInit(rwlock_t *rwlock);
extern void	RWLockRead(rwlock_t *rwlock);
extern void	RWLockWrite(rwlock_t *rwlock);
extern void	RWUnlock(rwlock_t *rwlock);
extern int	threadCreate(thread_func_t func, void *arg);
#endif
