#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <vector>

namespace svp { namespace util { namespace logging {

using namespace std;

/**
 * The <code>Logger</code> interface defines methods for printing log
 * messages (to the console). A log message consists of three parts: <br>
 * <ul>
 *		<li>The log level defining whether an error occurred or there is only
 *			an information for the user.
 *		<li>The message string describing what happened.
 *		<li>The parameters containing the current values of variables.<br>
 * </ul><br>
 * Example:<p>
 * logInstance.log( Logger::DEBUG, "Value of the loop variable: %1", counter );<p>
 *
 * The <code>log</code> call in the example leads to following output (if loop variable
 * has the value "2"):<p>
 * 'Value of the loop variable: 2'<p>
 * 
 * That means that the passed parameters can be written into the message text by using
 * the '%' character. The number behind the "%" character defines which parameter (the first,
 * second etc.) must be placed.
 * 
 *
 * @author magr74
 */
class Logger
{	
	public:

		/**
		 * Defines the possible log levels. A log message can be logged
		 * as error, trace or debug message.<br>
		 * Note: The "_" after 'ERROR' is a workaround and 
		 * is written due to compiling problems.
		 */
		enum LogLevel { NOLOG, ERROR_ = 1, TRACE, DEBUG };

		/**
		 * Prints out a log message.
		 *
		 * @param level
		 *			The log level. The log level must have one of following values:
		 *			ERROR_, TRACE, DEBUG.
		 * @param message
		 *			The text which describes what happened.
		 *
		 * @param param1
		 *			The first parameter.
		 *
		 * @param param2
		 *			The second parameter.
		 *
		 * @param param3
		 *			The third parameter.
		 *
		 */
		virtual void log( LogLevel level, const string& message, const string& param1,
				const string& param2, const string& param3 ) = 0;
				
		/**
		 * Prints out a log message.
		 *
		 * @param level
		 *			The log level.
		 * @param message
		 *			The text which describes what happened.
		 *
		 * @param param1
		 *			The first parameter.		 
		 */
		virtual void log( LogLevel level, const string& message, const string& param1 ) = 0;
		
		/**
		 * Prints out a log message.
		 *
		 * @param level
		 *			The log level.
		 * @param message
		 *			The text which describes what happened.
		 *
		 * @param param1
		 *			The first parameter.
		 *
		 * @param param2
		 *			The second parameter.
		 */
		virtual void log( LogLevel level, const string& message, const string& param1, const string& param2 ) = 0;
		
		/**
		 * Prints out a log message.
		 *
		 * @param level
		 *			The log level.
		 * @param message
		 *			The text which describes what happened.
		 */
		virtual void log( LogLevel level, const string& message ) = 0;
		
		/**
		 * Prints out a log message.
		 *
		 * @param level
		 *			The log level.
		 * @param message
		 *			The text which describes what happened.
		 *
		 * @param param1
		 *			The first parameter.
		 *
		 * @param param2
		 *			The second parameter.
		 *
		 * @param param3
		 *			The third parameter.		 
		 */
		virtual void log( LogLevel level, const string& message, int param1,
				int param2, int param3 ) = 0;
				
		/**
		 * Prints out a log message.
		 *
		 * @param level
		 *			The log level.
		 * @param message
		 *			The text which describes what happened.
		 */
		virtual void log( LogLevel level, const string& message, int param1 ) = 0;
		
		/**
		 * Prints out a log message.
		 *
		 * @param level
		 *			The log level.
		 * @param message
		 *			The text which describes what happened.
		 *
		 * @param param1
		 *			The first parameter.
		 *
		 * @param param2
		 *			The second parameter.		 
		 */
		virtual void log( LogLevel level, const string& message, int param1, int param2 ) = 0;
		
		/**
		 * Prints out a log message.
		 *
		 * @param level
		 *			The log level.
		 * @param message
		 *			The text which describes what happened.
		 *
		 * @param param1
		 *			The first parameter.
		 *
		 */
		virtual void log( LogLevel level, const string& message, bool param1 ) = 0;
		
		/**
		 * Prints out a log message.
		 *
		 * @param level
		 *			The log level.
		 * @param message
		 *			The text which describes what happened.
		 *
		 * @param param1
		 *			The first parameter.
		 *
		 * @param param2
		 *			The second parameter.
		 */
		virtual void log( LogLevel level, const string& message, bool param1, bool param2 ) = 0;
		
		/**
		 * Sets the log level for the logger instance.
		 *
		 * @param level
		 *			If the log level of the logger instance is set
		 *			to NOLOG, no log message will be visible on console.
		 *			The ERROR_ level prints out only the error messages and the
		 *			DEBUG level prints out all kind of log messages (ERROR_, TRACE, DEBUG).
		 *			That means the highest log level (regarding to the enum value) 
		 *			contains the lower levels.
		 */
		virtual void setLogLevel( LogLevel level ) = 0;
		
		
};

}}}
#endif 