#ifndef START_BUNDLE_CMD_H
#define START_BUNDLE_CMD_H

#include <string>
#include <vector>

#include "ConsoleCommand.h"

namespace svp { namespace services { namespace admin {

using namespace std;

using namespace svp::services::admin;

/**
 * The <code>StartBundleCmd<code> represents a console command
 * which starts a 'local' bundle.<br>
 * Example:<br>
 * <code>stb <bundle_name> <class_name></code>
 *
 * @author magr74
 */
class StartBundleCmd : public ConsoleCommand
{
	public:

		/**
		 * Creates instances of class </code>StartBundleCmd</code>.
		 */
		StartBundleCmd();

		/**
		 * @see svp::services::admin::ConsoleCommand::getName
		 */
		string getName();

		/**
		 * @see svp::services::admin::ConsoleCommand::getDescription
		 */
		string getDescription();

		/**
		 * @see svp::services::admin::ConsoleCommand::getParameterNum
		 */
		int getParameterNum();

		/**
		 * @see svp::services::admin::ConsoleCommand::execute
		 */
		string execute( IAdministrationService* const adminService, vector<string> params );
};

}}}

#endif