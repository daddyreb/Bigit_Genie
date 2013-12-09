
#include <stdlib.h>
#include <string>
#include <iostream>

#include "ObjectCreator.h"
#include "IBundleActivator.h"

#include <QtCore/qglobal.h>

#define DLL extern "C" Q_DECL_EXPORT
using namespace std;
using namespace svp::instantiation;
using namespace svp::framework;


DLL IBundleActivator* createObject( const string &className )
{
	ObjectCreator<IBundleActivator> OC_BUNDLE_ACTIVATOR;
	LoggerFactory::getLogger( "Test" ).log( Logger::DEBUG, "[bundle2_dll#createObject] Loading instance of class '%1'.", className );
	return OC_BUNDLE_ACTIVATOR.createObject( className );					
}

