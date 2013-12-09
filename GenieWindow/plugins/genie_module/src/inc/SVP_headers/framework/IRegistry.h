#ifndef IREGISTRY_H
#define IREGISTRY_H

#include <string>
#include <vector>

#include "BundleInfoBase.h"
#include "ServiceInfo.h"
#include "ServiceListenerInfo.h"
#include "IService.h"
#include "Properties.h"
#include "IServiceRegistration.h"

using namespace std;

using namespace svp::framework;

namespace svp { namespace framework {

/**
 * The <code>IRegistry</code> class defines the interface of the
 * registry implementation (see class {@link svp::framework::IRegistryImpl}), which holds
 * all relevant bundle data.<br>
 * This interface was introduced in order to avoid template dependency for classes which use
 * the registry. Other classes use the registry only via the interface.
 *
 * @author magr74
 */
class IRegistry
{
	public:

		/**
		 * Adds a <code>BundleInfo</code> object to the registry.
		 *
		 * @param bundleInfo
		 *			The <code>BundleInfo</code> object which describes a bundle.
		 */
		virtual void addBundleInfo( BundleInfoBase* bundleInfo ) = 0;

		/**
		 * Returns the <code>BundleInfo</code> object of a specific bundle.
		 *
		 * @param  bundleName
		 *				The name of the bundle whose bundle info object is returned.
		 *
		 * @return	
		 *				The <code>BundleInfo</code> object.
		 */
		virtual BundleInfoBase* getBundleInfo( const string& bundleName ) = 0;

		/**
		 * Returns all <code>BundleInfo</code> objects which are currently
		 * stored in registry.
		 *
		 * @return	
		 *				A vector of <code>BundleInfo</code> objects.
		 *
		 */
		virtual vector<BundleInfoBase*> getBundleInfos() = 0;

		/**
		 * Removes the <code>BundleInfo</code> object of a specific bundle.
		 *
		 * @param bundleName
		 *				The name of the bundle whose bundle info object is removed.
		 */
		virtual void removeBundleInfo( const string& bundleName ) = 0;

		/**
		 * Removes all <code>BundleInfo</code> objects which are currently
		 * stored in registry.		 
		 */
		virtual void removeAllBundleInfos() = 0;

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
		virtual IServiceRegistration::ConstPtr addServiceInfo( const string& bundleName, ServiceInfo* serviceInfo ) = 0;
		
		/**
		 * Removes a <code>ServiceInfo</code> object from the registry.
		 *
		 * @param bundleName
		 *				The name of the bundle the service belongs to.
		 *
		 * @param serviceInfo
		 *				The <code>ServiceInfo</code> object describing the service.
		 */
		virtual void removeServiceInfo( const string& bundleName, ServiceInfo* serviceInfo ) = 0;

		/**
		 * Returns the <code>ServiceInfo</code> object for a specific service.
		 *
		 * @param serviceName
		 *				The name of the service.
		 *
		 * @return 
		 *				A pointer to a vector of pointers to <code>ServiceInfo</code> objects.
		 */
		virtual vector<ServiceInfo*>* getServiceInfo( const string &serviceName ) = 0;

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
		virtual void addServiceListener( const string& bundleName, ServiceListenerInfo* listenerInfo ) = 0;
		
		/**
		 * Removes a service listener object from the registry.
		 *
		 * @param bundleName
		 *				The name of the bundle the service listener belongs to.
		 *
		 * @param serviceListener
		 *				The pointer to the service listener object.
		 */
		virtual void removeServiceListener( const string& bundleName, const ServiceListenerInfo& info ) = 0;
		
};

}}
#endif