#ifndef ISERVICE_REGISTRATION_IMPL_H
#define ISERVICE_REGISTRATION_IMPL_H


#include "../util/logging/LoggerFactory.h"
#include "../util/logging/Logger.h"

#include "IServiceRegistration.h"
#include "IRegistry.h"
#include "ServiceInfo.h"

namespace svp { namespace framework {

using namespace svp::framework;
using namespace svp::util::logging;

/**
 * Represents an implementation of the 
 * svp::framework::IServiceRegistration interface.
 *
 * @author magr74
 */
class IServiceRegistrationImpl : public IServiceRegistration
{
	private:

		/**
		 * The registry object which stores all bundle relevant data.
		 */
		IRegistry* const registry;

		/**
		 * The <code>ServiceInfo</code> describing the service which can
		 * be unregistered by this service registration object.
		 */
		ServiceInfo* const serviceInfo;

		/**
         * The name of the bundle which registered the service object.
		 */
		string bundleName;

		/**
		 * The logger instance.
		 */
		static Logger& logger;

	public:

		/**
		 * Creates instances of class <code>IServiceRegistrationImpl</code>.
		 *
		 * @param bundleName
		 *				The name of the bundle.
		 *
		 * @param reg
		 *				The framework registry which holds all bundle relevant data.
		 *
		 * @param serviceInfo
		 *				The service info object.
		 */
		IServiceRegistrationImpl( const string& bundleName, IRegistry* const reg, ServiceInfo* const serviceInfo );

		/**
		 * Deletes the object.
		 */
		~IServiceRegistrationImpl();

		/**
		 * Unregisters the service object with the framework.
		 */
		virtual void unregister();
};

}}
#endif