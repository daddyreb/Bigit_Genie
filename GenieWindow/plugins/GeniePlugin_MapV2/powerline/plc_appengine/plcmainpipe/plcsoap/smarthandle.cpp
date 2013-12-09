#include "smarthandle.h"

SmartHandle::SmartHandle(HANDLE handle):
        m_hHandle(handle)
{
}

SmartHandle::~SmartHandle()
{
    if(m_hHandle != INVALID_HANDLE_VALUE){
        CloseHandle(m_hHandle);
    }
}

HANDLE SmartHandle::get(void)
{
    return m_hHandle;
}
