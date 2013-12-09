#ifndef SERVICE_EVENT_H
#define SERVICE_EVENT_H

#include <string>
#include <sstream>

#include "ServiceReference.h"

namespace svp { namespace framework {

using namespace svp::framework;
using namespace std;

/**
 * The <code>ServiceEvent</code> class describes
 * a change in the lifecycle of a service.<br>
 * Currently there are two events defined:<br>
 * <ul>
 *		<li>REGISTER: Service is registered with the 
 *						framework.
 *		<li>UNREGISTER: Service is unregistered with the 
 *						framework.
 * </ul>
 *
 * @author magr74
 */
class ServiceEvent
{
	private:		
		
		/**
		 * The type of the event.
		 */
		int type;

		/**
		 * The service reference which represents
		 * the service whose lifecycle changed.
		 */
		ServiceReference &reference;

	public:

		/**
		 * Definition of the events.
		 */
		enum EventType { REGISTER, UNREGISTER };

	public:

		/**
		 * Creates instances of class <code>ServiceEvent</code>.
		 *
		 * @param type
		 *				The type of the event.
		 *
		 * @param reference
		 *				Describes the service.
		 */
		ServiceEvent( const int &type, ServiceReference &reference );

		/**
		 * Returns the type of the event.
		 *
		 * @return
		 *			The event type.
		 */
		int getType() const;

		/**
		 * Returns the service reference.
		 *
		 * @return
		 *			The <code>ServiceReference</code> object.
		 */
		ServiceReference& getReference() const;

		/**
		 * Returns a string representation of the <code>ServiceEvent</code> object.
		 *
		 * @return
		 *			The string representation.
		 */
		string toString() const;

};

}}
#endif