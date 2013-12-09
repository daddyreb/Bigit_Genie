#include "stable.h"
//NetgearSoapSchedule * QGenieInterfaceSoapManager::schedule = NULL;

QGenieInterfaceSoapManager::QGenieInterfaceSoapManager()
    :schedule(NULL)
{
}

QGenieInterfaceSoapManager::~QGenieInterfaceSoapManager()
{
    if(schedule)
    {
        delete schedule;
        schedule = NULL;
    }
}

NetgearSoapSchedule* QGenieInterfaceSoapManager::getSoapAPI()
{
    return Instance();
}

NetgearSoapSchedule*  QGenieInterfaceSoapManager::Instance()
{
    QGenieInterfaceLogManager *logmanager=QGenieInterfaceLogManager::GetSingletonPtr();
    ASSERT(logmanager);
    if(schedule == NULL)
    {
        schedule = new NetgearSoapSchedule;
    }

    return schedule;
}
