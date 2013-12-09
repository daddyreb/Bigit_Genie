#ifndef START_BUNDLES_FROM_FILE_CMD_H
#define START_BUNDLES_FROM_FILE_CMD_H

#include <string>
#include <vector>

#include "ConsoleCommand.h"

namespace svp { namespace services { namespace admin {

using namespace std;

using namespace svp::services::admin;

/**
 * The <code>StartBundlesFromFileCmd<code> represents a console command
 * which starts bundles specified in a configuration file.<br>
 * Example:<br>
 * <code>stbfile <config_file></code>
 *
 * @author magr74
 */
class StartBundlesFromFileCmd : public ConsoleCommand
{
	public:

		/**
		 * Creates instances of class </code>StartBundlesFromFileCmd</code>.
		 */
		StartBundlesFromFileCmd();

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