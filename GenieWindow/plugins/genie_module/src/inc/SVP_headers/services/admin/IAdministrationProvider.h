#ifndef IADMINISTRATION_PROVIDER_H
#define IADMINISTRATION_PROVIDER_H

#include <string>
#include <vector>
#include <QMainWindow>

#include "../../config/BundleConfiguration.h"

using namespace std;

using namespace svp::config;

namespace svp { namespace services { namespace admin {

/**
 * The <code>IAdministrationProvider</code> interface provides methods
 * for administrating the SOF container (starting, stopping bundles, asking
 * for bundle information etc.)
 *
 * @author magr74
 */
class IAdministrationProvider
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
                 * Starts a specific bundle. Can be also called after
                 * a <code>start()</code>.
                 *
                 * @param bundleConfig
                 *				The object containing information which
                 *				bundle must be started.
                 */
                virtual void startBundle( BundleConfiguration bundleConfig )=0;

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
		virtual void stop() = 0;

		/**
		 * Starts a bundle which must be loaded from a DLL.
		 *
		 * @param config
		 *			The bundle configuration.
		 */ 
		virtual void start( vector<BundleConfiguration> &config ) = 0;

                //------------added by duanlin.yi----------------------
                virtual bool startApp(QMainWindow *main_wnd)=0;
                //-----------------------------------------------------



};

}}}

#endif
