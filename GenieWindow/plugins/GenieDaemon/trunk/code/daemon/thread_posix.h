#ifndef __thread_posix_h__
#define __thread_posix_h__

#include "thread.h"
#include <pthread.h>

namespace GTF {

class ThreadImpl
{
public:
	ThreadImpl();
	~ThreadImpl();

	bool init(Thread::ThreadProc threadProc, void *ctx);
	void join();

	static void *__ThreadProc(void *ctx);

	pthread_t m_threadHandle;
	bool m_threadOk;
	Thread::ThreadProc m_threadProc;
	void *m_threadContext;
};

class MutexImpl
{
public:
	pthread_mutex_t m_mutex;
};

class ElapsedTimerImpl
{
public:
	ElapsedTimerImpl();
	~ElapsedTimerImpl();
	double restart();

	double current();

private:
	double m_lastVal;
};

class CondVarImpl
{
public:
	CondVarImpl();
	~CondVarImpl();

	bool timedWait(int ms);
	void signal();

private:
	pthread_cond_t m_cond;
	pthread_mutex_t m_mutex;
};

class RWLockImpl
{
public:
	RWLockImpl();
	~RWLockImpl();

	void lockReader();
	void lockWriter();
	void unlock();

private:
	pthread_rwlock_t m_lock;
};

} // namespace GTF

#endif // __thread_posix_h__
