#ifndef CONFIGURATION_EXCEPTION_H
#define CONFIGURATION_EXCEPTION_H

#include <stdexcept>
#include <string>

#include "../framework/SVPException.h"

namespace svp { namespace config {

using namespace std;

using namespace svp::framework;

/**
 * The <code>ConfigurationException</code> indicates that
 * something went wrong during reading configuration file.<br>
 * See class {@link svp::config::ConfigFileReader}.
 *
 * @author magr74
 */
class ConfigurationException : public SVPException
{

	public:

		/**
		 * Creates instances of class <code>ConfigurationException</code>.
		 *
		 * @param msg
		 *			The message text which describes the exception.
		 */
		ConfigurationException( const string &msg );


};

}}
#endif