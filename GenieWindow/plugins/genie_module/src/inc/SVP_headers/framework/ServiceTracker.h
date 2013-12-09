#ifndef SERVICE_TRACKER_H
#define SERVICE_TRACKER_H

#include <string>

#include "IBundleContext.h"
#include "IServiceTrackerCustomizer.h"
#include "IServiceListener.h"
#include "ServiceEvent.h"

#include "../util/logging/Logger.h"
#include "../util/logging/LoggerFactory.h"


using namespace std;

using namespace svp::framework;
using namespace svp::util::logging;

namespace svp { namespace framework {

/**
 * The <code>ServiceTracker</code> class implements the
 * <code>IServiceListener</code> interface and listens for
 * registered and unregistered services.<br>
 * The service tracker class simplifies the work of application
 * developers for tracking services.
 *
 * @author magr74
 */
class ServiceTracker : public IServiceListener
{
	private:

		/**
		 * The bundle context.
		 */
		IBundleContext::ConstPtr bundleCtxt;

		/**
		 * The customized service tracker which is notified
		 * when a service is registered or deregistered.
		 */
		IServiceTrackerCustomizer::ConstPtr serviceTracker;

		/**
		 * The name of the service the service tracker listens for.
		 */
		string serviceName;

		/**
		 * The logger instance.
		 */
		static Logger& logger;

	public:

		/**
		 * Creates instances of class <code>ServiceTracker</code>.
		 *
		 * @param bc
		 *			The bundle context.
		 *
		 * @param servName
		 *			The name of the service the service tracker listens for.
		 *
		 * @param customizer
		 *			The service tracker customizer which is notified when
		 *			a service is registered or deregistered.
		 */
		ServiceTracker( IBundleContext::ConstPtr bc, const string &servName, 
			IServiceTrackerCustomizer::ConstPtr customizer );

		/**
		 * Destroys the service tracker object.
		 */
		~ServiceTracker();

		/**
		 * Starts tracking for the specified service name. From now on
		 * the <code>IServiceTrackerCustomizer</code> instance is notified
		 * if a service is registered or deregistered.
		 */
		void startTracking();
		
		/**
		 * Stops service tracking. From now on
		 * the <code>IServiceTrackerCustomizer</code> instance 
		 * is no longer notified
		 * if a service is registered or deregistered.
		 */
		void stopTracking();

		/**
		 * Is called by the framework if the lifecycle of a service
		 * changed (registering, deregistering).
		 *
		 * @param serviceEvent
		 *			The event which occurred (registering, deregistering).
		 *
		 * @return True, if the service tracker is interested in the
		 *			changed service, otherwise false.
		 */
		bool serviceChanged( const ServiceEvent &serviceEvent );
		
};

}}

#endif