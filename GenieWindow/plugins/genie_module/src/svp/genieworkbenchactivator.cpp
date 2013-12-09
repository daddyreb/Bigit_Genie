#include "genieworkbenchactivator.h"
#include "ObjectCreator.h"
#include "Properties.h"


using namespace svp::instantiation;
using namespace svp::framework;


GenieWorkbenchActivator::GenieWorkbenchActivator(void)
{
}

GenieWorkbenchActivator::~GenieWorkbenchActivator(void)
{
}

void GenieWorkbenchActivator::start (IBundleContext::ConstPtr context)
{
        Properties props;
        props.put( "instance", "1" );

        this->m_service = new GenieWorkbenchRun();
        this->m_serviceReg = context->registerService ("IRunnable", this->m_service, props);

        this->m_tracker = new ServiceTracker( context, "ISvpShell", this );
        this->m_tracker->startTracking();
}

void GenieWorkbenchActivator::stop (IBundleContext::ConstPtr context)
{
        this->m_serviceReg->unregister();
        /*delete (this->m_serviceReg);*/
        delete (this->m_service);

        this->m_tracker->stopTracking();
        delete this->m_tracker;
}

bool GenieWorkbenchActivator::addingService( const ServiceReference& ref )
{
        if ( ref.getServiceName() == "ISvpShell" )
        {
                /*if ( ref.getServiceProperties().get( "instance" ) == "1" )
                {*/
                        this->m_svpshell_service = static_cast<ISvpShell*> ( ref.getService() );
                        GenieWorkbenchRun* temp=dynamic_cast<GenieWorkbenchRun*>(this->m_service);
                        if (temp)
                        {
                                temp->setSvpShell(this->m_svpshell_service);
                        }
                        return true;
                /*}*/
        }

        return false;
}

void GenieWorkbenchActivator::removedService( const ServiceReference& ref )
{
        if ( ref.getServiceName() == "ISvpShell" )
        {
                Properties props = ref.getServiceProperties();
                if ( props.get( "instance" ) == "1" )
                {
                        this->m_svpshell_service = 0;
                }
        }
}

REGISTER_BUNDLE_ACTIVATOR_CLASS("GenieWorkbenchActivator",GenieWorkbenchActivator)
