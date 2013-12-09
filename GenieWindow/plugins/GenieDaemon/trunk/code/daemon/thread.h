#ifndef __thread_h__
#define __thread_h__

namespace GTF {

class ThreadImpl;

class Thread
{
	friend class ThreadImpl;

public:
	typedef void (*ThreadProc)(void *);
	static Thread *create(ThreadProc threadProc, void *context);
	void join();
	~Thread();

protected:
	Thread();

private:
	ThreadImpl *m_impl;
};

class MutexImpl;

class Mutex
{
	friend class MutexImpl;

public:
	Mutex();
	~Mutex();

	void lock();
	void unlock();

private:
	MutexImpl *m_impl;
};

class Locker
{
public:
	Locker(Mutex *mutex)
	: m_mutex(mutex)
	{
		m_mutex->lock();
	}

	~Locker()
	{
		m_mutex->unlock();
	}

private:
	Mutex *m_mutex;
};

void sleep(int mseconds);

class ElapsedTimerImpl;

class ElapsedTimer
{
	friend class ElapsedTimerImpl;

public:
	ElapsedTimer();
	~ElapsedTimer();
	double restart();

private:
	ElapsedTimerImpl *m_impl;
};

class CondVarImpl;

class CondVar
{
	friend class CondVarImpl;

public:
	CondVar();
	~CondVar();

	bool timedWait(int ms);
	void signal();

private:
	CondVarImpl *m_impl;
};

class RWLockImpl;

class RWLock
{
	friend class RWLockImpl;

public:
	RWLock();
	~RWLock();

	void lockReader();
	void lockWriter();
	void unlock();

private:
	RWLockImpl *m_impl;
};

class ReaderLocker
{
public:
	ReaderLocker(RWLock *lock)
	: m_lock(lock)
	{
		m_lock->lockReader();
	}

	~ReaderLocker()
	{
		m_lock->unlock();
	}

private:
	RWLock *m_lock;
};

class WriterLocker
{
public:
	WriterLocker(RWLock *lock)
	: m_lock(lock)
	{
		m_lock->lockWriter();
	}

	~WriterLocker()
	{
		m_lock->unlock();
	}

private:
	RWLock *m_lock;
};

} // namespace GTF

#endif // __thread_h__
