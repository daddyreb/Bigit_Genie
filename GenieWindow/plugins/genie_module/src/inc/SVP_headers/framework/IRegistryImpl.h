#ifndef IREGISTRY_IMPL_H
#define IREGISTRY_IMPL_H

#include <map>
#include <string>
#include <vector>

#include "BundleInfo.h"
#include "BundleInfoBase.h"
#include "ServiceInfo.h"
#include "ServiceListenerInfo.h"
#include "IServiceListener.h"
#include "ServiceEvent.h"
#include "ServiceReference.h"
#include "IRegistry.h"
#include "IService.h"
#include "Properties.h"
#include "IServiceRegistration.h"
#include "IServiceRegistrationImpl.h"

#include "../util/logging/Logger.h"
#include "../util/logging/LoggerFactory.h"
#include "../util/threading/SingleThreaded.h"

using namespace std;

using namespace svp::framework;
using namespace svp::util::logging;
using namespace svp::util::threading;

namespace svp { namespace framework {

/**
 * The central class (the 'brain') of the SOF framework which
 * stores and holds all relevant data of the bundles.<br>
 * The <code>IRegistryImpl</code> class has the knowledge about
 * all registered service objects and service listeners for
 * example.<br>
 * The main tasks of the registry are:<br>* <ul>
 *		<li>Storing the information about started bundles wheres
 *			the bundle data consists of bundle name, activator object
 *			registered services and registered listeners.
 *		<li>Notifying the accordant service listener objects when 
 *			a new service object is registered or a service
 *			object is removed.<br>
 * </ul>
 *
 * @author magr74
 */
template<class ThreadingModel=SingleThreaded>
class IRegistryImpl : public IRegistry
{
	protected:

		/**
		 * The logger instance.
		 */
		static Logger &logger;

		/**
		 * The vector storing <code>BundleInfo</code> objects
		 * which hold all bundle relevant data.
		 */
		vector<BundleInfoBase*> bundleInfoVec;

		/**
		 * Stores vectors of <code>ServiceInfo</code> objects 
		 * relating to the service name. <code>ServiceInfo</code>
		 * objects store the name of the service, the service object 
		 * and the service properties. There can be several services
		 * (several <code>ServiceInfo</code> objects) with the same 
		 * service name.
		 * 
		 */
		map<string, vector<ServiceInfo*>* > serviceInfoMap;

		/**
		 * Maps <code>ServiceListenerInfo</code>  objects to the
		 * service name. <code>ServiceListenerInfo</code> objects contain
		 * the service name they are listen for.
		 */
		map<string, vector<ServiceListenerInfo*>* > serviceListenerMap;		

		/**
		 * Stops the activator object.
		 *
		 * @param bi
		 *				The bundle information object.
		 */
		virtual void stopActivator( BundleInfoBase* bi ) ;

		/**
		 * Deletes the activator object.
		 *
		 * @param bi
		 *			The bundle information object containing all relevant
		 *			bundle information.
		 */
		virtual void deleteActivator( BundleInfoBase* bi );

		/**
		 * Notifies service listener objects about a specific service which was
		 * registered.<br>
		 * Is called when a new service listener object is registered.
		 *
		 * @param bundleName
		 *				The name of the bundle the registered service belongs to.
		 *
		 * @param serviceInfoVec
		 *				A vector of <code>ServiceInfo</code> objects which have all the same
		 *				service name.
		 * 
		 * @param serviceListenerInfoVec
		 *				A vector of <code>ServiceListenerInfo</code> objects containing the
		 *				service listener objects which are notified.
	     * 
		 * @param serviceName
		 *				The name of the service which is registered.
		 */
		virtual void notifyListenersAboutRegisteredService( const string& bundleName, vector<ServiceInfo*>* serviceInfoVec, vector<ServiceListenerInfo*>* serviceListenerInfoVec, const string& serviceName );

		/**
		 * Notifies service listener objects about a specific service which was
		 * registered.<br>
		 * Is called when a new service listener object is registered.
		 *
		 * @param bundleName
		 *				The name of the bundle the registered service belongs to.
		 *
		 * @param serviceInfoVec
		 *				A vector of <code>ServiceInfo</code> objects which have all the same
		 *				service name.
		 * 
		 * @param serviceListenerInfoVec
		 *				A vector of <code>ServiceListenerInfo</code> objects containing the
		 *				service listener objects which are notified.
	     * 
		 * @param serviceName
		 *				The name of the service which is registered.
		 */
		virtual void notifyListenerAboutRegisteredService( const string& bundleName, vector<ServiceInfo*>* serviceInfoVec, ServiceListenerInfo* serviceListenerInfo, const string& serviceName );

		/**
		 * Notifies service listener objects about a specific service which was
		 * registered.<br>
		 * Is called when a new service is registered.
		 *
		 * @param bundleName
		 *				The name of the bundle the registered service belongs to.
		 *
		 * @param serviceInfo
		 *				The <code>ServiceInfo</code> object of the service which was registered.
		 * 
		 * @param serviceListenerInfoVec
		 *				A vector of <code>ServiceListenerInfo</code> objects containing the
		 *				service listener objects which are notified.
	     * 
		 * @param serviceName
		 *				The name of the service which is registered.
		 */
		virtual void notifyListenersAboutRegisteredService( const string& bundleName, ServiceInfo* serviceInfo, vector<ServiceListenerInfo*>* serviceListenerInfoVec, const string& serviceName );

