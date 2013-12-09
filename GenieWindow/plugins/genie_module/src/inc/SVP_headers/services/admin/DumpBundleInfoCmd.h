#ifndef DUMP_BUNDLE_INFO_CMD_H
#define DUMP_BUNDLE_INFO_CMD_H

#include <string>
#include <vector>

#include "ConsoleCommand.h"

namespace svp { namespace services { namespace admin {

using namespace std;

using namespace svp::services::admin;

/**
 * The <code>DumpBundleInfoCmd<code> represents a console command
 * which dumps all bundle data of a specific bundle.<br>
 * Example:<br>
 * <code>dbi <bundle_name></code>
 *
 * @author magr74
 */
class DumpBundleInfoCmd : public ConsoleCommand
{
	public:

		/**
		 * Creates instances of class </code>DumpBundleInfoCmd</code>.
		 */
		DumpBundleInfoCmd();

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