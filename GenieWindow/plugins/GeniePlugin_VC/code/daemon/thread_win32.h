#ifndef __thread_win32_h__
#define __thread_win32_h__

#include "thread.h"
#include <windows.h>

namespace GTF {

class ThreadImpl
{
public:
	ThreadImpl();
	~ThreadImpl();

	bool init(Thread::ThreadProc threadProc, void *ctx);
	void join();

	static unsigned __stdcall __ThreadProc(void *ctx);

	HANDLE m_threadHandle;
	Thread::ThreadProc m_threadProc;
	void *m_threadContext;
};

class MutexImpl
	: public CRITICAL_SECTION
{
public:
	MutexImpl();
	~MutexImpl();

	void lock();
	void unlock();
};

class ElapsedTimerImpl
{
public:
	ElapsedTimerImpl();
	~ElapsedTimerImpl();
	double restart();

private:
	DWORD m_tick;
};

class CondVarImpl
{
public:
	CondVarImpl();
	~CondVarImpl();

	bool timedWait(int ms);
	void signal();

private:
	HANDLE m_hEvent;
};

// TODO: real implementation

class RWLockImpl
{
public:
	RWLockImpl();
	~RWLockImpl();

	void lockReader();
	void lockWriter();
	void unlock();

private:
	CRITICAL_SECTION m_cs;
};

} // namespace GTF

#endif // __thread_win32_h__
