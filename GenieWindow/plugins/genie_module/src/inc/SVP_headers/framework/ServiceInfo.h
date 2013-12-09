#ifndef SERVICE_INFO_H
#define SERVICE_INFO_H

#include <string>
#include <sstream>

#include "IService.h"
#include "Properties.h"

#include "../util/logging/LoggerFactory.h"
#include "../util/logging/Logger.h"

namespace svp { namespace framework {

using namespace std;

using namespace svp::util::logging;
using namespace svp::framework;

/**
 * The <code>ServiceInfo</code> object is a helper class
 * for internal use of the framework.<br>
 * A <code>ServiceInfo</code> object holds all important
 * information about a service object.
 *
 * @author magr74
 */
class ServiceInfo
{
	protected:

		/**
		 * Defines a constant pointer to the service object.
		 */
		IService::ConstPtr service;

		/**
		 * The name of the service object.
		 */
		string serviceName;

		/**
		 * Holds the properties of a service object.
		 */
		Properties props;

		/**
		 * The logger instance.
		 */
		static Logger& logger;
		
	public:

		virtual bool equals( const ServiceInfo& info1, const ServiceInfo& info2 );

		/**
		 * Creates instances of class <code>ServiceInfo</code>.
		 *
		 * @param servName
		 *				The service name.
		 *
		 * @param service
		 *				The constant pointer to the service object.
		 *
		 * @param properties
		 *				The properties describing the service object.
		 */
		ServiceInfo( const string &servName, IService::ConstPtr service, const Properties &properties );

		/**
		 * Destroys the <code>ServiceInfo</code> object.
		 */
		virtual ~ServiceInfo();

		/**
		 * Returns the service name.
		 *
		 * @return
		 *		The name of the service.
		 */
		virtual string getServiceName() const;

		/**
		 * Returns the properties of the service.
		 *
		 * @return
		 *			The properties object.
		 */
		virtual Properties getProperties() const;

		/**
		 * Returns a constant pointer to the service object.
		 *
		 * @return
		 *			The service object.
		 */
		virtual IService::ConstPtr getService() const;

		/**
		 * Returns a string representation of the
		 * <code>ServiceInfo</code> object.
		 *
		 * @return
		 *		A string containing all data of the 
		 *		<code>ServiceInfo</code> object.
		 */
		virtual string toString() const;

		/**
		 * Compares two <code>ServiceInfo</code> objects.
		 *
		 * @return
		 *		True, if the passed service info object 
		 *		is equal to this object, otherwise false.
		 */
		virtual bool operator==( const ServiceInfo& serviceInfo1 );
};

}}
#endif