		/**
		 * Notifies service listener objects about a specific service which is
		 * deregistered.<br>
		 * Is called when a service is deregistered.
		 *
		 * @param bundleName
		 *				The name of the bundle the deregistered service belongs to.
		 *
		 * @param serviceInfo
		 *				The <code>ServiceInfo</code> object of the service which is deregistered.
		 * 
		 * @param serviceListenerInfoVec
		 *				A vector of <code>ServiceListenerInfo</code> objects containing the
		 *				service listener objects which must be notified.
		 */
		virtual void notifyListenersAboutDeregisteredService( const string& bundleName, ServiceInfo* serviceInfo, vector<ServiceListenerInfo*>* serviceListenerInfoVec );

		/**
		 * All registered service are cached by using <code>ServiceInfo</code> objects. This method
		 * adds a new <code>ServiceInfo</code> object to the internal storage.
		 *
		 * @param serviceName
		 *				The name of the service whose <code>ServiceInfo</code> object must be stored.
		 *
		 * @param serviceInfo
		 *				The object of type <code>ServiceInfo</code>.
		 */
		virtual void addToServiceInfoVector( const string& bundleName, const string& serviceName, ServiceInfo* serviceInfo ) ;

		/**
		 * Removes a <code>ServiceInfo</code> object from the internal storage.
		 *
		 * @param serviceInfo
		 *				The service info object which is removed.
		 */
		virtual void removeFromServiceInfoVector( ServiceInfo* serviceInfo ) ;

		/**
		 * Helper method which returns the vector of <code>ServiceListenerInfo</code>
		 * objects.
		 *
		 * @param serviceName
		 *				The service name the <code>ServiceListenerInfo</code> objects are mapped to.
		 */
		virtual vector<ServiceListenerInfo*>* getServiceListenerInfoVector( const string& serviceName ) ;

		/**
		 * Removes a service listener info object from the internal storage.
		 * 
		 * @param bundleName
		 *				The name of the bundle the service listener belongs to.
		 *
		 * @param serviceListener
		 *				The service listener object.
		 */
		virtual void removeFromServiceListenerInfoVector( const string& bundleName, const ServiceListenerInfo& info );

		/**
		 * Adds a <code>ServiceInfo</code> object to the bundle info storage.<br>
		 * The <code>BundleInfo</code> object contains all relevant information of
		 * a bundle (like information about registered services).
		 * 
		 * @param bundleName
		 *				The name of the bundle the registered service belongs to.
		 *
		 * @param serviceInfo
		 *				The service info object which is stored.
		 */
		virtual void addRegisteredServiceToBundleInfo( const string& bundleName, ServiceInfo* serviceInfo ) ;

		/**
		 * Removes a <code>ServiceInfo</code> object from the bundle info storage.<br>
		 * The <code>BundleInfo</code> object contains all relevant information of
		 * a bundle (like information about registered services).
		 * 
		 * @param bundleName
		 *				The name of the bundle the deregistered service belongs to.
		 *
		 * @param serviceInfo
		 *				The service info object which is removed.
		 */
		virtual void removeDeregisteredServiceFromBundleInfo( const string& bundleName, ServiceInfo* serviceInfo ) ;

		/**
		 * Adds a <code>ServiceInfo</code> object representing a used service to the bundle
		 * info object.
		 *
		 * @param bundleName
		 *				The name of the bundle which uses the specified service.
		 *
		 * @param serviceInfo
		 *				The <code>ServiceInfo</code> object representing a service
		 *				which is used by the specified bundle.
		 */
		virtual void addUsedServiceToBundleInfo( const string& bundleName, ServiceInfo* serviceInfo );
		
		/**
		 * Removes a <code>ServiceInfo</code> object representing a used service from the bundle
		 * info object.
		 *
		 * @param bundleName
		 *				The name of the bundle which used the specified service.
		 *
		 * @param serviceInfo
		 *				The <code>ServiceInfo</code> object representing a service
		 *				which was used by the specified bundle.
		 */
		virtual void removeUsedServiceFromBundleInfo( const string& bundleName, ServiceInfo* serviceInfo );

		/**
		 * Returns true if the listener objects of the passed <code>ServiceListenerInfo</code> objects are equal.
		 *
		 * @param info1
		 *				The first <code>ServiceListenerInfo</code> object.
		 *
		 * @param info2
		 *				The second <code>ServiceListenerInfo</code> object.
		 *
		 * @return True, if objects are equal, otherwise false.
		 */
		virtual bool areServiceListenerObjectsEqual( const ServiceListenerInfo& info1, const ServiceListenerInfo& info2 );

		/**
		 * Sends an event to a service listener.
		 * 
		 * @param listenerInfo 
		 *					The <code>ServiceListenerInfo</code> object describing the service listener which
		 *					is notified.
		 *
		 * @param event
		 *					The service event which is sent.
		 */
		virtual bool callServiceListenerObject( const ServiceListenerInfo& listenerInfo, const ServiceEvent& event );

