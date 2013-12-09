#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <vector>
#include <iostream>
#include <sstream>

#include "IBundleActivator.h"
#include "IRegistry.h"
#include "IRegistryImpl.h"
#include "BundleInfo.h"
#include "IBundleContextImpl.h"

#include "services/admin/AdministrationActivator.h"
#include "services/admin/isvpshellbundleactivator.h"
#include "services/admin/IAdministrationProvider.h"
#include "config/BundleConfiguration.h"
#include "instantiation/ObjectCreator.h"
#include "util/logging/Logger.h"
#include "util/logging/LoggerFactory.h"
#include "util/threading/SingleThreaded.h"

//#include "SvpShell.h"
//------------------------added by duanlin.yi-------------
#include "IRunnable.h"
class QMainWindow;
//-----------------------------end------------------------

using namespace std;

using namespace svp::config;
using namespace svp::framework;
using namespace svp::instantiation;
using namespace svp::util::logging;
using namespace svp::util::threading;
using namespace svp::services::admin;

namespace svp { namespace framework {

/**
 * The <code>Launcher</code> class is the entry point for
 * running the SOF framework.<br>
 * The main task of this class is to provide methods
 * for starting and stopping bundles.
 *
 * @author magr74
 */
template<
	class ThreadingModel = SingleThreaded,
	template <class> class CreationPolicy = NullCreator>
class Launcher : public IAdministrationProvider
{
	protected:

		/**
		 * The <code>ObjectCreator</code> instance which is used
		 * for instantiating the <code>IBundleActivator</code>
		 * objects.
		 */
		ObjectCreator<IBundleActivator,CreationPolicy> objectCreator;

		/**
		 * The registry object which holds all relevant data of
		 * all bundles. It is the central administration object.
		 */
		IRegistry* registry;		

		/**
		 * The logger instance.
		 */
		static Logger& logger;

		/**
		 * Creates the registry instance.
		 *
		 * @return
		 *		The registry instance.
		 */
		virtual IRegistry* createRegistry();

		/**
		 * Creates the bundle context instances.
		 *
		 * @param bundleName
		 *				The name of the bundle the bundle context object is
	     *				created for.
		 *
		 * @return
		 *				The bundle context instance.
		 */
		virtual IBundleContext* createBundleContext( const string& bundleName );

	public:

		/**
		 * Creates instances of class <code>Launcher</code>.
		 */
		Launcher();

		/**
		 * Destroys the <code>Launcher</code> instance.
		 */
		virtual ~Launcher();

		/**
		 * Sets the log level of the framework. Defines
		 * for example whether only error messages or
		 * also debug messages shall be logged.		 
		 *
		 * @param level
		 *			The log level (trace, debug, error).
		 */
		virtual void setLogLevel( Logger::LogLevel level );

		/**
		 * Starts bundles. The bundles which are started are
		 * defined in a vector of <code>BundleConfiguration</code>
		 * objects.
		 *
		 * @param configuration
		 *				The vector of <code>BundleConfiguration</code>
		 *				objects whereas each object describes what
		 *				bundle shall be started.
		 */
		virtual void start( vector<BundleConfiguration> &configuration );

		/**
		 * Stops all bundles which were started.
		 */
		virtual void stop();

		/**
		 * Starts a specific bundle. Can be also called after
		 * a <code>start()</code>.
		 *
		 * @param bundleConfig
		 *				The object containing information which
		 *				bundle must be started.
		 */
		virtual void startBundle( BundleConfiguration bundleConfig );

		/**
		 * Stops a bundle.
		 *
		 * @param bundleName
		 *				The name of the bundle which is stopped.
		 */
		virtual void stopBundle( const string& bundleName );

		/**
		 * Starts the administration bundle (which
		 * provides a console for user inputs).
		 */
		virtual void startAdministrationBundle();
		virtual void startSvpShellBundle();

		/**
		 * Returns the names of all started bundles.
		 *
		 * @return
		 *		A vector containing all bundle names.
		 */
		virtual vector<string> getBundleNames();			

		/**
		 * Dumps all information (registered services,
		 * registered service listeners, services in use)
		 * of a bundle.
		 *
		 * @param bundleName
		 *				The name of the bundle.
		 *
		 * @return
		 *				A string containing all information
		 *				about a bundle.
		 */
		virtual string dumpBundleInfo( const string& bundleName );

		/**
		 * Dumps the name of all started bundles.
		 *
		 * @return
		 *			A string containing all bundle names.
		 */
		virtual string dumpAllBundleNames();

		/**
		 * Returns the registry object.
		 *
		 * @return
		 *			The registry object.
		 */
		/*virtual IRegistry& getRegistry();*/

