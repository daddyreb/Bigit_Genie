#ifndef QT_DLL_CREATOR_H
#define QT_DLL_CREATOR_H

#include <string>
#include <QLibrary>

#include "../../util/logging/Logger.h"
#include "../../util/logging/LoggerFactory.h"
#include "../ObjectCreationException.h"

using namespace std;

using namespace svp::instantiation;
using namespace svp::util::logging;

namespace svp { namespace instantiation { namespace qt {

template <typename BaseT>
class QtDllCreator
{
	public:

		/**
		 * Creates an object from a Windows DLL.
		 *
		 * @param path
		 *			The path where the Windows DLL is located.
		 * @param dllName
		 *			The name of the Windows DLL.
		 * @param className
		 *			The name of the class an object is created from.
		 * @return
		 *			The pointer to the created object.
		 */
		static BaseT* createObjectFromDll( const string &path, const string &dllName, const string &className );
		
		/**
		 * Returns a logger instance.
		 *
		 * @return
		 *		Returns an instance of class <code>Logger</code>.
		 */
		static Logger& getLogger();
};

template <typename BaseT>
Logger& QtDllCreator<BaseT>::getLogger()
{
	static Logger& logger = LoggerFactory::getLogger( "ObjectCreation" );
	return logger;
}

template <typename BaseT>
BaseT* QtDllCreator<BaseT>::createObjectFromDll( const string &path, const string &dllName, const string &className )
{
	typedef BaseT* (*DLLPROC) ( const string& );

	DLLPROC pFunc = NULL;

	ostringstream str;
	
	int pos = path.find_last_of( '/' );
	if ( pos == ( path.size() - 1 ) )
	{
		str << path << dllName;	
	}
	else
	{
		str << path << '/' << dllName;	
	}
	
	getLogger().log( Logger::DEBUG, "[QtDllCreator#createObjectFromDll] Loading DLL: '%1'", str.str() );

	QLibrary qLib(str.str().c_str());
	try
	{
		qLib.load();
	}
	catch( exception &exc )
	{
		getLogger().log( Logger::ERROR_, "[QtDllCreator#createObjectFromDll] Error occurred during loading DLL: %1", exc.what() );
	}

	if (qLib.isLoaded()==false)
	{
                ObjectCreationException exc( "(Qt load)Error during loading DLL." );
		throw exc;
	}
	
	try
	{
		pFunc = ( DLLPROC )qLib.resolve("createObject"); 
	}
	catch( exception &exc)
	{
		getLogger().log( Logger::ERROR_, "[QtDllCreator#createObjectFromDll] Error occurred during calling DLL entry method, %1", exc.what() );
		pFunc = NULL;
	}

	if ( pFunc == NULL )
	{
                ObjectCreationException exc( "(Qt load)Error during loading object from DLL." );
		throw exc;
	}	
	return ((*pFunc)(className));
}

}}}
#endif
