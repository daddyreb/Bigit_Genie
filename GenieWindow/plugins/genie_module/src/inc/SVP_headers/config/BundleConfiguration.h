#ifndef BUNDLE_CONFIGURATION_H
#define BUNDLE_CONFIGURATION_H

#include <string>

namespace svp { namespace config {

using namespace std;

/**
 * The <code>BundleConfiguration</code> describes all relevant data 
 * which are necessary for the framework for starting a bundle.<br>
 *
 * @author magr74
 */
class BundleConfiguration
{
	private:		
		/**
		 * Represents the name of the bundle, e.g. 'test_bundle'.
		 */
		string bundleName;

		/**
		 * Indicates the class name of the bundle activator.
		 */
		string className;

		/**
		 * Represents the library path where the bundle is located as 
		 * library, e.g. as DLL on windows systems.<br>
		 * Example for library path: 'c:/temp/libraries'.
		 */
		string libraryPath;

		/**
		 * Specifies the name of the library which contains the bundle, e.g.
		 * 'test_bundle.dll'
		 */
		string libraryName;

	public:

		/**
		 * Constant value which indicates that no library path is available.
		 */
		static const string NO_LIB_PATH;

		/**
		 * Constant value which indicates that no library name is available.
		 */
		static const string NO_LIB_NAME;

		/**
		 * Creates instances of class <code>BundleConfiguration</code>.
		 *
		 * @param bundleName
		 *				The name of the bundle.
		 * @param className 
		 *				The name of the bundle activator class.
		 * @param libPath
		 *				The path to the bundle library.
		 * @param libName
		 *				The name of the bundle library.
		 */
		BundleConfiguration( const string &bundleName, const string &className, const string &libPath, const string &libName );
		
		/**
		 * Creates instances of class <code>BundleConfiguration</code>.
		 *
 		 * @param bundleName
		 *				The name of the bundle.
		 * @param className 
		 *				The name of the bundle activator class.		
		 */
		BundleConfiguration( const string &bundleName, const string &className );
		
		/**
		 * Returns the name of the bundle.
		 */
		string getBundleName();
		
		/**
		 * Returns the name of the activator class.
		 */
		string getClassName();
		
		/**
		 * Returns the path to the bundle library.
		 */
		string getLibraryPath();
		
		/**
		 * Returns the name of the bundle library.
		 */
		string getLibraryName();
		
		/**
		 * Returns the string representation of the <code>BundleConfiguration</code>
		 * object.
		 */
		string toString();
};

}}

#endif