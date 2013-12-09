#ifndef SMARTHANDLE_H
#define SMARTHANDLE_H

#include <windows.h>

class SmartHandle
{
public:
    explicit SmartHandle(HANDLE handle);
    ~SmartHandle();
public:
    HANDLE get(void);

private:
    SmartHandle(const SmartHandle&);
    SmartHandle& operator=(const SmartHandle&);

private:
    HANDLE m_hHandle;
};

#endif // SMARTHANDLE_H
