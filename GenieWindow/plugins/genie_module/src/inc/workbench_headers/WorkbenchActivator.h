#ifndef WORKBENCHACTIVATOR
#define WORKBENCHACTIVATOR

#include "IBundleActivator.h"
#include "IBundleContext.h"
#include "IServiceRegistration.h"
#include "ServiceTracker.h"

#include "WorkbenchRun.h"

using namespace svp::framework;

class WorkbenchActivator : public IBundleActivator, public IServiceTrackerCustomizer
{
public:
	WorkbenchActivator(void);
	~WorkbenchActivator(void);

public:
	virtual void start (IBundleContext::ConstPtr context)=0;	

	virtual void stop (IBundleContext::ConstPtr context)=0;

	virtual bool addingService( const ServiceReference& ref )=0;

	virtual void removedService( const ServiceReference& ref )=0;
};

#endif