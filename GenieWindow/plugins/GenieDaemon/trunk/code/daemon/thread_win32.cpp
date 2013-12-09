#include "thread_win32.h"
#include <process.h>

namespace GTF {

Thread::Thread()
{
	m_impl = new ThreadImpl();
}

Thread::~Thread()
{
	delete m_impl;
}

Thread *Thread::create(ThreadProc threadProc, void *context)
{
	Thread *obj = new Thread();
	if (!obj->m_impl->init(threadProc, context)) {
		delete obj;
		obj = NULL;
	}
	return obj;
}

void Thread::join()
{
	m_impl->join();
}

ThreadImpl::ThreadImpl()
: m_threadHandle(NULL)
{
}

ThreadImpl::~ThreadImpl()
{
	join();
}

bool ThreadImpl::init(Thread::ThreadProc threadProc, void *ctx)
{
	m_threadProc = threadProc;
	m_threadContext = ctx;

	m_threadHandle = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, &__ThreadProc, this, 0, NULL));
	if (!m_threadHandle) {
		return false;
	}

	return true;
}

void ThreadImpl::join()
{
	if (m_threadHandle) {
		WaitForSingleObject(m_threadHandle, INFINITE);
		CloseHandle(m_threadHandle);
		m_threadHandle = NULL;
	}
}

unsigned ThreadImpl::__ThreadProc(void *ctx)
{
	ThreadImpl *that = static_cast<ThreadImpl*>(ctx);
	that->m_threadProc(that->m_threadContext);
	return 0;
}

Mutex::Mutex()
{
	m_impl = new MutexImpl();
}

Mutex::~Mutex()
{
	delete m_impl;
}

void Mutex::lock()
{
	m_impl->lock();
}

void Mutex::unlock()
{
	m_impl->unlock();
}

MutexImpl::MutexImpl()
{
	InitializeCriticalSection(this);
}

MutexImpl::~MutexImpl()
{
	DeleteCriticalSection(this);
}

void MutexImpl::lock()
{
	EnterCriticalSection(this);
}

void MutexImpl::unlock()
{
	LeaveCriticalSection(this);
}

void sleep(int mseconds)
{
	Sleep(mseconds);
}

ElapsedTimer::ElapsedTimer()
{
	m_impl = new ElapsedTimerImpl();
}

ElapsedTimer::~ElapsedTimer()
{
	delete m_impl;
}

double ElapsedTimer::restart()
{
	return m_impl->restart();
}

ElapsedTimerImpl::ElapsedTimerImpl()
{
	m_tick = GetTickCount();
}

ElapsedTimerImpl::~ElapsedTimerImpl()
{
}

double ElapsedTimerImpl::restart()
{
	DWORD tick = GetTickCount();
	double retval = static_cast<double>(tick - m_tick) * 0.001;
	m_tick = tick;
	return retval;
}

CondVar::CondVar()
{
	m_impl = new CondVarImpl();
}

CondVar::~CondVar()
{
	delete m_impl;
}

bool CondVar::timedWait(int ms)
{
	return m_impl->timedWait(ms);
}

void CondVar::signal()
{
	m_impl->signal();
}

CondVarImpl::CondVarImpl()
{
	m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}

CondVarImpl::~CondVarImpl()
{
	CloseHandle(m_hEvent);
}

bool CondVarImpl::timedWait(int ms)
{
	return WaitForSingleObject(m_hEvent, ms) == WAIT_OBJECT_0;
}

void CondVarImpl::signal()
{
	SetEvent(m_hEvent);
}

RWLock::RWLock()
{
	m_impl = new RWLockImpl();
}

RWLock::~RWLock()
{
	delete m_impl;
}

void RWLock::lockReader()
{
	m_impl->lockReader();
}

void RWLock::lockWriter()
{
	m_impl->lockWriter();
}

void RWLock::unlock()
{
	m_impl->unlock();
}

RWLockImpl::RWLockImpl()
{
	InitializeCriticalSection(&m_cs);
}

RWLockImpl::~RWLockImpl()
{
	DeleteCriticalSection(&m_cs);
}

void RWLockImpl::lockReader()
{
	EnterCriticalSection(&m_cs);
}

void RWLockImpl::lockWriter()
{
	EnterCriticalSection(&m_cs);
}

void RWLockImpl::unlock()
{
	LeaveCriticalSection(&m_cs);
}

} // namespace GTF