		/**
		 * Creates a <code>ServiceReference</code> object.
		 *
		 * @param serviceInfo
		 *				The <code>ServiceInfo</code> instance.
		 * @return 
		 *				The created <code>ServiceReference</code>.
         */		
		virtual ServiceReference* createServiceReference( const ServiceInfo& serviceInfo );

	public:

		virtual ~IRegistryImpl();

		/**
		 * Adds a <code>BundleInfo</code> object to the registry.
		 *
		 * @param bundleInfo
		 *			The <code>BundleInfo</code> object which describes a bundle.
		 */
		virtual void addBundleInfo( BundleInfoBase* bundleInfo );

		/**
		 * Returns the <code>BundleInfo</code> object of a specific bundle.
		 *
		 * @param  bundleName
		 *				The name of the bundle whose bundle info object is returned.
		 *
		 * @return	The <code>BundleInfo</code> object.
		 */
		virtual BundleInfoBase* getBundleInfo( const string& bundleName );

		/**
		 * Returns all <code>BundleInfo</code> objects which are currently
		 * stored in registry.
		 *
		 * @return	A vector of <code>BundleInfo</code> objects.
		 *
		 */
		virtual vector<BundleInfoBase*> getBundleInfos();

		/**
		 * Removes the <code>BundleInfo</code> object of a specific bundle.
		 *
		 * @param bundleName
		 *				The name of the bundle whose bundle info object is removed.
		 */
		virtual void removeBundleInfo( const string& bundleName );

		/**
		 * Removes all <code>BundleInfo</code> objects which are currently
		 * stored in registry.		 
		 */
		virtual void removeAllBundleInfos();

		/**
		 * Adds a <code>ServiceInfo</code> object to the registry.
		 *
		 * @param bundleName
		 *				The name of the bundle the service belongs to.
		 *
		 * @param serviceName
		 *				The name of the service.
		 *
		 * @param serviceInfo
		 *				The <code>ServiceInfo</code> object describing the service.
		 */
		virtual IServiceRegistration::ConstPtr addServiceInfo( const string& bundleName, ServiceInfo* serviceInfo ) ;
		
		/**
		 * Removes a <code>ServiceInfo</code> object from the registry.
		 *
		 * @param bundleName
		 *				The name of the bundle the service belongs to.
		 *
		 * @param serviceInfo
		 *				The <code>ServiceInfo</code> object describing the service.
		 */
		virtual void removeServiceInfo( const string& bundleName, ServiceInfo* serviceInfo ) ;

		/**
		 * Returns the <code>ServiceInfo</code> object for a specific service.
		 *
		 * @param serviceName
		 *				The name of the service.
		 *
		 * @return A pointer to a vector of pointers to <code>ServiceInfo</code> objects.
		 */
		virtual vector<ServiceInfo*>* getServiceInfo( const string &serviceName ) ;

		/**
		 * Adds a service listener object to the registry.
		 *
		 * @param bundleName
		 *				The name of the bundle the service listener belongs to.
		 *
		 * @param serviceListener
		 *				The pointer to the service listener object.
		 *
		 * @param serviceName
		 *				The name of the service.
		 */
		virtual void addServiceListener( const string& bundleName, ServiceListenerInfo* serviceListenerInfo ) ;
		
