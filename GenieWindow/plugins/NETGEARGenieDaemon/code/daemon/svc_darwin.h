#ifndef __svc_darwin_h__
#define __svc_darwin_h__

#include <string>

class GlobalLock
{
public:
	GlobalLock(const char *name);
	~GlobalLock();

	bool acquire();
	void release();

private:
	bool acquireImpl();
	void releaseImpl();
	std::string fileName() const;

private:
	std::string m_name;
	int m_fd;
};

#endif // __svc_darwin_h__
