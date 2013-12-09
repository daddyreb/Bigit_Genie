#ifndef CONSOLE_COMMAND_H
#define CONSOLE_COMMAND_H

#include <string>
#include <vector>

#include "IAdministrationService.h"

namespace svp { namespace services { namespace admin {

using namespace std;

using namespace svp::services::admin;

/**
 * The <code>ConsoleCommand</code> is the base class for all
 * commands which are executed within the administration console.
 *
 * @author magr74
 */
class ConsoleCommand
{
	public:		

		/**
		 * Returns the name of the command which must be typed
		 * in administration console for executing.
		 *
		 * @return 
		 *		The name of the command.
		 */
		virtual string getName() = 0;

		/**
		 * Returns the description of the command. Can be called
		 * in the administration console.
		 *
		 * @return
		 *		The description (what the command does) of the command.
		 */
		virtual string getDescription() = 0;

		/**
		 * Returns the number parameters the command expects.
		 *
		 * @return
		 *		The number of parameters.
		 *
		 */
		virtual int getParameterNum() = 0;

		/**
		 * Executes the commmand.
		 *
		 * @param adminService
		 *				The <code>IAdministrationService</code> interface providing
		 *				the administration functionality which are executed by the commands.
		 *
		 * @param params
		 *				A vector of strings containing the parameter values.
		 *
		 * @return
		 *				The result string of the command which is displayed in the console.
		 */		
		virtual string execute( IAdministrationService* const adminService, vector<string> params ) = 0;
};

}}}
#endif