#ifndef IADMINISTRATION_SERVICE_H
#define IADMINISTRATION_SERVICE_H

#include <string>
#include <vector>

#include "../../framework/IService.h"

using namespace std;
using namespace svp::framework;

namespace svp { namespace services { namespace admin {

/**
 * The <code>IAdministrationService</code> interface
 * defines methods for administrating the framework 
 * (stopping and starting bundles, dumping bundle 
 * information etc.).<br>
 * Each command which is executed in the administration
 * console has access to this interface.<br>
 * The administration service is not registered with
 * the framework and can not be used by other bundles.
 *
 * @author magr74
 */
class IAdministrationService : public IService
{
	public:

		/**
		 * Returns a vector containing all names of the bundles
		 * which are currently started.
		 *
		 * @return
		 *			A vector of bundle names.		 
		 */
		virtual vector<string> getBundleNames() = 0;			

		/**
		 * Dumps the bundle information of the specified bundle.
		 *
		 * @param bundleName
		 *			The name of the bundle which is dumped.
		 *
		 * @return  
		 *			The string containing the complete bundle information
		 *			(registered services and service listeners etc.)
		 */
		virtual string dumpBundleInfo( const string& bundleName ) = 0;

		/**
		 * Dumps the name of all started bundles.		 
		 *
		 * @return  
		 *			The string containing the names of all started
		 *			bundles.
		 */
		virtual string dumpAllBundleNames() = 0;

		/**
		 * Stops the specified bundle.
		 *
		 * @param bundleName
		 *				The name of the bundle which is stopped.
		 */
		virtual void stopBundle( const string& bundleName ) = 0;

		/**
		 * Stops all bundles.
		 */
		virtual void stopAllBundles() = 0;

		/**
		 * Starts a bundle which must be loaded from a DLL.
		 *
		 * @param bundleName
		 *				The name of the bundle which is started.
		 *
		 * @param className
		 *				The class name of the activator.
		 *
		 * @param libPath
		 *				The library path where the DLL can be find.
		 *
		 * @param libName
		 *				The name of the DLL which is loaded.
		 */
		virtual void startBundleFromDLL( const string& bundleName, const string& className, const string& libPath, const string& libName ) = 0;

		/**
		 * Starts a 'local' bundle.
		 *
		 * @param bundleName
		 *			The name of the bundle.
		 *
		 * @param className
		 *			The name of the activator class.
		 */
		virtual void startBundle( const string& bundleName, const string& className ) = 0;

		/**
		 * Starts bundles which are defined in a configuration file.
		 *
		 * @param configFile
		 *			The configuration file which contains the bundle data
		 *			(activator class name, name of the DLL etc.)
		 */
		virtual void startBundlesFromConfigFile( const string& configFile ) = 0;
};

}}}
#endif