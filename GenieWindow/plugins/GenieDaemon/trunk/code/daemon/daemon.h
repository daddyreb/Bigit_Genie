#ifndef __daemon_h__
#define __daemon_h__

class GenieDaemon
{
public:
	static GenieDaemon *create();
	virtual ~GenieDaemon() {}
	virtual bool start() = 0;
	virtual void check() = 0;
	virtual void stop() = 0;

protected:
	GenieDaemon() {}
};

class TestCon2
{
public:
	TestCon2();
	~TestCon2();

	int run();

private:
	GenieDaemon *m_daemon;
};

#endif // __daemon_h__