		/**
		 * Removes a service listener object from the registry.
		 *
		 * @param bundleName
		 *				The name of the bundle the service listener belongs to.
		 *
		 * @param serviceListener
		 *				The pointer to the service listener object.
		 */
		virtual void removeServiceListener( const string& bundleName, const ServiceListenerInfo& info ) ;
		
};

//#include "IRegistryImpl.cpp"
template<class ThreadingModel>
Logger& IRegistryImpl<ThreadingModel>::logger = LoggerFactory::getLogger( "Framework" );

template<class ThreadingModel>
IRegistryImpl<ThreadingModel>::~IRegistryImpl() 
{
	logger.log( Logger::DEBUG, "[IRegistryImpl#destructor] Called." );
}

template<class ThreadingModel>
void IRegistryImpl<ThreadingModel>::addBundleInfo( BundleInfoBase* bundleInfo ) 
{
	// !!! synchronized access !!!
        typename ThreadingModel::Lock lock;

	logger.log( Logger::DEBUG, "[IRegistryImpl#addBundleInfo] Called, bundleName: %1", bundleInfo->getBundleName() );
	this->bundleInfoVec.push_back( bundleInfo );
}

template<class ThreadingModel>
BundleInfoBase* IRegistryImpl<ThreadingModel>::getBundleInfo( const string &bundleName )
{
	// !!! synchronized access !!!
        typename ThreadingModel::Lock lock;

	logger.log( Logger::DEBUG, "[IRegistryImpl#getBundleInfo] Called, bundleName: %1", bundleName );
	vector<BundleInfoBase*>::iterator iter;
	for ( iter = this->bundleInfoVec.begin(); iter != this->bundleInfoVec.end(); iter++ )
	{
		if ( (*iter)->getBundleName() == bundleName )
		{
			return (*iter);
		}
	}
	return 0;
}

template<class ThreadingModel>
vector<BundleInfoBase*> IRegistryImpl<ThreadingModel>::getBundleInfos()
{
	// !!! synchronized access !!!
        typename ThreadingModel::Lock lock;

	return this->bundleInfoVec;
}

template<class ThreadingModel>
void IRegistryImpl<ThreadingModel>::removeAllBundleInfos()
{
	// !!! synchronized access !!!
        typename ThreadingModel::Lock lock;

	logger.log( Logger::DEBUG, "[IRegistryImpl#removeAllBundleInfos] Called." );

	// At first the name of the bundles have to be cached in a vector
	// in inverted order the bundles were started.
	vector<string> bundleNames;
	vector<BundleInfoBase*>::reverse_iterator iter;
	for ( iter = this->bundleInfoVec.rbegin(); iter < this->bundleInfoVec.rend(); iter++ )
	{						
		logger.log( Logger::DEBUG, "[IRegistryImpl#removeAllBundleInfos] Add bundle name: %1", (*iter)->getBundleName() );
		bundleNames.push_back( (*iter)->getBundleName() );
	}	

	vector<string>::iterator strIterator;
	for ( strIterator = bundleNames.begin(); strIterator != bundleNames.end(); strIterator++ )
	{
		logger.log( Logger::DEBUG, "[Registry#removeAllBundleInfos] Remove bundle: %1", (*strIterator) );
		this->removeBundleInfo( (*strIterator) );
	}
	logger.log( Logger::DEBUG, "[IRegistryImpl#removeAllBundleInfos] Left." );
}

template<class ThreadingModel>
void IRegistryImpl<ThreadingModel>::removeBundleInfo( const string &bundleName ) 
{
	// !!! synchronized access !!!
        typename ThreadingModel::Lock lock;

	logger.log( Logger::DEBUG, "[IRegistryImpl#removeBundleInfo] Called, bundleName: %1", bundleName );

	BundleInfoBase* bi;
	vector<BundleInfoBase*>::iterator bundleIter;
	for ( bundleIter = this->bundleInfoVec.begin(); bundleIter != this->bundleInfoVec.end(); bundleIter++ )
	{
		if ( (*bundleIter)->getBundleName() == bundleName )
		{
			bi = (*bundleIter);			
			break;
		}
	}

	this->stopActivator( bi );	

	bi->removeAllUsedServices();

	vector<ServiceInfo*> serviceInfos = bi->getRegisteredServices();
	vector<ServiceInfo*>::iterator iter;
	for ( iter = serviceInfos.begin(); iter != serviceInfos.end(); iter++ )
	{
		logger.log( Logger::DEBUG, "[IRegistryImpl#removeBundleInfo] Deregister service: %1", (*iter)->getServiceName() );	
		this->removeServiceInfo( bundleName, *iter );
	}

	vector<ServiceListenerInfo*> serviceListenerInfos = bi->getRegisteredListeners();
	vector<ServiceListenerInfo*>::iterator listenerIter;
	for ( listenerIter = serviceListenerInfos.begin(); listenerIter != serviceListenerInfos.end(); listenerIter++ )
	{
		logger.log( Logger::DEBUG, "[IRegistryImpl#removeBundleInfo] Remove listener: %1", (*listenerIter)->getServiceName() );	
		this->removeServiceListener( bundleName, (*(*listenerIter)) );
	}	

	delete (bi->getBundleContext());
	delete bi;
	logger.log( Logger::DEBUG, "[IRegistryImpl#removeBundleInfo] Erase bundle info." );			
	this->bundleInfoVec.erase( bundleIter );
	logger.log( Logger::DEBUG, "[IRegistryImpl#removeBundleInfo] Left" );			
}

template<class ThreadingModel>
void IRegistryImpl<ThreadingModel>::stopActivator( BundleInfoBase* bi ) 
{
	logger.log( Logger::DEBUG, "[IRegistryImpl#stopActivator] Called." );	
	BundleInfo* bundleInfo = dynamic_cast<BundleInfo*>( bi );
	IBundleActivator* act = bundleInfo->getBundleActivator();
	act->stop( bi->getBundleContext() );
}

template<class ThreadingModel>
void IRegistryImpl<ThreadingModel>::deleteActivator( BundleInfoBase* bi ) 
{
	logger.log( Logger::DEBUG, "[IRegistryImpl#deleteActivator] Called." );	
	BundleInfo* bundleInfo = dynamic_cast<BundleInfo*>( bi );
	IBundleActivator* act = bundleInfo->getBundleActivator();
	delete act;
}

template<class ThreadingModel>
IServiceRegistration::ConstPtr IRegistryImpl<ThreadingModel>::addServiceInfo( const string& bundleName, ServiceInfo* serviceInfo ) 
{	
	// !!! synchronized access !!!
        typename ThreadingModel::Lock lock;

	string serviceName = serviceInfo->getServiceName();
	logger.log( Logger::DEBUG, "[IRegistryImpl#addServiceInfo] Called, bundle name: %1, service name: %2", bundleName, serviceName );		

	this->addToServiceInfoVector( bundleName, serviceName, serviceInfo );

	// adding ServiceInfo object to BundleInfo object
	this->addRegisteredServiceToBundleInfo( bundleName, serviceInfo );	

	// notifying all Listeners about new registered service
	vector<ServiceListenerInfo*>* serviceListenerInfoVec = this->getServiceListenerInfoVector( serviceName );
	this->notifyListenersAboutRegisteredService( bundleName, serviceInfo, serviceListenerInfoVec, serviceName );
	return new IServiceRegistrationImpl( bundleName, this, serviceInfo );
}

template<class ThreadingModel>
void IRegistryImpl<ThreadingModel>::removeServiceInfo( const string& bundleName, ServiceInfo* serviceInfo ) 
{
	// !!! synchronized access !!!
        typename ThreadingModel::Lock lock;

	logger.log( Logger::DEBUG, "[IRegistryImpl#removeServiceInfo] Called, serviceInfo: %1", 
		serviceInfo->toString() );	
	this->removeFromServiceInfoVector( serviceInfo );
	vector<ServiceListenerInfo*>* serviceListenerInfoVec = this->getServiceListenerInfoVector( serviceInfo->getServiceName() );
	this->notifyListenersAboutDeregisteredService( bundleName, serviceInfo, serviceListenerInfoVec );
	this->removeDeregisteredServiceFromBundleInfo( bundleName, serviceInfo );	
	logger.log( Logger::DEBUG, "[IRegistryImpl#removeServiceInfo] Left." );	
}

template<class ThreadingModel>
void IRegistryImpl<ThreadingModel>::addRegisteredServiceToBundleInfo( const string& bundleName, ServiceInfo* serviceInfo ) 
{
	logger.log( Logger::DEBUG, "[IRegistryImpl#addRegisteredServiceToBundleInfo] Called, bundle name: %1, service info: %2", 
		bundleName, serviceInfo->toString() );	
	BundleInfoBase* bundleInfo = this->getBundleInfo( bundleName );
	if ( bundleInfo == 0 )
	{
		logger.log( Logger::DEBUG, "[IRegistryImpl#addRegisteredServiceToBundleInfo] No bundle info available, do nothing." ); 	
	}
	else
	{
		logger.log( Logger::DEBUG, "[IRegistryImpl#addRegisteredServiceToBundleInfo] Add registered service to bundle info." ); 	
		bundleInfo->addRegisteredService( serviceInfo );
	}	
}

template<class ThreadingModel>
void IRegistryImpl<ThreadingModel>::removeDeregisteredServiceFromBundleInfo( const string& bundleName, ServiceInfo* serviceInfo ) 
{
	logger.log( Logger::DEBUG, "[IRegistryImpl#removeDeregisteredServiceFromBundleInfo] Called, bundle name: %1, service info: %2", 
		bundleName, serviceInfo->toString() );	
	BundleInfoBase* bundleInfo = this->getBundleInfo( bundleName );
	if ( bundleInfo == 0 )
	{
		logger.log( Logger::DEBUG, "[IRegistryImpl#removeDeregisteredServiceFromBundleInfo] No bundle info available, do nothing." ); 	
	}
	else
	{
		logger.log( Logger::DEBUG, "[IRegistryImpl#removeDeregisteredServiceFromBundleInfo] Remove deregistered service from bundle info." ); 	
		bundleInfo->removeDeregisteredService( serviceInfo );
	}
}

template<class ThreadingModel>
void IRegistryImpl<ThreadingModel>::addUsedServiceToBundleInfo( const string& bundleName, ServiceInfo* serviceInfo )
{
	logger.log( Logger::DEBUG, "[IRegistryImpl#addUsedServiceToBundleInfo] Called, bundle name: %1, service info: %2", 
		bundleName, serviceInfo->toString() );	
	BundleInfoBase* bundleInfo = this->getBundleInfo( bundleName );
	if ( bundleInfo == 0 )
	{
		logger.log( Logger::DEBUG, "[IRegistryImpl#addUsedServiceToBundleInfo] BundleInfo object does not exist (null), do nothing." );	
	}
	else
	{
		bundleInfo->addUsedService( serviceInfo );
	}	
}

template<class ThreadingModel>
void IRegistryImpl<ThreadingModel>::removeUsedServiceFromBundleInfo( const string& bundleName, ServiceInfo* serviceInfo )
{
	logger.log( Logger::DEBUG, "[IRegistryImpl#removeUsedServiceFromBundleInfo] Called, bundle name: %1, service info: %2", 
		bundleName, serviceInfo->toString() );	
	BundleInfoBase* bundleInfo = this->getBundleInfo( bundleName );
	if ( bundleInfo != 0 )
	{
		bundleInfo->removeUsedService( serviceInfo );
	}
	else
	{
		logger.log( Logger::DEBUG, "[IRegistryImpl#removeUsedServiceFromBundleInfo] BundleInfo is null, do nothing." );	
	}
}

template<class ThreadingModel>
void IRegistryImpl<ThreadingModel>::addToServiceInfoVector( const string& bundleName, const string& serviceName, ServiceInfo* serviceInfo ) 
{
	logger.log( Logger::DEBUG, "[IRegistryImpl#addToServiceInfoVector] Called, bundle name: %1, service name: %2", bundleName, serviceName );	

	BundleInfoBase* bundleInfo = this->getBundleInfo( bundleName );
	if ( bundleInfo == 0 )
	{
		logger.log( Logger::DEBUG, "[IRegistryImpl#addToServiceInfoVector] No bundle info available, do not add service." ); 	
		return;
	}

	vector<ServiceInfo*>* serviceVec = this->getServiceInfo( serviceName );

	serviceVec->push_back( serviceInfo );	

	logger.log( Logger::DEBUG, "[IRegistryImpl#addToServiceInfoVector] Left." );	
}

template<class ThreadingModel>
void IRegistryImpl<ThreadingModel>::removeFromServiceInfoVector( ServiceInfo* serviceInfo ) 
{
	logger.log( Logger::DEBUG, "[IRegistryImpl#removeFromServiceInfoVector] Called, service name: %1", serviceInfo->getServiceName() );	

	vector<ServiceInfo*>* vec = this->getServiceInfo( serviceInfo->getServiceName() );

	vector<ServiceInfo*>::iterator iter;	

	for ( iter = vec->begin(); iter != vec->end(); iter++ )
	{
		// TODO: check why following statement can not be used (ServiceInfo class is called)
		//if ( (*(*iter)) == (*serviceInfo) )

		if ( (*iter)->equals( (*(*iter)), (*serviceInfo) ) )
		{
			logger.log( Logger::DEBUG, "[IRegistryImpl#removeFromServiceInfoVector] Service was found in ServiceInfo vector." );	
			iter = vec->erase( iter );
			break;
		}	
	}
	logger.log( Logger::DEBUG, "[IRegistryImpl#removeFromServiceInfoVector] Left." );	
}


template<class ThreadingModel>
void IRegistryImpl<ThreadingModel>::notifyListenersAboutRegisteredService( const string& bundleName, vector<ServiceInfo*>* serviceVec, vector<ServiceListenerInfo*>* serviceListenerInfoVec, const string& serviceName ) 
{
	logger.log( Logger::DEBUG, "[IRegistryImpl#notifyListenersAboutRegisteredService] Called." );	

	vector<ServiceListenerInfo*>::iterator listenerIter;
	for ( listenerIter = serviceListenerInfoVec->begin(); listenerIter != serviceListenerInfoVec->end(); listenerIter++ )
	{
		vector<ServiceInfo*>::iterator serviceIter;
		for ( serviceIter = serviceVec->begin(); serviceIter != serviceVec->end(); serviceIter++ )
		{
			ServiceReference* serviceRef = this->createServiceReference( *(*serviceIter) );
			ServiceEvent serviceEvent( ServiceEvent::REGISTER, (*serviceRef) );

			bool interested = this->callServiceListenerObject( (*(*listenerIter)), serviceEvent );

			delete serviceRef;
			if ( interested )
			{
				logger.log( Logger::DEBUG, "[IRegistryImpl#notifyListenersAboutRegisteredService] Service listener is interested in registered service '%1'.",
					(*serviceIter)->getServiceName() );
				this->addUsedServiceToBundleInfo( bundleName, (*serviceIter) );	
			} else
			{
				logger.log( Logger::DEBUG, "[IRegistryImpl#notifyListenersAboutRegisteredService] Service listener is NOT interested in registered service '%1'.",
					(*serviceIter)->getServiceName() );
			}
		}
	}	

	logger.log( Logger::DEBUG, "[IRegistryImpl#notifyListenersAboutRegisteredService] Left." );	
}

template<class ThreadingModel>
void IRegistryImpl<ThreadingModel>::notifyListenersAboutRegisteredService( const string& bundleName, ServiceInfo* serviceInfo, vector<ServiceListenerInfo*>* serviceListenerInfoVec, const string& serviceName ) 
{
	logger.log( Logger::DEBUG, "[IRegistryImpl#notifyListenersAboutRegisteredService] Called." );	

	vector<ServiceListenerInfo*>::iterator listenerIter;
	for ( listenerIter = serviceListenerInfoVec->begin(); listenerIter != serviceListenerInfoVec->end(); listenerIter++ )
	{
		ServiceReference* serviceRef = this->createServiceReference( *serviceInfo );
		ServiceEvent serviceEvent( ServiceEvent::REGISTER, (*serviceRef) );

		bool interested = this->callServiceListenerObject( (*(*listenerIter)), serviceEvent );

		if ( interested )
		{
			logger.log( Logger::DEBUG, "[IRegistryImpl#notifyListenersAboutRegisteredService] Service listener is interested in registered service '%1'.",
				serviceInfo->getServiceName() );
			this->addUsedServiceToBundleInfo( (*listenerIter)->getBundleName(), serviceInfo );		
		} else
		{
			logger.log( Logger::DEBUG, "[IRegistryImpl#notifyListenersAboutRegisteredService] Service listener is NOT interested in registered service '%1'.",
				serviceInfo->getServiceName() );
		}
	}	

	logger.log( Logger::DEBUG, "[IRegistryImpl#notifyListenersAboutRegisteredService] Left." );	
}

template<class ThreadingModel>
void IRegistryImpl<ThreadingModel>::notifyListenerAboutRegisteredService( const string& bundleName, vector<ServiceInfo*>* serviceVec, ServiceListenerInfo* serviceListenerInfo, const string& serviceName ) 
{
	logger.log( Logger::DEBUG, "[IRegistryImpl#notifyListenerAboutRegisteredService] Called." );	

	vector<ServiceInfo*>::iterator serviceIter;
	logger.log( Logger::DEBUG, "[IRegistryImpl#notifyListenerAboutRegisteredService] Iterate through service info vector." );	
	for ( serviceIter = serviceVec->begin(); serviceIter != serviceVec->end(); serviceIter++ )
	{
		ServiceReference* serviceRef = this->createServiceReference( *(*serviceIter));
		ServiceEvent serviceEvent( ServiceEvent::REGISTER, (*serviceRef) );
		logger.log( Logger::DEBUG, "[IRegistryImpl#notifyListenerAboutRegisteredService] Service name: %1",				
			(*serviceIter)->getServiceName() );
		bool interested = this->callServiceListenerObject( (*serviceListenerInfo), serviceEvent );
		delete serviceRef;
		if ( interested )
		{
			logger.log( Logger::DEBUG, "[IRegistryImpl#notifyListenerAboutRegisteredService] Service listener is interested in registered service '%1'.",
				(*serviceIter)->getServiceName() );
			this->addUsedServiceToBundleInfo( bundleName, (*serviceIter) );	
		} else
		{
			logger.log( Logger::DEBUG, "[IRegistryImpl#notifyListenerAboutRegisteredService] Service listener is NOT interested in registered service '%1'.",
				(*serviceIter)->getServiceName() );
		}
	}


	logger.log( Logger::DEBUG, "[IRegistryImpl#notifyListenerAboutRegisteredService] Left." );	
}

template<class ThreadingModel>
void IRegistryImpl<ThreadingModel>::notifyListenersAboutDeregisteredService( const string& bundleName, ServiceInfo* serviceInfo, vector<ServiceListenerInfo*>* serviceListenerInfoVec ) 
{
	logger.log( Logger::DEBUG, "[IRegistryImpl#notifyListenersAboutDeregisteredService] Called." );	

	vector<ServiceListenerInfo*>::iterator listenerIter;
	for ( listenerIter = serviceListenerInfoVec->begin(); listenerIter != serviceListenerInfoVec->end(); listenerIter++ )
	{
		ServiceReference* serviceRef = this->createServiceReference( *serviceInfo );
		ServiceEvent serviceEvent( ServiceEvent::UNREGISTER, (*serviceRef) );

		bool interested = this->callServiceListenerObject( (*(*listenerIter)), serviceEvent );

		if ( interested )
		{
			logger.log( Logger::DEBUG, "[IRegistryImpl#notifyListenersAboutDeregisteredService] Listener is interested in deregistered service '%1'.", serviceInfo->getServiceName() );	
			this->removeUsedServiceFromBundleInfo( (*listenerIter)->getBundleName(), serviceInfo );		
		} else
		{
			logger.log( Logger::DEBUG, "[IRegistryImpl#notifyListenersAboutDeregisteredService] Listener is NOT interested in deregistered service '%1'.", serviceInfo->getServiceName() );	
		}

	}	

	logger.log( Logger::DEBUG, "[IRegistryImpl#notifyListenersAboutDeregisteredService] Left." );	
}

template<class ThreadingModel>
vector<ServiceInfo*>* IRegistryImpl<ThreadingModel>::getServiceInfo( const string& serviceName ) 
{
	// !!! synchronized access !!!
        typename ThreadingModel::Lock lock;

	logger.log( Logger::DEBUG, "[IRegistryImpl#getServiceInfo] Called, service name: %1", serviceName );	
	vector<ServiceInfo*>* vec = this->serviceInfoMap[serviceName];
	if ( vec == 0 )
	{
		logger.log( Logger::DEBUG, "[IRegistryImpl#getServiceInfo] ServiceInfo vector is null, create one." );		
		vec = new vector<ServiceInfo*>;
		this->serviceInfoMap[serviceName] = vec;
	}
	logger.log( Logger::DEBUG, "[IRegistryImpl#getServiceInfo] Left." );		
	return vec;
}

template<class ThreadingModel>
vector<ServiceListenerInfo*>* IRegistryImpl<ThreadingModel>::getServiceListenerInfoVector( const string& serviceName ) 
{
	logger.log( Logger::DEBUG, "[IRegistryImpl#getServiceListenerInfoVector] Called, service name: %1", serviceName );	
	vector<ServiceListenerInfo*>* vec = this->serviceListenerMap[serviceName];
	if ( vec == 0 )
	{
		logger.log( Logger::DEBUG, "[IRegistryImpl#getServiceListenerInfoVector] ServiceListenerInfo vector is null, create one." );				
		vec = new vector<ServiceListenerInfo*>;
		this->serviceListenerMap[serviceName] = vec;
	}
	logger.log( Logger::DEBUG, "[IRegistryImpl#ServiceListenerInfo] Left." );	
	return vec;
}

template<class ThreadingModel>
void IRegistryImpl<ThreadingModel>::addServiceListener( const string& bundleName, ServiceListenerInfo* listenerInfo ) 
{
	// !!! synchronized access !!!
        typename ThreadingModel::Lock lock;

	string serviceName = listenerInfo->getServiceName();
	logger.log( Logger::DEBUG, "[IRegistryImpl#addServiceListener] Called, bundle name: %1, service name: %2", bundleName, serviceName );	

	BundleInfoBase* bundleInfo = this->getBundleInfo( bundleName );

	if ( bundleInfo == 0 )
	{
		logger.log( Logger::DEBUG, "[IRegistryImpl#addServiceListener] Bundle info is null, can not add service listener." );	
	}
	else
	{
		logger.log( Logger::DEBUG, "[IRegistryImpl#addServiceListener] Add service listener to service listener vector." );	
		vector<ServiceListenerInfo*>* vec = this->getServiceListenerInfoVector( serviceName );
		vec->push_back( listenerInfo );
		logger.log( Logger::DEBUG, "[IRegistryImpl#addServiceListener] Add service listener to bundle info." );	
		bundleInfo->addRegisteredListener( listenerInfo );
	}

	vector<ServiceInfo*>* serviceVec = this->getServiceInfo( serviceName );

	this->notifyListenerAboutRegisteredService( bundleName, serviceVec, listenerInfo, serviceName );	
	logger.log( Logger::DEBUG, "[IRegistryImpl#addServiceListener] Left." );				
}

template<class ThreadingModel>
void IRegistryImpl<ThreadingModel>::removeServiceListener( const string& bundleName, const ServiceListenerInfo& serviceListener ) 
{
	// !!! synchronized access !!!
        typename ThreadingModel::Lock lock;

	logger.log( Logger::DEBUG, "[IRegistryImpl#removeServiceListener] Called, bundle name: %1", bundleName );
	this->removeFromServiceListenerInfoVector( bundleName, serviceListener );
	logger.log( Logger::DEBUG, "[IRegistryImpl#removeServiceListener] Left" );
}

template<class ThreadingModel>
void IRegistryImpl<ThreadingModel>::removeFromServiceListenerInfoVector( const string& bundleName, const ServiceListenerInfo& serviceListener )
{
	logger.log( Logger::DEBUG, "[IRegistryImpl#removeFromServiceListenerInfoVector] Called." );
	map<string, vector<ServiceListenerInfo*>* >::iterator iter;   
	for( iter = this->serviceListenerMap.begin(); iter != this->serviceListenerMap.end(); iter++ ) {
		vector<ServiceListenerInfo*>* vec = iter->second;
		vector<ServiceListenerInfo*>::iterator vecIterator;
		for ( vecIterator = vec->begin(); vecIterator != vec->end(); vecIterator++ )
		{
			if ( this->areServiceListenerObjectsEqual( (*(*vecIterator)), serviceListener ) )
			{											
				BundleInfoBase* bundleInfo  = this->getBundleInfo( bundleName );
				if ( bundleInfo == 0 )
				{
					logger.log( Logger::DEBUG, "[IRegistryImpl#removeFromServiceListenerInfoVector] BundleInfo object does not exist (null), do nothing." );	
				}
				else
				{
					logger.log( Logger::DEBUG, "[IRegistryImpl#removeFromServiceListenerInfoVector] BundleInfo object exists, remove from vector." );	
					bundleInfo->removeUsedService( (*vecIterator)->getServiceName() );
					bundleInfo->removeRegisteredListener( (*vecIterator) );												
				}	

				logger.log( Logger::DEBUG, "[IRegistryImpl#removeFromServiceListenerInfoVector] Removed." );				
				vec->erase( vecIterator );
				break;
			}
		}
	}	
	logger.log( Logger::DEBUG, "[IRegistryImpl#removeFromServiceListenerInfoVector] Left." );
}

template<class ThreadingModel>
bool IRegistryImpl<ThreadingModel>::areServiceListenerObjectsEqual( const ServiceListenerInfo& info1, const ServiceListenerInfo& info2 )
{
	logger.log( Logger::DEBUG, "[IRegistryImpl#areServiceListenerObjectsEqual] Called, info1: %1, info2: %2", info1.toString(), info2.toString() );
	if ( info1.getServiceListenerObj() == info2.getServiceListenerObj() )
	{
		logger.log( Logger::DEBUG, "[IRegistryImpl#areServiceListenerObjectsEqual] Objects are equal." );	
		return true;
	}
	else
	{
		logger.log( Logger::DEBUG, "[IRegistryImpl#areServiceListenerObjectsEqual] Objects are NOT equal." );	
		return false;
	}	
}

template<class ThreadingModel>
bool IRegistryImpl<ThreadingModel>::callServiceListenerObject( const ServiceListenerInfo& info, const ServiceEvent& serviceEvent )
{
	logger.log( Logger::DEBUG, "[IRegistryImpl#callServiceListenerObject] Called, listenerInfo: %1, event: %2", 
		info.toString(), serviceEvent.toString() );
	return info.getServiceListenerObj()->serviceChanged( serviceEvent );
}

template<class ThreadingModel>
ServiceReference* IRegistryImpl<ThreadingModel>::createServiceReference( const ServiceInfo& serviceInfo )
{
	logger.log( Logger::DEBUG, "[IRegistryImpl#createServiceReference] Called, serviceInfo: %1", 
		serviceInfo.toString() );

	return new ServiceReference( serviceInfo.getServiceName(), serviceInfo.getProperties(), serviceInfo.getService() );	
}
//-------------------
}}
#endif
