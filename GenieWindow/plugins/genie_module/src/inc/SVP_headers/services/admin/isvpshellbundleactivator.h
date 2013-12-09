#ifndef ISVPSHELLBUNDLEACTIVATOR_H
#define ISVPSHELLBUNDLEACTIVATOR_H
#include "IAdministrationProvider.h"
#include "Global.h"
#include "BundleConfiguration.h"
#include "IBundleActivator.h"
#include "IBundleContext.h"

#ifdef WIN
#include "WinDllCreator.h"
#else
#include "QtDllCreator.h"
#endif

#include "NullCreator.h"
#include "MultiThreaded.h"

using namespace svp::framework;
using namespace svp::config;
using namespace svp::util::threading;
using namespace svp::services::admin;

#ifdef WIN
using namespace svp::instantiation::win;
#else
#ifdef QT
using namespace svp::instantiation::qt;
#endif
#endif

class ISvpShellBundleActivator:public IBundleActivator
{
public:
        virtual void setPlatformLauncher(IAdministrationProvider *l)=0;
};

#endif // ISVPSHELLBUNDLEACTIVATOR_H