		//------------added by duanlin.yi----------------------
		virtual bool startApp(QMainWindow *main_wnd);
		//-----------------------------------------------------
};

//#include "Launcher.cpp"


template<
	class ThreadingModel,
		template <class> class CreationPolicy>
Logger& Launcher<ThreadingModel, CreationPolicy>::logger = LoggerFactory::getLogger( "Framework" );

template<
	class ThreadingModel,
		template <class> class CreationPolicy>
Launcher<ThreadingModel, CreationPolicy>::Launcher() 
{
	// !!! synchronized access !!!
        typename ThreadingModel::Lock lock;

	logger.log( Logger::DEBUG, "[Launcher#ctor] Called." );
	this->registry = this->createRegistry();
}

template<
	class ThreadingModel,
		template <class> class CreationPolicy>
Launcher<ThreadingModel, CreationPolicy>::~Launcher() 
{
	// !!! synchronized access !!!
        typename ThreadingModel::Lock lock;

	logger.log( Logger::DEBUG, "[Launcher#destructor] Called." );
	delete (this->registry);
}


//template<
//	class ThreadingModel,
//	template <class> class CreationPolicy>
//IRegistry& Launcher<ThreadingModel, CreationPolicy>::getRegistry()
//{
//	return (*(this->registry));
//}

template<
	class ThreadingModel,
		template <class> class CreationPolicy>
void Launcher<ThreadingModel, CreationPolicy>::setLogLevel( Logger::LogLevel level )
{
	LoggerFactory::setLogLevel( level );
}

template<
	class ThreadingModel,
		template <class> class CreationPolicy>
IRegistry* Launcher<ThreadingModel, CreationPolicy>::createRegistry()
{
	logger.log( Logger::DEBUG, "[Launcher#createRegistry] Called." );
	return new IRegistryImpl</*ThreadingModel*/>;
}

template<
	class ThreadingModel,
		template <class> class CreationPolicy>
IBundleContext* Launcher<ThreadingModel, CreationPolicy>::createBundleContext( const string& bundleName )
{
	logger.log( Logger::DEBUG, "[Launcher#createBundleContext] Called." );
	return new IBundleContextImpl( bundleName, this->registry );
}

template<
	class ThreadingModel,
		template <class> class CreationPolicy>
void Launcher<ThreadingModel, CreationPolicy>::start( vector<BundleConfiguration> &configVector )
{
	// !!! synchronized access !!!
        typename ThreadingModel::Lock lock;

	logger.log( Logger::DEBUG, "[Launcher#start] Called." );

	vector<BundleConfiguration>::iterator itVectorData;
	itVectorData = configVector.begin();
	for(itVectorData = configVector.begin(); itVectorData != configVector.end(); itVectorData++)
	{
		BundleConfiguration bundleConfig = *(itVectorData);
		this->objectCreator.setSearchConfiguration( true, 
			bundleConfig.getLibraryPath(), bundleConfig.getLibraryName() );

		logger.log( Logger::DEBUG, "[Launcher#start] Reading configuration: Library path: %1, class name: %2",
			bundleConfig.getLibraryPath(), bundleConfig.getClassName() );

		logger.log( Logger::DEBUG, "[Launcher#start] Loading bundle activator: Library path: %1, class name: %2",
			bundleConfig.getLibraryPath(), bundleConfig.getClassName() );

		IBundleActivator* bundleActivator;
		try
		{
			bundleActivator = this->objectCreator.createObject( bundleConfig.getClassName());
		}
		catch( ObjectCreationException &exc )
		{
			string msg( exc.what() );
			logger.log( Logger::ERROR_, "[Launcher#start] Error during loading bundle activator, exc: %1", msg );
			continue;
		}

		IBundleContext* bundleCtxt = this->createBundleContext( bundleConfig.getBundleName() );

		BundleInfoBase* bundleInfo = new BundleInfo( bundleConfig.getBundleName(), bundleActivator, bundleCtxt );		
		this->registry->addBundleInfo( bundleInfo );

		logger.log( Logger::DEBUG, "[Launcher#start] Start bundle." );

		bundleActivator->start( bundleCtxt );
	}	
}

template<
	class ThreadingModel,
		template <class> class CreationPolicy>
void Launcher<ThreadingModel, CreationPolicy>::startAdministrationBundle()
{
	// !!! synchronized access !!!
        typename ThreadingModel::Lock lock;

	logger.log( Logger::DEBUG, "[Launcher#startAdministrationBundle] Called." );
	IBundleActivator* adminBundleActivator = this->objectCreator.createObject( "svp::services::admin::AdministrationActivator" );
	IBundleContext* bundleCtxt = this->createBundleContext( "AdministrationBundle" );

	BundleInfoBase* bundleInfo = new BundleInfo( "AdministrationBundle", adminBundleActivator, bundleCtxt );		
	this->registry->addBundleInfo( bundleInfo );

	logger.log( Logger::DEBUG, "[Launcher#start] Start bundle." );

	AdministrationActivator* adminActivator = static_cast<AdministrationActivator*> (adminBundleActivator);	
	adminActivator->setAdministrationProvider( this );
	adminActivator->start( bundleCtxt );
}

template<
	class ThreadingModel,
		template <class> class CreationPolicy>
void Launcher<ThreadingModel, CreationPolicy>::startSvpShellBundle()
{
	// !!! synchronized access !!!
        typename ThreadingModel::Lock lock;

	logger.log( Logger::DEBUG, "[Launcher#startSvpShellBundle] Called." );
	IBundleActivator* svpShellBundleActivator = this->objectCreator.createObject( "SvpShell" );
	IBundleContext* bundleCtxt = this->createBundleContext( "SvpShellBundle" );

	BundleInfoBase* bundleInfo = new BundleInfo( "SvpShellBundle", svpShellBundleActivator, bundleCtxt );		
	this->registry->addBundleInfo( bundleInfo );

	logger.log( Logger::DEBUG, "[Launcher#start] Start bundle." );

        ISvpShellBundleActivator* svp_shell = static_cast<ISvpShellBundleActivator*> (svpShellBundleActivator);
        svp_shell->setPlatformLauncher( this );
        svp_shell->start( bundleCtxt );
}

template<
	class ThreadingModel,
		template <class> class CreationPolicy>
void Launcher<ThreadingModel, CreationPolicy>::stop()
{
	// !!! synchronized access !!!
        typename ThreadingModel::Lock lock;

	logger.log( Logger::DEBUG, "[Launcher#stop] Called." );	
	this->registry->removeAllBundleInfos();
}

template<
	class ThreadingModel,
		template <class> class CreationPolicy>
void Launcher<ThreadingModel, CreationPolicy>::startBundle( BundleConfiguration bundleConfig )
{
	// !!! synchronized access !!!
        typename ThreadingModel::Lock lock;

	logger.log( Logger::DEBUG, "[Launcher#startBundle] Called, bundle config: %1", bundleConfig.toString() );	
	vector<BundleConfiguration> vec;
	vec.push_back( bundleConfig );
	this->start( vec );
}

template<
	class ThreadingModel,
		template <class> class CreationPolicy>
void Launcher<ThreadingModel, CreationPolicy>::stopBundle( const string& bundleName )
{
	// !!! synchronized access !!!
        typename ThreadingModel::Lock lock;

	logger.log( Logger::DEBUG, "[Launcher#stopBundle] Called, bundle name: %1", bundleName );		
	this->registry->removeBundleInfo( bundleName );
}

template<
	class ThreadingModel,
		template <class> class CreationPolicy>
vector<string> Launcher<ThreadingModel, CreationPolicy>::getBundleNames()
{
	// !!! synchronized access !!!
        typename ThreadingModel::Lock lock;

	vector<string> bundleNameVec;
	vector<BundleInfoBase*> vec = this->registry->getBundleInfos();
	vector<BundleInfoBase*>::iterator iter;
	for ( iter = vec.begin(); iter != vec.end(); iter++ )
	{
		bundleNameVec.push_back( (*iter)->getBundleName() );
	}
	return bundleNameVec;
}

template<
	class ThreadingModel,
		template <class> class CreationPolicy>
string Launcher<ThreadingModel, CreationPolicy>::dumpBundleInfo( const string& bundleName )
{
	// !!! synchronized access !!!
        typename ThreadingModel::Lock lock;

	BundleInfoBase* bi = this->registry->getBundleInfo( bundleName );
	if ( bi == 0 )
	{
		return "Bundle not available!";
	}
	else
	{
		return bi->toString();
	}
}

template<
	class ThreadingModel,
		template <class> class CreationPolicy>
string Launcher<ThreadingModel, CreationPolicy>::dumpAllBundleNames()
{
	// !!! synchronized access !!!
        typename ThreadingModel::Lock lock;

	vector<BundleInfoBase*> vec = this->registry->getBundleInfos();
	vector<BundleInfoBase*>::iterator it;

	ostringstream stream;
	stream << "*** Started Bundles *** " << endl;

	for ( it = vec.begin(); it != vec.end(); it++ )
	{
		stream << (*it)->getBundleName() << endl;				
	}
	stream << endl;	
	return stream.str();
}

//-------------------------added by duanlin.yi---------------------
template<
	class ThreadingModel,
		template <class> class CreationPolicy>
bool Launcher<ThreadingModel, CreationPolicy>::startApp(QMainWindow *main_wnd)
{
	// !!! synchronized access !!!
        typename ThreadingModel::Lock lock;
	if (0==main_wnd)
	{
		return false;
	}

	vector<ServiceInfo*>* infos=registry->getServiceInfo("IRunnable");
	if (infos!=NULL)
	{
		if (infos->empty()==false)
		{
			ServiceInfo* svrInfo=*(infos->begin());
			IRunnable* runnable=static_cast<IRunnable*>(svrInfo->getService());
			runnable->run(main_wnd);
			return true;
		}
	}

	return false;
}
//------------------------------------------------------------------
//--------------------

}}




#endif
