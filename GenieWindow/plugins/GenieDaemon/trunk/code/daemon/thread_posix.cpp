#include "thread_posix.h"
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

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
: m_threadOk(false)
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

	int err = pthread_create(&m_threadHandle, NULL, &ThreadImpl::__ThreadProc, this);
	if (err != 0) {
		return false;
	}

	m_threadOk = true;
	return true;
}

void ThreadImpl::join()
{
	if (m_threadOk) {
		pthread_join(m_threadHandle, NULL);
		m_threadOk = false;
	}
}

void *ThreadImpl::__ThreadProc(void *ctx)
{
	ThreadImpl *that = static_cast<ThreadImpl*>(ctx);
	that->m_threadProc(that->m_threadContext);
	return NULL;
}

Mutex::Mutex()
{
	m_impl = new MutexImpl();
	pthread_mutex_init(&m_impl->m_mutex, NULL);
}

Mutex::~Mutex()
{
	pthread_mutex_destroy(&m_impl->m_mutex);
	delete m_impl;
}

void Mutex::lock()
{
	pthread_mutex_lock(&m_impl->m_mutex);
}

void Mutex::unlock()
{
	pthread_mutex_unlock(&m_impl->m_mutex);
}

void sleep(int mseconds)
{
	usleep(mseconds * 1000);
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
	m_lastVal = current();
}

ElapsedTimerImpl::~ElapsedTimerImpl()
{
}

double ElapsedTimerImpl::restart()
{
	double currVal = current();
	double retval = currVal - m_lastVal;
	m_lastVal = currVal;
	return retval;
}

double ElapsedTimerImpl::current()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	double retval = static_cast<double>(tv.tv_sec);
	retval += static_cast<double>(tv.tv_usec) * 0.000001;
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
	pthread_mutex_init(&m_mutex, NULL);
	pthread_cond_init(&m_cond, NULL);
}

CondVarImpl::~CondVarImpl()
{
	pthread_cond_destroy(&m_cond);
	pthread_mutex_destroy(&m_mutex);
}

bool CondVarImpl::timedWait(int ms)
{
	div_t d = div(ms, 1000);

	struct timeval tv;
	struct timespec ts;

	gettimeofday(&tv, NULL);
	ts.tv_sec = tv.tv_sec + d.quot;
	ts.tv_nsec = tv.tv_usec + d.rem * 1000;
	ts.tv_nsec *= 1000;

	pthread_mutex_lock(&m_mutex);
	int retval = pthread_cond_timedwait(&m_cond, &m_mutex, &ts);
	//printf("EE %d %lld %lld, %lld %lld\n", errno, tv.tv_sec, tv.tv_usec, ts.tv_sec, ts.tv_nsec);
	pthread_mutex_unlock(&m_mutex);
	return retval == 0;
}

void CondVarImpl::signal()
{
	pthread_cond_signal(&m_cond);
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
	pthread_rwlock_init(&m_lock, NULL);
}

RWLockImpl::~RWLockImpl()
{
	pthread_rwlock_destroy(&m_lock);
}

void RWLockImpl::lockReader()
{
	pthread_rwlock_rdlock(&m_lock);
}

void RWLockImpl::lockWriter()
{
	pthread_rwlock_wrlock(&m_lock);
}

void RWLockImpl::unlock()
{
	pthread_rwlock_unlock(&m_lock);
}

} // namespace GTF
