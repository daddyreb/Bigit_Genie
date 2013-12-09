#ifndef GENIEWORKBENCHACTIVATOR_H
#define GENIEWORKBENCHACTIVATOR_H

#include "workbenchactivator.h"
#include "GenieWorkbenchRun.h"
#include "ISvpShell.h"

class GenieWorkbenchActivator :
        public WorkbenchActivator
{
public:
        GenieWorkbenchActivator(void);
        ~GenieWorkbenchActivator(void);
        void start (IBundleContext::ConstPtr context);

        void stop (IBundleContext::ConstPtr context);

        bool addingService( const ServiceReference& ref );

        void removedService( const ServiceReference& ref );
private:
        GenieWorkbenchRun			*m_service;
        IServiceRegistration                    *m_serviceReg;

        ISvpShell				*m_svpshell_service;
        ServiceTracker				*m_tracker;
};

#endif // GENIEWORKBENCHACTIVATOR_H
