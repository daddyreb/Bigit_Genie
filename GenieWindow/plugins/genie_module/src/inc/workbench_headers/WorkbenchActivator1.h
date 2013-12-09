#ifndef WORKBENCHACTIVATOR1_H
#define WORKBENCHACTIVATOR1_H

#include "workbenchactivator.h"
#include "WorkbenchRun1.h"

class WorkbenchActivator1 :
	public WorkbenchActivator
{
public:
	WorkbenchActivator1(void);
	~WorkbenchActivator1(void);
	 void start (IBundleContext::ConstPtr context);	

	 void stop (IBundleContext::ConstPtr context);

	 bool addingService( const ServiceReference& ref );

	 void removedService( const ServiceReference& ref );
private:
	WorkbenchRun1			*m_service;
};

#endif